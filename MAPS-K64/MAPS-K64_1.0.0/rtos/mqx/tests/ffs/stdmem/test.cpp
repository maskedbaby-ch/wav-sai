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
* $FileName: test.h$
* $Version : 3.7.2.0$
* $Date    : Mar-17-2011$
*
* Comments:
*
*   This file is used to test NFCNandMedia functions; including:
*                      WritePage
*                      ReadPage
*                      ReadPageMetaData
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
#include "mutex.h"
#include "nandflash_wl_testing.h"
#include "system_testing_internal.h"
#include "testing_main_internal.h"
#include "media_buffer.h"
#include "media_buffer_manager.h"
#include "media_buffer_manager_internal.h"
#include "mem_management_internal.h"
#include "mem_management.h"
#include "test.h"

//------------------------------------------------------------------------
void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
void tc_4_main_task(void);
void tc_5_main_task(void);
void tc_6_main_task(void);
void task_function(uint32_t args);
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_stdmem)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Test allocate/free memory in single task"),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Test allocate/free memory in multiple task"),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Test the buffer is free when calling allocated memory function"),
EU_TEST_SUITE_END(suite_stdmem)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_stdmem, " - Testing Standard memory")
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
* Comments       :
*       The function is used to test alloc/free memory in single task
*
*END------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    uint32_t         allocate_size = 512, error;
    char *        p = NULL;
    MQX_FILE_PTR    nand_wl_ptr;

    /* Step 1: Open NANDFLASH WL*/
     nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (nand_wl_ptr != NULL), " Test #1.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    error = media_buffer_init();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error == SUCCESS), " Test #1.1: Initialize media buffer manager");

    media_buffer_report_stats();

    printf("\nAllocate 512 bytes using _wl_mem_alloc\n");
    p = (char *)_wl_mem_alloc(allocate_size);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (p != NULL), " Test #1.2: Allocate p Buffer");
    printf("\nFree allocated bytes using _wl_mem_free\n");
    _wl_mem_free(p);

    error =  media_buffer_deinit();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (error == SUCCESS), " Test #1.3: De-init media buffer manager");

    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (g_mem_allocation_table.totalAllocSize == g_mem_allocation_table.totalFreedSize), " Test #1.4: Comapare total size which is allocated and Freed");

     fclose(nand_wl_ptr);
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_2_main_task
* Comments       :
*      The function is used to test alloc/free memory in multiple tasks
*
*END------------------------------------------------------------------*/

void tc_2_main_task(void){
    uint32_t                 i = 0;
    _task_id                tasks[NUMBER_OF_TASKS];
    TASK_TEMPLATE_STRUCT    templates[NUMBER_OF_TASKS] = { 0 };
    MQX_FILE_PTR            nand_wl_ptr;
    uint32_t                 error;

    /* Step 1: Open NANDFLASH WL*/
     nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (nand_wl_ptr != NULL), " Test #2.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    error = media_buffer_init();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (error == SUCCESS), " Test #2.1: Initialize media buffer manager");

    media_buffer_report_stats();
    mm_display();

    for (i = 0; i < NUMBER_OF_TASKS; i++)
    {
        templates[i].TASK_TEMPLATE_INDEX = 10 + i; // Make current task not duplicate with system task
        templates[i].TASK_ADDRESS = (void (_CODE_PTR_)(uint32_t))task_function;
        templates[i].TASK_STACKSIZE = WL_TASK_STACK_SIZE;
        templates[i].TASK_PRIORITY = WL_TASK_PRIORITY;
        templates[i].TASK_NAME = WL_TASK_NAME;
        templates[i].CREATION_PARAMETER = i;
        tasks[i] = _task_create(0, 0, (uint32_t)&templates[i]);
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( tasks[i] != MQX_NULL_TASK_ID), " Test #2.3: Create Tasks");
    }

    error = media_buffer_deinit();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( error == SUCCESS), " Test #2.4: De-init media buffer manager");
    mm_display();
    error = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ( error == MQX_OK), " Test #2.5: Close nand_wl_ptr ");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_3_main_task
* Comments       :
*      The function is used to test that the buffer is not free when
*      calling allocated memory function
*
*END------------------------------------------------------------------*/

void tc_3_main_task(void)
{
     MQX_FILE_PTR   nand_wl_ptr;
    uint32_t         allocate_size = 512, error;
    char *        p = NULL;

    /* Step 1: Open NANDFLASH WL*/
    nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (nand_wl_ptr != NULL), " Test #3.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    error = media_buffer_init();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error == SUCCESS), " Test #3.2: Initialize media buffer manager");

    media_buffer_report_stats();

    printf("\nAllocate 512 bytes using _wl_mem_alloc\n");
    p = (char *)_wl_mem_alloc(allocate_size);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (p  != NULL), " Test #3.3: Allocate p Buffer");

    mm_display();
    mm_display_unfree();

    printf("\nFree allocated bytes using _wl_mem_free\n");
    _wl_mem_free(p);
    error = media_buffer_deinit();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (error  == SUCCESS), " Test #3.4: De-init media buffer manager");

    EU_ASSERT_REF_TC_MSG(tc_3_main_task,  (g_mem_allocation_table.totalAllocSize == g_mem_allocation_table.totalFreedSize), " Test #3.5: Compare total size which is allocated & which is freed");

    error = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task,  (error == MQX_OK), " Test #3.6: Close the nand_wl file object");

}
// TODO: Manual Test Cases
#if 0
/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_4_main_task
* Comments       :
*      The function is used to test alloc memory by using _mem_alloc function
*
*END------------------------------------------------------------------*/

void tc_4_main_task(void)
{
     MQX_FILE_PTR nand_wl_ptr;
    uint32_t allocate_size = 512, error;
    char * p1 = NULL, p2 = NULL;

    /* Step 1: Open NANDFLASH WL*/
     nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (nand_wl_ptr != NULL), " Test #4.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    error = media_buffer_init();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (error == SUCCESS), " Test #4.2: Initialize media buffer manager");

    media_buffer_report_stats();

    printf("\nAllocate 512 bytes on p1 using _wl_mem_alloc\n");
    p1 = (char *)_wl_mem_alloc(allocate_size);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (p1 != NULL), " Test #4.3: Allocated p1 buffer");

    printf("\nAllocate 512 bytes on p2 using _mem_alloc\n");
    p2 = (char *)_mem_alloc(allocate_size);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (p2 != NULL), " Test #4.4: Allocated p2 buffer");

    printf("\nFree allocated p1 using _wl_mem_free\n");
    _wl_mem_free(p1);
    printf("\nFree allocated p2 using _wl_mem_free\n");
    _wl_mem_free(p2);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (TRUE), " Test #4.5: Freed all allocated buffer");

    error = media_buffer_deinit();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (error == SUCCESS), " Test #4.6: De-init media buffer manager");

     error = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (error == MQX_OK), " Test #4.7: Close Nand_wl file object");

}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_5_main_task
* Comments       : The task is used to test standard memory
*
*
*END------------------------------------------------------------------*/

void tc_5_main_task(void)
{
     MQX_FILE_PTR       nand_wl_ptr;
    uint32_t             allocate_size = 512, error;
    char *            p = NULL;

   /* Step 1: Open NANDFLASH WL*/
     nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (nand_wl_ptr != NULL), " Test #5.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    error = media_buffer_init();
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (error == SUCCESS), " Test #5.2: Initialize media buffer manager");

    media_buffer_report_stats();

    printf("\nAllocate 512 bytes using _wl_mem_alloc\n");
    p = (char *)_wl_mem_alloc(allocate_size);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (p != NULL), " Test #5.3: Allocated p buffer");

    printf("\nWrite to byte whose index is 513...\n");
    p[allocate_size] = 0xFF;
    printf("\nFree allocated bytes using _wl_mem_free\n");
    _wl_mem_free(p);

    error = media_buffer_deinit();
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (error == SUCCESS), " Test #5.4: De-init media buffer manager");

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (g_mem_allocation_table.totalAllocSize != g_mem_allocation_table.totalFreedSize), " Test #5.5: Compare total size which is allocated & freed");

    error = fclose(nand_wl_ptr);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (error == MQX_OK), " Test #5.6:Close Nand_wl file object");

}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : tc_6_main_task
* Comments       :
*
*
*END------------------------------------------------------------------*/

void tc_6_main_task(void )
{
    uint32_t         allocate_size = 512, error;
    char *        p = NULL;
    MQX_FILE_PTR    nand_wl_ptr;

    /* Step 1: Open NANDFLASH WL*/
     nand_wl_ptr = fopen(NAND_FLASH_WL_FILE, NANDWL_OPEN_IGNORE_INIT_MEDIA);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (nand_wl_ptr != NULL), " Test #6.1: Open file failed");
    NandHalSetMqxPtr(0, nand_wl_ptr);

    error = media_buffer_init();
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (error == SUCCESS), " Test #6.2: Initialize media buffer manager");

    media_buffer_report_stats();

    printf("\nAllocate 512 bytes using _wl_mem_alloc\n");
    p = (char *)_wl_mem_alloc(allocate_size);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (p != NULL), " Test #6.3: Allocated p buffer");

    printf("\nWrite to byte whose index is -1...\n");
    p[-1] = 0xFF;
    printf("\nFree allocated bytes using _wl_mem_free\n");
    _wl_mem_free(p);

    error = media_buffer_deinit();
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (error == SUCCESS), " Test #6.4: De-init media bufffer manager");

    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (g_mem_allocation_table.totalAllocSize != g_mem_allocation_table.totalFreedSize), " Test #6.5:Compare total size which is allocated & freed");

     error = fclose(nand_wl_ptr);
}
#endif
/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : task_function
* Comments       :
*
*
*END------------------------------------------------------------------*/

void task_function(uint32_t args)
{
    uint32_t     allocate_size = 512;
    char *    p = NULL;

    printf("\nTask #%d\tallocate %d bytes by using _wl_mem_alloc\n", args, allocate_size);
    p = (char *)_wl_mem_alloc(allocate_size);
    printf("\nTask #%d\tfree %d bytes by using _wl_mem_free\n", args, allocate_size);
    _wl_mem_free(p);
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : main_task
* Comments       :
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
