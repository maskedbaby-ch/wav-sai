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
* $Version : 3.7.3.0$
* $Date    : Mar-24-2011$
*
* Comments:
*
*   This file tests the light weight semaphore functions.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <util.h>
// #include <log.h>
// #include <klog.h>
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test

#define MAIN_TASK       10
#define LWSEM_TASK      11
#define WAIT_TASK       12

#define MAIN_PRIO       9

#define NUMBER_OF_TASKS 5

extern void       main_task(uint32_t parameter);
extern void       lwsem_task(uint32_t index);
extern void       wait_task(uint32_t index);
extern char   *_mqx_get_task_error_code_string(uint32_t);

//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);// Test #1 - destruction of uninitialized semaphore....
void tc_2_main_task(void);// Test #2 - create lwsem....
void tc_3_main_task(void);// Test #3 - wait on lwsem with IN_ISR == 1...
void tc_4_main_task(void);// Test #4 - wait on invalid lwsem
void tc_5_main_task(void);// Test #5 - tasks wait on lwsem.....
void tc_6_main_task(void);// Test #6 - post to an invalid semaphore.....
void tc_7_main_task(void);// Test #7 - lwsem testing error codes...
void tc_8_main_task(void);// Test #8 - test of lwsem normal operation....
void tc_9_main_task(void);// Test #9 - testing task priorities....
//------------------------------------------------------------------------

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK,   main_task,  1000,   MAIN_PRIO,     "main",     MQX_AUTO_START_TASK},
   { LWSEM_TASK,  lwsem_task, 1000,   MAIN_PRIO - 1, "lwsem",    0},
   { WAIT_TASK,   wait_task,  1000,   MAIN_PRIO - 1, "wait",     0},
   { 0,           0,          0,      0,             0,          0}
};

LWSEM_STRUCT lwsem;
volatile _mqx_uint order_of_exec, counter, count[NUMBER_OF_TASKS];
_mqx_uint   tc_index = 0;

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : lwsem_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void lwsem_task
   (
      uint32_t index
   )
{
    _mqx_uint  result;

    result = _lwsem_wait(&lwsem);
    if (tc_index == 8)
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: _lwsem_wait operation")
    else if (tc_index == 9)
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: _lwsem_wait operation")

    count[index] = order_of_exec++;

    result = _lwsem_post(&lwsem);
    if (tc_index == 8)
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: _lwsem_post operation")
    else if (tc_index == 9)
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: _lwsem_post operation")
}

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : wait_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void wait_task
   (
      uint32_t index
   )
{
    _mqx_uint result;

    while (1) {
        result = _lwsem_wait(&lwsem);
        if (result == MQX_INVALID_LWSEM) {
            break;
        } else {
            if (index == 0)
                counter++;
        } /* Endif */
    } /* Endwhile */
    counter++;
}


//----------------- Begin Testcases --------------------------------------
KERNEL_DATA_STRUCT_PTR  kernel_data;

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST #1: Destroy uninitialized semaphore, returns MQX_INVALID_LWSEM, 0x72
//
//END---------------------------------------------------------------------

void tc_1_main_task(void)
{
    _mqx_uint               result;

    _mem_zero(&lwsem, sizeof(lwsem));
    result = _lwsem_destroy(&lwsem);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_INVALID_LWSEM), "Test #1 Testing: _lwsem_destroy should have failed")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST #2: Create the lwsem, return code should be MQX_OK, 0
//
//END---------------------------------------------------------------------

void tc_2_main_task(void)
{
    _mqx_uint               result;

    result = _lwsem_create(&lwsem, 2);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: _lwsem_create operation")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST #3: wait on lwsem with IN_ISR == 1 ,
//               expected error, MQX CANNOT CALL FUNCTION FROM ISR, 0x0C
//
//END---------------------------------------------------------------------

void tc_3_main_task(void)
{
    _mqx_uint               result;
    _mqx_uint               in_isr_value;

    _int_disable();
    in_isr_value = get_in_isr();   
    set_in_isr(1);
    result = _lwsem_wait(&lwsem);
    set_in_isr(in_isr_value);
    _int_enable();

    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR), "Test #3 Testing: _lwsem_wait should have returned MQX_CANNOT_CALL_FUNCTION_FROM_ISR")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST #4: wait on invalid lwsem error: MQX_INVALID_LWSEM = 0x72
//
//END---------------------------------------------------------------------

void tc_4_main_task(void)
{
    _mqx_uint               result;
    _mqx_uint               valid_value;

    valid_value = lwsem.VALID;
    lwsem.VALID = 0;      
    result = _lwsem_wait(&lwsem);
    lwsem.VALID = valid_value;

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_INVALID_LWSEM), "Test #4 Testing: _lwsem_wait should have returned MQX_INVALID_LWSEM")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_5_main_task
// Comments     : TEST #5: Create 4 tasks waiting on the semapohre 
//              then destroy semaphore, tasks should detect destruction.
//
//END---------------------------------------------------------------------

void tc_5_main_task(void)
{
    _mqx_uint               result;
    _task_id                tid;

    counter = 0;
    tid = _task_create(0, WAIT_TASK, 0);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (tid != MQX_NULL_TASK_ID), "Test #5 Testing: 5.1.1: create instance of wait_task operation")

    tid = _task_create(0, WAIT_TASK, 1);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (tid != MQX_NULL_TASK_ID), "Test #5 Testing: 5.1.2: create instance of wait_task operation")

    tid = _task_create(0, WAIT_TASK, 2);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (tid != MQX_NULL_TASK_ID), "Test #5 Testing: 5.1.3: create instance of wait_task operation")

    tid = _task_create(0, WAIT_TASK, 3);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (tid != MQX_NULL_TASK_ID), "Test #5 Testing: 5.1.4: create instance of wait_task operation")

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (lwsem.VALUE == 0), "Test #5 Testing: 5.1: lwsem.VALUE should be 0")

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (counter == 2), "Test #5 Testing: 5.2: task 0 should have obtained sem twice")

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (lwsem.TD_QUEUE.SIZE == 4), "Test #5 Testing: 5.3: lwsem.TD_QUEUE.SIZE should be 4")

    counter = 0;
    /* Destroy the semaphore */
    result = _lwsem_destroy(&lwsem);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.4: _lwsem_destroy operation")
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (counter == 4), "Test #5 Testing: 5.5: all tasks were readied after sem destroyed")

    result = _lwsem_create(&lwsem, 1);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.6: _lwsem_create operation")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_6_main_task
// Comments     : TEST #6: post to an invalid semaphore, error: MQX_INVALID_LWSEM = 0x72
//
//END---------------------------------------------------------------------

void tc_6_main_task(void)
{
    _mqx_uint               result;
    _mqx_uint               valid_value;

    valid_value = lwsem.VALID;
    lwsem.VALID = 0;      
    result = _lwsem_post(&lwsem);
    lwsem.VALID = valid_value;
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_INVALID_LWSEM), "Test #5 Testing: _lwsem_post should have returned MQX_INVALID_LWSEM")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_7_main_task
// Comments     : TEST #7: lwsem test, error: MQX_CANNOT_CALL_FUNCTION_FROM_ISR = 0x0C
//
//END---------------------------------------------------------------------

void tc_7_main_task(void)
{
    _mqx_uint               result;
    void                   *lwsem_error, *td_error;
    _mqx_uint               in_isr_value, size;
    _mqx_uint               valid_value;

    lwsem_error = NULL;
    td_error = NULL;
    _int_disable();
    in_isr_value = get_in_isr();   
    set_in_isr(1);
    result = _lwsem_test(&lwsem_error, &td_error);
    set_in_isr(in_isr_value);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR), "Test #7 Testing: 7.1: _lwsem_test should have returned MQX_CANNOT_CALL_FUNCTION_FROM_ISR")

    /*
    ** lwsem test, error: MQX_INVALID_LWSEM: 0x72
    */
    lwsem_error = NULL;
    td_error = NULL;
    valid_value = lwsem.VALID;
    lwsem.VALID = 0;      
    result = _lwsem_test(&lwsem_error, &td_error);
    lwsem.VALID = valid_value;
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_INVALID_LWSEM), "Test #7 Testing: 7.2: _lwsem_test should have returned MQX_INVALID_LWSEM")
    if (result == MQX_INVALID_LWSEM) {
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (lwsem_error == &lwsem), "Test #7 Testing: 7.3: lwsem_error is correct")
    } /* Endif */

    /*
    ** lwsem test, error: MQX_CORRUPT_QUEUE: 0x70
    */
    lwsem_error = NULL;
    td_error = NULL;
    size = kernel_data->LWSEM.SIZE;
    kernel_data->LWSEM.SIZE = 99;
    result = _lwsem_test(&lwsem_error, &td_error);
    kernel_data->LWSEM.SIZE = size;
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_CORRUPT_QUEUE), "Test #7 Testing: 7.3: _lwsem_test should have returned MQX_CORRUPT_QUEUE")

    /*
    ** lwsem test, error: MQX_CORRUPT_QUEUE
    */
    lwsem_error = NULL;
    td_error = NULL;
    size = lwsem.TD_QUEUE.SIZE;
    lwsem.TD_QUEUE.SIZE = 99;
    result = _lwsem_test(&lwsem_error, &td_error);
    lwsem.TD_QUEUE.SIZE = size;
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_CORRUPT_QUEUE), "Test #7 Testing: 7.4: _lwsem_test should have returned MQX_CORRUPT_QUEUE")

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (lwsem_error == &lwsem), "Test #7 Testing: 7.5: lwsem_error is correct")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_8_main_task
// Comments     : TEST #8: Test of lwsem normal operation....
//
//END---------------------------------------------------------------------

void tc_8_main_task(void)
{
    _mqx_uint               result;
    _mqx_uint               i;
    _task_id                id[NUMBER_OF_TASKS];
    
    tc_index = 8;

   result = _lwsem_wait(&lwsem);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: 8.1: _lwsem_wait operation")

   order_of_exec = 0;
   for (i = 0; i < NUMBER_OF_TASKS; i++) {
      id[i] = _task_create(0, LWSEM_TASK, i);
      count[i] = 0;
   } /* Endfor */
   
   result = _lwsem_post(&lwsem);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: 8.2: _lwsem_post operation")
   
   for (i = 0; i < NUMBER_OF_TASKS; i++) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, (count[i] == i), "Test #8 Testing: 8.3: count operation")
   } /* Endfor */
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_9_main_task
// Comments     : TEST #9: Testing task priorities...
//
//END---------------------------------------------------------------------

void tc_9_main_task(void)
{
    _mqx_uint               result;
    _mqx_uint               old_prio, i;
    _task_id                id[NUMBER_OF_TASKS];

    tc_index = 9;

    result = _lwsem_wait(&lwsem);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: 9.1: _lwsem_wait operation")

    order_of_exec = 0;
    for (i = 0; i < NUMBER_OF_TASKS; i++) {
        id[i] = _task_create(0, LWSEM_TASK, i);
        _task_set_priority(id[i], MAIN_PRIO - i - 1, &old_prio);
        count[i] = 0;
    } /* Endfor */

    result = _lwsem_post(&lwsem);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: 9.3: _lwsem_post operation")

    for (i = 0; i < NUMBER_OF_TASKS; i++) {
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (count[i] == i), "Test #9 Testing: 9.5: count operation")
    } /* Endfor */
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - destruction of uninitialized semaphore...."),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - create lwsem...."),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - wait on lwsem with IN_ISR == 1..."),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - wait on invalid lwsem"),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - tasks wait on lwsem....."),
    EU_TEST_CASE_ADD(tc_6_main_task, " Test #6 - post to an invalid semaphore....."),
    EU_TEST_CASE_ADD(tc_7_main_task, " Test #7 - lwsem testing error codes..."),
    EU_TEST_CASE_ADD(tc_8_main_task, " Test #8 - test of lwsem normal operation...."),
    EU_TEST_CASE_ADD(tc_9_main_task, " Test #9 - testing task priorities....")
EU_TEST_SUITE_END(suite_1)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, " - Test of Lightweight semaphores")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : main_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t parameter
   )
{
    kernel_data = _mqx_get_kernel_data();

    _int_install_unexpected_isr();

   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* Endbody */

/* EOF */
