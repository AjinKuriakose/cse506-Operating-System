#include "../include/stdio.h"

size_t my_strcspn(char *s1, char *s2)
{
    size_t ret=0;
    while(*s1)
        if(my_strchr(s2,*s1))
            return ret;
        else
            s1++,ret++;
    return ret;
}

