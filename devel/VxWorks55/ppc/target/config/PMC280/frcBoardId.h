#ifndef _FRCBOARDID_H_
#define _FRCBOARDID_H


/* Board information is stored in serial EEPROM with address = 0xa4 */
#define BRDID_EE_ADDR   0xa4

/* Number of bytes reserved for Board ID */
#define MAX_BOARD_ID_SIZE 3

/* Offset in serial EEPROM from where board identifier is stored */
#define BRDID_OFFSET    0x1FE0
#define VARIANT_INFO1   0x1FE1
#define VARIANT_INFO2   0x1FE2

/* Define the bytes which contain information */
#define PCB_REV_BYTE        0x00
#define BOM_REV_BYTE        0x00
#define FLASH_ORG_BYTE      0x00
#define FLASH_SIZE_BYTE     0x01
#define CS0_MEM_SIZE_BYTE   0x01
#define EXT_SRAM_BYTE       0x01
#define VARIANT_BYTE        0x02
#define CS1_MEM_SIZE_BYTE   0x02

/* Define the byte masks for information */
#define PCB_REV_MASK        0x07
#define BOM_REV_MASK        0x38
#define FLASH_ORG_MASK      0xC0
#define FLASH_SIZE_MASK     0x07
#define CS0_MEM_SIZE_MASK   0x38
#define EXT_SRAM_MASK       0xC0
#define VARIANT_MASK        0x0F
#define CS1_MEM_SIZE_MASK   0x70

#define PCB_REV_E0      0x0
#define PCB_REV_B       0x1
#define PCB_REV_C       0x2
#define PCB_REV_D       0x3
#define PCB_REV_E       0x4
#define PCB_REV_F       0x5
#define PCB_REV_G       0x6

#define BOM_REV_0       (0x0 << 3 )
#define BOM_REV_1       (0x1 << 3 )
#define BOM_REV_2       (0x2 << 3 )
#define BOM_REV_3       (0x3 << 3 )
#define BOM_REV_4       (0x4 << 3 )
#define BOM_REV_5       (0x5 << 3 )
#define BOM_REV_6       (0x6 << 3 )
#define BOM_REV_7       (0x7 << 3 )

#define FLASH_ORG_EQ    (0x00 << 6)
#define FLASH_ORG_B0    (0x01 << 6)
#define FLASH_ORG_B1    (0x02 << 6)


#define FLASH_SIZE_0MB   0x0
#define FLASH_SIZE_16MB  0x1
#define FLASH_SIZE_32MB  0x2
#define FLASH_SIZE_64MB  0x3
#define FLASH_SIZE_128MB 0x4

#define CS0_MEM_SIZE_128MB  (0x0 << 3 )
#define CS0_MEM_SIZE_256MB  (0x1 << 3 )
#define CS0_MEM_SIZE_512MB  (0x2 << 3 )
#define CS0_MEM_SIZE_1GB    (0x3 << 3 )
#define CS0_MEM_SIZE_2GB    (0x4 << 3 )

#define EXT_SRAM_0MB     (0x0 << 6) 
#define EXT_SRAM_1MB     (0x1 << 6) 
#define EXT_SRAM_2MB     (0x2 << 6) 

#define VARIANT_A       0x0
#define VARIANT_B       0x1
#define VARIANT_C       0x2
#define VARIANT_D       0x3
#define VARIANT_E       0x4
#define VARIANT_F       0x5
#define VARIANT_G       0x6
#define VARIANT_H       0x7
#define VARIANT_I       0x8
#define VARIANT_J       0x9
#define VARIANT_K       0xa
#define VARIANT_L       0xb
#define VARIANT_M       0xc
#define VARIANT_N       0xd
#define VARIANT_O       0xe
#define VARIANT_P       0xf

#define CS1_MEM_SIZE_0MB    (0x0 << 4 )
#define CS1_MEM_SIZE_128MB  (0x1 << 4 )
#define CS1_MEM_SIZE_256MB  (0x2 << 4 )
#define CS1_MEM_SIZE_512MB  (0x3 << 4 )
#define CS1_MEM_SIZE_1GB    (0x4 << 4 )
#define CS1_MEM_SIZE_2GB    (0x5 << 4 )

#endif /* _FRCBOARDID_H_ */
