#include <unistd.h>
#include <string.h>

int puts(char *s)
{
  return write(1, s, strlen(s));
}
