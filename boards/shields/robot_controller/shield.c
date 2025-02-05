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

#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/led.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(shield);



#if DT_NODE_EXISTS(DT_NODELABEL(gpio_leds)) || DT_NODE_EXISTS(DT_NODELABEL(pwm_leds)) || DT_NODE_EXISTS(DT_NODELABEL(led_strip)) || DT_NODE_EXISTS(DT_NODELABEL(pcf8574))
static int shield_init()
{
    /**
     * Turn off all leds on boot.
     */
    #if DT_NODE_EXISTS(DT_NODELABEL(gpio_leds))
    {
        const struct device *gpio_leds = DEVICE_DT_GET(DT_NODELABEL(gpio_leds));
        const size_t gpio_leds_count = DT_CHILD_NUM_STATUS_OKAY(DT_NODELABEL(gpio_leds));
        if (device_is_ready(gpio_leds)) {
            LOG_DBG("Turning off %u gpio leds", gpio_leds_count);
            for (size_t i = 0; i < gpio_leds_count; i++) {
                led_off(gpio_leds, i);
            }
        }
    }
    #endif
    #if DT_NODE_EXISTS(DT_NODELABEL(pwm_leds))
    {
        const struct device *pwm_leds = DEVICE_DT_GET(DT_NODELABEL(pwm_leds));
        const size_t pwm_leds_count = DT_CHILD_NUM_STATUS_OKAY(DT_NODELABEL(pwm_leds));
        if (device_is_ready(pwm_leds)) {
            LOG_DBG("Turning off %u pwm leds", pwm_leds_count);
            for (size_t i = 0; i < pwm_leds_count; i++) {
                led_off(pwm_leds, i);
            }
        }
    }
    #endif
    #if DT_NODE_EXISTS(DT_NODELABEL(led_strip))
    {
        const struct device *led_strip = DEVICE_DT_GET(DT_NODELABEL(led_strip));
        if (device_is_ready(led_strip)) {
            LOG_DBG("Turning off led strip");
            size_t led_count = led_strip_length(led_strip);
            struct led_rgb led[led_count];
            memset(led, 0, sizeof(led));
            led_strip_update_rgb(led_strip, led, led_count);
        }
    }
    #endif
    #if DT_NODE_EXISTS(DT_NODELABEL(pcf8574))
    {
        /**
         * Hack to fix the pcf8574 driver. 
         * For some reason the pcf8574 does not trigger interrupts unless the pins are set 
         * to high.
         */
        const struct device *pcf8574 = DEVICE_DT_GET(DT_NODELABEL(pcf8574));
        const struct i2c_dt_spec pcf8574_i2c = I2C_DT_SPEC_GET(DT_NODELABEL(pcf8574));
        if (device_is_ready(pcf8574)) {
            LOG_DBG("Fixing pcf8574 driver interrupts");
            uint8_t value = 0xFF;
            value = 0xFF;
            if (i2c_write_dt(&pcf8574_i2c, &value, 1) != 0) {
                LOG_ERR("pcf8574 write failed");
                return -EIO;
            }
        }
    }
    #endif
	return 0;
}
SYS_INIT(shield_init, POST_KERNEL, CONFIG_SHIELD_ROBOT_CONTROLLER_INIT_PRIORITY);
#endif
