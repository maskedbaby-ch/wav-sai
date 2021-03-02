/**HEADER********************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
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
* Comments   : This file contains the source for the task module testing.
* Requirement:
* TASK006,TASK019,TASK075,TASK076,TASK077,TASK079,TASK080,TASK081,TASK082,TASK085,
* TASK087,TASK088,TASK089,TASK091,TASK092,TASK093,TASK095,TASK096,TASK097,TASK098,
* TASK099,TASK101,TASK102,TASK103
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <mqx_prv.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

/*** Definitions and prototypes ***/
#define MAIN_TASK       1
#define TEST_TASK       2
#define MAIN_PRIO       7
#define TEST_PRIO       8

void main_task(uint32_t);
void test_task(uint32_t);
void new_tick_isr(void *);
void *test_taskq_in_isr(_mqx_uint *);
void invalidate_queue(void  **save_elem);
void restore_queue(void *save_elem);

/*** Test suite functions ***/
void tc_1_main_task(void);  /* TEST #1 - Test function _taskq_create with failure case      */
void tc_2_main_task(void);  /* TEST #2 - Test function _taskq_suspend in special cases      */
void tc_3_main_task(void);  /* TEST #3 - Test function _taskq_suspend_task in special cases */
void tc_4_main_task(void);  /* TEST #4 - Test function _taskq_destroy in special cases      */
void tc_5_main_task(void);  /* TEST #5 - Test function _taskq_resume in special cases       */
void tc_6_main_task(void);  /* TEST #6 - Test function _task_get_value in special case      */
void tc_7_main_task(void);  /* TEST #7 - Test function _taskq_test in special cases         */
/*** MQX task template ***/
TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/* Task number, Entry point, Stack,    Pri,       String, Auto? */
   {MAIN_TASK,   main_task,   1500,  MAIN_PRIO,   "main", MQX_AUTO_START_TASK},
   {TEST_TASK,   test_task,   1500,  TEST_PRIO,   "test",                   0},
   {        0,           0,      0,          0,        0,                   0}
};

typedef struct my_isr_struct
{
   void                 *OLD_ISR_DATA;
   void      (_CODE_PTR_ OLD_ISR)(void *);
   LWSEM_STRUCT          INT_EVENT;
   _mqx_uint             ERROR;
   void                 *RETURN_PTR;
} MY_ISR_STRUCT, * MY_ISR_STRUCT_PTR;


/*** Global variables ***/
bool                 is_task_running;    /* Check the state of task */
TASK_QUEUE_STRUCT_PTR   global_taskq_ptr;

/*TASK*--------------------------------------------------------------
*
* Task Name : test_task
* Comments  :
*
*END*--------------------------------------------------------------*/
void test_task
    (
        uint32_t test_no
    )
{
    is_task_running = TRUE;

    if (test_no == 3)
    {
        _mqx_int temp, result;

        _int_disable();
        temp = get_in_isr();
        set_in_isr(1);
        result = _taskq_suspend_task(_task_get_id(), global_taskq_ptr);
        set_in_isr(temp);
        _int_enable();

        EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR , "Test #3: Testing 3.02.1: Calling _taskq_suspend_task in ISR should return error MQX_CANNOT_CALL_FUNCTION_FROM_ISR");
        /* Resume the main task which has been suspended in taskq */
        _taskq_resume(global_taskq_ptr, FALSE);
    }
    _task_block();
} /* Endbody */

void new_tick_isr(void *ptr)
{
    MY_ISR_STRUCT_PTR isr_ptr = (MY_ISR_STRUCT_PTR)ptr;

    _task_set_error(MQX_OK);

    isr_ptr->RETURN_PTR = _taskq_create(MQX_TASK_QUEUE_BY_PRIORITY);
    /* Store task error code */
    isr_ptr->ERROR = _task_errno;

    _task_set_error(MQX_OK);

    /* signal the main task */
    _lwsem_post(&isr_ptr->INT_EVENT);

    /* Chain to the previous notifier */
    (*isr_ptr->OLD_ISR)(isr_ptr->OLD_ISR_DATA);
}

void *test_taskq_in_isr(_mqx_uint *error)
{
    MY_ISR_STRUCT_PTR      isr_ptr;
    void                   *return_ptr;

    isr_ptr = _mem_alloc_zero((_mem_size)sizeof(MY_ISR_STRUCT));

    isr_ptr->OLD_ISR = _int_get_isr(BSP_TIMER_INTERRUPT_VECTOR);
    isr_ptr->OLD_ISR_DATA = _int_get_isr_data(BSP_TIMER_INTERRUPT_VECTOR);
    _lwsem_create(&isr_ptr->INT_EVENT, 0);

    /* Install new isr for bsp timer */
    _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, new_tick_isr, isr_ptr);

    /* Waiting for bsp timer interrupt signal */
    *error = _lwsem_wait_ticks(&isr_ptr->INT_EVENT, 2);

    if (*error == MQX_OK)
    {
        /* get task error code and return pointer */
        *error = isr_ptr->ERROR;
        return_ptr = isr_ptr->RETURN_PTR;
    }

    /* Rstore bsp isr for bsp timer */
    _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, isr_ptr->OLD_ISR, isr_ptr->OLD_ISR_DATA);

    /* Free resources */
    _lwsem_destroy(&isr_ptr->INT_EVENT);
    _mem_free(isr_ptr);

    return return_ptr;
}

/******************** Begin Test cases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : This task is to test 2 cases of function _taskq_create:
*                MQX_OUT_OF_MEMORY, MQX_CANNOT_CALL_FUNCTION_FROM_ISR
* Requirement  : TASK076,TASK077
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    MEMORY_ALLOC_INFO_STRUCT    memory_alloc_info;
    TASK_QUEUE_STRUCT_PTR       taskq_ptr;
    _mqx_uint                   task_error;

    /* Allocate memory until system memory is full */
    memory_alloc_all(&memory_alloc_info);
    _task_set_error(MQX_OK);

    /* [_taskq_create] Create a task queue when system memory is full */
    taskq_ptr = _taskq_create(MQX_TASK_QUEUE_BY_PRIORITY);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, taskq_ptr == NULL , "Test #1: Testing 1.00: Failed to create a task queue when system memory is full");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MQX_OUT_OF_MEMORY , "Test #1: Testing 1.01: The task's error code should be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);
    /* Free the memory that was allocated before this case */
    memory_free_all(&memory_alloc_info);

    taskq_ptr = test_taskq_in_isr(&task_error);

    /* Verify the result of creating */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, taskq_ptr == NULL , "Test #1: Testing 1.02: Calling _taskq_create in ISR should be failed");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MQX_CANNOT_CALL_FUNCTION_FROM_ISR , "Test #1: Testing 1.03: The task's error code should be MQX_CANNOT_CALL_FUNCTION_FROM_ISR ");

}/* End tc_1_main_task */

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Test function  _taskq_suspend in special cases
* Requirement  : TASK075,TASK079,TASK091,TASK092,TASK093
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    KERNEL_DATA_STRUCT_PTR  kernel_data;
    TASK_QUEUE_STRUCT_PTR   taskq_ptr;
    _mqx_uint               result;
    _mqx_uint               temp;

    /* Get the current kernel data */
    kernel_data = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();

    /* Create a task queue */
    taskq_ptr = _taskq_create(MQX_TASK_QUEUE_BY_PRIORITY);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, taskq_ptr != NULL , "Test #2: Testing 2.00: Create a task queue");

    /* [_taskq_suspend] Call function _taskq_suspend from ISR */
    _int_disable();
    temp = kernel_data->IN_ISR;
    kernel_data->IN_ISR = 1;
    result = _taskq_suspend(taskq_ptr);
    kernel_data->IN_ISR = temp;
    _int_enable();
    /* Verify the returned result */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR , "Test #2: Testing 2.01: Calling _taskq_suspend in ISR should return error MQX_CANNOT_CALL_FUNCTION_FROM_ISR");

    /* [_taskq_suspend] Call function _taskq_suspend with invalid input parameter */
    result = _taskq_suspend(NULL);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_INVALID_PARAMETER , "Test #2: Testing 2.02: Calling _taskq_suspend with invalid input parameter should return error MQX_INVALID_PARAMETER");

    /* Make the created task queue to be an invalid task queue */
    temp = taskq_ptr->VALID;
    taskq_ptr->VALID = ~TASK_QUEUE_VALID;

    /* [_taskq_suspend] Call function _taskq_suspend with invalid task queue */
    result = _taskq_suspend(taskq_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_INVALID_TASK_QUEUE , "Test #2: Testing 2.03: Calling _taskq_suspend with invalid task queue should return MQX_INVALID_TASK_QUEUE");

    /* Recover the task queue value */
    taskq_ptr->VALID = temp;

    /* Destroy the created task queue */
    result = _taskq_destroy(taskq_ptr);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_OK , "Test #2: Testing 2.04: Destroy the created task queue");

}/* End tc_2_main_task */

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Test function _taskq_suspend_task in special cases
* Requirement  : TASK006,TASK019,TASK075,TASK079,TASK095,TASK096,TASK097,TASK098,TASK099
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    KERNEL_DATA_STRUCT_PTR  kernel_data;
    TASK_QUEUE_STRUCT_PTR   taskq_ptr;
    TD_STRUCT_PTR           td_ptr;
    _task_id                test_tid;
    _mqx_uint               result;
    _mqx_uint               temp;

    /* Get the current kernel data */
    kernel_data = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();

    /* Create a task queue */
    taskq_ptr = _taskq_create(MQX_TASK_QUEUE_BY_PRIORITY);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, taskq_ptr != NULL , "Test #3: Testing 3.00: Create a task queue ");

    /* Creating a test task with parameter is 3 */
    test_tid = _task_create(0, TEST_TASK, 3);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, test_tid != MQX_NULL_TASK_ID , "Test #3: Testing 3.01: Creating a task in the task queue");

    global_taskq_ptr = taskq_ptr;
    is_task_running = FALSE;
    /* suspend the main task to allow the test task run */
    result = _taskq_suspend_task(_task_get_id(), taskq_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, is_task_running && (result == MQX_OK) , "Test #3: Testing 3.02.2: Calling _taskq_suspend_task in normal operation");

    /* [_taskq_suspend_task] Call function _taskq_suspend with invalid input parameter */
    result = _taskq_suspend_task(test_tid, NULL);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_INVALID_PARAMETER , "Test #3: Testing 3.03: Calling _taskq_suspend_task with invalid input parameter should return error MQX_INVALID_PARAMETER");

    /* [_taskq_suspend_task] Suspend a task with invalid task ID */
    result = _taskq_suspend_task(-1, taskq_ptr);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_INVALID_TASK_ID , "Test #3: Testing 3.04: Suspend a task with invalid task ID, the error should be MQX_INVALID_TASK_ID");
    _task_set_error(MQX_OK);

    /* Make the created task queue to be an invalid task queue */
    temp = taskq_ptr->VALID;
    taskq_ptr->VALID = ~TASK_QUEUE_VALID;

    /* [_taskq_suspend_task] Call function _taskq_suspend_task with invalid task queue */
    result = _taskq_suspend_task(test_tid, taskq_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_INVALID_TASK_QUEUE , "Test #3: Testing 3.05: Calling _taskq_suspend_task with invalid task queue should return MQX_INVALID_TASK_QUEUE");

    /* Recover the task queue value */
    taskq_ptr->VALID = temp;

    /* Change state of the test task to BLOCKED */
    td_ptr = _task_get_td(test_tid);
    temp = td_ptr->STATE;
    td_ptr->STATE = BLOCKED;
    /* [_taskq_suspend_task] Suspend a task with BLOCKED state */
    result = _taskq_suspend_task(test_tid, taskq_ptr);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_INVALID_TASK_STATE , "Test #3: Testing 3.06: Suspend a task with BLOCKED state, the error should be MQX_INVALID_TASK_STATE");
    /* Restore state of the test task */
    td_ptr->STATE = temp;

    /* Destroy the created task */
    result = _task_destroy(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_OK , "Test #3: Testing 3.07: Destroys TEST_TASK that was create by main task ");

    /* Destroy the created task queue */
    result = _taskq_destroy(taskq_ptr);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_OK , "Test #3: Testing 3.08: Destroy the created task queue ");

}/* End tc_3_main_task */

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Test function _taskq_destroy in special cases
* Requirement  : TASK075,TASK079,TASK080,TASK081,TASK082
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    TASK_QUEUE_STRUCT_PTR       taskq_ptr;
    _mqx_uint                   result;
    _mqx_uint                   temp;

    /* Get the current kernel data */
    kernel_data = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();

    /* Create a task queue */
    taskq_ptr = _taskq_create(MQX_TASK_QUEUE_BY_PRIORITY);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, taskq_ptr != NULL , "Test #4: Testing 4.00: Create a task queue");

    /* [_taskq_destroy] Call function _taskq_destroy from ISR */
    _int_disable();
    temp = kernel_data->IN_ISR;
    kernel_data->IN_ISR = 1;
    result = _taskq_destroy(taskq_ptr);
    kernel_data->IN_ISR = temp;
    _int_enable();
    /* Verify the returned result */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR , "Test #4: Testing 4.01: Calling _taskq_destroy in ISR should return error MQX_CANNOT_CALL_FUNCTION_FROM_ISR");

    /* [_taskq_destroy] Call function _taskq_suspend with invalid input parameter */
    result = _taskq_destroy(NULL);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_INVALID_PARAMETER , "Test #4: Testing 4.02: Calling _taskq_destroy with invalid input parameter should return error MQX_INVALID_PARAMETER");

    /* Make the created task queue to be an invalid task queue */
    temp = taskq_ptr->VALID;
    taskq_ptr->VALID = ~TASK_QUEUE_VALID;

    /* [_taskq_destroy] Call function _taskq_destroy with invalid task queue */
    result = _taskq_destroy(taskq_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_INVALID_TASK_QUEUE , "Test #4: Testing 4.03: Calling _taskq_destroy with invalid task queue should return MQX_INVALID_TASK_QUEUE");

    /* Recover the task queue value */
    taskq_ptr->VALID = temp;

    /* Destroy the created task queue */
    result = _taskq_destroy(taskq_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK , "Test #4: Testing 4.04: Destroy the created task queue");

}/* End tc_4_main_task */

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Test function _taskq_resume in special cases
* Requirement  : TASK075,TASK079,TASK087,TASK088,TASK089
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    TASK_QUEUE_STRUCT_PTR       taskq_ptr;
    _mqx_uint                   result;
    _mqx_uint                   temp;

    /* [_taskq_resume] Call function _taskq_resume with invalid task queue (NULL) */
    result = _taskq_resume(NULL, TRUE);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_INVALID_TASK_QUEUE , "Test #5: Testing 5.00: Calling _taskq_resume with invalid task queue should return MQX_INVALID_TASK_QUEUE");

    /* Create a task queue */
    taskq_ptr = _taskq_create(MQX_TASK_QUEUE_BY_PRIORITY);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, taskq_ptr != NULL , "Test #5: Testing 5.01: Create a task queue");

    /* Make the created task queue to be an invalid task queue */
    temp = taskq_ptr->VALID;
    taskq_ptr->VALID = ~TASK_QUEUE_VALID;

    /* [_taskq_resume] Call function _taskq_resume with invalid task queue */
    result = _taskq_resume(taskq_ptr, TRUE);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_INVALID_TASK_QUEUE , "Test #5: Testing 5.02: Calling _taskq_resume with invalid task queue should return MQX_INVALID_TASK_QUEUE");

    /* Recover the task queue value */
    taskq_ptr->VALID = temp;

    /* [_taskq_resume] Resume task in an empty task queue */
    result = _taskq_resume(taskq_ptr, TRUE);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == MQX_TASK_QUEUE_EMPTY , "Test #5: Testing 5.03: Resume task in an empty task queue should return error MQX_TASK_QUEUE_EMPTY");

    /* Destroy the created task queue */
    result = _taskq_destroy(taskq_ptr);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK , "Test #5: Testing 5.04: Destroy the created task queue");

}/* End tc_5_main_task */

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Test function _task_get_value in special case
* Requirement  : TASK075,TASK079,TASK085
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    TASK_QUEUE_STRUCT_PTR       taskq_ptr;
    _mqx_uint                   result;
    _mqx_uint                   temp;

    /* Create a task queue */
    taskq_ptr = _taskq_create(MQX_TASK_QUEUE_BY_PRIORITY);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, taskq_ptr != NULL , "Test #6: Testing 6.00: Create a task queue");

    /* Make the created task queue to be an invalid task queue */
    temp = taskq_ptr->VALID;
    taskq_ptr->VALID = ~TASK_QUEUE_VALID;

    /* [_taskq_get_value] Call function _taskq_get_value with invalid task queue */
    result = _taskq_get_value(taskq_ptr);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == MQX_INVALID_TASK_QUEUE , "Test #6: Testing 6.01: Calling _taskq_get_value with invalid task queue should return MQX_INVALID_TASK_QUEUE");

    /* Recover the task queue value */
    taskq_ptr->VALID = temp;

    /* Destroy the created task queue */
    result = _taskq_destroy(taskq_ptr);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK , "Test #6: Testing 6.02: Destroy the created task queue");

}/* End tc_6_main_task */

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7: Test function _taskq_test in special cases
* Requirement  : TASK075,TASK079,TASK101,TASK102,TASK103
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    TASK_QUEUE_STRUCT_PTR       taskq_ptr;
    void                       *taskq_error_ptr, *task_error_ptr;
    void                       *save_elem;
    _mqx_uint                   result;
    _mqx_uint                   temp;

    /* Get the current kernel data */
    kernel_data = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();

    /* Create a task queue */
    taskq_ptr = _taskq_create(MQX_TASK_QUEUE_BY_PRIORITY);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, taskq_ptr != NULL , "Test #7: Testing 7.00: Create a task queue");

    /* [_taskq_test] Call function _taskq_test from ISR */
    _int_disable();
    temp = kernel_data->IN_ISR;
    kernel_data->IN_ISR = 1;
    result = _taskq_test(&taskq_error_ptr, &task_error_ptr);
    kernel_data->IN_ISR = temp;
    _int_enable();
    /* Verify the returned result */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR , "Test #7: Testing 7.01: Calling _taskq_test in ISR should return error MQX_CANNOT_CALL_FUNCTION_FROM_ISR");

    /* Make the created task queue to be an invalid task queue */
    temp = taskq_ptr->VALID;
    taskq_ptr->VALID = ~TASK_QUEUE_VALID;

    /* [_taskq_test] Call function _taskq_test with invalid task queue */
    result = _taskq_test(&taskq_error_ptr, &task_error_ptr);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_INVALID_TASK_QUEUE , "Test #7: Testing 7.02: Calling _taskq_test with invalid task queue should return MQX_INVALID_TASK_QUEUE");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, taskq_error_ptr == taskq_ptr , "Test #7: Testing 7.03: Verify the returned error task queue");

    /* Recover the task queue value */
    taskq_ptr->VALID = temp;

    invalidate_queue(&save_elem);

    result = _taskq_test(&taskq_error_ptr, &task_error_ptr);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_CORRUPT_QUEUE , "Test #7: Testing 7.04: Test function _taskq_test when a task in task queue is not valid ");
    _task_set_error(MQX_OK);

    restore_queue(save_elem);

    /* Destroy the created task queue */
    result = _taskq_destroy(taskq_ptr);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK , "Test #7: Testing 7.05: Destroy the created task queue");

}/* End tc_7_main_task */
/******************** End Testcases ********************/

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(task_taskq2)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Testing _taskq_create, _taskq_resume failure case"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Test function _taskq_suspend in special cases "),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST #3 - Test function _taskq_suspend_task in special cases "),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST #4 - Test function _taskq_destroy in special cases "),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST #5 - Test function _taskq_resume in special cases "),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST #6 - Test function _task_get_value in special case "),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST #7 - Test function _taskq_test in special cases "),
EU_TEST_SUITE_END(task_taskq2)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(task_taskq2, " - MQX Task Component Test Suite, 6 Tests")
EU_TEST_REGISTRY_END()
/*------------------------------------------------------------------------*/

/*TASK*--------------------------------------------------------------
*
* Task Name : main_task
* Comments  :
*   This is the main task to MQX Kernel program
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

/*FUNCTION*-----------------------------------------------------
*
* Function Name : invalidate_queue
* Comments      : Make the task list in the task queue to be corrupt
*
*END*-----------------------------------------------------*/
void invalidate_queue
    (
        void    **save_elem
    )
{
    QUEUE_ELEMENT_STRUCT_PTR element_ptr;
    KERNEL_DATA_STRUCT_PTR kernel_data;
    _GET_KERNEL_DATA(kernel_data);
    QUEUE_STRUCT_PTR q_ptr = (QUEUE_STRUCT_PTR)&kernel_data->KERNEL_TASK_QUEUES;

    _int_disable();
    element_ptr = q_ptr->NEXT;
    *save_elem = element_ptr->PREV;
    element_ptr->PREV = NULL;
    _int_enable();

}

/*FUNCTION*-----------------------------------------------------
*
* Function Name : restore_queue
* Comments      : Recover the task list in the task queue to original value
*
*END*-----------------------------------------------------*/
void restore_queue
    (
        void   *save_elem
    )
{
    QUEUE_ELEMENT_STRUCT_PTR element_ptr;
    KERNEL_DATA_STRUCT_PTR kernel_data;
    _GET_KERNEL_DATA(kernel_data);
    QUEUE_STRUCT_PTR q_ptr = (QUEUE_STRUCT_PTR)&kernel_data->KERNEL_TASK_QUEUES;

    _int_disable();
    element_ptr = q_ptr->NEXT;
    element_ptr->PREV = save_elem;
    _int_enable();
}
/* EOF */
