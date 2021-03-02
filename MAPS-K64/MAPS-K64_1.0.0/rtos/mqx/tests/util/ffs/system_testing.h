#ifndef _WL_SYTEM_TESTING_H_
#define _WL_SYTEM_TESTING_H_

#include "system_testing_internal.h"


extern MQX_FILE_PTR g_fd_ptr;

#ifdef __cplusplus 
extern  "C" {
#endif 

  
void system_test_read_write_same_sector_00(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void system_test_read_write_multi_sector_01(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void system_test_read_write_with_mfs_00(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void system_test_read_write_with_mfs_01(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_read_write_04(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);

void st_init_shutdown_00(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_init_shutdown_01(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_init_shutdown_02(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_init_shutdown_03(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_init_shutdown_04(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_init_shutdown_05(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_init_shutdown_06(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_init_shutdown_07(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);

void st_an_init_shutdown_00(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_an_init_shutdown_01(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_an_init_shutdown_02(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_an_init_shutdown_04(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_an_init_shutdown_05(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_an_init_shutdown_06(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);

void st_an_read_write_00(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_an_read_write_01(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_an_read_write_02(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_an_read_write_03(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_an_read_write_05(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_an_read_write_06(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_an_read_write_07(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_an_read_write_08(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_an_read_write_09(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);
void st_an_read_write_10(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);

void st_performance_00(NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr);

#ifdef __cplusplus    
}
#endif

#endif

/* EOF */
