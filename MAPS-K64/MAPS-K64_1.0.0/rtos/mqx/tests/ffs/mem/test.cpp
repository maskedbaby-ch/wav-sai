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
* $FileName: test.cpp$
* $Version : 3.7.2.0$
* $Date    : Mar-17-2011$
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
#include "ddi_media.h"
#include "simple_mutex.h"
#include "test.h"
#include "testing_main_internal.h"

void red_write_task(uint32_t);

// Test suite function prototype
void tc_1_main_task(void);
void tc_2_main_task(void);

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_mutex)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Test Simple mutex "),   
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #1 - Test Simple mutex "),
EU_TEST_SUITE_END(suite_mutex)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_mutex, " - Testing simple mutex")
EU_TEST_REGISTRY_END()

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   main_task,   2000,  9,   "main", MQX_AUTO_START_TASK},
   {0,           0,           0,     0,   0,      0,                 }
};

bool read_write_success = FALSE;
unsigned char * write_buffer;
unsigned char * read_buffer;
MQX_FILE_PTR            nand_wl_ptr;

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_1_main_task
* Comments       :
*    Test simplemutex for NAND driver
*
*END------------------------------------------------------------------*/

void tc_1_main_task(void)
{
    _task_id                task_id;
    uint32_t                 i, test_count = 1000;
    TASK_TEMPLATE_STRUCT    template_task1;
    TASK_TEMPLATE_STRUCT    template_task2;
    uint32_t                 result, buf_size;
       
    /* Step 1: Init Task's template*/
     template_task1.TASK_TEMPLATE_INDEX = 10;
     template_task1.TASK_ADDRESS = (void (_CODE_PTR_)(uint32_t))red_write_task;
     template_task1.TASK_STACKSIZE = WL_TASK_STACK_SIZE;
     template_task1.TASK_PRIORITY = WL_TASK_PRIORITY;
     template_task1.TASK_NAME = WL_TASK1_NAME;
     template_task1.CREATION_PARAMETER = result;

     template_task2.TASK_TEMPLATE_INDEX = 11;
     template_task2.TASK_ADDRESS = (void (_CODE_PTR_)(uint32_t))red_write_task;
     template_task2.TASK_STACKSIZE = WL_TASK_STACK_SIZE;
     template_task2.TASK_PRIORITY = WL_TASK_PRIORITY;
     template_task2.TASK_NAME = WL_TASK1_NAME;
     template_task2.CREATION_PARAMETER = result;
     
    /* Step 1: Open NANDFLASH WL*/
     nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_NORMAL);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (nand_wl_ptr != NULL), " Test #1.1: Open FFS file in normal mode failed");
    
    /* ERASE entire nandflash chip */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == TRUE), " Test #1.2: Erase entire chip");
    
    result = nandwl_testing_get_info(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == IO_OK), " Test #1.3: Get information of Nand_wl ");
    buf_size = nandwl_phy_page_size + nandwl_phy_page_spare_size;
    
    write_buffer = allocate_buffer(buf_size, "write");
    fill_continous_buff(write_buffer, buf_size);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (write_buffer != NULL), " Test #1.4: Allocate new buffer (write_buffer)");

    read_buffer = allocate_buffer(buf_size, "read");
    zero_buff(read_buffer, buf_size );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (read_buffer != NULL), " Test #1.5: Allocate new buffer (read_buffer)");
    
    /* Step 4: Create Task */
    task_id = _task_create(0, 0, (uint32_t)&template_task1);
    /* Verify Step 4 */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (task_id != MQX_NULL_TASK_ID), " Test #1.6: Testing create task1");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (read_write_success == TRUE), " Test #1.7: Test read/write file in separate task failed");
    /* Destroy task */
    _task_destroy (task_id);
    task_id = _task_create(0, 0, (uint32_t)&template_task2);
    /* Verify Step 4 */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (task_id != MQX_NULL_TASK_ID), " Test #1.8: Testing create task2 ");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (read_write_success == TRUE), " Test #1.9: Test read/write data in separate task");
    /* Destroy task */
    _task_destroy (task_id);
    
    result = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), " Test #1.14: Close nand_wl object file ");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_2_main_task
* Comments       :
*    Test simplemutex for NAND driver
*
*END------------------------------------------------------------------*/

void tc_2_main_task(void)
{
    _task_id                task_id;
    uint32_t                i, test_count = 1000;
    TASK_TEMPLATE_STRUCT    template_task;
    uint32_t                 result, buf_size;
    _task_id                tasks[NUMBER_OF_TASKS];
    TASK_TEMPLATE_STRUCT    templates[NUMBER_OF_TASKS] = { 0 };

    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_NORMAL);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (nand_wl_ptr != NULL), " Test #2.1: Open FFS file in normal mode");
    
    /* ERASE entire nandflash chip */
    result = nandwl_erase_entire_chip(nand_wl_ptr, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK, NANDFLASH_1ST_DATA_DRIVE_START_BLOCK + NANDFLASH_1ST_DATA_DRIVE_SIZE_IN_BLOCK);;
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == TRUE), " Test #2.2: Erase entire chip");

    buf_size = nandwl_phy_page_size + nandwl_phy_page_spare_size;
    
    write_buffer = allocate_buffer(buf_size, "write");
    fill_continous_buff(write_buffer, buf_size);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (write_buffer != NULL), " Test #2.3: Allocate new buffer (write_buffer)");

    read_buffer = allocate_buffer(buf_size, "read");
    zero_buff(read_buffer, buf_size );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (read_buffer != NULL), " Test #2.4: Allocate new buffer (read_buffer)");

    for (i = 0; i < NUMBER_OF_TASKS; i++)
    {
        templates[i].TASK_TEMPLATE_INDEX = 10 + i; // Make current task not duplicate with system task
        templates[i].TASK_ADDRESS = (void (_CODE_PTR_)(uint32_t))red_write_task;
        templates[i].TASK_STACKSIZE = WL_TASK_STACK_SIZE;
        templates[i].TASK_PRIORITY = WL_TASK_PRIORITY;
        templates[i].TASK_NAME = WL_TASK_NAME;
        templates[i].CREATION_PARAMETER = 0;
        
        read_write_success = FALSE;
        
        tasks[i] = _task_create(0, 0, (uint32_t)&templates[i]);
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( tasks[i] != MQX_NULL_TASK_ID), " Test #2.5: Create Tasks failed");
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (read_write_success == TRUE), " Test #2.6: Test read/write data in separate task failed");
    }
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : red_write_task
* Comments       :
*
*
*END------------------------------------------------------------------*/

void red_write_task(uint32_t args) {

    uint32_t              buf_size, secnum;
    uint32_t              rand, numberSector, result;
    
    rand = simple_rand();
    result = ioctl(nand_wl_ptr, IO_IOCTL_GET_NUM_SECTORS, &numberSector);
    secnum = rand % numberSector;
    buf_size = 1;
    fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
    result = fwrite(write_buffer, 1, buf_size, nand_wl_ptr);
    if (result != buf_size) {
        read_write_success = FALSE;
        _task_block();
    }
    fseek(nand_wl_ptr, secnum, IO_SEEK_SET);
    result = fread(read_buffer, 1, buf_size, nand_wl_ptr);
    if (result != buf_size) {
        read_write_success = FALSE;
        _task_block();
    }
    result = compare_test(write_buffer, read_buffer, nandwl_virt_page_size);
    if (FALSE == result) {
        read_write_success = FALSE;
        _task_block();
    }
    read_write_success = TRUE;
    _task_block();
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : main_task
* Comments       : The task is used to test simple mutex
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
