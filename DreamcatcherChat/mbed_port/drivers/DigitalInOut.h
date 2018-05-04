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
#ifndef MBED_DIGITALINOUT_H
#define MBED_DIGITALINOUT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "gpio.h"

#include "libsoc_gpio.h"
#include "libsoc_debug.h"

class DigitalInOut {

public:
    DigitalInOut(PinName pin);
    DigitalInOut(PinName pin, PinDirection direction, PinMode mode, int value);
    void write(int value);
    int read();
    void output();
    void input() ;
    void mode(PinMode pull);
    int is_connected();

    /** A shorthand for write()
     */
    DigitalInOut& operator= (int value) {
        // Underlying write is thread safe
        write(value);
        return *this;
    }

    DigitalInOut& operator= (DigitalInOut& rhs) {
        write(rhs.read());
        return *this;
    }

    operator int() {
        return read();
    }

protected:
    gpio * gpio_io;
};

#endif
