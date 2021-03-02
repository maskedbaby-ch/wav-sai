#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <string.h>
#include <flashx.h>
#include "flashx_settings.h"


#if LINKER_USED == LINKER_USED_INTFLASH

/* flashx settings for Flash target */
const FLASHX_FILE_BLOCK _test_flashx_file_blocks[] = {
   { "cfm",                         0x00000000,   (0x00000800 - 1) }, 
   /* workspace rw */
   { "work_rw",                     0x00040000,   (0x00080000 - 1) }, 
   /* workspace mirror rw */
   { "mirror_rw",                   0x00040000,   (0x00080000 - 1) }, 
   /* ro part of flash tc 6 */
   { "work_ro",                     0x00040000,   (0x00080000 - 1) }, 
   /* part of flash affected by FLASH_IOCTL_ERASE_CHIP */
   { "erased_ro",                   0x00000000,   (0x00080000 - 1) }, 
   /* whole flash to test hw blocks */
   { "whole_ro",                    0x00000000,   (0x00080000 - 1) }, 
   { NULL , 0, 0 } 
};

#elif LINKER_USED == LINKER_USED_INTRAM

/* flashx settings for Ram target */
const FLASHX_FILE_BLOCK _test_flashx_file_blocks[] = {
   { "cfm",                         0x00000000,   (0x00000800 - 1) }, 
   /* workspace rw */
   { "work_rw",                     0x00000800,   (0x00080000 - 1) }, 
   /* workspace mirror rw */
   { "mirror_rw",                   0x00000800,   (0x00080000 - 1) }, 
   /* ro part of flash tc 6 */
   { "work_ro",                     0x00000000,   (0x00080000 - 1) }, 
   /* part of flash affected by FLASH_IOCTL_ERASE_CHIP */
   { "erased_ro",                   0x00000000,   (0x00080000 - 1) }, 
   /* whole flash to test hw blocks */
   { "whole_ro",                    0x00000000,   (0x00080000 - 1) }, 
   { NULL , 0, 0 } 
};

#else

#error "Unsupported LINKER_USED value"

#endif


/* flashx init struct */
const FLASHX_INIT_STRUCT _test_flashx_init = {
    0x00000000, /* BASE_ADDR should be 0 for internal flashes */
    _flashx_kinetisN_block_map, /* HW block map for KinetisN devices */
    _test_flashx_file_blocks, /* Files on the device defined by the BSP */
    &_flashx_ftfl_if, /* Interface for low level driver */
    32, /* For external devices, data lines for the flash. Not used for internal flash devices. */
    1,  /* Number of parallel external flash devices. Not used for internal flash devices. */
    0,  /* 0 if the write verify is requested, non-zero otherwise */
    NULL /* low level driver specific data */
};

/* cfm array  */
const uint32_t cfm[4] = { 
    0xffffffff, 
    0xffffffff, 
    0xffffffff, 
    0xfffffffe 
};
