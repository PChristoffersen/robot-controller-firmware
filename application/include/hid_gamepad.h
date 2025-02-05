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

#include "hid_types.h"

typedef int16_t axis_value_t;
typedef uint8_t led_state_t;


typedef struct HID_STRUCT_ATTR {
    uint16_t select: 1;
    uint16_t south: 1;
    uint16_t east: 1;
    uint16_t north: 1;
    uint16_t west: 1;
    uint16_t extra1: 1;
    uint16_t extra2: 1;
    uint16_t extra3: 1;
    uint16_t extra4: 1;
    uint16_t extra5: 1;
    uint16_t extra6: 1;
    uint16_t _padding: 5;
} button_state_t;


typedef struct HID_STRUCT_ATTR {
    hid_report_id_t report_id;

    // Joysticks
    axis_value_t axis[6];

    // Direction Pad
    uint8_t dpad: 4;

    // Dial
    int8_t dial: 4;

    // Buttons
    button_state_t buttons;
} hid_gamepad_input_t;


typedef struct HID_STRUCT_ATTR {
    hid_report_id_t report_id;
    led_state_t leds;
} hid_gamepad_output_t;

