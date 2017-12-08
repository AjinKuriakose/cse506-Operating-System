#include <sys/dirent.h>
#include <sys/syscall.h>
#include <sys/task.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/vmm.h>
#include <sys/gdt.h>
#include <sys/utils.h>
#include <sys/terminal.h>
#include <sys/tarfs.h>

#if 0
typedef struct dirent {
 uint64_t inode_no;
 char d_name[NAME_MAX+1];
} dirent;

typedef struct file_t {
  char      file_name[32];
  uint8_t   file_type;
  uint64_t  file_size;
  uint64_t  file_begin;
  uint64_t  file_end;
  uint64_t  file_cursor;
  uint16_t  num_children;
  struct file_t *parent_node;
  struct file_t *child_node[MAX_CHILDREN];
} file_t;

typedef struct DIR { 
  //int      fd;
  file_t   *node;
  //uint64_t current;
  //dirent_t current_dentry;
  char     name[32];
} DIR;
#endif

DIR *opendir(const char *name) {

  DIR *dir = NULL;
  if (name[0] == '/') {

  } else {


  }

  return dir;
}

struct dirent *readdir(DIR *dirp) {

  dirent *d_ent = NULL;

  return d_ent;
}

int closedir(DIR *dirp) {

  return 0;
}

