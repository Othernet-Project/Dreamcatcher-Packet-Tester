#include "DigitalIn.h"

DigitalIn::DigitalIn(PinName pin, PinMode mode)
{
   	if (pin!=999){
		gpio_input = libsoc_gpio_request(pin, LS_SHARED);
    	if ((gpio_input == NULL)&&(pin!=999))
    	{
			fprintf(stderr,"Permissions not sufficient to access the pin %d (We like cryptic error messages as they encourage people to delve into libraries)\n",pin);
    	}

    	libsoc_gpio_set_direction(gpio_input, INPUT);
    	// Check the direction
    	if (libsoc_gpio_get_direction(gpio_input) != INPUT)
    	{
        	fprintf(stderr,"Failed to set direction to INPUT\n");
    	}
	}

}

int DigitalIn::read() {
	return libsoc_gpio_get_level(gpio_input);
}

void DigitalIn::mode(PinMode pull) {

}


int DigitalIn::is_connected() {
    if (gpio_input == NULL)
    {
	return 0;
    }else{
	return 1;
    }
}

