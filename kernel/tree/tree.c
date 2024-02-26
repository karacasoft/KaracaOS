#include <tree/tree.h>

void __binary_tree_node_insert(binary_tree_node_t *node,
                               binary_tree_node_t *new_node,
                               compare_fn compare);
binary_tree_node_t *__binary_tree_node_find(binary_tree_node_t *node,
                                            void *value, compare_fn compare);
void __binary_tree_node_delete(binary_tree_node_t *node, void *value,
                               compare_fn compare);

void binary_tree_insert(binary_tree_t *tree, binary_tree_node_t *new_node) {
  if (tree->root_node == NULL) {
    tree->root_node = new_node;
  } else {
    __binary_tree_node_insert(tree->root_node, new_node, tree->compare);
  }
}

binary_tree_node_t *binary_tree_find(binary_tree_t *tree, void *value) {
  return __binary_tree_node_find(tree->root_node, value, tree->compare);
}

void binary_tree_delete(binary_tree_t *tree, void *value) {
  binary_tree_node_t *node = binary_tree_find(tree, value);
  if (node == NULL) {
    return;
  }
  __binary_tree_node_delete(tree->root_node, value, tree->compare);
}

void __binary_tree_node_delete(binary_tree_node_t *node, void *value,
                               compare_fn compare) {
  if (node == NULL) {
    return;
  }
  int compare_res = (*compare)(node->value, value);
  if (compare_res == 0) {
    binary_tree_node_t *parent = node->parent;
    if (node->left == NULL && node->right == NULL) {
      if (node->is_left) {
        parent->left = NULL;
      } else if (node->is_right) {
        parent->right = NULL;
      }
    } else if (node->left == NULL) {
      if (node->is_left) {
        parent->left = node->right;
      } else if (node->is_right) {
        parent->right = node->right;
      }
    } else if(node->right == NULL) {
      if(node->is_left) {
        parent->left = node->left;
      } else if(node->is_right) {
        parent->right = node->left;
      }
    } else {
      binary_tree_node_t *current_node = node->right;
      while(current_node->left) {
        current_node = current_node->left;
      }
      node->value = current_node->value;
      // current_node->parent->left = NULL;
      node->right = current_node->right;
    }
  } else if (compare_res < 0) {
    __binary_tree_node_delete(node->left, value, compare);
  } else if (compare_res > 0) {
    __binary_tree_node_delete(node->right, value, compare);
  }
}

void __binary_tree_node_insert(binary_tree_node_t *node,
                               binary_tree_node_t *new_node,
                               compare_fn compare) {
  int compare_res = (*compare)(node->value, new_node->value);
  if (compare_res <= 0) {
    if (node->left == NULL) {
      node->left = new_node;
      new_node->parent = node;
      new_node->is_left = TRUE;
      new_node->is_right = FALSE;
    } else {
      __binary_tree_node_insert(node->left, new_node, compare);
    }
  } else {
    if (node->right == NULL) {
      node->right = new_node;
      new_node->parent = node;
      new_node->is_left = FALSE;
      new_node->is_right = TRUE;
    } else {
      __binary_tree_node_insert(node->right, new_node, compare);
    }
  }
}

binary_tree_node_t *__binary_tree_node_find(binary_tree_node_t *node,
                                            void *value, compare_fn compare) {
  if (node == NULL) {
    return NULL;
  }
  int compare_res = (*compare)(node->value, value);
  if (compare_res == 0) {
    return node;
  } else if (compare_res < 0) {
    return __binary_tree_node_find(node->left, value, compare);
  } else if (compare_res > 0) {
    return __binary_tree_node_find(node->right, value, compare);
  } else {
    return NULL;
  }
}
