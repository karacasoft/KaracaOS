#ifndef __SYS_DEFS_H__
#define __SYS_DEFS_H__ 1

#include <stdint.h>


enum __SYS_RET {
  SYS_RET_NO_ERROR = 0,
  SYS_RET_UNIDENTIFIED_ERROR,
  SYS_RET_OUT_OF_RANGE,
  SYS_RET_NO_DEVICE,

  SYS_RET_NOT_FOUND,
  SYS_RET_UNAVAILABLE,

  SYS_RET_NOT_IMPLEMENTED,

  SYS_RET_MEM_ALLOC_ERROR,
};

// System information
#define SYSTEM_NAME "KaracaOS"

#define SYSTEM_VERSION 0

#define INTERRUPT_VECTOR_COUNT 256

// System data types
typedef uint8_t BOOLEAN;
#define TRUE (1)
#define FALSE (0)


typedef enum __SYS_RET SYS_RET;

#endif
