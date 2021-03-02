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
* $Version : 3.8.12.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file tests the cache capabilities in MFS
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <mfs.h>
#include <mfs_prv.h>
#include "crc32.h"
#include "test.h"
#include "util.h"
#include "mfs_init.h"
#include "io_mem.h"
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"
#define FILE_BASENAME   test
void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
void test_task(void);
uint32_t get_crc(MQX_FILE_PTR mfs_fd_ptr);


TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
#if defined(TI320C30D) || defined(TI320C40D)
   { MAIN_TASK,      main_task,       200L,  11L, "Main",  MQX_AUTO_START_TASK},
#else
   { MAIN_TASK,      main_task,      5000L,  11L, "Main",  MQX_AUTO_START_TASK},
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
  #include "usb_host_msd_bo.h"
  #include "usb_host_msd_ufi.h"
  #include "usbmfspr.h"
  volatile bool bCallBack = FALSE;
  volatile USB_STATUS bStatus       = USB_OK;
  volatile uint32_t  dBuffer_length = 0;
  COMMAND_OBJECT_PTR pCmd;
  USB_STATUS     status = USB_OK;
  MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO read_capacity;
  uint32_t block_len;
  USB_FILESYSTEM_STRUCT_PTR usb_fs_handle;
  void callback_fn(USB_STATUS, void *, void *, uint32_t);
  void set_Command_Object(void);
#endif

MQX_FILE_PTR               mfs_fd_ptr;

/*TASK*-------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : Testing MFS cache capabilities on USB disk
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
* Task Name    : tc_2_main_task
* Comments     : Testing MFS cache capabilities on RAM disk
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
* Task Name    : tc_3_main_task
* Comments     : Testing MFS cache capabilities on SD Card disk
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
   uint32_t                   error_code;
   MFS_IOCTL_FORMAT_PARAM     format_struct;
   TIME_STRUCT                time;
   char                       *lfn;
   uint32_t                   crc1, crc2;

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
   chksum_crc32gentab();
   lfn = _mem_alloc_system_zero(FILENAME_SIZE + 1);
   format_struct.FORMAT_PTR = &MFS_format;

   ioctl(mfs_fd_ptr, IO_IOCTL_FAT_CACHE_ON, NULL);
   ioctl(mfs_fd_ptr, IO_IOCTL_WRITE_CACHE_ON, NULL);

   _time_delay(1);
   _time_get(&time);
   _test_suite(mfs_fd_ptr, lfn);

   //crc
   crc1 = get_crc(mfs_fd_ptr);
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FORMAT,(uint32_t *) &format_struct);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 Formatting MFS");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 Formatting MFS");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 Formatting MFS");
#endif
   if (error_code != MFS_NO_ERROR) {
      return;
   }

   ioctl(mfs_fd_ptr, IO_IOCTL_FAT_CACHE_OFF, NULL);
   ioctl(mfs_fd_ptr, IO_IOCTL_WRITE_CACHE_OFF, NULL);

   /* Reset time to keep timestamps the same */
   _time_delay(1);
   _time_set(&time);
   _test_suite(mfs_fd_ptr, lfn);

   //crc2
   crc2 = get_crc(mfs_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (crc1 == crc2), "Test #1 Compare CRC's");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (crc1 == crc2), "Test #1 Compare CRC's");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (crc1 == crc2), "Test #1 Compare CRC's");
#endif
   if (crc1 != crc2)
      return;
}

void _test_suite
   (
      MQX_FILE_PTR mfs_fd_ptr,
      char *lfn
   )
{
   uint32_t                   error_code, i;
   MFS_SEARCH_DATA            search_data;
   MFS_SEARCH_PARAM           search;
   MFS_GET_LFN_STRUCT         lfn_struct;

   /* Create an 8.3 subdirectory  */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CREATE_SUBDIR, (void *)"short.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 3.0 Create 8.3 subdirectory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 3.0 Create 8.3 subdirectory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 3.0 Create 8.3 subdirectory");
#endif
   if (error_code != MFS_NO_ERROR) {
      return;
   }

   /* Re-create an 8.3 subdirectory  */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CREATE_SUBDIR, (void *)"short.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code != MFS_NO_ERROR), "Test #1 Testing: 3.1 Create duplicate 8.3 subdirectory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code != MFS_NO_ERROR), "Test #1 Testing: 3.1 Create duplicate 8.3 subdirectory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code != MFS_NO_ERROR), "Test #1 Testing: 3.1 Create duplicate 8.3 subdirectory");
#endif
   if (error_code == MFS_NO_ERROR) {
      return;
   }

   /* Create a long file name subdirectory  */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CREATE_SUBDIR, (void *)"longfilename1.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 3.2 Create long file name subdirectory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 3.2 Create long file name subdirectory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 3.2 Create long file name subdirectory");
#endif
   if (error_code != MFS_NO_ERROR) {
      return;
   }

   /* Re-create a long file name subdirectory  */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CREATE_SUBDIR, (void *)"longfilename1.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code != MFS_NO_ERROR), "Test #1 Testing: 3.3 Create duplicate long file name subdirectory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code != MFS_NO_ERROR), "Test #1 Testing: 3.3 Create duplicate long file name subdirectory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code != MFS_NO_ERROR), "Test #1 Testing: 3.3 Create duplicate long file name subdirectory");
#endif
   if (error_code == MFS_NO_ERROR) {
      return;
   }

   /* Create another long file name subdirectory  */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CREATE_SUBDIR, (void *)"longfilename2.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 3.4 Create a second long file name subdirectory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 3.4 Create a second long file name subdirectory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 3.4 Create a second long file name subdirectory");
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
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR ), "Test #1 Testing: 4.0 List subdirectories");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR ), "Test #1 Testing: 4.0 List subdirectories");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR ), "Test #1 Testing: 4.0 List subdirectories");
#endif
   if (error_code) {
      return;
   }

   /* Should match 8.3 directory name */
   i = strncmp(search_data.NAME, "SHORT.XXX", FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i == 0), "Test #1 Testing: 4.0.1 Should match 8.3 directory name");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == 0), "Test #1 Testing: 4.0.1 Should match 8.3 directory name");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == 0), "Test #1 Testing: 4.0.1 Should match 8.3 directory name");
#endif
   if (i) {
      return;
   }

   /* Find first lfn directory. The 8.3 equivalent should be longfi~1.xxx */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_NEXT_FILE, (uint32_t *)&search_data);
   i = strncmp(search_data.NAME, "LONGFI~1.XXX", FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i == 0), "Test #1 Testing: 4.0.2 Find first lfn directory. The 8.3 equivalent should be longfi~1.xxx");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == 0), "Test #1 Testing: 4.0.2 Find first lfn directory. The 8.3 equivalent should be longfi~1.xxx");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == 0), "Test #1 Testing: 4.0.2 Find first lfn directory. The 8.3 equivalent should be longfi~1.xxx");
#endif
   if (i) {
      return;
   }

   /* Convert to lfn */
   lfn_struct.PATHNAME      = "longfi~1.xxx";
   lfn_struct.LONG_FILENAME = lfn;

   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_GET_LFN, (uint32_t *)&lfn_struct);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 4.0.2 Convert to lfn");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 4.0.2 Convert to lfn");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 4.0.2 Convert to lfn");
#endif
   if (error_code) {
      return;
   }

   i = strncmp("longfilename1.xxx", lfn, FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i == 0), "Test #1 Testing: 4.0.2.1 Convert to lfn");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == 0), "Test #1 Testing: 4.0.2.1 Convert to lfn");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == 0), "Test #1 Testing: 4.0.2.1 Convert to lfn");
#endif
   if (i) {
      return;
   }

   /* Find second lfn directory. The 8.3 equivalent should be longfi~2.xxx */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_NEXT_FILE, (uint32_t *)&search_data);
   i = strncmp(search_data.NAME, "LONGFI~2.XXX", FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i == 0), "Test #1 Testing: 4.0.3 Find second lfn directory. The 8.3 equivalent should be longfi~2.xxx");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == 0), "Test #1 Testing: 4.0.3 Find second lfn directory. The 8.3 equivalent should be longfi~2.xxx");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == 0), "Test #1 Testing: 4.0.3 Find second lfn directory. The 8.3 equivalent should be longfi~2.xxx");
#endif
   if (i) {
      return;
   }

   /* Convert to lfn */
   lfn_struct.PATHNAME      = "longfi~2.xxx";
   lfn_struct.LONG_FILENAME = lfn;

   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_GET_LFN, (uint32_t *)&lfn_struct);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 4.0.4 Convert to lfn");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 4.0.4 Convert to lfn");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 4.0.4 Convert to lfn");
#endif
   if (error_code) {
      return;
   }

   i = strncmp("longfilename2.xxx", lfn, FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i == 0), "Test #1 Testing: 4.0.4.1 Convert to lfn");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == 0), "Test #1 Testing: 4.0.4.1 Convert to lfn");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == 0), "Test #1 Testing: 4.0.4.1 Convert to lfn");
#endif
   if (i) {
      return;
   }

   /* Delete the 8.3 subdirectory */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_REMOVE_SUBDIR, (void *)"short.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 4.1 Delete 8.3 subdirectory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 4.1 Delete 8.3 subdirectory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 4.1 Delete 8.3 subdirectory");
#endif
   if (error_code) {
      return;
   }

   /* Create a 3rd lfn subdirectory */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CREATE_SUBDIR, (void *)"longfilename3.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 4.2 Create third lfn subdirectory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 4.2 Create third lfn subdirectory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 4.2 Create third lfn subdirectory");
#endif
   if (error_code != MFS_NO_ERROR) {
      return;
   }

   /* Search for files again */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_FIRST_FILE,(uint32_t *)&search);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 4.3 List subdirectories again");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 4.3 List subdirectories again");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 4.3 List subdirectories again");
#endif
   if (error_code) {
      return;
   }

   i = strncmp(search_data.NAME, "LONGFI~1.XXX", FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i == 0), "Test #1 Testing: 4.3.1 List subdirectories again");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == 0), "Test #1 Testing: 4.3.1 List subdirectories again");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == 0), "Test #1 Testing: 4.3.1 List subdirectories again");
#endif
   if (i) {
      return;
   }

   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_NEXT_FILE,(uint32_t *)&search_data);
   i = strncmp(search_data.NAME, "LONGFI~2.XXX", FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i == 0), "Test #1 Testing: 4.3.2 List subdirectories again");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == 0), "Test #1 Testing: 4.3.2 List subdirectories again");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == 0), "Test #1 Testing: 4.3.2 List subdirectories again");
#endif
   if (i) {
      return;
   }

   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_NEXT_FILE,(uint32_t *)&search_data);
   i = strncmp(search_data.NAME, "LONGFI~3.XXX", FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i == 0), "Test #1 Testing: 4.3.2 List subdirectories again");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == 0), "Test #1 Testing: 4.3.2 List subdirectories again");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == 0), "Test #1 Testing: 4.3.2 List subdirectories again");
#endif
   if (i) {
      return;
   }

   /* Change current directory */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CHANGE_CURRENT_DIR, (uint32_t *)"\\longfilename1.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 5.0 Change to 1st lfn directory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 5.0 Change to 1st lfn directory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 5.0 Change to 1st lfn directory");
#endif
   if (error_code) {
      return;
   }

   /* Change to 2nd lfn directory */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CHANGE_CURRENT_DIR, (uint32_t *)"\\longfilename2.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 5.1 Change to 2nd lfn directory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 5.1 Change to 2nd lfn directory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 5.1 Change to 2nd lfn directory");
#endif
   if (error_code) {
      return;
   }

   /* Change to 3rd lfn directory */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_CHANGE_CURRENT_DIR, (uint32_t *)"\\longfilename3.xxx");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 5.2 Change to 3rd lfn directory");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 5.2 Change to 3rd lfn directory");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 5.2 Change to 3rd lfn directory");
#endif
   if (error_code) {
      return;
   }

   /* Flush output */
   error_code = fflush(mfs_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 6.0 Flush output");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 6.0 Flush output");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 Testing: 6.0 Flush output");
#endif
   if (error_code) {
      return;
   }
} /* Endbody */

uint32_t get_crc(MQX_FILE_PTR mfs_fd_ptr)
{
  uint32_t    crc;

#if DISK_FOR_MFS_TEST == RAM_DISK
   MQX_FILE_PTR   handle;
   char           *data;
   uint32_t       error_code;

   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_GET_DEVICE_HANDLE, &handle);
   error_code = ioctl(handle, IO_MEM_IOCTL_GET_BASE_ADDRESS, &data);
   crc = chksum_crc32((unsigned char *)data, MFS_format.BYTES_PER_SECTOR * NUM_SECTORS);
   return crc;
#endif
#if DISK_FOR_MFS_TEST == USB_DISK
   unsigned char  *data_buf;
   uint32_t       block_cnt,i;
   void           (*org_callback)(USB_STATUS, void *, void *, uint32_t);

   bCallBack = FALSE;
//   pCmd = &((IO_USB_MFS_STRUCT_PTR)usb_fs_handle->DEV_FD_PTR->DEV_PTR->DRIVER_INIT_PTR)->COMMAND;

  DEV_MEMORY_PTR         block_ptr;
  IO_USB_MFS_STRUCT_PTR  info_ptr;
  block_ptr = (DEV_MEMORY_PTR)usb_fs_handle->DEV_FD_PTR->DEV_PTR->DRIVER_INIT_PTR;
  info_ptr = (IO_USB_MFS_STRUCT_PTR)(block_ptr->payload.data + block_ptr->offset);
  pCmd = &(info_ptr->COMMAND);

  org_callback = pCmd->CALLBACK;
  pCmd -> CALLBACK=callback_fn;
  status = usb_mass_ufi_read_capacity(pCmd, (unsigned char *)&read_capacity,
    sizeof(MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO));
  if ((status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
  {
    printf ("\n...ERROR reading block length");
    return -1;
  }
  else
  {
    while (!bCallBack){;}
    bCallBack = FALSE;
    if (bStatus) {
      printf("...Unsupported by device (bStatus=0x%x)\n", bStatus);
      return -1;
    }
  }

  block_len = HOST_TO_BE_LONG(* (uint32_t *) &read_capacity.BLENGTH);
  block_cnt = HOST_TO_BE_LONG(* (uint32_t *) &read_capacity.BLLBA);
  crc=0xFFFFFFFF;
  data_buf = _mem_alloc_system_zero(block_len);
  for(i=1;i<90;i++){
    status = usb_mass_ufi_read_10(pCmd, i, data_buf, block_len, 1);

    if ((status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
    {
      printf ("\n...ERROR");
      return -1;
    }
    else
    {
      /* Wait till command comes back */
      while (!bCallBack){;}
      bCallBack = FALSE;
      if (bStatus) {
        printf("...Unsupported by device (bStatus=0x%x)\n", bStatus);
        return -1;
      }
      crc = chksum_crc32_feed(crc, data_buf, block_len);

    }
  }
  _mem_free(data_buf);
  pCmd->CALLBACK = org_callback;
  return crc;
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   MQX_FILE_PTR   handle;
   uint8_t          *data_buf;
   uint32_t     error_code, num, i;

   crc=0xFFFFFFFF;
   data_buf = _mem_alloc_system_zero(MFS_format.BYTES_PER_SECTOR);
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_GET_DEVICE_HANDLE, &handle);
   if(error_code)
      return 1;
   for(i=1;i<NUM_SECTORS;i++){
#if defined BSP_SDCARD_ESDHC_CHANNEL
      num = _io_sdcard_esdhc_read_block(handle, data_buf, i, 1);
#elif defined BSP_SDCARD_SDHC_CHANNEL
      num = _io_sdcard_sdhc_read_block(handle, data_buf, i, 1);
#elif defined BSP_SDCARD_SPI_CHANNEL
      num = _io_sdcard_spi_read_block(handle, data_buf, i, 1);
#endif
      if (num != 1)
         return 2;
      crc = chksum_crc32_feed(crc, data_buf, MFS_format.BYTES_PER_SECTOR);
   }
   return crc;
#endif
}

#if DISK_FOR_MFS_TEST == USB_DISK
void callback_fn
   (
      USB_STATUS status,
      void * p1,
      void *  p2,
      uint32_t buffer_length
   )
{ /* Body */
   dBuffer_length = buffer_length;
   bCallBack = TRUE;
   bStatus = status;
}
#endif

 /******************** End Testcases ********************/

/******************** Register tests *******************/
EU_TEST_SUITE_BEGIN(suite_mfs)
#if DISK_FOR_MFS_TEST == USB_DISK
    EU_TEST_CASE_ADD(tc_1_main_task, "- Testing cache on USB disk"),
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
    EU_TEST_CASE_ADD(tc_2_main_task, "- Testing cache on RAM disk"),
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
    EU_TEST_CASE_ADD(tc_3_main_task, "- Testing cache on SD Card disk"),
#endif
EU_TEST_SUITE_END(suite_mfs)

EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_mfs, "Testing MFS cache ")
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
