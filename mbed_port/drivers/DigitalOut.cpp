#include "DigitalOut.h"

DigitalOut::DigitalOut(PinName pin)
{
	if (pin!=999){
		gpio_output = libsoc_gpio_request(pin, LS_SHARED);
		if ((gpio_output == NULL))
		{
		fprintf(stderr,"Permissions not sufficient to access the pin %d \n",pin);
		}

		libsoc_gpio_set_direction(gpio_output, OUTPUT);
		// Check the direction
		if (libsoc_gpio_get_direction(gpio_output) != OUTPUT)
		{
		  fprintf(stderr,"Failed to set direction to OUTPUT\n");
		}
	}
	else{
		gpio_output=NULL;
	}
}

DigitalOut::DigitalOut(PinName pin, int value)
{		
	if (pin!=999){
    	gpio_output = libsoc_gpio_request(pin, LS_SHARED);
		if ((gpio_output == NULL))
		{
			fprintf(stderr,"Permissions not sufficient to access the pin %d \n",pin);
			return;
		}
		libsoc_gpio_set_direction(gpio_output, OUTPUT);
		// Check the direction
		if (libsoc_gpio_get_direction(gpio_output) != OUTPUT)
		  fprintf(stderr,"Failed to set direction to OUTPUT pin %d\n",pin);

		libsoc_gpio_set_level(gpio_output, value?HIGH:LOW);
		if (libsoc_gpio_get_level(gpio_output) != value?HIGH:LOW)
	   	 printf("Failed setting gpio level %s for pin %d\n",value?"HIGH":"LOW",pin);
		
	}
	else{
		gpio_output=NULL;
	}
	
}

void  DigitalOut::write(int value){
	if(gpio_output!=NULL){
    	libsoc_gpio_set_level(gpio_output, value?HIGH:LOW);
    	if (libsoc_gpio_get_level(gpio_output) != value?HIGH:LOW)
   	 		printf("Failed setting gpio level test%s\n",value?"HIGH":"LOW");
	}
}

int   DigitalOut::read(){
    return libsoc_gpio_get_level(gpio_output);
}

int   DigitalOut::is_connected(){
    if (gpio_output == NULL)
    {
	return 0;
    }else{
	return 1;
    }
}
