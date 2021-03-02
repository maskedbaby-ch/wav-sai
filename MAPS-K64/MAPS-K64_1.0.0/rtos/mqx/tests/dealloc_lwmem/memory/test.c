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
* $Version : 3.8.4.0$
* $Date    : Aug-28-2012$
*
* Comments:
*
*   This file contains the source for memory deallocation testing.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include "util.h"
#include "test.h"
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test
//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
void tc_4_main_task(void);
void tc_5_main_task(void);
void tc_6_main_task(void);
void tc_7_main_task(void);
void tc_8_main_task(void);
void tc_9_main_task(void);
void tc_10_main_task(void);
void tc_11_main_task(void);
void tc_12_main_task(void);
void tc_13_main_task(void);
void tc_14_main_task(void);
//------------------------------------------------------------------------
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
{ MAIN_TASK, main_task,  2000,   8, "Main_Task", MQX_AUTO_START_TASK},
{ TEST_TASK, test_task,  1000,   7, "Test_Task", 0},
{ RUN_DRY,   run_dry,    1000,   7, "Dry_Task",  0},
{ 0,         0,          0,      0, 0,           0}
};

MEMORY_ALLOC_INFO_STRUCT memory_alloc_info;

unsigned char   mem_pool[MEM_POOL_SIZE];
unsigned int    test_stack[TEST_STACK_SIZE/4];

_mem_pool_id    pool_id;

unsigned char   block_task;
unsigned char   alloc_system;
unsigned char   allocated;
//allocate memory from private memory pool
void test_task(uint32_t index)
{
   unsigned char   *array;

   array = _mem_alloc_from(pool_id, TEST_ALLOC_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, array != NULL, "Test #2: Testing 1.2: Allocate system in TEST_TASK")
   _task_block();

}

//allocate all available memory
void run_dry(uint32_t index)
{
   MEMORY_ALLOC_INFO_STRUCT   *memory;

   allocated = 0;

   /* initialize the memory alloc_info_struct */
   memory_alloc_info.block_ptr = NULL;

   if(block_task == 1)
      _task_block();

   do{
      if(alloc_system == 0)
         memory = _mem_alloc(TEST_ALLOC_SIZE);
      else {
         memory = _mem_alloc_system(TEST_ALLOC_SIZE);
         if(memory != NULL){
            memory->block_ptr = memory_alloc_info.block_ptr;
            memory_alloc_info.block_ptr = memory;
         }
      }
   }while(memory != NULL);

   _task_set_error(MQX_OK);

   if(memory == NULL)
      allocated = 1;

   if(block_task == 1)
      _task_block();

}
//----------------- Begin Testcases --------------------------------------
/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: _mem_create_pool, _mem_alloc, _mem_alloc_from, _mem_alloc_system
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   uint8_t     *memory;
   _task_id    tid;

   pool_id = _mem_create_pool(mem_pool, MEM_POOL_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, pool_id != NULL, "Test #1: Testing 1.1: Create memory pool")
   if(pool_id == NULL){
      return;
   }
   tid = _task_create(0, TEST_TASK, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, tid != MQX_NULL_TASK_ID, "Test #1: Testing 1.2: Test task created")
   if(tid == MQX_NULL_TASK_ID)
      return;

   memory = _mem_alloc_from(pool_id, TEST_ALLOC_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, memory == NULL, "Test #1: Testing 1.3: Memory should be still allocated")
   _task_set_error(MQX_OK);

   _task_destroy(tid);
   memory = _mem_alloc_from(pool_id, TEST_ALLOC_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, memory != NULL, "Test #1: Testing 1.4: Memory should be free")

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: _task_create; finishes; private memory
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
   _mem_size   free_mem_pre, free_mem_post;
   _task_id    tid;

   block_task = 0;
   alloc_system = 0;

   free_mem_pre = get_free_mem();
   tid = _task_create(0, RUN_DRY, 0);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, tid != MQX_NULL_TASK_ID, "Test #2: Testing 2.1: Test task created")
   if(tid == MQX_NULL_TASK_ID)
      return;

   free_mem_post = get_free_mem();
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, allocated, "Test #2: Testing 2.2: All memory was allocated")
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, free_mem_pre == free_mem_post, "Test #2: Testing 2.3: Private memory should be released")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: _task_create; finishes; system memory
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
   _mem_size   free_mem_pre, free_mem_post;
   _task_id    tid;

   block_task = 0;
   alloc_system = 1;

   free_mem_pre = get_free_mem();
   tid = _task_create(0, RUN_DRY, 0);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, tid != MQX_NULL_TASK_ID, "Test #3: Testing 3.1: Test task created")
   if(tid == MQX_NULL_TASK_ID)
      return;

   /* since we don't know how much memory was allocated by task creation and how much durring allocation
      let assume that all memory should be released after task finishes and system memory is deallocated */
   memory_free_all(&memory_alloc_info);
   free_mem_post = get_free_mem();
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, allocated, "Test #3: Testing 3.2: All memory was allocated")
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, free_mem_pre == free_mem_post, "Test #3: Testing 3.3: System memory should be released")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4: _task_create; destroy; private memory
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
   _mem_size   free_mem_begin, free_mem_pre, free_mem_post, task_cost;
   _task_id    tid;
   uint8_t     *memory;
   
   block_task = 1;
   alloc_system = 0;

   free_mem_pre = get_free_mem();
   free_mem_begin = free_mem_pre;
   tid = _task_create(0, RUN_DRY, 0);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, tid != MQX_NULL_TASK_ID, "Test #4: Testing 4.1: Test task created")
   if(tid == MQX_NULL_TASK_ID)
      return;

   free_mem_post = get_free_mem();
   task_cost = free_mem_pre - free_mem_post;

   _task_ready(_task_get_td(tid));
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, allocated, "Test #4: Testing 4.2: All memory was allocated")

   free_mem_pre = get_free_mem();
   memory = _mem_alloc_system(TEST_ALLOC_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, memory == NULL, "Test #4: Testing 4.3: Memory should be occupied")

   _task_destroy(tid);
   free_mem_post = get_free_mem();

   EU_ASSERT_REF_TC_MSG(tc_4_main_task, free_mem_post == free_mem_begin, "Test #4: Testing 4.4: All resources deallocated succesfully")

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5: _task_create; destroy; system memory
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
   _mem_size   free_mem_begin, free_mem_pre, free_mem_post, task_cost;
   _task_id    tid;
   uint8_t     *memory;
   
   block_task = 1;
   alloc_system = 1;

   free_mem_pre = get_free_mem();
   free_mem_begin = free_mem_pre;
   tid = _task_create(0, RUN_DRY, 0);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, tid != MQX_NULL_TASK_ID, "Test #5: Testing 5.1: Test task created")
   if(tid == MQX_NULL_TASK_ID)
      return;

   free_mem_post = get_free_mem();
   task_cost = free_mem_pre - free_mem_post;

   _task_ready(_task_get_td(tid));
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, allocated, "Test #5: Testing 5.2: All memory was allocated")

   free_mem_pre = get_free_mem();
   memory = _mem_alloc_system(TEST_ALLOC_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, memory == NULL, "Test #5: Testing 5.3: System memory should be occupied")

   _task_destroy(tid);
   free_mem_post = get_free_mem();

   EU_ASSERT_REF_TC_MSG(tc_5_main_task, free_mem_pre == (free_mem_post - task_cost), "Test #5: Testing 5.4: System memory should be occupied after task detroy")

   memory_free_all(&memory_alloc_info);
   free_mem_post = get_free_mem();

   EU_ASSERT_REF_TC_MSG(tc_5_main_task, free_mem_post == free_mem_begin, "Test #5: Testing 5.5: All resources deallocated succesfully")

}



/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6: _task_create_at; finishes; private memory
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
   _mem_size   free_mem_pre, free_mem_post;
   _task_id    tid;

   block_task = 0;
   alloc_system = 0;

   free_mem_pre = get_free_mem();
   tid = _task_create_at(0, RUN_DRY, 0, test_stack, TEST_STACK_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, tid != MQX_NULL_TASK_ID, "Test #6: Testing 6.1: Test task created (_task_create_at)")
   if(tid == MQX_NULL_TASK_ID)
      return;

   free_mem_post = get_free_mem();
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, allocated, "Test #6: Testing 6.2: All memory was allocated")
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, free_mem_pre == free_mem_post, "Test #6: Testing 6.3: Private memory should be released")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7: _task_create_at; finishes; system memory
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
   _mem_size   free_mem_pre, free_mem_post;
   _task_id    tid;

   block_task = 0;
   alloc_system = 1;

   free_mem_pre = get_free_mem();
   tid = _task_create_at(0, RUN_DRY, 0, test_stack, TEST_STACK_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, tid != MQX_NULL_TASK_ID, "Test #7: Testing 7.1: Test task created (_task_create_at)")
   if(tid == MQX_NULL_TASK_ID)
      return;

   /* since we don't know how much memory was allocated by task creation and how much durring allocation
      let assume that all memory should be released after task finishes and system memory is deallocated */
   memory_free_all(&memory_alloc_info);
   free_mem_post = get_free_mem();
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, allocated, "Test #7: Testing 7.2: All memory was allocated")
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, free_mem_pre == free_mem_post, "Test #7: Testing 7.3: System memory should be released")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8: _task_create_at; destroy; private memory
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
   _mem_size   free_mem_begin, free_mem_pre, free_mem_post, task_cost;
   _task_id    tid;
   uint8_t     *memory;
   
   block_task = 1;
   alloc_system = 0;

   free_mem_pre = get_free_mem();
   free_mem_begin = free_mem_pre;
   tid = _task_create_at(0, RUN_DRY, 0, test_stack, TEST_STACK_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, tid != MQX_NULL_TASK_ID, "Test #8: Testing 8.1: Test task created (_task_create_at)")
   if(tid == MQX_NULL_TASK_ID)
      return;

   free_mem_post = get_free_mem();
   task_cost = free_mem_pre - free_mem_post;

   _task_ready(_task_get_td(tid));
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, allocated, "Test #8: Testing 8.2: All memory was allocated")

   free_mem_pre = get_free_mem();
   memory = _mem_alloc_system(TEST_ALLOC_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, memory == NULL, "Test #8: Testing 8.3: Memory should be occupied")

   _task_destroy(tid);
   free_mem_post = get_free_mem();

   EU_ASSERT_REF_TC_MSG(tc_8_main_task, free_mem_post == free_mem_begin, "Test #8: Testing 8.4: All resources deallocated succesfully")

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : TEST #9: _task_create_at; destroy; system memory
*
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
   _mem_size   free_mem_begin, free_mem_pre, free_mem_post, task_cost;
   _task_id    tid;
   uint8_t     *memory;
   
   block_task = 1;
   alloc_system = 1;

   free_mem_pre = get_free_mem();
   free_mem_begin = free_mem_pre;
   tid = _task_create_at(0, RUN_DRY, 0, test_stack, TEST_STACK_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, tid != MQX_NULL_TASK_ID, "Test #9: Testing 9.1: Test task created (_task_create_at)")
   if(tid == MQX_NULL_TASK_ID)
      return;

   free_mem_post = get_free_mem();
   task_cost = free_mem_pre - free_mem_post;

   _task_ready(_task_get_td(tid));
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, allocated, "Test #9: Testing 9.2: All memory was allocated")

   free_mem_pre = get_free_mem();
   memory = _mem_alloc_system(TEST_ALLOC_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, memory == NULL, "Test #9: Testing 9.3: System memory should be occupied")

   _task_destroy(tid);
   free_mem_post = get_free_mem();

   EU_ASSERT_REF_TC_MSG(tc_9_main_task, free_mem_pre == (free_mem_post - task_cost), "Test #9: Testing 9.4: System memory should be occupied after task detroy")

   memory_free_all(&memory_alloc_info);
   free_mem_post = get_free_mem();

   EU_ASSERT_REF_TC_MSG(tc_9_main_task, free_mem_post == free_mem_begin, "Test #9: Testing 9.5: All resources deallocated succesfully")

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_10_main_task
* Comments     : TEST #10: _task_create (dynamically); finishes; private memory
*
*END*---------------------------------------------------------------------*/
void tc_10_main_task(void)
{
   _mem_size   free_mem_pre, free_mem_post;
   _task_id    tid;

   block_task = 0;
   alloc_system = 0;

   free_mem_pre = get_free_mem();
   tid = _task_create(0, 0, (uint32_t)&MQX_template_list[2]);
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, tid != MQX_NULL_TASK_ID, "Test #10: Testing 10.1: Test task created (_task_create_at)")
   if(tid == MQX_NULL_TASK_ID)
      return;

   free_mem_post = get_free_mem();
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, allocated, "Test #10: Testing 10.2: All memory was allocated")
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, free_mem_pre == free_mem_post, "Test #10: Testing 10.3: Private memory should be released")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_11_main_task
* Comments     : TEST #11: _task_create (dynamically); finishes; system memory
*
*END*---------------------------------------------------------------------*/
void tc_11_main_task(void)
{
   _mem_size   free_mem_pre, free_mem_post;
   _task_id    tid;

   block_task = 0;
   alloc_system = 1;

   free_mem_pre = get_free_mem();
   tid = _task_create(0, 0, (uint32_t)&MQX_template_list[2]);
   EU_ASSERT_REF_TC_MSG(tc_11_main_task, tid != MQX_NULL_TASK_ID, "Test #11: Testing 11.1: Test task created (dynamically)")
   if(tid == MQX_NULL_TASK_ID)
      return;

   /* since we don't know how much memory was allocated by task creation and how much durring allocation
      let assume that all memory should be released after task finishes and system memory is deallocated */
   memory_free_all(&memory_alloc_info);
   free_mem_post = get_free_mem();
   EU_ASSERT_REF_TC_MSG(tc_11_main_task, allocated, "Test #11: Testing 11.2: All memory was allocated")
   EU_ASSERT_REF_TC_MSG(tc_11_main_task, free_mem_pre == free_mem_post, "Test #11: Testing 11.3: System memory should be released")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_12_main_task
* Comments     : TEST #12: _task_create (dynamically); destroy; private memory
*
*END*---------------------------------------------------------------------*/
void tc_12_main_task(void)
{
   _mem_size   free_mem_begin, free_mem_pre, free_mem_post, task_cost;
   _task_id    tid;
   uint8_t     *memory;
   
   block_task = 1;
   alloc_system = 0;

   free_mem_pre = get_free_mem();
   free_mem_begin = free_mem_pre;
   tid = _task_create(0, 0, (uint32_t)&MQX_template_list[2]);
   EU_ASSERT_REF_TC_MSG(tc_12_main_task, tid != MQX_NULL_TASK_ID, "Test #12: Testing 12.1: Test task created (dynamically)")
   if(tid == MQX_NULL_TASK_ID)
      return;

   free_mem_post = get_free_mem();
   task_cost = free_mem_pre - free_mem_post;

   _task_ready(_task_get_td(tid));
   EU_ASSERT_REF_TC_MSG(tc_12_main_task, allocated, "Test #12: Testing 12.2: All memory was allocated")

   free_mem_pre = get_free_mem();
   memory = _mem_alloc_system(TEST_ALLOC_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_12_main_task, memory == NULL, "Test #12: Testing 12.3: Memory should be occupied")

   _task_destroy(tid);
   free_mem_post = get_free_mem();

   EU_ASSERT_REF_TC_MSG(tc_12_main_task, free_mem_post == free_mem_begin, "Test #12: Testing 12.4: All resources deallocated succesfully")

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_13_main_task
* Comments     : TEST #13: _task_create (dynamically); destroy; system memory
*
*END*---------------------------------------------------------------------*/
void tc_13_main_task(void)
{
   _mem_size   free_mem_begin, free_mem_pre, free_mem_post, task_cost;
   _task_id    tid;
   uint8_t     *memory;
   
   block_task = 1;
   alloc_system = 1;

   free_mem_pre = get_free_mem();
   free_mem_begin = free_mem_pre;
   tid = _task_create(0, 0, (uint32_t)&MQX_template_list[2]);
   EU_ASSERT_REF_TC_MSG(tc_13_main_task, tid != MQX_NULL_TASK_ID, "Test #13: Testing 13.1: Test task created (dynamically)")
   if(tid == MQX_NULL_TASK_ID)
      return;

   free_mem_post = get_free_mem();
   task_cost = free_mem_pre - free_mem_post;

   _task_ready(_task_get_td(tid));
   EU_ASSERT_REF_TC_MSG(tc_13_main_task, allocated, "Test #13: Testing 13.2: All memory was allocated")

   free_mem_pre = get_free_mem();
   memory = _mem_alloc_system(TEST_ALLOC_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_13_main_task, memory == NULL, "Test #13: Testing 13.3: System memory should be occupied")

   _task_destroy(tid);
   free_mem_post = get_free_mem();

   EU_ASSERT_REF_TC_MSG(tc_13_main_task, free_mem_pre == (free_mem_post - task_cost), "Test #13: Testing 13.4: System memory should be occupied after task detroy")

   memory_free_all(&memory_alloc_info);
   free_mem_post = get_free_mem();

   EU_ASSERT_REF_TC_MSG(tc_13_main_task, free_mem_post == free_mem_begin, "Test #13: Testing 13.5: All resources deallocated succesfully")

}

/******************** End Testcases ********************/

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_dealloc_memory)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST1 - Testing deallocation within private memory pool after task exits"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST2 - _task_create; finishes; private memory"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST3 - _task_create; finishes; system memory"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST4 - _task_create; destroyed; private memory"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST5 - _task_create; destroyed; system memory"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST6 - _task_create_at; finishes; private memory"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST7 - _task_create_at; finishes; system memory"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST8 - _task_create_at; destroyed; private memory"),
    EU_TEST_CASE_ADD(tc_9_main_task, " TEST9 - _task_create_at; destroyed; system memory"),
    EU_TEST_CASE_ADD(tc_10_main_task, " TEST10 - _task_create(dynamically); finishes; private memory"),
    EU_TEST_CASE_ADD(tc_11_main_task, " TEST11 - _task_create(dynamically); finishes; system memory"),
    EU_TEST_CASE_ADD(tc_12_main_task, " TEST12 - _task_create(dynamically); destroyed; private memory"),
    EU_TEST_CASE_ADD(tc_13_main_task, " TEST13 - _task_create(dynamically); destroyed; system memory"),
EU_TEST_SUITE_END(suite_dealloc_memory)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_dealloc_memory, " - Test of resource deallocation")
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
