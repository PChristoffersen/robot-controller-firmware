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

#include "lighting.h"

#include <zephyr/kernel.h>  
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/led.h>
#include <zephyr/drivers/led_strip.h>


LOG_MODULE_REGISTER(lighting);


static const struct device *led_strip = DEVICE_DT_GET(DT_NODELABEL(led_strip));

#define LED_STRIP_PIXEL_COUNT (DT_PROP(DT_NODELABEL(led_strip), chain_length))

static const size_t strip_pixel_count = LED_STRIP_PIXEL_COUNT;
static struct led_rgb strip_pixels[LED_STRIP_PIXEL_COUNT];

static const struct device * pwm_leds = DEVICE_DT_GET(DT_NODELABEL(pwm_leds));
static const size_t pwm_leds_count = DT_CHILD_NUM(DT_NODELABEL(pwm_leds));
static const struct device * gpio_leds = DEVICE_DT_GET(DT_NODELABEL(gpio_leds));
static const size_t gpio_leds_count = DT_CHILD_NUM(DT_NODELABEL(gpio_leds));


int lighting_init(void)
{
    if (!device_is_ready(led_strip)) {
        LOG_ERR("LED Strip Device is not ready");
        return -ENODEV;
    }
    LOG_DBG("LED Strip Device is ready");
    LOG_DBG("Pixel Count: %u", strip_pixel_count);
    LOG_DBG("Pixel Buffer size: %u", sizeof(strip_pixels));

    memset(strip_pixels, 0, sizeof(strip_pixels));
    led_strip_update_rgb(led_strip, strip_pixels, strip_pixel_count);

    return 0;
}




void led_strip_clear()
{
    memset(strip_pixels, 0, sizeof(strip_pixels));
    led_strip_update_rgb(led_strip, strip_pixels, strip_pixel_count);
}



void led_strip_set_color(uint32_t led, uint8_t red, uint8_t green, uint8_t blue)
{
    if (led >= strip_pixel_count) {
        return;
    }
    strip_pixels[led].r = red;
    strip_pixels[led].g = green;
    strip_pixels[led].b = blue;
}


void led_strip_update(void)
{
    led_strip_update_rgb(led_strip, strip_pixels, strip_pixel_count);
}


void led_set(uint8_t led, uint8_t brightness)
{
    if (led <pwm_leds_count) {
        led_set_brightness(pwm_leds, led, brightness);
    }
    else if (led < pwm_leds_count + gpio_leds_count) {
        led_set_brightness(gpio_leds, led - pwm_leds_count, brightness);
    }
    else {
        LOG_ERR("Invalid LED index: %u", led);
    }
}

