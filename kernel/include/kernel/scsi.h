#ifndef __SCSI_H__
#define __SCSI_H__ 1

#include <stdint.h>

typedef __scsi_cdb6
{
    uint8_t opcode;
    uint8_t info[4];
    uint8_t control;
} scsi_cdb6_t;

typedef __scsi_cdb10
{
    uint8_t opcode;
    uint8_t info[8];
    uint8_t control;
} scsi_cdb10_t;

typedef __scsi_cdb12
{
    uint8_t opcode;
    uint8_t info[10];
    uint8_t control;
} scsi_cdb12_t;

typedef __scsi_cdb16
{
    uint8_t opcode;
    uint8_t info[14];
    uint8_t control;
} scsi_cdb16_t;

// TODO: add variable size CDB's

#endif