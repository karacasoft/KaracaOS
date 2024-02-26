#include <kernel/sysdefs.h>
#include <libc/stdio.h>
#include <mm/buddy_allocator.h>
#include <mm/mm.h>
#include <tree/tree.h>

#define VALUE(node) ((buddy_allocator__alloc_info_t *)(node)->value)
#define MIN_MEM_SPLIT (64 * 1024 * 1024)

extern uint32_t mm_total_memory_block_count;
extern uint32_t mm_available_block_count;

typedef struct __buddy_allocator__alloc_info buddy_allocator__alloc_info_t;

struct __buddy_allocator__alloc_info {
  void *vaddr;
  size_t len;
  BOOLEAN is_allocated;
};

void const *tree_location = (void *)0xC8000000;
size_t tree_area_size = 128 * 1024 * 1024;

SYS_RET __buddy_allocator__alloc_node(binary_tree_node_t **out_node);
SYS_RET __buddy_allocator__split_node(binary_tree_node_t *node);
SYS_RET __buddy_allocator__find_space(size_t len, binary_tree_node_t *node,
                                      binary_tree_node_t **out_node);

SYS_RET buddy_allocator__init() {
  SYS_RET ret;
  void *allocated = tree_location;
  size_t i;
  binary_tree_t *tree = (binary_tree_t *)(tree_location + 8);
  for (i = 0; i < 32 * 1024; i++) {
    void *ret_mem;
    if ((ret = mm_alloc(&ret_mem, 0xC8000000U + (i * 4096), 4096, 0))) {
      kaos_printf("Hata??\n");
      while(TRUE);
      return ret;
    }
  }

  *((uint32_t *)allocated) = TRUE;
  *((uint32_t *)allocated + 1) = sizeof(binary_tree_t) + 8;
  allocated += sizeof(binary_tree_t) + 8;
  *((uint32_t *)allocated) = FALSE;
  *((uint32_t *)allocated + 1) = tree_area_size - sizeof(binary_tree_t) - 8;

  binary_tree_node_t *root_node;
  ret = __buddy_allocator__alloc_node(&root_node);
  if (ret != SYS_RET_NO_ERROR) {
    kaos_printf("Alloc node error\n");
    return ret;
  }

  buddy_allocator__alloc_info_t *alloc_info =
      (buddy_allocator__alloc_info_t *)(root_node->value);

  alloc_info->len = 0x90000000 - 0x10000000;
  alloc_info->vaddr = (void *)0x10000000;
  alloc_info->is_allocated = FALSE;
  tree->root_node = root_node;

  kaos_printf("Test\n");

  return SYS_RET_NO_ERROR;
}

SYS_RET __buddy_allocator__alloc_node(binary_tree_node_t **out_node) {
  SYS_RET ret = SYS_RET_NO_ERROR;
  void *ptr = tree_location;

  BOOLEAN found = FALSE;
  size_t i = 0;
  while (!found) {
    if(*((uint32_t *)ptr) == FALSE && (*((uint32_t *)ptr + 1) >=
          sizeof(binary_tree_node_t) + sizeof(buddy_allocator__alloc_info_t) + 8)) {
      break;
    }
    ptr += *((uint32_t *)ptr + 1);

    if (ptr - tree_location > tree_area_size) {
      // PANIC
      kaos_printf("Tree size overflow\n");
      return SYS_RET_UNAVAILABLE;
    }
  }

  *((uint32_t *)ptr) = TRUE;
  uint32_t old_size = *((uint32_t *)ptr + 1);
  *((uint32_t *)ptr + 1) =
      sizeof(binary_tree_node_t) + sizeof(buddy_allocator__alloc_info_t) + 8;

  void *next_block_ptr = ptr + *((uint32_t *) ptr + 1);
  *((uint32_t *)next_block_ptr) = FALSE;
  *((uint32_t *)next_block_ptr + 1) = old_size - *((uint32_t *) ptr + 1);


  binary_tree_node_t *new_node = (binary_tree_node_t *)(ptr + 8);
  buddy_allocator__alloc_info_t *new_alloc_info =
      (buddy_allocator__alloc_info_t *)(ptr + 8 + sizeof(binary_tree_node_t));
  new_node->left = NULL;
  new_node->right = NULL;
  new_node->parent = NULL;
  new_node->is_left = FALSE;
  new_node->is_right = FALSE;
  new_node->value = new_alloc_info;

  *out_node = new_node;
  return ret;
}

SYS_RET __buddy_allocator__split_node(binary_tree_node_t *node) {
  SYS_RET ret = SYS_RET_NO_ERROR;
  buddy_allocator__alloc_info_t *info_parent = VALUE(node);
  if (info_parent->len <= MIN_MEM_SPLIT) {
    // Minimum size reached. No more splitting
    return SYS_RET_OUT_OF_RANGE;
  }

  binary_tree_node_t *left, *right;
  ret = __buddy_allocator__alloc_node(&left);
  if (ret != SYS_RET_NO_ERROR)
    return ret;
  ret = __buddy_allocator__alloc_node(&right);
  if (ret != SYS_RET_NO_ERROR)
    return ret;

  buddy_allocator__alloc_info_t *info_left = VALUE(left);
  buddy_allocator__alloc_info_t *info_right = VALUE(right);

  info_left->vaddr = info_parent->vaddr;
  info_right->vaddr =
      (void *)(((uint64_t)info_parent->vaddr + info_parent->len) / 2);

  info_left->len = info_right->vaddr - info_left->vaddr;
  info_right->len = ((uint64_t)info_parent->vaddr + info_parent->len) -
                    (uint64_t)info_right->vaddr;

  left->is_left = TRUE;
  right->is_right = TRUE;

  node->left = left;
  node->right = right;

  return ret;
}

SYS_RET buddy_allocator__alloc(uint32_t len, void **out_addr) {
  SYS_RET ret = SYS_RET_NO_ERROR;
  binary_tree_t *tree = (binary_tree_t *)(tree_location + 8);

  binary_tree_node_t *node = tree->root_node;

  uint32_t rem_mem = mm_available_block_count * 4096;
  if (len > rem_mem) {
    kaos_printf("Phys mem not enough\n");
    return SYS_RET_MEM_ALLOC_ERROR;
  }

  ret = __buddy_allocator__find_space(len, node, &node);

  buddy_allocator__alloc_info_t *info = VALUE(node);

  uint64_t i;
  void *ret_mem;
  for (i = (uint64_t)info->vaddr; i < (uint64_t)info->vaddr + info->len;
       i += 4096) {
    if ((ret = mm_alloc(&ret_mem, i, 4096, 0))) {
      kaos_printf("mm_alloc error\n");
      return ret;
    }
  }

  info->is_allocated = TRUE;
  return SYS_RET_NO_ERROR;
}

SYS_RET buddy_allocator__free(void *addr) {
  SYS_RET ret = SYS_RET_NO_ERROR;
  binary_tree_t *tree = (binary_tree_t *)(tree_location + 8);

  binary_tree_node_t *node = tree->root_node;

  while (VALUE(node)->vaddr != addr && node->right) {
    node = node->right;
  }

  if (VALUE(node)->vaddr != addr) {
    return SYS_RET_NOT_FOUND;
  }

  while (node->left) {
    node = node->left;
  }

  mm_free(addr);
  VALUE(node)->is_allocated = FALSE;

  return ret;
}

SYS_RET __buddy_allocator__find_space(size_t len, binary_tree_node_t *node,
                                      binary_tree_node_t **out_node) {
  SYS_RET ret = SYS_RET_NO_ERROR;
  buddy_allocator__alloc_info_t *cur_value = VALUE(node);
  if (cur_value->is_allocated) {
    return SYS_RET_NOT_FOUND;
  }

  if (cur_value->len > 2 * len && cur_value < 4 * len) {
    *out_node = node;
    return SYS_RET_NO_ERROR;
  }
  if (node->left == NULL && node->right == NULL) {
    if(cur_value->len <= MIN_MEM_SPLIT) {
      *out_node = node;
      return SYS_RET_NO_ERROR;
    }
    ret = __buddy_allocator__split_node(node);
    if (ret != SYS_RET_NO_ERROR) {
      return ret;
    }
  }

  binary_tree_node_t *found_node;
  ret = __buddy_allocator__find_space(len, node->left, &found_node);
  cur_value = VALUE(found_node);
  if (ret == SYS_RET_NO_ERROR && !cur_value->is_allocated) {
    *out_node = found_node;
    return ret;
  }

  ret = __buddy_allocator__find_space(len, node->right, &found_node);
  cur_value = VALUE(found_node);
  if (ret == SYS_RET_NO_ERROR && !cur_value->is_allocated) {
    *out_node = found_node;
    return ret;
  }

  return SYS_RET_NOT_FOUND;
}
