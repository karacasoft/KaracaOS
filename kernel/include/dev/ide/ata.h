#ifndef __ATA_H__
#define __ATA_H__ 1

#include <stdint.h>

#define ATA_REG_DATA_OFFSET     (0x00)
#define ATA_REG_ERROR_OFFSET    (0x01)
#define ATA_REG_FEATURES_OFFSET (0x01)
#define ATA_REG_SECCOUNT_OFFSET (0x02)

#define ATA_REG_LBA0_7_OFFSET   (0x03)
#define ATA_REG_LBA8_15_OFFSET  (0x04)
#define ATA_REG_LBA16_23_OFFSET (0x05)
#define ATA_REG_LBA24_27_OFFSET (0x06)

#define ATA_REG_SECNUM_OFFSET       (0x03)
#define ATA_REG_CYLINDERLOW_OFFSET  (0x04)
#define ATA_REG_CYLINDERHIGH_OFFSET (0x05)
#define ATA_REG_DRIVEHEAD_OFFSET    (0x06)

#define ATA_REG_STATUS_OFFSET       (0x07)
#define ATA_REG_COMMAND_OFFSET      (0x07)



#define ATA_CMD_IDENTIFY        (0xEC)
#define ATA_CMD_READ            (0x20)
#define ATA_CMD_WRITE           (0x30)



#define ATA_STATUS_BSY_MASK     (0x80)
#define ATA_STATUS_DRDY_MASK    (0x40)
#define ATA_STATUS_DWF_MASK     (0x20)
#define ATA_STATUS_DSC_MASK     (0x10)
#define ATA_STATUS_DRQ_MASK     (0x08)
#define ATA_STATUS_CORR_MASK    (0x04)
#define ATA_STATUS_IDX_MASK     (0x02)
#define ATA_STATUS_ERR_MASK     (0x01)


#define ATA_ERROR_BBK_MASK      (0x80)
#define ATA_ERROR_UNC_MASK      (0x40)
#define ATA_ERROR_MC_MASK       (0x20)
#define ATA_ERROR_IDNF_MASK     (0x10)
#define ATA_ERROR_ABRT_MASK     (0x08)
#define ATA_ERROR_MCR_MASK      (0x04)
#define ATA_ERROR_TK0NF_MASK    (0x02)
#define ATA_ERROR_AMNF_MASK     (0x01)



#define ATA_CAPABILITY_DMA_SUPPORTED_MASK    (0x100)
#define ATA_CAPABILITY_LBA_SUPPORTED_MASK    (0x200)
#define ATA_CAPABILITY_IORDY_DISABLED_MASK   (0x400)
#define ATA_CAPABILITY_IORDY_SUPPORTED_MASK  (0x800)


typedef struct __ata_drive_identify_buffer ata_drive_identify_buffer_t;

struct __ata_drive_identify_buffer {
    uint16_t generalInfo; //0
    uint16_t __numCylinders;
    uint16_t reserved0;
    uint16_t __numHeads;
    uint16_t __bytesPerTrack;
    uint16_t __bytesPerSector;
    uint16_t __obsolete0;
    uint16_t vendorUnique0[2];
    uint16_t __retired0;
    char serialNumber[20]; // 10-19
    uint16_t __bufferType;
    uint16_t __bufferSize; // In 512 byte increments
    uint16_t __numECCbytes;
    char firmwareRev[8];
    char modelNum[40]; // 27-46
    uint8_t __vendorUnique1;
    uint8_t maxSectorsPerRWMultiple;
    uint16_t trustedComputingOptions;
    uint16_t capabilities0;
    uint16_t capabilities1;
    uint16_t __obsolete[2];
    uint16_t freeFallCtrlSens;
    uint16_t __numCurrentCylinders;
    uint16_t __numCurrentHeads;
    uint16_t __numCurrentSectorPerTrack;
    uint32_t __totalCapacityInSectors;
    uint16_t multipleSectorSetting;
    uint32_t totalAddressableSectors;
    uint16_t __singleWordDMAMode;
    uint16_t multiWordDMAMode; // 63
    uint16_t pioModeSupport;
    uint16_t minMultiwordTransferCycleTime;
    uint16_t recommendedMultiwordTransferCycleTime;
    uint16_t minPIOCycleTime;
    uint16_t minPIOCycleTimeIORDY;
    uint16_t additionalSupported;
    uint16_t reserved1;
    uint16_t identifyPacketReserved[4];
    uint16_t queueDepth; // 75
    uint16_t sataCapabilities;
    uint16_t sataAdditionalCapabilities;
    uint16_t sataFeaturesSupported;
    uint16_t sataFeaturesEnabled;
    uint16_t majorVersionNumber;
    uint16_t minorVersionNumber;
    uint16_t cmdsAndFeatureSetsSupported0;
    uint16_t cmdsAndFeatureSetsSupported1;
    uint16_t cmdsAndFeatureSetsSupported2; // 84
    uint16_t cmdsAndFeatureSetsSupportedOrEnabled0;
    uint16_t cmdsAndFeatureSetsSupportedOrEnabled1;
    uint16_t cmdsAndFeatureSetsSupportedOrEnabled2;
    uint16_t ultraDMAModes;
    uint16_t timeRequiredForNormalEraseMode;
    uint16_t timeRequiredForEnhancedEraseMode; // 90
    uint8_t reserved2;
    uint8_t currentAPMLevel;
    uint16_t masterPasswordIdentifier;
    uint16_t hardResetResults;
    uint16_t currentAAMValue;
    uint16_t minRequestSize;
    uint16_t streamingTransferTime;
    uint16_t streamingAccessLatency;
    uint32_t streamingPerformanceGranularity;
    uint64_t nrAddressableLogicalSectors;
    uint16_t streamingTransferTimePIO;
    uint16_t maxBlocksPerDataSetManagementCmd;
    uint16_t physSectorSizeLogicalSectorSizeRatio; // 106
    uint16_t interSeekDelay;
    char worldWideName[8];
    uint16_t reserved3[4];
    uint16_t __obsolete1;
    uint32_t logicalSectorSize;
    uint16_t cmdsAndFeatureSetsSupported3;
    uint16_t cmdsAndFeatureSetsSupportedOrEnabled3; // 120
    uint16_t reserved4[6];
    uint16_t __obsolete2;
    uint16_t securityStatus;
    uint16_t vendorSpecific[31];
    uint16_t reserved5[8]; // 160-167
    uint16_t deviceNominalFormFactor;
    uint16_t dataSetManagementCmdSupport;
    char additionalProductIdentifier[8];
    uint16_t reserved6[2];
    char currentMediaSerialNumber[60]; // 176-205
    uint16_t sctCommandTransport;
    uint16_t reserved7[2];
    uint16_t sectorAlignment;
    uint32_t writeReadVerifySectorMode3Count;
    uint32_t writeReadVerifySectorMode2Count;
    uint16_t __obsolete3[3];
    uint16_t nominalMediaRotationRate;
    uint16_t reserved8;
    uint16_t __obsolete4; // 219
    uint8_t reserved9;
    uint8_t writeReadVerifyFeatureSetCurrentMode;
    uint16_t reserved10;
    uint16_t transportMajorVersionNumber;
    uint16_t transportMinorVersionNumber;
    uint16_t reserved11[6];
    uint64_t nrAddressableSectorsExtended;
    uint16_t downloadMicroCodeInfo[2];
    uint16_t reserved12[19];
    uint16_t integrityWord;
} __attribute__((packed));


#endif // __ATA_H__