#include <sys/kprintf.h>
#include <stdarg.h>

int strlen(char *str) {
  int i;
  for (i = 0; *str != '\0'; str++)
    i++;
  return i;
}

void *memset(void *dest, int ch, int num_bytes) {
  char *tmp = dest;
  while (num_bytes) {
    *tmp++ = ch;
    num_bytes--;
  }
  return dest;
}

void my_memcpy(void *dest, const void *src, int n) {
  char *d = (char *)dest;
  char *s = (char *)src;
  while (n) {
    *d = *s;
    d++;
    s++;
    n--;
  }
}

void convert(char *a, unsigned long n, int base, int i) {

  int rem = n % base;
  if (n == 0)
    return;

  convert(a, n / base, base, i + 1);
  if (rem < 10) {
    a[i] = rem + 48;

  } else {
    a[i] = rem - 10 + 'A';
  }
}

void display(const char *fmt) {
  static int row = 1;
  static int col = 1;
  char *c;
  static char *temp = (char *)VIDEO_MEM_BEGIN;

  for (c = (char *)fmt; *c; c += 1, temp += CHAR_WIDTH) {
    if (row > 25) {
      my_memcpy((char *)VIDEO_MEM_BEGIN, (char *)VIDEO_MEM_BEGIN + SCREEN_WIDTH, 3840);
      temp -= SCREEN_WIDTH;
      memset(temp, 0, SCREEN_WIDTH);
      row = 25;
    }

    if (*c == '\n') {
      temp += 2 * (80 - col);
      col = 1;
      row++;
      continue;
    }

    if (col != 81) {
      memset(temp, 0, SCREEN_WIDTH - 2 * (col - 1));
      *temp = *c;
      col++;
    } else {
      display("\n");
      col = 1;
      c -= 1;
      temp -= CHAR_WIDTH;
      continue;
    }
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

void reverse(char *a) {
  int i;
  int j = strlen(a);
  char c;

  for (i = 0,j = strlen(a) - 1; i < j; i++, j--) {
    c = a[i];
    a[i] = a[j];
    a[j] = c;
  }

}

void int2char(int num, char *input) {

  int i = 0, r;

  while (num) {
    r = num % 10;
    num = num / 10;
    input[i++] = r + 48;
  }

  reverse(input);
}

void kprintf(const char *fmt, ...)
{
  char buff[1024] = {0};
  char sbuff[1024] = {0};
	va_list args;
	va_start(args, fmt);

  char *s = buff;
  char *st = 0;
  int str_len;
  int int_arg;
  int q = 0;
  unsigned long gg;

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
        q = 0;
        memset(sbuff, 0, sizeof(sbuff));
        int_arg = va_arg(args, int);
        int2char(int_arg, sbuff); 
        str_len = find_length(sbuff);
        while (str_len--) {
          *s++ = sbuff[q++];
        }
        break;
      case 's':
        st = va_arg(args, char *);
        str_len = find_length(st);
        while (str_len--) {
          *s++ = *st++;
        }
        break;
      case 'x':
        q = 0;
        memset(sbuff, 0, sizeof(sbuff));
        int_arg = va_arg(args, int);
  			convert(sbuff, int_arg, 16, 0);
        reverse(sbuff);
        str_len = find_length(sbuff);
        while (str_len--) {
          *s++ = sbuff[q++];
        }
        break;
      case 'p':
        q = 0;
        memset(sbuff, 0, sizeof(sbuff));
        sbuff[0] = '0';
        sbuff[1] = 'x';
        gg = (unsigned long)va_arg(args, unsigned long);
        convert(&sbuff[2], gg, 16, 0);
        reverse(&sbuff[2]);
        str_len = find_length(sbuff);
        while (str_len--) {
          *s++ = sbuff[q++];
        }
        break;
      default:
        display("Unknown format specifier\n");
    }

    fmt++;
  }
  
  va_end(args);

  display(buff);
}
