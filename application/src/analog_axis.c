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

#include <zephyr/kernel.h>  
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/input/input.h>

#include "controller.h"

LOG_MODULE_REGISTER(axis);

#define AXIS_THREAD_STACK_SIZE 512
#define AXIS_THREAD_PRIORITY 5
#define AXIS_THREAD_DELAY (4* MSEC_PER_SEC)
#define AXIS_POLL_INTERVAL K_MSEC(15)

#define AXIS_ADC_OFFSET 2048

#define FILTER_SHIFT 1



K_TIMER_DEFINE(axis_timer, NULL, NULL);


#define AXIS_ADC_SPEC(node_id, prop, idx) ADC_DT_SPEC_GET_BY_IDX(node_id, idx)

static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM_SEP(DT_PATH(zephyr_user), io_channels, AXIS_ADC_SPEC, (,))
};

static const uint16_t adc_channel_axis[] = {
    DT_FOREACH_PROP_ELEM_SEP(DT_PATH(zephyr_user), zephyr_axis, DT_PROP_BY_IDX, (,))
};

#if DT_PROP_LEN(DT_PATH(zephyr_user), io_channels) != DT_PROP_LEN(DT_PATH(zephyr_user), zephyr_axis)
#error "Number of ADC channels and axis mappings must match"
#endif


static uint16_t adc_prev_values[ARRAY_SIZE(adc_channels)] = {0};


static inline int16_t map_axis(uint16_t raw) 
{
    return ((int16_t)raw) - AXIS_ADC_OFFSET;
}


static void axis_thread_func(void *arg1, void *arg2, void *arg3)
{
    int err;

	uint16_t buf;
    struct adc_sequence sequence = {
		.buffer = &buf,
		/* buffer size in bytes, not number of samples */
		.buffer_size = sizeof(buf),
	};


	for (size_t i=0; i<ARRAY_SIZE(adc_channels); i++) {
		if (!adc_is_ready_dt(&adc_channels[i])) {
			LOG_ERR("ADC controller device %s not ready", adc_channels[i].dev->name);
			return;
		}

		err = adc_channel_setup_dt(&adc_channels[i]);
		if (err < 0) {
			LOG_ERR("Could not setup channel #%d (%d)", i, err);
			return;
		}
	}


    k_timer_start(&axis_timer, AXIS_POLL_INTERVAL, AXIS_POLL_INTERVAL);

    while (true) {
        k_timer_status_sync(&axis_timer);

	    for (size_t i=0; i<ARRAY_SIZE(adc_channels); i++) {
            adc_sequence_init_dt(&adc_channels[i], &sequence);

            err = adc_read_dt(&adc_channels[i], &sequence);
            if (err < 0) {
                LOG_ERR("Could not read channel #%d (%d)", i, err);
                continue;
            }

           // Apply the fixed-point low-pass filter
           buf = (adc_prev_values[i] * (1 << FILTER_SHIFT) - adc_prev_values[i] + buf) >> FILTER_SHIFT;

            if (buf != adc_prev_values[i]) {
                adc_prev_values[i] = buf;
                LOG_DBG("ADC reading[%u]: %d -> %5d", i, buf, map_axis(buf));
                input_report_abs(adc_channels[i].dev, adc_channel_axis[i], map_axis(buf), true, K_FOREVER);
            }
        }
       

    }
}


K_THREAD_DEFINE(axis_thread, AXIS_THREAD_STACK_SIZE, axis_thread_func, NULL, NULL, NULL, AXIS_THREAD_PRIORITY, 0, AXIS_THREAD_DELAY);

