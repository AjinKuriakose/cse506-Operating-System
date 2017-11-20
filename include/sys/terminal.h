#ifndef _TERMINAL_H
#define _TERMINAL_H

#include <sys/defs.h> 

#define TERMINAL_BUFFER_SIZE      400
#define TERMINAL_BUFFER_NOT_READY 0
#define TERMINAL_BUFFER_READY     1

typedef struct terminal_t {
  uint8_t   buffer_ready;
  char      buffer[TERMINAL_BUFFER_SIZE];
  uint16_t  buffer_offset;
} terminal_t;

void init_terminal();
void terminal_display(const char *fmt);
int read_from_terminal();
int write_to_terminal();
void handle_keyboard_input(unsigned char glyph, int flags);

#endif
