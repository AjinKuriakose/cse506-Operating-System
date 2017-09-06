#include "../include/stdio.h"	
#include "../include/sys/defs.h"	

char *my_strstr(char *str1, char *str2) {
  char *s1 = str1, *s2 = str2, *ret = NULL;
  int match_len = 0;

  if (!str1 || !str2 || !*str1 || !*str2)
    return ret;

  while (*s1 && *s2) {
    if (my_strlen(s1) < my_strlen(s2))
      break;

    if (*s1 == *s2) {
      match_len++;
      if (!ret)
        ret = s1;
      s2++;
    } else {
      match_len = 0;
      s2 = str2;
      ret = NULL;
    }
    s1++;
  }

  if (match_len != my_strlen(str2))
    ret = NULL;

  return ret;
}

