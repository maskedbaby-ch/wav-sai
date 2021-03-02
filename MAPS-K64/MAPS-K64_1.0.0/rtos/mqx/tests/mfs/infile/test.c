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
* $Version : 3.8.10.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file tests the infile navigation in MFS
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

#if DISK_FOR_MFS_TEST == USB_DISK
	USB_FILESYSTEM_STRUCT_PTR usb_fs_handle;
#endif 

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK,      main_task,      3000L,  11L, "Main",  MQX_AUTO_START_TASK},
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

MQX_FILE_PTR               mfs_fd_ptr=NULL;
char                       write_buffer[2048];

/*TASK*-------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : Testing infile navigation on USB disk
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
* Task Name    : tc_2_main_task
* Comments     : Testing infile navigation on RAM disk
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
   uint64_t                   size,i,j;
   uint32_t                   error_code;
   MQX_FILE_PTR               file_fd_ptr;
   int32_t                    size_to_write, size_written,eof;     
   char                       value;                    
   char                       filename[30];

   _int_install_unexpected_isr();

#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mfs_fd_ptr != NULL), "Test #1 Creating File System");
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (mfs_fd_ptr != NULL), "Test #1 Creating File System");
#endif
   if(!mfs_fd_ptr){
   	return;
   }  
   for(i=0; i<2048;i++){
      write_buffer[i]= i&0xFF;
   }

   ioctl(mfs_fd_ptr, IO_IOCTL_FAT_CACHE_ON, NULL);
   ioctl(mfs_fd_ptr, IO_IOCTL_WRITE_CACHE_ON, NULL);

   /* Create a file  */
   sprintf(filename, "%s\\vadim.txt", MFS);
   file_fd_ptr = fopen(filename, "wb");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file_fd_ptr != NULL), "Test #1 3.0 Create file for write")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file_fd_ptr != NULL), "Test #1 3.0 Create file for write")
#endif
   if (!file_fd_ptr) {
        return;
   }

/* Get disk space */
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FREE_SPACE, &i);
   size = (NUM_SECTORS-RESERVED_SECTS)*512;
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( error_code == MQX_OK ), "Test #1 3.1 Get disk space IOCTL")
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( i <= size), "Test #1 3.1 Get disk space")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( error_code == MQX_OK ), "Test #1 3.1 Get disk space IOCTL")
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( i <= size), "Test #1 3.1 Get disk space")
#endif
   if (!i) {
      return;
   } else if (i > size) {
      return;
   }

/* Write data to file */
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( i >= FILESIZE   ), "Test #1 3.2 Enough space available")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( i >= FILESIZE   ), "Test #1 3.2 Enough space available")
#endif
   if(i < FILESIZE ){
      return;
   }
   
//*Write data to the file *//
    size = FILESIZE;
    j    = 0;
    
   while (size >0) {
      if (size > 512) {
         size_to_write = 512;
      } else {
         size_to_write = size;
      }
      j = size_to_write;
      size_written=0;
      size_written += fwrite(&write_buffer,sizeof(char) , size_to_write, file_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( size_written == size_to_write   ), "Test #1 3.3 Writing")
#else
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( size_written == size_to_write   ), "Test #1 3.3 Writing")
#endif
      if (size_written != size_to_write) {
         return;
      }
      size -= size_to_write;  
   }

   error_code = fclose(file_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 3.4 Close file")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 3.4 Close file")
#endif
   if (error_code != MFS_NO_ERROR) {
      return;   
   }

   /* 3.5 Flush output */
   error_code = fflush(mfs_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.5 Flush output")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == MFS_NO_ERROR), "Test #1 3.5 Flush output")
#endif
   if (error_code) {
      return;   
   }
   
/* Open the file for reading  */
   file_fd_ptr = fopen(filename, "rb");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file_fd_ptr != NULL), "Test #1 4.0 Open the file for reading")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file_fd_ptr != NULL), "Test #1 4.0 Open the file for reading")
#endif
   if (!file_fd_ptr) {
      return;
   }
   
//*Read data from the file *//
   j = 0;
   size = 0;
   eof=feof(file_fd_ptr);
   while (!eof){
      size += fread(&value, sizeof(char),1, file_fd_ptr);
      eof=feof(file_fd_ptr);    
      if(!eof){
#if DISK_FOR_MFS_TEST == USB_DISK
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (value == write_buffer[j]), "Test #1 4.1 Testing a read value")
#else
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (value == write_buffer[j]), "Test #1 4.1 Testing a read value")
#endif
         if(value != write_buffer[j]){
            return;  
         }
      }
      j++;
      if(j>=512){
         j=0;
      }
   }

#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (size == FILESIZE), "Test #1 4.2 Wrong file size")    
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (size == FILESIZE), "Test #1 4.2 Wrong file size")    
#endif
   if(size != FILESIZE){
      return;
   }

   error_code = fclose(file_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 4.3 Close file")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 4.3 Close file")
#endif
   if (error_code != MFS_NO_ERROR) {
      return;   
   }

/* Open the file for append  */
   file_fd_ptr = fopen(filename, "a+");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file_fd_ptr != NULL), "Test #1 5.0 Open the file in append mode")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file_fd_ptr != NULL), "Test #1 5.0 Open the file in append mode")
#endif
   if (!file_fd_ptr) {
      return;
   }
   
//*Read and test data from the start of the file *//
   fseek(file_fd_ptr, 0, IO_SEEK_SET );
   j = 0;
   size = 0;
   eof=feof(file_fd_ptr);
   while (!eof){
      size += fread(&value, sizeof(char),1, file_fd_ptr);
      eof=feof(file_fd_ptr);    
      if(!eof){
#if DISK_FOR_MFS_TEST == USB_DISK
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (value == write_buffer[j]), "Test #1 5.1.0 Testing a read from the start of the file")
#else
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (value == write_buffer[j]), "Test #1 5.1.0 Testing a read from the start of the file")
#endif
         if(value != write_buffer[j]){
            return;  
         }
      }
      j++;
      if(j>=512) 
      {
         j=0;
      }
   }

#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (size == FILESIZE), "Test #1 5.1.1 Wrong file size")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (size == FILESIZE), "Test #1 5.1.1 Wrong file size")
#endif
   if(size != FILESIZE){
      return;
   }

/*Read and test data from the midle of the sector */
   fseek(file_fd_ptr, 500, IO_SEEK_SET );
   j = 500;
   size = 0;
   eof=feof(file_fd_ptr);
   while (!eof){
      size += fread(&value, sizeof(char),1, file_fd_ptr);
      eof=feof(file_fd_ptr);    
      if(!eof){
#if DISK_FOR_MFS_TEST == USB_DISK
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (value == write_buffer[j]), "Test #1 5.2.0 Testing start reading from midle of a sector")
#else
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (value == write_buffer[j]), "Test #1 5.2.0 Testing start reading from midle of a sector")
#endif
         if(value != write_buffer[j]){
           return;  
         }
      }
      j++;
      if(j>=512) 
      {
         j=0;
      }
   }
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (size == FILESIZE-500), "Test #1 5.2.1 Wrong file size")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (size == FILESIZE-500), "Test #1 5.2.1 Wrong file size")
#endif
   if(size != FILESIZE - 500){
      return;
   }


/*Read and test data from the start of the sector */
   fseek(file_fd_ptr, 512, IO_SEEK_SET );
   j = 0;
   size = 0;
   eof=feof(file_fd_ptr);
   while (!eof){
      size += fread(&value, sizeof(char),1, file_fd_ptr);
      eof=feof(file_fd_ptr);    
      if(!eof){
#if DISK_FOR_MFS_TEST == USB_DISK
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (value == write_buffer[j]), "Test #1 5.3.0 Testing start reading from the start of the sector")
#else
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (value == write_buffer[j]), "Test #1 5.3.0 Testing start reading from the start of the sector")
#endif
         if(value != write_buffer[j]){
            return;  
         }
      }
      j++;
      if(j>=512) 
      {
         j=0;
      }
   }

#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (size == FILESIZE-512), "Test #1 5.3.1 Wrong file size")    
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (size == FILESIZE-512), "Test #1 5.3.1 Wrong file size")    
#endif
   if(size != FILESIZE - 512){
      return;
   }

/*Read and test data from the midle of the next sector */
   fseek(file_fd_ptr, 1000, IO_SEEK_SET );
   j = 488;
   size = 0;
   eof=feof(file_fd_ptr);
   while (!eof){
      size += fread(&value, sizeof(char),1, file_fd_ptr);
      eof=feof(file_fd_ptr);    
      if(!eof){
#if DISK_FOR_MFS_TEST == USB_DISK
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (value == write_buffer[j]), "Test #1 5.3.0 Testing start reading from midle of the next sector")
#else
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (value == write_buffer[j]), "Test #1 5.3.0 Testing start reading from midle of the next sector")
#endif
         if(value != write_buffer[j]){
            return;  
         }
      }
      j++;
      if(j>=512){
         j=0;
      }
   }

#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (size == FILESIZE-1000), "Test #1 5.3.1 Wrong file size")    
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (size == FILESIZE-1000), "Test #1 5.3.1 Wrong file size")    
#endif
   if(size != FILESIZE - 1000){
      return;
   }

/*Rewrite and test data from the midle of the  sector, size = 1 sector */
   fseek(file_fd_ptr, 500, IO_SEEK_SET );
   size_to_write = 512;
   size_written =  fwrite(&write_buffer,sizeof(char) , size_to_write, file_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( size_written == size_to_write   ), "Test #1 6.1.0 Rewrite and test data from the midle of the  sector")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( size_written == size_to_write   ), "Test #1 6.1.0 Rewrite and test data from the midle of the  sector")
#endif
   if (size_written != size_to_write) {
      return;
   }

//Read data
   fseek(file_fd_ptr, 0, IO_SEEK_SET );
   j = 0;
   size = 0;
   eof=feof(file_fd_ptr);
   while (!eof){
      size += fread(&value, sizeof(char),1, file_fd_ptr);
      eof=feof(file_fd_ptr);    
      if(!eof){
#if DISK_FOR_MFS_TEST == USB_DISK
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (value == write_buffer[j]), "Test #1 6.1.1 Testing start reading from midle of the next sector")
#else
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (value == write_buffer[j]), "Test #1 6.1.1 Testing start reading from midle of the next sector")
#endif
         if(value != write_buffer[j]){
            return;  
         }
      }
      j++;
      if( size==500){
         j=0;
      }
      if( size ==500+512){
         j=500;
      }
      if( j >= 512){
         j=0;
      }
   }

#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (size == FILESIZE), "Test #1 6.1.2 Wrong file size")    
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (size == FILESIZE), "Test #1 6.1.2 Wrong file size")    
#endif
   if(size != FILESIZE){
      return;
   }

/*Rewrite and test data from the midle of the  sector, size more 2 sectors */
   fseek(file_fd_ptr, 300, IO_SEEK_SET );
   size_to_write = 512+512+100;
   size_written =  fwrite(&write_buffer,sizeof(char) , size_to_write, file_fd_ptr);

#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( size_written == size_to_write   ), "Test #1 6.2.0 Rewrite and test data from the midle of the  sector")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( size_written == size_to_write   ), "Test #1 6.2.0 Rewrite and test data from the midle of the  sector")
#endif
   if (size_written != size_to_write) {
      return;
   }

//Read data
   fseek(file_fd_ptr, 0, IO_SEEK_SET );
   j = 0;
   size = 0;
   eof=feof(file_fd_ptr);
   while (!eof){
      size += fread(&value, sizeof(char),1, file_fd_ptr);
      eof=feof(file_fd_ptr);    
      if(!eof){
#if DISK_FOR_MFS_TEST == USB_DISK
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (value == write_buffer[j]), "Test #1 6.2.1 Testing start reading from midle of the next sector")
#else
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (value == write_buffer[j]), "Test #1 6.2.1 Testing start reading from midle of the next sector")
#endif
         if(value != write_buffer[j]){
            return;  
         }
      }
      j++;
      if( size==300){
         j=0;
      }
      if( size ==300+512+512+100){
         j=300+100;
      }
      if( j >= 512){
         j=0;
      }
   }

#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (size == FILESIZE), "Test #1 6.2.2 Wrong file size")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (size == FILESIZE), "Test #1 6.2.2 Wrong file size")
#endif
   if(size != FILESIZE){
      return;
   }

   error_code = fclose(file_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #6.2.3 Close file")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #6.2.3 Close file")
#endif
   if (error_code != MFS_NO_ERROR) {
      return;   
   }
   
/* Open the file for append  */
   
   file_fd_ptr = fopen(filename, "wb");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file_fd_ptr != NULL), "Test #1 7.0.0 Create file for write")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file_fd_ptr != NULL), "Test #1 7.0.0 Create file for write")
#endif
   if (!file_fd_ptr) {
      return;
   }
   
//*Write data to the file *//
   size = FILESIZE;
   j    = 0;
    
   while (size >0) {
      if (size > 512) {
         size_to_write = 512;
      } else {
         size_to_write = size;
      }
      j = size_to_write;
      size_written=0;
      size_written += fwrite(&write_buffer,sizeof(char) , size_to_write, file_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( size_written == size_to_write   ), "Test #1 7.0.1 Writing")
#else
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( size_written == size_to_write   ), "Test #1 7.0.1 Writing")
#endif
      if (size_written != size_to_write) {
         return;
      }
      size -= size_to_write;  
   } /* Endwhile */

   error_code = fclose(file_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test # 7.0.2 Close file")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test # 7.0.2 Close file")
#endif
   if (error_code != MFS_NO_ERROR) {
      return;   
   }
   
   file_fd_ptr = fopen(filename, "a+");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file_fd_ptr != NULL), "Test #1 7.1.0 Open the file in append mode")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file_fd_ptr != NULL), "Test #1 7.1.0 Open the file in append mode")
#endif
   if (!file_fd_ptr) {
      return;
   }

   /*append data to the file */
   size_to_write = 512;
   size_written =  fwrite(&write_buffer,sizeof(char) , size_to_write, file_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( size_written == size_to_write   ), "Test #1 7.1.1 append 1 sector data to the file")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( size_written == size_to_write   ), "Test #1 7.1.1 append 1 sector data to the file")
#endif
   if (size_written != size_to_write) {
      return;
   }
    
   /*Read and test data*/
   fseek(file_fd_ptr, 0, IO_SEEK_SET );
   j = 0;
   size = 0;
   eof=feof(file_fd_ptr);
   while (!eof){
      size += fread(&value, sizeof(char),1, file_fd_ptr);
      eof=feof(file_fd_ptr);    
      if(!eof){
#if DISK_FOR_MFS_TEST == USB_DISK
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (value == write_buffer[j]), "Test #1 7.1.2 Testing file data")
#else
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (value == write_buffer[j]), "Test #1 7.1.2 Testing file data")
#endif
         if(value != write_buffer[j]) {
            return;  
         }
      }
      j++;
      if( size==FILESIZE) {
         j=0;
      }
      if( j >= 512) {
         j=0;
      }
   }

#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (size == FILESIZE+512), "Test #1  7.1.3 Wrong file size")    
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (size == FILESIZE+512), "Test #1  7.1.3 Wrong file size")    
#endif
   if(size != FILESIZE+512){
      return;
   }

   error_code = fclose(file_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 7.1.4 Close file")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 7.1.4 Close file")
#endif
   if (error_code != MFS_NO_ERROR) {
      return;   
   }
    
    
/* Open the file in append mode and write more SIZETOAPPEND bytes   */
   file_fd_ptr = fopen(filename, "a+");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file_fd_ptr != NULL), "Test #1 8.0.0 Open the file in append mode")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file_fd_ptr != NULL), "Test #1 8.0.0 Open the file in append mode")
#endif
   if (!file_fd_ptr) {
      return;
   }
   
   /*append data to the file */
   size = SIZETOAPPEND;//512*2+400 = 1424
   j    = 0;
   i    = 0;
   while (size >0) {
      if (size > 2048) {
         size_to_write = 2048;
      } else {
         size_to_write = size;
      }
      j = size_to_write;
      //size_written=0;
      size_written = fwrite(&write_buffer,sizeof(char) , size_to_write, file_fd_ptr);
        
      i += size_written;
#if DISK_FOR_MFS_TEST == USB_DISK
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( size_written == size_to_write   ), "Test #1 8.0.1 append  more  2 sectors of data to the file")
#else
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( size_written == size_to_write   ), "Test #1 8.0.1 append  more  2 sectors of data to the file")
#endif
      if (size_written != size_to_write) {
         return;
      }
      size -= size_to_write;  
   } /* Endwhile */

#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ( i == SIZETOAPPEND   ), "Test #1 8.0.2 append  more  2 sectors of data to the file")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( i == SIZETOAPPEND   ), "Test #1 8.0.2 append  more  2 sectors of data to the file")
#endif
   if (i != SIZETOAPPEND) {
      return;
   } /* Endif */   

   /*Read and test data*/
   fseek(file_fd_ptr, 0, IO_SEEK_SET );
   j = 0;
   size = 0;
   eof=feof(file_fd_ptr);
   while (!eof){
      size += fread(&value, sizeof(char),1, file_fd_ptr);
      eof=feof(file_fd_ptr);    
      if(!eof){
#if DISK_FOR_MFS_TEST == USB_DISK
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (value == write_buffer[j]), "Test #1 8.1.0 Testing file data")
#else
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (value == write_buffer[j]), "Test #1 8.1.0 Testing file data")
#endif
         if(value != write_buffer[j]) {
            return;  
         }
      }
      j++;
      if( size==FILESIZE) {
         j=0;
      }
      if( j >= 512) {
         j=0;
      }
   }

#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (size == FILESIZE+512+SIZETOAPPEND), "Test #1  8.1.1 Wrong file size")    
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (size == FILESIZE+512+SIZETOAPPEND), "Test #1  8.1.1 Wrong file size")    
#endif
   if(size != FILESIZE+512+SIZETOAPPEND) {
      return;
   }

   error_code = fclose(file_fd_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 8.1.2 Close file")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 8.1.2 Close file")
#endif
   if (error_code != MFS_NO_ERROR) {
      return;   
   }
}
         
/******************** End Testcases ********************/

/******************** Register tests *******************/
EU_TEST_SUITE_BEGIN(suite_mfs)
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_TEST_CASE_ADD(tc_1_main_task, "- Testing infile navigation on USB disk"),
#else   
   EU_TEST_CASE_ADD(tc_2_main_task, "- Testing infile navigation on RAM disk"),
#endif   
EU_TEST_SUITE_END(suite_mfs)

EU_TEST_REGISTRY_BEGIN()
   EU_TEST_SUITE_ADD(suite_mfs, "Testing MFS infile navigation")
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
