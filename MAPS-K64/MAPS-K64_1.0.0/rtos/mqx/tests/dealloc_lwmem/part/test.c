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
* $Version : 3.8.3.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file contains the source for partition deallocation testing.
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <name.h>
#include <name_prv.h>
#include <partition.h>
#include <partition_prv.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test
#define MAIN_TASK       10
#define TEST_TASK       11
#define CLEAN_TASK      12

#define BLOCK_SIZE      100
#define BLOCK_CNT       4
//------------------------------------------------------------------------
// Test suite function prototype
void main_task(uint32_t parameter);
void test_task(uint32_t parameter);
void clean_task(uint32_t parameter); 
void tc_1_main_task(void);//Testing deallocation after task exits
//------------------------------------------------------------------------                      
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
{ MAIN_TASK, main_task,  2000,   8, "Main_Task", MQX_AUTO_START_TASK},
{ TEST_TASK, test_task,  1000,   7, "Test_Task", 0},
{ CLEAN_TASK, clean_task,  1000,   7, "CleanUp_Task", 0},
{ 0,         0,          0,      0, 0,           0}
};

KERNEL_DATA_STRUCT_PTR  kernel_data;
_partition_id  part_id;
_mem_size      system_owned;

//destroy partition created in test_task
void clean_task(uint32_t index)
{
   _mqx_uint   result;
   
   result = _partition_destroy(part_id);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.2 Creating PARTITION ")
}

//create partition and allocate blocks from it
void test_task(uint32_t index)
{
   _mqx_uint   free_block_cnt;
   _mem_size   pre_free_mem, post_free_mem;
   void       *block, *zero_block;
   
   pre_free_mem = get_free_mem();
   part_id = _partition_create(BLOCK_SIZE, BLOCK_CNT, 0, 0);
   post_free_mem = get_free_mem();
   system_owned = pre_free_mem - post_free_mem;
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, part_id != PARTITION_NULL_ID, "Test #1: Testing 1.2 Creating PARTITION ")
   
   free_block_cnt = _partition_get_free_blocks(part_id);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, free_block_cnt == BLOCK_CNT, "Test #1: Testing 1.4 All block should be free")

   block = _partition_alloc(part_id);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, block != NULL, "Test #1: Testing 1.5 Allocate one block from partition")

   free_block_cnt = _partition_get_free_blocks(part_id);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, free_block_cnt == (BLOCK_CNT - 1), "Test #1: Testing 1.6 Free blocks count should decrease")

   zero_block = _partition_alloc(part_id);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, zero_block != NULL, "Test #1: Testing 1.7 Allocate next block")

   free_block_cnt = _partition_get_free_blocks(part_id);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, free_block_cnt == (BLOCK_CNT - 2), "Test #1: Testing 1.8 Free blocks count should decrease")      
}
//----------------- Begin Testcases --------------------------------------

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test function:  _partition_create_component, _partition_create
                                          _partition_get_free_blocks, _partition_destroy
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   _mqx_uint   result, post_counter;
   _mem_size   pre_free_mem, post_free_mem;
   
   _GET_KERNEL_DATA(kernel_data);
   
   result = _partition_create_component();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.1 Creating PARTITION component") 
   
   pre_free_mem = get_free_mem();
   
   _task_create(0, TEST_TASK, 0);
   
   post_free_mem = get_free_mem();
   post_counter = _partition_get_free_blocks(part_id);

   EU_ASSERT_REF_TC_MSG(tc_1_main_task, pre_free_mem == (post_free_mem + system_owned), "Test #1: Testing 1.9 All blocks allocated in task are released") 
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, post_counter == BLOCK_CNT, "Test #1: Testing 1.9 All blocks allocated in task are released") 
      
   _task_create(0, CLEAN_TASK, 0);
   post_free_mem = get_free_mem();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, pre_free_mem == post_free_mem, "Test #1: Testing 1.9 Memory allocated in test_task is released") 
}

/******************** End Testcases ********************/

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_dealloc_part)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST1 - Testing partition deallocation after task exits"),
EU_TEST_SUITE_END(suite_dealloc_part)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_dealloc_part, " - Test of resource deallocation")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*--------------------------------------------------------------
* 
* Task Name : main_task
* Comments  :
*
*END*--------------------------------------------------------------*/

void main_task
   (
      uint32_t dummy
   )
{   
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
}/* Endbody */
/* EOF */
