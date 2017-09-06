#ifndef _STDIO_H
#define _STDIO_H

static const int EOF = -1;
typedef unsigned long	size_t;

int putchar(int c);
int puts(const char *s);
int printf(const char *format, ...);

char *gets(char *s);

//new functions : manu

char *m_strcpy(char *dest, char *src);
char *m_strncpy(char *dest, char *src, int num_bytes);
char *my_strtok_r(char *str, char *delim, char **nextp);
char *my_strstr(char *str1, char *str2);
char *my_strchr(char *, int);
void *my_memset(void *dest, int ch, size_t num_bytes);

int  my_strlen(char *str);
size_t my_strspn(char *s1, char *s2);
size_t my_strcspn(char *s1, char *s2);
char *my_strchr(char *s, int c);
int my_strncmp (const char *f_str, const char *s_str, size_t n);
int my_strcmp (const char *f_str, const char *s_str); 

#endif
