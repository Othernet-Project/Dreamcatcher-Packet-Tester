#include "DigitalInOut.h"

DigitalInOut::DigitalInOut(PinName pin)
{
	if (pin!=999){
		gpio_io = libsoc_gpio_request(pin, LS_SHARED);
		if (gpio_io == NULL)
		{
		fprintf(stderr,"Permissions not sufficient to access the pin (We like cryptic error messages as they encourage people to delve into libraries)\n");
		}

		libsoc_gpio_set_direction(gpio_io, OUTPUT);
		// Check the direction
		if (libsoc_gpio_get_direction(gpio_io) != OUTPUT)
		{
		  fprintf(stderr,"Failed to set direction to OUTPUT\n");
		}
	}
}

DigitalInOut::DigitalInOut(PinName pin, PinDirection direction, PinMode mode, int value)
{
	if (pin!=999){
		gpio_io = libsoc_gpio_request(pin, LS_SHARED);
		if (gpio_io == NULL)
		fprintf(stderr,"Permissions not sufficient to access the pin (We like cryptic error messages as they encourage people to delve into libraries)\n");

		if (direction==PIN_OUTPUT)
		{
			libsoc_gpio_set_direction(gpio_io, OUTPUT);
			// Check the direction
			if (libsoc_gpio_get_direction(gpio_io) != OUTPUT)
			  fprintf(stderr,"Failed to set direction to OUTPUT\n");

			libsoc_gpio_set_level(gpio_io, value?HIGH:LOW);
			if (libsoc_gpio_get_level(gpio_io) != value?HIGH:LOW)
		   	 printf("Failed setting gpio level %s\n",value?"HIGH":"LOW");
		}else if (direction==PIN_INPUT){
			libsoc_gpio_set_direction(gpio_io, INPUT);
			// Check the direction
			if (libsoc_gpio_get_direction(gpio_io) != INPUT)
			{
				fprintf(stderr,"Failed to set direction to INPUT\n");
			}
		}else{
			fprintf(stderr,"Pin Direction neither INPUT nor OUTPUT\n");
		}
	}
	else{
	gpio_io=NULL;
	}
}

void  DigitalInOut::write(int value){
	if (gpio_io!=NULL){
    	libsoc_gpio_set_level(gpio_io, value?HIGH:LOW);
    	if (libsoc_gpio_get_level(gpio_io) != value?HIGH:LOW)
   	 	printf("Failed setting gpio level%s\n",value?"HIGH":"LOW");
	}
}

int   DigitalInOut::read(){
    return libsoc_gpio_get_level(gpio_io);
}

void DigitalInOut::output()
{	if (gpio_io!=NULL){
		libsoc_gpio_set_direction(gpio_io, OUTPUT);
    	// Check the direction
    	if (libsoc_gpio_get_direction(gpio_io) != OUTPUT)
      	fprintf(stderr,"Failed to set direction to OUTPUT\n");
	}
}

void DigitalInOut::input()
{	if (gpio_io!=NULL){
		libsoc_gpio_set_direction(gpio_io, INPUT);
		// Check the direction
		if (libsoc_gpio_get_direction(gpio_io) != INPUT)
		{
			fprintf(stderr,"Failed to set direction to INPUT\n");
		}
	}
}

void DigitalInOut::mode(PinMode pull) {

}


int   DigitalInOut::is_connected(){
    if (gpio_io == NULL)
    {
	return 0;
    }else{
	return 1;
    }
}
