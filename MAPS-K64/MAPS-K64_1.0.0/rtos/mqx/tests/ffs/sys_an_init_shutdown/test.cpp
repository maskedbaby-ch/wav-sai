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
#include "wl_common.h"
#include "testing_main_internal.h"
#include "ddi_nand_hal.h"
#include "system_testing.h"
#include "system_testing_internal.h"
#include "simple_timer.h"
#include "mem_management.h"
#include "test.h"

// Test suite function prototype
void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
void tc_4_main_task(void);
void tc_5_main_task(void);
void tc_6_main_task(void);

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_sys_init_shutdown)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1: Test init/shutdown process in abnormal case"),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2: Test read/write sector while re-init media in abnormal case"),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3: Test repair process in abnormal case"),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4: Test read/write a files/directories while re-init MFS"),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5: Measure the time of init/shutdown process in normal case"),
    EU_TEST_CASE_ADD(tc_6_main_task, " Test #6: Test the init/shutdown MFS in loop 100 times"),
EU_TEST_SUITE_END(suite_sys_init_shutdown)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_sys_init_shutdown, " - Testing System Init-Shutdown in abnormal case")
EU_TEST_REGISTRY_END()

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   main_task   ,   2000,  9,   "main", MQX_AUTO_START_TASK},
   {0,           0,           0,     0,   0,      0,                 }
};

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_1_main_task
* Comments       :
*  This function is used to test init/shutdown process in abnormal case
*
*  This test case is manual:
*  - Erase chip before running this testcase
*  - Enable log on mapper module
*  - Using debugger
*  - Set breakpoint at system_test_finalize_with_spy in the first time then reboot board
*  - Run the second time normally.
*END------------------------------------------------------------------*/

void tc_1_main_task(void ) {
   MQX_FILE_PTR        nand_wl_ptr; 
   uint32_t             result;
   RtStatus_t          status;

   printf("\n============ tc_1_main_task ============\n");
   /* Step 1: Open NANDFLASH WL*/ 
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, "I");
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (nand_wl_ptr != NULL), " Test #1.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);
   
   status =  system_test_initialize(TRUE, nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status != SUCCESS), " Test #1.2: Initialize Nand flash system with spy is failed");
  
   status = system_test_finalize(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (status != SUCCESS), " Test #1.3: De-init nand flash system");
   
   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result != MQX_OK), " Test #1.4: Close the nand_wl flash file");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_2_main_task
* Comments       :
*  This function is used to test read/write sector while re-init media in abnormal case
*
*  This test case is manual:
*  - Erase chip before running this testcase
*  - Enable log on mapper module and NFC_DEBUG
*  - Using debugger
*  - Set breakpoint at read in the first time then reboot board
*  - Comment writing code block
*  - Run the second time normally.
*END------------------------------------------------------------------*/
void tc_2_main_task(void ) {
   MQX_FILE_PTR           nand_wl_ptr;
   uint32_t                buf_size; 
   unsigned char *              write_buffer; 
   unsigned char *              read_buffer;
   uint32_t                secnum = 100;
   uint32_t                i = 0;
   uint32_t                result;
   RtStatus_t             status;
   
   /* Step 1: Open NANDFLASH WL*/ 
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, "I");
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (nand_wl_ptr != NULL), " Test #2.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);
      
   /* ERASE entire nandflash chip */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == TRUE), " Test #2.2: Erase entire chip");
   
   printf("\n============ tc_2_main_task ============\n");
   
   nandwl_testing_get_info(nand_wl_ptr);    /*Get nandwl_phy_page_size & nandwl_phy_page_spare_size*/
   buf_size = nandwl_phy_page_size + nandwl_phy_page_spare_size; 
         
   write_buffer = allocate_buffer(buf_size, "write");
   fill_continous_buff(write_buffer, buf_size);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (write_buffer != NULL), " Test #2.3: Allocate write_buffer");      
   
   read_buffer = allocate_buffer(buf_size, "read");
   zero_buff(read_buffer, buf_size );
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (read_buffer != NULL), " Test #2.4: Allocate read_buffer");         
   
   status =  system_test_initialize(TRUE,nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status == SUCCESS), " Test #2.5: Init Nand_wl flash logical drive");            
   
   WL_TEST_LOG("Write data on lsector %d \n", secnum);
   fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
   result = write(nand_wl_ptr, write_buffer, 1);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == 1), " Test #2.6: Write 1 sector to Nandflash");               
   WL_TEST_LOG("Read data on lsector %d \n", secnum);
   fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
   result = read(nand_wl_ptr, read_buffer, 1);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == 1), " Test #2.7: Read 1 sector from Nandflash");               
   
   status = system_test_finalize(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (status == SUCCESS), " Test #2.8: Shutdown & De-init logical media & data drive");               
   
   _mem_free(write_buffer);
   _mem_free(read_buffer);
   
   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), " Test #2.9: Close the nand_wl file");                  
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_3_main_task
* Comments       :
*    This function is used to test repair process in abnormal case
*
*END------------------------------------------------------------------*/
void tc_3_main_task(void ) {
   MQX_FILE_PTR         nand_wl_ptr;
   RtStatus_t           status;
   _mqx_int             result;
   
   printf("\n============ tc_3_main_task ============\n");
   
   /* Step 1: Open NANDFLASH WL*/ 
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, "I");
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (nand_wl_ptr != NULL), " Test #3.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);
   
   status = system_test_initialize_drive_failed(nand_wl_ptr);  
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (status == ERROR_DDI_LDL_LDRIVE_REPAIR_REQUIRED), " Test #3.2: Init nand_wl logical manager & data drive");   
   
   result = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_REPAIR, NULL);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK || result == ERROR_DDI_LDL_LDRIVE_FS_FORMAT_REQUIRED), " Test #3.3: Init nand_wl logical manager & data drive");   
   
   status = system_test_finalize(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (status == SUCCESS), " Test #3.4: Shutdown & de-init nand_wl logical manager & data drive");   
   
   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), " Test #3.5: Close the nand_wl file");      
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_4_main_task
* Comments       :
*    This function is used to test read/write a file/directories while re-init MFS
*
*END------------------------------------------------------------------*/
void tc_4_main_task(void ) 
{
    int32_t          write_buffer_size[]= { 512 };
    unsigned char *       write_buffer_arr[sizeof(write_buffer_size) / sizeof(uint32_t)] = { NULL }; 
    unsigned char *       read_buffer_arr[sizeof(write_buffer_size) / sizeof(uint32_t)] = { NULL };
    dir_arrs_type   mkdirs_arr={ 0 };
    dir_arrs_type   readdirs_arr={ 0 };
    uint32_t         num_files_write = 1, num_mkdirs = 1, num_readdirs;
    char *        fsname = "a:";
    uint32_t         i = 0, j = 0;
    uint32_t         index;
    MQX_FILE_PTR    fhandle = NULL;
    MQX_FILE_PTR    fshandle = NULL;
    MQX_FILE_PTR    nand_wl_ptr;
    uint32_t         result;
    RtStatus_t      status; 
    char            fpath[PATHNAME_SIZE] = {0};
    int32_t          error;
    _mfs_cache_policy policy = MFS_WRITE_THROUGH_CACHE;
    DIR_STRUCT_PTR  dir_ptr;
    char            dir_name[MAX_DIR_NAME_LENGTH];
    char            dir_path[PATHNAME_SIZE] =   { 0 };
    _mqx_int        error_code;     

    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, "I");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (nand_wl_ptr != NULL), " Test #4.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    /* ERASE entire nandflash chip */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == TRUE), " Test #4.2: Erase entire nand_wl flash");    

    /* Allocate buffer */
    index = sizeof(write_buffer_size) / sizeof(uint32_t);
    for (i = 0; i < index; i++)
    {
        write_buffer_arr[i] = allocate_buffer(write_buffer_size[i], "write");
        fill_continous_buff(write_buffer_arr[i], write_buffer_size[i]);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (write_buffer_arr[i] != NULL), " Test #4.3: Allocate new buffer for write_buffer_arr");         
         
        read_buffer_arr[i] = allocate_buffer(write_buffer_size[i], "read");
        zero_buff(read_buffer_arr[i],  write_buffer_size[i]);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (read_buffer_arr[i] != NULL), " Test #4.4: Allocate new buffer for read_buffer_arr");                 
    }

    WL_TEST_LOG("\n ============== tc_4_main_task  ================== \n");

    /* ******************************
            INIT and write
    ****************************** */

    status =  system_test_initialize(TRUE,nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS), " Test #4.5: Init nand_wl flash system (logical media & data drive)");

    result = system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, true);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MFS_NO_ERROR), " Test #4.6: Install & open the MFS file");    

    /* Write file testing */
    WL_TEST_LOG("\n ********** Write file testing ********** \n");
    ioctl(fshandle, IO_IOCTL_SET_WRITE_CACHE_MODE, (void *)&policy);

    for (i = 0; i < num_files_write; i++) 
    {   
        index = i % (sizeof(write_buffer_size) / sizeof(uint32_t));
        sprintf(fpath, "%s\\FILE%d", fsname, i);
        WL_TEST_LOG("Write data on file %s \n",&fpath);
        
        fhandle = fopen(fpath, "w");
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (fhandle != NULL), " Test #4.7: Open the file for writing");           
        
        fseek(fhandle, 0, IO_SEEK_SET );
        WL_TEST_LOG("Write to file %d by using buffer 0x%0x with size %d\n", 
        i,(unsigned char *)(write_buffer_arr[index]), write_buffer_size[index] );
        
        result = write(fhandle, (unsigned char *)(write_buffer_arr[index]), write_buffer_size[index]);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == write_buffer_size[index]), " Test #4.8: Write to fhandle file");

        /*dump_buff((unsigned char *)(write_buffer_arr[i]), 512, fpath);*/
        fclose(fhandle);
        fhandle = NULL;
    }   

    /* Create directory */
    /* Move to root directory X:\ */
    sprintf(fpath, "%s\\", fsname );
    error = ioctl(fshandle, IO_IOCTL_CHANGE_CURRENT_DIR, fpath);
    
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (error == IO_OK), " Test #4.9: Move to Root directory");    
     
    WL_TEST_LOG("\n ------------> Create dir %d   ================== \n", j);
    for (i = 0; i < num_mkdirs; i++ )
    {
        sprintf( (char *)&mkdirs_arr[i], "DIR%d", i);
    }

    i = 0;
    while (i < num_mkdirs)
    {      
        WL_TEST_LOG("Create dir %s \n", mkdirs_arr[i]);        
        error = ioctl(fshandle, IO_IOCTL_CREATE_SUBDIR, (void *) mkdirs_arr[i]);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (error == IO_OK), " Test #4.10: Create a new directory");
        i++; 
    }

    if (NULL != fshandle )
    {
        system_test_mfs_close(fshandle, fsname);
    }

    status = system_test_finalize(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS), " Test #4.11: Shutdown & de-init media buffer & data drive");   

    /* ******************************
            RESET and read
    ****************************** */
    status =  system_test_initialize(TRUE,nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS), " Test #4.12 : Init logical media & data drive");
    
    fshandle = NULL;   
    result = system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, false);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MFS_NO_ERROR), " Test #4.13 : Install & open the MFS file");

    /* Read file testing and compare */
    WL_TEST_LOG("\n ********** Read file testing ********** \n");

    for (i = 0; i < num_files_write; i++) 
    {   
        index = i % (sizeof(write_buffer_size) / sizeof(uint32_t));
        sprintf(fpath, "%s\\FILE%d", fsname, i);
        WL_TEST_LOG("Read data on file %s \n",&fpath);
        
        fhandle = fopen(fpath, "r");
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (fhandle != NULL), " Test #4.14 : Open file for reading");

        fseek(fhandle, 0, IO_SEEK_SET );
        WL_TEST_LOG("\nRead in file %d by using buffer 0x%0x with size %d\n", 
        i,(unsigned char *)(read_buffer_arr[index]), write_buffer_size[index] );
        
        uint32_t actual = read(fhandle, (unsigned char *)(read_buffer_arr[index]), write_buffer_size[index]);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (actual == write_buffer_size[index]), " Test #4.15 : Read data from MFS file");
                
        fclose(fhandle);
        fhandle = NULL;
    }  

    sprintf(fpath, "%s\\", fsname); /* Root dir */
    error = ioctl(fshandle, IO_IOCTL_CHANGE_CURRENT_DIR, fpath);

    /* Read directory */
    /*  Move to root directory */
    sprintf((char *)&dir_path, "%s\\", fsname );
    error = ioctl(fshandle, IO_IOCTL_CHANGE_CURRENT_DIR, (char *)&dir_path);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (error == IO_OK), " Test #4.16 : Move to root directory");    
    
    sprintf((char *)&dir_path, "%s\\*.*", fsname);      
    dir_ptr =(DIR_STRUCT_PTR) _io_mfs_dir_open(fshandle, (char *)&dir_path, "d" ); /*MFS_ATTR_DIR_NAME*/

    i = 0;
    while (i < num_mkdirs)
    {

        if ( dir_ptr->FIRST )
        {
            error_code = ioctl(fshandle, IO_IOCTL_FIND_FIRST_FILE, (void *)&dir_ptr->SEARCH);
            dir_ptr->FIRST = FALSE;
        }
        else
        {
            error_code = ioctl(fshandle, IO_IOCTL_FIND_NEXT_FILE,  (void *) &dir_ptr->SEARCH_DATA);
        }
        if ( error_code != MFS_NO_ERROR )
        {
            break;
        }
        
        error_code = ioctl(fshandle, IO_IOCTL_GET_LFN, (void *)&dir_ptr->LFN_DATA);

        if ( !error_code )
        { 
            strcpy( (char *)&readdirs_arr[i] , (char *)&dir_ptr->LFN);
            WL_TEST_LOG("Read dir %s\n", (char *)&readdirs_arr[i]);
            i++;
        }
        else
        {
            strcpy( (char *)&readdirs_arr[i] , (char *)&dir_ptr->SEARCH_DATA.NAME);
            WL_TEST_LOG("Read dir %s\n", (char *)&readdirs_arr[i]);
            i++;
        }
    }

    _io_mfs_dir_close(dir_ptr);

    if ( NULL != fshandle )
    {
        system_test_mfs_close(fshandle, fsname);        
    }
    
    status = system_test_finalize(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (status == SUCCESS), " Test #4.17 : Shutdown & de-init logical media & data drive");    

    /* Free buffer */
    for (i=0; i< num_files_write; i++)
    {
        if (write_buffer_arr[i]!= NULL) _mem_free(write_buffer_arr[i]);
        if (read_buffer_arr[i]!= NULL) _mem_free(read_buffer_arr[i]);
    }
    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), " Test #4.18 : Close the nand_wl file ");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_5_main_task
* Comments       :
*    This function is used to measure the time of init/shutdown in abnormal case
*
*END------------------------------------------------------------------*/
void tc_5_main_task(void ) {
   SimpleTimer *          timer;
   uint64_t                amount = 0;
   uint64_t                total = 0;
   char *               fsname = "a:";
   MQX_FILE_PTR           fshandle = NULL;
   MQX_FILE_PTR           fhandle = NULL;
   MQX_FILE_PTR           nand_wl_ptr;
   RtStatus_t             status;
   uint32_t                result;
     
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, "I");
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (nand_wl_ptr != NULL), " Test #5.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);
   
   /* Init with spy */
   timer = new SimpleTimer;
   
   status =  system_test_initialize(TRUE,nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), " Test #5.2: Initialize the logical media & buffer manager");   
   
   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for initializing MEDIA with spy\n", (uint32_t)amount);
   delete timer;
   
   /* Open MFS */
   timer = new SimpleTimer;
   
   result = system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, TRUE);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MFS_NO_ERROR), " Test #5.3: Install & open the MFS file ");        

   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for opening MFS\n", (uint32_t)amount);
   delete timer;
   
   /* Close MFS */
   timer = new SimpleTimer;
   
   if (NULL != fshandle ) {
      result = system_test_mfs_close(fshandle, fsname);
      EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MFS_NO_ERROR), " Test #5.4: Close the MFS file ");            
   }
   
   amount = timer->getElapsed();
   total += amount;
   
   WL_TEST_LOG("\n%d microsecond(s) for closing MFS\n", (uint32_t)amount);
   delete timer;
   
   /* Shutdown */
   timer = new SimpleTimer;
   
   status = system_test_finalize(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (status == SUCCESS), " Test #5.5: Shutdown & de-init ");            

   amount = timer->getElapsed();
   total += amount;
   
   delete timer;
   
   WL_TEST_LOG("\n%d microsecond(s) for shutdowning MEDIA\n", (uint32_t)amount);
   
   WL_TEST_LOG("\n%d microsecond(s) for all processes\n", (uint32_t)total);
   result = fclose(nand_wl_ptr);
 
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_6_main_task
* Comments       :
*    Test init/shutdown MFS in loop-for with 20 times
*
*END------------------------------------------------------------------*/
void tc_6_main_task(void ) {
   char *         fsname = "a:";
   MQX_FILE_PTR     fshandle = NULL;
   MQX_FILE_PTR     fhandle = NULL;
   uint32_t          result,i;
   MQX_FILE_PTR     nand_wl_ptr;
   RtStatus_t       status;
   
   nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, "I");
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (nand_wl_ptr != NULL), " Test #6.1: Open file failed");
   NandHalSetMqxPtr(0, nand_wl_ptr);
       
   /* ERASE entire nandflash chip */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == TRUE), " Test #6.2: Erase entire nand flash");
   
   for (i = 0; i < 20; i++) {  
    status = system_test_initialize((i==0)? TRUE :FALSE, nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), " Test #6.3: Init logical media & nand drive");
 
    result = system_test_mfs_open(nand_wl_ptr, fsname, &fshandle, (i==0)? TRUE :FALSE);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MFS_NO_ERROR), " Test #6.4: Install & open the MFS");
      
    if (NULL != fshandle ) {
       result = system_test_mfs_close(fshandle, fsname);
       EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MFS_NO_ERROR), " Test #6.5: Close the MFS");
    }
    
    status = system_test_finalize(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (status == SUCCESS), " Test #6.6: Shutdown & de-init the logical media & nand drive");      
   }   
   
   result = fclose(nand_wl_ptr);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), " Test #6.7: Close the nand_wl file ");             
}


/*******************************************************************************************************************/

/*TASK*-----------------------------------------------------------------
*
* Function Name  : main_task   
* Comments       : This task test init & shudown drive in abnormal case
*    
*
*END------------------------------------------------------------------*/

void main_task (uint32_t initial_data)
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