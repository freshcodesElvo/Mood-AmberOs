
#include <stdint.h>
#include "keyboard.h"
#include "screen.h"

#define KEYBOARD_BUFFER_SIZE 128

// Read a byte from an I/O port
static inline uint8_t inb(uint16_t port)
{
    uint8_t result;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(result)
        : "Nd"(port)
    );

    return result;
}

// Write a byte to an I/O port
static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

// Scancode-to-character lookup table
static const char scancode_table[128] = {
    [0x02] = '1',
    [0x03] = '2',
    [0x04] = '3',
    [0x05] = '4',
    [0x06] = '5',
    [0x07] = '6',
    [0x08] = '7',
    [0x09] = '8',
    [0x0A] = '9',
    [0x0B] = '0',
    [0x0E] = '\b',
    [0x1C] = '\n',
    [0x34] = '.',   
    [0x28] = '\'',

    [0x10] = 'q',
    [0x11] = 'w',
    [0x12] = 'e',
    [0x13] = 'r',
    [0x14] = 't',
    [0x15] = 'y',
    [0x16] = 'u',
    [0x17] = 'i',
    [0x18] = 'o',
    [0x19] = 'p',

    [0x1E] = 'a',
    [0x1F] = 's',
    [0x20] = 'd',
    [0x21] = 'f',
    [0x22] = 'g',
    [0x23] = 'h',
    [0x24] = 'j',
    [0x25] = 'k',
    [0x26] = 'l',

    [0x2C] = 'z',
    [0x2D] = 'x',
    [0x2E] = 'c',
    [0x2F] = 'v',
    [0x30] = 'b',
    [0x31] = 'n',
    [0x32] = 'm',

    [0x39] = ' '

};

// Circular keyboard buffer
static volatile int keyboard_buffer[KEYBOARD_BUFFER_SIZE];

static volatile uint32_t buffer_head = 0;
static volatile uint32_t buffer_tail = 0;
static volatile uint8_t extended_scancode = 0;
static int left_shift_pressed = 0;
static int right_shift_pressed = 0;
// Retrieve a character from the buffer
int keyboard_getchar(void)
{
    // Temporarily disable interrupts while accessing shared data
    uint32_t flags;

    __asm__ volatile (
        "pushfl\n\t"
        "popl %0\n\t"
        "cli"
        : "=r"(flags)
        :
        : "memory"
    );

    if (buffer_head == buffer_tail)
    {
        // Restore previous interrupt state
        if (flags & (1 << 9))
            __asm__ volatile ("sti");

        return -1;
    }

    int character = keyboard_buffer[buffer_tail];

    buffer_tail = (buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;

    // Restore previous interrupt state
    if (flags & (1 << 9))
        __asm__ volatile ("sti");

    return character;
}

// Keyboard IRQ1 handler


// Keyboard IRQ1 handler
void keyboard_handler(void)
{
    uint8_t scancode = inb(0x60);

    // Detect extended scancode prefix
    if (scancode == 0xE0)
    {
        extended_scancode = 1;
        outb(0x20, 0x20);
        return;
    }

    // Handle the next byte after an extended prefix
    if (extended_scancode)
    {
        extended_scancode = 0;

        // Ignore extended key releases
        if (scancode & 0x80)
        {
            outb(0x20, 0x20);
            return;
        }

        int key = 0;

        switch (scancode)
        {
            case 0x48:
                key = KEY_UP;
                break;

            case 0x50:
                key = KEY_DOWN;
                break;

            case 0x4B:
            case 0x4D:
                // Left and right arrows (not implemented yet)
                break;
        }

        if (key != 0)
        {
            uint32_t next_head =
                (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;

            if (next_head != buffer_tail)
            {
                keyboard_buffer[buffer_head] = key;
                buffer_head = next_head;
            }
        }

        outb(0x20, 0x20);
        return;
    }

    // Detect Shift key presses
    if (scancode == 0x2A)
    {
        left_shift_pressed = 1;
        outb(0x20, 0x20);
        return;
    }

    if (scancode == 0x36)
    {
        right_shift_pressed = 1;
        outb(0x20, 0x20);
        return;
    }

    // Detect Shift key releases
    if (scancode == 0xAA)
    {
        left_shift_pressed = 0;
        outb(0x20, 0x20);
        return;
    }

    if (scancode == 0xB6)
    {
        right_shift_pressed = 0;
        outb(0x20, 0x20);
        return;
    }

    // Ignore other ordinary key releases
    if (scancode & 0x80)
    {
        outb(0x20, 0x20);
        return;
    }

    // Translate scancode into a character
    int character;
    int shift_pressed =
        left_shift_pressed || right_shift_pressed;

    if (scancode == 0x0F)
    {
        character = KEY_TAB;
    }
    else
    {
        character = scancode_table[scancode];

        // Convert lowercase letters to uppercase
        if (shift_pressed &&
            character >= 'a' && character <= 'z')
        {
            character = character - ('a' - 'A');
        }

        // Apostrophe key: ' becomes "
        if (scancode == 0x28)
        {
            character = shift_pressed ? '"' : '\'';
        }
    }

    // Add translated character to keyboard buffer
    if (character != 0)
    {
        uint32_t next_head =
            (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;

        if (next_head != buffer_tail)
        {
            keyboard_buffer[buffer_head] = character;
            buffer_head = next_head;
        }
    }

    // Send End Of Interrupt to PIC
    outb(0x20, 0x20);
}




/*
void keyboard_handler(void)
{
    uint8_t scancode = inb(0x60);

    // Detect extended scancode prefix
    if (scancode == 0xE0)
    {
        extended_scancode = 1;
        outb(0x20, 0x20);
        return;
    }

    // Handle the next byte after an extended prefix
    if (extended_scancode)
    {
        extended_scancode = 0;

        // Ignore extended key releases for now
        if (scancode & 0x80)
        {
            outb(0x20, 0x20);
            return;
        }

        // Arrow keys: Set 1 scancodes

	int key =0;
        switch (scancode)
        {
            case 0x48:
                // Up arrow
		key = KEY_UP;
                break;

            case 0x50:
                // Down arrow
		key=KEY_DOWN;
                break;

            case 0x4B:
                // Left arrow
                break;

            case 0x4D:
                // Right arrow
                break;
        }
	if(key !=0){
		uint32_t next_head = (buffer_head+1)%KEYBOARD_BUFFER_SIZE;
		if(next_head !=buffer_tail){
			keyboard_buffer[buffer_head] = key;
			buffer_head = next_head;
	
		}
	}

        outb(0x20, 0x20);
        return;
    }

    // Ignore ordinary key releases
    if (scancode & 0x80)
    {
        outb(0x20, 0x20);
        return;
    }

    // Translate scancode into a character
    int character;
	if(scancode==0x0F){
		character = KEY_TAB;
	}else{
		character=scancode_table[scancode];
	}
    if (character != 0)
    {
        uint32_t next_head =
            (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;

        if (next_head != buffer_tail)
        {
            keyboard_buffer[buffer_head] = character;
            buffer_head = next_head;
        }
    }

    outb(0x20, 0x20);
}
*/
