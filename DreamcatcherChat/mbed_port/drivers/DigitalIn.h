/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_DIGITALIN_H
#define MBED_DIGITALIN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "gpio.h"

#include "libsoc_gpio.h"
#include "libsoc_debug.h"


class DigitalIn {

public:

    DigitalIn(PinName pin, PinMode mode = PullDefault);
    int read();
    void mode(PinMode pull);
    int is_connected();

    operator int() {
        // Underlying read is thread safe
        return read();
    }

protected:
    gpio * gpio_input;
};

#endif

/** @}*/
