#ifndef _DIR_H
#define _DIR_H

#define FILE_TYPE_DIR   0x35
#define FILE_TYPE_FILE  0x30

#define MAX_CHILDREN    256

typedef struct file_t {
  char      file_name[32];
  uint8_t   file_type;
  uint64_t  file_size;
  uint16_t  num_children;
  struct file_t *parent_node;
  struct file_t *child_node[MAX_CHILDREN];
} file_t;

file_t *create_child_node(file_t *parent_node, char *file_name, uint64_t file_size, uint8_t file_type);

#endif
