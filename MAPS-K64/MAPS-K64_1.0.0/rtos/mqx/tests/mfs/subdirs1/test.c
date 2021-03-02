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
*   This file tests the subdirectory capabilities in MFS
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

MQX_FILE_PTR               mfs_fd_ptr=NULL;

/*TASK*-------------------------------------------------------------------
*
* Task Name    : void tc_1_main_task(void)
* Comments     : Testing MFS subdirectories on USB disk
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
* Comments     : Testing MFS subdirectories on RAM disk
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
* Comments     : Testing MFS subdirectories on SD Card disk
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
   uint32_t                   error_code, i;
   MQX_FILE_PTR               file_fd_ptr;
   MFS_SEARCH_DATA            search_data;
   MFS_SEARCH_PARAM           search;
   MFS_GET_LFN_STRUCT         lfn_struct;
   char                       *lfn;
   char                       filename[30];

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
      
   lfn      = _mem_alloc_system_zero(FILENAME_SIZE + 1);

   /* Create an 8.3 subdirectory  */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CREATE_SUBDIR, (void *)"short.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.0 Create 8.3 subdirectory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.0 Create 8.3 subdirectory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.0 Create 8.3 subdirectory");
#endif
   if (error_code != MFS_NO_ERROR) {
      return;
   }
   
   /* Re-create an 8.3 subdirectory  */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CREATE_SUBDIR, (void *)"short.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code != MFS_NO_ERROR), "Test #1 3.1 Create duplicate 8.3 subdirectory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code != MFS_NO_ERROR), "Test #1 3.1 Create duplicate 8.3 subdirectory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code != MFS_NO_ERROR), "Test #1 3.1 Create duplicate 8.3 subdirectory");
#endif
   if (error_code == MFS_NO_ERROR) {
      return;
   }

   /* Create a long file name subdirectory  */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CREATE_SUBDIR, (void *)"longfilename1.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.2 Create long file name subdirectory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.2 Create long file name subdirectory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.2 Create long file name subdirectory");
#endif
   if (error_code != MFS_NO_ERROR) {
      return;
   }

   /* Re-create a long file name subdirectory  */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CREATE_SUBDIR, (void *)"longfilename1.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code != MFS_NO_ERROR), "Test #1 3.3 Create duplicate long file name subdirectory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code != MFS_NO_ERROR), "Test #1 3.3 Create duplicate long file name subdirectory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code != MFS_NO_ERROR), "Test #1 3.3 Create duplicate long file name subdirectory");
#endif
   if (error_code == MFS_NO_ERROR) {
      return;
   }

   /* Create another long file name subdirectory  */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CREATE_SUBDIR, (void *)"longfilename2.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.4 Create a second long file name subdirectory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.4 Create a second long file name subdirectory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.4 Create a second long file name subdirectory");
#endif
   if (error_code != MFS_NO_ERROR) {
      return;
   }
   
   /* List the directories. We should get them in order they were created */
   search.ATTRIBUTE       = MFS_ATTR_ANY;
   search.WILDCARD        = "*.*";
   search.SEARCH_DATA_PTR = &search_data;
   
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_FIRST_FILE,(uint32_t *)&search);
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 4.0 List subdirectories");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 4.0 List subdirectories");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 4.0 List subdirectories");
#endif
   if (error_code) {
      return;
   } 

   /* Should match 8.3 directory name */
   i = strncmp(search_data.NAME, "SHORT.XXX", FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i == 0), "Test #1 4.1 Match 8.3 directory name");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == 0), "Test #1 4.1 Match 8.3 directory name");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == 0), "Test #1 4.1 Match 8.3 directory name");
#endif
   if (i) {
      return;
   }
   
   /* Find first lfn directory. The 8.3 equivalent should be longfi~1.xxx */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_NEXT_FILE,(uint32_t *)&search_data);
   i = strncmp(search_data.NAME, "LONGFI~1.XXX", FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i == 0), "Test #1 4.2 Find first LFN directory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == 0), "Test #1 4.2 Find first LFN directory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == 0), "Test #1 4.2 Find first LFN directory");
#endif
   if (i) {
      return;
   }

   /* Convert to lfn */
   lfn_struct.PATHNAME      = "longfi~1.xxx";
   lfn_struct.LONG_FILENAME = lfn;
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_GET_LFN, (uint32_t *)&lfn_struct);
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 4.3 Get LFN from 8.3 name");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 4.3 Get LFN from 8.3 name");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 4.3 Get LFN from 8.3 name");
#endif
   if (error_code) {
      return;
   }

   i = strncmp("longfilename1.xxx", lfn, FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i == 0), "Test #1 4.4 Check LFN conversion");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == 0), "Test #1 4.4 Check LFN conversion");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == 0), "Test #1 4.4 Check LFN conversion");
#endif
   if (i) {
      return;
   }

   /* Find second lfn directory. The 8.3 equivalent should be longfi~2.xxx */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_NEXT_FILE,(uint32_t *)&search_data);
   i = strncmp(search_data.NAME, "LONGFI~2.XXX", FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i == 0), "Test #1 4.5 Find second LFN directory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == 0), "Test #1 4.5 Find second LFN directory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == 0), "Test #1 4.5 Find second LFN directory");
#endif
   if (i) {
      return;
   } /* Endif */

   /* Convert to lfn */
   lfn_struct.PATHNAME      = "longfi~2.xxx";
   lfn_struct.LONG_FILENAME = lfn;
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_GET_LFN, (uint32_t *)&lfn_struct);
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 4.6 Get LFN from 8.3 name");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 4.6 Get LFN from 8.3 name");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 4.6 Get LFN from 8.3 name");
#endif
   if (error_code) {
      return;
   } /* Endif */

   i = strncmp("longfilename2.xxx", lfn, FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i == 0), "Test #1 4.7 Check LFN conversion");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == 0), "Test #1 4.7 Check LFN conversion");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == 0), "Test #1 4.7 Check LFN conversion");
#endif
   if (i) {
      return;
   } /* Endif */


   /* Delete the 8.3 subdirectory */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_REMOVE_SUBDIR, (void *)"short.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 5.0 Delete 8.3 subdirectory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 5.0 Delete 8.3 subdirectory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 5.0 Delete 8.3 subdirectory");
#endif
   if (error_code) {
      return;
   }

   /* Create a 3rd lfn subdirectory */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CREATE_SUBDIR, (void *)"longfilename3.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 5.1 Create third lfn subdirectory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 5.1 Create third lfn subdirectory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 5.1 Create third lfn subdirectory");
#endif
   if (error_code != MFS_NO_ERROR) {
      return;
   }

   /* Search for files again */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_FIRST_FILE,(uint32_t *)&search);
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 5.2 List subdirectories again");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 5.2 List subdirectories again");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 5.2 List subdirectories again");
#endif
   if (error_code) {
      return;
   } /* Endif */

   i = strncmp(search_data.NAME, "LONGFI~1.XXX", FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i == 0), "Test #1 5.3.1 Find first LFN directory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == 0), "Test #1 5.3.1 Find first LFN directory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == 0), "Test #1 5.3.1 Find first LFN directory");
#endif
   if (i) {
      return;
   } /* Endif */

   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_NEXT_FILE,(uint32_t *)&search_data);
   i = strncmp(search_data.NAME, "LONGFI~2.XXX", FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i == 0), "Test #1 5.3.2 Find second LFN dorectory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == 0), "Test #1 5.3.2 Find second LFN dorectory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == 0), "Test #1 5.3.2 Find second LFN dorectory");
#endif
   if (i) {
      return;
   } /* Endif */

   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_NEXT_FILE,(uint32_t *)&search_data);
   i = strncmp(search_data.NAME, "LONGFI~3.XXX", FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i == 0), "Test #1 5.3.3 Find third LFN directory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == 0), "Test #1 5.3.3 Find third LFN directory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == 0), "Test #1 5.3.3 Find third LFN directory");
#endif
   if (i) {
      return;
   } /* Endif */

   /* Change current directory */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CHANGE_CURRENT_DIR,(uint32_t *)"\\longfilename1.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 5.4.1 Change to 1st lfn directory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 5.4.1 Change to 1st lfn directory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 5.4.1 Change to 1st lfn directory");
#endif
   if (error_code) {
      return;
   } /* Endif */

   /* Change to 2nd lfn directory */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CHANGE_CURRENT_DIR,(uint32_t *)"\\longfilename2.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 5.4.2 Change to 2nd lfn directory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 5.4.2 Change to 2nd lfn directory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 5.4.2 Change to 2nd lfn directory");
#endif
   if (error_code) {
      return;
   } /* Endif */

   /* Change to 3rd lfn directory */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CHANGE_CURRENT_DIR,(uint32_t *)"\\longfilename3.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 5.4.3 Change to 3rd lfn directory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 5.4.3 Change to 3rd lfn directory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 5.4.3 Change to 3rd lfn directory");
#endif
   if (error_code) {
      return;
   } /* Endif */

   /* Change to root directory */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CHANGE_CURRENT_DIR,(uint32_t *)"\\");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 5.4.4 Change to root directory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 5.4.4 Change to root directory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 5.4.4 Change to root directory");
#endif
   if (error_code) {
      return;
   } /* Endif */

   /* Create special file */
   sprintf(filename, "%s\\kl_dct1152_2a.xml", MFS);
   file_fd_ptr = fopen(filename, "n+");
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file_fd_ptr != NULL), "Test #1 6.0 Create special file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file_fd_ptr != NULL), "Test #1 6.0 Create special file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (file_fd_ptr != NULL), "Test #1 6.0 Create special file");
#endif
   if (!file_fd_ptr) {
      return;
   } /* Endif */


   /* Create a 4th lfn subdirectory which is special */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CREATE_SUBDIR, (void *)"kl_dct1152_2a");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 6.1 Create special subdirectory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 6.1 Create special subdirectory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 6.1 Create special subdirectory");
#endif
   if (error_code != MFS_NO_ERROR) {
      return;
   } /* Endif */

   /* Change to special directory */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CHANGE_CURRENT_DIR,(uint32_t *)"kl_dct1152_2a");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 6.2 Change to special directory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 6.2 Change to special directory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 6.2 Change to special directory");
#endif
   if (error_code) {
      return;
   } /* Endif */

   /* Attempt to change to special file */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CHANGE_CURRENT_DIR,(uint32_t *)"..\\kl_dct1152_2a.xml");
#if DISK_FOR_MFS_TEST == USB_DISK   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_PATH_NOT_FOUND), "Test #1 6.2 Change to special directory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_PATH_NOT_FOUND), "Test #1 6.2 Change to special directory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_PATH_NOT_FOUND), "Test #1 6.2 Change to special directory");
#endif
   if (error_code != MFS_PATH_NOT_FOUND) {
      return;
   } /* Endif */

} /* Endbody */


/******************** End Testcases ********************/

/******************** Register tests *******************/
EU_TEST_SUITE_BEGIN(suite_mfs)
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_TEST_CASE_ADD(tc_1_main_task, "- Testing subdirectories on USB disk"),
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_TEST_CASE_ADD(tc_2_main_task, "- Testing subdirectories on RAM disk"),
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_TEST_CASE_ADD(tc_3_main_task, "- Testing subdirectories on SD Card disk"),
#endif
EU_TEST_SUITE_END(suite_mfs)

EU_TEST_REGISTRY_BEGIN()
   EU_TEST_SUITE_ADD(suite_mfs, "Testing MFS subdirectories")
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
