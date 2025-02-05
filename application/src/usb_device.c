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

#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/hwinfo.h>

#include "controller.h"

LOG_MODULE_REGISTER(usbd);

static const char *const blocklist[] = {
	"dfu_dfu",
	NULL,
};


USBD_DEVICE_DEFINE(
    controller_usbd, 
    DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)), 
    CONFIG_ROBOT_CONTROLLER_USBD_VID, 
    CONFIG_ROBOT_CONTROLLER_USBD_PID
);



USBD_DESC_LANG_DEFINE(controller_lang);
USBD_DESC_MANUFACTURER_DEFINE(controller_mfr, CONFIG_ROBOT_CONTROLLER_USBD_MANUFACTURER);
USBD_DESC_PRODUCT_DEFINE(controller_product, CONFIG_ROBOT_CONTROLLER_USBD_PRODUCT);
USBD_DESC_SERIAL_NUMBER_DEFINE(controller_sn);

USBD_DESC_CONFIG_DEFINE(fs_cfg_desc, "FS Configuration");

/* doc configuration instantiation start */
static const uint8_t attributes = 
	(IS_ENABLED(CONFIG_ROBOT_CONTROLLER_USBD_REMOTE_WAKEUP) ? USB_SCD_REMOTE_WAKEUP : 0);

/* Full speed configuration */
USBD_CONFIGURATION_DEFINE(controller_fs_config, attributes, CONFIG_ROBOT_CONTROLLER_USBD_MAX_POWER, &fs_cfg_desc);
/* doc configuration instantiation end */


/* ------------------------------------------------
 * Message callback
 * ------------------------------------------------ */
static void on_usb_suspend(struct usbd_context *const ctx)
{
	controller_set_suspend(true);
}

static void on_usb_resume(struct usbd_context *const ctx)
{
	controller_set_suspend(false);
}

static void on_usb_reset(struct usbd_context *const ctx)
{
	controller_reset();
}

static void on_usb_configuation(struct usbd_context *const ctx, int status)
{
	controller_set_enabled(true);
}

static void on_usb_error(struct usbd_context *const ctx)
{
}


static void usbd_msg_cb(struct usbd_context *const ctx, const struct usbd_msg *const msg)
{
	LOG_INF("USBD message: %s", usbd_msg_type_string(msg->type));

	switch (msg->type) {
	case USBD_MSG_RESUME:
		on_usb_resume(ctx);
		break;
	case USBD_MSG_SUSPEND:
		on_usb_suspend(ctx);
		break;
	case USBD_MSG_RESET:
		on_usb_reset(ctx);
		break;
	case USBD_MSG_CONFIGURATION:
		LOG_INF("\tConfiguration value %d", msg->status);
		on_usb_configuation(ctx, msg->status);
		break;
	case USBD_MSG_UDC_ERROR:
		on_usb_error(ctx);
		break;
	default:
		break;
	}
}




/* ------------------------------------------------
 * Initialization
 * ------------------------------------------------ */

int controller_usb_device_init()
{
    int err;

	/* doc add string descriptor start */
	err = usbd_add_descriptor(&controller_usbd, &controller_lang);
	if (err) {
		LOG_ERR("Failed to initialize language descriptor (%d)", err);
		return err;
	}

	err = usbd_add_descriptor(&controller_usbd, &controller_mfr);
	if (err) {
		LOG_ERR("Failed to initialize manufacturer descriptor (%d)", err);
		return err;
	}

	err = usbd_add_descriptor(&controller_usbd, &controller_product);
	if (err) {
		LOG_ERR("Failed to initialize product descriptor (%d)", err);
		return err;
	}

	err = usbd_add_descriptor(&controller_usbd, &controller_sn);
	if (err) {
		LOG_ERR("Failed to initialize SN descriptor (%d)", err);
		return err;
	}
	/* doc add string descriptor end */

	/* doc configuration register start */
	err = usbd_add_configuration(&controller_usbd, USBD_SPEED_FS, &controller_fs_config);
	if (err) {
		LOG_ERR("Failed to add Full-Speed configuration");
		return err;
	}
	/* doc configuration register end */

	/* doc functions register start */
	err = usbd_register_all_classes(&controller_usbd, USBD_SPEED_FS, 1, blocklist);
	if (err) {
		LOG_ERR("Failed to add register classes");
		return err;
	}
	/* doc functions register end */

    usbd_device_set_code_triple(&controller_usbd, USBD_SPEED_FS, 0, 0, 0);

	/* doc device init-and-msg start */
	err = usbd_msg_register_cb(&controller_usbd, usbd_msg_cb);
	if (err) {
		LOG_ERR("Failed to register message callback");
		return err;
	}
	/* doc device init-and-msg end */

	err = usbd_init(&controller_usbd);
	if (err) {
		LOG_ERR("Failed to initialize device support");
		return err;
	}


	err = usbd_enable(&controller_usbd);
	if (err) {
		LOG_ERR("Failed to enable device support");
		return err;
	}

	LOG_DBG("USB device support enabled");

	return 0;
}


#if CONFIG_BOARD_STM32_MIN_DEV
/**
 * If 
 * 
 * Force a reset of the USB interface by pulling the PD pin low for 500us
 * before initializing the USB stack.
 */
#define USB_GPIO_PORT gpioa
#define USB_GPIO_PIN_PD 12

static int reset_usb_interface()
{
	uint32_t cause = 0;
	hwinfo_get_reset_cause(&cause);
	if (!(cause & RESET_POR)) {
		// Reset USB interface unless reset cause is power-on-reset
		const struct device *gpio_port = DEVICE_DT_GET(DT_NODELABEL(USB_GPIO_PORT));
		gpio_pin_configure(gpio_port, USB_GPIO_PIN_PD, GPIO_OUTPUT);
		gpio_pin_set(gpio_port, USB_GPIO_PIN_PD, 0);
		k_sleep(K_USEC(500));
		gpio_pin_configure(gpio_port, USB_GPIO_PIN_PD, GPIO_INPUT);
	}
    return 0;
}
SYS_INIT(reset_usb_interface, POST_KERNEL, 40);
#endif


