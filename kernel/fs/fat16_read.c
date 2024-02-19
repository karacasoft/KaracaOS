#include "dev/block/block_dev.h"
#include "fs/fat16/fat16.h"
#include "kernel/sysdefs.h"
#include "libc/stdio.h"
#include "libc/string.h"
#include "mm/mm.h"

SYS_RET fat16_read_cluster(fat16_context_t *context, uint32_t cluster,
                           void *out_buffer);
SYS_RET fat16_read_cluster_fat_chain(fat16_context_t *context, uint32_t cluster,
                                     uint32_t *out_chain);

SYS_RET fat16_create_context(block_device_t *device, fat16_context_t *context) {
  SYS_RET ret;
  context->device = device;
  block_dev_read(device, 0, &context->boot_sector);
  int total_clusters =
      device->size_in_kb * 2 / context->boot_sector.sectorsPerCluster;
  int fat_size = total_clusters * 2; // two byte per cluster
  int fat_size_in_sectors = fat_size / device->sectorSize;
  int fat_count = context->boot_sector.nrOfFATCopies;
  int reserved_sectors = context->boot_sector.reservedSectors;

  int fat_start = reserved_sectors;
  int dir_start = fat_start + (fat_size_in_sectors * fat_count);
  int nr_max_dir_in_root = context->boot_sector.nrPossibleRootEntries;
  context->fat_start = fat_start;
  context->root_dir_start = dir_start;
  context->data_area_start_cluster =
      (dir_start / context->boot_sector.sectorsPerCluster) +
      ((nr_max_dir_in_root * 32) / device->sectorSize /
       context->boot_sector.sectorsPerCluster);

  void *buffer;
  ret = mm_alloc(
      &buffer, 0xD0000000,
      context->device->sectorSize * context->boot_sector.sectorsPerCluster, 0);
  if (ret != SYS_RET_NO_ERROR) {
    return ret;
  }
  context->buffer = buffer;
  return SYS_RET_NO_ERROR;
}

SYS_RET fat16_read_root_dir(fat16_context_t *context,
                            fat16_dir_entry_t *dir_entries,
                            uint32_t max_entries, uint32_t cluster_offset) {
  return fat16_read_directory(
      context,
      (context->root_dir_start / context->boot_sector.sectorsPerCluster) +
          cluster_offset,
      dir_entries, max_entries);
}

SYS_RET fat16_read_directory(fat16_context_t *context, uint32_t cluster,
                             fat16_dir_entry_t *out_dir_entries,
                             uint32_t max_entries) {
  fat16_read_cluster(context, cluster, context->buffer);

  fat16_dir_entry_t *dir_entries_retrieved =
      (fat16_dir_entry_t *)context->buffer;

  uint32_t i;
  for (i = 0; i < max_entries; i++) {
    if (dir_entries_retrieved[i].filename[0] == 0) {
      // We have reached the end of entries
      break;
    }
    if ((uint8_t)dir_entries_retrieved[i].filename[0] == 0xE5) {
      // The entry is deleted
      continue;
    }
    if (dir_entries_retrieved[i].attributes & 0x8) {
      // This is the volume label entry
      continue;
    }
    kaos_memcpy(&out_dir_entries[i], &dir_entries_retrieved[i],
                sizeof(fat16_dir_entry_t));
  }
  return SYS_RET_NO_ERROR;
}

void fat16_convert_filename(char *filename, char *out_converted) {
  char name[8];
  char extension[3];
  kaos_memset(name, ' ', 8);
  kaos_memset(extension, ' ', 3);
  size_t i;
  BOOLEAN has_extension = FALSE;
  for (i = 0; i < 8; i++) {

    if (filename[0] != '.' && filename[0] != '\0') {
      name[i] = filename[0];
      filename++;
    } else {
      has_extension = TRUE;
      break;
    }
  }
  if (has_extension) {
    filename++;
    for (i = 0; i < 3; i++) {
      if (filename[0] != '\0') {
        extension[i] = filename[0];
        filename++;
      } else {
        break;
      }
    }
  }
  kaos_memcpy(out_converted, name, 8);
  kaos_memcpy(out_converted + 8, extension, 3);
}

/**
 * Traverse the root directory to find an entry named `file_to_find`.
 *
 * @param `context` FAT16 context
 * @param `file_to_find` Must be the name of the file. specifically 11 bytes
 *                     with proper space padding.
 * @param `out_dir_entry` This will be filled with the directory entry that`
 *                      is found.
 * @return `SYS_RET_NOT_FOUND` in case of error, otherwise `SYS_RET_NO_ERROR`.
 */
SYS_RET fat16_find_in_root_dir(fat16_context_t *context, char *file_to_find,
                               fat16_dir_entry_t *out_dir_entry) {
  BOOLEAN found = FALSE;
  uint32_t current_cluster = 0;
  while (!found) {
    uint32_t max_entries_per_cluster =
        (context->boot_sector.sectorsPerCluster * context->device->sectorSize) /
        sizeof(fat16_dir_entry_t);
    fat16_dir_entry_t *entries_retrieved = (fat16_dir_entry_t *)context->buffer;

    fat16_read_root_dir(context, entries_retrieved, max_entries_per_cluster,
                        current_cluster);

    size_t i;
    for (i = 0; i < max_entries_per_cluster; i++) {
      if (entries_retrieved[i].filename[0] == '\0') {
        // directory end, file not found
        return SYS_RET_NOT_FOUND;
      } else if ((uint8_t)entries_retrieved[i].filename[0] == 0xE5) {
        // this entry is deleted, skip
        continue;
      }
      int cmp = kaos_strncmp(entries_retrieved[i].filename, file_to_find, 11);
      if (cmp == 0) {
        kaos_memcpy(out_dir_entry, &entries_retrieved[i],
                    sizeof(fat16_dir_entry_t));
        return SYS_RET_NO_ERROR;
      }
    }
    // We have traversed all entries in the cluster, go to next one now
    current_cluster++;
    kaos_printf("Current cluster skipped\n");
    // TODO: Check if we have reached the last cluster already
    // Just hoping that we will already reach the end of the directory before
    // that
  }
  return SYS_RET_UNIDENTIFIED_ERROR;
}

SYS_RET fat16_find_in_dir(fat16_context_t *context, char *file_to_find,
                          fat16_dir_entry_t *subdir,
                          fat16_dir_entry_t *out_dir_entry) {
  if(!(subdir->attributes & FAT_ATTRIB_SUBDIR)) {
    return SYS_RET_NOT_FOUND;
  }
  uint32_t fat_chain[1024];
  uint32_t cluster = CLUSTER_OF(*subdir);
  fat16_read_cluster_fat_chain(context, cluster, fat_chain);

  BOOLEAN found = FALSE;
  uint32_t current_cluster = 0;
  while (!found) {
    uint32_t max_entries_per_cluster =
        (context->boot_sector.sectorsPerCluster * context->device->sectorSize) /
        sizeof(fat16_dir_entry_t);
    fat16_dir_entry_t *entries_retrieved = (fat16_dir_entry_t *)context->buffer;

    fat16_read_directory(context,
                         (fat_chain[current_cluster] - 2) +
                             context->data_area_start_cluster,
                         entries_retrieved, max_entries_per_cluster);
    size_t i;
    for (i = 0; i < max_entries_per_cluster; i++) {
      if (entries_retrieved[i].filename[0] == '\0') {
        // directory end, file not found
        return SYS_RET_NOT_FOUND;
      } else if ((uint8_t)entries_retrieved[i].filename[0] == 0xE5) {
        // this entry is deleted, skip
        continue;
      }
      int cmp = kaos_strncmp(entries_retrieved[i].filename, file_to_find, 11);
      if (cmp == 0) {
        kaos_memcpy(out_dir_entry, &entries_retrieved[i],
                    sizeof(fat16_dir_entry_t));
        return SYS_RET_NO_ERROR;
      }
    }
    // We have traversed all entries in the cluster, go to next one now
    current_cluster++;
    if(fat_chain[current_cluster] == 0xFFFF) return SYS_RET_NOT_FOUND;
  }

  return SYS_RET_NO_ERROR;
}

SYS_RET fat16_find_file_entry(fat16_context_t *context, char *path,
                              fat16_dir_entry_t *out_entry) {
  char path_copy[1024];
  char *path_copy_ptr = path_copy;
  kaos_strncpy(path_copy, path, 1024);

  uint32_t max_entries_per_cluster =
      (context->boot_sector.sectorsPerCluster * context->device->sectorSize) /
      sizeof(fat16_dir_entry_t);
  SYS_RET ret;
  fat16_dir_entry_t *entries_retrieved = (fat16_dir_entry_t *)context->buffer;

  fat16_read_root_dir(context, entries_retrieved, max_entries_per_cluster, 0);
  BOOLEAN found = FALSE;
  BOOLEAN in_root_dir = TRUE;
  while (!found) {
    size_t i = 0;
    BOOLEAN is_subdir = TRUE;
    char path_fragment[12];
    kaos_memset(path_fragment, ' ', 12);
    path_fragment[11] = '\0';
    while (TRUE) {
      if (path_copy_ptr[0] == '/' && i != 0) {
        path_fragment[i + 1] = '\0';
        break;
      } else if (path_copy_ptr[0] == '\0') {
        path_fragment[i + 1] = '\0';
        is_subdir = FALSE;
        break;
      } else if(path_copy_ptr[0] != '/') {
        path_fragment[i] = path_copy_ptr[0];
        i++;
      }
      path_copy_ptr++;
    }

    char cmp_filename[12];
    kaos_memset(cmp_filename, ' ', 11);
    cmp_filename[11] = '\0';
    fat16_convert_filename(path_fragment, cmp_filename);
    fat16_dir_entry_t entry;
    if (in_root_dir) {
      ret = fat16_find_in_root_dir(context, cmp_filename, &entry);
      in_root_dir = FALSE;
    } else {
      ret = fat16_find_in_dir(context, cmp_filename, &entry, &entry);
      if(!is_subdir) {
        kaos_memcpy(out_entry, &entry, sizeof(fat16_dir_entry_t));
        return SYS_RET_NO_ERROR;
      }
    }
    if(ret != SYS_RET_NO_ERROR) {
      return ret;
    }
  }
  return SYS_RET_UNIDENTIFIED_ERROR;
}

SYS_RET fat16_read_cluster_fat_chain(fat16_context_t *context, uint32_t cluster,
                                     uint32_t *out_chain) {
  uint32_t cluster_size =
      context->boot_sector.sectorsPerCluster * context->device->sectorSize;
  uint32_t which_cluster = (cluster * 2) / cluster_size;
  uint16_t *fat = (uint16_t *)context->buffer;
  uint32_t current_cluster = cluster;
  size_t i = 0;
  do {
    out_chain[i++] = current_cluster;
    which_cluster = (current_cluster * 2) / cluster_size;
    fat16_read_cluster(
        context,
        (context->fat_start / context->boot_sector.sectorsPerCluster) +
            which_cluster,
        context->buffer);
    current_cluster = fat[current_cluster % cluster_size];
    kaos_printf("Cluster: %d\n", current_cluster);
    if(current_cluster == 0) {
      while(1);
    }
  } while (current_cluster != 0xFFFF);
  out_chain[i] = 0xFFFF;
  return SYS_RET_NO_ERROR;
}

SYS_RET fat16_read_file(fat16_context_t *context, fat16_dir_entry_t *entry,
                        void *buffer, size_t len) {
  if (entry->attributes & (FAT_ATTRIB_SUBDIR | FAT_ATTRIB_VOLUME_LABEL)) {
    return SYS_RET_MALFORMED;
  }

  uint32_t cluster = CLUSTER_OF(*entry);
  uint32_t length = entry->file_size > len ? len : entry->file_size;
  if(length == 0) {
    return SYS_RET_NO_ERROR;
  }
  kaos_printf("File size: %d\n", length);
  uint32_t cluster_chain[128];
  fat16_read_cluster_fat_chain(context, cluster, cluster_chain);
  kaos_printf("Cluster 0: %d\n", cluster_chain[0]);

  size_t i;
  uint32_t cluster_size =
      context->boot_sector.sectorsPerCluster * context->device->sectorSize;
  for (i = 0; i < length; i += cluster_size) {
    fat16_read_cluster(context,
                       (cluster_chain[i / cluster_size] - 2) +
                           context->data_area_start_cluster,
                       context->buffer);
    kaos_memcpy(((uint8_t *)buffer) + i, context->buffer, cluster_size);
    kaos_printf("Read done at: %d\n", i);
  }
  fat16_read_cluster(context,
                     (cluster_chain[(i / cluster_size) + 1] - 2) +
                         context->data_area_start_cluster,
                     context->buffer);
  kaos_memcpy(((uint8_t *)buffer) + i, context->buffer, length % cluster_size);
  return SYS_RET_NO_ERROR;
}

SYS_RET fat16_destroy_context(fat16_context_t *context) {
  return mm_free(context->buffer);
}

SYS_RET fat16_read_cluster(fat16_context_t *context, uint32_t cluster,
                           void *out_buffer) {
  block_device_t *device = context->device;
  uint32_t sector = cluster * context->boot_sector.sectorsPerCluster;
  uint8_t *buffer = (uint8_t *)out_buffer;
  uint32_t i;
  for (i = 0; i < context->boot_sector.sectorsPerCluster; i++) {
    block_dev_read(device, sector + i, buffer + (device->sectorSize * i));
  }
  return SYS_RET_NO_ERROR;
}
