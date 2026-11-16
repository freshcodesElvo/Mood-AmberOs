#include "shell.h"
#include "screen.h"
#include <stdint.h>
#define HISTORY_SIZE 10
#define COMMAND_BUFFER_SIZE 128

static char command_history[HISTORY_SIZE][COMMAND_BUFFER_SIZE];
static  uint32_t history_count = 0;
static uint32_t history_next = 0;
static int  history_position = -1;
static char current_directory[128] = "/";

void shell_autocomplete(char *command, uint32_t *length, uint32_t capacity){
	static const char *commands[]={
		"help",
		"about",
		"clear",
		"echo",
	 	"pwd",
	       	"cd",
	};
	const char *match = 0;
	uint32_t matches = 0;

	//find commands matching what user types
	for(uint32_t i=0;i<6;i++){
		uint32_t j=0;
		int matches_prefix=1;
		while(j<*length){
			if(commands[i][j]=='\0' || command[j] != commands[i][j]){
				matches_prefix = 0;
				break;
			}
			j++;
		}
		if(matches_prefix){
			match=commands[i];
			matches++;
		}
	}
	//complete only when there is one match exactly
	if(matches==1){
		while(match[*length] !='\0' && *length<capacity-1){
			command[*length]=match[*length];
			print_char(match[*length]);

			(*length)++;

		}
		command[*length]='\0';
	}
}
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

static const char *skip_spaces(const char *text){
	while(*text == ' '){
		text++;
	}
	return text;
}

//change dir
static void change_directory(const char *path){
	//move to the root dir
	if(string_compare(path, "/")==0){
		current_directory[0]='/';
		current_directory[1]='\0';
		return;
	}
	//move one dir level up
	if (string_compare(path, "..") == 0)
{
    uint32_t length = 0;

    // Find the string length
    while (current_directory[length] != '\0')
    {
        length++;
    }

    // Don't go above root
    if (length <= 1)
    {
        return;
    }

    // Remove trailing directory characters
    while (length > 1 && current_directory[length - 1] != '/')
    {
        length--;
    }

    // Remove the final slash, unless it is root
    if (length > 1)
    {
        length--;
    }

    // Terminate the updated path
    current_directory[length] = '\0';

    return;
}
	//return to root if path is empty
	if(path[0]=='\0'){
		return;
	}
	//move  into a dir
	uint32_t current_length = 0;
	while(current_directory[current_length] !='\0'){
		current_length++;
		
	}
	uint32_t path_length=0;
	while(path[path_length] !='\0'){
		path_length++;
	}
	//add  a slash if we are not already at root
	if(current_length>1){
		if(current_length+1 >= sizeof(current_directory)){
			return;
		}
		current_directory[current_length++]='/';
		
	}
	//check buffer capacty
	if(current_length + path_length >=sizeof(current_directory)){
		return;
	}
	//copy the dir name
	for(uint32_t i = 0; i<path_length; i++){
		current_directory[current_length +i] =path[i];
	}
	current_directory[current_length + path_length] = '\0';
	
}


// Execute shell commands


void shell_execute(const char *command)
{
    // Skip leading spaces
    command = skip_spaces(command);

    char command_name[32];
    uint32_t i = 0;

    // Copy the command name
    while (command[i] != '\0' &&
           command[i] != ' ' &&
           i < sizeof(command_name) - 1)
    {
        command_name[i] = command[i];
        i++;
    }

    command_name[i] = '\0';

    // Locate the arguments
    const char *args = skip_spaces(command + i);

    if (string_compare(command_name, "help") == 0)
    {
        print("Available commands:\n");
        print("help  - Show available commands\n");
        print("about - About Mood-Amber OS\n");
        print("clear - Clear the screen\n");
        print("echo  - Display text\n");
	print("pwd - print current working directory");
	print("cd - change working director\n");
    }
    else if (string_compare(command_name, "about") == 0)
    {
        print("Mood-Amber OS\n");
        print("A hobby operating system developed from scratch!\n");
    }
    else if(string_compare(command_name, "pwd") == 0){
	print(current_directory);
	print("\n");	
    }
    else if(string_compare(command_name, "cd")==0){
	change_directory(args);	
    
    }
    else if (string_compare(command_name, "clear") == 0)
    {
        clear_screen();
    }
    else if (string_compare(command_name, "echo") == 0)
    {
        print(args);
        print("\n");
    }
    else if (*command_name == '\0')
    {
        // Ignore empty commands
    }
    else
    {
        print("Unknown command: ");
        print(command_name);
        print("\n");
    }
}





void shell_history_add(const char *command){
	//no saving empty commands
	if(command[0] == '\0')
		return;
	uint32_t i = 0;
	
	//copy commands into the nxt history slot
	while(command[i] != '\0' && i<COMMAND_BUFFER_SIZE -1){
		command_history[history_next][i]=command[i];
		i++;
	}
	command_history[history_next][i] = '\0';
	
	//advance circular history postion
	history_next = (history_next+1)%HISTORY_SIZE;

	if(history_count<HISTORY_SIZE)
		history_count++;
	//reset the navigation postion
	history_position = -1;
	
	
}

const char *shell_history_up(void){ 
	if(history_count ==0)
		return 0;
	
	//move towards older commands
	if(history_position<(int)history_count - 1)
		history_position++;
	uint32_t index =  (history_next + HISTORY_SIZE -1 - history_position) % HISTORY_SIZE;
	return command_history[index];


}
const char *shell_history_down(void){
	if(history_position == -1){return 0;}
	if(history_position == 0){history_position =-1;return "";}
	history_position --;
	uint32_t index= (history_next + HISTORY_SIZE-1 - history_position)%HISTORY_SIZE;
	return command_history[index];
}
