#include "interrupts.h"
#include <stdint.h>

//write 1 byte to i/o port
static inline void outb(uint16_t port, uint8_t value){
	__asm__ volatile(
		"outb %0, %1"
		:
		:"a"(value), "Nd"(port)
	);
}
static inline uint8_t inb(uint16_t port){
	uint8_t result;
	__asm__ volatile (
		"inb %1, %0"
		: "=a"(result)
		: "Nd"(port)
	);return result;
}
//small delay for for pic communication
static inline void io_wait(void){
	outb(0x08,0);	
}
void interrupts_init(void){
	//start pic initialization
	outb(0x20, 0x11);
	io_wait();

	outb(0xA0, 0x11);
	io_wait();

	//remap mastter pic to vectors 0x20 = 0x27
	outb(0x21, 0x20);
	io_wait();

	//remap slave pic to vectors 0x28 - 0x2F
	outb(0xA1, 0x28);
	io_wait();

	//tell master pic that slave is connected  to irq2
	outb(0x21, 0x04);
	io_wait();

	//tell slave pic  its cascade identity
	outb(0xA1, 0x02);
	io_wait();

	//set bot pics to  8086 mode
	outb(0x21,0x01);
	io_wait();

	outb(0xA1, 0x01);
	io_wait();
	//mask all hardware interrupts for now
	outb(0x21,0xFF);
	outb(0xA1, 0xFF);
	
}
void keyboard_irq_enable(void){
	uint8_t mask = inb(0x21);
	//clear bit 1to unmask IRQ1 (keyboard)
	mask &= ~(1<<1);
	outb(0x21, mask);
}
