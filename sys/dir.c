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
      if (node->num_children) {
        strcpy(dir->curr_dentry.d_name, node->child_node[0]->file_name);
      }
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
        if (node->num_children) {
          strcpy(dir->curr_dentry.d_name, node->child_node[0]->file_name);
        }
      }
    } else {

      strncpy(pathname, get_current_running_task()->cwd, len);
      strncpy(pathname + len, (char *)name, strlen((char *)name));
      node = find_node(pathname);
      if (node) {
        dir = (DIR *)vmm_alloc_page();
        dir->node = node;
        if (node->num_children) {
          strcpy(dir->curr_dentry.d_name, node->child_node[0]->file_name);
        }
      }
    }
  }

  return dir;
}

struct dirent *readdir(DIR *dirp) {

  dirent *d_ent = NULL;
  if (dirp && dirp->curr_child < dirp->node->num_children) {

    if (dirp->curr_child == 0) {
      memset(dirp->curr_dentry.d_name, 0, sizeof(dirp->curr_dentry.d_name));
      strcpy(dirp->curr_dentry.d_name, ".");

    } else if (dirp->curr_child == 1) {
      memset(dirp->curr_dentry.d_name, 0, sizeof(dirp->curr_dentry.d_name));
      strcpy(dirp->curr_dentry.d_name, "..");

    } else {
      memset(dirp->curr_dentry.d_name, 0, sizeof(dirp->curr_dentry.d_name));
      strcpy(dirp->curr_dentry.d_name, dirp->node->child_node[dirp->curr_child - 2]->file_name);
    }

    d_ent = &(dirp->curr_dentry);
    dirp->curr_child++;
  }

  return d_ent;
}

int closedir(DIR *dirp) {

  if (dirp) {
    vmm_dealloc_page((uint64_t)dirp);
    return 0;
  }

  return 1;
}

