#include "shell.h"
#include "screen.h"
#include <stdint.h>
#include "ramfs.h"
#define HISTORY_SIZE 10
#define COMMAND_BUFFER_SIZE 128

static char command_history[HISTORY_SIZE][COMMAND_BUFFER_SIZE];
static uint32_t history_count = 0;
static uint32_t history_next = 0;
static int  history_position = -1;
//static char current_directory[128] = "/";
static ramfs_node_t *current_directory = 0;

void shell_init(void){
	current_directory=ramfs_root();
}
void shell_autocomplete(char *command, uint32_t *length, uint32_t capacity){
	static const char *commands[]={
		"help",
		"about",
		"clear",
		"echo",
	 	"pwd",
	       	"cd",
		"cat", "append", "mkdir", "write"
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
static  void change_directory(const char *path){
	if(current_directory==0){
		print("file syst not initialized !! \n");
		return;
	}
	//return to root
	if(string_compare(path, "/")==0){
		current_directory = ramfs_root();
		return;
	}
	//move to parent dir
	if(string_compare(path, "..")==0){
		if(current_directory->parent !=0){
			current_directory=current_directory->parent;
		}
		return;
	}
	//ignore empty paths
	if(path[0]=='\0'){
		return;
	}
	//search for the requested dir
	ramfs_node_t  *target = ramfs_find(current_directory,path);
	if(target==0){
		print("dir not found\n");
		return;
	}
	if(target->type !=RAMFS_DIRECTORY){
		print("not a dir\n");
		return;
	}
	//change the current working dir
	current_directory=target;
}



/*

*/





//implement ls command
static void list_directory(void){
	if(current_directory==0){
		print("no current  directory!\n");
		return;
	}
	ramfs_node_t *current=ramfs_first_child(current_directory);
	while(current !=0){
		print(current->name);
		if(current->type == RAMFS_DIRECTORY){
			print("\n");
			
		}
		print("\n");
		current = current->next_sibling;
	}

}



//cat files
static void cat_file(const char *filename){
	ramfs_node_t *file = ramfs_find(current_directory, filename);
	if(file==0){
		print("file not found\n");
		return;
	}
	if(file->type != RAMFS_FILE){
		print("Not a file\n");
		return;
	}
	char buffer[RAMFS_DATA_MAX+1];
	int bytes_read = ramfs_read_file(file,buffer,sizeof(buffer));
	if(bytes_read <0){
		print("failed to red file\n");
		return;
	}
	print(buffer);
	print("\n");
}
//write file
static void write_file(const char *args){
	//skip leading spaces
	while(*args == ' '){
		args++;
	}
	//extract filename
	char filename[RAMFS_NAME_MAX];
	uint32_t i =0;
	
	while(*args != '\0' && *args != ' ' && i<RAMFS_NAME_MAX-1){
		filename[i] = *args;
		i++;
		args++;
	
	}
	filename[i] = '\0';

	//skip spaces btwn filename and content
	while(*args==' '){
		args++;
	}
	//check for opening quotation mark
	if(*args != '"'){
		print("Usage: write <filename> \"content\"\n");
		return;
	}
	//move past opening quotation mark
	args++;

	//extract content
	char content[RAMFS_DATA_MAX];
	uint32_t j=0;
	
	while(*args != '\0' && *args !='"' && j<RAMFS_DATA_MAX-1){
		content[j]=*args;
		j++;
		args++;
		
	}
	content[j]='\0';
	
	//check for closing quotation mark
	if(*args != '"'){
		print("Missing closing quotation mark!\n");
		return;
	}
	//find the file in the current dir
	ramfs_node_t *file=ramfs_find(current_directory,filename);
	if(file ==0){
		print("file not found\n");
		return;
	}

	//make  sure the target is a  file
	if(file->type !=RAMFS_FILE){
		print("not a file\n");
		return;
	}
	//write the content
	int bytes_written = ramfs_write_file(file,content);
	if(bytes_written<0){
		print("failed to write the file\n");
		return;
	}
	print("succesfully wrote ");
	print_hex((uint32_t)bytes_written);
	print(" bytes to ");
	print(filename);
	print("\n");
}

static void append_file(const char *args)
{
    // Skip leading spaces
    while (*args == ' ')
    {
        args++;
    }

    // Extract filename
    char filename[RAMFS_NAME_MAX];
    uint32_t i = 0;

    while (*args != '\0' && *args != ' ' &&
           i < RAMFS_NAME_MAX - 1)
    {
        filename[i] = *args;
        i++;
        args++;
    }

    filename[i] = '\0';

    if (i == 0)
    {
        print("Usage: append <filename> \"content\"\n");
        return;
    }

    // Skip spaces between filename and content
    while (*args == ' ')
    {
        args++;
    }

    // Check opening quotation mark
    if (*args != '"')
    {
        print("Usage: append <filename> \"content\"\n");
        return;
    }

    args++;

    // Extract content
    char content[RAMFS_DATA_MAX];
    uint32_t j = 0;

    while (*args != '\0' && *args != '"' &&
           j < RAMFS_DATA_MAX - 1)
    {
        content[j] = *args;
        j++;
        args++;
    }

    content[j] = '\0';

    // Check closing quotation mark
    if (*args != '"')
    {
        print("Missing closing quotation mark!\n");
        return;
    }

    // Find the target file
    ramfs_node_t *file =
        ramfs_find(current_directory, filename);

    if (file == 0)
    {
        print("File not found!\n");
        return;
    }

    if (file->type != RAMFS_FILE)
    {
        print("Not a file!\n");
        return;
    }

    // Append the content
    int bytes_written = ramfs_append_file(file, content);

    if (bytes_written < 0)
    {
        print("Failed to append to file. Check available space!\n");
        return;
    }

    print("Successfully appended ");
    print_hex((uint32_t)bytes_written);
    print(" bytes to ");
    print(filename);
    print("\n");
}

//remove file
static void remove_file(const char *args){
	//skip leading spaces
	while(*args ==' '){
		args++;
	}
	//check whether a FS was provided
	if(*args == '\0'){
		print("usage: rm <filename>\n");
		return;
	}
	//extract file name
	char filename[RAMFS_NAME_MAX];
	uint32_t i =0;
	while(*args !='\0' && *args !=' ' && i< RAMFS_NAME_MAX-1){
		filename[i]=*args;
		i++;
		args++;
		
	}	

	filename[i]='\0';
	//reject names that  exeeds the supported length
	if(*args !='\0' && *args!=' '){
		print("file name too long\n");
		return;
	}
	//remove the file from the current dir
	int result = ramfs_remove_file(current_directory, filename);
	if(result ==0){
		print("file deleted succesfully\n");
	}else{
		print("failed to delete file : the file may not exist or might be a directorv.\n");
	}
}



//remove dir
static void remove_directory(const char *args){
	//skip leading spaces
	while(*args == ' '){
		args++;
	}
	if(*args =='\0'){
		print("usage rmdir <directory>\n");
		return;
		
	}
	//extract dir name
	char dirname[RAMFS_NAME_MAX];
	uint32_t i =0;
	while(*args != '\0' && *args != ' ' && 	 i< RAMFS_NAME_MAX-1){
		dirname[i]= *args;
		i++;
		args++;
	}
	dirname[i]='\0';
	//check for an exessively long name
	if(*args !='\0' && *args !=' '){
		print("directory name too long\n");
		return;
	}
	//remove the dir
	int result = ramfs_remove_directory(current_directory, dirname);
	if(result ==0){
		print("directory removed succesfully\n");
	}else{
		print("failed to remove directory. it may not exist, be non empty or not be a directory\n");
	}
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
	print("ls - list directory contents\n");
	print("mkdir - create a directory\n");
	print("Touch - create an emptyor new file\n");
	print("cat - display file contents\n");
	print("write - Write content into a file\n");
	print("append - Append content into a file\n");
	print("rm - delete a file");
	print("rmdir - remove an empty directory\n");

    }
    else if (string_compare(command_name, "about") == 0)
    {
        print("Mood-Amber OS\n");
        print("A hobby operating system developed from scratch!\n");
    }
    else if(string_compare(command_name, "pwd") == 0){
	if(current_directory == ramfs_root()){
		print("/");
		
		
	}else{
		print(current_directory->name);
	}
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
    else if(string_compare(command_name, "ls")==0){
	list_directory();	
    }
    else if(string_compare(command_name, "mkdir")==0){
	if(args[0]=='\0'){
		print("Usage: mkdir <directory_name>\n");
		return;
	}
	ramfs_node_t *directory=ramfs_create_directory(current_directory, args);
	if(directory !=0){
		print("Dir created!\n");
		print(directory->name);
		print("\n");
	}else{
		print("Failed to create a directory\n");
	}
		
    }
    else if(string_compare(command_name, "touch")==0){
	if(args[0]=='\0'){
		print("Usage: touch <filename>\n");
		return;
	}	
	ramfs_node_t *file =ramfs_create_file(current_directory, args);
	if(file !=0){
		print("File created \n");
		print(file->name);
		print("\n");
		
	}else{
		print("failed to create file\n");
	}
    }
    else if(string_compare(command_name, "cat")==0){
		if(args[0]=='\0'){
			print("Usage: cat <filename>\n");
			return;
		}
		cat_file(args);
    }
    else if(string_compare(command_name, "write")==0){
	write_file(args);	
    }
   /* else if(string_compare(command_name, "append")==0){
   	append_file(args);
    }*/

    else if(string_compare(command_name, "append")==0){
   	 print("DEBUG: append arguments = [");
   	 print(args);
   	 print("]\n");
   	 append_file(args);
    }
    else if(string_compare(command_name, "rm")==0){
	remove_file(args);
    }
    else if(string_compare(command_name, "rmdir")==0){
    	remove_directory(args);
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
