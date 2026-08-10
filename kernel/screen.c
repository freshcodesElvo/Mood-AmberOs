#include "screen.h"
#define VIDEO_MEMORY 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define WHITE_ON_BLACK 0x0F
static char *video_memory = (char *)VIDEO_MEMORY;
static int cursor_row = 0;
static int cursor_column =  0;
void clear_screen(void){
	for(int row = 0; row < SCREEN_HEIGHT; row++){
		for(int column = 0; column < SCREEN_WIDTH; column++){
			int position = (row * SCREEN_WIDTH + column) * 2;

			video_memory[position] = ' ';
			video_memory[position + 1] = WHITE_ON_BLACK;	
		}
	}
	cursor_row = 0;
	cursor_column = 0;

}

void print_char(char character){
	if(character == '\n'){
		cursor_column = 0;
		cursor_row++;
		return;
	}
	
	int position =  (cursor_row * SCREEN_WIDTH + cursor_column) * 2;
	
	video_memory[position] = character;
	video_memory[position + 1] = WHITE_ON_BLACK;
	
	cursor_column++;
	
	if(cursor_column >= SCREEN_WIDTH){
		cursor_column = 0;
		cursor_row++;
		
	}
}
void print(const char *message){
	while(*message !='\0'){
		print_char(*message);
		message++;
	}
}
