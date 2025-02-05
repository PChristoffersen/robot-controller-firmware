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

#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/version.h>
#include <zephyr/drivers/hwinfo.h>
#include <zephyr/input/input.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main);

#include "controller.h"
#include "usb_device.h"
#include "hid_device.h"
#include "lamp_array.h"
#include "lighting.h"
#include "input_mapping.h"
#include "util.h"


K_MUTEX_DEFINE(input_mutex);
K_CONDVAR_DEFINE(input_cond);
K_CONDVAR_DEFINE(report_cond);

static hid_gamepad_input_t input_state = {
    .report_id = REPORT_ID_GAMEPAD,
    .dpad = 0xF,
};
static bool input_state_changed = false;

static led_state_t led_state = 0;

static uint32_t idle_duration = 0;

static bool enabled = false;
static bool autonomous = true;
static bool suspended = false;
static bool report_pending = false;


static void update_leds(led_state_t state)
{
    for (uint8_t i=0; i<CONTROLLER_LED_COUNT; i++) {
        led_set(i, (state & (1<<i)) ? 100 : 0);
    }
}


/* -----------------------------------------------------------------------------------
 * State handling
 * ----------------------------------------------------------------------------------- */

void controller_reset()
{
    LOG_INF("Controller Reset");
    k_mutex_lock(&input_mutex, K_FOREVER);
    idle_duration = 0;
    autonomous = true;
    led_state = 0;
    update_leds(led_state);
    lamp_array_reset();
    k_mutex_unlock(&input_mutex);
}

void controller_set_enabled(bool state)
{
    k_mutex_lock(&input_mutex, K_FOREVER);
    if (state != enabled) {
        LOG_INF("Controller %s", state?"enabled":"disabled");
        enabled = state;
    }
    k_mutex_unlock(&input_mutex);
}

void controller_set_suspend(bool state)
{
    k_mutex_lock(&input_mutex, K_FOREVER);
    if (state != suspended) {
        LOG_INF("Controller %s", state?"suspended":"resumed");
        suspended = state;
        if (suspended) {
            autonomous = true;
            controller_set_idle(0);
            led_strip_clear();
            lamp_array_reset();
        }
    }
    k_mutex_unlock(&input_mutex);
}



int controller_set_output(const uint8_t *buffer, uint16_t bufsize)
{
    if (bufsize < sizeof(hid_gamepad_output_t)) {
        return -EINVAL;
    }

    const hid_gamepad_output_t *output = (const hid_gamepad_output_t *)buffer;

    LOG_INF("Set Output: leds=%02x", output->leds);
    k_mutex_lock(&input_mutex, K_FOREVER);
    if (led_state != output->leds) {
        led_state = output->leds;
        if (autonomous) {
            update_leds(led_state);
        }
    }
    k_mutex_unlock(&input_mutex);

    return -ENOTSUP;
}


void controller_set_idle(const uint32_t duration)
{
    LOG_INF("Set Idle: duration=%u", duration);
    k_mutex_lock(&input_mutex, K_FOREVER);
    idle_duration = duration;
    k_condvar_broadcast(&input_cond);
    k_mutex_unlock(&input_mutex);
}


uint32_t controller_get_idle()
{
    LOG_INF("Get Idle: duration=%u", idle_duration);
    return idle_duration;
}





void controller_set_autonomous_mode(bool enabled)
{
    k_mutex_lock(&input_mutex, K_FOREVER);
    if (enabled != autonomous) {
        autonomous = enabled;
        led_strip_clear();
        if (autonomous) {
            update_leds(led_state);
        }
        else {
            update_leds(0x00);
        }
    }
    k_mutex_unlock(&input_mutex);
}

bool controller_is_autonomous_mode()
{
    return autonomous;
}


void controller_input_report_done()
{
    k_mutex_lock(&input_mutex, K_FOREVER);
    report_pending = false;
    k_condvar_broadcast(&report_cond);
    k_mutex_unlock(&input_mutex);
}



/* -----------------------------------------------------------------------------------
 * Input Event handling
 * ----------------------------------------------------------------------------------- */

static void input_cb(struct input_event *event, void *user_data)
{
    if (k_mutex_lock(&input_mutex, K_MSEC(20)) != 0) {
        // Drop event if it takes too long to acquire the mutex
        return;
    }
    static dpad_state_t dpad_state = {0};
    switch (event->type) {
        case INPUT_EV_KEY:
            LOG_DBG("Key Event: code=0x%x, value=%d", event->code, event->value);
            input_state_changed |= input_process_key(event, &input_state, &dpad_state);
            break;
        case INPUT_EV_REL:
            LOG_DBG("Relative Event: code=0x%x, value=%d", event->code, event->value);
            input_state_changed |= input_process_dial(event, &input_state);
            break;
        case INPUT_EV_ABS:
            LOG_DBG("Absolute Event: code=0x%x, value=%d", event->code, event->value);
            input_state_changed |= input_process_axis(event, &input_state);
            break;
    }
    if (input_state_changed) {
        input_state.dpad = input_dpad_map(&dpad_state);
        k_condvar_broadcast(&input_cond);
    }
    k_mutex_unlock(&input_mutex);
}

INPUT_CALLBACK_DEFINE(NULL, input_cb, NULL);



/* -----------------------------------------------------------------------------------
 * Main loop
 * ----------------------------------------------------------------------------------- */

static void banner()
{
	printf("\n");
	printf("---------------------------------\n");
	printf("Zephyr OS version: %s\n", KERNEL_VERSION_STRING);
	printf("---------------------------------\n");
	printf("\n");

	print_reset_cause();
	hwinfo_clear_reset_cause();

	printf("\n\n");
}


int main(void)
{
	banner();

	lighting_init();
	controller_hid_device_init();
	controller_usb_device_init();


    int ret;

    k_mutex_lock(&input_mutex, K_FOREVER);
    while (true) {
        // Wait for input event or idle timeout
        if (!input_state_changed) {
    		k_condvar_wait(&input_cond, &input_mutex, idle_duration>0 ? K_MSEC(idle_duration) : K_FOREVER);
        }

        if ((input_state_changed || idle_duration>0) && enabled && !suspended) {
            LOG_DBG("Submit input_state_changed=%d idle_state=%u  enabled=%d,  suspended=%d", input_state_changed, idle_duration, enabled, suspended);
            
            // Wait for previous report to be sent
            while (report_pending) {
                k_condvar_wait(&report_cond, &input_mutex, K_FOREVER);
            }

            // Submit gamepad state
            ret = controller_hid_submit_gamepad((const uint8_t *)&input_state, sizeof(input_state));
            if (ret != 0) {
                LOG_ERR("Failed to submit gamepad state: %d", ret);
                break;
            }
            input_state.dial = 0; // Reset dial relative value
            input_state_changed = false; // Clear input state change flag
            report_pending = true; // Set report pending flag
        
        }

        // Make sure we are not spamming reports
        k_mutex_unlock(&input_mutex);
        k_sleep(K_MSEC(10));
        k_mutex_lock(&input_mutex, K_FOREVER);
    }
    k_mutex_unlock(&input_mutex);

	return 0;
}
