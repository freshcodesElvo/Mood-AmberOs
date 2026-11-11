#include "shell.h"
#include "screen.h"
void shell_prompt(void){
	print("root@mudamba:~$ ");
}
//compare 2 strings
static int string_compare(const char *str1, const char *str2){
	while(*str1 && *str2){
		if(*str1 != *str2){
			return 1;
		}	
		str1++;
		str2++;
	}
	return(*str1 =='\0' && *str2 == '\0')?0:1;
}
// Execute shell commands
void shell_execute(const char *command)
{
    if (string_compare(command, "help") == 0)
    {
        print("Available commands:\n");
        print("help  - Show available commands\n");
        print("about - About Mood-Amber OS\n");
        print("clear - Clear the screen\n");
    }
    else if (string_compare(command, "about") == 0)
    {
        print("Mood-Amber OS\n");
        print("A hobby operating system developed from scratch!\n");
    }
    else if (string_compare(command, "clear") == 0)
    {
        clear_screen();
    }
    else if (command[0] == '\0')
    {
        // Ignore empty commands
    }
    else
    {
        print("Unknown command: ");
        print(command);
        print("\n");
    }
}
