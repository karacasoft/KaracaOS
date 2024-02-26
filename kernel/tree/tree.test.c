#include "tree/tree.h"
#include <stdio.h>

int compare_function(void *v1, void *v2) {
  int int_v1 = *((int *)(v1));
  int int_v2 = *((int *)(v2));
  return int_v1 - int_v2;
}

void create_binary_tree_node(int *value, binary_tree_node_t *node) {
  node->value = (void *)value;
  node->left = 0;
  node->right = 0;
  node->is_left = 0;
  node->is_right = 0;
}

void print_node(binary_tree_node_t *node, int space) {
  if(node == NULL) {
    return;
  }

  space += 5;
  print_node(node->right, space);

  printf("\n");
  for(int i= 5; i < space; i++) {
    printf(" ");
  }
  printf("%d\n", *((int *)node->value));

  print_node(node->left, space);
}

void visualize_tree(binary_tree_t *tree) {
  print_node(tree->root_node, 0);
}

int main() {
  binary_tree_t tree;
  tree.compare = compare_function;
  int v1 = 20;
  int v2 = 50;
  int v3 = 70;
  int v4 = 40;
  int v5 = 60;
  int v6 = 80;
  int v7 = 30;
  binary_tree_node_t node1;
  binary_tree_node_t node2;
  binary_tree_node_t node3;
  binary_tree_node_t node4;
  binary_tree_node_t node5;
  binary_tree_node_t node6;
  binary_tree_node_t node7;
  int *values[7] = {&v1, &v2, &v3, &v4, &v5, &v6, &v7};

  create_binary_tree_node(&v1, &node1);
  create_binary_tree_node(&v2, &node2);
  create_binary_tree_node(&v3, &node3);
  create_binary_tree_node(&v4, &node4);
  create_binary_tree_node(&v5, &node5);
  create_binary_tree_node(&v6, &node6);
  create_binary_tree_node(&v7, &node7);

  binary_tree_insert(&tree, &node1);
  binary_tree_insert(&tree, &node2);
  binary_tree_insert(&tree, &node3);
  binary_tree_insert(&tree, &node4);
  binary_tree_insert(&tree, &node5);
  binary_tree_insert(&tree, &node6);
  binary_tree_insert(&tree, &node7);

  // visualize_tree(&tree);

  binary_tree_node_t *node = binary_tree_find(&tree, &v1);
  if (*((int *)node->value) != v1) {
    printf("Error in find v1");
    return 1;
  }
  node = binary_tree_find(&tree, &v5);
  if (*((int *)node->value) != v5) {
    printf("Error in find v5\n");
    return 5;
  }

  node = binary_tree_find(&tree, &v7);
  if (*((int *)node->value) != v7) {
    printf("Error in find v7\n");
    return 5;
  }

  binary_tree_delete(&tree, &v3);
  node = binary_tree_find(&tree, &v3);
  if (node != 0) {
    printf("Error delete\n");
    return 10;
  }

  visualize_tree(&tree);
  int i;
  for (i = 0; i < 7; i++) {
    if (i == 2)
      continue;
    node = binary_tree_find(&tree, values[i]);
    if (node == 0 || (*((int *)node->value) != *(values[i]))) {
      printf("Error in value %d\n", i);
      return 12;
    }
  }

  return 0;
}
