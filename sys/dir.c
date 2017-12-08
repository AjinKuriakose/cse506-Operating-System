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
  char     name[128];
} DIR;
#endif

DIR *opendir(const char *name) {

  /* TODO : Name is not copied in dir for now.. */
  char pathname[128] = {0};
  DIR *dir = NULL;
  file_t *node = NULL;

  if (name[0] == '.' && name[1] == '.') {
    /* TODO : Handling pending */

  } else if (name[0] == '/') {
    node = find_node((char *)&name[1]);
    if (node) {
      dir = (DIR *)vmm_alloc_page();
      dir->node = node;
    }
  } else {

    int len = strlen(get_current_running_task()->cwd);
    if (name[0] == '.' && name[1] == '/') {

      strncpy(pathname, get_current_running_task()->cwd, len);
      strncpy(pathname + len, (char *)&name[2], strlen((char *)&name[2]));
      node = find_node(pathname);
      if (node) {
        dir = (DIR *)vmm_alloc_page();
        dir->node = node;
      }
    } else {

      strncpy(pathname, get_current_running_task()->cwd, len);
      strncpy(pathname + len, (char *)name, strlen((char *)name));
      dir = (DIR *)vmm_alloc_page();
      node = find_node(pathname);
      if (node) {
        dir = (DIR *)vmm_alloc_page();
        dir->node = node;
      }
    }
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

