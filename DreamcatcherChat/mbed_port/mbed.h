#ifndef MBED_H
#define MBED_H

// Useful C libraries
#include <math.h>
#include <time.h>

// mbed Peripheral components
#include "drivers/DigitalIn.h"
#include "drivers/DigitalOut.h"
#include "drivers/DigitalInOut.h"
#include "drivers/SPI.h"

// mbed Internal components
#include "drivers/InterruptIn.h"


#define NC 999

void wait_ms(int ms);
void __enable_irq( );
void __disable_irq( );

#endif
