#ifndef INTERRUPT_HANDLERS_H
#define INTERRUPT_HANDLERS_H

#include <stdint.h>

struct interrupt_frame
{
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
};

void divide_by_zero_handler(void);
void gpf_handler(void);

void kernel_exception(
    const char *name,
    uint32_t vector,
    struct interrupt_frame *frame,
    uint32_t error_code
);

#endif
