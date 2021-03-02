/**HEADER********************************************************************
* 
* Copyright (c) 2008-2013 Freescale Semiconductor;
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
* $Version : 3.8.5.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file tests the rename capabilities in MFS
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
void tc_3_main_task(void);
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
#if DISK_FOR_MFS_TEST == USB_DISK
	USB_FILESYSTEM_STRUCT_PTR usb_fs_handle;
#endif 

MQX_FILE_PTR               mfs_fd_ptr;

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  Test_dir_list
* Returned Value   : uint32_t error code
* Comments         :
*    This is the equivalent of "DIR".
*
*END*----------------------------------------------------------------------*/

uint32_t  Test_dir_list
   (
       MQX_FILE_PTR     mfs_fd_ptr
   )
{ /* Body */
   MFS_GET_LFN_STRUCT   lfn_data;
   MFS_SEARCH_DATA      search_data;
   MFS_SEARCH_PARAM     search;
   char                 lfn[FILENAME_SIZE + 1];
   unsigned char        attr;
   uint32_t             error_code;

   attr = MFS_ATTR_ANY;

   search.ATTRIBUTE = attr;
   search.WILDCARD = "*.*";
   search.SEARCH_DATA_PTR = &search_data;
   
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_FIRST_FILE, 
      (uint32_t *)&search);

   lfn_data.PATHNAME = search_data.NAME;
   lfn_data.LONG_FILENAME = lfn;
   while (error_code == MFS_NO_ERROR) {
      error_code = ioctl(mfs_fd_ptr, IO_IOCTL_GET_LFN, (void *)&lfn_data);
      error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_NEXT_FILE, (uint32_t *) &search_data);
   } /* Endwhile */

   if ( error_code == MFS_FILE_NOT_FOUND ) {
      error_code = MFS_NO_ERROR;
   }/* Endif */

   return( error_code );
   
} /* Endbody */

/*TASK*-------------------------------------------------------------------
*
* Task Name    : void tc_1_main_task(void)
* Comments     : Testing MFS rename on USB disk
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
* Comments     : Testing MFS rename on RAM disk
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

/*TASK*-------------------------------------------------------------------
*
* Task Name    : void tc_3_main_task(void)
* Comments     : Testing MFS rename on SD Card disk
*
*END*----------------------------------------------------------------------*/

void tc_3_main_task(void)
{
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   #define MFS       	"MFS:"
   mfs_fd_ptr = (MQX_FILE_PTR)Init_MFS_SDCARD();
   test_task();
#endif
}

void test_task(void)
{
   uint32_t                   error_code, i, j;
   uint32_t                   compare_from;
   MFS_SEARCH_DATA            search_data;
   MFS_SEARCH_PARAM           search;
   MFS_RENAME_PARAM           rename_struct[NUM_FILES];
   char                       old_name[NUM_FILES][20],
                              new_name[NUM_FILES][30];
   MQX_FILE_PTR               fd_array[NUM_FILES];
   char                       name_array[NUM_FILES][20];
   
   _int_install_unexpected_isr();
   
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mfs_fd_ptr != NULL), "Test #1 Creating File System");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (mfs_fd_ptr != NULL), "Test #1 Creating File System");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (mfs_fd_ptr != NULL), "Test #1 Creating File System");
#endif
   if(!mfs_fd_ptr){
   	return;
   }   
   
   /* Create a file  */
   for (i = 0; i < NUM_FILES; i++) {
      sprintf(name_array[i], "%s/TEST_FN%d.TXT", MFS, i);
      fd_array[i] = fopen(name_array[i], "w+");
#if DISK_FOR_MFS_TEST == USB_DISK
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (fd_array[i] !=0), "Test #1 3.0 Creating files");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, (fd_array[i] !=0), "Test #1 3.0 Creating files");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, (fd_array[i] !=0), "Test #1 3.0 Creating files");
#endif
      if (!fd_array[i]) {
         return;
      }
   }

   /* Close a file */   
   for (i = 0; i < NUM_FILES; i++) {
      error_code = fclose(fd_array[i]);
#if DISK_FOR_MFS_TEST == USB_DISK
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 3.1 Closing files");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 3.1 Closing files");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 3.1 Closing files");
#endif
      if (error_code) {
         return;
      } 
   } 

   /* Find a file */
   search.ATTRIBUTE       = MFS_ATTR_ANY;
   search.SEARCH_DATA_PTR = &search_data;
   compare_from = strlen(MFS)+1;
   for (i = 0; i < NUM_FILES; i++) {
      search.WILDCARD        = name_array[i];
      error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_FIRST_FILE, (uint32_t *)&search);
#if DISK_FOR_MFS_TEST == USB_DISK
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 3.2.1 Finding files");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 3.2.1 Finding files");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 3.2.1 Finding files");
#endif
      if (error_code) {
         return;
      } /* Endif */

      j = strncmp(search_data.NAME, &name_array[i][compare_from], FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (j == 0), "Test #1 3.2.2 Finding files");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, (j == 0), "Test #1 3.2.2 Finding files");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, (j == 0), "Test #1 3.2.2 Finding files");
#endif
      if (j) {
         return;
      } /* Endif */
   } /* Endfor */

   /* List a file */
   error_code = Test_dir_list(mfs_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.3 Listing files");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.3 Listing files");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.3 Listing files");
#endif
   if ( error_code != MFS_NO_ERROR ){
      return;
   }
   
   /* Rename a file with lfn */
   for (i = 0; i < NUM_FILES; i++) {
      sprintf(old_name[i], "%s/TEST_FN%d.TXT", MFS, i);
      rename_struct[i].OLD_PATHNAME = old_name[i];
      sprintf(new_name[i], "%s/TEST_FN%d.TXTT", MFS, i);
      rename_struct[i].NEW_PATHNAME = new_name[i];
      error_code = ioctl(mfs_fd_ptr, IO_IOCTL_RENAME_FILE, (uint32_t *) &rename_struct[i]);
#if DISK_FOR_MFS_TEST == USB_DISK
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.4 Renaming files with lfn");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.4 Renaming files with lfn");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.4 Renaming files with lfn");
#endif
      if (error_code != MFS_NO_ERROR) {
         return;
      } /* Endif */
   } /* Endfor */

   /* List files */
   error_code = Test_dir_list(mfs_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.5 Listing files");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.5 Listing files");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.5 Listing files");
#endif
   if ( error_code != MFS_NO_ERROR ){
      return;
   }
   
   /* Find a lfn file */
   search.ATTRIBUTE       = MFS_ATTR_ANY;
   search.SEARCH_DATA_PTR = &search_data;
   
   for (i = 0; i < NUM_FILES; i++) {
      search.WILDCARD        = new_name[i];
      error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_FIRST_FILE, (uint32_t *)&search);
#if DISK_FOR_MFS_TEST == USB_DISK
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.6 Finding lfn files");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.6 Finding lfn files");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.6 Finding lfn files");
#endif
      if (error_code == MFS_FILE_NOT_FOUND) {
         return;
      } /* Endif */
   } /* Endfor */

} /* Endbody */


    
/******************** End Testcases ********************/

/******************** Register tests *******************/
EU_TEST_SUITE_BEGIN(suite_mfs)
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_TEST_CASE_ADD(tc_1_main_task, "- Testing rename on USB disk"),
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_TEST_CASE_ADD(tc_2_main_task, "- Testing rename on RAM disk"),
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_TEST_CASE_ADD(tc_3_main_task, "- Testing rename on SD Card disk"),
#endif
EU_TEST_SUITE_END(suite_mfs)

EU_TEST_REGISTRY_BEGIN()
   EU_TEST_SUITE_ADD(suite_mfs, "Testing MFS rename")
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
