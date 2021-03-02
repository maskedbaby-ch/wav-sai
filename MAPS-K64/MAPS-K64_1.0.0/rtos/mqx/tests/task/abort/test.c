/**HEADER********************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
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
* $Version : 4.0.1.1$
* $Date    : Mar-27-2013$
*
* Comments:
*
*   This file contains code for _task_abort function
*   Requirement : TASK001, TASK002, TASK003, TASK004, TASK006, TASK019, TASK021
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <mqx_prv.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include "test.h"
#include "util.h"
#include <string.h>
#define FILE_BASENAME test

void tc_1_main_task(void); /* TEST #1 - Testing _task_abort */
void tc_2_main_task(void); /* TEST #2 - Testing a task aborts itself */

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { SUPER_TASK,  super_task, 3000,  8,  "super", MQX_AUTO_START_TASK},
   { MAIN_TASK,   main_task,  3000,  9,  "main",  MQX_USER_TASK},
   { TEST_TASK,   test_task,  3000,  9,  "test",  MQX_USER_TASK},
   { TEST_TASK2,  test_task2, 3000,  8,  "test2", MQX_USER_TASK},
   { TEST_TASK3,  test_task3, 3000,  10, "test3", MQX_USER_TASK},
   { 0,            0L,         0,     0,  0,       0}
};

USER_RO_ACCESS LWSEM_STRUCT lwsem;

void       *Old_ISR_data;
void        (_CODE_PTR_ Old_ISR)(void *);
_mqx_uint   glob_result, use_isr_tesing;
_task_id    glob_tid;
TD_STRUCT_PTR  td;
bool     is_task_running;
bool     is_task_aborted;
/******   Test cases body definition      *******/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test function: _task_abort
* Requirement  : TASK001, TASK003, TASK004, TASK006, TASK019, TASK021
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   _mqx_uint   result, i;
   _task_id    test_tid;
   _mem_size   pre_free_mem, post_free_mem;

   result = _task_abort(_mqx_get_system_task_id());
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result != MQX_OK), "Test #1 Testing 1.1: Attempt to abort SYSTEM TASK should fail")

   test_tid= _task_create(0, TEST_TASK, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (test_tid!= MQX_NULL_TASK_ID), "Test #1 Testing 1.2: Creating TEST TASK")
   result = _task_destroy(test_tid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing 1.3: Destroying TEST TASK")
   result = _task_abort(test_tid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result != MQX_OK), "Test #1 Testing: 1.4: Aborting destroyed task should fail")

   for(i = 0; i < TEST_COUNT; i++){
      pre_free_mem = get_free_mem();
      test_tid = _task_create(0, TEST_TASK2, 0);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (test_tid != MQX_NULL_TASK_ID), "Test #1 Testing: 1.5: Creating test task")
      result = _task_abort(test_tid);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.6: Aborting test task")
      result = _task_destroy(test_tid);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result != MQX_OK), "Test #1 Testing: 1.7: Destroying aborted task should fail")
      post_free_mem = get_free_mem();
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (post_free_mem == pre_free_mem), "Test #1 Testing: 1.8: Resources allocated by task should be released")
      if(pre_free_mem != post_free_mem) break;
   }

   result = _lwsem_create(&lwsem, 0);
   use_isr_tesing = 1;
   for(i = 0; i < TEST_COUNT; i++){
      glob_result = -1;
      glob_tid= _task_create(0, TEST_TASK, i%2);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (glob_tid!= MQX_NULL_TASK_ID), "Test #1 Testing 1.9: Creating TEST TASK")

      result = _lwsem_wait_ticks(&lwsem, 10);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK) , "Test #1 Testing 1.10: Waiting on interrupt")

      result = _task_destroy(glob_tid);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result != MQX_OK) , "Test #1 Testing 1.13: Task should be aborted and destroy should fail")
      if(glob_result != MQX_OK) break;
   }
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (glob_result == MQX_OK), "Test #1 Testing 1.14: All test competed succesfully")

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:  Testing a task aborts itself
* Requirement  : TASK002, TASK006
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _task_id      test_tid;
    _mqx_uint     result;

    test_tid = _task_create(0, TEST_TASK3, 0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, test_tid != MQX_NULL_TASK_ID , "Test #2: Testing 2.00: Create a task in normal operation will be successful ");

    is_task_running = FALSE;
    is_task_aborted = TRUE;
    /* suspend main task and allow TEST_TASK run */
    _time_delay_ticks(10);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (is_task_running)&&(is_task_aborted) , "Test #2: Testing 2.01: Aborting task that is calling function");
    /* Try to abort a task when it was aborted */
    result = _task_abort(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result != MQX_OK , "Test #2: Testing 2.02: Tests function _task_abort with the task was aborted ");

}


/*TASK*-------------------------------------------------------------------
*
* Task Name    : test_task
*
*END*----------------------------------------------------------------------*/

void test_task
   (
      uint32_t param
   )
{
   glob_result = 1;
   if(param)
      _task_block();
   else
      while(TRUE){};
}

void test_task2
   (
      uint32_t param
   )
{
   _task_block();
}


void test_task3
   (
       uint32_t param
   )
{
    is_task_running = TRUE;
    _task_abort(MQX_NULL_TASK_ID);    /* Test _task_abort function with argument is MQX_NULL_TASK_ID*/
    is_task_aborted = FALSE;          /* If come here, the above task_abort was failed   */
}

void test_ISR(void *isr_ptr)
{
   _mqx_uint   result;

   if(use_isr_tesing){
      if(glob_result == 1){
          result = _task_abort(glob_tid);
          EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK) , "Test #1 Testing 1.11: Aborting task from ISR")

          glob_result = MQX_OK;

          result = _lwsem_post(&lwsem);
          EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK) , "Test #1 Testing 1.12: Posting semaphore fom main task")
      }
   }
   (*Old_ISR)(Old_ISR_data);
}

//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_abort)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Testing _task_abort"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Testing a task aborts itself ")
 EU_TEST_SUITE_END(suite_abort)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_abort, " - Test task suite")
 EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*    This is the main task of the test program
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      /* [IN] The MQX task parameter */
      uint32_t param
   )
{ /* Body */
   td = _task_get_td(MQX_NULL_TASK_ID);
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* Endbody */

void super_task
   (
      uint32_t param
   )
{
   use_isr_tesing = 0;
   Old_ISR_data = _int_get_isr_data(BSP_TIMER_INTERRUPT_VECTOR);
   Old_ISR      = _int_get_isr(BSP_TIMER_INTERRUPT_VECTOR);
   _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, test_ISR, 0);

   _task_create(0, MAIN_TASK, 0);

   //_int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, Old_ISR, Old_ISR_data);
}
/* EOF */
