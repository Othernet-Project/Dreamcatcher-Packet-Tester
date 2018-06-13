/*
 * Copyright 2018 Fausto Annicchiarico Petruzzelli <petruzzelli.fausto@gmail.com>
 * Copyright 2018 Luigi Zevola <luigi.zevola@gmail.com>
 *
 * This file is part of DreamcatcherChat.
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

#include "gpio_RFFC.h"
#include <stdio.h>

void gpio_init(gpio_t_rffc gpio){
gpio->gpio_io = libsoc_gpio_request(gpio->pin, LS_SHARED);
    if (gpio->gpio_io == NULL)
    {
	fprintf(stderr,"Permissions not sufficient to access the pin\n");
    }
}

void gpio_set(gpio_t_rffc gpio){
//printf("GPIO SET pin %d\n",gpio->pin);
	if (gpio->gpio_io == NULL)
    {
	gpio->gpio_io = libsoc_gpio_request(gpio->pin, LS_SHARED);
    }

    if (gpio->gpio_io == NULL)
    {
	fprintf(stderr,"Permissions not sufficient to access the pin\n");
    }

	libsoc_gpio_set_level(gpio->gpio_io, HIGH);
    if (libsoc_gpio_get_level(gpio->gpio_io) != HIGH)
   	 	printf("Failed setting gpio level HIGH (GPIO set)\n");

}

void gpio_clear(gpio_t_rffc gpio){
//printf("GPIO Clear pin %d\n",gpio->pin);
	if (gpio->gpio_io == NULL)
    {
	gpio->gpio_io = libsoc_gpio_request(gpio->pin, LS_SHARED);
    }

    if (gpio->gpio_io == NULL)
    {
	fprintf(stderr,"Permissions not sufficient to access the pin \n");
    }
	
	libsoc_gpio_set_level(gpio->gpio_io, LOW);
    if (libsoc_gpio_get_level(gpio->gpio_io) != LOW)
   	 	printf("Failed setting gpio level LOW (GPIO clear)\n");

}

void gpio_toggle(gpio_t_rffc gpio){
	if (gpio->gpio_io == NULL)
    {
	gpio->gpio_io = libsoc_gpio_request(gpio->pin, LS_SHARED);
    }

    if (gpio->gpio_io == NULL)
    {
	fprintf(stderr,"Permissions not sufficient to access the pin \n");
    }

if(libsoc_gpio_get_level(gpio->gpio_io)){
	libsoc_gpio_set_level(gpio->gpio_io, LOW);
    if (libsoc_gpio_get_level(gpio->gpio_io) != LOW)
   	 	printf("Failed setting gpio level LOW (GPIO toggle)\n");
}else{
	libsoc_gpio_set_level(gpio->gpio_io, HIGH);
    if (libsoc_gpio_get_level(gpio->gpio_io) != HIGH)
   	 	printf("Failed setting gpio level HIGH (GPIO toggle)\n");
}
	
}

void gpio_output(gpio_t_rffc gpio){	
	if ((gpio ==NULL) || (gpio->gpio_io == NULL))
    {
		if (gpio !=NULL)
			gpio->gpio_io = libsoc_gpio_request(gpio->pin, LS_SHARED);
    }

    if (gpio->gpio_io == NULL)
    {
		fprintf(stderr,"Permissions not sufficient to access the pin \n");
    }
	
	// Check the direction
	if (libsoc_gpio_get_direction(gpio->gpio_io) == INPUT)
	{
		printf("Pin was declared as input\n"); 
	    libsoc_gpio_free(gpio->gpio_io);
		gpio->gpio_io = libsoc_gpio_request(gpio->pin, LS_SHARED);
		    if (gpio->gpio_io == NULL)
    		{
				fprintf(stderr,"Permissions not sufficient to access the pin\n");
    		}
		
	}

	libsoc_gpio_set_direction(gpio->gpio_io, OUTPUT);
    // Check the direction
    if (libsoc_gpio_get_direction(gpio->gpio_io) != OUTPUT)
      fprintf(stderr,"Failed to set direction to OUTPUT (GPIO output)\n");
}

void gpio_input(gpio_t_rffc gpio){
	if (gpio->gpio_io == NULL)
    {
	gpio->gpio_io = libsoc_gpio_request(gpio->pin, LS_SHARED);
    }

    if (gpio->gpio_io == NULL)
    {
	fprintf(stderr,"Permissions not sufficient to access the pin \n");
    }
	
	// Check the direction
	if (libsoc_gpio_get_direction(gpio->gpio_io) == OUTPUT)
	{
		printf("Pin was declared as output\n"); 
		libsoc_gpio_set_level(gpio->gpio_io, HIGH);
    	if (libsoc_gpio_get_level(gpio->gpio_io) != HIGH)
   	 	printf("Failed setting gpio level HIGH (GPIO set)\n");
	    libsoc_gpio_free(gpio->gpio_io);
		gpio->gpio_io = libsoc_gpio_request(gpio->pin, LS_SHARED);
		    if (gpio->gpio_io == NULL)
    		{
				fprintf(stderr,"Permissions not sufficient to access the pin \n");
    		}
		
	}

	libsoc_gpio_set_direction(gpio->gpio_io, INPUT);
	// Check the direction
	if (libsoc_gpio_get_direction(gpio->gpio_io) != INPUT)
	{
	    fprintf(stderr,"Failed to set direction to INPUT (GPIO input)\n");
	}
}

void gpio_write(gpio_t_rffc gpio, const bool value){

	if (gpio->gpio_io == NULL)
    {
	gpio->gpio_io = libsoc_gpio_request(gpio->pin, LS_SHARED);
    }

    if (gpio->gpio_io == NULL)
    {
	fprintf(stderr,"Permissions not sufficient to access the pin \n");
    }
	if (libsoc_gpio_get_direction(gpio->gpio_io) == OUTPUT){
    	libsoc_gpio_set_level(gpio->gpio_io, value?HIGH:LOW);
    	if (libsoc_gpio_get_level(gpio->gpio_io) != value?HIGH:LOW)
   	 		printf("Failed setting gpio level (GPIO write )%s\n",value?"HIGH":"LOW");
	}
}

bool gpio_read(gpio_t_rffc gpio){
	if (gpio->gpio_io == NULL)
    {
	gpio->gpio_io = libsoc_gpio_request(gpio->pin, LS_SHARED);
    }

    if (gpio->gpio_io == NULL)
    {
	fprintf(stderr,"Permissions not sufficient to access the pin\n");
    }

 	return libsoc_gpio_get_level(gpio->gpio_io);
}

