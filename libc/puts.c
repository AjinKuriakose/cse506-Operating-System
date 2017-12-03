#include <unistd.h>
#include <string.h>

int puts(char *s)
{
  return write(1, s, strlen(s));
//  for( ; *s; ++s) if (putchar(*s) != *s) return EOF;
 // return (putchar('\n') == '\n') ? 0 : EOF;
}
