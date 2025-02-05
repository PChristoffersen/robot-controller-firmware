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

#pragma once

#include <stdint.h>

#include <zephyr/usb/class/usb_hid.h>

#define HID_VAL_BYTE0(x) ((x) & 0xFF)
#define HID_VAL_BYTE1(x) (((x) & 0xFF00)>>8)
#define HID_VAL_BYTE2(x) (((x) & 0xFF0000)>>16)
#define HID_VAL_BYTE3(x) (((x) & 0xFF000000)>>24)


#define HID_PHYSICAL_MIN8(a) HID_ITEM(HID_ITEM_TAG_PHYSICAL_MIN, HID_ITEM_TYPE_GLOBAL, 1), a
#define HID_PHYSICAL_MAX8(a) HID_ITEM(HID_ITEM_TAG_PHYSICAL_MAX, HID_ITEM_TYPE_GLOBAL, 1), a
#define HID_PHYSICAL_MIN16(a, b) HID_ITEM(HID_ITEM_TAG_PHYSICAL_MIN, HID_ITEM_TYPE_GLOBAL, 2), a, b
#define HID_PHYSICAL_MAX16(a, b) HID_ITEM(HID_ITEM_TAG_PHYSICAL_MAX, HID_ITEM_TYPE_GLOBAL, 2), a, b

#define HID_LOGICAL_MIN8_(a) HID_LOGICAL_MIN8(((uint8_t)a))
#define HID_LOGICAL_MAX8_(a) HID_LOGICAL_MAX8(((uint8_t)a))
#define HID_LOGICAL_MIN16_(a) HID_LOGICAL_MIN16(HID_VAL_BYTE0(((uint16_t)a)), HID_VAL_BYTE1(((uint16_t)a)))
#define HID_LOGICAL_MAX16_(a) HID_LOGICAL_MAX16(HID_VAL_BYTE0(((uint16_t)a)), HID_VAL_BYTE1(((uint16_t)a)))
#define HID_LOGICAL_MIN32_(a) HID_LOGICAL_MIN32(HID_VAL_BYTE0(((uint32_t)a)), HID_VAL_BYTE1(((uint32_t)a)), HID_VAL_BYTE2(((uint32_t)a)), HID_VAL_BYTE3(((uint32_t)a)))
#define HID_LOGICAL_MAX32_(a) HID_LOGICAL_MAX32(HID_VAL_BYTE0(((uint32_t)a)), HID_VAL_BYTE1(((uint32_t)a)), HID_VAL_BYTE2(((uint32_t)a)), HID_VAL_BYTE3(((uint32_t)a)))

#define HID_PHYSICAL_MIN16_(a) HID_PHYSICAL_MIN16(HID_VAL_BYTE0(((uint16_t)a)), HID_VAL_BYTE1(((uint16_t)a)))
#define HID_PHYSICAL_MAX16_(a) HID_PHYSICAL_MAX16(HID_VAL_BYTE0(((uint16_t)a)), HID_VAL_BYTE1(((uint16_t)a)))

#define HID_UNIT(a)	HID_ITEM(HID_ITEM_TAG_UNIT, HID_ITEM_TYPE_GLOBAL, 1), a
#define HID_UNIT_EXPONENT(a) HID_ITEM(HID_ITEM_TAG_UNIT_EXPONENT, HID_ITEM_TYPE_GLOBAL, 1), a


/** --------------------------------------------------------------
 * HID 1.11 - 6.2.2.5 Input, Output, and Feature Items
 * --------------------------------------------------------------- */
#define HID_DATA             (0<<0)
#define HID_CONSTANT         (1<<0)

#define HID_ARRAY            (0<<1)
#define HID_VARIABLE         (1<<1)

#define HID_ABSOLUTE         (0<<2)
#define HID_RELATIVE         (1<<2)

#define HID_WRAP_NO          (0<<3)
#define HID_WRAP             (1<<3)

#define HID_LINEAR           (0<<4)
#define HID_NONLINEAR        (1<<4)

#define HID_PREFERRED_STATE  (0<<5)
#define HID_PREFERRED_NO     (1<<5)

#define HID_NO_NULL_POSITION (0<<6)
#define HID_NULL_STATE       (1<<6)

#define HID_NON_VOLATILE     (0<<7)
#define HID_VOLATILE         (1<<7)

#define HID_BITFIELD         (0<<8)
#define HID_BUFFERED_BYTES   (1<<8)



/* ---------------------------------------------------------------
 * HUT 1.6 - Generic Desktop Page (0x01)
 * --------------------------------------------------------------- */
#define HID_USAGE_GEN_DESKTOP_Z 0x32
#define HID_USAGE_GEN_DESKTOP_RX 0x33
#define HID_USAGE_GEN_DESKTOP_RY 0x34
#define HID_USAGE_GEN_DESKTOP_RZ 0x35
#define HID_USAGE_GEN_DESKTOP_DIAL 0x37
#define HID_USAGE_GEN_DESKTOP_HAT_SWITCH 0x39



/* ---------------------------------------------------------------
 * HUT 1.12 - Usage Pages
 * --------------------------------------------------------------- */
#define HID_USAGE_LIGHTING 0x59


/* ---------------------------------------------------------------
 * Button Usages
 * --------------------------------------------------------------- */
#define HID_USAGE_GEN_BUTTON_1  0x01
#define HID_USAGE_GEN_BUTTON_2  0x02
#define HID_USAGE_GEN_BUTTON_3  0x03
#define HID_USAGE_GEN_BUTTON_4  0x04
#define HID_USAGE_GEN_BUTTON_5  0x05
#define HID_USAGE_GEN_BUTTON_6  0x06
#define HID_USAGE_GEN_BUTTON_7  0x07
#define HID_USAGE_GEN_BUTTON_8  0x08
#define HID_USAGE_GEN_BUTTON_9  0x09
#define HID_USAGE_GEN_BUTTON_10 0x0A
#define HID_USAGE_GEN_BUTTON_12 0x0B
#define HID_USAGE_GEN_BUTTON_13 0x0C
#define HID_USAGE_GEN_BUTTON_14 0x0D
#define HID_USAGE_GEN_BUTTON_15 0x0E
#define HID_USAGE_GEN_BUTTON_16 0x0F

#define HID_USAGE_GAMEPAD_BUTTON_SOUTH  HID_USAGE_GEN_BUTTON_1
#define HID_USAGE_GAMEPAD_BUTTON_A      HID_USAGE_GAMEPAD_SOUTH
#define HID_USAGE_GAMEPAD_BUTTON_EAST   HID_USAGE_GEN_BUTTON_2
#define HID_USAGE_GAMEPAD_BUTTON_B      HID_USAGE_GAMEPAD_EAST
#define HID_USAGE_GAMEPAD_BUTTON_C      HID_USAGE_GEN_BUTTON_3
#define HID_USAGE_GAMEPAD_BUTTON_NORTH  HID_USAGE_GEN_BUTTON_4
#define HID_USAGE_GAMEPAD_BUTTON_X      HID_USAGE_GAMEPAD_NORTH
#define HID_USAGE_GAMEPAD_BUTTON_WEST   HID_USAGE_GEN_BUTTON_5
#define HID_USAGE_GAMEPAD_BUTTON_Y      HID_USAGE_GAMEPAD_WEST
#define HID_USAGE_GAMEPAD_BUTTON_Z      HID_USAGE_GEN_BUTTON_6
#define HID_USAGE_GAMEPAD_BUTTON_L1     HID_USAGE_GEN_BUTTON_7
#define HID_USAGE_GAMEPAD_BUTTON_R1     HID_USAGE_GEN_BUTTON_8
#define HID_USAGE_GAMEPAD_BUTTON_L2     HID_USAGE_GEN_BUTTON_9
#define HID_USAGE_GAMEPAD_BUTTON_R2     HID_USAGE_GEN_BUTTON_10
#define HID_USAGE_GAMEPAD_BUTTON_SELECT HID_USAGE_GEN_BUTTON_12
#define HID_USAGE_GAMEPAD_BUTTON_START  HID_USAGE_GEN_BUTTON_13
#define HID_USAGE_GAMEPAD_BUTTON_MODE   HID_USAGE_GEN_BUTTON_14
#define HID_USAGE_GAMEPAD_BUTTON_THUMBL HID_USAGE_GEN_BUTTON_15
#define HID_USAGE_GAMEPAD_BUTTON_THUMBR HID_USAGE_GEN_BUTTON_16

#define HID_USAGE_JOYSTICK_BUTTON_TRIGGER   HID_USAGE_GEN_BUTTON_1
#define HID_USAGE_JOYSTICK_BUTTON_THUMB     HID_USAGE_GEN_BUTTON_2
#define HID_USAGE_JOYSTICK_BUTTON_THUMB2    HID_USAGE_GEN_BUTTON_3
#define HID_USAGE_JOYSTICK_BUTTON_TOP       HID_USAGE_GEN_BUTTON_4
#define HID_USAGE_JOYSTICK_BUTTON_TOP2      HID_USAGE_GEN_BUTTON_5
#define HID_USAGE_JOYSTICK_BUTTON_PINKIE    HID_USAGE_GEN_BUTTON_6
#define HID_USAGE_JOYSTICK_BUTTON_BASE      HID_USAGE_GEN_BUTTON_7
#define HID_USAGE_JOYSTICK_BUTTON_BASE2     HID_USAGE_GEN_BUTTON_8
#define HID_USAGE_JOYSTICK_BUTTON_BASE3     HID_USAGE_GEN_BUTTON_9
#define HID_USAGE_JOYSTICK_BUTTON_BASE4     HID_USAGE_GEN_BUTTON_10
#define HID_USAGE_JOYSTICK_BUTTON_BASE5     HID_USAGE_GEN_BUTTON_11
#define HID_USAGE_JOYSTICK_BUTTON_BASE6     HID_USAGE_GEN_BUTTON_12
#define HID_USAGE_JOYSTICK_BUTTON_DEAD      HID_USAGE_GEN_BUTTON_13

#define HID_USAGE_EXTRA_BUTTON_1    0x11
#define HID_USAGE_EXTRA_BUTTON_2    0x12
#define HID_USAGE_EXTRA_BUTTON_3    0x13
#define HID_USAGE_EXTRA_BUTTON_4    0x14
#define HID_USAGE_EXTRA_BUTTON_5    0x15
#define HID_USAGE_EXTRA_BUTTON_6    0x16
#define HID_USAGE_EXTRA_BUTTON_7    0x17
#define HID_USAGE_EXTRA_BUTTON_8    0x18
#define HID_USAGE_EXTRA_BUTTON_9    0x19
#define HID_USAGE_EXTRA_BUTTON_10   0x1A


/* ---------------------------------------------------------------
 * HUT 1.12 - LED Page (0x08)
 * --------------------------------------------------------------- */
/* HID Usage values support by the linux driver */
#define HID_USAGE_LED_LINUX_NUML     0x01 /*   "Num Lock"                 */
#define HID_USAGE_LED_LINUX_CAPSL    0x02 /*   "Caps Lock"                */
#define HID_USAGE_LED_LINUX_SCROLLL  0x03 /*   "Scroll Lock"              */
#define HID_USAGE_LED_LINUX_COMPOSE  0x04 /*   "Compose"                  */
#define HID_USAGE_LED_LINUX_KANA     0x05 /*   "Kana"                     */
#define HID_USAGE_LED_LINUX_SLEEP    0x27 /*   "Stand-By"                 */
#define HID_USAGE_LED_LINUX_SUSPEND  0x4c /*   "System Suspend"           */
#define HID_USAGE_LED_LINUX_MUTE     0x09 /*   "Mute"                     */
#define HID_USAGE_LED_LINUX_MISC     0x4b /*   "Generic Indicator"        */
#define HID_USAGE_LED_LINUX_MAIL     0x19 /*   "Message Waiting"          */
#define HID_USAGE_LED_LINUX_CHARGING 0x4d /*   "External Power Connected" */

#define HID_USAGE_LED_PLAYER1        0x61
#define HID_USAGE_LED_PLAYER2        0x62
#define HID_USAGE_LED_PLAYER3        0x63
#define HID_USAGE_LED_PLAYER4        0x64
#define HID_USAGE_LED_PLAYER5        0x65
#define HID_USAGE_LED_PLAYER6        0x66
#define HID_USAGE_LED_PLAYER7        0x67
#define HID_USAGE_LED_PLAYER8        0x68


/** --------------------------------------------------------------
 * HUT 1.12 - Lighting And Illumination Page (0x59)
 * --------------------------------------------------------------- */

#define HID_USAGE_LIGHTING_LAMP_ARRAY                           0x01
#define HID_USAGE_LIGHTING_LAMP_ARRAY_ATTRIBUTES_REPORT         0x02
#define HID_USAGE_LIGHTING_LAMP_COUNT                           0x03
#define HID_USAGE_LIGHTING_BOUNDING_BOX_WIDTH_IN_MICROMETERS    0x04
#define HID_USAGE_LIGHTING_BOUNDING_BOX_HEIGHT_IN_MICROMETERS   0x05
#define HID_USAGE_LIGHTING_BOUNDING_BOX_DEPTH_IN_MICROMETERS    0x06
#define HID_USAGE_LIGHTING_LAMP_ARRAY_KIND                      0x07
#define HID_USAGE_LIGHTING_MIN_UPDATE_INTERVAL_IN_MICROSECONDS  0x08
#define HID_USAGE_LIGHTING_LAMP_ATTRIBUTES_REQUEST_REPORT       0x20
#define HID_USAGE_LIGHTING_LAMP_ID                              0x21
#define HID_USAGE_LIGHTING_LAMP_ATTRIBUTES_RESPONSE_REPORT      0x22
#define HID_USAGE_LIGHTING_POSITION_X_IN_MICROMETERS            0x23
#define HID_USAGE_LIGHTING_POSITION_Y_IN_MICROMETERS            0x24
#define HID_USAGE_LIGHTING_POSITION_Z_IN_MICROMETERS            0x25
#define HID_USAGE_LIGHTING_LAMP_PURPOSES                        0x26
#define HID_USAGE_LIGHTING_UPDATE_LATENCY_IN_MICROSECONDS       0x27
#define HID_USAGE_LIGHTING_RED_LEVEL_COUNT                      0x28
#define HID_USAGE_LIGHTING_GREEN_LEVEL_COUNT                    0x29
#define HID_USAGE_LIGHTING_BLUE_LEVEL_COUNT                     0x2A
#define HID_USAGE_LIGHTING_INTENSITY_LEVEL_COUNT                0x2B
#define HID_USAGE_LIGHTING_IS_PROGRAMMABLE                      0x2C
#define HID_USAGE_LIGHTING_INPUT_BINDING                        0x2D
#define HID_USAGE_LIGHTING_LAMP_MULTI_UPDATE_REPORT             0x50
#define HID_USAGE_LIGHTING_RED_UPDATE_CHANNEL                   0x51
#define HID_USAGE_LIGHTING_GREEN_UPDATE_CHANNEL                 0x52
#define HID_USAGE_LIGHTING_BLUE_UPDATE_CHANNEL                  0x53
#define HID_USAGE_LIGHTING_INTENSITY_UPDATE_CHANNEL             0x54
#define HID_USAGE_LIGHTING_LAMP_UPDATE_FLAGS                    0x55
#define HID_USAGE_LIGHTING_LAMP_RANGE_UPDATE_REPORT             0x60
#define HID_USAGE_LIGHTING_LAMP_ID_START                        0x61
#define HID_USAGE_LIGHTING_LAMP_ID_END                          0x62
#define HID_USAGE_LIGHTING_LAMP_ARRAY_CONTROL_REPORT            0x70
#define HID_USAGE_LIGHTING_AUTONOMOUS_MODE                      0x71
