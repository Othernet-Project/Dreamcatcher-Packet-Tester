#include "InterruptIn.h"
#include <cstdio>

InterruptIn::InterruptIn(PinName pin) : _rise(NULL),
                                        _fall(NULL) {
	if (pin!=999){	
		gpio_irq.pin = pin;
		gpio_irq.gp = libsoc_gpio_request(pin, LS_SHARED);
		if (gpio_irq.gp == NULL)
		{
			fprintf(stderr,"Permissions not sufficient to access the pin (We like cryptic error messages as they encourage people to delve into libraries)\n");
		}

		libsoc_gpio_set_edge(gpio_irq.gp, NONE);

		libsoc_gpio_set_direction(gpio_irq.gp, INPUT);
		// Check the direction
		if (libsoc_gpio_get_direction(gpio_irq.gp) != INPUT)
		{
		    fprintf(stderr,"Failed to set direction to INPUT\n");
		}

		irq_init();
	}
}

InterruptIn::InterruptIn(PinName pin, PinMode mode) :
                                        _rise(NULL),
                                        _fall(NULL) {

	gpio_irq.gp = libsoc_gpio_request(pin, LS_SHARED);
    if (gpio_irq.gp == NULL)
    {
		fprintf(stderr,"Permissions not sufficient to access the pin\n");
    }

	libsoc_gpio_set_edge(gpio_irq.gp, NONE);

	libsoc_gpio_set_direction(gpio_irq.gp, INPUT);
    // Check the direction
    if (libsoc_gpio_get_direction(gpio_irq.gp) != INPUT)
    {
        fprintf(stderr,"Failed to set direction to INPUT\n");
    }

	libsoc_gpio_set_edge(gpio_irq.gp, NONE);
	if (libsoc_gpio_get_edge(gpio_irq.gp) != NONE)
  	{
    	printf("Failed to set edge to NONE\n");
  	}	

	irq_init();
}


void InterruptIn::irq_init() {
	#ifdef DEBUG
   		printf("Initializing IRQ on pin %u\n",gpio_irq.pin);
	#endif
   gpio_irq_init2(&gpio_irq, (&InterruptIn::_irq_handler), (uint32_t)this);
}

InterruptIn::~InterruptIn() {
	#ifdef DEBUG
   		printf("Destroying interrupt on pin %u\n",gpio_irq.pin);
	#endif
    // No lock needed in the destructor
    gpio_irq_free(&gpio_irq);
}

int InterruptIn::read() {
    // Read only
    return libsoc_gpio_get_level(gpio_irq.gp);
}

void InterruptIn::mode(PinMode pull) {
    //core_util_critical_section_enter();
    //gpio_mode(&gpio, pull);
    //core_util_critical_section_exit();
}

void InterruptIn::rise(Callback<void()> func) {
    //core_util_critical_section_enter();
    if (func) {
        _rise = func;
		#ifdef DEBUG
			printf("Binding function to rising edge on pin %u\n", gpio_irq.pin);
		#endif
        gpio_irq_set(&gpio_irq, IRQ_RISE, 1);
    } else {
        _rise = NULL;
		#ifdef DEBUG
			printf("Removing binding function to rising edge on pin %u\n", gpio_irq.pin);
		#endif
        gpio_irq_set(&gpio_irq, IRQ_RISE, 0);
    }
    //core_util_critical_section_exit();
}

void InterruptIn::fall(Callback<void()> func) {
    //core_util_critical_section_enter();
    if (func) {
        _fall = func;
        gpio_irq_set(&gpio_irq, IRQ_FALL, 1);
    } else {
        _fall = NULL;
        gpio_irq_set(&gpio_irq, IRQ_FALL, 0);
    }
   // core_util_critical_section_exit();
}

void InterruptIn::_irq_handler(uint32_t id, gpio_irq_event event) {
    InterruptIn *handler = (InterruptIn*)id;
    switch (event) {
        case IRQ_RISE: 
            if (handler->_rise) {
                handler->_rise();
            }
            break;
        case IRQ_FALL: 
            if (handler->_fall) {
                handler->_fall(); 
            }
            break;
        case IRQ_NONE: break;
    }
}

void InterruptIn::enable_irq() {
   // core_util_critical_section_enter();
    gpio_irq_enable(&gpio_irq);
    //core_util_critical_section_exit();
}

void InterruptIn::disable_irq() {
   // core_util_critical_section_enter();
    gpio_irq_disable(&gpio_irq);
   // core_util_critical_section_exit();
}

InterruptIn::operator int() {
    // Underlying call is atomic
    return read();
}

