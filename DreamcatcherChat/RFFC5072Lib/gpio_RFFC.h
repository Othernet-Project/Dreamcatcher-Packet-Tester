/*
 * Copyright (C) 2014 Jared Boone, ShareBrained Technology, Inc.
 *
 * This file is part of HackRF.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __GPIO_RFFC_H__
#define __GPIO_RFFC_H__

#include <stdbool.h>
#include <cstdlib>
#include <libsoc_gpio.h>
#include <libsoc_debug.h>
#include "../mbed_port/drivers/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif


struct gpio_rffc {
 	PinName pin;
	gpio * gpio_io;
 };

typedef struct gpio_rffc* gpio_t_rffc; 

void gpio_init(gpio_t_rffc gpio);
void gpio_set(gpio_t_rffc gpio);
void gpio_clear(gpio_t_rffc gpio);
void gpio_toggle(gpio_t_rffc gpio);
void gpio_output(gpio_t_rffc gpio);
void gpio_input(gpio_t_rffc gpio);
void gpio_write(gpio_t_rffc gpio, const bool value);
bool gpio_read(gpio_t_rffc gpio);

#ifdef __cplusplus
}
#endif

#endif/*__GPIO_RFFC_H__*/
