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
* $Version : 3.8.2.0$
* $Date    : Apr-17-2012$
*
* Comments:
*
*   This file contains code for _task_restart function 
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <event.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include "test.h"
#include "util.h"
#define FILE_BASENAME test

void tc_1_main_task(void);
void tc_2_main_task(void);

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK,   main_task,  3000,  9,  "main",  MQX_AUTO_START_TASK | MQX_USER_TASK},
   { TEST_TASK,   test_task,  1000,  8,  "test",  MQX_USER_TASK},
   { TEST_TASK2,  test_task,  1000,  8,  "test",  0},
   { 0,           0L,         0,     0,  0,       0}
};

KERNEL_DATA_STRUCT_PTR  kernel_data;
TD_STRUCT   td_global;
_mqx_uint   glob_result;
/******   Test cases body definition      *******/

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test function: _task_create
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   _mqx_uint   result;
   _task_id    tid;
   
   _GET_KERNEL_DATA(kernel_data);
   
   tid = _task_create(0, TEST_TASK, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (tid != MQX_NULL_TASK_ID), "Test #1 Testing 1.1 crate priv->priv")
   result = _task_destroy(tid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing 1.2 Destroy priv->priv")
#if MQX_ENABLE_USER_MODE
   tid = _task_create(0, TEST_TASK2, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (tid == MQX_NULL_TASK_ID), "Test #1 Testing 1.3 create user->priv")
   _usr_task_set_error(MQX_OK);
#endif
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Test function: _task_create with 0 task index
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
   _mqx_uint      result;
   _task_id       tid, td_tid, stack_tid;
      
   _GET_KERNEL_DATA(kernel_data);
   
   tid = _task_create(0, 0, (uint32_t)&MQX_template_list[1]);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (tid != MQX_NULL_TASK_ID), "Test #2 Testing 2.1 crate test task dunamicaly")
   
   td_tid = get_td_owner(tid);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (td_tid == tid), "Test #2 Testing 2.2 Task descriptor is owned")
   stack_tid = get_stack_owner(tid);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (stack_tid == tid), "Test #2 Testing 2.3 Task stack is owned")
   result = _task_destroy(tid);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing 2.4 Destroy task")
   
   tid = _task_create(0, TEST_TASK, 0);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (tid != MQX_NULL_TASK_ID), "Test #2 Testing 2.5 crate test task staticaly")
   
   td_tid = get_td_owner(tid);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (td_tid == tid), "Test #2 Testing 2.6 Task descriptor is owned")
   stack_tid = get_stack_owner(tid);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (stack_tid == tid), "Test #2 Testing 2.7 Task stack is owned")
   result = _task_destroy(tid);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing 2.8 Destroy task")

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
   _task_block();
}


//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_task)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Testing _task_create"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Testing _task_create special")
 EU_TEST_SUITE_END(suite_task)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_task, " - Test task suite")
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
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* Endbody */


_task_id get_td_owner(_task_id tid)
{
   TD_STRUCT_PTR  td;
   
   td = _task_get_td(tid);
   return get_mem_owner(td);
}

_task_id get_stack_owner(_task_id tid)
{
   TD_STRUCT_PTR  td;
   
   td = _task_get_td(tid);  
   return get_mem_owner(td->STACK_ALLOC_BLOCK);
}

_task_id get_mem_owner(void *addr)
{
   _task_id                result = (_task_id)0;
   KERNEL_DATA_STRUCT_PTR  kernel_data;
   
#if MQX_USE_LWMEM_ALLOCATOR
   LWMEM_BLOCK_STRUCT_PTR  block_ptr;
   
   _GET_KERNEL_DATA(kernel_data);
   block_ptr = GET_LWMEMBLOCK_PTR(addr);
   result = BUILD_TASKID(kernel_data->INIT.PROCESSOR_NUMBER, block_ptr->U.S.TASK_NUMBER);
#else      
   STOREBLOCK_STRUCT_PTR   block_ptr;

   _GET_KERNEL_DATA(kernel_data);   
   block_ptr = GET_MEMBLOCK_PTR(addr);
   result = BUILD_TASKID(kernel_data->INIT.PROCESSOR_NUMBER, block_ptr->TASK_NUMBER);
#endif      
   
   return result;
}

/* EOF */
