
#include <LPC407x_8x_177x_8x.h>
#include "platform.h"
#include "gpio.h"

//static void (*port_callbacks[5])(int status) = { 0 };

void gpio_toggle(Pin pin) {
	LPC_GPIO_TypeDef* p = GET_PORT(pin);
	uint32_t pin_index = GET_PIN_INDEX(pin);	
	short state = gpio_get(pin);
	if (state == 1){p->CLR |=  (1u << pin_index);}
	else{p->SET |=  (1u << pin_index);}

}

void gpio_set(Pin pin, int value) {
	LPC_GPIO_TypeDef* p = GET_PORT(pin);
	uint32_t pin_index = GET_PIN_INDEX(pin);
		if (value == 1){p->SET |=  (1u << pin_index);}
		else {p->CLR |=  (1u << pin_index);}
}

int gpio_get(Pin pin) {
	LPC_GPIO_TypeDef* p = GET_PORT(pin);
	uint32_t pin_index = GET_PIN_INDEX(pin);
	return (p->PIN >> pin_index) & 0x1;
}

void gpio_set_range(Pin pin_base, int count, int value) {}

//unsigned int gpio_get_range(Pin pin_base, int count) {}

void gpio_set_mode(Pin pin, PinMode mode) { 
	LPC_GPIO_TypeDef* p = GET_PORT(pin);
	uint32_t pin_index = GET_PIN_INDEX(pin);
	uint32_t* pIOCON = GET_IOCON(pin);
	LPC_SC->PCONP |= (1 << 15);  //Power control gpio pheriperals
	
	switch(mode) {
		case Reset:
			break;
		case Input:
			p->DIR &= ~(1 << pin_index); // Set as input.
			*pIOCON &= ~(3UL << 0 ); // Function 0 = GPIO
			break;
		case Output:
			p->DIR |= (1 << pin_index); // Set as output.
			*pIOCON &= ~(3UL << 0 ); // Function 0 = GPIO
			break;
		case PullUp:
			p->DIR |= (1 << pin_index); // Set as output.
			*pIOCON &= ~(3UL << 0 ); // Function 0 = GPIO
			*pIOCON |= (1UL << 4 ); // Enable pull-up resistor.
			gpio_set(pin, 1);
			break;
		case PullDown:
			p->DIR |= (1 << pin_index); // Set as output.
			*pIOCON &= ~(3UL << 0 ); // Function 0 = GPIO
			*pIOCON |= (1UL << 3 ); // Enable pull-down resistor.
			gpio_set(pin, 0);
			break;
	}

}

void gpio_set_trigger(Pin pin, TriggerMode trig) {
	/*struct PORT* p = GET_PORT(pin);
	uint32_t pin_index = GET_PIN_INDEX(pin);
	uint32_t reg = p->INTCFG & ~PRT_INTTYPE_MASK(pin_index);
	
	switch(trig) {
		case None:
			reg |= PRT_INTTYPE(PRT_INTTYPE_DISABLE, pin_index);
			break;
		case Rising:
			reg |= PRT_INTTYPE(PRT_INTTYPE_RISING, pin_index);
			break;
		case Falling:
			reg |= PRT_INTTYPE(PRT_INTTYPE_FALLING, pin_index);
			break;
	}
	
	p->INTCFG = reg;*/
}

void gpio_set_callback(Pin pin, void (*callback)(int status)) {
	/*uint32_t port_index = GET_PORT_INDEX(pin);
	IRQn_Type irq = (IRQn_Type)port_index;
	
	NVIC_DisableIRQ(irq);
	
	// Set up the callbacks.
	port_callbacks[port_index] = callback;
	switch (port_index) {
		case 0: RAM_VECTORS[irq] = &port_isr0; break;
		case 1: RAM_VECTORS[irq] = &port_isr1; break;
		case 2: RAM_VECTORS[irq] = &port_isr2; break;
		case 3: RAM_VECTORS[irq] = &port_isr3; break;
		case 4: RAM_VECTORS[irq] = &port_isr4; break;
	}
	
	// Enable the interrupt
	NVIC_SetPriority(irq, 3);
	NVIC_ClearPendingIRQ(irq);
	NVIC_EnableIRQ(irq);*/
}

void port_isr(int port) {
	/*uint32_t status;
	struct PORT* p = GET_PORT(port);
	void (*callback)(int);
	status = p->INTSTAT;
	
	// Execute the callback with the port state.
	callback = port_callbacks[port];
	if (callback) {
		callback(status);
	}
	
	// Clear interrupt flag
	p->INTSTAT = status;*/
}

void port_isr0(void) {
	port_isr(0);
}

void port_isr1(void) {
	port_isr(1);
}

void port_isr2(void) {
	port_isr(2);
}

void port_isr3(void) {
	port_isr(3);
}

void port_isr4(void) {
	port_isr(4);
}

// *******************************ARM University Program Copyright © ARM Ltd 2014*************************************   
