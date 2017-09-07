#include <unistd.h>

struct linux_dirent64 {
	unsigned long  d_ino;    /* 64-bit inode number */
	unsigned long  d_off;    /* 64-bit offset to next structure */
	unsigned short d_reclen; /* Size of this dirent */
	unsigned char  d_type;   /* File type */
	char           d_name[]; /* Filename (null-terminated) */
};

enum {
    DT_UNKNOWN = 0,
#define DT_UNKNOWN DT_UNKNOWN
    DT_FIFO = 1,
#define DT_FIFO  DT_FIFO
    DT_CHR = 2,
#define DT_CHR   DT_CHR
    DT_DIR = 4,
#define DT_DIR   DT_DIR
    DT_BLK = 6,
#define DT_BLK   DT_BLK
    DT_REG = 8,
#define DT_REG   DT_REG
    DT_LNK = 10,
#define DT_LNK   DT_LNK
    DT_SOCK = 12,
#define DT_SOCK  DT_SOCK
    DT_WHT = 14
#define DT_WHT   DT_WHT
};

#define	PROT_READ       0x1
#define	PROT_WRITE      0x2
#define	MAP_PRIVATE     0x02
#define MAP_ANONYMOUS   0x20
#define MAP_FAILED      ((void *)-1)

#define O_RDONLY        0x0000
#define O_WRONLY        0x0001
#define O_RDWR          0x0002

#define S_IREAD         0000400
#define S_IWRITE        0000200

#define __NR_read       0
#define __NR_write      1
#define __NR_open       2
#define __NR_close      3
#define __NR_mmap       9
#define __NR_munmap     11
#define __NR_pipe       22
#define __NR_dup2       33
#define __NR_fork       57
#define __NR_exit       60
#define __NR_wait4      61
#define __NR_getdents   78
#define __NR_getcwd     79
#define __NR_chdir      80
#define __NR_getdents64 217
#define __NR_waitid     247

#define EOF             -1

long sys_call(int syscall_number, ...);
char *my_getcwd(char *buf, size_t size);
int  my_open(const char *pathname, int flags);
int  my_close(int fd);
int  my_write(int fd, char *c, int size);
int  my_putchar(int c);
int  my_puts_nonewline(const char *s);
int  my_puts(const char *s);
void handle_cwd();
void handle_ls();

int my_getdents64(int fd, struct linux_dirent64 *dirp, int count) {
  return sys_call(__NR_getdents64, fd, dirp, count);
}

char *my_getcwd(char *buf, size_t size) {
  return (char *)sys_call(__NR_getcwd, buf, size);
}

int my_open(const char *pathname, int flags) {
  return sys_call(__NR_open, pathname, flags);
}

int my_close(int fd) {
  return sys_call(__NR_close, fd);
}

long sys_call(int syscall_number, ...) {
  long ret;
  __asm__(
  "mov    %%rdi,%%rax;"
  "mov    %%rsi,%%rdi;"
  "mov    %%rdx,%%rsi;"
  "mov    %%rcx,%%rdx;"
  "mov    %%r8,%%r10;"
  "mov    %%r9,%%r8;"
  "mov    0x8(%%rsp),%%r9;"
  "syscall;"
  "cmp    $0xfffffffffffff001,%%rax;"
  :"=r"(ret)
  );

  return ret;
}

int my_write(int fd, char *c, int size) {
  return sys_call(__NR_write, fd, c, size);
}

int my_putchar(int c) {
  char ch = c;
  if (my_write(1, &ch, 1) == 1)
    return c;

  return 0;
}

int my_puts_nonewline(const char *s) {
  int ret;
  while (*s) {
    if ((ret = my_putchar(*s)) != *s)
      return EOF;
    s++;
  } 
  return 0;
}

int my_puts(const char *s) {
  if (my_puts_nonewline(s) < 0)
    return EOF;

  return (my_putchar('\n') == '\n') ? 0 : EOF;
}

void handle_ls() {

  char buff[1024] = {0};
  if (my_getcwd(buff, sizeof(buff)) == NULL)
    return;

  int fd;
  int ret;
	int i = 0;
	char buf[1024];
	struct linux_dirent64 *d_ent;
	fd = my_open(buff, O_RDONLY);
  ret = my_getdents64(fd, (struct linux_dirent64 *)buf, 1024);

	while (i < ret) {
		d_ent = (struct linux_dirent64 *) (buf + i);
    if ((d_ent->d_name)[0] != '.')
      my_puts(d_ent->d_name);

		i += d_ent->d_reclen;
	}
}

int main(int argc, char *argv[], char *envp[]) {

  handle_ls();

  return 0;
}

