#include "hid_device.h"

#include <algorithm>
#include <assert.h>
#include <Arduino.h>

#include "debug.h"


#define HID_VAL_BYTE0(x) ((x) & 0xFF)
#define HID_VAL_BYTE1(x) (((x) & 0xFF00)>>8)
#define HID_VAL_BYTE2(x) (((x) & 0xFF0000)>>16)
#define HID_VAL_BYTE3(x) (((x) & 0xFF000000)>>24)

#define HID_DESC_REPORT_ID(VAL) 0x85, VAL

#define HID_DESC_USAGE_PAGE(VAL) 0x05, VAL
#define HID_DESC_USAGE_PAGE16(VAL) 0x06, HID_VAL_BYTE0(static_cast<uint16>(VAL)), HID_VAL_BYTE1(static_cast<uint16>(VAL))
#define HID_DESC_USAGE(VAL) 0x09, VAL
#define HID_DESC_USAGE16(VAL) 0x0A, HID_VAL_BYTE0(static_cast<uint16>(VAL)), HID_VAL_BYTE1(static_cast<uint16>(VAL))
#define HID_DESC_USAGE_MIN(VAL) 0x19, VAL
#define HID_DESC_USAGE_MAX(VAL) 0x29, VAL
#define HID_DESC_REPORT_SIZE(VAL) 0x75, VAL
#define HID_DESC_REPORT_COUNT(VAL) 0x95, VAL
#define HID_DESC_COLLECTION_BEGIN(VAL) 0xA1, VAL
#define HID_DESC_COLLECTION_END() 0xC0
#define HID_DESC_LOGICAL_MIN8(VAL) 0x15, static_cast<uint8>(VAL)
#define HID_DESC_LOGICAL_MAX8(VAL) 0x25, static_cast<uint8>(VAL)
#define HID_DESC_LOGICAL_MIN16(VAL) 0x16, HID_VAL_BYTE0(static_cast<uint16>(VAL)), HID_VAL_BYTE1(static_cast<uint16>(VAL))
#define HID_DESC_LOGICAL_MAX16(VAL) 0x26, HID_VAL_BYTE0(static_cast<uint16>(VAL)), HID_VAL_BYTE1(static_cast<uint16>(VAL))
#define HID_DESC_LOGICAL_MIN32(VAL) 0x17, HID_VAL_BYTE0(static_cast<uint16>(VAL)), HID_VAL_BYTE1(static_cast<uint16>(VAL)), HID_VAL_BYTE2(static_cast<uint16>(VAL)), HID_VAL_BYTE3(static_cast<uint16>(VAL))
#define HID_DESC_LOGICAL_MAX32(VAL) 0x27, HID_VAL_BYTE0(static_cast<uint16>(VAL)), HID_VAL_BYTE1(static_cast<uint16>(VAL)), HID_VAL_BYTE2(static_cast<uint16>(VAL)), HID_VAL_BYTE3(static_cast<uint16>(VAL))
#define HID_DESC_PHYSICAL_MIN8(VAL) 0x35, static_cast<uint8>(VAL)
#define HID_DESC_PHYSICAL_MAX8(VAL) 0x45, static_cast<uint8>(VAL)
#define HID_DESC_PHYSICAL_MIN16(VAL) 0x36, HID_VAL_BYTE0(static_cast<uint16>(VAL)), HID_VAL_BYTE1(static_cast<uint16>(VAL))
#define HID_DESC_PHYSICAL_MAX16(VAL) 0x46, HID_VAL_BYTE0(static_cast<uint16>(VAL)), HID_VAL_BYTE1(static_cast<uint16>(VAL))
#define HID_DESC_UNIT(VAL) 0x65, VAL
#define HID_DESC_UNIT_EXPONENT(VAL) 0x55, VAL
#define HID_DESC_INPUT(VAL) 0x81, VAL
#define HID_DESC_OUTPUT(VAL) 0x91, VAL
#define HID_DESC_FEATURE(VAL) 0xb1, VAL

#define HID_DESC_FEATURE_REPORT_ID(VAL) HID_DESC_REPORT_ID(HIDDevice::FEATURE_REPORT_ID(VAL))
#define HID_DESC_FEATURE_USAGE(VAL) HID_DESC_USAGE(0x01+static_cast<uint8>(VAL))


static constexpr uint8 report_descriptor[] {
    HID_DESC_USAGE_PAGE(0x01),                                  // Usage Page (Generic Desktop)
    HID_DESC_USAGE(0x04),                                       // Usage (Joystick - 0x04; Gamepad - 0x05; Multi-axis Controller - 0x08)
    HID_DESC_COLLECTION_BEGIN(0x01),                            // Collection (Application)
        HID_DESC_REPORT_ID(HIDDevice::REPORT_ID),               //   Report ID

        HID_DESC_USAGE(0x01),                                   //   Usage (Pointer)
            HID_DESC_LOGICAL_MIN16(0x8000),                     //      Logical Minimum (-32768)
            HID_DESC_LOGICAL_MAX16(0x7FFF),                     //      Logical Maximum (32767)
            HID_DESC_COLLECTION_BEGIN(0x00),                    //      Collection (Physical)
                HID_DESC_USAGE(0x30),                           //         Usage (X)
                HID_DESC_USAGE(0x31),					        //         Usage (Y)
                HID_DESC_USAGE(0x32),					        //         Usage (Z)
                HID_DESC_USAGE(0x33),					        //         Usage (Rx)
                HID_DESC_USAGE(0x34),					        //         Usage (Ry)
                HID_DESC_USAGE(0x35),					        //         Usage (Rz)
                HID_DESC_REPORT_SIZE(16),				        //            Report Size (16)
                HID_DESC_REPORT_COUNT(HIDDevice::AXIS_COUNT),   //         Report Count (AXIS_COUNT)
                HID_DESC_INPUT(0x02),					        //         Input (variable,absolute)
            HID_DESC_COLLECTION_END(),                          //      End Collection (Physical)

        HID_DESC_USAGE(0x39),                                   //   Usage (Hat Switch)
            HID_DESC_LOGICAL_MIN8(0),                           //      Logical Minimum (0)
            HID_DESC_LOGICAL_MAX8(7),                           //      Logical Maximum (7)
            HID_DESC_PHYSICAL_MIN8(0),                          //      Physical Minimum (0)
            HID_DESC_PHYSICAL_MAX16(315),                       //      Physical Maximum (315)
            HID_DESC_UNIT(0x14),                                //      Unit (eng rot: angular pos)
            HID_DESC_REPORT_SIZE(4),                            //      Report Size (4)
            HID_DESC_REPORT_COUNT(1),                           //      Report Count (1)
            HID_DESC_INPUT(0x02),						        //      Input (variable,absolute)

        HID_DESC_USAGE(0x37),                                   //   Usage (Dial)
	        HID_DESC_LOGICAL_MIN8(-7),				            // 	    Logical Minimum (-7)
	        HID_DESC_LOGICAL_MAX8(7),				            // 	    Logical Minimum (7)
            HID_DESC_REPORT_SIZE(4),                            //      Report Size (4)
            HID_DESC_REPORT_COUNT(1),                           //      Report Count (1)
	        HID_DESC_INPUT(0x06),						        //      Input (variable,data,relative)

        HID_DESC_USAGE_PAGE(0x09),						        //   Usage Page (Button)
	        HID_DESC_USAGE_MIN(0x01),				            //      Usage Minimum (Button #1)
	        HID_DESC_USAGE_MAX(HIDDevice::BUTTON_COUNT),     	//      Usage Maximum (Button #)
	        HID_DESC_LOGICAL_MIN8(0),				            // 	    Logical Minimum (0)
	        HID_DESC_LOGICAL_MAX8(1),				            //      Logical Maximum (1)
   	        HID_DESC_REPORT_SIZE(1),                            //      Report Size (1)
	        HID_DESC_REPORT_COUNT(HIDDevice::BUTTON_COUNT), 	//      Report Count (<BUTTON_COUNT>)
            HID_DESC_UNIT_EXPONENT(0x00),                       //      Unit Exponent (0) 
            HID_DESC_UNIT(0x00),                                //      Unit (None)
	        HID_DESC_INPUT(0x02),						        //      Input (variable,absolute)
            HID_DESC_REPORT_SIZE(1),                            //      Padding Report Size (1)
            HID_DESC_REPORT_COUNT(sizeof(HIDDevice::button_value)*8-HIDDevice::BUTTON_COUNT),  //      Padding Report Count (X)
            HID_DESC_OUTPUT(0x03),		    		            //      Padding Output (Const,Var,Abs)

        HID_DESC_USAGE_PAGE(0x08),                              //   Usage Page (Leds)
            HID_DESC_COLLECTION_BEGIN(0x02),                    //      Collection (Logical)
                HID_DESC_USAGE(0x27),                           //          Usage (Stand-By)
                HID_DESC_USAGE(0x4c),                           //          Usage (System suspend)
                HID_DESC_USAGE(0x4b),                           //          Usage (Charging)
                HID_DESC_USAGE(0x19),                           //          Usage (Message)
                HID_DESC_USAGE(0x4b),                           //          Usage (Generic)
                //HID_DESC_USAGE(0x03),                         //          Usage (Scroll Lock)
                //HID_DESC_USAGE(0x05),                         //          Usage (Kana)
                //HID_DESC_USAGE(0x09),                         //          Usage (Mute)
   	            HID_DESC_REPORT_SIZE(1),                        //          Report Size (1)
	            HID_DESC_REPORT_COUNT(HIDDevice::LED_COUNT),    //          Report Count (8)
	            HID_DESC_OUTPUT(0x02),				            //          Output (Data,Var,Abs)
            HID_DESC_COLLECTION_END(),                          //      End Collection (Logical)
            HID_DESC_REPORT_SIZE(1),                            //      Padding Report Size (1)
            HID_DESC_REPORT_COUNT(8-HIDDevice::LED_COUNT),	    //      Padding Report Count (X)
            HID_DESC_OUTPUT(0x03),		    		            //      Padding Output (Const,Var,Abs)


        HID_DESC_USAGE_PAGE16(0xFF00),                                      //   Usage Page (Vendor Defined Page 1)
            HID_DESC_FEATURE_USAGE(HIDDevice::FEATURE_COMMAND),             //      Usage (Vendor Usage X)
            HID_DESC_REPORT_SIZE(8*Feature::COMMAND_SIZE),                  //      Report size (8)
            HID_DESC_REPORT_COUNT(1), 	                                    //      Report Count (*)
            HID_DESC_FEATURE(0x02),                                         //      Feature (Data,Var,Abs)

        HID_DESC_FEATURE_REPORT_ID(HIDDevice::FEATURE_STATE),
        HID_DESC_USAGE_PAGE16(0xFF00),                                      //   Usage Page (Vendor Defined Page 1)
            HID_DESC_FEATURE_USAGE(HIDDevice::FEATURE_STATE),               //      Usage (Vendor Usage X)
            HID_DESC_REPORT_SIZE(8*Feature::STATE_SIZE),                    //      Report size (8)
            HID_DESC_REPORT_COUNT(1), 	                                    //      Report Count (*)
            HID_DESC_FEATURE(0x02),                                         //      Feature (Data,Var,Abs)

        HID_DESC_FEATURE_REPORT_ID(HIDDevice::FEATURE_OUTPUT_CONFIGS),
        HID_DESC_USAGE_PAGE16(0xFF00),                                      //   Usage Page (Vendor Defined Page 1)
            HID_DESC_FEATURE_USAGE(HIDDevice::FEATURE_OUTPUT_CONFIGS),      //      Usage (Vendor Usage X)
            HID_DESC_REPORT_SIZE(8*Feature::OUTPUT_CONFIG_SIZE),            //      Report size (*)
            HID_DESC_REPORT_COUNT(Feature::OUTPUT_CONFIG_COUNT),            //      Report Count (*)
            HID_DESC_FEATURE(0x02),                                         //      Feature (Data,Var,Abs)

        HID_DESC_FEATURE_REPORT_ID(HIDDevice::FEATURE_MODE_CONFIGS),
        HID_DESC_USAGE_PAGE16(0xFF00),                                      //   Usage Page (Vendor Defined Page 1)
            HID_DESC_FEATURE_USAGE(HIDDevice::FEATURE_MODE_CONFIGS),        //      Usage (Vendor Usage X)
            HID_DESC_REPORT_SIZE(8*Feature::MODE_CONFIG_SIZE),              //      Report size (*)
            HID_DESC_REPORT_COUNT(Feature::MODE_CONFIG_COUNT),              //      Report Count (*)
            HID_DESC_FEATURE(0x02),                                         //      Feature (Data,Var,Abs)

        HID_DESC_FEATURE_REPORT_ID(HIDDevice::FEATURE_COLOR_LUT),
        HID_DESC_USAGE_PAGE16(0xFF00),                                      //   Usage Page (Vendor Defined Page 1)
            HID_DESC_FEATURE_USAGE(HIDDevice::FEATURE_COLOR_LUT),           //      Usage (Vendor Usage X)
            HID_DESC_REPORT_SIZE(8*Feature::COLOR_LUT_ENTRY_SIZE),          //      Report size (*)
            HID_DESC_REPORT_COUNT(Feature::COLOR_LUT_ENTRY_COUNT),          //      Report Count (*)
            HID_DESC_FEATURE(0x02),                                         //      Feature (Data,Var,Abs)

        HID_DESC_FEATURE_REPORT_ID(HIDDevice::FEATURE_BRIGHTNESS_LUT),
        HID_DESC_USAGE_PAGE16(0xFF00),                                      //   Usage Page (Vendor Defined Page 1)
            HID_DESC_FEATURE_USAGE(HIDDevice::FEATURE_BRIGHTNESS_LUT),      //      Usage (Vendor Usage X)
            HID_DESC_REPORT_SIZE(8*Feature::BRIGHTNESS_LUT_ENTRY_SIZE),     //      Report size (*)
            HID_DESC_REPORT_COUNT(Feature::BRIGHTNESS_LUT_ENTRY_COUNT),     //      Report Count (*)
            HID_DESC_FEATURE(0x02),                                         //      Feature (Data,Var,Abs)

    HID_DESC_COLLECTION_END(),                                  // End Collection (Application)
};



static constexpr size_t OUTPUT_BUFFER_ALLOC_SIZE { HID_BUFFER_ALLOCATE_SIZE(HIDDevice::OUTPUT_BUFFER_SIZE,HIDDevice::REPORT_ID) };

static constexpr size_t FEATURE_SIZE[HIDDevice::FEATURE_COUNT] {
    Feature::COMMAND_SIZE,
    Feature::STATE_SIZE,
    Feature::OUTPUT_CONFIGS_SIZE,
    Feature::MODE_CONFIGS_SIZE,
    Feature::COLOR_LUT_SIZE,
    Feature::BRIGHTNESS_LUT_SIZE
};
static constexpr size_t FEATURE_SIZES[HIDDevice::FEATURE_COUNT] {
    HID_BUFFER_SIZE(FEATURE_SIZE[0], HIDDevice::FEATURE_REPORT_ID(static_cast<HIDDevice::FeatureId>(0))),
    HID_BUFFER_SIZE(FEATURE_SIZE[1], HIDDevice::FEATURE_REPORT_ID(static_cast<HIDDevice::FeatureId>(1))),
    HID_BUFFER_SIZE(FEATURE_SIZE[2], HIDDevice::FEATURE_REPORT_ID(static_cast<HIDDevice::FeatureId>(2))),
    HID_BUFFER_SIZE(FEATURE_SIZE[3], HIDDevice::FEATURE_REPORT_ID(static_cast<HIDDevice::FeatureId>(3))),
    HID_BUFFER_SIZE(FEATURE_SIZE[4], HIDDevice::FEATURE_REPORT_ID(static_cast<HIDDevice::FeatureId>(4))),
    HID_BUFFER_SIZE(FEATURE_SIZE[5], HIDDevice::FEATURE_REPORT_ID(static_cast<HIDDevice::FeatureId>(5))),
};
static constexpr size_t FEATURE_BUFFER_SIZES[HIDDevice::FEATURE_COUNT] {
    HID_BUFFER_ALLOCATE_SIZE(FEATURE_SIZE[0], HIDDevice::FEATURE_REPORT_ID(static_cast<HIDDevice::FeatureId>(0))),
    HID_BUFFER_ALLOCATE_SIZE(FEATURE_SIZE[1], HIDDevice::FEATURE_REPORT_ID(static_cast<HIDDevice::FeatureId>(1))),
    HID_BUFFER_ALLOCATE_SIZE(FEATURE_SIZE[2], HIDDevice::FEATURE_REPORT_ID(static_cast<HIDDevice::FeatureId>(2))),
    HID_BUFFER_ALLOCATE_SIZE(FEATURE_SIZE[3], HIDDevice::FEATURE_REPORT_ID(static_cast<HIDDevice::FeatureId>(3))),
    HID_BUFFER_ALLOCATE_SIZE(FEATURE_SIZE[4], HIDDevice::FEATURE_REPORT_ID(static_cast<HIDDevice::FeatureId>(4))),
    HID_BUFFER_ALLOCATE_SIZE(FEATURE_SIZE[5], HIDDevice::FEATURE_REPORT_ID(static_cast<HIDDevice::FeatureId>(5))),
};
static constexpr size_t FEATURE_BUFFER_OFFSET[HIDDevice::FEATURE_COUNT] {
    OUTPUT_BUFFER_ALLOC_SIZE,
    OUTPUT_BUFFER_ALLOC_SIZE+FEATURE_BUFFER_SIZES[0],
    OUTPUT_BUFFER_ALLOC_SIZE+FEATURE_BUFFER_SIZES[0]+FEATURE_BUFFER_SIZES[1],
    OUTPUT_BUFFER_ALLOC_SIZE+FEATURE_BUFFER_SIZES[0]+FEATURE_BUFFER_SIZES[1]+FEATURE_BUFFER_SIZES[2],
    OUTPUT_BUFFER_ALLOC_SIZE+FEATURE_BUFFER_SIZES[0]+FEATURE_BUFFER_SIZES[1]+FEATURE_BUFFER_SIZES[2]+FEATURE_BUFFER_SIZES[3],
    OUTPUT_BUFFER_ALLOC_SIZE+FEATURE_BUFFER_SIZES[0]+FEATURE_BUFFER_SIZES[1]+FEATURE_BUFFER_SIZES[2]+FEATURE_BUFFER_SIZES[3]+FEATURE_BUFFER_SIZES[4],
};
static volatile uint8 g_feature_buffer_data[FEATURE_BUFFER_OFFSET[HIDDevice::FEATURE_COUNT-1]+FEATURE_BUFFER_SIZES[HIDDevice::FEATURE_COUNT-1]];






const uint8 *HIDDevice::descriptor()
{
    return report_descriptor;
}

const size_t HIDDevice::descriptor_size()
{
    return sizeof(report_descriptor);
}



HIDDevice::HIDDevice(USBHID& HID) : 
    HIDReporter(HID, NULL, (uint8*)&m_report, sizeof(m_report), REPORT_ID),
    m_report_pending { false },
    m_output_buffer(g_feature_buffer_data, HID_BUFFER_SIZE(OUTPUT_BUFFER_SIZE, REPORT_ID), REPORT_ID, HID_BUFFER_MODE_NO_WAIT),
    m_feature_buffers {
        { &g_feature_buffer_data[FEATURE_BUFFER_OFFSET[0]], FEATURE_SIZES[0], FEATURE_REPORT_ID(static_cast<FeatureId>(0)), HID_BUFFER_MODE_NO_WAIT },
        { &g_feature_buffer_data[FEATURE_BUFFER_OFFSET[1]], FEATURE_SIZES[1], FEATURE_REPORT_ID(static_cast<FeatureId>(1)), HID_BUFFER_MODE_NO_WAIT },
        { &g_feature_buffer_data[FEATURE_BUFFER_OFFSET[2]], FEATURE_SIZES[2], FEATURE_REPORT_ID(static_cast<FeatureId>(2)), HID_BUFFER_MODE_NO_WAIT },
        { &g_feature_buffer_data[FEATURE_BUFFER_OFFSET[3]], FEATURE_SIZES[3], FEATURE_REPORT_ID(static_cast<FeatureId>(3)), HID_BUFFER_MODE_NO_WAIT },
        { &g_feature_buffer_data[FEATURE_BUFFER_OFFSET[4]], FEATURE_SIZES[4], FEATURE_REPORT_ID(static_cast<FeatureId>(4)), HID_BUFFER_MODE_NO_WAIT },
        { &g_feature_buffer_data[FEATURE_BUFFER_OFFSET[5]], FEATURE_SIZES[4], FEATURE_REPORT_ID(static_cast<FeatureId>(5)), HID_BUFFER_MODE_NO_WAIT },
    }
{
    m_report.reportID = REPORT_ID;
    for (size_t i=0; i<AXIS_COUNT; i++) {
        m_report.axis[i] = AXIS_CENTER;
    }
    m_report.buttons = 0x0000;
    m_report.dial = 0;
    m_report.hat = HAT_NONE;

}


void HIDDevice::begin(void)
{
    memset((void*)g_feature_buffer_data, 0x00, sizeof(g_feature_buffer_data));
    m_output_buffer.buffer[0] = REPORT_ID;
    for (size_t i=0; i<FEATURE_COUNT; i++) {
        m_feature_buffers[i].buffer[0] = FEATURE_REPORT_ID(static_cast<FeatureId>(i));
    }

    HID.addOutputBuffer(&m_output_buffer);
    for (size_t i=0; i<FEATURE_COUNT; i++) {
        HID.addFeatureBuffer(&m_feature_buffers[i]);
    }
}

void HIDDevice::end(void)
{
}



