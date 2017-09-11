#include <sys/kprintf.h>
#include <stdarg.h>

void display(const char *fmt) {
  static char *temp1, *temp2 = (char *)VIDEO_MEM_BEGIN;
  static int char_count = CHAR_WIDTH;
  for (temp1 = (char *)fmt; *temp1; temp1 += 1, temp2 += CHAR_WIDTH) {
    if (*temp1 == '\n') {
      temp2 += (SCREEN_WIDTH - char_count);
      char_count = 0;
    } else {
      *temp2 = *temp1;
    }
    if (!(char_count % SCREEN_WIDTH))
      char_count = 0;

    char_count += CHAR_WIDTH;
  }
}

int find_length(char *str) {

  int len = 0;
  while (*str) {
    len++;
    str++;
  }

  return len;
}

void kprintf(const char *fmt, ...)
{
  char buff[1024] = {0};
	va_list args;
	va_start(args, fmt);

  char *s = buff;
  char *st;
  int str_len;

  while (*fmt) {
    if (*fmt != '%') {
      *s = *fmt;
      s++;
      fmt++;
      continue;
    }
    fmt++; 
    switch (*fmt) {
      case 'c':
        *s++ = (unsigned char)va_arg(args, int);
        break;
      case 'd':
        break;
      case 's':
        st = va_arg(args, char *);
        str_len = find_length(st);
        while (str_len--) {
          *s++ = *st++;
        }
        break;
      case 'x':
        break;
      case 'p':
        break;
      default:
        display("Unknown format specifier\n");
    }

    fmt++;
  }
  
  va_end(args);

  display(buff);
}
