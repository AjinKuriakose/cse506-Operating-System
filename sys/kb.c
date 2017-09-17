#include <sys/kb.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/utils.h>

/*
 * scancode reference: http://www.osdever.net/bkerndev/Docs/keyboard.htm
 *
 */

unsigned char scancode_arr[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
 '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};		

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ __volatile__( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

void key_handler() {
  unsigned char scode;
  scode = inb(0x60);
  if (scode & 0x80) {

    kprintf("Shift, ctrl, .. pressed\n");
  } else {

    kprintf("%c---\n", scancode_arr[scode]);
    display_glyph(scancode_arr[scode], 0);
  }
}

void display_glyph(unsigned char glyph, int is_ctrl_char) {
  unsigned char sbuff[6] = {0};
  unsigned char *c;
  int i = 0;
  char *temp = (char *)VIDEO_MEM_BEGIN + SCREEN_WIDTH * SCREEN_HEIGHT - 10;
  memset(temp, 0, 10);
  
  sbuff[0] = '[';
  if (is_ctrl_char)
    sbuff[1] = '^';
  else
    sbuff[1] = ' ';
  sbuff[2] = glyph;
  sbuff[3] = ' ';
  sbuff[4] = ']';

  for (c = sbuff; i < 5; c += 1, i++, temp += CHAR_WIDTH) {
      *temp = *c;
  }
}

