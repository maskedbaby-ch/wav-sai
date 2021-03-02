#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <string.h>
#include <flashx.h>
#include "flashx_settings.h"


#if LINKER_USED == LINKER_USED_INTRAM

/* flashx files definition */
/* 1 M = 16 KB + 2 × 48 KB + 16 KB + 2 × 64 KB + 2 × 128 KB + 2 × 256 KB*/
const FLASHX_FILE_BLOCK _test_flashx_file_blocks[] = {
   /* workspace rw */
   { "work_rw",                     0x00000000,   (0x00100000 - 1) }, 
   /* workspace mirror rw */
   { "mirror_rw",                   0x00000000,   (0x00100000 - 1) }, 
   /* ro part of flash tc 6 */
   { "work_ro",                     0x00000000,   (0x00100000 - 1) }, 
   /* part of flash affected by FLASH_IOCTL_ERASE_CHIP */
   { "erased_ro",                   0x00000000,   (0x00100000 - 1) }, 
   /* whole flash to test hw blocks */
   { "whole_ro",                    0x00000000,   (0x00100000 - 1) }, 
   { NULL , 0, 0 } 
};

#else

#error "Unsupported LINKER_USED value"

#endif


const FLASHX_INIT_STRUCT _test_flashx_init = {
    0x00000000, /* BASE_ADDR should be 0 for internal flashes */
    _mpxs20_block_map, /* HW block map for PXS20 devices */
    _test_flashx_file_blocks, /* Files on the device defined by the BSP */
    &_flashx_c90_if, /* Interface for low level driver */
    8,  /* For external devices, data lines for the flash. Not used for internal flash devices. */
    1,  /* Number of parallel external flash devices. Not used for internal flash devices. */
    0,  /* 0 if the write verify is requested, non-zero otherwise */
    NULL /* low level driver specific data */
};


/* cfm array  */
const uint32_t * cfm = NULL;
