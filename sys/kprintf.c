#include <sys/kprintf.h>
#include <stdarg.h>

int strlen(char *str) {
  int i;
  for (i = 0; *str != '\0'; str++)
    i++;
  return i;
}

void convert(char *a, int n, int base, int i) {

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

void reverse(char *a) {
  int i;
  int j = strlen(a);
  char c;

  for (i=0,j=strlen(a)-1;i<j;i++,j--) {
    c = a[i];
    a[i]=a[j];
    a[j]=c;
  }

}

void int2char(int num, char *input) {

  int i = 0, r;

  while(num) {
    r = num % 10;
    num = num/10;
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
        int_arg = va_arg(args, int);
  			convert(sbuff, int_arg, 16, 0);
        reverse(sbuff);
        str_len = find_length(sbuff);
        while (str_len--) {
          *s++ = sbuff[q++];
        }
        break;
      case 'p':
        /* TODO : Wrong... */
        sbuff[0] = '0';
        sbuff[1] = 'x';
        int_arg = va_arg(args, int);
  			convert(&sbuff[2], int_arg, 16, 0);
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
