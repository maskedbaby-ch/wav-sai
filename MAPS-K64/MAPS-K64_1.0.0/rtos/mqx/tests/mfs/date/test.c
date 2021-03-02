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
* $Version : 3.8.10.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file tests the date/time capabilities in MFS
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
   
/*TASK*-------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : MFS date tested on USB disk
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
* Comments     : MFS date tested on RAM disk
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
* Task Name    : tc_3_main_task
* Comments     : MFS date tested on SD Card disk
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
   uint32_t       error_code, i;
   MQX_FILE_PTR   file_ptr;
   DATE_STRUCT    new_date;
   DATE_STRUCT    saved_date;
   DATE_STRUCT    date;
   TIME_STRUCT    time;
   unsigned char  test_buffer[TEST_SIZE];
   char           file_name[20];

   _int_install_unexpected_isr();

   date.MONTH    = 6;
   date.DAY      = 26;
   date.YEAR     = 2002;
   date.HOUR     = 12;
   date.MINUTE   = 12;
   date.SECOND   = 0;
   date.MILLISEC = 0;
      
   _time_from_date(&date, &time);
   _time_set(&time);

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
   
   /* Create a file */
   sprintf(file_name, "%s\\%s", MFS, FILE_NAME);
   file_ptr = fopen(file_name, "n+");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file_ptr != NULL), "Test #1 Testing: 3.0 Create a file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file_ptr != NULL), "Test #1 Testing: 3.0 Create a file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (file_ptr != NULL), "Test #1 Testing: 3.0 Create a file");
#endif
   if (!file_ptr) {
      return;   
   }
   
   /* Close a file */
   error_code = fclose(file_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 Testing: 3.1 Close a file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 Testing: 3.1 Close a file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 Testing: 3.1 Close a file");
#endif
   if (error_code) {
      return;   
   }
   
   /* Get date and time */
   error_code = test_get_date_from_file(mfs_fd_ptr, file_name, &saved_date); 
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code != 0), "Test #1 Testing: 3.2 Get date and time");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code != 0), "Test #1 Testing: 3.2 Get date and time");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code != 0), "Test #1 Testing: 3.2 Get date and time");
#endif
   if (!error_code) {
      return;   
   }
   
   /* Re-open file */
   file_ptr = fopen(file_name, "r+");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file_ptr != NULL), "Test #1 Testing: 3.3 Re-open file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file_ptr != NULL), "Test #1 Testing: 3.3 Re-open file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (file_ptr != NULL), "Test #1 Testing: 3.3 Re-open file");
#endif
   if (!file_ptr) {
      return;   
   }
   
   /* Close a file */
   error_code = fclose(file_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code == 0), "Test #1 Testing: 3.4 Close a file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code == 0), "Test #1 Testing: 3.4 Close a file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code == 0), "Test #1 Testing: 3.4 Close a file");
#endif
   if (error_code) {
      return;   
   }
   
   /* Get date and time */
   error_code = test_get_date_from_file(mfs_fd_ptr, file_name, &date);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code != 0), "Test #1 Testing: 3.5 Get date and time");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code != 0), "Test #1 Testing: 3.5 Get date and time");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code != 0), "Test #1 Testing: 3.5 Get date and time");
#endif
   if (!error_code) {
      return;   
   }

   /* Compare dates */
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (saved_date.YEAR == date.YEAR)          , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (saved_date.MONTH == date.MONTH)        , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (saved_date.DAY == date.DAY)            , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (saved_date.HOUR == date.HOUR)          , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (saved_date.MINUTE == date.MINUTE)      , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (saved_date.SECOND == date.SECOND)      , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (saved_date.MILLISEC == date.MILLISEC)  , "Test #1 Testing: 3.6 Compare dates");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (saved_date.YEAR == date.YEAR)          , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (saved_date.MONTH == date.MONTH)        , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (saved_date.DAY == date.DAY)            , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (saved_date.HOUR == date.HOUR)          , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (saved_date.MINUTE == date.MINUTE)      , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (saved_date.SECOND == date.SECOND)      , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (saved_date.MILLISEC == date.MILLISEC)  , "Test #1 Testing: 3.6 Compare dates");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (saved_date.YEAR == date.YEAR)          , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (saved_date.MONTH == date.MONTH)        , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (saved_date.DAY == date.DAY)            , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (saved_date.HOUR == date.HOUR)          , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (saved_date.MINUTE == date.MINUTE)      , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (saved_date.SECOND == date.SECOND)      , "Test #1 Testing: 3.6 Compare dates");
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (saved_date.MILLISEC == date.MILLISEC)  , "Test #1 Testing: 3.6 Compare dates");
#endif
   if (saved_date.YEAR != date.YEAR) {
      return;   
   }if (saved_date.MONTH != date.MONTH) {
      return;   
   }if (saved_date.DAY != date.DAY) {
      return;   
   }if (saved_date.HOUR != date.HOUR) {
      return;   
   }if (saved_date.MINUTE != date.MINUTE) {
      return;   
   }if (saved_date.SECOND != date.SECOND) {
      return;   
   }if (saved_date.MILLISEC != date.MILLISEC) {
      return;   
   }
   
   /* Re-re-open file */
   file_ptr = fopen(file_name, "r+");
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (file_ptr != NULL),"Test #1 Testing: 3.7 Re-re-open file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (file_ptr != NULL),"Test #1 Testing: 3.7 Re-re-open file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (file_ptr != NULL),"Test #1 Testing: 3.7 Re-re-open file");
#endif
   if (!file_ptr) {
      return;   
   }
   
   date.YEAR += 10;
   _time_from_date(&date, &time);
   _time_set(&time);

   /* Write file */
   i = write(file_ptr, test_buffer, TEST_SIZE);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (i == TEST_SIZE),"Test #1 Testing: 3.8 Write file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == TEST_SIZE),"Test #1 Testing: 3.8 Write file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == TEST_SIZE),"Test #1 Testing: 3.8 Write file");
#endif
   if (i != TEST_SIZE) {
      return;   
   }
   
   /* Close a file */
   error_code = fclose(file_ptr);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code ==0),"Test #1 Testing: 3.9 Close a file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code ==0),"Test #1 Testing: 3.9 Close a file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code ==0),"Test #1 Testing: 3.9 Close a file");
#endif
   if (error_code) {
      return;   
   }
   
   /* Get date and time */
   error_code = test_get_date_from_file(mfs_fd_ptr, file_name, &new_date);
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error_code !=0),"Test #1 Testing: 3.9 Close a file");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error_code !=0),"Test #1 Testing: 3.9 Close a file");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error_code !=0),"Test #1 Testing: 3.9 Close a file");
#endif
   if (!error_code) {
      return;   
   }
   
   /* Comparing dates */  
#if DISK_FOR_MFS_TEST == USB_DISK
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,(date.YEAR == new_date.YEAR),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,(date.MONTH == new_date.MONTH),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,(date.DAY == new_date.DAY),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,(date.HOUR == new_date.HOUR),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,(date.MINUTE == new_date.MINUTE),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,(date.SECOND == new_date.SECOND),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,(date.MILLISEC == new_date.MILLISEC),"Test #1 Testing: 3.11 Comparing dates");
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,(date.YEAR == new_date.YEAR),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,(date.MONTH == new_date.MONTH),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,(date.DAY == new_date.DAY),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,(date.HOUR == new_date.HOUR),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,(date.MINUTE == new_date.MINUTE),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,(date.SECOND == new_date.SECOND),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,(date.MILLISEC == new_date.MILLISEC),"Test #1 Testing: 3.11 Comparing dates");
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,(date.YEAR == new_date.YEAR),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,(date.MONTH == new_date.MONTH),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,(date.DAY == new_date.DAY),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,(date.HOUR == new_date.HOUR),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,(date.MINUTE == new_date.MINUTE),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,(date.SECOND == new_date.SECOND),"Test #1 Testing: 3.11 Comparing dates");
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,(date.MILLISEC == new_date.MILLISEC),"Test #1 Testing: 3.11 Comparing dates");
#endif
   
   if (date.YEAR != new_date.YEAR) {
      return;   
   } if (date.MONTH != new_date.MONTH) {
      return;   
   } if (date.DAY != new_date.DAY) {
      return;   
   } if (date.HOUR != new_date.HOUR) {
      return;   
   } if (date.MINUTE != new_date.MINUTE) {
      return;   
   } if (date.SECOND != new_date.SECOND) {
      return;   
   } if (date.MILLISEC != new_date.MILLISEC) {
      return;   
   }

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name : test_mfs_date_to_mqx_date
* Comments      :
*
*END*----------------------------------------------------------------------*/

void test_mfs_date_to_mqx_date
   (
      DATE_STRUCT_PTR          mqx_date_ptr,
      MFS_DATE_TIME_PARAM_PTR  mfs_date_ptr
   )
{
   uint16_t date_word, time_word;

   date_word = *mfs_date_ptr->DATE_PTR;
   time_word = *mfs_date_ptr->TIME_PTR;

   mqx_date_ptr->MONTH = (date_word & MFS_MASK_MONTH) >> MFS_SHIFT_MONTH;
   mqx_date_ptr->DAY   = (date_word & MFS_MASK_DAY)   >> MFS_SHIFT_DAY;
   mqx_date_ptr->YEAR  = ((date_word & MFS_MASK_YEAR) >> MFS_SHIFT_YEAR) + 1980;

   mqx_date_ptr->HOUR     = (time_word & MFS_MASK_HOURS)   >> MFS_SHIFT_HOURS;
   mqx_date_ptr->MINUTE   = (time_word & MFS_MASK_MINUTES) >> MFS_SHIFT_MINUTES;
   mqx_date_ptr->SECOND   = (time_word & MFS_MASK_SECONDS) << 1;
   mqx_date_ptr->MILLISEC = 0;

} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name : test_get_date_from_file
* Comments      :
*
*END*----------------------------------------------------------------------*/

bool test_get_date_from_file
   (
      MQX_FILE_PTR             mfs_fd_ptr,
      char                     *file_name,
      DATE_STRUCT_PTR          mqx_date_ptr
   )
{
   MFS_SEARCH_DATA      search_data;
   MFS_SEARCH_PARAM     search;
   MFS_DATE_TIME_PARAM  mfs_date;
   uint32_t             error_code;

   search.ATTRIBUTE       = MFS_ATTR_ANY;
   search.WILDCARD        = file_name;
   search.SEARCH_DATA_PTR = &search_data;
   
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FIND_FIRST_FILE, 
      (uint32_t *)&search);
   if (error_code) {
      return FALSE;
   } /* Endif */

   mfs_date.DATE_PTR = &search_data.DATE;
   mfs_date.TIME_PTR = &search_data.TIME;

   test_mfs_date_to_mqx_date(mqx_date_ptr, &mfs_date);

   return TRUE;

} /* EndBody */


 /******************** End Testcases ********************/

/******************** Register tests *******************/
EU_TEST_SUITE_BEGIN(suite_mfs)
#if DISK_FOR_MFS_TEST == USB_DISK
    EU_TEST_CASE_ADD(tc_1_main_task, "- Testing date/time on USB disk"),
#endif
#if DISK_FOR_MFS_TEST == RAM_DISK
    EU_TEST_CASE_ADD(tc_2_main_task, "- Testing date/time on RAM disk"),
#endif
#if DISK_FOR_MFS_TEST == SDCARD_DISK
    EU_TEST_CASE_ADD(tc_3_main_task, "- Testing date/time on SD Card disk"),
#endif
EU_TEST_SUITE_END(suite_mfs)

EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_mfs, "Testing MFS date/time")
EU_TEST_REGISTRY_END()
/******************** End Register *********************/        
         
         
 /*TASK*-------------------------------------------------------------------
* 
* Task Name    : main_task
* Comments     :
*   This task tests the formatted I/O library.
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
