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


K_MUTEX_DEFINE(g_input_mutex);
K_CONDVAR_DEFINE(g_input_cond);
K_CONDVAR_DEFINE(g_report_cond);

static hid_gamepad_input_t g_input_state = {
    .report_id = REPORT_ID_GAMEPAD,
    .dpad = 0xF,
};
static bool g_input_state_changed = false;

static led_state_t g_led_state = 0;

static uint32_t g_idle_duration = 0;

static bool g_enabled = false;
static bool g_autonomous = true;
static bool g_suspended = false;
static bool g_report_pending = false;
K_TIMER_DEFINE(g_report_timer, NULL, NULL);


static void update_leds(led_state_t state)
{
    for (uint8_t i=0; i<CONTROLLER_LED_COUNT; i++) {
        led_set(i, (state & (1<<i)) ? 100 : 0);
    }
}

static void start_report_timer()
{
    k_timer_start(&g_report_timer, K_MSEC(CONFIG_ROBOT_CONTROLLER_MIN_INPUT_REPORT_INTERVAL_MS), K_NO_WAIT);
}


/* -----------------------------------------------------------------------------------
 * State handling
 * ----------------------------------------------------------------------------------- */

void controller_reset()
{
    k_mutex_lock(&g_input_mutex, K_FOREVER);
    if (g_enabled) {
        LOG_INF("Controller Reset");
        update_leds(g_led_state);
        lamp_array_reset();
        g_idle_duration = 0;
        g_autonomous = true;
        g_led_state = 0;
        g_input_state_changed = true;
        start_report_timer();
        k_condvar_broadcast(&g_input_cond);
    }
    k_mutex_unlock(&g_input_mutex);
}

void controller_set_enabled(bool enabled)
{
    k_mutex_lock(&g_input_mutex, K_FOREVER);
    if (enabled != g_enabled) {
        LOG_INF("Controller %s", enabled?"enabled":"disabled");
        g_enabled = enabled;
        if (enabled) {
            start_report_timer();
        }
        k_condvar_broadcast(&g_input_cond);
    }
    k_mutex_unlock(&g_input_mutex);
}

void controller_set_suspend(bool suspended)
{
    k_mutex_lock(&g_input_mutex, K_FOREVER);
    if (suspended != g_suspended) {
        g_suspended = suspended;
        if (g_enabled) {
            LOG_INF("Controller %s", suspended?"suspended":"resumed");
            if (g_suspended) {
                g_autonomous = true;
                controller_set_idle(0);
                led_strip_clear();
                lamp_array_reset();
                update_leds(0x00);
            }
            else {
                if (g_autonomous) {
                    update_leds(g_led_state);
                }
                start_report_timer();
                g_input_state_changed = true;
            }
            k_condvar_broadcast(&g_input_cond);
        }
    }
    k_mutex_unlock(&g_input_mutex);
}



int controller_set_output(const uint8_t *buffer, uint16_t bufsize)
{
    if (bufsize < sizeof(hid_gamepad_output_t)) {
        return -EINVAL;
    }

    const hid_gamepad_output_t *output = (const hid_gamepad_output_t *)buffer;

    LOG_DBG("Set Output: leds=%02x", output->leds);
    k_mutex_lock(&g_input_mutex, K_FOREVER);
    if (g_led_state != output->leds) {
        g_led_state = output->leds;
        if (g_autonomous && !g_suspended) {
            update_leds(g_led_state);
        }
    }
    k_mutex_unlock(&g_input_mutex);

    return -ENOTSUP;
}


void controller_set_idle(const uint32_t duration)
{
    LOG_DBG("Set Idle: duration=%u", duration);
    k_mutex_lock(&g_input_mutex, K_FOREVER);
    if (g_idle_duration != duration) {
        g_idle_duration = duration;
        k_condvar_broadcast(&g_input_cond);
    }
    k_mutex_unlock(&g_input_mutex);
}


uint32_t controller_get_idle()
{
    LOG_DBG("Get Idle: duration=%u", g_idle_duration);
    return g_idle_duration;
}





void controller_set_autonomous_mode(bool enabled)
{
    k_mutex_lock(&g_input_mutex, K_FOREVER);
    if (enabled != g_autonomous) {
        g_autonomous = enabled;
        led_strip_clear();
        if (g_autonomous) {
            update_leds(g_led_state);
        }
        else {
            update_leds(0x00);
        }
    }
    k_mutex_unlock(&g_input_mutex);
}

bool controller_is_autonomous_mode()
{
    return g_autonomous;
}


void controller_input_report_done()
{
    k_mutex_lock(&g_input_mutex, K_FOREVER);
    g_report_pending = false;
    k_condvar_broadcast(&g_report_cond);
    k_mutex_unlock(&g_input_mutex);
}



/* -----------------------------------------------------------------------------------
 * Input Event handling
 * ----------------------------------------------------------------------------------- */

static void input_cb(struct input_event *event, void *user_data)
{
    if (k_mutex_lock(&g_input_mutex, K_MSEC(20)) != 0) {
        // Drop event if it takes too long to acquire the mutex
        return;
    }
    static dpad_state_t dpad_state = {0};
    switch (event->type) {
        case INPUT_EV_KEY:
            LOG_DBG("Key Event: code=0x%x, value=%d", event->code, event->value);
            g_input_state_changed |= input_process_key(event, &g_input_state, &dpad_state);
            break;
        case INPUT_EV_REL:
            LOG_DBG("Relative Event: code=0x%x, value=%d", event->code, event->value);
            g_input_state_changed |= input_process_dial(event, &g_input_state);
            break;
        case INPUT_EV_ABS:
            LOG_DBG("Absolute Event: code=0x%x, value=%d", event->code, event->value);
            g_input_state_changed |= input_process_axis(event, &g_input_state);
            break;
    }
    if (g_input_state_changed) {
        g_input_state.dpad = input_dpad_map(&dpad_state);
        k_condvar_broadcast(&g_input_cond);
    }
    k_mutex_unlock(&g_input_mutex);
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
    int ret;

	banner();

	lighting_init();
	controller_hid_device_init();
	controller_usb_device_init();

    // Initialization done - lower priority and start main loop
    k_thread_priority_set(k_current_get(), K_LOWEST_APPLICATION_THREAD_PRIO);

    k_mutex_lock(&g_input_mutex, K_FOREVER);
    start_report_timer();
    while (true) {
        if (!g_enabled || g_suspended) {
            LOG_DBG("Wait... enabled=%d, suspended=%d", g_enabled, g_suspended);
            k_mutex_unlock(&g_input_mutex);
            k_msleep(100);
            k_mutex_lock(&g_input_mutex, K_FOREVER);
            continue;
        }

        k_mutex_unlock(&g_input_mutex);
        k_timer_status_sync(&g_report_timer);
        k_mutex_lock(&g_input_mutex, K_FOREVER);

        // Wait for input event or idle timeout
        if (!g_input_state_changed) {
    		k_condvar_wait(&g_input_cond, &g_input_mutex, g_idle_duration>0 ? K_MSEC(g_idle_duration) : K_FOREVER);
        }
        if (!g_enabled || g_suspended) {
            continue;
        }

        if (g_input_state_changed || g_idle_duration>0) {
            LOG_DBG("Submit input_state_changed=%d idle_state=%u  enabled=%d,  suspended=%d", g_input_state_changed, g_idle_duration, g_enabled, g_suspended);
            
            // Wait for previous report to be sent
            while (g_report_pending) {
                k_condvar_wait(&g_report_cond, &g_input_mutex, K_FOREVER);
            }

            // Submit gamepad state
            ret = controller_hid_submit_gamepad((const uint8_t *)&g_input_state, sizeof(g_input_state));
            if (ret != 0) {
                LOG_ERR("Failed to submit gamepad state: %d", ret);
                break;
            }
            start_report_timer();
            g_input_state.dial = 0; // Reset dial relative value
            g_input_state_changed = false; // Clear input state change flag
            g_report_pending = true; // Set report pending flag
        }
    }
    k_mutex_unlock(&g_input_mutex);

	return 0;
}
