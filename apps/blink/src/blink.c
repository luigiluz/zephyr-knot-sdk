/* blink.c - KNoT Application Client */

/*
 * Copyright (c) 2019, CESAR. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * The knot client application is acting as a client that is run in Zephyr OS,
 * The client sends sensor data encapsulated using KNoT protocol.
 */


#include <zephyr.h>
#include <net/net_core.h>
#include <logging/log.h>
#include <device.h>
#include <gpio.h>

#include "knot.h"
#include <knot/knot_types.h>
#include <knot/knot_protocol.h>

#define LED_PORT	LED1_GPIO_CONTROLLER /* General GPIO Controller */
#define LED_PIN		LED1_GPIO_PIN /* User LED */

LOG_MODULE_REGISTER(blink, LOG_LEVEL_DBG);

bool led = true; 			/* Tracked value */
struct device *gpio_led;		/* GPIO device */

int write_led(int id)
{
	LOG_INF("Value for led changed to %d", led);
	gpio_pin_write(gpio_led, LED_PIN, !led); /* Led is On at LOW */

	return KNOT_CALLBACK_SUCCESS;
}

void setup(void)
{
	/* Peripherals control */
	gpio_led = device_get_binding(LED_PORT);
	gpio_pin_configure(gpio_led, LED_PIN, GPIO_DIR_OUT);

	/* KNoT config */
	knot_data_register(0, "LED", KNOT_TYPE_ID_SWITCH,
			   KNOT_VALUE_TYPE_BOOL, KNOT_UNIT_NOT_APPLICABLE,
			   &led, sizeof(led), write_led, NULL);
	knot_data_config(0, KNOT_EVT_FLAG_CHANGE, NULL);

}

int64_t last_toggle_time = 0;

void loop(void)
{
	/* Get current time */
	int64_t current_time = k_uptime_get();

	/* Toggle led every 10 seconds */
	if (current_time - last_toggle_time > 10000) {
		led = !led;
		gpio_pin_write(gpio_led, LED_PIN, !led); /* Update GPIO */
		last_toggle_time = current_time;
	}
}
