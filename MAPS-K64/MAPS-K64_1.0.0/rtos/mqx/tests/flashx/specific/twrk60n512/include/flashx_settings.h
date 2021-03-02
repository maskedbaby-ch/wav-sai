#ifndef __flashx_settings_h__
#define __flashx_settings_h__


#include "util.h"


/* cfm settings */
#define CASE3_CFM_ENABLE            (1)
#define CASE3_CFM_OFFSET            (0x400)
#define CASE3_CFM_SIZE              (16)


/* specific settings of test case no. 4 */
#define CASE4_HALF_PARTIAL          ( \
                                    (CASE4_USE_CACHED_BUFFERED     << (CASE4_SHIFT_SIZE * 0)) | \
                                    (CASE4_USE_UNCACHED            << (CASE4_SHIFT_SIZE * 1)) | \
                                    (CASE4_USE_CACHED_UNBUFFERED   << (CASE4_SHIFT_SIZE * 2)) | \
                                    (CASE4_USE_ERASE_SECTOR        << (CASE4_SHIFT_SIZE * 3)) | \
                                    (CASE4_USE_UNCACHED            << (CASE4_SHIFT_SIZE * 4))   \
                                    )

#define CASE4_ONE_PARTIAL           ( \
                                    (CASE4_USE_CACHED_BUFFERED     << (CASE4_SHIFT_SIZE * 0)) | \
                                    (CASE4_USE_UNCACHED            << (CASE4_SHIFT_SIZE * 1)) | \
                                    (CASE4_USE_CACHED_UNBUFFERED   << (CASE4_SHIFT_SIZE * 2)) | \
                                    (CASE4_USE_ERASE_SECTOR        << (CASE4_SHIFT_SIZE * 3)) | \
                                    (CASE4_USE_UNCACHED            << (CASE4_SHIFT_SIZE * 4))   \
                                    )

#define CASE4_THREE_FULL            ( \
                                    (CASE4_USE_CACHED_BUFFERED     << (CASE4_SHIFT_SIZE * 0)) | \
                                    (CASE4_USE_UNCACHED            << (CASE4_SHIFT_SIZE * 1)) | \
                                    (CASE4_USE_CACHED_UNBUFFERED   << (CASE4_SHIFT_SIZE * 2)) | \
                                    (CASE4_USE_ERASE_SECTOR        << (CASE4_SHIFT_SIZE * 3)) | \
                                    (CASE4_USE_UNCACHED            << (CASE4_SHIFT_SIZE * 4))   \
                                    )

#define CASE4_THREE_PARTIAL         ( \
                                    (CASE4_USE_CACHED_BUFFERED     << (CASE4_SHIFT_SIZE * 0)) | \
                                    (CASE4_USE_UNCACHED            << (CASE4_SHIFT_SIZE * 1)) | \
                                    (CASE4_USE_CACHED_UNBUFFERED   << (CASE4_SHIFT_SIZE * 2)) | \
                                    (CASE4_USE_ERASE_SECTOR        << (CASE4_SHIFT_SIZE * 3)) | \
                                    (CASE4_USE_UNCACHED            << (CASE4_SHIFT_SIZE * 4))   \
                                    )


/* specific settings of test case no. 5 */
#define CASE5_HALF_PARTIAL          (1)
#define CASE5_ONE_PARTIAL           (1)
#define CASE5_ONE_FULL              (1)
#define CASE5_TWO_PARTIAL           (1)
#define CASE5_THREE_FULL            (1)


/* enable/disable test case */
#if LINKER_USED == LINKER_USED_INTRAM
#   define ENABLE_CASE1             (1)
#   define ENABLE_CASE2             (1)
#   define ENABLE_CASE3             (1)
#   define ENABLE_CASE4             (1)
#   define ENABLE_CASE5             (1)
#   define ENABLE_CASE6             (1)
#   define ENABLE_CASE7             (1)
#elif LINKER_USED == LINKER_USED_INTFLASH 
#   define ENABLE_CASE1             (1)
#   define ENABLE_CASE2             (1)
#   define ENABLE_CASE3             (0)
#   define ENABLE_CASE4             (1)
#   define ENABLE_CASE5             (1)
#   define ENABLE_CASE6             (1)
#   define ENABLE_CASE7             (1)
#else
#   error "Unsupported LINKER_USED value"
#endif


// override settings for: 
// cw10 - because of openbdm problem and mass erase
#if defined(__CODEWARRIOR__)
#   undef  ENABLE_CASE3
#   define ENABLE_CASE3             (0)
#endif


#endif
