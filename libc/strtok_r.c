#include "../include/stdio.h"
#include "../include/sys/defs.h"
char *my_strtok_r(char *str, char *delim, char **nextp) {
  char *ret;
  if (str == NULL)
    str = *nextp;

  str += my_strspn(str, delim);
  if (*str == '\0')
    return NULL;

  ret = str;
  str += my_strcspn(str, delim);
  if (*str)
    *str++ = '\0';

  *nextp = str;
  return ret;
}

