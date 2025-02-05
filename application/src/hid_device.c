/**
 * BSD 3-Clause License
 * 
 * Copyright (c) 2025, Peter Christoffersen
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "hid_device.h"
#include <zephyr/version.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/class/usbd_hid.h>
#include <zephyr/logging/log.h>

#include "hid_descriptor.h"
#include "hid_gamepad.h"
#include "lamp_array.h"
#include "controller.h"

LOG_MODULE_REGISTER(hid);


static const struct device *hid_dev = DEVICE_DT_GET_ONE(zephyr_hid_device);



static const uint8_t hid_report_desc[] = {
    HID_USAGE_PAGE(HID_USAGE_GEN_DESKTOP),
    HID_USAGE(HID_USAGE_GEN_DESKTOP_GAMEPAD),
    HID_COLLECTION(HID_COLLECTION_APPLICATION),
        HID_REPORT_ID(REPORT_ID_GAMEPAD),

        // Joysitcks
        HID_USAGE(HID_USAGE_GEN_DESKTOP_POINTER),
            HID_LOGICAL_MIN16_(CONTROLLER_AXIS_MIN),
            HID_LOGICAL_MAX16_(CONTROLLER_AXIS_MAX),
            HID_COLLECTION(HID_COLLECTION_PHYSICAL),
                HID_USAGE(HID_USAGE_GEN_DESKTOP_X),
                HID_USAGE(HID_USAGE_GEN_DESKTOP_Y),
                HID_USAGE(HID_USAGE_GEN_DESKTOP_Z),
                HID_USAGE(HID_USAGE_GEN_DESKTOP_RX),
                HID_USAGE(HID_USAGE_GEN_DESKTOP_RY),
                HID_USAGE(HID_USAGE_GEN_DESKTOP_RZ),
                HID_REPORT_SIZE(sizeof(axis_value_t)*8),
                HID_REPORT_COUNT(CONTROLLER_AXIS_COUNT),
                HID_INPUT(HID_DATA|HID_VARIABLE|HID_ABSOLUTE),
            HID_END_COLLECTION,

        // ANO - Direction Pad
        HID_USAGE(HID_USAGE_GEN_DESKTOP_HAT_SWITCH),
            HID_LOGICAL_MIN8(1),
            HID_LOGICAL_MAX8(8),
            HID_PHYSICAL_MIN8(0),
            HID_PHYSICAL_MAX16_(315),
            HID_UNIT(0x14),// Unit (eng rot: angular pos)
            HID_REPORT_SIZE(4),
            HID_REPORT_COUNT(1),
            HID_INPUT(HID_DATA|HID_VARIABLE|HID_NULL_STATE|HID_ABSOLUTE),

        // ANO - Dial
        HID_USAGE(HID_USAGE_GEN_DESKTOP_DIAL),
	        HID_LOGICAL_MIN8_(CONTROLLER_DIAL_MIN),
	        HID_LOGICAL_MAX8_(CONTROLLER_DIAL_MAX),
            HID_REPORT_SIZE(4),
            HID_REPORT_COUNT(1),
	        HID_INPUT(HID_DATA|HID_VARIABLE|HID_RELATIVE),

        // Other buttons
        HID_USAGE_PAGE(HID_USAGE_GEN_BUTTON),
            /*
	        HID_USAGE_MIN8(0x01),
	        HID_USAGE_MAX8(CONTROLLER_BUTTON_COUNT),
            */
            HID_USAGE(HID_USAGE_GAMEPAD_BUTTON_SELECT), // ANO Select
            HID_USAGE(HID_USAGE_GAMEPAD_BUTTON_SOUTH),  // Right Pad
            HID_USAGE(HID_USAGE_GAMEPAD_BUTTON_EAST),   // Right Pad
            HID_USAGE(HID_USAGE_GAMEPAD_BUTTON_NORTH),  // Right Pad
            HID_USAGE(HID_USAGE_GAMEPAD_BUTTON_WEST),   // Right Pad
            HID_USAGE(CONTROLLER_BUTTON_1_USAGE),    // Center top button 1
            HID_USAGE(CONTROLLER_BUTTON_2_USAGE),    // Center top button 2
            HID_USAGE(CONTROLLER_BUTTON_3_USAGE),    // Center bottom button 1
            HID_USAGE(CONTROLLER_BUTTON_4_USAGE),    // Center bottom button 2
            #if CONTROLLER_BUTTON_COUNT >=11
            HID_USAGE(CONTROLLER_BUTTON_5_USAGE),    // Toggle switch 1
            HID_USAGE(CONTROLLER_BUTTON_6_USAGE),    // Toggle switch 2
            #endif
	        HID_LOGICAL_MIN8(0),
	        HID_LOGICAL_MAX8(1),
   	        HID_REPORT_SIZE(1),
	        HID_REPORT_COUNT(CONTROLLER_BUTTON_COUNT),
            HID_UNIT_EXPONENT(0x00),//      Unit Exponent (0) 
            HID_UNIT(0x00),         //      Unit (None)
	        HID_INPUT(HID_DATA|HID_VARIABLE|HID_ABSOLUTE),
            HID_REPORT_SIZE(1),
            HID_REPORT_COUNT((sizeof(button_state_t)*8-CONTROLLER_BUTTON_COUNT)),
            HID_INPUT(HID_CONSTANT|HID_VARIABLE|HID_ABSOLUTE),

        HID_USAGE_PAGE(HID_USAGE_GEN_LEDS),
            HID_COLLECTION(HID_COLLECTION_LOGICAL),
                HID_USAGE(CONTROLLER_LED_1_USAGE),
                HID_USAGE(CONTROLLER_LED_2_USAGE),
                HID_USAGE(CONTROLLER_LED_3_USAGE),
                HID_USAGE(CONTROLLER_LED_4_USAGE),
                #if CONTROLLER_LED_COUNT >= 6
                HID_USAGE(CONTROLLER_LED_5_USAGE),
                HID_USAGE(CONTROLLER_LED_6_USAGE),
                #endif
                HID_REPORT_SIZE(1),
                HID_REPORT_COUNT(CONTROLLER_LED_COUNT),
	            HID_OUTPUT(HID_DATA|HID_VARIABLE|HID_ABSOLUTE),
            HID_END_COLLECTION,
            HID_REPORT_SIZE(1),
            HID_REPORT_COUNT((sizeof(led_state_t)*8-CONTROLLER_LED_COUNT)),
            HID_OUTPUT(HID_CONSTANT|HID_VARIABLE|HID_ABSOLUTE),
    HID_END_COLLECTION,

    /* Lighting and Illumination */
    HID_USAGE_PAGE(HID_USAGE_LIGHTING),        
    HID_USAGE(HID_USAGE_LIGHTING_LAMP_ARRAY),  
    HID_COLLECTION(HID_COLLECTION_APPLICATION),
        /* Lamp Array Attributes Report */ 
        HID_REPORT_ID(REPORT_ID_FEATURE_LAMP_ARRAY_ATTRIBUTES),
        HID_USAGE(HID_USAGE_LIGHTING_LAMP_ARRAY_ATTRIBUTES_REPORT),
            HID_COLLECTION(HID_COLLECTION_LOGICAL),                
                HID_USAGE(HID_USAGE_LIGHTING_LAMP_COUNT),          
                    HID_LOGICAL_MIN16_(0x0000),                        
                    HID_LOGICAL_MAX16_(0xFFFF),                        
                    HID_REPORT_SIZE(16),                               
                    HID_REPORT_COUNT(1),                               
                    HID_FEATURE(HID_CONSTANT|HID_VARIABLE|HID_ABSOLUTE),
                HID_USAGE(HID_USAGE_LIGHTING_BOUNDING_BOX_WIDTH_IN_MICROMETERS), 
                HID_USAGE(HID_USAGE_LIGHTING_BOUNDING_BOX_HEIGHT_IN_MICROMETERS),
                HID_USAGE(HID_USAGE_LIGHTING_BOUNDING_BOX_DEPTH_IN_MICROMETERS), 
                HID_USAGE(HID_USAGE_LIGHTING_LAMP_ARRAY_KIND),
                HID_USAGE(HID_USAGE_LIGHTING_MIN_UPDATE_INTERVAL_IN_MICROSECONDS),
                    HID_LOGICAL_MIN32_(0x00000000),
                    HID_LOGICAL_MAX32_(0x7FFFFFFF),
                    HID_REPORT_SIZE(32),           
                    HID_REPORT_COUNT(5),           
                    HID_FEATURE(HID_CONSTANT|HID_VARIABLE|HID_ABSOLUTE),
            HID_END_COLLECTION,                                     

        /* Lamp Attributes Request Report */ 
        HID_REPORT_ID(REPORT_ID_FEATURE_LAMP_ATTRIBUTES_REQUEST),
        HID_USAGE(HID_USAGE_LIGHTING_LAMP_ATTRIBUTES_REQUEST_REPORT),
            HID_COLLECTION(HID_COLLECTION_LOGICAL),   
                HID_USAGE(HID_USAGE_LIGHTING_LAMP_ID),
                    HID_LOGICAL_MIN16_(0x0000),           
                    HID_LOGICAL_MAX16_(0xFFFF),           
                    HID_REPORT_SIZE(16),                  
                    HID_REPORT_COUNT(1),                  
                    HID_FEATURE(HID_DATA|HID_VARIABLE|HID_ABSOLUTE),
            HID_END_COLLECTION,

        /* Lamp Attributes Response Report */ 
        HID_REPORT_ID(REPORT_ID_FEATURE_LAMP_ATTRIBUTES),
        HID_USAGE(HID_USAGE_LIGHTING_LAMP_ATTRIBUTES_RESPONSE_REPORT),
            HID_COLLECTION(HID_COLLECTION_LOGICAL),
                HID_USAGE(HID_USAGE_LIGHTING_LAMP_ID),
                    HID_LOGICAL_MIN16_(0x0000),
                    HID_LOGICAL_MAX16_(0xFFFF),
                    HID_REPORT_SIZE(16),       
                    HID_REPORT_COUNT(1),       
                    HID_FEATURE(HID_DATA|HID_VARIABLE|HID_ABSOLUTE),        
                HID_USAGE(HID_USAGE_LIGHTING_POSITION_X_IN_MICROMETERS),
                HID_USAGE(HID_USAGE_LIGHTING_POSITION_Y_IN_MICROMETERS),
                HID_USAGE(HID_USAGE_LIGHTING_POSITION_Z_IN_MICROMETERS),
                HID_USAGE(HID_USAGE_LIGHTING_UPDATE_LATENCY_IN_MICROSECONDS),
                HID_USAGE(HID_USAGE_LIGHTING_LAMP_PURPOSES),
                    HID_LOGICAL_MIN32_(0x00000000),             
                    HID_LOGICAL_MAX32_(0x7FFFFFFF),             
                    HID_REPORT_SIZE(32),                        
                    HID_REPORT_COUNT(5),                        
                    HID_FEATURE(HID_DATA|HID_VARIABLE|HID_ABSOLUTE),    
                HID_USAGE(HID_USAGE_LIGHTING_RED_LEVEL_COUNT),      
                HID_USAGE(HID_USAGE_LIGHTING_GREEN_LEVEL_COUNT),    
                HID_USAGE(HID_USAGE_LIGHTING_BLUE_LEVEL_COUNT),     
                HID_USAGE(HID_USAGE_LIGHTING_INTENSITY_LEVEL_COUNT),
                HID_USAGE(HID_USAGE_LIGHTING_IS_PROGRAMMABLE),      
                HID_USAGE(HID_USAGE_LIGHTING_INPUT_BINDING),        
                    HID_LOGICAL_MIN8(0),   
                    HID_LOGICAL_MAX8(0xFF),
                    HID_REPORT_SIZE(8),    
                    HID_REPORT_COUNT(6),   
                    HID_FEATURE(HID_DATA|HID_VARIABLE|HID_ABSOLUTE),
            HID_END_COLLECTION,

        /* Lamp Multi-Update Report */ 
        HID_REPORT_ID(REPORT_ID_FEATURE_LAMP_MULTI_UPDATE),
        HID_USAGE(HID_USAGE_LIGHTING_LAMP_MULTI_UPDATE_REPORT),
            HID_COLLECTION(HID_COLLECTION_LOGICAL),
                HID_USAGE(HID_USAGE_LIGHTING_LAMP_COUNT),
                HID_USAGE(HID_USAGE_LIGHTING_LAMP_UPDATE_FLAGS),
                    HID_LOGICAL_MIN8(0),
                    HID_LOGICAL_MAX8(8),
                    HID_REPORT_SIZE(8), 
                    HID_REPORT_COUNT(2),
                    HID_FEATURE(HID_DATA|HID_VARIABLE|HID_ABSOLUTE),
                HID_USAGE(HID_USAGE_LIGHTING_LAMP_ID),
                    HID_LOGICAL_MIN16_(0x0000),           
                    HID_LOGICAL_MAX16_(0xFFFF),           
                    HID_REPORT_SIZE(16),                  
                    HID_REPORT_COUNT(8),                  
                    HID_FEATURE(HID_DATA|HID_VARIABLE|HID_ABSOLUTE),       
                HID_USAGE(HID_USAGE_LIGHTING_RED_UPDATE_CHANNEL),      
                HID_USAGE(HID_USAGE_LIGHTING_GREEN_UPDATE_CHANNEL),    
                HID_USAGE(HID_USAGE_LIGHTING_BLUE_UPDATE_CHANNEL),     
                HID_USAGE(HID_USAGE_LIGHTING_INTENSITY_UPDATE_CHANNEL),
                HID_USAGE(HID_USAGE_LIGHTING_RED_UPDATE_CHANNEL),      
                HID_USAGE(HID_USAGE_LIGHTING_GREEN_UPDATE_CHANNEL),    
                HID_USAGE(HID_USAGE_LIGHTING_BLUE_UPDATE_CHANNEL),     
                HID_USAGE(HID_USAGE_LIGHTING_INTENSITY_UPDATE_CHANNEL),
                HID_USAGE(HID_USAGE_LIGHTING_RED_UPDATE_CHANNEL),      
                HID_USAGE(HID_USAGE_LIGHTING_GREEN_UPDATE_CHANNEL),    
                HID_USAGE(HID_USAGE_LIGHTING_BLUE_UPDATE_CHANNEL),     
                HID_USAGE(HID_USAGE_LIGHTING_INTENSITY_UPDATE_CHANNEL),
                HID_USAGE(HID_USAGE_LIGHTING_RED_UPDATE_CHANNEL),      
                HID_USAGE(HID_USAGE_LIGHTING_GREEN_UPDATE_CHANNEL),    
                HID_USAGE(HID_USAGE_LIGHTING_BLUE_UPDATE_CHANNEL),     
                HID_USAGE(HID_USAGE_LIGHTING_INTENSITY_UPDATE_CHANNEL),
                HID_USAGE(HID_USAGE_LIGHTING_RED_UPDATE_CHANNEL),      
                HID_USAGE(HID_USAGE_LIGHTING_GREEN_UPDATE_CHANNEL),    
                HID_USAGE(HID_USAGE_LIGHTING_BLUE_UPDATE_CHANNEL),     
                HID_USAGE(HID_USAGE_LIGHTING_INTENSITY_UPDATE_CHANNEL),
                HID_USAGE(HID_USAGE_LIGHTING_RED_UPDATE_CHANNEL),      
                HID_USAGE(HID_USAGE_LIGHTING_GREEN_UPDATE_CHANNEL),    
                HID_USAGE(HID_USAGE_LIGHTING_BLUE_UPDATE_CHANNEL),     
                HID_USAGE(HID_USAGE_LIGHTING_INTENSITY_UPDATE_CHANNEL),
                HID_USAGE(HID_USAGE_LIGHTING_RED_UPDATE_CHANNEL),      
                HID_USAGE(HID_USAGE_LIGHTING_GREEN_UPDATE_CHANNEL),    
                HID_USAGE(HID_USAGE_LIGHTING_BLUE_UPDATE_CHANNEL),     
                HID_USAGE(HID_USAGE_LIGHTING_INTENSITY_UPDATE_CHANNEL),
                HID_USAGE(HID_USAGE_LIGHTING_RED_UPDATE_CHANNEL),      
                HID_USAGE(HID_USAGE_LIGHTING_GREEN_UPDATE_CHANNEL),    
                HID_USAGE(HID_USAGE_LIGHTING_BLUE_UPDATE_CHANNEL),     
                HID_USAGE(HID_USAGE_LIGHTING_INTENSITY_UPDATE_CHANNEL),
                    HID_LOGICAL_MIN8(0),
                    HID_LOGICAL_MAX8(255),
                    HID_REPORT_SIZE(8),
                    HID_REPORT_COUNT(32),
                    HID_FEATURE(HID_DATA|HID_VARIABLE|HID_ABSOLUTE),
            HID_END_COLLECTION,

        /* Lamp Range Update Report */ 
        HID_REPORT_ID(REPORT_ID_FEATURE_LAMP_RANGE_UPDATE),
        HID_USAGE(HID_USAGE_LIGHTING_LAMP_RANGE_UPDATE_REPORT),
            HID_COLLECTION(HID_COLLECTION_LOGICAL),
                HID_USAGE(HID_USAGE_LIGHTING_LAMP_UPDATE_FLAGS),
                    HID_LOGICAL_MIN8(0),
                    HID_LOGICAL_MAX8(8),
                    HID_REPORT_SIZE(8),
                    HID_REPORT_COUNT(1),
                    HID_FEATURE(HID_DATA|HID_VARIABLE|HID_ABSOLUTE),
                HID_USAGE(HID_USAGE_LIGHTING_LAMP_ID_START),
                HID_USAGE(HID_USAGE_LIGHTING_LAMP_ID_END),
                    HID_LOGICAL_MIN16_(0x0000),
                    HID_LOGICAL_MAX16_(0xFFFF),
                    HID_REPORT_SIZE(16),
                    HID_REPORT_COUNT(2),
                    HID_FEATURE(HID_DATA|HID_VARIABLE|HID_ABSOLUTE),
                HID_USAGE(HID_USAGE_LIGHTING_RED_UPDATE_CHANNEL),
                HID_USAGE(HID_USAGE_LIGHTING_GREEN_UPDATE_CHANNEL),
                HID_USAGE(HID_USAGE_LIGHTING_BLUE_UPDATE_CHANNEL),
                HID_USAGE(HID_USAGE_LIGHTING_INTENSITY_UPDATE_CHANNEL),
                    HID_LOGICAL_MIN8(0),
                    HID_LOGICAL_MAX8(0xFF),
                    HID_REPORT_SIZE(8),
                    HID_REPORT_COUNT(4),
                    HID_FEATURE(HID_DATA|HID_VARIABLE|HID_ABSOLUTE),
            HID_END_COLLECTION,

        /* Lamp Array Control Report */ 
        HID_REPORT_ID(REPORT_ID_FEATURE_LAMP_ARRAY_CONTROL),
        HID_USAGE(HID_USAGE_LIGHTING_LAMP_ARRAY_CONTROL_REPORT),
            HID_COLLECTION(HID_COLLECTION_LOGICAL),
                HID_USAGE(HID_USAGE_LIGHTING_AUTONOMOUS_MODE),
                    HID_LOGICAL_MIN8(0),
                    HID_LOGICAL_MAX8(1),
                    HID_REPORT_SIZE(8),
                    HID_REPORT_COUNT(1),
                    HID_FEATURE(HID_CONSTANT|HID_VARIABLE|HID_ABSOLUTE),
            HID_END_COLLECTION,

    HID_END_COLLECTION,
};




static void iface_ready_cb(const struct device *dev, const bool ready)
{
	LOG_INF("HID device %s interface is %s", dev->name, ready ? "ready" : "not ready");
    lamp_array_reset();
    controller_reset();
}


static int get_report_cb(const struct device *dev,
			 const uint8_t type, const uint8_t id, const uint16_t len,
			 uint8_t *const buf)
{
    // Fill in Report ID as the first byte
    if (len < 1) {
        return -EINVAL;
    }
    int ret = -ENOTSUP;

    if (type == HID_REPORT_TYPE_FEATURE) {
        switch (id) {
            case REPORT_ID_FEATURE_LAMP_ARRAY_ATTRIBUTES:
                ret = lamp_array_attributes_response(buf, len);
                break;
            case REPORT_ID_FEATURE_LAMP_ATTRIBUTES:
                ret = lamp_attributes_response(buf, len);
                break;
            default:
                break;
        }
    }

    if (ret>=0) {
        buf[0] = id; // Set the report ID
        return ret; // Return the total length
    }

	LOG_WRN("Get Report: type=%u, id=%u, len=%u", type, id, len);
    return -ENOTSUP;
}

static int set_report_cb(const struct device *dev,
			 const uint8_t type, const uint8_t id, const uint16_t len,
			 const uint8_t *const buf)
{
    // Report ID is the first byte
    if (len < 1 || buf[0] != id) {
        return -EINVAL;
    }

	if (type == HID_REPORT_TYPE_OUTPUT) {
        switch (id) {
            case REPORT_ID_GAMEPAD:
                return controller_set_output(buf, len);
            default:
                break;
        }
	}
    else if (type == HID_REPORT_TYPE_FEATURE) {
        switch (id) {
            case REPORT_ID_FEATURE_LAMP_ATTRIBUTES_REQUEST:
                return lamp_attributes_request(buf, len);
            case REPORT_ID_FEATURE_LAMP_MULTI_UPDATE:
                return lamp_multi_update(buf, len);
            case REPORT_ID_FEATURE_LAMP_RANGE_UPDATE:
                return lamp_range_update(buf, len);
            case REPORT_ID_FEATURE_LAMP_ARRAY_CONTROL:
                return lamp_array_control(buf, len);
            default:
                break;
        }
    } 

    LOG_WRN("Set Report: type=%u, id=%u, len=%u", type, id, len);
    return -ENOTSUP;
}


static void set_idle_cb(const struct device *dev,
			const uint8_t id, const uint32_t duration)
{
    controller_set_idle(duration);
}


static uint32_t get_idle_cb(const struct device *dev, const uint8_t id)
{
    return controller_get_idle();
}


static void set_protocol_cb(const struct device *dev, const uint8_t proto)
{
	LOG_INF("Protocol changed to (%02x) %s", proto, proto == 0U ? "Boot Protocol" : "Report Protocol");
}



#if ZEPHYR_VERSION_CODE >= ZEPHYR_VERSION(4,0,99)
static void input_report_done_cb(const struct device *dev, const uint8_t *const report)
#else
static void input_report_done_cb(const struct device *dev)
#endif
{
    LOG_DBG("Input Report done");
    controller_input_report_done();
}



static const struct hid_device_ops ops = {
    .iface_ready = iface_ready_cb,
    .get_report = get_report_cb,
    .set_report = set_report_cb,
    .set_idle = set_idle_cb,
    .get_idle = get_idle_cb,
    .set_protocol = set_protocol_cb,
    .input_report_done = input_report_done_cb,
};


int controller_hid_device_init()
{
    int ret;

    if (hid_dev == NULL) {
        LOG_ERR("Cannot get USB HID Device");
        return -ENODEV;
    }
	if (!device_is_ready(hid_dev)) {
		LOG_ERR("HID Device is not ready");
		return -EIO;
	}

    LOG_INF("USB Register HID Device");
	ret = hid_device_register(hid_dev, hid_report_desc, sizeof(hid_report_desc),&ops);
    if (ret != 0) {
        LOG_ERR("Failed to register HID Device: %d", ret);
        return ret;
    }

    return 0;
}



int controller_hid_submit_gamepad(const uint8_t *const report, uint16_t size)
{
	return hid_device_submit_report(hid_dev, size, report);
}

