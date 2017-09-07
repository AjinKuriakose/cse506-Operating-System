#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct linux_dirent64 {
	unsigned long  d_ino;    /* 64-bit inode number */
	unsigned long  d_off;    /* 64-bit offset to next structure */
	unsigned short d_reclen; /* Size of this dirent */
	unsigned char  d_type;   /* File type */
	char           d_name[]; /* Filename (null-terminated) */
};

int  puts_nonewline(const char *s);
void handle_cwd();
void handle_ls();

int getdents64(int fd, struct linux_dirent64 *dirp, int count) {
  return syscall(__NR_getdents64, fd, dirp, count);
}

int puts_nonewline(const char *s) {
  int ret;
  while (*s) {
    if ((ret = putchar(*s)) != *s)
      return EOF;
    s++;
  } 
  return 0;
}

void handle_ls() {
	char buff[1024] = {0};
	if (getcwd(buff, sizeof(buff)) == NULL)
		return;

	int fd;
	int ret;
	int i = 0;
	char buf[1024];
	struct linux_dirent64 *d_ent;
	fd = open(buff, O_RDONLY);
	ret = getdents64(fd, (struct linux_dirent64 *)buf, 1024);

	while (i < ret) {
		d_ent = (struct linux_dirent64 *) (buf + i);
		if ((d_ent->d_name)[0] != '.')
			puts(d_ent->d_name);

		i += d_ent->d_reclen;
	}
}

int main(int argc, char *argv[], char *envp[]) {

	handle_ls();

	return 0;
}

