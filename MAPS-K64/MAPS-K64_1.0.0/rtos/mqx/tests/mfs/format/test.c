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
* $Version : 3.8.7.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file tests the format capabilities in MFS
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

MQX_FILE_PTR   mfs_fd_ptr=NULL;
char           write_buffer[512];

/*TASK*-------------------------------------------------------------------
*
* Task Name    : tc_1_main_task(void)
* Comments     : Testing MFS Format on USB disk
*
*END*----------------------------------------------------------------------*/

void tc_1_main_task(void)
{
#if DISK_FOR_MFS_TEST == USB_DISK
   #define MFS        "c:"
   usb_fs_handle = (USB_FILESYSTEM_STRUCT_PTR)Init_MFS_USB();
   mfs_fd_ptr = usb_filesystem_handle(usb_fs_handle);
   test_task();
#endif
}
/*TASK*-------------------------------------------------------------------
*
* Task Name    : tc_2_main_task(void)
* Comments     : Testing MFS Format on RAM disk
*
*END*----------------------------------------------------------------------*/

void tc_2_main_task(void)
{
#if DISK_FOR_MFS_TEST == RAM_DISK
   #define MFS        "MFS:"
   mfs_fd_ptr = (MQX_FILE_PTR)Init_MFS_RAM(MFS_format.BYTES_PER_SECTOR * NUM_SECTORS);
   test_task();
#endif
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : tc_3_main_task(void)
* Comments     : Testing MFS Format on SD Card disk
*
*END*----------------------------------------------------------------------*/

void tc_3_main_task(void)
{
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   #define MFS        "MFS:"
   mfs_fd_ptr = (MQX_FILE_PTR)Init_MFS_SDCARD();
   test_task();
#endif
}

void test_task(void)
{
   uint32_t                   error_code, i;
   MQX_FILE_PTR               tmp_fd_ptr[10];
   MQX_FILE_PTR               file_fd_ptr;
   MFS_IOCTL_FORMAT_PARAM     format_struct;
   char                       *filepath;
   char                       *tmp_filepath[10];
   MFS_SEARCH_DATA            search_data;
   MFS_SEARCH_PARAM           search;
   int32_t                    size;

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

   format_struct.FORMAT_PTR = &MFS_format;
   filepath = _mem_alloc_system_zero(PATHNAME_SIZE + 1);

   /* Build pathname string */
   sprintf(filepath, "%s%s", MFS, FILE_NAME);
   for (i = 0; i < 10; i++) {
      tmp_filepath[i] = _mem_alloc_system_zero(PATHNAME_SIZE + 1);
      sprintf(tmp_filepath[i], "%s\\", MFS);
   } /* Endfor */

   /* Create a file */
   file_fd_ptr = fopen(filepath, "n+");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file_fd_ptr !=NULL), "Test #1 3.0 Creating file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file_fd_ptr !=NULL), "Test #1 3.0 Creating file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (file_fd_ptr !=NULL), "Test #1 3.0 Creating file");
#endif
   if (!file_fd_ptr) {
      return;
   }

   /* Close file */
   error_code = fclose(file_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 3.1 Closing file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 3.1 Closing file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 3.1 Closing file");
#endif
   if (error_code) {
      return;
   }
   file_fd_ptr  = NULL;

   /* Close MFS */
   error_code = fclose(mfs_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 4.0 Closing MFS");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 4.0 Closing MFS");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 4.0 Closing MFS");
#endif
   if (error_code) {
      return;
   }
   mfs_fd_ptr = NULL;

   /* Re-open MFS device. Shouldn't need to format */
   mfs_fd_ptr = fopen(MFS, 0);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mfs_fd_ptr !=0), "Test #1 4.1 Re-opening MFS");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (mfs_fd_ptr !=0), "Test #1 4.1 Re-opening MFS");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (mfs_fd_ptr !=0), "Test #1 4.1 Re-opening MFS");
#endif
   if ( !mfs_fd_ptr ) {
      return;
   }

   //printf("4.2 Checking format ");
   error_code = ferror(mfs_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 4.2 Checking format");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 4.2 Checking format");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 4.2 Checking format");
#endif
   if ( error_code ) {
      return;
   }

   /* Our file should still be here */
   file_fd_ptr = fopen(filepath, "r+");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file_fd_ptr !=NULL), "Test #1 4.3 Re-opening file ");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file_fd_ptr !=NULL), "Test #1 4.3 Re-opening file ");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (file_fd_ptr !=NULL), "Test #1 4.3 Re-opening file ");
#endif
   if ( !file_fd_ptr ) {
      return;
   }

   /* Write to file */
   size = write(file_fd_ptr, write_buffer, 512);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (size == 512), "Test #1 4.3.1 Write to file ");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (size == 512), "Test #1 4.3.1 Write to file ");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (size == 512), "Test #1 4.3.1 Write to file ");
#endif
   if ( size != 512 ) {
      return;
   }

   /* Read file */
   fseek(file_fd_ptr, 0, IO_SEEK_SET);
   size = read(file_fd_ptr, write_buffer, 512);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (size == 512), "Test #1 4.3.2 Read from file ");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (size == 512), "Test #1 4.3.2 Read from file ");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (size == 512), "Test #1 4.3.2 Read from file ");
#endif
   if ( size != 512 ) {
      return;
   }

   /* Try to re-format with file open. Should get a sharing violation */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FORMAT, (uint32_t *) &format_struct);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_SHARING_VIOLATION), "Test #1 4.4 Re-formatting MFS with open file ");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_SHARING_VIOLATION), "Test #1 4.4 Re-formatting MFS with open file ");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_SHARING_VIOLATION), "Test #1 4.4 Re-formatting MFS with open file ");
#endif
   if (error_code != MFS_SHARING_VIOLATION) {
      return;
   }

   /* Close file */
   error_code = fclose(file_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 4.5 Closing file ");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 4.5 Closing file ");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 4.5 Closing file ");
#endif
   if (error_code) {
      return;
   }
   file_fd_ptr = NULL;

   /* Re-format with all files closed. Should not get any errors */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FORMAT,(uint32_t *) &format_struct);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 5.0 Re-formatting MFS with no open files ");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 5.0 Re-formatting MFS with no open files ");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 5.0 Re-formatting MFS with no open files ");
#endif
   if (error_code ) {
      return;
   }

   /* Try to open our file. Should fail */
   file_fd_ptr = fopen(filepath, "r");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file_fd_ptr == NULL), "Test #1 5.1 Re-opening file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file_fd_ptr == NULL), "Test #1 5.1 Re-opening file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (file_fd_ptr == NULL), "Test #1 5.1 Re-opening file");
#endif
   if ( file_fd_ptr ) {
      return;
   }

   /* Try to create our file. Should pass */
   file_fd_ptr = fopen(filepath, "n+");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file_fd_ptr != NULL), "Test #1 5.2 Re-creating file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file_fd_ptr != NULL), "Test #1 5.2 Re-creating file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (file_fd_ptr != NULL), "Test #1 5.2 Re-creating file");
#endif
   if (!file_fd_ptr) {
        return;
   }

   /* Create a string of temporary files */
   for (i = 0; i < 10; i++) {
      tmp_fd_ptr[i] = fopen(tmp_filepath[i], "x");
#if DISK_FOR_MFS_TEST == USB_DISK
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (tmp_fd_ptr[i] != NULL), "Test #1 5.2.1 Creating temporary files");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, (tmp_fd_ptr[i] != NULL), "Test #1 5.2.1 Creating temporary files");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, (tmp_fd_ptr[i] != NULL), "Test #1 5.2.1 Creating temporary files");
#endif
      if ( !tmp_fd_ptr[i] ) {
         return;
      } /* Endif */
   } /* Endfor */

   /* Write to file */
   size = write(file_fd_ptr, write_buffer, 512);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (size == 512), "Test #1 5.2.2 Write to file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (size == 512), "Test #1 5.2.2 Write to file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (size == 512), "Test #1 5.2.2 Write to file");
#endif
   if ( size != 512 ) {
      return;
   }

   /* flush sector cache */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FLUSH_OUTPUT, NULL);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 5.2.3 Flush sector cache");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 5.2.3 Flush sector cache");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 5.2.3 Flush sector cache");
#endif
   if ( error_code ) {
      return;
   }

   /* Find first file. Should get our created file */
   search.ATTRIBUTE       = MFS_ATTR_ANY;
   search.WILDCARD        = "*.*";
   search.SEARCH_DATA_PTR = &search_data;
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_FIRST_FILE,(uint32_t *)&search);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 5.3 Search for files");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 5.3 Search for files");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 5.3 Search for files");
#endif
   if (error_code) {
      return;
   }

   /* Should match our file */
   i = strncmp(search_data.NAME, FILE_NAME, FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i==0), "Test #1 5.3 Search for files");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i==0), "Test #1 5.3 Search for files");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i==0), "Test #1 5.3 Search for files");
#endif
   if (i) {
        return;
   }

   i = 0;
   while (!error_code) {
      i++;
      error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_NEXT_FILE,(uint32_t *) &search_data);
   } /* Endwhile */
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i > 10), "Test #1 5.3 Search for files");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i > 10), "Test #1 5.3 Search for files");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i > 10), "Test #1 5.3 Search for files");
#endif
   if (i <= 10) {
      return;
   }
} /* Endbody */

/******************** End Testcases ********************/

/******************** Register tests *******************/
EU_TEST_SUITE_BEGIN(suite_mfs)
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_TEST_CASE_ADD(tc_1_main_task, "- Testing Format Command on USB disk"),
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_TEST_CASE_ADD(tc_2_main_task, "- Testing Format Command on RAM disk"),
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_TEST_CASE_ADD(tc_3_main_task, "- Testing Format Command on SD Card disk"),
#endif
EU_TEST_SUITE_END(suite_mfs)

EU_TEST_REGISTRY_BEGIN()
   EU_TEST_SUITE_ADD(suite_mfs, "Testing MFS Format Command")
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
