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
*   This file tests the format capabilities in MFS
*
*END************************************************************************/

#include <string.h>
#include <mqx_inc.h>
//#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <mfs.h>
#include <mfs_prv.h>
//#include <psp_prv.h>

#include "test.h"
#include "util.h"
#include "mfs_init.h"

#if MQX_VERSION >= 250
#include <psp_comp.h>
//#include <mem_prv.h>
#endif

//#include <mqx_prv.h>
#include <partition_prv.h>

//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test
void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);

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

/*TASK*-------------------------------------------------------------------
*
* Task Name    : void tc_1_main_task(void)
* Comments     : Testing MFS install/uninstall on USB disk
*
*END*----------------------------------------------------------------------*/

void tc_1_main_task(void)
{
#if DISK_FOR_MFS_TEST == USB_DISK
   #define MFS_DEVICE     "USB:"
   #define PART_NAME      "PM_C1:"
   #define MFS            "c:"

   uint32_t                   error_code;
   MQX_FILE_PTR               mfs_fd_ptr;
   uint32_t                   count;
   _partition_id              part_id;
   void                       *pool_in_err_ptr;
   void                       *block_in_err_ptr;

   _int_install_unexpected_isr();

   usb_fs_handle = (USB_FILESYSTEM_STRUCT_PTR)Init_MFS_USB();
   mfs_fd_ptr = usb_filesystem_handle(usb_fs_handle);

   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (usb_fs_handle != NULL), "Test #1 Creating file system");
   if(usb_fs_handle == NULL){
      return;
   }

   count = _ftest_count_parts();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (count == 1), "Test #1 Partition count after file system instalation");
   if(count != 1){
      return;
   }

   usb_filesystem_uninstall(usb_fs_handle);
   count = _ftest_count_parts();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (count == 0), "Test #1 Partition count after file system uninstall");
   if(count != 0){
      return;
   }

   for (count = 0; count < EXTRA_PARTS; count++) {
      part_id = _partition_create(10, 1, 0, 0);
      if (!part_id) {
         return;
      }
   }
   count = _ftest_count_parts();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (count == EXTRA_PARTS), "Test #1 Extra partitions creation");
   if(count != EXTRA_PARTS){
      return;
   }

   usb_fs_handle = usb_filesystem_install(usb_dev_handle, MFS_DEVICE, PART_NAME, MFS);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (usb_fs_handle != NULL), "Test #1 Creating USB file system");
   if(usb_fs_handle == NULL){
      return;
   }

   count = _ftest_count_parts();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (count == EXTRA_PARTS +1), "Test #1 Extra partitions + USB filesystem");
   if(count != EXTRA_PARTS+1){
      return;
   }

   usb_filesystem_uninstall(usb_fs_handle);
   for (count = 0; count < EXTRA_PARTS; count++) {
      part_id = _partition_create(10, 1, 0, 0);
      if (!part_id) {
         return;
      }
   }
   count = _ftest_count_parts();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (count == 2*EXTRA_PARTS ), "Test #1 USB uninstall + Extra partitions created");
   if(count != 2*EXTRA_PARTS){
      return;
   }

   usb_fs_handle = usb_filesystem_install(usb_dev_handle, MFS_DEVICE, PART_NAME, MFS);
   count = _ftest_count_parts();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (count == 2*EXTRA_PARTS+1 ), "Test #1 USB install + Extra partitions created");
   if(count != 2*EXTRA_PARTS +1){
      return;
   }

   usb_filesystem_uninstall(usb_fs_handle);
   error_code = _partition_test(&part_id, &pool_in_err_ptr, &block_in_err_ptr);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 Partition test");
   if (error_code) {
        return;
   }

   mfs_fd_ptr = fopen(MFS, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mfs_fd_ptr == NULL), "Test #1 Open file system without installing first");
   if (mfs_fd_ptr) {
        return;
   }

#endif //#if DISK_FOR_MFS_TEST == USB_DISK
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : void tc_2_main_task(void)
* Comments     : Testing MFS install/uninstall on RAM disk
*
*END*----------------------------------------------------------------------*/

void tc_2_main_task(void)
{
#if DISK_FOR_MFS_TEST == RAM_DISK
#define MFS_DEVICE     "mfsram:"
#define MFS            "mfs:"

   uint32_t                   error_code;
   MQX_FILE_PTR               dev_handle,
                              mfs_fd_ptr;
   unsigned char              *ramdisk_ptr;
   uint32_t                   start_count;
   uint32_t                   count;
   _partition_id              part_id;
   void                       *pool_in_err_ptr;
   void                       *block_in_err_ptr;

   _int_install_unexpected_isr();

   /* Install the memory device */
   /* Add an extra sector for testing purposes */
   ramdisk_ptr = (unsigned char *)_mem_alloc_system_zero(BYTES_PER_SECT * NUM_SECTORS);

   error_code = _io_mem_install(MFS_DEVICE, ramdisk_ptr,BYTES_PER_SECT * NUM_SECTORS);

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MQX_OK ), "Test #1 1.0 _io_mem_intall");
   if ( error_code != MQX_OK ) {
      return;
   }

   /* Open the device which MFS will be installed on */
   dev_handle = fopen(MFS_DEVICE, MEM_OPEN_MODE);

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (dev_handle != NULL ), "Test #1 1.1 fopen ram disk");
   if ( dev_handle == NULL ) {
      return;
   }
   start_count = _ftest_count_parts();

   /* Install MFS  */
   error_code = _io_mfs_install(dev_handle, MFS, 0);

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR ), "Test #1 2.0.1 Installing MFS - no previous memory partitions created");
   if (error_code != MFS_NO_ERROR) {
      return;
   }

   /* Should have one more partition */
   count = _ftest_count_parts();

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (count == (start_count + 1) ), "Test #1 2.0.2 Installing MFS - no previous memory partitions created");
   if (count != (start_count + 1)) {
      return;
   } /* Endif */

   /* Uninstall MFS  */
   error_code = _io_mfs_uninstall(MFS);

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR ), "Test #1 2.1.1 Uninstalling MFS - no previous memory partitions created");
   if (error_code != MFS_NO_ERROR) {
      return;
   }

   /* Should be back to the original number of partitions */
   count = _ftest_count_parts();

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (count == start_count ), "Test #1 2.1.2 Uninstalling MFS - no previous memory partitions created");
   if (count != start_count) {
      return;
   } /* Endif */

   /* Re-install MFS  */
   error_code = _io_mfs_install(dev_handle, MFS, 0);

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR ), "Test #1 2.2.1 Re-installing MFS");
   if (error_code != MFS_NO_ERROR) {
      return;
   }

   /* Should have one more partition */
   count = _ftest_count_parts();

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (count == (start_count + 1)), "Test #1 2.2.2 Re-installing MFS");
   if (count != (start_count + 1)) {
      return;
   } /* Endif */

   /* Create a few more partitions */
   for (count = 0; count < EXTRA_PARTS; count++) {
      part_id = _partition_create(10, 1, 0, 0);

      EU_ASSERT_REF_TC_MSG(tc_2_main_task, (part_id != 0), "Test #1 2.2.3 Re-installing MFS - Extra partitions created after");
      if (!part_id) {
         return;
      } /* Endif */
   } /* Endfor */

   /* Uninstall MFS */
   error_code = _io_mfs_uninstall(MFS);

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.3.1 Uninstalling MFS - extra partitions created");
   if (error_code != MFS_NO_ERROR) {
      return;
   }

   /* Should be back to the original number plus of partitions plus the extra ones we created */
   count = _ftest_count_parts();

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (count == (start_count + EXTRA_PARTS)), "Test #1 2.3.2 Uninstalling MFS - extra partitions created");
   if (count != (start_count + EXTRA_PARTS)) {
      return;
   } /* Endif */

   /* Run the partition test to make sure nothing was corrupted */
   error_code = _partition_test(&part_id, &pool_in_err_ptr, &block_in_err_ptr);

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 2.4 Partition test");
   if (error_code) {
      return;
   }

   /* Now we have for sure partitions already created */

   /* Re-install MFS  */
   error_code = _io_mfs_install(dev_handle, MFS, 0);

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.5.1 Re-installing MFS");
   if (error_code != MFS_NO_ERROR) {
      return;
   }

   /* Should have one more partition */
   count = _ftest_count_parts();

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (count == (start_count + 1 + EXTRA_PARTS)), "Test #1 2.5.2 Re-installing MFS");
   if (count != (start_count + 1 + EXTRA_PARTS)) {
      return;
   } /* Endif */

   /* Create a few more partitions */
   for (count = 0; count < EXTRA_PARTS; count++) {
      part_id = _partition_create(10, 1, 0, 0);

      EU_ASSERT_REF_TC_MSG(tc_2_main_task, (part_id != 0), "Test #1 2.5.3 Re-installing MFS - Extra partitions created after and before");
      if (!part_id) {
         return;
      } /* Endif */
   } /* Endfor */

   /* Uninstall MFS */
   error_code = _io_mfs_uninstall(MFS);

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.6.1 Uninstalling MFS - extra partitions created");
   if (error_code != MFS_NO_ERROR) {
      return;
   }

   /* Should be back to the original number plus of partitions plus the extra ones we created */
   count = _ftest_count_parts();

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (count == (start_count + 2*EXTRA_PARTS)), "Test #1 2.6.2 Uninstalling MFS - extra partitions created");
   if (count != (start_count + 2*EXTRA_PARTS)) {
      return;
   } /* Endif */

   /* Run the partition test to make sure nothing was corrupted */
   error_code = _partition_test(&part_id, &pool_in_err_ptr, &block_in_err_ptr);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 2.7 Partition test");

   if (error_code) {
      return;
   }

   mfs_fd_ptr = fopen(MFS, 0);

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (mfs_fd_ptr == 0), "Test #1 2.8 Open file system without installing first");
   if (mfs_fd_ptr) {
      return;
   }

#endif    // #if DISK_FOR_MFS_TEST == RAM_DISK
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : void tc_3_main_task(void)
* Comments     : Testing MFS install/uninstall on SDcard disk
*
*END*----------------------------------------------------------------------*/

void tc_3_main_task(void)
{
#if DISK_FOR_MFS_TEST == SDCARD_DISK

#if defined BSP_SDCARD_ESDHC_CHANNEL
#if ! BSPCFG_ENABLE_ESDHC
#error "This application requires BSPCFG_ENABLE_ESDHC defined non-zero in user_config.h. Please recompile libraries with this option."
#else
#define SDCARD_COM_CHANNEL BSP_SDCARD_ESDHC_CHANNEL
#endif

#elif defined BSP_SDCARD_SDHC_CHANNEL
#if ! BSPCFG_ENABLE_SDHC
#error "This application requires BSPCFG_ENABLE_SDHC defined non-zero in user_config.h. Please recompile libraries with this option."
#else
#define SDCARD_COM_CHANNEL BSP_SDCARD_SDHC_CHANNEL
#endif

#elif defined BSP_SDCARD_SPI_CHANNEL
#define SDCARD_COM_CHANNEL BSP_SDCARD_SPI_CHANNEL
#else
#error "SDCARD low level communication device not defined!"
#endif

#define MFS_DEVICE          "sdcard:"
#define MFS                 "mfs:"
#define PARTITION_MANAGER   "pm:"
#define PARTITION_1         "pm:1"
#define PARTITION_2         "pm:2"

   uint32_t                   error_code;
   MQX_FILE_PTR               com_handle,
                              sdcard_handle,
                              partition_handle,
                              mfs_handle;
   PMGR_PART_INFO_STRUCT      part_info;
   uint32_t                   part_id;

   _int_install_unexpected_isr();

   /* Open low level communication device */
   com_handle = fopen(SDCARD_COM_CHANNEL, (void *)(SPI_FLAG_FULL_DUPLEX));
   if (NULL == com_handle)
   {
       printf("Error opening communication handle %s, check your user_config.h.\n", SDCARD_COM_CHANNEL);
       return;
   }

   /* Install SD card device */
   error_code = _io_sdcard_install(MFS_DEVICE, (void *)&_bsp_sdcard0_init, com_handle);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MQX_OK ), "Test #1 1.0 _io_sdcard_intall");
   if ( error_code != MQX_OK )
   {
       return;
   }

   /* Open the device which MFS will be installed on */
   sdcard_handle = fopen(MFS_DEVICE, MEM_OPEN_MODE);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (sdcard_handle != NULL ), "Test #1 1.1 fopen sd card");
   if (sdcard_handle == NULL)
   {
      return;
   }

   /* Install partition manager over SD card driver */
   error_code = _io_part_mgr_install(sdcard_handle, PARTITION_MANAGER, 0);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR ), "Test #1 2.0.0 Install partition manager");
   if (error_code != MFS_NO_ERROR)
   {
	   return;
   }

   /* Open partition manager */
   partition_handle = fopen(PARTITION_MANAGER, NULL);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (partition_handle != NULL ), "Test #1 2.0.1 fopen partition manager");
   if (partition_handle == NULL)
   {
	   return;
   }

   /* Create two partitions */
   /* A 1-Gigabyte partition : partition 1 */
   part_info.SLOT = 1;
   part_info.TYPE = PMGR_PARTITION_FAT32_LBA;
   part_info.START_SECTOR = 32;
   part_info.LENGTH = 2097152;
   error_code = ioctl(partition_handle, IO_IOCTL_SET_PARTITION, (uint32_t *) &part_info);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.1.0 Create a 1-Gigabyte partition");
   if (error_code != MFS_NO_ERROR)
   {
      return;
   }

   /* A 2-Gigabyte partition : partition 2 */
   part_info.SLOT = 2;
   part_info.TYPE = PMGR_PARTITION_FAT32_LBA;
   part_info.START_SECTOR = 2097184;
   part_info.LENGTH = 4194304;
   error_code = ioctl(partition_handle, IO_IOCTL_SET_PARTITION, (uint32_t *) &part_info);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.1.1 Create a 2-Gigabyte partition");
   if (error_code != MFS_NO_ERROR)
   {
      return;
   }

   /* Open partition 1 */
   part_id = 1;
   error_code = ioctl(partition_handle, IO_IOCTL_SEL_PART, (uint32_t *) &part_id);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.2.0 Open the first partition");
   if (error_code != MFS_NO_ERROR)
   {
      return;
   }

   /* Validate partition 1 */
   error_code = _io_ioctl(partition_handle, IO_IOCTL_VAL_PART, NULL);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.2.1 Validate the first partition");
   if (error_code != MFS_NO_ERROR)
   {
      return;
   }

   /* Install MFS over partition 1 */
   error_code = _io_mfs_install(partition_handle, MFS, 0);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.2.2 Install MFS on the first partition");
   if (error_code != MFS_NO_ERROR)
   {
      return;
   }

   /* Open filesystem on partition 1 */
   mfs_handle = fopen(MFS, NULL);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (mfs_handle != NULL ), "Test #1 2.2.3 fopen filesystem");
   if (mfs_handle == NULL)
   {
      return;
   }

   /* Close the filesystem and uninstall MFS on partition 1 */
   if (MFS_NO_ERROR != fclose(mfs_handle))
   {
      return;
   }
   mfs_handle = NULL;
   error_code = _io_dev_uninstall(MFS);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.2.4 Uninstall MFS on the first partition");
   if (error_code != MFS_NO_ERROR)
   {
      return;
   }

   /* Open partition 2 */
   part_id = 0;
   error_code = ioctl(partition_handle, IO_IOCTL_SEL_PART, (uint32_t *) &part_id);
   part_id = 2;
   error_code = ioctl(partition_handle, IO_IOCTL_SEL_PART, (uint32_t *) &part_id);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.2.5 Open the second partition");
   if (error_code != MFS_NO_ERROR)
   {
      return;
   }

   /* Validate partition 2 */
   error_code = _io_ioctl(partition_handle, IO_IOCTL_VAL_PART, NULL);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.2.6 Validate the second partition");
   if (error_code != MFS_NO_ERROR)
   {
      return;
   }

   /* Install MFS over partition 2 */
   error_code = _io_mfs_install(partition_handle, MFS, 0);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.2.7 Install MFS on the second partition");
   if (error_code != MFS_NO_ERROR)
   {
      return;
   }

   /* Open filesystem on partition 2 */
   mfs_handle = fopen(MFS, NULL);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (mfs_handle != NULL ), "Test #1 2.2.8 fopen filesystem (2)");
   if (mfs_handle == NULL)
   {
      return;
   }

   /* Close the filesystem and uninstall MFS on partition 2 */
   if (MFS_NO_ERROR != fclose(mfs_handle))
   {
      return;
   }
   mfs_handle = NULL;
   error_code = _io_dev_uninstall(MFS);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.2.9 Uninstall MFS on the second partition");
   if (error_code != MFS_NO_ERROR)
   {
      return;
   }

   /* Clear the first partition */
   part_id = 0;
   error_code = ioctl(partition_handle, IO_IOCTL_SEL_PART, (uint32_t *) &part_id);
   part_id = 1;
   error_code = ioctl(partition_handle, IO_IOCTL_CLEAR_PARTITION, (uint32_t *) &part_id);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.3.0 Clear the first partition");
   if (error_code != MFS_NO_ERROR)
   {
      return;
   }

   /* Clear the second partition */
   part_id = 0;
   error_code = ioctl(partition_handle, IO_IOCTL_SEL_PART, (uint32_t *) &part_id);
   part_id = 2;
   error_code = ioctl(partition_handle, IO_IOCTL_CLEAR_PARTITION, (uint32_t *) &part_id);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.3.1 Clear the second partition");
   if (error_code != MFS_NO_ERROR)
   {
      return;
   }

   /* Close partition */
   error_code = fclose(partition_handle);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.3.2 Close partition");
   if (error_code != MFS_NO_ERROR)
   {
      return;
   }
   partition_handle = NULL;

   /* Uninstall partition manager  */
   error_code = _io_dev_uninstall(PARTITION_MANAGER);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.3.3 Uninstall partition manager");
   if (error_code != MFS_NO_ERROR)
   {
     return;
   }

   /* Close the SD card device */
   error_code = fclose(sdcard_handle);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_NO_ERROR), "Test #1 2.3.4 Close the SD card device");
   if (error_code != MFS_NO_ERROR)
   {
	   return;
   }
   sdcard_handle = NULL;

#endif    // #if DISK_FOR_MFS_TEST == SDCARD_DISK
}


/*FUNCTION*-----------------------------------------------------------------
*
* Task Name    : _ftest_count_parts
* Comments     :
*
*END*----------------------------------------------------------------------*/

uint32_t _ftest_count_parts
   (
      void
   )
{
   uint32_t                   count = 0;
   KERNEL_DATA_STRUCT_PTR     kernel_data;
#if MQX_VERSION >= 250
   PARTITION_COMPONENT_STRUCT_PTR      part_component_ptr;
#else
   PARTPOOL_STRUCT_PTR        partpool_ptr;
#endif


   _GET_KERNEL_DATA(kernel_data);

#if MQX_VERSION >= 250
   part_component_ptr = (PARTITION_COMPONENT_STRUCT_PTR)
      kernel_data->KERNEL_COMPONENTS[KERNEL_PARTITIONS];

   if (part_component_ptr) {
      count = part_component_ptr->PARTITIONS.SIZE;
   } else {
      count = 0;
   } /* Endif */
#else
   /* Count partitions. We don't know exactly how many there will be at the start */
   count = 0;
   partpool_ptr = kernel_data->PARTITION_PTR;
   while(partpool_ptr) {
      count++;
      partpool_ptr = partpool_ptr->NEXT_PARTITION_PTR;
   } /* Endwhile */
#endif

   return count;
}


/******************** End Testcases ********************/

/******************** Register tests *******************/
EU_TEST_SUITE_BEGIN(suite_mfs)
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_TEST_CASE_ADD(tc_1_main_task, "- Testing install/uninstall on USB disk"),
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_TEST_CASE_ADD(tc_2_main_task, "- Testing install/uninstall on RAM disk"),
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_TEST_CASE_ADD(tc_3_main_task, "- Testing install/uninstall on SDCARD disk"),
#endif
EU_TEST_SUITE_END(suite_mfs)

EU_TEST_REGISTRY_BEGIN()
   EU_TEST_SUITE_ADD(suite_mfs, "Testing MFS install/uninstall")
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
