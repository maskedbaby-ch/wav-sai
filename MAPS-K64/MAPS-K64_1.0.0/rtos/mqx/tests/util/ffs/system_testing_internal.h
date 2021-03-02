/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved                       
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: $
* $Version : 3.8.1.0$
* $Date    : Jul-20-2012$
*
* Comments:
*
*   
*   
*
*END************************************************************************/

#ifndef _WL_SYTEM_TESTING_INTERNAL_H_
#define _WL_SYTEM_TESTING_INTERNAL_H_


#include "mfs.h"

#define MAX_DIR_NAME_LENGTH         50UL
#define NUM_DIR_TEST                20
typedef char dir_arrs_type[NUM_DIR_TEST][MAX_DIR_NAME_LENGTH] ;
typedef dir_arrs_type * dir_arrs_type_ptr;

extern uint32_t g_wl_log_level;
extern bool g_nfc_log_enable;

typedef struct
{
   MQX_FILE_PTR         FS_PTR;
   MFS_GET_LFN_STRUCT   LFN_DATA;
   MFS_SEARCH_DATA      SEARCH_DATA;
   MFS_SEARCH_PARAM     SEARCH;
   char                 LFN[PATHNAME_SIZE];
   uint32_t              MODE;
   bool              FIRST;
} DIR_STRUCT, * DIR_STRUCT_PTR;

extern MQX_FILE_PTR g_fd_ptr;


#ifdef __cplusplus 
extern  "C" {
#endif 

RtStatus_t system_test_initialize(bool isRepairWhenInitFailed, MQX_FILE_PTR nand_wl_ptr);
RtStatus_t system_test_initialize_drive_failed(MQX_FILE_PTR nand_wl_ptr);
RtStatus_t system_test_finalize(MQX_FILE_PTR nand_wl_ptr);

int32_t system_test_mfs_open(MQX_FILE_PTR nand_wl_ptr, char * filesystem_name, MQX_FILE_PTR * filesystem_handle, bool isFormatWhenOpenFailed);
int32_t system_test_mfs_close(MQX_FILE_PTR filesystem_handle, char * filesystem_name);
int32_t system_test_read_write_with_mfs_write_internal(MQX_FILE_PTR nand_wl_ptr,char * fsname, unsigned char * * write_buffer, int32_t * write_buffer_size, uint32_t num_files_write );
int32_t system_test_read_write_with_mfs_read_internal(MQX_FILE_PTR nand_wl_ptr, char * fsname, unsigned char * * read_buffer, int32_t * read_buffer_size, uint32_t num_files_write );
int32_t system_test_read_write_with_mfs_mkdir_internal(MQX_FILE_PTR nand_wl_ptr, char * fsname, dir_arrs_type_ptr dirs_arr, uint32_t num_dir, bool isNested);
/*int32_t system_test_read_write_with_mfs_readdir_internal_recursive(
   char * start_from_dir, dir_arrs_type_ptr dirs_arr, uint32_t * dir_idx, MQX_FILE_PTR fshandle);*/
int32_t system_test_read_write_with_mfs_readdir_internal( MQX_FILE_PTR nand_wl_ptr,
char * fsname, dir_arrs_type_ptr dirs_arr, uint32_t num_dir, bool isNested);
RtStatus_t system_test_force_repair_scan_bad(MQX_FILE_PTR nand_wl_ptr);

int32_t system_test_read_write_with_mfs_write(MQX_FILE_PTR nand_wl_ptr,char * fsname, unsigned char * write_buffer, int32_t write_buffer_size, uint32_t num_files_write);
int32_t system_test_read_write_with_mfs_read(MQX_FILE_PTR nand_wl_ptr,char * fsname, unsigned char * write_buffer, int32_t write_buffer_size, uint32_t num_files_write);

#ifdef __cplusplus    
}
#endif

#endif

/* EOF */
