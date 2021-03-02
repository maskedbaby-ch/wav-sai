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
void test_task(void);

#if DISK_FOR_MFS_TEST == USB_DISK
	USB_FILESYSTEM_STRUCT_PTR usb_fs_handle;
#endif 

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
#if defined(TI320C30D) || defined(TI320C40D)
   { MAIN_TASK,      main_task,       200L,  11L, "Main",  MQX_AUTO_START_TASK},
#else
   { MAIN_TASK,      main_task,      4000L,  11L, "Main",  MQX_AUTO_START_TASK},
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

MQX_FILE_PTR               mfs_fd_ptr=NULL;
/*TASK*-------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : Testing MFS fileswrite on USB disk
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
* Comments     : Testing MFS fileswrite on RAM disk
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
   char     file_name[20];
   uint32_t                    i, result;
   MQX_FILE_PTR               file1_fp,
                              file2_fp;
   TIME_STRUCT                time; 
   unsigned char                      data1[FIELD_SIZE];
   unsigned char                      data2[]= { 2,2,2,2,2 };
   unsigned char                      tmp;
   _mqx_int                   counter = 0;      
   unsigned char                      adata1 = 0;
   unsigned char                      adata2 = 0;
   unsigned char                      cycle_counter;
   
   _int_install_unexpected_isr();

#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mfs_fd_ptr != NULL), "Test #1 Creating File System");
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (mfs_fd_ptr != NULL), "Test #1 Creating File System");
#endif
   if(!mfs_fd_ptr){
   	return;
   }
   /* Generate buffer */
   for ( i = 0; i<FIELD_SIZE; i++)
   {
      data1[i] = 1;
   }
   
   /* Create test file 1*/
   sprintf(file_name, "%s/data1.txt", MFS);
   file1_fp = fopen(file_name,"w");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file1_fp != NULL), "Test #1 Testing: 3.1.1 Open file 1")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file1_fp != NULL), "Test #1 Testing: 3.1.1 Open file 1")
#endif
   if ( file1_fp == NULL ) {
      return;   
   }
   
   /* Create test file 1*/
   sprintf(file_name, "%s/data2.txt", MFS);
   file2_fp = fopen(file_name,"w");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file2_fp != NULL), "Test #1 Testing: 3.1.2 Open file 2")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file2_fp != NULL), "Test #1 Testing: 3.1.2 Open file 2")
#endif
   if ( file2_fp == NULL ) {
      return;   
   }

/* Write 5 bytes to file1 */   
   result = _io_write(file1_fp, data1, 5);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == 5), "Test #1 Testing: 3.2.1 Write 5 bytes to file 1")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == 5), "Test #1 Testing: 3.2.1 Write 5 bytes to file 1")
#endif
   if ( result != 5 ) {
      return;   
   }
   
/* Write 5 bytes to file2 */   
   result = 0;
   result = _io_write(file2_fp, data2, 5);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == 5), "Test #1 Testing: 3.2.2 Write 5 bytes to file 2")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == 5), "Test #1 Testing: 3.2.2 Write 5 bytes to file 2")
#endif
   if ( result !=5 ) {
      return;   
   }

/* Write 5 bytes again to file1 */   
   result = 0;
   result = _io_write(file1_fp, data1, 5);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == 5), "Test #1 Testing: 3.2.3 Write 5 bytes to file 1")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == 5), "Test #1 Testing: 3.2.3 Write 5 bytes to file 1")
#endif
   if ( result != 5 ) {
      return;   
   }
   
/* Write 5 bytes again to file2 */     
   result = _io_write(file2_fp, data2, 5);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == 5), "Test #1 Testing: 3.2.4 Write 5 bytes to file 2")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == 5), "Test #1 Testing: 3.2.4 Write 5 bytes to file 2")
#endif
   if ( result != 5 ) {
      return;   
   }
   
/* Write FIELD_SIZE bytes to file1, this is over 1 sector */
   result = 0;
   result = _io_write(file1_fp, data1, FIELD_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == FIELD_SIZE), "Test #1 Testing: 3.2.5 Write 550 bytes to file 1")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == FIELD_SIZE), "Test #1 Testing: 3.2.5 Write 550 bytes to file 1")
#endif
   if ( result != FIELD_SIZE ) {
      return;   
   }

/* Write FIELD_SIZE bytes to file2, this is over 1 sector */   
   for ( i = 0; i<FIELD_SIZE; i++)
   {
      data1[i] = 2;
   }
   result = 0;
   result = _io_write(file2_fp, data1, FIELD_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == FIELD_SIZE), "Test #1 Testing: 3.2.6 Write 550 bytes to file 2")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == FIELD_SIZE), "Test #1 Testing: 3.2.6 Write 550 bytes to file 2")
#endif
   if ( result != FIELD_SIZE ) {
      return;   
   }
   
   result = 0;
   result = fclose(file1_fp);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 3.2.7 Close file 1")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #1 Testing: 3.2.7 Close file 1")
#endif
   if ( result != MQX_OK ) {
      return;   
   }
   
   result = 0;
   result = fclose(file2_fp);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 3.2.8 Close file 2")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #1 Testing: 3.2.8 Close file 2")
#endif
   if ( result != MQX_OK ) {
      return;   
   }   
   
/* Test reading file1 there must be a FIELD_SIZE of 1*/
   sprintf(file_name, "%s/data1.txt", MFS);
   file1_fp = fopen(file_name,"r");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file1_fp != NULL), "Test #1 Testing: 3.3.1 Open file 1")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file1_fp != NULL), "Test #1 Testing: 3.3.1 Open file 1")
#endif
   if ( file1_fp == NULL ) {
      return;   
   }
   
   counter = 0;
   while(!(feof(file1_fp)))
   {
      tmp = 0;
      _io_read(file1_fp, &tmp, 1);
      counter += tmp;      
   }
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (counter == CRC_1), "Test #1 Testing: 3.3.2 Testing file1 content")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (counter == CRC_1), "Test #1 Testing: 3.3.2 Testing file1 content")
#endif
   if ( counter != CRC_1 ) {
      return;   
   }
   
   result = fclose(file1_fp);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 3.3.3 Close file 1")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #1 Testing: 3.3.3 Close file 1")
#endif
   if ( result != MQX_OK ) {
      return;   
   }
     
/* Test reading file2 there must be a FIELD_SIZE of 1*/
   sprintf(file_name, "%s/data2.txt", MFS);
   file2_fp = fopen(file_name,"r");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file2_fp != NULL), "Test #1 Testing: 3.3.4 Open file 2")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file2_fp != NULL), "Test #1 Testing: 3.3.4 Open file 2")
#endif
   if ( file2_fp == NULL ) {
      return;   
   }
   
   counter = 0;
   while(!(feof(file2_fp)))
   {
      tmp = 0;
      _io_read(file2_fp, &tmp, 1);
      counter += tmp;      
   }
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (counter == CRC_2), "Test #1 Testing: 3.3.5 Testing file2 content")
#else   
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (counter == CRC_2), "Test #1 Testing: 3.3.5 Testing file2 content")
#endif
   if ( counter != CRC_2 ) {
      return;   
   }
   
   result = fclose(file2_fp);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 3.3.6 Close file 2")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #1 Testing: 3.3.6 Close file 2")
#endif
   if ( result != MQX_OK ) {
      return;   
   }
   
   _time_delay(1);
   _time_get(&time);
   
/* append test for write to 2 files */
   sprintf(file_name, "%s/append1.txt", MFS);
   file1_fp = fopen(file_name, "a+");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file1_fp != NULL), "Test #2 Testing: 3.4.0 Open append file 1")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file1_fp != NULL), "Test #2 Testing: 3.4.0 Open append file 1")
#endif
   if ( file1_fp == NULL ) {
      return;   
   }
   
   sprintf(file_name, "%s/append2.txt", MFS);   
   file2_fp = fopen(file_name, "a+");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file2_fp != NULL), "Test #2 Testing: 3.4.1 Open append file 2")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file2_fp != NULL), "Test #2 Testing: 3.4.1 Open append file 2")
#endif
   if ( file2_fp == NULL ) {
      return;   
   }
   
   adata1 = FP1_INCREMENT;
   adata2 = FP2_INCREMENT;  
   for( cycle_counter = 0; cycle_counter < NUM_CYCLES; cycle_counter ++)
   {
      result = write(file1_fp, &adata1, 1);
#if DISK_FOR_MFS_TEST == USB_DISK
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == 1), "Test #2 Testing: 3.4.2 Write to file append1.txt ")
#else
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == 1), "Test #2 Testing: 3.4.2 Write to file append1.txt ")
#endif
      if( result != 1 ) {
         return; 
      }      
      result = write(file2_fp, &adata2, 1);
#if DISK_FOR_MFS_TEST == USB_DISK
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == 1), "Test #2 Testing: 3.4.3 Write to file append2.txt ")
#else
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == 1), "Test #2 Testing: 3.4.3 Write to file append2.txt ")
#endif
      if( result != 1 ){
         return; 
      }      
   }   
   
   fseek(file1_fp,0,IO_SEEK_SET);
   fseek(file2_fp,0,IO_SEEK_SET);
   
   counter = 0;
   while(!feof(file1_fp))
   {
      tmp = 0;
      _io_read(file1_fp, &tmp, 1);  
      counter += tmp;
   }
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (counter == NUM_CYCLES*FP1_INCREMENT), "Test #2 Testing: 3.4.4 file CRC append1.txt ")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (counter == NUM_CYCLES*FP1_INCREMENT), "Test #2 Testing: 3.4.4 file CRC append1.txt ")
#endif
   if( counter != NUM_CYCLES*FP1_INCREMENT ){
      return;
   }

   result = fclose(file1_fp);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #2 Testing: 3.4.5 Close file append1.txt")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 3.4.5 Close file append1.txt")
#endif
   if ( result != MQX_OK ) {
      return;   
   } 
   
   counter = 0;
   while(!feof(file2_fp))
   {
      tmp = 0;
      _io_read(file2_fp, &tmp, 1);  
      counter += tmp;
   }
   
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (counter == NUM_CYCLES*FP2_INCREMENT), "Test #2 Testing: 3.4.6 file CRC append2.txt ")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (counter == NUM_CYCLES*FP2_INCREMENT), "Test #2 Testing: 3.4.6 file CRC append2.txt ")
#endif
   if( counter != NUM_CYCLES*FP2_INCREMENT ){
      return;
   }
  
   result = fclose(file2_fp);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #2 Testing: 3.4.7 Close file append2.txt")
#else
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 3.4.7 Close file append2.txt")
#endif
   if ( result != MQX_OK ) {
      return;   
   }
}
        
 /******************** End Testcases ********************/

/******************** Register tests *******************/
EU_TEST_SUITE_BEGIN(suite_mfs)
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_TEST_CASE_ADD(tc_1_main_task, "- Testing files write on USB disk"),
#else   
   EU_TEST_CASE_ADD(tc_2_main_task, "- Testing files write on RAM disk"),
#endif   
EU_TEST_SUITE_END(suite_mfs)

EU_TEST_REGISTRY_BEGIN()
   EU_TEST_SUITE_ADD(suite_mfs, "Testing MFS files write")
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
