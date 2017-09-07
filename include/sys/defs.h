#ifndef _DEFS_H
#define _DEFS_H

#define NULL ((void*)0)

typedef unsigned long  uint64_t;
typedef          long   int64_t;
typedef unsigned int   uint32_t;
typedef          int    int32_t;
typedef unsigned short uint16_t;
typedef          short  int16_t;
typedef unsigned char   uint8_t;
typedef          char    int8_t;

typedef uint64_t size_t;
typedef int64_t ssize_t;

typedef uint64_t off_t;

typedef uint32_t pid_t;

// add //
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
#define __NR_execve     59	
#define __NR_exit       60
#define __NR_wait4      61
#define __NR_getdents   78
#define __NR_getcwd     79
#define __NR_chdir      80
#define __NR_getdents64 217
#define __NR_waitid     247

#endif
