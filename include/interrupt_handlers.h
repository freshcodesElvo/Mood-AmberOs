#ifndef INTERRUPT_HANDLERS_H
#define INTERRUPT_HANDLERS_H

#include <stdint.h>


struct interrupt_frame{
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	
};
void divide_by_zero_handler(void);
void divide_by_zero_handler_c(struct interrupt_frame *frame);

#endif
