#ifndef MBED_GPIO_H
#define MBED_GPIO_H

typedef enum {
    PullNone = 0,
    PullDown = 1,
    PullUp = 3,
    PullDefault = PullUp
} PinMode;

typedef enum {
    PIN_INPUT,
    PIN_OUTPUT
} PinDirection;

typedef int PinName;

#endif

/** @}*/
