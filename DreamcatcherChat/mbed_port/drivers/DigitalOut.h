/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
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
#ifndef MBED_DIGITALOUT_H
#define MBED_DIGITALOUT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gpio.h"

#include "libsoc_gpio.h"
#include "libsoc_debug.h"

class DigitalOut {

public:
    DigitalOut(PinName pin);
    DigitalOut(PinName pin, int value);
    void write(int value);
    int read();
    int is_connected();

    DigitalOut& operator= (int value) {
        write(value);
        return *this;
    }

    DigitalOut& operator= (DigitalOut& rhs) {
        write(rhs.read());
        return *this;
    }

    operator int() {
        return read();
    }

protected:
    gpio * gpio_output;
};
#endif

