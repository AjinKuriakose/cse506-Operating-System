#include "../include/stdio.h"

size_t my_strspn(char *s1, char *s2)
{
    size_t ret=0;
    while(*s1 && my_strchr(s2,*s1++))
        ret++;
    return ret;
}

