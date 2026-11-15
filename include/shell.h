#ifndef SHELL_H
#define SHELL_H
#include <stdint.h>
void shell_execute(const char *command);
void shell_prompt(void);
void shell_history_add(const char *command);
void shell_autocomplete(char *command, uint32_t *length, uint32_t capacity);
const char *shell_history_up(void);
const char *shell_history_down(void);
#endif
