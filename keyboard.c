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

static uint8_t keys[128] = 
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

uint8_t scancode_to_ascii(uint8_t scancode)
{
    if(scancode < 127)
        return keys[scancode];
    return ERR_ILLVAL;
}
