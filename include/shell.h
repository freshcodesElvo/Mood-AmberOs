#ifndef SHELL_H
#define SHELL_H
void shell_execute(const char *command);
void shell_prompt(void);
void shell_history_add(const char *command);
const char *shell_history_up(void);
const char *shell_history_down(void);
#endif
