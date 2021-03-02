/**HEADER*******************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
**************************************************************************** 
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
****************************************************************************
*
* Comments:
*
*   This file contains main initialization for your application
*   and infinite loop
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include <nandflash_wl.h>
#include <mfs.h>
#include "testing_main_internal.h"
#include "simple_timer.h"
#include "media_unit_test_helpers.h"   /* Need to check */
#include "ddi_nand_hal.h"
#include "ddi_media.h"
#include "bad_block_table.h"
#include "phy_map.h"
#include "ddi_NandHalSpy.h"
#include "system_testing.h"
#include "mem_management.h"
#include "test.h"

//------------------------------------------------------------------------
void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
void tc_4_main_task(void);
void tc_5_main_task(void);
void tc_6_main_task(void);
void tc_7_main_task(void);
void tc_8_main_task(void);

// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_sys_init_shutdown)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1: Test init/shutdown process"),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2: Test read/write buffer whule re-init process"),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3: Test init/shutdown with MFS "),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4: Test file and directory while init/shutdown MFS"),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5: Measure the total time of init/shutdown process"),
    EU_TEST_CASE_ADD(tc_6_main_task, " Test #6: Measure the time of read/write sector while init/shutdown the media"),
    EU_TEST_CASE_ADD(tc_7_main_task, " Test #7: Measure the time of init/shutdown the MFS"),
    EU_TEST_CASE_ADD(tc_8_main_task, " Test #8: Measure the time of read/write the file/directory with MFS"),
EU_TEST_SUITE_END(suite_sys_init_shutdown)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_sys_init_shutdown, " - Testing Wrapper")
EU_TEST_REGISTRY_END()

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   main_task,   2000,  9,   "main", MQX_AUTO_START_TASK},
   {0,           0,           0,     0,   0,      0,                 }
};

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_1_main_task
* Comments       : The function is used to init/shutdown process
*
*
*END------------------------------------------------------------------*/
void tc_1_main_task(void) {
   MQX_FILE_PTR    nand_wl_ptr;
   _mqx_uint       result;
   RtStatus_t      status;

  /* Step 1: Open NANDFLASH WL*/ 
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, "I");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (nand_wl_ptr != NULL), " Test #1.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);
           
   status = system_test_initialize(TRUE,nand_wl_ptr);          
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), " Test #1.2: Initialize all objects which related nand_wl drive with try repair if initialization is failed");

   status = system_test_finalize(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), " Test #1.3: De-init all objects which related nand_wl drive ");   
   
   status = system_test_initialize(FALSE,nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), " Test #1.4: Initialize all objects which related nand_wl drive without try repair if initialization is failed ");   
  
   status = system_test_finalize(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status == SUCCESS), " Test #1.5: De-init all objects which related nand_wl drive ");   
   
   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), " Test #1.7: Close the nand_wl object file. ");       
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_2_main_task
* Comments       : The function is used to test read/write buffer while 
*                  re-init process
*
*END------------------------------------------------------------------*/
void tc_2_main_task(void) {
   MQX_FILE_PTR     nand_wl_ptr;
   RtStatus_t       status;
   uint32_t          result;
   uint32_t          buf_size; 
   unsigned char        *write_buffer, *read_buffer;
   uint32_t          secnum;
   uint32_t          i = 0;
   uint64_t          numberSector = 0;   
   uint32_t          rand = simple_rand();      
 
  /* Step 1: Open NANDFLASH WL*/ 
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, "I");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (nand_wl_ptr != NULL), " Test #2.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);

   nandwl_testing_get_info(nand_wl_ptr);    /*Get nandwl_phy_page_size & nandwl_phy_page_spare_size*/
   buf_size = nandwl_phy_page_size + nandwl_phy_page_spare_size; 
          
   write_buffer = allocate_buffer(buf_size, "write");
   fill_continous_buff(write_buffer, buf_size);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (write_buffer != NULL), " Test #2.2: Allocate write_buffer buffer with size is stored in buf_size");
      
   read_buffer = allocate_buffer(buf_size, "read");
   zero_buff(read_buffer, buf_size );
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (read_buffer != NULL), " Test #2.3: Allocate read_buffer buffer with size is stored in buf_size");
      
   /* Try to repair when first time initialize */
   status = system_test_initialize(TRUE,nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status  ==  SUCCESS), " Test #2.4: Initialize all objects which related nand_wl drive ");

   status = DriveGetInfo(DRIVE_TAG_DATA, kDriveInfoSizeInSectors, &numberSector);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status  ==  SUCCESS), " Test #2.5: Get information from Nand_wl Drive ");   
   secnum = rand % numberSector;   
   
   WL_TEST_LOG("Write data on lsector %d \n", secnum);
   fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
   
   result =  write(nand_wl_ptr, write_buffer, 1);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result  ==  1), " Test #2.6: Write data from write_buffer to nandflash_wl drive");   
   
   status = system_test_finalize(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status  ==  SUCCESS), " Test #2.7: De-init all object which related nand_wl drive");   
   
   status = system_test_initialize(FALSE,nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status  ==  SUCCESS), " Test #2.8: Init all object which related nand_wl drive");   
        
   WL_TEST_LOG("Read data on lsector %d \n", secnum);
   fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
   
   result = read(nand_wl_ptr, read_buffer, 1);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result  ==  1), " Test #2.9: Init all object which related nand_wl drive");   
   
   result = compare_test(write_buffer, read_buffer, nandwl_virt_page_size);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result  ==  TRUE), " Test #2.10: Compare data between write_buffer & read_buffer");   
   
   status = system_test_finalize(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status  ==  SUCCESS), " Test #2.11: De-init & shutdown all objects which related with Nand_wl drive");   
   
   _mem_free(write_buffer);
   _mem_free(read_buffer);
   result = fclose(nand_wl_ptr);  
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), " Test #2.12: Close nand_wl object file ptr");      
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_3_main_task
* Comments       : The function is used to test init/shutdown with MFS
*
*
*END------------------------------------------------------------------*/
void tc_3_main_task(void) {
   char *          fsname = "a:";
   MQX_FILE_PTR      fshandle = NULL;
   MQX_FILE_PTR      fhandle = NULL;
 
   MQX_FILE_PTR      nand_wl_ptr;
   RtStatus_t        status;
   uint32_t           result;
   
   /* Step 1: Open NANDFLASH WL*/ 
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, "I");
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (nand_wl_ptr != NULL), " Test #3.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);
      
   status = system_test_initialize(TRUE,nand_wl_ptr);   
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (status == SUCCESS), " Test #3.2: Init all objects which related with nand_wl drive");
  
   result = system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, TRUE);  
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((fshandle != NULL) && (result == MFS_NO_ERROR)), " Test #3.3:Test Open MFS file (fsname)");
   
   result = system_test_mfs_close(fshandle, fsname);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MFS_NO_ERROR), " Test #3.4:Test Open MFS file (fsname)");
   status = system_test_finalize(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (status == SUCCESS), " Test #3.4: De-init & shutdown all objects which related with nand_wl drive");

   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), " Test #3.5:Close the nand_wl file object");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_4_main_task
* Comments       : The function is used to test read/write file/directory
*                  while re-init process
*
*END------------------------------------------------------------------*/
void tc_4_main_task(void ) {
   char *         fsname = "a:";
   char *         dirname = "test_directory";
   char *         filename = "test_file";
   char *         filecontent = "123456";
   char *         readcontent;
   uint32_t          contentsize = 6;
   char             fpath[PATHNAME_SIZE * 2] = {0};
   char             dir_path[PATHNAME_SIZE] =   {0};
   MQX_FILE_PTR     fshandle = NULL;
   MQX_FILE_PTR     fhandle = NULL;
   DIR_STRUCT_PTR   dir_ptr;
   _mqx_int         error_code;   
   MQX_FILE_PTR     nand_wl_ptr;
   RtStatus_t       status;      
   uint32_t          result;
   /* The first lap: 
      - Init & open
      - Write directory
      - Close & shutdown */
   /* Initialize Nand Media & DataDrive */
   
  /* Step 1: Open NANDFLASH WL*/ 
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, "I");
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (nand_wl_ptr != NULL), " Test #4.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);
      
   status = system_test_initialize(TRUE,nand_wl_ptr);   
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS), " Test #4.2: Init all objects which related with nand_wl drive");
  
   /* Open MFS */
   result =  system_test_mfs_open(nand_wl_ptr,fsname, &fshandle, true);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MFS_NO_ERROR), " Test #4.3: Open the MFS file");
   
   /* MFS default format */
   result = ioctl(fshandle, IO_IOCTL_DEFAULT_FORMAT, NULL);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == IO_OK), " Test #4.5: MFS default format ");  /*Need to check - ToanLn*/
   
   /* Move to root directory */
   sprintf(fpath, "%s\\", fsname );   
   result = ioctl(fshandle, IO_IOCTL_CHANGE_CURRENT_DIR, fpath);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == IO_OK), " Test #4.6: Move to root directory ");  

   /* Create a directory */
   WL_TEST_LOG("Create dir %s \n", dirname);      
   result =  ioctl(fshandle, IO_IOCTL_CREATE_SUBDIR, (void *)dirname);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == IO_OK), " Test #4.6:Creat a directory"); 
   
   /* Create a file */
   WL_TEST_LOG("Create file %s \n", filename);
   sprintf(fpath, "%s\\%s", fsname, filename);
   
   fhandle = fopen(fpath, "w");
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (fhandle != NULL), " Test #4.7: Open the MFS file (fpath) to write"); 
     
   fseek(fhandle, 0, IO_SEEK_SET );   
   result = write(fhandle, (unsigned char *)filecontent, contentsize);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == contentsize), " Test #4.8: Write from filecontent to fhandle "); 
   
   dump_buff((unsigned char *)filecontent, contentsize, fpath);
   fclose(fhandle);
   fhandle = NULL;
   
   /* Close MFS */
   if (NULL != fshandle ) {
      system_test_mfs_close(fshandle, fsname);
   }
   
   /* Shutdown Nand Media & DataDrive */
   status = system_test_finalize(nand_wl_ptr);   
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS), " Test #4.9:  Shutdown Nand Media & Data Drive"); 
   
   /* The second lap:
      - Init & open
      - Read directory
      - Close & shutdown */
   /* Initialize Nand Media & DataDrive */
   status = system_test_initialize(TRUE,nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS), " Test #4.10:  Initialize Nand Media & DataDrive"); 
      
   /* Open MFS */
   result = system_test_mfs_open(nand_wl_ptr,fsname, &fshandle, true);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MFS_NO_ERROR), " Test #4.11: Open MFS file (fsname) "); 
   
   /* Move to root directory */
   sprintf(fpath, "%s\\", fsname );
   result = ioctl(fshandle, IO_IOCTL_CHANGE_CURRENT_DIR, fpath);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == IO_OK), " Test #4.12: Move to root directory "); 
   
   sprintf((char *)&dir_path, "%s\\*.*", fsname);      
   dir_ptr =(DIR_STRUCT_PTR) _io_mfs_dir_open(fshandle, (char *)&dir_path, "d" );
   
   /* Get created directory */
   WL_TEST_LOG("\nDIRECTORIES\n");
   while (true) {
      if ( dir_ptr->FIRST ) {
         error_code = ioctl(fshandle, IO_IOCTL_FIND_FIRST_FILE, (void *)&dir_ptr->SEARCH);
         dir_ptr->FIRST = FALSE;
      } else {
         error_code = ioctl(fshandle, IO_IOCTL_FIND_NEXT_FILE,  (void *) &dir_ptr->SEARCH_DATA);
      }         
      
      if (error_code != MFS_NO_ERROR) {
         break;
      }
      
      error_code = ioctl(fshandle, IO_IOCTL_GET_LFN, (void *)&dir_ptr->LFN_DATA);

      if ( !error_code ) { 
         WL_TEST_LOG("\n%s\n", (char *)&dir_ptr->LFN);
      } else {
         WL_TEST_LOG("\n%s\n", (char *)&dir_ptr->SEARCH_DATA.NAME);
      }
   }
   _io_mfs_dir_close(dir_ptr);
   
   /* Read written file */
   WL_TEST_LOG("Read file %s \n", filename);
   sprintf(fpath, "%s\\%s", fsname, filename);
   
   fhandle = fopen(fpath, "r");
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (fhandle != NULL), " Test #4.13: Open the MFS file (fpath) to read "); 
   
   fseek(fhandle, 0, IO_SEEK_SET );
   readcontent = (char *)allocate_buffer(contentsize, "readcontent");
   result = read(fhandle, (unsigned char *)readcontent, contentsize);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == contentsize), " Test #4.14: Read from fhandle to readcontent buffer "); 
   
   dump_buff((unsigned char *)readcontent, contentsize, fpath);
   fclose(fhandle);
   fhandle = NULL;
   
   /* Close MFS */
   if (NULL != fshandle ) {
      system_test_mfs_close(fshandle, fsname);
   }
   
   _mem_free(readcontent);
   /* Shutdown Nand Media & DataDrive */
   status  = system_test_finalize(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS), " Test #4.15: Shutdown Nand Media & DataDrive"); 
   
   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), " Test #4.16: Close the nand_wl file");    
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_5_main_task
* Comments       : The function is used to measure the time of init/shutdown 
*                  MFS
*
*END------------------------------------------------------------------*/
void tc_5_main_task(void) {
   SimpleTimer *        timer;
   uint64_t              amount = 0;
   uint64_t              total = 0;
   MQX_FILE_PTR         nand_wl_ptr;
   RtStatus_t           status;
   uint32_t              result;
   
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, "I");
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (nand_wl_ptr != NULL), " Test #5.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);
      
   status = media_buffer_init();
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), " Test #5.2: Init media buffer manager");
  
   /* Initialize MEDIA */
   timer = new SimpleTimer;
   
   status = MediaInit(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), " Test #5.3: Init media buffer");

   status = MediaBuildFromPreDefine() ;
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), " Test #5.4: Build media from pre-defined information");

   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for initializing MEDIA\n", (uint32_t)amount);
   delete timer;
   
   /* Initialize DATADRIVE */
   timer = new SimpleTimer;
   
   status = DriveInit(DRIVE_TAG_DATA);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), " Test #5.5: Init Nand_wl drive");
   
   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for initializing DATADRIVE\n", (uint32_t)amount);
   delete timer;
   
   
   /* Shutdown MEDIA */
   timer = new SimpleTimer;
   
   status = MediaShutdown(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), " Test #5.6: Shutdown the initialized Media");

   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for shutdowning MEDIA\n", (uint32_t)amount);
   delete timer;
   
   
   /* Initialize MEDIA */
   timer = new SimpleTimer;
   
   status = MediaInit(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), " Test #5.7: Create & Initialize the logical media");
   
   status = MediaBuildFromPreDefine();
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), " Test #5.8: Build media from pre-defined information");
   
   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for re-initializing MEDIA\n", (uint32_t)amount);
   delete timer;
   
   
   /* Initialize DATADRIVE */
   timer = new SimpleTimer;
   
   status = DriveInit(DRIVE_TAG_DATA);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), " Test #5.9: Init the Nand_wl drive");
   
   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for re-initializing DATADRIVE\n", (uint32_t)amount);
   delete timer;
   
   
   /* Shutdown MEDIA */
   timer = new SimpleTimer;
   
   status = MediaShutdown(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), " Test #5.10: Shutdown the initialized media");
 
   amount = timer->getElapsed();
   total += amount;
   delete timer;
   
   WL_TEST_LOG("\n%d microsecond(s) for re-shutdowning MEDIA\n", (uint32_t)amount);
   
   status = media_buffer_deinit();
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), " Test #5.11: De-init the media buffer");
   
   WL_TEST_LOG("\n%d microsecond(s) for all processes\n", (uint32_t)total);
   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), " Test #5.12: Close the nand_wl file");
  
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_6_main_task
* Comments       : The function is used to measure the time of init/shutdown 
*                  process
*
*END------------------------------------------------------------------*/
void tc_6_main_task(void) {
   SimpleTimer *        timer;
   uint64_t              amount = 0;
   uint64_t              total = 0;
   unsigned char            *writeBuffer, *readBuffer;
   uint32_t              secnum = 1000;
   MQX_FILE_PTR         nand_wl_ptr;
   RtStatus_t           status;
   uint32_t              result;
   
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, "I");
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (nand_wl_ptr != NULL), " Test #6.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);
      
   writeBuffer = allocate_buffer(nandwl_virt_page_size, "writeBuffer");
   fill_continous_buff(writeBuffer, nandwl_virt_page_size);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (writeBuffer != NULL), " Test #6.2: allocate the write_buffer buffer");
     
   readBuffer = allocate_buffer(nandwl_virt_page_size, "readBuffer");
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (writeBuffer != NULL), " Test #6.3: allocate the read_buffer buffer");
  
   status = media_buffer_init();
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), " Test #6.5: Initialize the meia buffer manager");
   
   /* Initialize MEDIA */
   timer = new SimpleTimer;
   
   status = MediaInit(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), " Test #6.6: Init the logical media");
   
   status = MediaBuildFromPreDefine();
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), " Test #6.7: Build the logical media from pre-defined information");

   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for initializing MEDIA\n", (uint32_t)amount);
   delete timer;
   
   /* Initialize DATADRIVE */
   timer = new SimpleTimer;
   
   status = DriveInit(DRIVE_TAG_DATA);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), " Test #6.8: Init the nand_wl drive");

   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for initializing DATADRIVE\n", (uint32_t)amount);
   delete timer;
   
   /* Write a sector */
   timer = new SimpleTimer;
   
   status =  DriveWriteSector(DRIVE_TAG_DATA, secnum, writeBuffer);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), " Test #6.9: Write sector to nand_wl drive");
   
   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for writing a sector\n", (uint32_t)amount);
   delete timer;
   
   /* Shutdown MEDIA */
   timer = new SimpleTimer;
   
   status = MediaShutdown(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), " Test #6.10: Shutdown the logical media");

   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for shutdowning MEDIA\n", (uint32_t)amount);
   delete timer;
   
   /* Initialize MEDIA */
   timer = new SimpleTimer;
   
   status = MediaInit(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), " Test #6.10: Init the logical media");

   status = MediaBuildFromPreDefine();
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), " Test #6.11: Build the media from pre-defined information");

   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for re-initializing MEDIA\n", (uint32_t)amount);
   delete timer;
   
   /* Initialize DATADRIVE */
   timer = new SimpleTimer;
   
   status = DriveInit(DRIVE_TAG_DATA);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), " Test #6.12: Init the Nand_wl drive");
  
   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for re-initializing DATADRIVE\n", (uint32_t)amount);
   delete timer;
   
   /* Read a sector */
   timer = new SimpleTimer;
   
   status = DriveReadSector(DRIVE_TAG_DATA, secnum, readBuffer);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), " Test #6.13: Read sector from Nand_wl drive");

   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for writing a sector\n", (uint32_t)amount);
   delete timer;
   
   /* Shutdown MEDIA */
   timer = new SimpleTimer;
   
   status = MediaShutdown(kInternalNandMedia);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), " Test #6.13: Shutdown the logical media");
     
   amount = timer->getElapsed();
   total += amount;
   delete timer;
   
   WL_TEST_LOG("\n%d microsecond(s) for re-shutdowning MEDIA\n", (uint32_t)amount);
   
   status = media_buffer_deinit();
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), " Test #6.13: De-Init the logical media");
   
   WL_TEST_LOG("\n%d microsecond(s) for all processes\n", (uint32_t)total);
   
   _mem_free(writeBuffer);
   _mem_free(readBuffer);
   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), " Test #6.14: Close the nand_wl file");
   
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_7_main_task
* Comments       : The function is used to measure the time of init/shutdown MFS
*
*END------------------------------------------------------------------*/
void tc_7_main_task(void) {
   SimpleTimer *      timer;
   uint64_t            amount = 0;
   uint64_t            total = 0;
   char *           fsname = "a:";
   MQX_FILE_PTR       fshandle = NULL;
   MQX_FILE_PTR       fhandle = NULL;
   MQX_FILE_PTR       nand_wl_ptr;
   RtStatus_t         status;
   uint32_t            result;
   
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, "I");
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, (nand_wl_ptr != NULL), " Test #7.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);
        
   /* Init with spy */
   timer = new SimpleTimer;
   
   status = system_test_initialize(TRUE,nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, (status == SUCCESS), " Test #7.2: Init the logical media & Nand_wl drive");
   
   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for initializing MEDIA with spy\n", (uint32_t)amount);
   delete timer;
   
   /* Open MFS */
   timer = new SimpleTimer;
   
   result = system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, TRUE);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MFS_NO_ERROR), " Test #7.3: Test to Open the MFS file");
 
   amount = timer->getElapsed();
   total += amount;
   delete timer;
   
   WL_TEST_LOG("\n%d microsecond(s) for opening MFS\n", (uint32_t)amount);
   
   /* Close MFS */
   timer = new SimpleTimer;
   
   if (NULL != fshandle ) {
      result = system_test_mfs_close(fshandle, fsname);     
      EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MFS_NO_ERROR), " Test #7.4: Test to Close the MFS file");
   }
   
   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for closing MFS\n", (uint32_t)amount);
   delete timer;
   
   
   /* Shutdown */
   timer = new SimpleTimer;
   
   status = system_test_finalize(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, (status == SUCCESS), " Test #7.5: Shutdown & de-init the logical media & Nand_wl drive");
   
   amount = timer->getElapsed();
   total += amount;
   delete timer;
   WL_TEST_LOG("\n%d microsecond(s) for shutdowning MEDIA\n", (uint32_t)amount);
   
   WL_TEST_LOG("\n%d microsecond(s) for all processes\n", (uint32_t)total);
   
   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), " Test #7.6: Close the nand_wl file");
  
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_8_main_task
* Comments       : The function is used to measure the time of read/write
*                  file/directory with MFS
*
*END------------------------------------------------------------------*/
void tc_8_main_task(void) {
   SimpleTimer *              timer;
   char *                   fsname = "a:";
   char *                   dirname = "test_directory";
   char *                   filename = "test_file";
   char *                   filecontent = "123456";
   char *                   readcontent;
   uint32_t                    contentsize = 6;
   char                       fpath[PATHNAME_SIZE * 2] = {0};
   char                       dir_path[PATHNAME_SIZE] =   {0};
   MQX_FILE_PTR               fshandle = NULL;
   MQX_FILE_PTR               fhandle = NULL;
   DIR_STRUCT_PTR             dir_ptr;
   _mqx_int                   error_code;
   uint64_t                    amount = 0;
   uint64_t                    total = 0;   
   MQX_FILE_PTR               nand_wl_ptr;
   RtStatus_t                 status;
   uint32_t                    result;
   
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, "I");
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (nand_wl_ptr != NULL), " Test #8.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);
          
   /* The first lap: 
      - Init & open
      - Write directory
      - Close & shutdown */
   /* Initialize Nand Media & DataDrive */
   timer = new SimpleTimer;
   status = system_test_initialize(TRUE,nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (status == SUCCESS), " Test #8.2: Create & init logical media & Nand_wl drive");
   
   result = system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, true);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MFS_NO_ERROR), " Test #8.3: Test to open the MFS file");

   /* MFS default format */
   ioctl(fshandle, IO_IOCTL_DEFAULT_FORMAT, NULL);
   
   /* Move to root directory */
   sprintf(fpath, "%s\\", fsname );
   result = ioctl(fshandle, IO_IOCTL_CHANGE_CURRENT_DIR, fpath);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == IO_OK), " Test #8.4: Move to root directory");

   /* Create a directory */
   WL_TEST_LOG("Create dir %s \n", dirname);      
   result = ioctl(fshandle, IO_IOCTL_CREATE_SUBDIR, (void *)dirname);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == IO_OK), " Test #8.4: Create a directory");

   /* Create a file */
   WL_TEST_LOG("Create file %s \n", filename);
   sprintf(fpath, "%s\\%s", fsname, filename);
   
   fhandle = fopen(fpath, "w");
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (fhandle != NULL), " Test #8.5: Open the mfs file to write");
   
   fseek(fhandle, 0, IO_SEEK_SET );
   result = write(fhandle, (unsigned char *)filecontent, contentsize);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == contentsize), " Test #8.6: Write data from filecontent to fhandle file");
   
   dump_buff((unsigned char *)filecontent, contentsize, fpath);
   fclose(fhandle);
   fhandle = NULL;
   
   /* Close MFS */
   if (NULL != fshandle ) {
      system_test_mfs_close(fshandle, fsname);
   }
   
   /* Shutdown Nand Media & DataDrive */
   status = system_test_finalize(nand_wl_ptr);   
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (status == SUCCESS), " Test #8.7: Shutdown the Nand Media & Data drive");
   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for the first lap\n", (uint32_t)amount);
   delete timer;
   /* The second lap:
      - Init & open
      - Read directory
      - Close & shutdown */
   /* Initialize Nand Media & DataDrive */
   timer = new SimpleTimer;
   status = system_test_initialize(TRUE, nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (status == SUCCESS), " Test #8.8: Create & Init the Nand Media & Data drive");

   /* Open MFS */
   result = system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, true);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MFS_NO_ERROR), " Test #8.9: Test to open the MFS file");
   
   /* Move to root directory */
   sprintf(fpath, "%s\\", fsname );
   result = ioctl(fshandle, IO_IOCTL_CHANGE_CURRENT_DIR, fpath);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == IO_OK), " Test #8.10:Move to root directory");
    
   sprintf((char *)&dir_path, "%s\\*.*", fsname);      
   dir_ptr =(DIR_STRUCT_PTR) _io_mfs_dir_open(fshandle, (char *)&dir_path, "d" );
   
   /* Get created directory */
   WL_TEST_LOG("\nDIRECTORIES\n");
   while (true) {
      if ( dir_ptr->FIRST ) {
         error_code = ioctl(fshandle, IO_IOCTL_FIND_FIRST_FILE, (void *)&dir_ptr->SEARCH);
         dir_ptr->FIRST = FALSE;
      } else {
         error_code = ioctl(fshandle, IO_IOCTL_FIND_NEXT_FILE,  (void *) &dir_ptr->SEARCH_DATA);
      }         
      
      if (error_code != MFS_NO_ERROR) {
         break;
      }
      
      error_code = ioctl(fshandle, IO_IOCTL_GET_LFN, (void *)&dir_ptr->LFN_DATA);

      if ( !error_code ) { 
         WL_TEST_LOG("\n%s\n", (char *)&dir_ptr->LFN);
      } else {
         WL_TEST_LOG("\n%s\n", (char *)&dir_ptr->SEARCH_DATA.NAME);
      }
   }
   _io_mfs_dir_close(dir_ptr);
   
   /* Read written file */
   WL_TEST_LOG("Read file %s \n", filename);
   sprintf(fpath, "%s\\%s", fsname, filename);
   
   fhandle = fopen(fpath, "r");
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (fhandle != NULL), " Test #8.10: open the MFS file to read");

   fseek(fhandle, 0, IO_SEEK_SET );
   readcontent = (char *)allocate_buffer(contentsize, "readcontent");
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (readcontent != NULL), " Test #8.11: Allocate the read_buffer buufer");
   
   result = read(fhandle, (unsigned char *)readcontent, contentsize);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == contentsize), " Test #8.12: Read data from mfs file to read_buffer buffer");
   

   dump_buff((unsigned char *)readcontent, contentsize, fpath);
   fclose(fhandle);
   fhandle = NULL;
   
   /* Close MFS */
   if (NULL != fshandle ) {
      system_test_mfs_close(fshandle, fsname);
   }
   
   /* Shutdown Nand Media & DataDrive */
   status = system_test_finalize(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (status == SUCCESS), " Test #8.13: shutdown & de-init nand media & data drive");
   
   
   amount = timer->getElapsed();
   delete timer;
   total += amount;
   WL_TEST_LOG("\n%d microsecond(s) for the second lap\n", (uint32_t)amount);
   WL_TEST_LOG("\n%d microsecond(s) for all processes\n", (uint32_t)total);
   
   _mem_free(readcontent);
   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), " Test #8.14: Close the nand_wl file");
  
}


/*******************************************************************************************************************/

/*TASK*-----------------------------------------------------------------
*
* Function Name  : main_task
* Comments       : The task is used to test the init/shutdown process
*     
*
*END------------------------------------------------------------------*/

void main_task(uint32_t initial_data)
{
    if (_io_nandflash_wl_install(&_bsp_nandflash_wl_init, NAND_FLASH_WL_FILE) != MQX_OK)
    {
      /* Can not install FFS */
      _task_block();
    }

    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}/* Endbody */

/* EOF */