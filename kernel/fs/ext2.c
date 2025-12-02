#include "kernel/sysdefs.h"
#include <fs/ext2/ext2.h>
#include <libc/stdio.h>
#include <libc/string.h>
#include <mm/mm.h>

SYS_RET fill_superblock(ext2_superblock_t *superblock, uint32_t block_size,
                        uint32_t sector_size, uint32_t nr_kbytes,
                        uint32_t inodes_per_group);
SYS_RET fill_block_group_descriptor(ext2_block_group_descriptor_t *bg_desc,
                                    const ext2_superblock_t *superblock,
                                    uint32_t bg_index);
SYS_RET setup_initial_block_bitmap(uint8_t initial_block_bitmap[1024],
                                   uint32_t inode_table_length);
SYS_RET setup_initial_inode_bitmap(uint8_t initial_inode_bitmap[1024],
                                   uint32_t block_size, uint32_t inodes_per_group);
SYS_RET create_dir_inode(ext2_inode_t *root);
SYS_RET allocate_blocks(ext2_mount_context_t *mnt, uint32_t *allocated_block,
                        size_t count);
SYS_RET allocate_inode(ext2_mount_context_t *mnt, uint32_t *allocated_inode,
                       BOOLEAN is_dir, BOOLEAN allocate_given);

BOOLEAN is_inode_allocated(char inode_bitmap[1024], uint32_t inode_nr);
void set_inode_bit(char inode_bitmap[1024], uint32_t inode_nr);
void clear_inode_bit(char inode_bitmap[1024], uint32_t inode_nr);

SYS_RET ext2_format_device(block_device_t *blockDevice) {
  SYS_RET ret;
  size_t i, j;
  ext2_superblock_t superblock;
  uint32_t block_size = 1024;
  uint32_t inodes_per_group = 1712;
  uint8_t initial_block_bitmap[1024];
  kaos_memset(initial_block_bitmap, 0, 1024);
  uint8_t initial_inode_bitmap[1024];
  kaos_memset(initial_inode_bitmap, 0, 1024);
  if ((ret = fill_superblock(&superblock, block_size, blockDevice->sectorSize,
          blockDevice->size_in_kb, inodes_per_group))) {
    return ret;
  }
  uint32_t block_group_count = superblock.s_blocks_count / (block_size * 8) + 1;
  uint32_t bg_desc_table_size =
      block_group_count * sizeof(ext2_block_group_descriptor_t);
  if (bg_desc_table_size > 1024) {
    kaos_printf("ext2_format_device(): Block group count is reduced to 51 to "
                "fit bg desc table into 1 block\n");
    block_group_count = 51;
  }
  bg_desc_table_size = 1024;
  ext2_block_group_descriptor_t *block_group_desc =
      (ext2_block_group_descriptor_t *)kaos_kernmalloc(bg_desc_table_size);

  for (i = 0; i < block_group_count; i++) {
    fill_block_group_descriptor(block_group_desc + i, &superblock, i);
  }
  uint32_t free_blocks = 0;
  uint32_t free_inodes = 0;
  for (i = 0; i < block_group_count; i++) {
    free_blocks += block_group_desc[i].bg_free_blocks_count;
    free_inodes += block_group_desc[i].bg_free_inodes_count;
  }
  superblock.s_free_blocks_count = free_blocks;
  superblock.s_free_inodes_count = free_inodes;

  uint32_t inode_table_length =
      inodes_per_group / (block_size / superblock.s_inode_size);
  uint32_t curr_block = 1;
  uint32_t sectors_per_block = block_size / blockDevice->sectorSize;

  setup_initial_block_bitmap(initial_block_bitmap, inode_table_length);

  for (i = 0; i < block_group_count; i++) {
    // Write superblock (or backup)
    ext2_write_blocks(blockDevice, curr_block++, (void *)&superblock, 1);

    // Write block group descriptor table
    ext2_write_blocks(blockDevice, curr_block++, (void *)block_group_desc, 1);

    // Write block bitmap
    if (i == block_group_count - 1) {
      // Add padding for the blocks that would map outside of the filesystem
      uint32_t free_blocks =
          block_group_desc[i].bg_free_blocks_count + inode_table_length + 4;
      uint32_t breaking_byte = free_blocks / 8;
      uint32_t nr_empty = free_blocks % 8;

      for (j = 0; j < nr_empty; j++) {
        initial_block_bitmap[breaking_byte] <<= 1;
        initial_block_bitmap[breaking_byte] += 1;
      }
      initial_block_bitmap[breaking_byte] =
          ~initial_block_bitmap[breaking_byte];

      for (j = breaking_byte + 1; j < 1024; j++) {
        initial_block_bitmap[j] = 0xFF;
      }
    }
    ext2_write_blocks(blockDevice, curr_block++, (void *)initial_block_bitmap,
                      1);

    setup_initial_inode_bitmap(initial_inode_bitmap, block_size, inodes_per_group);

    // Write inode bitmap
    ext2_write_blocks(blockDevice, curr_block++, (void *)initial_inode_bitmap,
                      1);

    // Fill inode table
    for (j = 0; j < inode_table_length; j++) {
      ext2_write_blocks(blockDevice, curr_block++, (void *)initial_inode_bitmap,
                        1);
    }
    // Skip data blocks
    curr_block += block_group_desc[i].bg_free_blocks_count;
  }

  ext2_mount_context_t mnt;
  if ((ret = ext2_mount_device(blockDevice, &mnt))) {
    kaos_printf(
        "An error occured while mounting newly created system (Code: %d)\n",
        ret);
    return ret;
  }
  uint32_t inode_nr = 1;
  SYS_RET inode_ret;
  inode_ret = allocate_inode(&mnt, &inode_nr, FALSE, TRUE);
  if(inode_ret != SYS_RET_NO_ERROR) {
    kaos_printf("Error allocating inode 1");
  }


  uint32_t root_inode_nr = 2;
  inode_ret = allocate_inode(&mnt, &root_inode_nr, TRUE, TRUE); // / directory
  if(inode_ret != SYS_RET_NO_ERROR) {
    kaos_printf("Error allocating inode 2");
  }
  i = 11;
  inode_ret = allocate_inode(&mnt, (uint32_t *)&i, TRUE, TRUE); // /lost+found directory
  if(inode_ret != SYS_RET_NO_ERROR) {
    kaos_printf("Error allocating inode 11");
  }

  ext2_inode_t root_dir;
  create_dir_inode(&root_dir);
  uint32_t root_dir_block;
  allocate_blocks(&mnt, &root_dir_block, 1);
  
  root_dir.i_block[0] = root_dir_block;
  ext2_write_inode(&mnt, &root_dir, 2);

  ext2_inode_t lostfound_dir;
  create_dir_inode(&lostfound_dir);
  uint32_t lostfound_dir_block;
  allocate_blocks(&mnt, &lostfound_dir_block, 1);
  lostfound_dir.i_block[0] = lostfound_dir_block;
  ext2_write_inode(&mnt, &lostfound_dir, 11);

  ext2_add_link(&mnt, 2, ".", 0, 1, 2);
  ext2_add_link(&mnt, 2, "..", 0, 2, 2);

  ext2_add_link(&mnt, 2, "lost+found", 0, 10, 11);
  ext2_add_link(&mnt, 11, ".", 0, 1, 11);
  ext2_add_link(&mnt, 11, "..", 0, 2, 2);

  ext2_unmount_device(&mnt);

  return SYS_RET_NO_ERROR;
}

SYS_RET ext2_mount_device(block_device_t *block_device,
                          ext2_mount_context_t *mnt) {
  SYS_RET ret = SYS_RET_NO_ERROR;
  if (block_device->sectorSize != 512) {
    kaos_printf("KaracaOS cannot handle disks with sector_size != 512\n");
    return SYS_RET_OUT_OF_RANGE;
  }
  void *superblock_ptr = (void *)&mnt->ctx_superblock;
  uint32_t superblock_sector = 1024 / block_device->sectorSize;
  if ((ret = ext2_read_blocks(block_device, 1, superblock_ptr, 1))) {
    kaos_printf(
        "Unexpected error while mounting device, (ext2_read_blocks: ret: %d)\n",
        ret);
    return ret;
  }
  // TODO maybe check superblock integrity... :V :V
  mnt->ctx_superblock.s_state = EXT2_ERROR_FS;
  if ((ret = ext2_write_blocks(block_device, 1, superblock_ptr, 1))) {
    kaos_printf("Unexpected error while mounting device, (ext2_write_blocks: "
                "ret: %d)\n",
                ret);
    return ret;
  }
  mnt->ctx_block_device = block_device;
  return ret;
}

SYS_RET ext2_unmount_device(ext2_mount_context_t *mnt) {
  SYS_RET ret = SYS_RET_NO_ERROR;
  block_device_t *block_device = mnt->ctx_block_device;
  void *superblock_ptr = (void *)&mnt->ctx_superblock;
  mnt->ctx_superblock.s_state = EXT2_VALID_FS;
  if ((ret = ext2_write_blocks(block_device, 1, superblock_ptr, 1))) {
    kaos_printf("Unexpected error while unmounting device, (ext2_write_blocks: "
                "ret: %d)\n",
                ret);
    return ret;
  }
  kaos_memset((void *)mnt, 0, sizeof(ext2_mount_context_t));
  return ret;
}

SYS_RET ext2_add_link(ext2_mount_context_t *mnt, uint32_t dir_inode,
                      const char *name, uint8_t type, uint8_t name_len,
                      uint32_t inode) {
  SYS_RET ret = SYS_RET_NO_ERROR;

  ext2_inode_t dir;
  ext2_read_inode(mnt, &dir, dir_inode);

  ext2_inode_t added_inode;
  ext2_read_inode(mnt, &added_inode, inode);

  uint32_t dir_block_nr = dir.i_block[0];

  unsigned char *rw_buffer = mnt->ctx_block_device->rw_buffer;
  ext2_read_blocks(mnt->ctx_block_device, dir_block_nr, rw_buffer, 1);

  ext2_directory_element_t element;
  ext2_init_dir_walk(mnt, rw_buffer, &element);

  while (element.inode != 0) {
    ext2_dir_walk_next(mnt, &element);
  }

  uint16_t name_len_with_padding = name_len + (4 - name_len % 4) % 4;
  uint16_t rec_len = 8 + name_len_with_padding;

  unsigned char *buffer_pos = element.buffer_pos;

  *((uint32_t *)buffer_pos) = inode;
  *((uint16_t *)(buffer_pos + 4)) = rec_len;
  *((uint8_t *)(buffer_pos + 6)) = name_len;
  *((uint8_t *)(buffer_pos + 7)) = type;
  kaos_memcpy(buffer_pos + 8, name, name_len);
  kaos_memset(buffer_pos + 8 + name_len, 0, 4 - (name_len % 4));

  buffer_pos += rec_len;
  kaos_printf("%x\n", buffer_pos - rw_buffer);

  *((uint32_t *)buffer_pos) = 0;
  *((uint16_t *)(buffer_pos + 4)) = 1024 - (buffer_pos - rw_buffer);
  *((uint8_t *)buffer_pos + 6) = 0;
  *((uint8_t *)buffer_pos + 7) = 0;

  ext2_write_blocks(mnt->ctx_block_device, dir_block_nr, rw_buffer, 1);

  added_inode.i_links_count++;
  ext2_write_inode(mnt, &added_inode, inode);

  return ret;
}

SYS_RET ext2_init_dir_walk(ext2_mount_context_t *mnt, void *buffer,
                           ext2_directory_element_t *first) {
  SYS_RET ret = SYS_RET_NO_ERROR;

  first->inode = *((uint32_t *)buffer);
  first->rec_len = *((uint16_t *)(buffer + 4));
  first->name_len = *((uint8_t *)(buffer + 6));
  first->file_type = *((uint8_t *)(buffer + 7));
  first->name = ((char *)(buffer + 8));
  first->buffer_pos = buffer;

  return ret;
}

SYS_RET ext2_dir_walk_next(ext2_mount_context_t *mnt,
                           ext2_directory_element_t *element) {
  SYS_RET ret = SYS_RET_NO_ERROR;

  uint16_t prev_rec_length = element->rec_len;
  unsigned char *buffer = element->buffer_pos + prev_rec_length;

  element->inode = *((uint32_t *)buffer);
  element->rec_len = *((uint16_t *)(buffer + 4));
  element->name_len = *((uint8_t *)(buffer + 6));
  element->file_type = *((uint8_t *)(buffer + 7));
  element->name = ((char *)(buffer + 8));
  element->buffer_pos = buffer;

  return ret;
}

// ------------------------------------------------
// Inode manipulation routines
// *** Inodes start from 1 ***
// ------------------------------------------------

SYS_RET ext2_read_inode(ext2_mount_context_t *mnt, ext2_inode_t *inode,
                        uint32_t inode_nr) {
  SYS_RET ret = SYS_RET_NO_ERROR;
  uint32_t inodes_per_group = mnt->ctx_superblock.s_inodes_per_group;
  uint32_t blocks_per_group = mnt->ctx_superblock.s_blocks_per_group;

  uint32_t target_block_group = (inode_nr - 1) / inodes_per_group;
  uint32_t local_inode_index = (inode_nr - 1) % inodes_per_group;

  ext2_block_group_descriptor_t desc_table[32];
  ext2_read_blocks(mnt->ctx_block_device, 2, (void *)desc_table, 1);

  char inode_bitmap_block[1024];
  kaos_memset(inode_bitmap_block, 0, 1024);
  ext2_read_blocks(mnt->ctx_block_device,
                   desc_table[target_block_group].bg_inode_bitmap,
                   (void *)inode_bitmap_block, 1);

  if (!is_inode_allocated(inode_bitmap_block, local_inode_index)) {
    kaos_memset(inode, 0, sizeof(ext2_inode_t));
    return SYS_RET_NOT_FOUND;
  }

  void *rw_buffer = mnt->ctx_block_device->rw_buffer;

  uint32_t target_block =
      desc_table[target_block_group].bg_inode_table + local_inode_index / 8;
  ext2_read_blocks(mnt->ctx_block_device, target_block, rw_buffer, 1);

  ext2_inode_t *inode_table = (ext2_inode_t *)rw_buffer;
  *inode = inode_table[local_inode_index % 8];

  return ret;
}

SYS_RET ext2_write_inode(ext2_mount_context_t *mnt, ext2_inode_t *inode,
                         uint32_t inode_nr) {
  SYS_RET ret = SYS_RET_NO_ERROR;
  uint32_t inodes_per_group = mnt->ctx_superblock.s_inodes_per_group;
  uint32_t blocks_per_group = mnt->ctx_superblock.s_blocks_per_group;

  uint32_t target_block_group = (inode_nr - 1) / inodes_per_group;
  uint32_t local_inode_index = (inode_nr - 1) % inodes_per_group;

  ext2_block_group_descriptor_t desc_table[32];
  ext2_read_blocks(mnt->ctx_block_device, 2, (void *)desc_table, 1);

  char inode_bitmap_block[1024];
  kaos_memset(inode_bitmap_block, 0, 1024);
  ext2_read_blocks(mnt->ctx_block_device,
                   desc_table[target_block_group].bg_inode_bitmap,
                   (void *)inode_bitmap_block, 1);

  set_inode_bit(inode_bitmap_block, local_inode_index);

  ext2_write_blocks(mnt->ctx_block_device,
                    desc_table[target_block_group].bg_inode_bitmap,
                    (void *)inode_bitmap_block, 1);

  void *rw_buffer = mnt->ctx_block_device->rw_buffer;

  uint32_t target_block =
      desc_table[target_block_group].bg_inode_table + local_inode_index / 8;
  ext2_read_blocks(mnt->ctx_block_device, target_block, rw_buffer, 1);

  ext2_inode_t *inode_table = (ext2_inode_t *)rw_buffer;
  inode_table[local_inode_index % 8] = *inode;

  ext2_write_blocks(mnt->ctx_block_device, target_block, rw_buffer, 1);

  return ret;
}

// --------------------------------------
// Lowest level sub routines
// --------------------------------------

SYS_RET ext2_read_blocks(block_device_t *block_device, uint32_t block_id,
                         void *buffer, size_t count) {
  SYS_RET ret = SYS_RET_NO_ERROR;
  size_t i;
  size_t sectors_per_block = 1024 / block_device->sectorSize;
  for (i = 0; i < count * sectors_per_block; i++) {
    if ((ret = block_dev_read(block_device, block_id * sectors_per_block + i,
                              (buffer + i * block_device->sectorSize)))) {
      kaos_printf("An error occured while reading data from disk (Code: %d)\n",
                  ret);
      return ret;
    }
  }
  return ret;
}

SYS_RET ext2_write_blocks(block_device_t *block_device, uint32_t block_id,
                          void *buffer, size_t count) {
  SYS_RET ret = SYS_RET_NO_ERROR;
  size_t i;
  size_t sectors_per_block = 1024 / block_device->sectorSize;
  for (i = 0; i < count * sectors_per_block; i++) {
    if ((ret = block_dev_write(block_device, block_id * sectors_per_block + i,
                               (buffer + i * block_device->sectorSize)))) {
      kaos_printf("An error occured while writing data to disk (Code: %d)\n",
                  ret);
      return ret;
    }
  }
  return ret;
}

//-------------------------------------
// Local sub routines
//-------------------------------------

SYS_RET fill_superblock(ext2_superblock_t *superblock, uint32_t block_size,
                        uint32_t sector_size, uint32_t nr_kbytes,
                        uint32_t inodes_per_group) {
  if (sector_size > 1024) {
    // TODO implement this later
    kaos_printf(
        "KaracaOS cannot handle disks with sector size > 1024, for now...\n");
    return SYS_RET_OUT_OF_RANGE;
  }
  if (block_size % sector_size != 0) {
    // TODO warn about unused bytes
    kaos_printf(
        "fill_superblock(): block size is not a multiple of sector size, every "
        "sector on disk will have some wasted space left...\n");
  }

  kaos_memset((void *)superblock, 0, sizeof(ext2_superblock_t));

  uint16_t sector_per_block = block_size / sector_size;
  uint32_t nr_blocks = nr_kbytes;
  uint32_t nr_block_groups = nr_blocks / (block_size * 8) + 1;
  uint32_t nr_inodes = nr_block_groups * inodes_per_group;

  superblock->s_inodes_count = nr_inodes;
  superblock->s_blocks_count = nr_blocks;
  superblock->s_r_blocks_count = 100;
  superblock->s_free_blocks_count = nr_blocks;
  superblock->s_free_inodes_count = nr_inodes;
  superblock->s_first_data_block = (block_size > 1024) ? 0 : 1;
  superblock->s_log_block_size = 0; // TODO block_size = 1024 assumed
  superblock->s_log_frag_size = 0;  // TODO frag_size = 1024 assumed
  superblock->s_blocks_per_group = block_size * 8;
  superblock->s_frags_per_group = block_size * 8;
  superblock->s_inodes_per_group = inodes_per_group;
  superblock->s_mtime = 0;
  superblock->s_wtime = 0;
  superblock->s_mnt_count = 0;
  superblock->s_max_mnt_count = 32767;
  superblock->s_magic = 0xEF53;
  superblock->s_state = EXT2_VALID_FS;
  superblock->s_errors = EXT2_ERRORS_RO;
  superblock->s_minor_rev_level = 0;
  superblock->s_lastcheck = 0;
  superblock->s_checkinterval = 0;
  superblock->s_creator_os = EXT2_OS_KARACAOS;
  superblock->s_rev_level = EXT2_DYNAMIC_REV;
  superblock->s_def_resuid = 0;
  superblock->s_def_resgid = 0;
  superblock->s_first_ino = 11;
  superblock->s_inode_size = 128;
  superblock->s_block_group_nr = 0;
  superblock->s_feature_compat = 0;
  superblock->s_feature_incompat = 0;
  superblock->s_feature_ro_compat = 0;
  kaos_memcpy(superblock->s_uuid, "wowdiskuuid10000", 16);
  kaos_memcpy(superblock->s_volume_name, "KaracaOS", 9);
  superblock->s_last_mounted[0] = '/';
  superblock->s_last_mounted[1] = 0;
  superblock->s_algo_bitmap = 0;
  superblock->s_prealloc_blocks = 0;
  superblock->s_prealloc_dir_blocks = 0;
  superblock->s_default_mount_options = 0;
  superblock->s_first_meta_bg = 0;

  return SYS_RET_NO_ERROR;
}

SYS_RET fill_block_group_descriptor(ext2_block_group_descriptor_t *bg_desc,
                                    const ext2_superblock_t *superblock,
                                    uint32_t bg_index) {
  // TODO bg descriptor table is assumed to have size of 1 block
  kaos_printf("fill_block_group_descriptor() assumes 1 block size of block "
              "descriptor table\n");
  uint32_t total_blocks = superblock->s_blocks_count;

  uint32_t inodes_per_group = 1712;
  uint32_t blocks_per_group = superblock->s_blocks_per_group;
  uint32_t block_size = 1024 << superblock->s_log_block_size;
  uint32_t inode_table_length =
      inodes_per_group / (block_size / superblock->s_inode_size);

  uint32_t block_bitmap_addr = bg_index * blocks_per_group + 3;
  uint32_t inode_bitmap_addr = bg_index * blocks_per_group + 4;
  uint32_t inode_table_addr = bg_index * blocks_per_group + 5;

  uint32_t free_blocks_count = blocks_per_group - inode_table_length - 4;
  if ((bg_index + 1) * blocks_per_group > total_blocks) {
    // this value will overflow if leftover blocks are not enough to hold the
    // inode table
    // FIXME if you will...
    free_blocks_count = total_blocks - (bg_index * blocks_per_group) -
                        inode_table_length - 4 - 1;
  }

  bg_desc->bg_block_bitmap = block_bitmap_addr;
  bg_desc->bg_inode_bitmap = inode_bitmap_addr;
  bg_desc->bg_inode_table = inode_table_addr;
  bg_desc->bg_free_blocks_count = free_blocks_count;
  bg_desc->bg_free_inodes_count = inodes_per_group;
  bg_desc->bg_used_dirs_count = 0;
  bg_desc->bg_pad = 0;
  bg_desc->bg_reserved[0] = 0;
  bg_desc->bg_reserved[1] = 0;
  bg_desc->bg_reserved[2] = 0;
  bg_desc->bg_reserved[3] = 0;
  bg_desc->bg_reserved[4] = 0;
  bg_desc->bg_reserved[5] = 0;

  return SYS_RET_NO_ERROR;
}

SYS_RET setup_initial_block_bitmap(uint8_t initial_block_bitmap[1024],
                                   uint32_t inode_table_length) {
  size_t i;
  for (i = 0; i < (inode_table_length + 4) / 8; i++) {
    initial_block_bitmap[i] = 0xff;
  }
  uint8_t last_val = 0x0;
  for (i = 0; i < (inode_table_length + 4) % 8; i++) {
    last_val <<= 1;
    last_val++;
  }
  initial_block_bitmap[(inode_table_length + 4) / 8] = last_val;
  return SYS_RET_NO_ERROR;
}

SYS_RET setup_initial_inode_bitmap(uint8_t initial_inode_bitmap[1024], uint32_t block_size, uint32_t inodes_per_group) {
  size_t i;

  unsigned char transition_byte = 0xff;
  for(i = 0; i < (inodes_per_group % 8); i++) {
    transition_byte <<= 1;
  }  
  initial_inode_bitmap[inodes_per_group / 8] = transition_byte;
  for(i = 0; i < (inodes_per_group / 8); i++) {
    initial_inode_bitmap[i] = 0x0;
  }
  for(i = (inodes_per_group / 8) + 1; i < 1024; i++){
    initial_inode_bitmap[i] = 0xff;
  }
  
  return SYS_RET_NO_ERROR;
}

SYS_RET create_dir_inode(ext2_inode_t *root) {
  root->i_mode = EXT2_S_IFDIR | 0x1FF;
  root->i_uid = 0;
  root->i_size = 1024;
  root->i_atime = 0;
  root->i_ctime = 0;
  root->i_mtime = 0;
  root->i_dtime = 0;
  root->i_gid = 0;
  root->i_links_count = 0;
  root->i_blocks = 2;
  root->i_flags = 0;
  root->i_osd1 = 0;
  kaos_memset((void *)root->i_block, 0, sizeof(root->i_block));
  root->i_generation = 0;
  root->i_file_acl = 0;
  root->i_dir_acl = 0;
  root->i_faddr = 0;
  kaos_memset((void *)root->i_osd2, 0, sizeof(root->i_osd2));
  return SYS_RET_NO_ERROR;
}

SYS_RET allocate_blocks(ext2_mount_context_t *mnt, uint32_t *allocated_blocks,
                        size_t count) {
  SYS_RET ret = SYS_RET_NO_ERROR;
  size_t i = 0;
  uint32_t *_allocated_blocks;

  ext2_block_group_descriptor_t desc_table[32];
  ext2_read_blocks(mnt->ctx_block_device, 2, (void *)desc_table, 1);

  for (i = 0; i < 32; i++) {
    BOOLEAN found_all = FALSE;
    _allocated_blocks = allocated_blocks;
    if (desc_table[i].bg_block_bitmap == 0) {
      return SYS_RET_NOT_FOUND;
    }
    if (desc_table[i].bg_free_blocks_count < count) {
      // Continue on to next block
      continue;
    }
    size_t j, k;
    uint8_t block_bitmap[1024];
    ext2_read_blocks(mnt->ctx_block_device, desc_table[i].bg_block_bitmap,
                     block_bitmap, 1);

    for (j = 0; j < 1024; j++) {
      if (block_bitmap[j] == 0xFF) {
        continue;
      }
      for (k = 0; k < 8 && (block_bitmap[j] & (1 << k)) != 0; k++)
        ;
      *_allocated_blocks++ =
          i * mnt->ctx_superblock.s_blocks_per_group + j * 8 + k + 1;
      if (_allocated_blocks - allocated_blocks == count) {
        found_all = TRUE;
        break;
      }
    }
    if (found_all) {
      for (j = 0; j < count; j++) {
        size_t byte_idx = ((allocated_blocks[j] - 1) %
                           mnt->ctx_superblock.s_blocks_per_group) /
                          8;
        size_t bit_offset = ((allocated_blocks[j] - 1) %
                             mnt->ctx_superblock.s_blocks_per_group) %
                            8;
        block_bitmap[byte_idx] = block_bitmap[byte_idx] | (1 << bit_offset);
      }

      ext2_write_blocks(mnt->ctx_block_device, desc_table[i].bg_block_bitmap,
                        block_bitmap, 1);
      mnt->ctx_superblock.s_free_blocks_count -= count;
      desc_table[i].bg_free_blocks_count -= count;
      ext2_write_blocks(mnt->ctx_block_device, 2, (void *)desc_table, 1);
      ret = SYS_RET_NO_ERROR;
      break;
    }
  }

  return ret;
}

SYS_RET allocate_inode(ext2_mount_context_t *mnt, uint32_t *allocated_inode,
                       BOOLEAN is_dir, BOOLEAN allocate_given) {
  SYS_RET ret = SYS_RET_NO_ERROR;
  size_t i = 0, j, k;

  uint8_t inode_bitmap[1024];

  ext2_block_group_descriptor_t desc_table[32];
  ext2_read_blocks(mnt->ctx_block_device, 2, (void *)desc_table, 1);

  BOOLEAN found = FALSE;
  if (allocate_given) {
    i = (*allocated_inode - 1) / mnt->ctx_superblock.s_inodes_per_group;
    j = (*allocated_inode - 1) % mnt->ctx_superblock.s_inodes_per_group;
    k = j % 8;
    j = j / 8;

    ext2_read_blocks(mnt->ctx_block_device, desc_table[i].bg_inode_bitmap,
                     inode_bitmap, 1);

    if ((inode_bitmap[j] & (1 << k)) == 0) {
      found = TRUE;
    } else {
      return SYS_RET_UNAVAILABLE;
    }

  } else {
    for (i = 0; i < 32; i++) {
      if (desc_table[i].bg_free_inodes_count != 0) {

        ext2_read_blocks(mnt->ctx_block_device, desc_table[i].bg_inode_bitmap,
                         inode_bitmap, 1);

        for (j = 0; j < 1024; j++) {
          if (inode_bitmap[j] == 0xFF) {
            continue;
          }
          for (k = 0; (inode_bitmap[j] & (1 << k)) != 0; k++)
            ;
          *allocated_inode =
              i * mnt->ctx_superblock.s_inodes_per_group + j * 8 + k + 1;
          found = TRUE;
          break;
        }
      }
    }
  }
  if (found) {
    mnt->ctx_superblock.s_free_inodes_count--;
    inode_bitmap[j] = inode_bitmap[j] | (1 << k);

    ext2_write_blocks(mnt->ctx_block_device, desc_table[i].bg_inode_bitmap,
                      inode_bitmap, 1);
    desc_table[i].bg_free_inodes_count--;
    if (is_dir) {
      desc_table[i].bg_used_dirs_count++;
    }
    ext2_write_blocks(mnt->ctx_block_device, 2, (void *)desc_table, 1);
  }

  return ret;
}

BOOLEAN is_inode_allocated(char inode_bitmap[1024], uint32_t inode_nr) {
  return (inode_bitmap[inode_nr / 8] & (1 << (inode_nr % 8))) ? TRUE : FALSE;
}

void set_inode_bit(char inode_bitmap[1024], uint32_t inode_nr) {
  inode_bitmap[inode_nr / 8] = inode_bitmap[inode_nr / 8] | (1 << inode_nr % 8);
}

void clear_inode_bit(char inode_bitmap[1024], uint32_t inode_nr) {
  inode_bitmap[inode_nr / 8] =
      inode_bitmap[inode_nr / 8] & ~(1 << inode_nr % 8);
}
