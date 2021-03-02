#ifndef __test_h__
#define __test_h__


/* define names of flashx files and driver name */
#define FLASHX_DRIVER_NAME                  "flashx_test:"
#define FLASHX_CFM                          "flashx_test:cfm"
#define FLASHX_WORK_RW                      "flashx_test:work_rw"
#define FLASHX_MIRROR_RW                    "flashx_test:mirror_rw"
#define FLASHX_WORK_RO                      "flashx_test:work_ro"
#define FLASHX_ERASED_RO                    "flashx_test:erased_ro"
#define FLASHX_WHOLE_RO                     "flashx_test:whole_ro"

/* Test case 4 definition*/
#define CASE4_SHIFT_SIZE                    (4)
#define CASE4_SHIFT_VALUE(index)            ((index) * CASE4_SHIFT_SIZE)
#define CASE4_SHIFT_MASK                    (0xF)

#define CASE4_USE_END                       (0)
#define CASE4_USE_CACHED_BUFFERED           (1)
#define CASE4_USE_CACHED_UNBUFFERED         (2)
#define CASE4_USE_ERASE_SECTOR              (3)
#define CASE4_USE_UNCACHED                  (4)
#define CASE4_USE_UNCACHED_PASS             (5)

/*
 The parts of test case4.
 These settings should be overriden in specific "flashx_settings.h" 
 Default value (0) will cause disable test case part. 
 example settings: 
 #define CASE4_HALF_PARTIAL          ( \
     (CASE4_USE_CACHED_BUFFERED     << (CASE4_SHIFT_SIZE * 0)) | \
     (CASE4_USE_UNCACHED            << (CASE4_SHIFT_SIZE * 1)) | \
     (CASE4_USE_CACHED_UNBUFFERED   << (CASE4_SHIFT_SIZE * 2)) | \
     (CASE4_USE_ERASE_SECTOR        << (CASE4_SHIFT_SIZE * 3)) | \
     (CASE4_USE_UNCACHED            << (CASE4_SHIFT_SIZE * 4))   \
 )
 will enable test part CASE4_HALF_PARTIAL as commands execution order 
 CASE4_USE_CACHED_BUFFERED, CASE4_USE_UNCACHED, ... .
 8 execution 'commands' can be used. 
 */

// test case 4
// #define CASE4_HALF_PARTIAL              (0)
// #define CASE4_ONE_PARTIAL               (0)
// #define CASE4_THREE_FULL                (0)
// #define CASE4_THREE_PARTIAL             (0)

// test case 5
// #define CASE5_HALF_PARTIAL              (0)
// #define CASE5_ONE_PARTIAL               (0)
// #define CASE5_ONE_FULL                  (0)
// #define CASE5_TWO_PARTIAL               (0)
// #define CASE5_THREE_FULL                (0)



#endif
