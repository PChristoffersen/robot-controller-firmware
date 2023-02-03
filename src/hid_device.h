#pragma once

#include <Arduino.h>
#include <USBComposite.h>
#include <limits>

#include <config.h>
#include <debug.h>
#include "util.h"
#include "feature_types.h"

class HIDDevice : public HIDReporter {
    public:
        using axis_value = int16;
        using button_value = uint16;

        static constexpr uint8 BUTTON_COUNT { 9 };
        static constexpr uint8 LED_COUNT { 5 };

        enum axis_type : uint8 {
            AXIS_X,
            AXIS_Y,
            AXIS_Z,
            AXIS_RX,
            AXIS_RY,
            AXIS_RZ,
            AXIS_COUNT
        };
        enum hat_position : uint8 {
            HAT_UP         = 0x00,
            HAT_UP_RIGHT   = 0x01,
            HAT_RIGHT      = 0x02,
            HAT_DOWN_RIGHT = 0x03,
            HAT_DOWN       = 0x04,
            HAT_DOWN_LEFT  = 0x05,
            HAT_LEFT       = 0x06,
            HAT_UP_LEFT    = 0x07,
            HAT_NONE       = 0x0F,
        };

        static constexpr uint8 REPORT_ID { 1 };

        enum FeatureId : uint8 {
            FEATURE_COMMAND,
            FEATURE_SOFT_INPUT,
            FEATURE_OUTPUT_CONFIGS,
            FEATURE_MODE_CONFIGS,
            FEATURE_COLOR_LUT,
            FEATURE_BRIGHTNESS_LUT,
            _FEATURE_COUNT,
        };
        static constexpr uint8 FEATURE_REPORT_ID(FeatureId id) { return REPORT_ID+static_cast<uint8>(id); }
        static constexpr size_t FEATURE_COUNT { static_cast<size_t>(_FEATURE_COUNT) };

        typedef struct {
            uint8 leds;
        } __packed OutputReport;
        static constexpr uint8 OUTPUT_BUFFER_SIZE { sizeof(OutputReport) };

        typedef struct {
            uint8        reportID;
            axis_value   axis[AXIS_COUNT];
            uint8        hat : 4;
            int8         dial : 4;
            button_value buttons;
        } __packed Report;


        HIDDevice(USBHID& HID);
        void begin(void);
        void end(void);

        uint16 read_output(OutputReport &output)
        {
            return getOutput((uint8*)&output);
        }
        template<typename buffer_type>
        uint16 read_feature(FeatureId id, buffer_type &buffer)
        {
            return usb_hid_get_data(HID_REPORT_TYPE_FEATURE, FEATURE_REPORT_ID(id), (uint8*)&buffer, 1);
        }
        template<typename buffer_type>
        void set_feature(FeatureId id, buffer_type &buffer)
        {
            usb_hid_set_feature(FEATURE_REPORT_ID(id), (uint8*)&buffer);
        }
        

        void setAxis(axis_type axis, axis_value value) 
        { 
            m_report.axis[axis] = value; 
            //m_report_pending = true;
        }
        void set_dial(int value) 
        { 
            value = clamp_value(value, -7, 7); 
            m_report_pending = (value!=0) && (value!=m_report.dial);
            m_report.dial = value;
        }
        void set_hat(hat_position pos) 
        { 
            m_report_pending = (pos!=m_report.hat);
            m_report.hat = pos; 
        }
        void set_buttons(button_value buttons, button_value mask)
        { 
            button_value value = (m_report.buttons & ~mask) | buttons;
            m_report_pending = (value!=m_report.buttons);
            m_report.buttons = value;
        }


        void set_report_pending() { m_report_pending = true; }
        bool is_report_pending() { return m_report_pending;  }
        void send_report() 
        { 
            sendReport(); 
            m_report_pending = false; 
        }

        static const uint8 *descriptor();
        static const size_t descriptor_size();

        const Report &report() const { return m_report; }
    private:
        static constexpr unsigned long REPORT_INTERVAL { 100u };
        static constexpr axis_value AXIS_DEADZONE { 30u };

        static constexpr axis_value AXIS_MIN    { std::numeric_limits<axis_value>::min() };
        static constexpr axis_value AXIS_MAX    { std::numeric_limits<axis_value>::max() };
        static constexpr axis_value AXIS_CENTER { std::numeric_limits<axis_value>::is_signed?(0):((static_cast<int>(std::numeric_limits<axis_value>::max())+1)/2) };

        Report m_report;
        bool m_report_pending;

        HIDBuffer_t m_output_buffer;
        HIDBuffer_t m_feature_buffers[FEATURE_COUNT];
};
