#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "gpio_irq_api.h"

#define CHANNEL_NUM    128

static uint32_t channel_ids[CHANNEL_NUM] = {0};
static gpio_irq_handler irq_handler;

static void handle_interrupt_in(uint32_t ch) {
       
    uint32_t id = channel_ids[ch];

    gpio_irq_event event = IRQ_RISE;
	//printf("Received interrupt on pin %u. Handler associated is %u\n",ch,id);

    if (id != 0) {
        irq_handler(id, event);
    }

}

/* IRQ only on PORTA and PORTB */
int gpio_irq(int pin) {
    handle_interrupt_in(pin);
	return 0;
}

int callback_test(void* arg)
{
  uint32_t pin = (uint32_t)arg;
  handle_interrupt_in(pin);
  
  return EXIT_SUCCESS;
}


void gpio_irqB(void) {
    /*handle_interrupt_in(PORTB, 32);*/
}

int gpio_irq_init2(gpio_irq_t *obj, gpio_irq_handler handler, uint32_t id) {

    irq_handler = handler;

    // put us in the interrupt table
    channel_ids[obj->pin] = id;
	obj->pinRf=(void *)obj->pin;

  	// Set edge to RISING
  	libsoc_gpio_set_edge(obj->gp, RISING);
  
  	// Check Edge
  	if (libsoc_gpio_get_edge(obj->gp) != RISING)
  	{
    printf("Failed to set edge to RISING\n");
  	}

	libsoc_gpio_callback_interrupt(obj->gp,&callback_test, obj->pinRf);
 
    return 0;
}

void gpio_irq_free(gpio_irq_t *obj) {
    channel_ids[obj->pin] = 0;
	libsoc_gpio_free(obj->gp);
}

void gpio_irq_set(gpio_irq_t *obj, gpio_irq_event event, uint32_t enable) {
/*
    PORT_Type *port = (PORT_Type *)(PORTA_BASE + 0x1000 * obj->port);

    uint32_t irq_settings = IRQ_DISABLED;

    switch (port->PCR[obj->pin] & PORT_PCR_IRQC_MASK) {
        case IRQ_DISABLED:
            if (enable) {
                irq_settings = (event == IRQ_RISE) ? (IRQ_RAISING_EDGE) : (IRQ_FALLING_EDGE);
            }
            break;

        case IRQ_RAISING_EDGE:
            if (enable) {
                irq_settings = (event == IRQ_RISE) ? (IRQ_RAISING_EDGE) : (IRQ_EITHER_EDGE);
            } else {
                if (event == IRQ_FALL)
                    irq_settings = IRQ_RAISING_EDGE;
            }
            break;

        case IRQ_FALLING_EDGE:
            if (enable) {
                irq_settings = (event == IRQ_FALL) ? (IRQ_FALLING_EDGE) : (IRQ_EITHER_EDGE);
            } else {
                if (event == IRQ_RISE)
                    irq_settings = IRQ_FALLING_EDGE;
            }
            break;

        case IRQ_EITHER_EDGE:
            if (enable) {
                irq_settings = IRQ_EITHER_EDGE;
            } else {
                irq_settings = (event == IRQ_RISE) ? (IRQ_FALLING_EDGE) : (IRQ_RAISING_EDGE);
            }
            break;
    }

    // Interrupt configuration and clear interrupt
    port->PCR[obj->pin] = (port->PCR[obj->pin] & ~PORT_PCR_IRQC_MASK) | irq_settings | PORT_PCR_ISF_MASK;*/
}

void gpio_irq_enable(gpio_irq_t *obj) {
/*
    if (obj->port == PortA) {
        NVIC_EnableIRQ(PORTA_IRQn);
    } else if (obj->port == PortB) {
        NVIC_EnableIRQ(PORTB_IRQn);
    }*/
}

void gpio_irq_disable(gpio_irq_t *obj) {
/*
    if (obj->port == PortA) {
        NVIC_DisableIRQ(PORTA_IRQn);
    } else if (obj->port == PortB) {
        NVIC_DisableIRQ(PORTB_IRQn);
    }
*/
}
