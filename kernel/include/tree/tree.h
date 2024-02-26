#ifndef __TREE_H__
#define __TREE_H__ 1

#include "kernel/sysdefs.h"

typedef int (*compare_fn)(void *value1, void *value2);

typedef struct __binary_tree_node binary_tree_node_t;
typedef struct __binary_tree binary_tree_t;

struct __binary_tree_node {
  binary_tree_node_t *left;
  binary_tree_node_t *right;
  binary_tree_node_t *parent;
  void *value;
  BOOLEAN is_left;
  BOOLEAN is_right;
};

struct __binary_tree {
  binary_tree_node_t *root_node;
  compare_fn compare;
};


void binary_tree_insert(binary_tree_t *tree, binary_tree_node_t *new_node);
binary_tree_node_t *binary_tree_find(binary_tree_t *tree, void *value);
void binary_tree_delete(binary_tree_t *tree, void *value);



#endif // __TREE_H__
