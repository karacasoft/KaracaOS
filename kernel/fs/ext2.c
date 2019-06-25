#include <fs/ext2/ext2.h>
#include <libc/string.h>
#include <libc/stdio.h>
#include <mm/mm.h>


SYS_RET fill_superblock(ext2_superblock_t *superblock, uint32_t block_size, uint32_t sector_size, uint32_t nr_kbytes, uint32_t inodes_per_group);
SYS_RET fill_block_group_descriptor(ext2_block_group_descriptor_t *bg_desc, const ext2_superblock_t *superblock, uint32_t bg_index);
SYS_RET setup_initial_block_bitmap(char initial_block_bitmap[1024], uint32_t inode_table_length);

SYS_RET write_block(void *data, uint32_t block, uint32_t sectors_per_block, block_device_t *device);


SYS_RET ext2_format_device(block_device_t *blockDevice) {
    SYS_RET ret;
    size_t i, j;
    ext2_superblock_t superblock;
    uint32_t block_size = 1024;
    uint32_t inodes_per_group = 1712;
    char initial_block_bitmap[1024] = {0};
    char initial_inode_bitmap[1024] = {0};
    if(ret = fill_superblock(&superblock, block_size, blockDevice->sectorSize, blockDevice->nrSectors, inodes_per_group)) {
        return ret;
    }
    uint32_t block_group_count = superblock.s_blocks_count / (block_size * 8);
    uint32_t bg_desc_table_size = block_group_count * sizeof(ext2_block_group_descriptor_t);
    if(bg_desc_table_size > 1024) {
        kaos_printf("ext2_format_device(): Block group count is reduced to 51 to fit bg desc table into 1 block\n");
        block_group_count = 51;
    }
    bg_desc_table_size = 1024;
    ext2_block_group_descriptor_t *block_group_desc = (ext2_block_group_descriptor_t *)
            kaos_kernmalloc(bg_desc_table_size);
    
    for (i = 0; i < block_group_count; i++) {
        fill_block_group_descriptor(block_group_desc + i, &superblock, i);
    }
    uint32_t free_blocks = 0;
    uint32_t free_inodes = 0;
    for(i = 0; i < block_group_count; i++) {
        free_blocks += block_group_desc[i].bg_free_blocks_count;
        free_inodes += block_group_desc[i].bg_free_inodes_count;
    }
    superblock.s_free_blocks_count = free_blocks;
    superblock.s_free_inodes_count = free_inodes;
    
    uint32_t inode_table_length = inodes_per_group / (block_size / superblock.s_inode_size);
    uint32_t curr_block = 1;
    uint32_t sectors_per_block = block_size / blockDevice->sectorSize;

    setup_initial_block_bitmap(initial_block_bitmap, inode_table_length);

    for (i = 0; i < block_group_count; i++) {
        // Write superblock (or backup)
        write_block((void *)&superblock, curr_block++, sectors_per_block, blockDevice);
        
        // Write block group descriptor table
        write_block((void *)block_group_desc, curr_block++, sectors_per_block, blockDevice);

        // Write bitmaps
        for(j = 0; j < 2; j++) {
            write_block((void *)initial_block_bitmap, curr_block++, sectors_per_block, blockDevice);
        }

        // Fill inode table
        for(j = 0; j < inode_table_length; j++) {
            write_block((void *)initial_inode_bitmap, curr_block++, sectors_per_block, blockDevice);
        }
        curr_block += block_group_desc[i].bg_free_blocks_count;
    }
}

SYS_RET ext2_mount_device(block_device_t *blockDevice);
//SYS_RET ext2_unmount_device(block_device_t *blockDevice);

SYS_RET ext2_get_file(uint8_t *retfd, char *filename);
SYS_RET ext2_read_file(uint8_t fd, void *buffer, size_t bufferSize);
SYS_RET ext2_write_file(uint8_t fd, void *buffer, size_t bufferSize);


SYS_RET fill_superblock(ext2_superblock_t *superblock, uint32_t block_size, uint32_t sector_size, uint32_t nr_kbytes, uint32_t inodes_per_group) {
    if(sector_size > 1024) {
        // TODO implement this later
        kaos_printf("KaracaOS cannot handle disks with sector size > 1024, for now...\n");
        return SYS_RET_OUT_OF_RANGE;
    }
    if(block_size % sector_size != 0) {
        // TODO warn about unused bytes
        kaos_printf("fill_superblock(): block size is not a multiple of sector size, every sector on disk will have some wasted space left...\n");
    }

    kaos_memset((void *)superblock, 0, sizeof(ext2_superblock_t));

    uint16_t sector_per_block = block_size / sector_size;
    uint32_t nr_blocks = nr_kbytes;
    uint32_t nr_block_groups = nr_blocks / (block_size * 8);
    uint32_t nr_inodes = nr_block_groups * inodes_per_group;

    superblock->s_inodes_count = nr_inodes;
    superblock->s_blocks_count = nr_blocks;
    superblock->s_r_blocks_count = 100;
    superblock->s_free_blocks_count = nr_blocks;
    superblock->s_free_inodes_count = nr_inodes;
    superblock->s_first_data_block = (block_size > 1024) ? 0 : 1;
    superblock->s_log_block_size = 0; // TODO block_size = 1024 assumed
    superblock->s_log_frag_size = 0; // TODO frag_size = 1024 assumed
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

SYS_RET fill_block_group_descriptor(ext2_block_group_descriptor_t *bg_desc, const ext2_superblock_t *superblock, uint32_t bg_index) {
    // TODO bg descriptor table is assumed to have size of 1 block
    kaos_printf("fill_block_group_descriptor() assumes 1 block size of block descriptor table\n");
    uint32_t inodes_per_group = 1712;
    uint32_t blocks_per_group = superblock->s_blocks_per_group;
    uint32_t block_size = 1024 << superblock->s_log_block_size;
    uint32_t inode_table_length = inodes_per_group / (block_size / superblock->s_inode_size);

    uint32_t block_bitmap_addr = bg_index * blocks_per_group + 3;
    uint32_t inode_bitmap_addr = bg_index * blocks_per_group + 4;
    uint32_t inode_table_addr = bg_index * blocks_per_group + 5;

    bg_desc->bg_block_bitmap = block_bitmap_addr;
    bg_desc->bg_inode_bitmap = inode_bitmap_addr;
    bg_desc->bg_inode_table = inode_table_addr;
    bg_desc->bg_free_blocks_count = blocks_per_group - inode_table_length - 4;
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

SYS_RET write_block(void *data, uint32_t block, uint32_t sectors_per_block, block_device_t *device) {
    SYS_RET ret = SYS_RET_NO_ERROR;
    size_t i;
    for(i = 0; i < sectors_per_block; i++) {
        if((ret = block_dev_write(device, block * sectors_per_block + i, (data + i * device->sectorSize)))) {
            kaos_printf("An error occured while writing data to disk (Code: %d)\n", ret);
            return ret;
        }
    }
    return ret;
}

SYS_RET setup_initial_block_bitmap(char initial_block_bitmap[1024], uint32_t inode_table_length) {
    size_t i;
    for(i = 0; i < (inode_table_length + 4) / 8; i++) {
        initial_block_bitmap[i] = 0xff;
    }
    uint8_t last_val = 0x0;
    for(i = 0; i < (inode_table_length + 4) % 8; i++) {
        last_val <<= 1;
        last_val++;
    }
    initial_block_bitmap[(inode_table_length + 4) / 8] = last_val;
    return SYS_RET_NO_ERROR;
}

SYS_RET setup_initial_inode_bitmap(char initial_inode_bitmap[1024]) {
    return SYS_RET_NO_ERROR;
}