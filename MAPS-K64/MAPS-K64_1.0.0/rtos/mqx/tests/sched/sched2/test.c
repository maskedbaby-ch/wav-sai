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
* $Version : 4.0.2$
* $Date    : Aug-30-2013$
*
* Comments:
*
*   This file contains code for MQX Scheduler component verification program,
*   includes all remaining test cases, which haven't been verified yet,
*   of following functions:
*   _sched_get_policy, _sched_get_rr_interval_ticks, _sched_get_rr_interval,
*   _sched_set_policy, _sched_set_rr_interval_ticks, _sched_set_rr_interval.
*
* Requirements:
*
*   SCHED004, SCHED006, SCHED008, SCHED009, SCHED011, SCHED012, SCHED014,
*   SCHED016, SCHED018.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include "test.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

/*------------------------------------------------------------------------
** Test suite function prototype
*/
void tc_1_main_task(void); /* TEST #1 - Testing function _sched_get_policy in special cases */
void tc_2_main_task(void); /* TEST #2 - Testing function _sched_set_policy in special cases */
void tc_3_main_task(void); /* TEST #3 - Testing function _sched_get_rr_interval_ticks in special cases */
void tc_4_main_task(void); /* TEST #4 - Testing function _sched_set_rr_interval_ticks in special cases */
void tc_5_main_task(void); /* TEST #5 - Testing function _sched_get_rr_interval in special cases */
void tc_6_main_task(void); /* TEST #6 - Testing function _sched_set_rr_interval in special cases */

/*------------------------------------------------------------------------
** MQX task template
*/
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    {  MAIN_TASK,  main_task, 3000,   9,  "main", MQX_AUTO_START_TASK },
    {  TEST_TASK,  test_task, 1000,   9,  "test",                   0 },
    {          0,          0,    0,   0,       0,                   0 }
};

/******************** Begin Test cases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*   This test case is to verify function _sched_get_policy in the case of:
*   - Getting scheduling policy from invalid task.
* Requirements:
*   SCHED004
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint current_policy;
    _mqx_uint result;

    /**********************************************************************************************
    **                                                                                           **
    **                    Failure: _sched_get_policy - Input task is invalid                     **
    **                                                                                           **
    **********************************************************************************************/

    /* Assign the value INVALID_POLICY to variable current_policy */
    current_policy = INVALID_POLICY;

    /* Try to get policy of invalid task by calling function _sched_get_policy with invalid input ID */
    result = _sched_get_policy(INVALID_TID, &current_policy);
    /* Check the result must be MQX_SCHED_INVALID_TASK_ID to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_SCHED_INVALID_TASK_ID, "TEST #1: Testing 1.01: Failed to get scheduling policy from invalid task");
    /* The returned policy is stored in current_policy, check it must be INVALID_POLICY to verify no value was returned */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, current_policy == INVALID_POLICY, "TEST #1: Testing 1.02: Verify the policy wasn't returned");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   This test case is to verify function _sched_set_policy in the cases of:
*   - Setting the policy for an invalid task
*   - Setting an invalid policy for current task and other task
* Requirements:
*   SCHED011, SCHED012
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _task_id test_tid;
    _mqx_uint result;

    /**********************************************************************************************
    **                                                                                           **
    **                    Failure: _sched_set_policy - Input task is invalid                     **
    **                                                                                           **
    **********************************************************************************************/

    /* Try to set policy MQX_SCHED_FIFO for invalid task by calling function _sched_set_policy with invalid input ID */
    result = _sched_set_policy(INVALID_TID, MQX_SCHED_FIFO);
    /* Check the result must be MAX_MQX_UINT to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MAX_MQX_UINT, "TEST #2: Testing 2.01: Failed to set scheduling policy to an invalid task");
    /* Check the task error code must be MQX_SCHED_INVALID_TASK_ID to verify correct error was set */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_SCHED_INVALID_TASK_ID, "TEST #2: Testing 2.02: Verify the task error code must be correct");
    /* Reset task error to MQX_OK */
    _task_set_error(MQX_OK);

    /* Try to set policy MQX_SCHED_RR for invalid task by calling function _sched_set_policy with invalid input ID */
    result = _sched_set_policy(INVALID_TID, MQX_SCHED_RR);
    /* Check the result must be MAX_MQX_UINT to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MAX_MQX_UINT, "TEST #2: Testing 2.03: Failed to set scheduling policy to an invalid task");
    /* Check the task error code must be MQX_SCHED_INVALID_TASK_ID to verify correct error was set */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_SCHED_INVALID_TASK_ID, "TEST #2: Testing 2.04: Verify the task error code must be correct");
    /* Reset task error to MQX_OK */
    _task_set_error(MQX_OK);

    /**********************************************************************************************
    **                                                                                           **
    **                   Failure: _sched_set_policy - Input policy is invalid                    **
    **                                      (Current task)                                       **
    **                                                                                           **
    **********************************************************************************************/

    /* Try to set policy to current task by calling function _sched_set_policy with invalid policy value */
    result = _sched_set_policy(MQX_NULL_TASK_ID, INVALID_POLICY);
    /* Check the result must be MAX_MQX_UINT to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MAX_MQX_UINT, "TEST #2: Testing 2.05: Failed to set invalid scheduling policy to current task");
    /* Check the task error code must be MQX_SCHED_INVALID_POLICY to verify correct error was set */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_SCHED_INVALID_POLICY, "TEST #2: Testing 2.06: Verify the task error code must be correct");
    /* Reset task error to MQX_OK */
    _task_set_error(MQX_OK);

    /**********************************************************************************************
    **                                                                                           **
    **                   Failure: _sched_set_policy - Input policy is invalid                    **
    **                                       (Other task)                                        **
    **                                                                                           **
    **********************************************************************************************/

    /* Create TEST_TASK by calling function _task_create */
    test_tid = _task_create(0, TEST_TASK, 0);
    /* Check the returned task id must be non-zero to verify successfully creating */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, test_tid != 0, "TEST #2: Testing 2.07: Create TEST_TASK");

    /* Try to set policy to TEST_TASK by calling function _sched_set_policy with invalid policy value */
    result = _sched_set_policy(test_tid, INVALID_POLICY);
    /* Check the result must be MAX_MQX_UINT to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MAX_MQX_UINT, "TEST #2: Testing 2.08: Failed to set invalid scheduling policy to TEST_TASK");
    /* Check the task error code must be MQX_SCHED_INVALID_POLICY to verify correct error was set */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_SCHED_INVALID_POLICY, "TEST #2: Testing 2.09: Verify the task error code must be correct");
    /* Reset task error to MQX_OK */
    _task_set_error(MQX_OK);

    /* Destroy TEST_TASK by calling function _task_destroy */
    result = _task_destroy(test_tid);
    /* Check the result must be MQX_OK to verify successfully destroying */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "TEST #2: Testing 2.10: Destroy TEST_TASK");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   This test case is to verify function _sched_get_rr_interval_ticks in the cases of:
*   - Getting time slice in ticks of an invalid task
*   - Getting time slice in ticks of current task and other task when input time is NULL
* Requirements:
*   SCHED008, SCHED009
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    MQX_TICK_STRUCT rr_interval;
    _task_id test_tid;
    _mqx_uint result;

    /**********************************************************************************************
    **                                                                                           **
    **                   Failure: _sched_get_rr_interval_ticks - Invalid task                    **
    **                                                                                           **
    **********************************************************************************************/

    /* Initilize the tick struct rr_interval with 0 tick */
    _time_init_ticks(&rr_interval, 0);

    /* Try to get time slice in ticks from invalid task by calling function _sched_get_rr_interval_ticks with invalid ID */
    result = _sched_get_rr_interval_ticks(INVALID_TID, &rr_interval);
    /* Check the result must be MQX_SCHED_INVALID_TASK_ID to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_SCHED_INVALID_TASK_ID, "TEST #3: Testing 3.01: Failed to get time slice from invalid task");
    /* Check the task error code must be MQX_SCHED_INVALID_TASK_ID to verify correct error was set */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_SCHED_INVALID_TASK_ID, "TEST #3: Testing 3.02: Verify the task error code must be correct");
    /* Reset task error to MQX_OK */
    _task_set_error(MQX_OK);

    /* Get the tick value from tick struct rr_interval */
    result = PSP_GET_ELEMENT_FROM_TICK_STRUCT(&rr_interval, 0);
    /* Check tick value must be 0 to verify the time slice value wasn't returned */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == 0, "TEST #3: Testing 3.03: Verify the time slice wasn't returned");

    /**********************************************************************************************
    **                                                                                           **
    **                 Failure: _sched_get_rr_interval_ticks - Invalid parameter                 **
    **                                      (Current task)                                       **
    **                                                                                           **
    **********************************************************************************************/

    /* Try to get time slice in ticks of current task by calling function _sched_get_rr_interval_ticks with invalid input time */
    result = _sched_get_rr_interval_ticks(MQX_NULL_TASK_ID, NULL);
    /* Check the result must be MQX_SCHED_INVALID_PARAMETER_PTR to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_SCHED_INVALID_PARAMETER_PTR, "TEST #3: Testing 3.04: Failed to get time slice from current task when input time pointer is NULL");
    /* Check the task error code must be MQX_SCHED_INVALID_PARAMETER_PTR to verify correct error was set */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_SCHED_INVALID_PARAMETER_PTR, "TEST #3: Testing 3.05: Verify the task error code must be correct");
    /* Reset task error to MQX_OK */
    _task_set_error(MQX_OK);

    /**********************************************************************************************
    **                                                                                           **
    **                 Failure: _sched_get_rr_interval_ticks - Invalid parameter                 **
    **                                       (Other task)                                        **
    **                                                                                           **
    **********************************************************************************************/

    /* Create TEST_TASK by calling function _task_create */
    test_tid = _task_create(0, TEST_TASK, 0);
    /* Check the returned task id must be non-zero to verify successfully creating */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, test_tid != 0, "TEST #3: Testing 3.06: Create TEST_TASK");

    /* Try to get time slice in ticks of TEST_TASK by calling function _sched_get_rr_interval_ticks with invalid input time */
    result = _sched_get_rr_interval_ticks(test_tid, NULL);
    /* Check the result must be MQX_SCHED_INVALID_PARAMETER_PTR to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_SCHED_INVALID_PARAMETER_PTR, "TEST #3: Testing 3.07: Failed to get time slice from TEST_TASK when input time pointer is NULL");
    /* Check the task error code must be MQX_SCHED_INVALID_PARAMETER_PTR to verify correct error was set */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_SCHED_INVALID_PARAMETER_PTR, "TEST #3: Testing 3.08: Verify the task error code must be correct");
    /* Reset task error to MQX_OK */
    _task_set_error(MQX_OK);

    /* Destroy TEST_TASK by calling function _task_destroy */
    result = _task_destroy(test_tid);
    /* Check the result must be MQX_OK to verify successfully destroying */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "TEST #3: Testing 3.09: Destroy TEST_TASK");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*   This test case is to verify function _sched_set_rr_interval_ticks in the case of:
*   - Setting time slice in ticks to an invalid task.
* Requirements:
*   SCHED016
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    MQX_TICK_STRUCT new_rr_interval;
    MQX_TICK_STRUCT old_rr_interval;
    _mqx_uint result;

    /**********************************************************************************************
    **                                                                                           **
    **               Failure: _sched_set_rr_interval_ticks - Input task in invalid               **
    **                                                                                           **
    **********************************************************************************************/

    /* Initilize the tick struct new_rr_interval with INTERVAL_TICKS tick */
    _time_init_ticks(&new_rr_interval, INTERVAL_TICKS);
    /* Initilize the tick struct old_rr_interval with 0 tick */
    _time_init_ticks(&old_rr_interval, 0);

    /* Try to set new time slice (new_rr_interval) to invalid task by calling function _sched_set_rr_interval_ticks with invalid ID */
    result = _sched_set_rr_interval_ticks(INVALID_TID, &new_rr_interval, &old_rr_interval);
    /* Check the result must be MQX_SCHED_INVALID_TASK_ID to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_SCHED_INVALID_TASK_ID, "TEST #4: Testing 4.01: Failed to set time slice of invalid task");

    /* The old time slice should be stored in old_rr_interval. Get the tick value from it */
    result = PSP_GET_ELEMENT_FROM_TICK_STRUCT(&old_rr_interval, 0);
    /* Check tick value must be 0 to verify the old time slice value wasn't returned */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == 0, "TEST #4: Testing 4.02: Verify the old time slice wasn't returned");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5:
*   This test case is to verify function _sched_get_rr_interval in the cases of:
*   - Getting time slice in ms of invalid task
*   - Getting time slice in ms of current task and other task when its value is invalid
* Requirements:
*   SCHED006
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    MQX_TICK_STRUCT new_rr_interval;
    MQX_TICK_STRUCT old_rr_interval;
    uint32_t rr_interval_ms;
    uint32_t res;
    _task_id test_tid;
    _mqx_uint result;

    /**********************************************************************************************
    **                                                                                           **
    **           Failure: _sched_get_rr_interval - Reach maximum size of uint_32 type            **
    **                                     (Invalid Task ID)                                     **
    **                                                                                           **
    **********************************************************************************************/

    /* Try to get time slice in ms of invalid task by calling function _sched_get_rr_interval with invalid input ID */
    res = _sched_get_rr_interval(INVALID_TID, &rr_interval_ms);
    /* Check the result must be MAX_UINT_32 to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, res == MAX_UINT_32, "TEST #5: Testing 5.01: Failed to get time slice of invalid task");
    /* Check the returned time slice must be MAX_UINT_32 to verify error value was returned */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, rr_interval_ms == MAX_UINT_32, "TEST #5: Testing 5.02: Verify the returned time slice is error value");

    /**********************************************************************************************
    **                                                                                           **
    **           Failure: _sched_get_rr_interval - Reach maximum size of uint_32 type            **
    **                           (Invalid time slice of current task)                            **
    **                                                                                           **
    **********************************************************************************************/

    /* Initilize the tick struct new_rr_interval with MAX_UINT_32 ticks */
    _time_init_ticks(&new_rr_interval, MAX_UINT_32);

    /* Set the new time slice (new_rr_interval) for current task by calling function _sched_set_rr_interval_ticks */
    result = _sched_set_rr_interval_ticks(MQX_NULL_TASK_ID, &new_rr_interval, &old_rr_interval);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "TEST #5: Testing 5.03: Set very big time slice to current task");

    /* Try to get time slice in ms of current task by calling function _sched_get_rr_interval when its value is very big */
    res = _sched_get_rr_interval(MQX_NULL_TASK_ID, &rr_interval_ms);
    /* Check the result must be MAX_UINT_32 to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, res == MAX_UINT_32, "TEST #5: Testing 5.04: Failed to get time slice when task has invalid time slice");
    /* Check the returned time slice must be MAX_UINT_32 to verify error value was returned */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, rr_interval_ms == MAX_UINT_32, "TEST #5: Testing 5.05: Verify the returned time slice is error value");

    /* Restore the time slice of current task to original value by calling function _sched_set_rr_interval_ticks */
    result = _sched_set_rr_interval_ticks(MQX_NULL_TASK_ID, &old_rr_interval, &new_rr_interval);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "TEST #5: Testing 5.06: Reset the time slice of current task");

    /**********************************************************************************************
    **                                                                                           **
    **           Failure: _sched_get_rr_interval - Reach maximum size of uint_32 type            **
    **                            (Invalid time slice of other task)                             **
    **                                                                                           **
    **********************************************************************************************/

    /* Create TEST_TASK by calling function _task_create */
    test_tid = _task_create(0, TEST_TASK, 0);
    /* Check the returned task id must be non-zero to verify successfully creating */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, test_tid != 0, "TEST #5: Testing 5.07: Create TEST_TASK");

    /* Initilize the tick struct new_rr_interval with MAX_UINT_32 ticks */
    _time_init_ticks(&new_rr_interval, MAX_UINT_32);

    /* Set the new time slice (new_rr_interval) for TEST_TASK by calling function _sched_set_rr_interval_ticks */
    result = _sched_set_rr_interval_ticks(test_tid, &new_rr_interval, &old_rr_interval);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "TEST #5: Testing 5.08: Set very big time slice to TEST_TASK");

    /* Try to get time slice in ms of TEST_TASK by calling function _sched_get_rr_interval when its value is very big */
    res = _sched_get_rr_interval(test_tid, &rr_interval_ms);
    /* Check the result must be MAX_UINT_32 to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, res == MAX_UINT_32, "TEST #5: Testing 5.09: Failed to get time slice when task has invalid time slice");
    /* Check the returned time slice must be MAX_UINT_32 to verify error value was returned */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, rr_interval_ms == MAX_UINT_32, "TEST #5: Testing 5.10: Verify the returned time slice is error value");

    /* Restore the time slice of TEST_TASK to original value by calling function _sched_set_rr_interval_ticks */
    result = _sched_set_rr_interval_ticks(test_tid, &old_rr_interval, &new_rr_interval);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "TEST #5: Testing 5.11: Reset the time slice of TEST_TASK");

    /* Destroy TEST_TASK by calling function _task_destroy */
    result = _task_destroy(test_tid);
    /* Check the result must be MQX_OK to verify successfully destroying */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "TEST #5: Testing 5.12: Destroy TEST_TASK");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6:
*   This test case is to verify function _sched_set_rr_interval in the cases of:
*   - Setting time slice in milliseconds to an invalid task.
*   - Setting invalid time slice value to current task and other task.
* Requirements:
*   SCHED014, SCHED018
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    _task_id test_tid;
    _mqx_uint result;

    /**********************************************************************************************
    **                                                                                           **
    **                  Failure: _sched_set_rr_interval - Input task in invalid                  **
    **                                                                                           **
    **********************************************************************************************/

    /* Try to set new time slice (new_rr_interval) to invalid task by calling function _sched_set_rr_interval_ticks with invalid ID */
    result = _sched_set_rr_interval(INVALID_TID, INTERVAL_TICKS);
    /* Check the result must be MAX_UINT_32 to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MAX_UINT_32, "TEST #6: Testing 6.01: Failed to set time slice of invalid task");
    /* Check the task error code must be MQX_SCHED_INVALID_TASK_ID to verify correct error was set */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == MQX_SCHED_INVALID_TASK_ID, "TEST #6: Testing 6.02: Verify the task error code must be correct");
    /* Reset task error to MQX_OK */
    _task_set_error(MQX_OK);

    /**********************************************************************************************
    **                                                                                           **
    **                Failure: _sched_set_rr_interval - Input interval is invalid                **
    **                           (Invalid time slice for current task)                           **
    **                                                                                           **
    **********************************************************************************************/

    /* Try to set invalid time slice to current task by calling function _sched_set_rr_interval_ticks input value */
    result = _sched_set_rr_interval(MQX_NULL_TASK_ID, 0);
    /* Check the result must be MAX_UINT_32 to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MAX_UINT_32, "TEST #6: Testing 6.03: Failed to set invalid time slice to current task");
    /* Check the task error code must be MQX_SCHED_INVALID_PARAMETER_PTR to verify correct error was set */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == MQX_SCHED_INVALID_PARAMETER_PTR, "TEST #6: Testing 6.04: Verify the task error code must be correct");
    /* Reset task error to MQX_OK */
    _task_set_error(MQX_OK);

    /**********************************************************************************************
    **                                                                                           **
    **                Failure: _sched_set_rr_interval - Input interval is invalid                **
    **                            (Invalid time slice for other task)                            **
    **                                                                                           **
    **********************************************************************************************/

    /* Create TEST_TASK by calling function _task_create */
    test_tid = _task_create(0, TEST_TASK, 0);
    /* Check the returned task id must be non-zero to verify successfully creating */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, test_tid != 0, "TEST #6: Testing 6.05: Create TEST_TASK");

    /* Try to set invalid time slice to TEST_TASK by calling function _sched_set_rr_interval_ticks input value */
    result = _sched_set_rr_interval(test_tid, 0);
    /* Check the result must be MAX_UINT_32 to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MAX_UINT_32, "TEST #6: Testing 6.06: Failed to set invalid time slice to TEST_TASK");
    /* Check the task error code must be MQX_SCHED_INVALID_PARAMETER_PTR to verify correct error was set */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == MQX_SCHED_INVALID_PARAMETER_PTR, "TEST #6: Testing 6.07: Verify the task error code must be correct");
    /* Reset task error to MQX_OK */
    _task_set_error(MQX_OK);

    /* Destroy TEST_TASK by calling function _task_destroy */
    result = _task_destroy(test_tid);
    /* Check the result must be MQX_OK to verify successfully destroying */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "TEST #6: Testing 6.08: Destroy TEST_TASK");
}

/*------------------------------------------------------------------------
** Define Test Suite
*/
EU_TEST_SUITE_BEGIN(suite_sched2)
    EU_TEST_CASE_ADD(tc_1_main_task, "TEST #1 - Testing function _sched_get_policy in special cases"),
    EU_TEST_CASE_ADD(tc_2_main_task, "TEST #2 - Testing function _sched_set_policy in special cases"),
    EU_TEST_CASE_ADD(tc_3_main_task, "TEST #3 - Testing function _sched_get_rr_interval_ticks in special cases"),
    EU_TEST_CASE_ADD(tc_4_main_task, "TEST #4 - Testing function _sched_set_rr_interval_ticks in special cases"),
    EU_TEST_CASE_ADD(tc_5_main_task, "TEST #5 - Testing function _sched_get_rr_interval in special cases"),
    EU_TEST_CASE_ADD(tc_6_main_task, "TEST #6 - Testing function _sched_set_rr_interval in special cases"),
EU_TEST_SUITE_END(suite_sched2)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_sched2, " - MQX Scheduler Component Test Suite, 6 Tests")
EU_TEST_REGISTRY_END()
/*------------------------------------------------------------------------*/

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   This task creates all defined test tasks and lets them run continuously.
*
*END*----------------------------------------------------------------------*/
void main_task(uint32_t param)
{
    /* Create test tasks */
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : test_task
* Comments     :
*   This task is created by MAIN_TASK to do nothing but test setting/getting
*   task's attributes.
*
*END*----------------------------------------------------------------------*/
void test_task(uint32_t param)
{
    _task_block();
}
