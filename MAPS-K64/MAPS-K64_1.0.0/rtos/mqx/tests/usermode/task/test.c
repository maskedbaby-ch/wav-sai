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
* $Version : 4.0.2.0$
* $Date    : Jun-14-2013$
*
* Comments:
*
*   This file contains code for the MQX Usermode verification program,
*   includes all remaining test cases, which haven't been verified yet,
*   of Task functions in usermode.
*
* Requirements:
*
*   USERMODE057, USERMODE058, USERMODE060, USERMODE061, USERMODE062,
*   USERMODE064, USERMODE066, USERMODE067, USERMODE068, USERMODE069.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include "test.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

/*------------------------------------------------------------------------
 * Test suite function prototype
 */
void tc_1_main_task(void);  /* TEST # 1 - Testing function _usr_task_create in special cases */
void tc_2_main_task(void);  /* TEST # 2 - Testing function _usr_task_ready in special cases */
void tc_3_main_task(void);  /* TEST # 3 - Testing function _usr_task_get_td in special cases */
void tc_4_main_task(void);  /* TEST # 4 - Testing function _usr_task_abort in special cases */
void tc_5_main_task(void);  /* TEST # 5 - Testing function _usr_task_destroy in special cases */

/*------------------------------------------------------------------------
 * MQX task template
 */
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    { SUPER_TASK, super_task, 1500, 10, "super", MQX_AUTO_START_TASK },
    {  MAIN_TASK,  main_task, 3000,  8,  "main",       MQX_USER_TASK },
    {   SUB_TASK,   sub_task, 1500,  9,   "sub",       MQX_USER_TASK },
    {          0,          0,    0,  0,       0,                   0 }
};

/******************** Begin Test cases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*   This test case is to verify function _usr_task_create in the cases of:
*   - Creating task with invalid processor number.
*   - Creating task with invalid task number.
*   - Creating task while cannot allocate memory for task struct.
* Requirements:
*   USERMODE060, USERMODE061, USERMODE062.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    MEMORY_ALLOC_INFO_STRUCT memory_alloc_info;
    _task_id tid;

    /* Allocate all the remaining blocks of user memory pool, save the block pointers to memory_alloc_info */
    memory_alloc_all(&memory_alloc_info);
    /* Reset the task's error code to MQX_OK */
    _usr_task_set_error(MQX_OK);
    /* Try to create a task when the MQX cannot allocate memory by calling function _usr_task_create */
    tid = _usr_task_create(0, SUB_TASK, 0);
    /* Check the returned task ID must be MQX_NULL_TASK_ID to verify unsuccessfully creating */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, tid == MQX_NULL_TASK_ID, "TEST #1: Testing 1.01: Failed to create task when memory is insufficient");
    /* Check the task's error code must be MQX_OUT_OF_MEMORY to verify correct error code was set */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MQX_OUT_OF_MEMORY, "TEST #1: Testing 1.02: The task's error code must be MQX_OUT_OF_MEMORY ");
    /* Reset the task's error code to MQX_OK */
    _usr_task_set_error(MQX_OK);
    /* Free all the memory blocks whose pointers was stored in memory_alloc_info */
    memory_free_all(&memory_alloc_info);

    /* Try to create a task with invalid task index by calling function _usr_task_create */
    tid = _usr_task_create(0, INVALID_INDEX, 0);
    /* Check the returned task ID must be MQX_NULL_TASK_ID to verify unsuccessfully creating */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, tid == MQX_NULL_TASK_ID, "TEST #1: Testing 1.03: Failed to create a task with invalid task index");
    /* Check the task's error code must be MQX_NO_TASK_TEMPLATE to verify correct error code was set */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MQX_NO_TASK_TEMPLATE, "TEST #1: Testing 1.04: The task's error code must be MQX_NO_TASK_TEMPLATE ");
    /* Reset the task's error code to MQX_OK */
    _usr_task_set_error(MQX_OK);

    /* Try to create a task with invalid processor number by calling function _usr_task_create */
    tid = _usr_task_create(INVALID_PROC_NUM, SUB_TASK, 0);
    /* Check the returned task ID must be MQX_NULL_TASK_ID to verify unsuccessfully creating */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, tid == MQX_NULL_TASK_ID, "TEST #1: Testing 1.05: Failed to create a task with invalid processor number");
    /* Check the task's error code must be MQX_INVALID_PROCESSOR_NUMBER to verify correct error code was set */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MQX_INVALID_PROCESSOR_NUMBER, "TEST #1: Testing 1.06: The task's error code must be MQX_INVALID_PROCESSOR_NUMBER ");
    /* Reset the task's error code to MQX_OK */
    _usr_task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   This test case is to verify function _usr_task_ready in the cases of:
*   - Make a task ready properly.
*   - Make an invalid task ready.
*   - Make a task ready when it is already in ready queue.
* Requirements:
*   USERMODE057, USERMODE067, USERMODE068, USERMODE069.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    TD_STRUCT_PTR td_ptr;
    _task_id tid;
    _mqx_uint result;

    /* Try to set state of an invalid task (NULL desciptor) by calling function _usr_task_ready */
    _usr_task_ready(NULL);
    /* Check the task's error code must be MQX_INVALID_TASK_ID to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_INVALID_TASK_ID, "TEST #2: Testing 2.01: Failed to set state of an invalid task");
    /* Reset the task's error code to MQX_OK */
    _usr_task_set_error(MQX_OK);

    /* Create the SUB_TASK with lower priority by calling function _usr_task_create */
    tid = _usr_task_create(0, SUB_TASK, 0);
    /* Check the resturned task ID must be NOT NULL to verify successful creating */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, tid != MQX_NULL_TASK_ID, "TEST #2: Testing 2.02: Create a task in normal operation will be successful ");
    /* Because of low priority, the SUB_TASK is at ready state now */
    td_ptr = _usr_task_get_td(tid);

    /* Try to make the SUB_TASK ready when it is already in ready queue by calling function _usr_task_ready */
    _usr_task_ready(td_ptr);
    /* Check the task's error code must be MQX_INVALID_TASK_STATE to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_INVALID_TASK_STATE, "TEST #2: Testing 2.03: Failed to make the ready when it is already ready");
    /* Reset the task's error code to MQX_OK */
    _usr_task_set_error(MQX_OK);

    /* Suspend main task for a while by calling function _usr_time_delay_ticks */
    _usr_time_delay_ticks(10);

    /* Main task runs, it set SUB_TASK as ready state by calling function _usr_task_ready */
    _usr_task_ready(td_ptr);
    /* Check the state of SUB_TASK must be READY to verify successfully changing */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, td_ptr->STATE == READY, "TEST #2: Testing 2.04: Set SUB_TASK as ready");

    /* Abort SUB_TASK by calling function _usr_task_abort */
    result = _usr_task_abort(tid);
    /* Check the returned result must be MQX_OK to verify successfully aborting */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "TEST #2: Testing 2.05: Abort SUB_TASK");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   This test case is to verify function _usr_task_get_td in the case of:
*   - Get descripton of a task with invalid task ID.
* Requirements:
*   USERMODE066.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    TD_STRUCT_PTR td_ptr;

    /* Try to get descripton of a task with invalid task ID by calling function _usr_task_get_td */
    td_ptr = _usr_task_get_td(INVALID_TASK_ID);
    /* Check the returned result must be NULL to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, td_ptr == NULL, "TEST #3 Testing 3.1: Failed to get descripton of invalid task");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*   This test case is to verify function _usr_task_abort in the case of:
*   - Aborting a task with invalid task ID.
* Requirements:
*   USERMODE058.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    _mqx_uint result;

    /* Try to abort a task with invalid task ID by calling function _usr_task_abort */
    result = _usr_task_abort(INVALID_TASK_ID);
    /* Check the returned result must be MQX_NULL_TASK_ID to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_INVALID_TASK_ID, "TEST #4 Testing 4.1: Failed to abort invalid task");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5:
*   This test case is to verify function _usr_task_destroy in the case of:
*   - Destroy a task with invalid task ID.
* Requirements:
*   USERMODE064.
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    _mqx_uint result;

    /* Try to destroy a task with invalid task ID by calling function _usr_task_get_td */
    result = _usr_task_destroy(INVALID_TASK_ID);
    /* Check the returned result must be MQX_NULL_TASK_ID to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_INVALID_TASK_ID, "TEST #5 Testing 5.1: Failed to destroy invalid task");
}

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_usr_task)
    EU_TEST_CASE_ADD(tc_1_main_task, "TEST # 1 - Testing function _usr_task_create in special cases"),
    EU_TEST_CASE_ADD(tc_2_main_task, "TEST # 2 - Testing function _usr_task_ready in special cases"),
    EU_TEST_CASE_ADD(tc_3_main_task, "TEST # 3 - Testing function _usr_task_get_td in special cases"),
    EU_TEST_CASE_ADD(tc_4_main_task, "TEST # 4 - Testing function _usr_task_abort in special cases"),
    EU_TEST_CASE_ADD(tc_5_main_task, "TEST # 5 - Testing function _usr_task_destroy in special cases"),
EU_TEST_SUITE_END(suite_usr_task)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_usr_task, "MQX Usermode Component Test Suite, 5 Tests")
EU_TEST_REGISTRY_END()
/*------------------------------------------------------------------------*/

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   This is the main task with user access rights, this task creates
*   all defined test tasks and lets them run continuously.
*
*END*----------------------------------------------------------------------*/
void main_task(uint32_t param)
{
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : super_task
* Comments     :
*   This is the privilege task to the MQX Kernel program, this task creates
*   main task with user access rights.
*
*END*----------------------------------------------------------------------*/
void super_task(uint32_t param)
{
    _task_id tid;

    /* Install all unexpected interrupt */
    _int_install_unexpected_isr();

    /* Create the MAIN_TASK with user access rights and make it ready */
    tid = _task_create(0, MAIN_TASK, 0);
    /* Check if the MAIN_TASK was created */
    if(tid == MQX_NULL_TASK_ID)
    {
        printf("Unable to create test application");
        return;
    }
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : sub_task
* Comments     :
*   This task is created to test changing state of a task in usermode.
*
*END*----------------------------------------------------------------------*/
void sub_task(uint32_t param)
{
    /* SUB_TASK runs, it block itself immediately */
    _task_block();
}

