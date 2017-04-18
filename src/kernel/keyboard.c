#include "keyboard.h"

uint8_t init_keyboard()
{
    /* flush input buffer */
    while(inb(0x64) & 0x1) {
        inb(0x60);
    }

    keyboard_command(0xF4);	
    return ERR_OK;
}

void keyboard_command(uint8_t command)
{
    while((inb(0x64) & 0x2));
    outb(0x60, command);
}

static uint8_t one_more_byte = 0;

static uint8_t pressed[128] = {0};

static uint8_t ascii_map[128] =
{
    0,'^','1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0, 'a','s',
    'd','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v',
    'b','n','m',',','.','/',0,0,0,' ',0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

static void key_released(uint8_t scancode)
{
    //kprintf(COL_NOR, "released %x\r\n", scancode);
    pressed[scancode] = 0;
}

static void key_pressed(uint8_t scancode)
{
    pressed[scancode] = 1;
}


uint8_t is_key_pressed(uint8_t scancode)
{
    return pressed[scancode];
}

void process_keyboard(uint8_t scancode)
{
    if (one_more_byte == 0xE0) {
        if(scancode & 0x80) {
            key_released(0xE0);
            key_released(scancode);
        } else {
            key_pressed(scancode);
        }
        one_more_byte = 0;
        return;
    }

    if(scancode == 0xE0) {
        /* "one more byte" scancode */
        one_more_byte = 0xE0;
        key_pressed(scancode);
        return;
    }

    if(scancode & 0x80) {
        key_released(scancode & ~0x80);
    } else {
        key_pressed(scancode);
    }
}

uint8_t scancode_to_ascii(uint8_t scancode)
{
    if(scancode < 127)
        return ascii_map[scancode];
    return ERR_ILLVAL;
}
