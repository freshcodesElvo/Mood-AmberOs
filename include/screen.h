#ifndef SCREEN_H
#define SCREEN_H
#include <stdint.h>
void clear_screen(void);
void print_char(char character);
void print(const char *message);
void print_hex(uint32_t value);

#endif
