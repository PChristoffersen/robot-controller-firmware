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

#include "lamp_array.h"

#include <zephyr/kernel.h>  
#include <zephyr/logging/log.h>

#include "hid_descriptor.h"
#include "hid_lamp_array.h"
#include "lighting.h"
#include "controller.h"


LOG_MODULE_REGISTER(lamp_array);


/* -----------------------------------------------------------------------
 * Lamp Array Attributes
 * ----------------------------------------------------------------------- */

static const hid_lamp_attributes_response_t lamp_attributes[] = {
    /* WS2812 LEDS */
    {
        .lamp_id = 0,
        .position_x = 0,
        .position_y = 0,
        .position_z = 0,
        .update_latency = 0,
        .purpose = LAMP_PURPOSE_STATUS,
        .red_level_count = 0xFF,
        .green_level_count = 0xFF,
        .blue_level_count = 0xFF,
        .intensity_level_count = 1,
        .is_programmable = 1,
        .input_binding = 0,
    },
    {
        .lamp_id = 1,
        .position_x = 0,
        .position_y = 0,
        .position_z = 0,
        .update_latency = 0,
        .purpose = LAMP_PURPOSE_STATUS,
        .red_level_count = 0xFF,
        .green_level_count = 0xFF,
        .blue_level_count = 0xFF,
        .intensity_level_count = 1,
        .is_programmable = 1,
        .input_binding = 0,
    },
    {
        .lamp_id = 2,
        .position_x = 0,
        .position_y = 0,
        .position_z = 0,
        .update_latency = 0,
        .purpose = LAMP_PURPOSE_STATUS,
        .red_level_count = 0xFF,
        .green_level_count = 0xFF,
        .blue_level_count = 0xFF,
        .intensity_level_count = 1,
        .is_programmable = 1,
        .input_binding = 0,
    },
    {
        .lamp_id = 3,
        .position_x = 0,
        .position_y = 0,
        .position_z = 0,
        .update_latency = 0,
        .purpose = LAMP_PURPOSE_STATUS,
        .red_level_count = 0xFF,
        .green_level_count = 0xFF,
        .blue_level_count = 0xFF,
        .intensity_level_count = 1,
        .is_programmable = 1,
        .input_binding = 0,
    },

    #if CONFIG_SHIELD_ROBOT_CONTROLLER_STANDALONE
    /* Illuminated Buttons */
    {
        .lamp_id = 4,
        .position_x = 0,
        .position_y = 0,
        .position_z = 0,
        .update_latency = 0,
        .purpose = LAMP_PURPOSE_CONTROL,
        .red_level_count = 0x00,
        .green_level_count = 0x1,
        .blue_level_count = 0x00,
        .intensity_level_count = 100,
        .is_programmable = 1,
        .input_binding = HID_USAGE_EXTRA_BUTTON_1,
    },
    {
        .lamp_id = 5,
        .position_x = 0,
        .position_y = 0,
        .position_z = 0,
        .update_latency = 0,
        .purpose = LAMP_PURPOSE_CONTROL,
        .red_level_count = 0x00,
        .green_level_count = 0x1,
        .blue_level_count = 0x00,
        .intensity_level_count = 100,
        .is_programmable = 1,
        .input_binding = HID_USAGE_EXTRA_BUTTON_2,
    },
    {
        .lamp_id = 6,
        .position_x = 0,
        .position_y = 0,
        .position_z = 0,
        .update_latency = 0,
        .purpose = LAMP_PURPOSE_CONTROL,
        .red_level_count = 0x00,
        .green_level_count = 0x1,
        .blue_level_count = 0x00,
        .intensity_level_count = 100,
        .is_programmable = 1,
        .input_binding = HID_USAGE_EXTRA_BUTTON_3,
    },
    {
        .lamp_id = 7,
        .position_x = 0,
        .position_y = 0,
        .position_z = 0,
        .update_latency = 0,
        .purpose = LAMP_PURPOSE_CONTROL,
        .red_level_count = 0x00,
        .green_level_count = 0x01,
        .blue_level_count = 0x00,
        .intensity_level_count = 100,
        .is_programmable = 1,
        .input_binding = HID_USAGE_EXTRA_BUTTON_4,
    },
    /* Indicator buttons */
    {
        .lamp_id = 8,
        .position_x = 0,
        .position_y = 0,
        .position_z = 0,
        .update_latency = 0,
        .purpose = LAMP_PURPOSE_STATUS,
        .red_level_count = 0x01,
        .green_level_count = 0x00,
        .blue_level_count = 0x00,
        .intensity_level_count = 0x01,
        .is_programmable = 1,
        .input_binding = 0,
    },
    {
        .lamp_id = 9,
        .position_x = 0,
        .position_y = 0,
        .position_z = 0,
        .update_latency = 0,
        .purpose = LAMP_PURPOSE_STATUS,
        .red_level_count = 0x01,
        .green_level_count = 0x00,
        .blue_level_count = 0x00,
        .intensity_level_count = 0x01,
        .is_programmable = 1,
        .input_binding = 0,
    },
    #elif CONFIG_SHIELD_ROBOT_CONTROLLER_EMBEDDED
    #endif
};
#define LAMP_COUNT (sizeof(lamp_attributes)/sizeof(lamp_attributes[0]))


static const hid_lamp_array_attributes_report_t lamp_array_attributes = {
    .lamp_count = LAMP_COUNT,
    .width  = 1000,
    .height = 1000,
    .depth  = 1000,
    .kind = LAMP_ARRAY_KIND_GAME_CONTROLLER,
    .min_update_interval = 5 * USEC_PER_MSEC, // 5 ms
};





/* -----------------------------------------------------------------------
 * State
 * ----------------------------------------------------------------------- */


static hid_lamp_id_t current_lamp_id = 0;


static inline void set_lamp_color(uint8_t lamp_id, const hid_lamp_color_t *color) 
{
    if (lamp_id < CONTROLLER_WS2812_COUNT) {
        led_strip_set_color(lamp_id, color->red, color->green, color->blue);
    }
    else if (lamp_id < CONTROLLER_WS2812_COUNT + CONTROLLER_LED_COUNT) {
        led_set(lamp_id - CONTROLLER_WS2812_COUNT, color->intensity);
    }
}





int lamp_array_reset() {
    current_lamp_id = 0;
    return 0;
}

/* -----------------------------------------------------------------------
 * HID Reports
 * ----------------------------------------------------------------------- */

int lamp_array_attributes_response(uint8_t *buffer, uint16_t bufsize)
{
    if (bufsize < sizeof(lamp_array_attributes)) {
        return -EINVAL;
    }

    LOG_DBG("Lamp Array Attributes  %u", bufsize);
    memcpy(buffer, &lamp_array_attributes, sizeof(lamp_array_attributes));

    return sizeof(lamp_array_attributes);
}


int lamp_attributes_request(const uint8_t *buffer, uint16_t bufsize)
{
    if (bufsize < sizeof(hid_lamp_attributes_request_t)) {
        return -EINVAL;
    }

    const hid_lamp_attributes_request_t *request = (const hid_lamp_attributes_request_t *)buffer;

    if (request->lamp_id >= LAMP_COUNT) {
        return -EINVAL;
    }

    LOG_INF("Lamp Attributes Request: lamp_id=%u", request->lamp_id);
    current_lamp_id = request->lamp_id;

    return 0;
}


int lamp_attributes_response(uint8_t *buffer, uint16_t bufsize)
{
    if (bufsize < sizeof(hid_lamp_attributes_response_t)) {
        return -EINVAL;
    }

    LOG_INF("Lamp Attributes Response: lamp_id=%u", current_lamp_id);
    memcpy(buffer, &lamp_attributes[current_lamp_id], sizeof(hid_lamp_attributes_response_t));

    if (current_lamp_id < LAMP_COUNT-1) {
        current_lamp_id++;
    }
    else {
        current_lamp_id = 0;
    }

    return sizeof(hid_lamp_attributes_response_t);
}


int lamp_multi_update(const uint8_t *buffer, uint16_t bufsize)
{
    if (bufsize < sizeof(hid_lamp_multi_update_t)) {
        return -EINVAL;
    }
    if (controller_is_autonomous_mode()) {
        // Ignore updates if we are in autonomous mode
        return 0;
    }

    const hid_lamp_multi_update_t *update = (const hid_lamp_multi_update_t *)buffer;

    LOG_DBG("Lamp Multi Update: lamp_count=%u", update->lamp_count);
    for (uint8_t i=0; i<update->lamp_count; i++) {
        set_lamp_color(update->lamp_ids[i], &update->color[i]);
    }
    if (update->flags == LAMP_ARRAY_FLAG_UPDATE_COMPLETE) {
        led_strip_update();
    }

    return -ENOTSUP;
}


int lamp_range_update(const uint8_t *buffer, uint16_t bufsize)
{
    if (bufsize < sizeof(hid_lamp_range_update_t)) {
        return -EINVAL;
    }
    if (controller_is_autonomous_mode()) {
        // Ignore updates if we are in autonomous mode
        return 0;
    }

    const hid_lamp_range_update_t *update = (const hid_lamp_range_update_t *)buffer;

    LOG_DBG("Lamp Range Update: lamp_id_start=%u lamp_id_end=%u", update->lamp_id_start, update->lamp_id_end);
    for (hid_lamp_id_t lamp_id = update->lamp_id_start; lamp_id <= update->lamp_id_end; lamp_id++) {
        set_lamp_color(lamp_id, &update->color);
    }
    if (update->flags == LAMP_ARRAY_FLAG_UPDATE_COMPLETE) {
        led_strip_update();
    }

    return -ENOTSUP;
}


int lamp_array_control(const uint8_t *buffer, uint16_t bufsize)
{
    if (bufsize < sizeof(hid_lamp_array_control_t)) {
        return -EINVAL;
    }

    const hid_lamp_array_control_t *control = (const hid_lamp_array_control_t *)buffer;

    LOG_DBG("Lamp Array Control: autonomous=%s", control->autonomous?"true":"false");
    controller_set_autonomous_mode(control->autonomous);

    return 0;
}


