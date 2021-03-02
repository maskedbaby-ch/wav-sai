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
* $FileName: test.c$
* $Version : 3.8.7.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file tests the search capabilities in MFS
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <mfs.h>
#include <mfs_prv.h>

#include "test.h"
#include "util.h"
#include "mfs_init.h"

//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test
void tc_1_main_task(void);
void tc_2_main_task(void);
void test_task(void);


TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
#if defined(TI320C30D) || defined(TI320C40D)
   { MAIN_TASK,      main_task,       200L,  11L, "Main",  MQX_AUTO_START_TASK},
#else
   { MAIN_TASK,      main_task,      3000L,  11L, "Main",  MQX_AUTO_START_TASK},
#endif
   { 0L,             0L,             0L,     0L, 0L,      0L }
};


MFS_FORMAT_DATA MFS_format =
{
   /* PHYSICAL_DRIVE;    */   PHYSICAL_DRI,
   /* MEDIA_DESCRIPTOR;  */   MEDIA_DESC,
   /* BYTES_PER_SECTOR;  */   BYTES_PER_SECT,
   /* SECTORS_PER_TRACK; */   SECTS_PER_TRACK,
   /* NUMBER_OF_HEADS;   */   NUM_OF_HEADS,
   /* NUMBER_OF_SECTORS; */   NUM_SECTORS,
   /* HIDDEN_SECTORS;    */   HIDDEN_SECTS,
   /* RESERVED_SECTORS;  */   RESERVED_SECTS
};

char *File_array[NUM_FILES] = {
   FILE1_STR,
   FILE2_STR,
   FILE3_STR,
   FILE4_STR,
   FILE5_STR,
   FILE6_STR,
   FILE7_STR,
   FILE8_STR,
   FILE9_STR,
   FILE10_STR,
   FILE11_STR
};

FTEST_SRCH_RES_STRUCT Search_results[NUM_PATTERNS] = 
{
   { SRC1_STR,  SFILE1_STR,  SFILE2_STR,  SFILE3_STR, SFILE4_STR, SFILE5_STR, SFILE6_STR, SFILE7_STR, SFILE8_STR, SFILE9_STR, SFILE10_STR, SFILE11_STR, 0},
   { SRC2_STR,  SFILE1_STR,  SFILE2_STR,  SFILE3_STR, SFILE4_STR, SFILE5_STR, SFILE6_STR, SFILE7_STR, SFILE8_STR, SFILE9_STR, SFILE10_STR, SFILE11_STR, 0},
   { SRC3_STR,  SFILE3_STR,  SFILE4_STR,  SFILE5_STR, 0,          0,          0,          0,          0,          0,          0,           0,           0},
   { SRC4_STR,  SFILE1_STR,  SFILE2_STR,  SFILE4_STR, 0,          0,          0,          0,          0,          0,          0,           0,           0},
   { SRC5_STR,  SFILE3_STR,  SFILE6_STR,  0,          0,          0,          0,          0,          0,          0,          0,           0,           0},
   { SRC6_STR,  SFILE5_STR,  0,           0,          0,          0,          0,          0,          0,          0,          0,           0,           0},
   { SRC7_STR,  SFILE2_STR,  0,           0,          0,          0,          0,          0,          0,          0,          0,           0,           0},
   { SRC8_STR,  SFILE1_STR,  SFILE2_STR,  SFILE3_STR, SFILE4_STR, SFILE5_STR, SFILE6_STR, SFILE7_STR, SFILE8_STR, SFILE9_STR, SFILE10_STR, SFILE11_STR, 0},
   { SRC9_STR,  SFILE3_STR,  SFILE4_STR,  SFILE5_STR, SFILE6_STR, SFILE7_STR, SFILE9_STR, 0,          0,          0,          0,           0,           0},
   { SRC10_STR, SFILE1_STR,  SFILE2_STR,  SFILE3_STR, SFILE4_STR, SFILE5_STR, SFILE6_STR, SFILE7_STR, SFILE8_STR, SFILE9_STR, SFILE10_STR, SFILE11_STR, 0},
   { SRC11_STR, SFILE3_STR,  0,           0,          0,          0,          0,          0,          0,          0,          0,           0,           0},
   { SRC12_STR, SFILE3_STR,  0,           0,          0,          0,          0,          0,          0,          0,          0,           0,           0},
   { SRC13_STR, SFILE3_STR,  SFILE4_STR,  SFILE5_STR, 0,          0,          0,          0,          0,          0,          0,           0,           0},
   { SRC14_STR, SFILE1_STR,  SFILE2_STR,  SFILE3_STR, SFILE4_STR, SFILE5_STR, SFILE6_STR, SFILE7_STR, SFILE8_STR, SFILE9_STR, SFILE10_STR, SFILE11_STR, 0},
   { SRC15_STR, SFILE6_STR,  0,           0,          0,          0,          0,          0,          0,          0,          0,           0,           0},
   { SRC16_STR, SFILE6_STR,  0,           0,          0,          0,          0,          0,          0,          0,          0,           0,           0},
   { SRC17_STR, SFILE8_STR,  0,           0,          0,          0,          0,          0,          0,          0,          0,           0,           0},
   { SRC18_STR, SFILE8_STR,  SFILE9_STR,  0,          0,          0,          0,          0,          0,          0,          0,           0,           0},
   { SRC19_STR, SFILE11_STR, 0,           0,          0,          0,          0,          0,          0,          0,          0,           0,           0},
   { SRC20_STR, SFILE10_STR, SFILE11_STR, 0,          0,          0,          0,          0,          0,          0,          0,           0,           0},
   /*Start CR1300 & CR1313*/
   { SRC21_STR, SFILE8_STR, SFILE9_STR,   0,          0,          0,          0,          0,          0,          0,          0,           0,           0}
   /*End CR1300 & CR1313*/   
};

#if DISK_FOR_MFS_TEST == USB_DISK
	USB_FILESYSTEM_STRUCT_PTR usb_fs_handle;
#endif 

MQX_FILE_PTR      mfs_fd_ptr;

/*TASK*-------------------------------------------------------------------
*
* Task Name    : void tc_1_main_task(void)
* Comments     : Testing MFS search on USB disk
*
*END*----------------------------------------------------------------------*/

void tc_1_main_task(void)
{
#if DISK_FOR_MFS_TEST == USB_DISK
   #define MFS       	"c:"
   usb_fs_handle = (USB_FILESYSTEM_STRUCT_PTR)Init_MFS_USB();
   mfs_fd_ptr = usb_filesystem_handle(usb_fs_handle);
   test_task();
#endif
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : void tc_2_main_task(void)
* Comments     : Testing MFS search on RAM disk
*
*END*----------------------------------------------------------------------*/

void tc_2_main_task(void)
{
#if DISK_FOR_MFS_TEST == RAM_DISK 
   #define MFS       	"MFS:"
   mfs_fd_ptr = (MQX_FILE_PTR)Init_MFS_RAM(MFS_format.BYTES_PER_SECTOR * NUM_SECTORS);
   test_task();
#endif
}

void test_task(void)
{
   uint32_t                    error_code, i, j, k;
   MQX_FILE_PTR               file_ptr;
   MFS_SEARCH_DATA            search_data;
   MFS_SEARCH_PARAM           search;
   char                   *filename;

   _int_install_unexpected_isr();

#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mfs_fd_ptr != NULL), "Test #1 Creating File System");
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (mfs_fd_ptr != NULL), "Test #1 Creating File System");
#endif
   if(!mfs_fd_ptr){
   	return;
   }   
   filename = _mem_alloc_system_zero(PATHNAME_SIZE + 1);
      
   /* Creating subdir */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CREATE_SUBDIR,(uint32_t *)SUBDIR_NAME);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.0 Creating subdir");
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.0 Creating subdir");
#endif
   if (error_code != MFS_NO_ERROR) {
      return;
   }
   
   /* Changing to subdir */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CHANGE_CURRENT_DIR,(uint32_t *)SUBDIR_NAME);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.1 Changing to subdir");
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.1 Changing to subdir");
#endif
   if (error_code != MFS_NO_ERROR) {
      return;
   }
   
   /* Create Files */
   for (i = 0; i < NUM_FILES; i++) {
      sprintf(filename, "%s%s", MFS, File_array[i]);
      file_ptr = fopen(filename, "n+");
#if DISK_FOR_MFS_TEST == USB_DISK
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file_ptr != NULL), "Test #1 3.1 Create new file");
#else
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file_ptr != NULL), "Test #1 3.1 Create new file");
#endif
      if (!file_ptr) {
         return;
      } /* Endif */
      error_code = fclose(file_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 3.2 Close file");
#else
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 3.2 Close file");
#endif
      if (error_code) {
         return;
      } /* Endif */
   } /* Endfor */

   /* List files */
   search.ATTRIBUTE       = MFS_SEARCH_ARCHIVE;
   search.SEARCH_DATA_PTR = &search_data;
   for (i = 0; i < NUM_PATTERNS; i++) {
      search.WILDCARD = Search_results[i].PATTERN;
      error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_FIRST_FILE,(uint32_t *)&search);
#if DISK_FOR_MFS_TEST == USB_DISK
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.3 List files");
#else
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.3 List files");
#endif
      if (error_code != MFS_NO_ERROR) {
         return;      
      } /* Endif */

      k = strncmp(search_data.NAME, Search_results[i].RESULTS[0], FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (k==0), "Test #1 3.3 List files");
#else
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, (k==0), "Test #1 3.3 List files");
#endif
      if (k) {
         return;
      }
      /* Endif */
      for (j = 1; j < (NUM_FILES + 1); j++) {
         error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_NEXT_FILE,(uint32_t *)&search_data);                
         if (!Search_results[i].RESULTS[j]) {
#if DISK_FOR_MFS_TEST == USB_DISK
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_FILE_NOT_FOUND), "Test #1 3.3 List files");
#else
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_FILE_NOT_FOUND), "Test #1 3.3 List files");
#endif
            if (error_code != MFS_FILE_NOT_FOUND) {
               /* Found a file or got an unexpected error code */
               return;
            } /* Endif */
         } else {
#if DISK_FOR_MFS_TEST == USB_DISK
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.3 List files");
#else
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.3 List files");
#endif
            if (error_code != MFS_NO_ERROR) {
               return;
            } /* Endif */
            k = strncmp(search_data.NAME, Search_results[i].RESULTS[j], FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, (k==0), "Test #1 3.3 List files Found file");
#else
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, (k==0), "Test #1 3.3 List files Found file");
#endif
            if (k) {
               return;   
            } /* Endif */
         } /* Endif */
      } /* Endfor */
   } /* Endfor */
} /* Endbody */

/******************** End Testcases ********************/

/******************** Register tests *******************/
EU_TEST_SUITE_BEGIN(suite_mfs)
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_TEST_CASE_ADD(tc_1_main_task, "- Testing searching on USB disk"),
#else   
   EU_TEST_CASE_ADD(tc_2_main_task, "- Testing searching on RAM disk"),
#endif   
EU_TEST_SUITE_END(suite_mfs)

EU_TEST_REGISTRY_BEGIN()
   EU_TEST_SUITE_ADD(suite_mfs, "Testing MFS searching")
EU_TEST_REGISTRY_END()
/******************** End Register *********************/        
         
         
 /*TASK*-------------------------------------------------------------------
* 
* Task Name    : main_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t dummy
   )
{ /* Body */
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* Endbody */  
         
/* EOF */
