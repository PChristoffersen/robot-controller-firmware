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
#include <zephyr/input/input.h>

#include "hid_gamepad.h"


typedef struct {
    union {
        uint8_t value;
        struct __attribute__((packed)) {
            uint8_t up:1;
            uint8_t down:1;
            uint8_t left:1;
            uint8_t right:1;
        } buttons;
    };
} dpad_state_t;

#define DPAD_STATE_UP 0b0001
#define DPAD_STATE_DOWN 0b0010
#define DPAD_STATE_LEFT 0b0100
#define DPAD_STATE_RIGHT 0b1000



static inline bool input_process_axis(struct input_event *evt, hid_gamepad_input_t *input_state)
{
    size_t idx;
    axis_value_t diff;

    switch (evt->code) {
        case INPUT_ABS_X...INPUT_ABS_RZ:
            idx = evt->code - INPUT_ABS_X;
            if (idx>CONTROLLER_AXIS_COUNT) {
                LOG_ERR("Invalid axis index: %d", idx);
                return false;
            }
            diff = input_state->axis[idx] - evt->value;
            input_state->axis[idx] = evt->value;
            return diff > CONTROLLER_AXIS_CHANGE_THRESHOLD || diff < -CONTROLLER_AXIS_CHANGE_THRESHOLD;
        default:
            break;
    }

    return false;
}


static inline bool input_process_key(struct input_event *evt, hid_gamepad_input_t *input_state, dpad_state_t *dpad_state)
{
    switch (evt->code) {
        // ANO - Left pad
        case INPUT_BTN_SELECT:
            input_state->buttons.select = evt->value;
            return true;
        case INPUT_BTN_DPAD_UP:
            dpad_state->buttons.up = evt->value;
            return true;
        case INPUT_BTN_DPAD_DOWN:
            dpad_state->buttons.down = evt->value;
            return true;
        case INPUT_BTN_DPAD_LEFT:
            dpad_state->buttons.left = evt->value;
            return true;
        case INPUT_BTN_DPAD_RIGHT:
            dpad_state->buttons.right = evt->value;
            return true;

        // Right pad
        case INPUT_BTN_SOUTH:
            input_state->buttons.south = evt->value;
            return true;
        case INPUT_BTN_EAST:
            input_state->buttons.east = evt->value;
            return true;
        case INPUT_BTN_NORTH:
            input_state->buttons.north = evt->value;
            return true;
        case INPUT_BTN_WEST:
            input_state->buttons.west = evt->value;
            return true;

        // Iluminated buttons
        case INPUT_BTN_1:
            input_state->buttons.extra1 = evt->value;
            return true;
        case INPUT_BTN_2:
            input_state->buttons.extra2 = evt->value;
            return true;
        case INPUT_BTN_3:
            input_state->buttons.extra3 = evt->value;
            return true;
        case INPUT_BTN_4:
            input_state->buttons.extra4 = evt->value;
            return true;

        // Ext 
        case INPUT_BTN_5:
            input_state->buttons.extra5 = evt->value;
            return true;
        case INPUT_BTN_6:
            input_state->buttons.extra6 = evt->value;
            return true;
    }
    return false;
}


static inline bool input_process_dial(struct input_event *evt, hid_gamepad_input_t *input_state)
{
    if (evt->code == INPUT_REL_DIAL) {
        int8_t dial = input_state->dial + evt->value;
        if (dial >= CONTROLLER_DIAL_MIN && dial <= CONTROLLER_DIAL_MAX) {
            input_state->dial = dial;
            return true;
        }
    }
    return false;
}


static inline uint8_t input_dpad_map(dpad_state_t *dpad_state)
{
    switch (dpad_state->value) {
        case DPAD_STATE_UP: 
            return 0x1;
        case DPAD_STATE_UP | DPAD_STATE_RIGHT:
            return 0x2;
        case DPAD_STATE_RIGHT:
            return 0x3;
        case DPAD_STATE_DOWN | DPAD_STATE_RIGHT:
            return 0x4;
        case DPAD_STATE_DOWN: 
            return 0x5;
        case DPAD_STATE_DOWN | DPAD_STATE_LEFT:
            return 0x6;
        case DPAD_STATE_LEFT:
            return 0x7;
        case DPAD_STATE_UP | DPAD_STATE_LEFT:
            return 0x8;
        default: // No direction, or impossible combination
            return 0x0;
    }


}

