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
* $Version : 3.8.9.0$
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
#include "io_mem.h"
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test
void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
void test_task(void);
uint32_t fill_extra_sector(MQX_FILE_PTR);
uint32_t check_extra_sector(MQX_FILE_PTR);
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
#if defined(TI320C30D) || defined(TI320C40D)
   { MAIN_TASK,      main_task,       200L,  11L, "Main",  MQX_AUTO_START_TASK},
#else
   { MAIN_TASK,      main_task,      2000L,  11L, "Main",  MQX_AUTO_START_TASK},
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
	USB_FILESYSTEM_STRUCT_PTR usb_fs_handle;
	void callback_fn(USB_STATUS, void *, void *, uint32_t);
#endif

MQX_FILE_PTR   mfs_fd_ptr=NULL;
char           write_buffer[512];   

/*TASK*-------------------------------------------------------------------
*
* Task Name    : void tc_1_main_task(void)
* Comments     : Testing MFS full feature on USB disk
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
* Comments     : Testing MFS full feature on RAM disk
*
*END*----------------------------------------------------------------------*/

void tc_2_main_task(void)
{
#if DISK_FOR_MFS_TEST == RAM_DISK
   #define MFS       	"MFS:"
   mfs_fd_ptr = (MQX_FILE_PTR)Init_MFS_RAM(MFS_format.BYTES_PER_SECTOR * (NUM_SECTORS + 1));
   test_task();
#endif
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : void tc_3_main_task(void)
* Comments     : Testing MFS full feature on SD Card disk
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
   uint32_t                   error_code, j;
   uint64_t                   size, i;
   MQX_FILE_PTR               file_fd_ptr;
   char                       *filepath;
   MFS_SEARCH_DATA            search_data;
   MFS_SEARCH_PARAM           search;
   int32_t                    size_to_write, size_written;
   
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
   filepath = _mem_alloc_system_zero(PATHNAME_SIZE + 1);

   error_code = fill_extra_sector(mfs_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 Filling the extra sector");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 Filling the extra sector");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 Filling the extra sector");
#endif
	if(error_code != 0){
		return;
	}
   
	error_code = check_extra_sector(mfs_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 Checking the extra sector");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 Checking the extra sector");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 Checking the extra sector");
#endif
	if(error_code != 0){
		return;
	}
   
   /* Create a file  */
   sprintf(filepath, "%s\\%s", MFS, FILE_NAME);
   file_fd_ptr = fopen(filepath, "n+");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file_fd_ptr != NULL), "Test #1 3.0 Create file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file_fd_ptr != NULL), "Test #1 3.0 Create file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (file_fd_ptr != NULL), "Test #1 3.0 Create file");
#endif
   if (!file_fd_ptr) {
      return;
   }
   
   /* Get disk space */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FREE_SPACE, &i);
   size = (NUM_SECTORS-RESERVED_SECTS)*512;
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( error_code == MQX_OK ), "Test #1 3.1 Get disk space IOCTL");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( i <= size), "Test #1 3.1 Get disk space");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( error_code == MQX_OK ), "Test #1 3.1 Get disk space IOCTL");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( i <= size), "Test #1 3.1 Get disk space");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ( error_code == MQX_OK ), "Test #1 3.1 Get disk space IOCTL");
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ( i <= size), "Test #1 3.1 Get disk space");
#endif
   
   if (!i) {
      return;
   } else if (i > size) {
       return;
   }
   
   /* Write up to i-1 bytes to file */
   size = i - 1;
   j    = 0;
   while (size) {
      if (size > 512) {
         size_to_write = 512;
      } else {
         size_to_write = size;
      } /* Endif */
      size_written = write(file_fd_ptr, write_buffer, size_to_write);
      if (size_written != size_to_write) {
         break;
      } /* Endif */
      j += size_written;
      size -= size_to_write;
   } /* Endwhile */

#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( j == (i-1)), "Test #1 3.2 Write to 1 bytes from end of disk");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( j == (i-1)), "Test #1 3.2 Write to 1 bytes from end of disk");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ( j == (i-1)), "Test #1 3.2 Write to 1 bytes from end of disk");
#endif
   if (j != (i-1)) {
        return;
   }
   
   /* Check for end of file */
   error_code = ferror(file_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( error_code == 0), "Test #1 3.3 Check for end of file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( error_code == 0), "Test #1 3.3 Check for end of file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ( error_code == 0), "Test #1 3.3 Check for end of file");
#endif
   if (error_code) {
      return;
   }
  
   /* Write 1 byte */
   j = write(file_fd_ptr, write_buffer, 1);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( j == 1), "Test #1 3.4 Write 1 byte");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( j == 1), "Test #1 3.4 Write 1 byte");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ( j == 1), "Test #1 3.4 Write 1 byte");
#endif
   if (j != 1) {
      return; 
   }
   
   error_code = ferror(file_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( error_code == 0), "Test #1 3.4 Write 1 byte");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( error_code == 0), "Test #1 3.4 Write 1 byte");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ( error_code == 0), "Test #1 3.4 Write 1 byte");
#endif
   if (error_code) {
      return;
   }
   
   /* Write 1 one more byte */
   j = write(file_fd_ptr, write_buffer, 1);
   error_code = ferror(file_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( error_code == MFS_DISK_FULL), "Test #1 3.5 Write 1 one more byte");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( error_code == MFS_DISK_FULL), "Test #1 3.5 Write 1 one more byte");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ( error_code == MFS_DISK_FULL), "Test #1 3.5 Write 1 one more byte");
#endif
   if (error_code != MFS_DISK_FULL) {
      return;
   }

   /* Close file */
   error_code = fclose(file_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( error_code == 0), "Test #1 3.6 Closing file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( error_code == 0), "Test #1 3.6 Closing file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ( error_code == 0), "Test #1 3.6 Closing file");
#endif
   if (error_code) {
      return;
   }
   fflush(mfs_fd_ptr);

   /* Check the extra sector for the known pattern */
   error_code = check_extra_sector(mfs_fd_ptr); 
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 3.7 Writing off the end");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 3.7 Writing off the end");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 3.7 Writing off the end");
#endif
   if (error_code != 0) {
      return;
   }
   
   /* Find file */
   search.ATTRIBUTE       = MFS_ATTR_ANY;
   search.WILDCARD        = "*.*";
   search.SEARCH_DATA_PTR = &search_data;   
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_FIRST_FILE,(uint32_t *)&search);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 3.8.0 Find file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 3.8.0 Find file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 3.8.0 Find file");
#endif
   if (error_code) {
      return;
   }

   j = strncmp(search_data.NAME, FILE_NAME, FILENAME_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (j == 0), "Test #1 3.8.0 Find file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (j == 0), "Test #1 3.8.0 Find file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (j == 0), "Test #1 3.8.0 Find file");
#endif
   if (j) {
      return;
   }
   
   /* Find more files */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_NEXT_FILE,(uint32_t *)&search_data);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_FILE_NOT_FOUND), "Test #1 3.8.1 Find more files");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_FILE_NOT_FOUND), "Test #1 3.8.1 Find more files");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MFS_FILE_NOT_FOUND), "Test #1 3.8.1 Find more files");
#endif
   if (error_code != MFS_FILE_NOT_FOUND) {
      return;
   }
   
   /* Close file */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_DELETE_FILE, filepath);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 3.9 Deleting file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 3.9 Deleting file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 3.9 Deleting file");
#endif
   if (error_code) {
      return;
   }

   /* Check free space */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FREE_SPACE, &size);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MQX_OK), "Test #1 3.10 Check free space IOCTL after deleting file");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (size == i), "Test #1 3.10 Check free space after deleting file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MQX_OK), "Test #1 3.10 Check free space IOCTL after deleting file");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (size == i), "Test #1 3.10 Check free space after deleting file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == MQX_OK), "Test #1 3.10 Check free space IOCTL after deleting file");
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (size == i), "Test #1 3.10 Check free space after deleting file");
#endif
   if (size != i) {
      return;
   }
   
   /* Re-create the file */
   file_fd_ptr = fopen(filepath,"n+");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file_fd_ptr != NULL), "Test #1 4.0 Re-creating file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file_fd_ptr != NULL), "Test #1 4.0 Re-creating file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (file_fd_ptr != NULL), "Test #1 4.0 Re-creating file");
#endif
   if (!file_fd_ptr) {
      return;
   }
   
   /* Make file as big as possible */
   size = i;
   j    = 0;
   while (size) {
      if (size > 512) {
         size_to_write = 512;
      } else {
         size_to_write = size;
      } /* Endif */
      size_written = write(file_fd_ptr, write_buffer, size_to_write);
      if (size_written != size_to_write) {
         break;
      } /* Endif */
      j += size_written;
      size -= size_to_write;
   } /* Endwhile */

#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (j == i), "Test #1 4.1 Writing to file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (j == i), "Test #1 4.1 Writing to file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (j == i), "Test #1 4.1 Writing to file");
#endif
   if (j != i) {
      return;
   }
   
   fclose(file_fd_ptr);

} /* Endbody */

uint32_t check_extra_sector(MQX_FILE_PTR mfs_fd_ptr)
{
#if DISK_FOR_MFS_TEST == RAM_DISK
    char      		*data;
	MQX_FILE_PTR 	handle;
	uint32_t	    error_code, i;
   	
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_GET_DEVICE_HANDLE, &handle);
   error_code = ioctl(handle, IO_MEM_IOCTL_GET_BASE_ADDRESS, &data);
   if(error_code !=0)
   	return 1;
   for (i = 0; i < MFS_format.BYTES_PER_SECTOR; i++) {
      if(data[MFS_format.BYTES_PER_SECTOR * NUM_SECTORS + i] != '!')
      	return 2;
   }
   return 0;
#endif

#if DISK_FOR_MFS_TEST == USB_DISK
    unsigned char  *data_buf;
	void           (*org_callback)(USB_STATUS, void *, void *, uint32_t);
	uint32_t	   i;
	
   bCallBack = FALSE;
	pCmd = &((IO_USB_MFS_STRUCT_PTR)usb_fs_handle->DEV_FD_PTR->DEV_PTR->DRIVER_INIT_PTR)->COMMAND;
	org_callback = pCmd->CALLBACK;
	pCmd -> CALLBACK=callback_fn;

	data_buf = _mem_alloc_system_zero(MFS_format.BYTES_PER_SECTOR);
	status = usb_mass_ufi_read_10(pCmd, NUM_SECTORS, data_buf, MFS_format.BYTES_PER_SECTOR, 1);

	if ((status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
	{
		printf ("\n...ERROR");
		return 1;
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
		for(i=0;i<512;i++){
			if(data_buf[i]!='!')
				return 2;
		}
			
	}	
	pCmd->CALLBACK = org_callback;
	return 0;
#endif

#if DISK_FOR_MFS_TEST == SDCARD_DISK
   unsigned char    *data_buf;
   MQX_FILE_PTR 	handle;
   uint32_t			error_code, num, i;

   data_buf = _mem_alloc(MFS_format.BYTES_PER_SECTOR);
   memset(data_buf,' ',MFS_format.BYTES_PER_SECTOR);
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_GET_DEVICE_HANDLE, &handle);
   if(error_code)
      return 1;
#if defined BSP_SDCARD_ESDHC_CHANNEL
   num = _io_sdcard_esdhc_read_block(handle, data_buf, NUM_SECTORS, 1);
#elif defined BSP_SDCARD_SDHC_CHANNEL
   num = _io_sdcard_sdhc_read_block(handle, data_buf, NUM_SECTORS, 1);
#elif defined BSP_SDCARD_SPI_CHANNEL
   num = _io_sdcard_spi_read_block(handle, data_buf, NUM_SECTORS, 1);
#endif
   if (num != 1)
      return 2;
   for(i=0;i<512;i++)
   {
      if(data_buf[i]!='!')
         return 3;
   }

   return 0;
#endif
}
   

uint32_t fill_extra_sector(MQX_FILE_PTR mfs_fd_ptr)
{
#if DISK_FOR_MFS_TEST == RAM_DISK
   char     		*data;
   MQX_FILE_PTR 	handle;
   uint32_t			error_code,i;
   	
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_GET_DEVICE_HANDLE, &handle);
   error_code = ioctl(handle, IO_MEM_IOCTL_GET_BASE_ADDRESS, &data);
   for (i = 0; i < MFS_format.BYTES_PER_SECTOR; i++) {
      data[MFS_format.BYTES_PER_SECTOR * NUM_SECTORS + i] = '!';
   }
   return error_code;
#endif

#if DISK_FOR_MFS_TEST == USB_DISK
    unsigned char  *data_buf;
	void           (*org_callback)(USB_STATUS, void *, void *, uint32_t);
   
   bCallBack = FALSE;
   data_buf = _mem_alloc(MFS_format.BYTES_PER_SECTOR);
   memset(data_buf,'!',MFS_format.BYTES_PER_SECTOR);
	
   pCmd = &((IO_USB_MFS_STRUCT_PTR)usb_fs_handle->DEV_FD_PTR->DEV_PTR->DRIVER_INIT_PTR)->COMMAND;
	org_callback = pCmd->CALLBACK;
	pCmd -> CALLBACK=callback_fn;

	status = usb_mass_ufi_write_10(pCmd, NUM_SECTORS, data_buf, MFS_format.BYTES_PER_SECTOR, 1);

	if ((status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
	{
		printf ("\n...ERROR");
		return 1;
	}
	else
	{
		/* Wait till command comes back */
		while (!bCallBack){;}
		bCallBack = FALSE;			
		if (bStatus) {
			printf("...Unsupported by device (bStatus=0x%x)\n", bStatus);
			return 2;
		}	
	}
	
	pCmd->CALLBACK = org_callback;
	return 0;
#endif

#if DISK_FOR_MFS_TEST == SDCARD_DISK
   unsigned char    *data_buf;
   MQX_FILE_PTR 	handle;
   uint32_t			error_code,num;

   data_buf = _mem_alloc(MFS_format.BYTES_PER_SECTOR);
   memset(data_buf,'!',MFS_format.BYTES_PER_SECTOR);
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_GET_DEVICE_HANDLE, &handle);
   if (error_code)
      return 1;
#if defined BSP_SDCARD_ESDHC_CHANNEL
   num = _io_sdcard_esdhc_write_block(handle, data_buf, NUM_SECTORS, 1);
#elif defined BSP_SDCARD_SDHC_CHANNEL
   num = _io_sdcard_sdhc_write_block(handle, data_buf, NUM_SECTORS, 1);
#elif defined BSP_SDCARD_SPI_CHANNEL
   num = _io_sdcard_spi_write_block(handle, data_buf, NUM_SECTORS, 1);
#endif
   if (num != 1)
      return 2;

   return 0;
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
   EU_TEST_CASE_ADD(tc_1_main_task, "- Testing disk full on USB disk"),
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_TEST_CASE_ADD(tc_2_main_task, "- Testing disk full on RAM disk"),
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_TEST_CASE_ADD(tc_3_main_task, "- Testing disk full on SD Card disk"),
#endif
EU_TEST_SUITE_END(suite_mfs)

EU_TEST_REGISTRY_BEGIN()
   EU_TEST_SUITE_ADD(suite_mfs, "Testing MFS disk full")
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
