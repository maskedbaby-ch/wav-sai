/**HEADER*********************************************************************
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
* $Date    : Jun-1-2013$
*
* Comments   : This file contains the source functions for testing the accuracy
*              of the timer.
*
* Requirement:  TIME052,TIME053,TIME057,TIME058,TIME059,TIME060,TIME062,TIME004,
                TIME034,TIME056,TIME047,TIME070,TIME050,TIME024,TIME055,TIME071,
                TIME066,TIME051,TIME017,TIME018,TIME019,TIME020,TIME021,TIME022,
*
*END***********************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define MAIN_TASK                   10
#define DELAY_TASK                  11
#define NUM_SAMPLES                 128

#define DELAY_TICKS                 10

#define RESOLUTION_TOO_HIGH         (2 * MILLISECS_IN_SECOND)
#define RESOLUTION_NOT_DIVISIBLE    7

void    main_task(uint32_t);
void    delay_task(uint32_t);

volatile _mqx_uint  enabled_index;
volatile uint32_t   enabled_data[NUM_SAMPLES];
uint32_t            delay_task_run;
MQX_TICK_STRUCT     zero_tick_struct;


TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    { MAIN_TASK,  main_task,  1500, 9,  "Main",  MQX_AUTO_START_TASK,  0},
    { DELAY_TASK, delay_task, 1000, 8, "Delay",                    0,   },
    {         0,          0,    0,  0,       0,                    0,  0}
};

void tc_1_main_task(void);/* TEST #1: Testing functions _time_get_hwticks_per_tick and _time_set_hwticks_per_tick  */
void tc_2_main_task(void);/* TEST #2: Testing functions _time_get_resolution and _time_set_resolution */
void tc_3_main_task(void);/* TEST #3: Testing function _time_get_ticks_per_sec and _time_set_ticks_per_sec */
void tc_4_main_task(void);/* TEST #4: Testing function _time_get_ticks and _time_set_ticks   */
void tc_5_main_task(void);/* TEST #5: Testing function _time_get_hwticks */
void tc_6_main_task(void);/* TEST #6: Testing function _time_get_nanoseconds  */
void tc_7_main_task(void);/* TEST #7: Testing function _time_set_timer_vector and _time_get_timer_vector */
void tc_8_main_task(void);/* TEST #8: Testing function _time_notify_kernel  */
void tc_9_main_task(void);/* TEST #9: Testing function _time_dequeue */
void tc_10_main_task(void);/* TEST #10: Testing function _time_dequeue_td */

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Testing functions _time_get_hwticks_per_tick and
*                _time_set_hwticks_per_tick
*
* Requirement  : TIME052,TIME053
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint               hw_ticks_per_tick, result;
    KERNEL_DATA_STRUCT_PTR  kernel_data;
    MQX_TICK_STRUCT         ticks_1, ticks_2;

    _GET_KERNEL_DATA(kernel_data);

    hw_ticks_per_tick = kernel_data->HW_TICKS_PER_TICK;

    /* Calling function _time_get_hwticks_per_tick and verify the function return correct value */
    result = _time_get_hwticks_per_tick();
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == hw_ticks_per_tick, "Test #1 Testing 1.00: Test function _time_get_hwticks_per_tick operation");

    /* Initialize ticks_1 struct with TICKS field is zero and HW_TICKS field is hw_ticks_per_ticks */
    ticks_1.TICKS[0] = 0;
    ticks_1.TICKS[1] = 0;
    ticks_1.HW_TICKS = hw_ticks_per_tick;

    ticks_2 = ticks_1;

    /* Normalize ticks_1 before changing HW_TICKS_PER_TICK */
    PSP_NORMALIZE_TICKS(&ticks_1);

    /* Calling function _time_set_hwticks_per_tick to set new value of HW_TICKS_PER_TICKS */
    _time_set_hwticks_per_tick(hw_ticks_per_tick/2);

    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _time_get_hwticks_per_tick() == hw_ticks_per_tick/2, "Test #1 Testing 1.01: Test function _time_set_hwticks_per_tick operation");

    /* Normalize ticks_2 after changing HW_TICKS_PER_TICK */
    PSP_NORMALIZE_TICKS(&ticks_2);

    /* Verify ticks_2 must be greater than ticks_1 */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, PSP_CMP_TICKS(&ticks_2, &ticks_1) == 1, "Test #1 Testing 1.02: Test function _time_set_hwticks_per_tick operation");

    /* Recover HW_TICKS_PER_TICKS */
    _time_set_hwticks_per_tick(hw_ticks_per_tick);

    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _time_get_hwticks_per_tick() == hw_ticks_per_tick, "Test #1 Testing 1.03: Test function _time_get_hwticks_per_tick operation");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Testing functions _time_set_resolution
*                _time_get_resolution
*
* Requirement  : TIME057,TIME058,TIME034,TIME056
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    bool                 ovf;
    _mqx_uint               result, i;
    _mqx_uint               resolution, new_res;
    MQX_TICK_STRUCT         ticks_one, ticks;

    _time_init_ticks(&ticks_one, 1);

    resolution = _time_diff_milliseconds(&ticks_one, &zero_tick_struct, &ovf);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, resolution == BSP_ALARM_RESOLUTION, "Test #2 Testing 2.00: Test function _time_diff_milliseconds operation");

    result = _time_get_resolution();
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == resolution, "Test #2 Testing 2.01: Test function _time_get_resolution operation");

    /* Searching for new resolution which is divisible with MILLISECS_IN_SECOND */
    new_res = resolution + 1;
    new_res = new_res + MILLISECS_IN_SECOND % new_res;

    /* TODO: [ENGR00270026] CPU is halted after calling the _time_set_resolution function */
    /* Set to new resolution and verify the function _time_set_resolution should return MQX_OK */
    result = _time_set_resolution(new_res);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_OK, "Test #2 Testing 2.02: Test function _time_set_resolution operation");

    /* Call _time_get_resolution and verify the return value is correct */
    result = _time_get_resolution();
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == new_res, "Test #2 Testing 2.03: Test function _time_set_resolution operation");

    /* Verify the correct resolution make time functions work properly */
    ticks = ticks_one;
    for (i = 1; i < MILLISECS_IN_SECOND; i++)
    {
        result = _time_diff_milliseconds(&ticks, &zero_tick_struct, &ovf);
        PSP_INC_TICKS(&ticks);
        if (result != i * new_res) break;
    }
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, i == MILLISECS_IN_SECOND, "Test #2 Testing 2.04: Test function _time_set_resolution operation");

    /* Set to new resolution with the value is not divisible with 1000 */
    new_res = RESOLUTION_NOT_DIVISIBLE;
    result = _time_set_resolution(new_res);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_OK, "Test #2 Testing 2.05: Test function _time_set_resolution operation");

    /* Call _time_get_resolution and verify the return value is correct */
    result = _time_get_resolution();
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == new_res, "Test #2 Testing 2.06: Test function _time_set_resolution operation");

    /* Verify this indivisible resolution make time functions does not return matched value  */
    ticks = ticks_one;
    for (i = 1; i < MILLISECS_IN_SECOND; i++)
    {
        result = _time_diff_milliseconds(&ticks, &zero_tick_struct, &ovf);
        PSP_INC_TICKS(&ticks);
        if (result != i * new_res) break;
    }

    EU_ASSERT_REF_TC_MSG( tc_2_main_task, i != MILLISECS_IN_SECOND, "Test #2 Testing 2.07: Test function _time_set_resolution operation");

    /* Set resolution too high */
    new_res = RESOLUTION_TOO_HIGH;
    result = _time_set_resolution(new_res);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_INVALID_PARAMETER, "Test #2 Testing 2.08: Test function _time_set_resolution operation");

    /* Verify the resolution too high cause time functions does not return matched value */
    result = _time_diff_milliseconds(&ticks_one, &zero_tick_struct, &ovf);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result != new_res, "Test #2 Testing 2.09: Test function _time_set_resolution operation");

    /* Restore origin resolution */
    result = _time_set_resolution(resolution);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_OK, "Test #2 Testing 2.10: Test function _time_set_resolution operation");

    result = _time_diff_milliseconds(&ticks_one, &zero_tick_struct, &ovf);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == resolution, "Test #2 Testing 2.11: Test function _time_set_resolution operation");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Testing functions _time_get_ticks_per_sec and
*                _time_set_ticks_per_sec
*
* Requirement  : TIME059,TIME060,TIME062,TIME004,TIME034,TIME056
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    bool                 ovf;
    _mqx_uint               result, ticks_per_sec, new_value;
    MQX_TICK_STRUCT         ticks;

    _time_init_ticks(&ticks, 0);
    _time_add_sec_to_ticks(&ticks, 1);
    ticks_per_sec = PSP_GET_ELEMENT_FROM_TICK_STRUCT(&ticks, 0);

    /* Calling function _time_get_ticks_per_sec to get the current number of ticks per second and verify this value is correct */
    result = _time_get_ticks_per_sec();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == ticks_per_sec, "Test #3 Testing 3.00: Test function _time_set_ticks_per_sec operation");

    /* Searching for new value */
    new_value = ticks_per_sec + 1;
    new_value = new_value + MILLISECS_IN_SECOND % new_value;

    /* Calling function _time_set_ticks_per_sec to set new number of ticks per second and verify the function work properly */
    _time_set_ticks_per_sec(new_value);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, _time_get_ticks_per_sec() == new_value, "Test #3 Testing 3.01: Test function _time_set_ticks_per_sec operation");

    /* Verify the number of ticks per second is correct */
    _time_init_ticks(&ticks, 0);
    _time_add_sec_to_ticks(&ticks, 1);
    result = PSP_GET_ELEMENT_FROM_TICK_STRUCT(&ticks, 0);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == new_value, "Test #3 Testing 3.02: Test function _time_set_ticks_per_sec operation");

    result = _time_diff_milliseconds(&ticks, &zero_tick_struct, &ovf);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MILLISECS_IN_SECOND, "Test #3 Testing 3.03: Test function _time_set_ticks_per_sec operation");

    /* set number ticks per second to the value larger than 1000 */
    new_value = RESOLUTION_TOO_HIGH;
    _time_set_ticks_per_sec(new_value);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, _time_get_ticks_per_sec() == new_value, "Test #3 Testing 3.04: Test function _time_get_ticks_per_sec operation");

    /* Verify the too high number of ticks per second affect to some time function */
    result = _time_get_resolution();
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == 0, "Test #3 Testing 3.05: Test function _time_set_ticks_per_sec operation");

    /* Restore number of ticks per second to original value */
    _time_set_ticks_per_sec(ticks_per_sec);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _time_get_ticks_per_sec() == ticks_per_sec, "Test #3 Testing 3.06: Test function _time_set_ticks_per_sec operation");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Testing the following functions:
*               _time_set_ticks, _time_get_ticks
*
* Requirement  : TIME047,TIME070,TIME050,TIME024
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    MQX_TICK_STRUCT       start_ticks;
    MQX_TICK_STRUCT       end_ticks;
    MQX_TICK_STRUCT       diff_ticks;

    _time_delay_ticks(1);
    _int_disable();

    /* Obtain kernel time */
    _time_get_elapsed_ticks(&end_ticks);

    /* Move absolute MQX time forward */
    PSP_INC_TICKS(&end_ticks);
    _time_set_ticks(&end_ticks);

    /* Count diff between actual MQX and kernel time */
    _time_get_elapsed_ticks(&start_ticks);
    _time_get_ticks(&end_ticks);
    _time_diff_ticks(&end_ticks, &start_ticks, &diff_ticks);

    /* Diff should be 1 tick */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, PSP_GET_ELEMENT_FROM_TICK_STRUCT(&diff_ticks, 0) == 1, "Test #4 Testing 4.00: diff time between 1st get and 2nd get should be 1 tick");

    _int_enable();
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Testing function _time_get_hwticks
*
* Requirement  : TIME051
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    uint32_t result = 1;

    enabled_index  = 0;
    _time_delay_ticks(1); /* suspend the active task for the specified number of ticks */

    _mem_zero((void *)enabled_data, NUM_SAMPLES * sizeof(enabled_data[0]));

    while (enabled_index < NUM_SAMPLES)
    {
        /* Gets the calculated number hwticks since the last periodic timer interrupt. */
        enabled_data[enabled_index++] = (uint32_t)_time_get_hwticks();
        if(enabled_data[enabled_index] == enabled_data[enabled_index-1])
        {
            result = 0;
        }
    }   /* Endwhile */

    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result==1, "Test #5 Testing 5.00: Gets the calculated number of hwticks since the last periodic timer interrupt");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Testing function _time_get_nanoseconds
*
* Requirement  : TIME055
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    _mqx_uint result =1;

    enabled_index  = 0;
    _time_delay_ticks(1); /* suspend the active task for the specified number of ticks */

    _mem_zero((void *)enabled_data, NUM_SAMPLES * sizeof(enabled_data[0]));

    while (enabled_index < NUM_SAMPLES)
    {
        /* Gets the calculated number of nanoseconds, since the last periodic timer interrupt. */
        enabled_data[enabled_index++] = (_mqx_uint)_time_get_nanoseconds();
        if(enabled_data[enabled_index] == enabled_data[enabled_index-1])
        {
            result = 0;
        }
    }   /* Endwhile */

    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == 1, "Test #6 Testing 6.00: Gets the calculated number of nanoseconds since the last periodic timer interrupt");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7: Testing function _time_set_timer_vector
*
* Requirement  : TIME071
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    _mqx_uint  temp;
    KERNEL_DATA_STRUCT_PTR kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    /* Backups */
    temp = kernel_data->SYSTEM_CLOCK_INT_NUMBER;

    /* Calling function _time_set_timer_vector to set the periodic timer interrupt vector number */
    _time_set_timer_vector(temp + 1);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (kernel_data->SYSTEM_CLOCK_INT_NUMBER == (temp +1)), "Test #7 Testing 7.00: Testing function _time_set_timer_vector");

    /* Return */
    _time_set_timer_vector(temp);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (kernel_data->SYSTEM_CLOCK_INT_NUMBER == temp ), "Test #7 Testing 7.01: Testing function _time_set_timer_vector");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8: Testing function _time_notify_kernel
*
* Requirement  : TIME066,TIME051
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
    _mqx_uint               state_1, state_2;
    _mqx_uint               hw_ticks, max;
    KERNEL_DATA_STRUCT_PTR  kernel_data;
    MQX_TICK_STRUCT         ticks, ticks_1, ticks_2;
    _task_id                tid;
    TD_STRUCT_PTR           td_ptr;

    _GET_KERNEL_DATA(kernel_data);

    tid = _task_create(0, DELAY_TASK, 1000);

    td_ptr = _task_get_td(tid);

    /* disable interrupt to ensure _time_notify_kernel is not called */
    _int_disable();

    /* Get current ticks */
    ticks = kernel_data->TIME;

    /* Set the TIMEOUT of the delay_task to ticks + 1 */
    td_ptr->TIMEOUT = ticks;
    PSP_INC_TICKS(&td_ptr->TIMEOUT);

    /* This loop is to wait for one tick occurred */
    max = 0;
    do
    {
        hw_ticks = _time_get_hwticks();
        if (hw_ticks > max) max = hw_ticks;

    } while (hw_ticks > max);

    /* Get the current time in ticks_1 */
    ticks_1 = kernel_data->TIME;
    /* Get the current state of delay_task in state_1 */
    state_1 = td_ptr->STATE;

    /* Call _time_notify_kernel function */
    _time_notify_kernel();

    /* Get the current time in kernel to ticks_2 */
    ticks_2 = kernel_data->TIME;

    /* Get the current state of delay_task to state_2 */
    state_2 = td_ptr->STATE;

    /* We can enable interrupt now */
    _int_enable();

    /* Destroys delay_task */
    _task_destroy(tid);

    /* Check ticks_1 equals to ticks to verify the kernel time was not updated because _time_notify_kernel was not called */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, PSP_CMP_TICKS(&ticks_1, &ticks) == 0, "Test #8 Testing 8.00: Testing the function _time_notify_kernel");

    /* Check ticks_2 must be larger than ticks because kernel time is updated after _time_notify_kernel called */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, PSP_CMP_TICKS(&ticks_2, &ticks) > 0, "Test #8 Testing 8.01: Testing the function _time_notify_kernel");

    /* Check delay_task change from UNREADY state to READY state after _time_notify_kernel called */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (state_1 != READY) && (state_2 == READY), "Test #8 Testing 8.02: Testing the function _time_notify_kernel");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : TEST #9: Testing function _time_dequeue
*
* Requirement  : TIME017,TIME018,TIME019
*
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
    _task_id        taskb_id, taskc_id;
    TD_STRUCT_PTR   taskb_td;

    delay_task_run = 0;
    taskb_id = _task_create (0, DELAY_TASK, DELAY_TICKS);
    taskb_td = _task_get_td(taskb_id);

    /* Remove taskb_id from timeout queue */
    _time_dequeue(taskb_id);
    /* Suspend the current task to allow others task run */
    _time_delay_ticks(DELAY_TICKS);

    /* Verify the taskb_id is still not run */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (delay_task_run == 0), "Test #9 Testing: 9.00: Testing the _time_dequeue operation");
    /* Verify the taskb_id is not in ready state */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, taskb_td->STATE != READY, "Test #9 Testing: 9.01: Testing the _time_dequeue operation");
    /* Need to call task_ready to make taskb_td ready */
    _task_ready(taskb_td);

    /* Just suspend current task for 1 tick */
    _time_delay_ticks(1);
    /* Verify the taskb_id was run */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (delay_task_run > 0), "Test #9 Testing: 9.02: Testing the _time_dequeue operation");

    delay_task_run = 0;
    taskc_id = _task_create(0, DELAY_TASK, DELAY_TICKS);

    /* remove invalid task_id */
    _time_dequeue((_task_id)-1);

    /* Suspend the current task to allow others task run */
    _time_delay_ticks(DELAY_TICKS);
    /* Verify _time_dequeue with invalid task ID will do nothing */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (delay_task_run > 0), "Test #9 Testing: 9.03: Testing the _time_dequeue operation");

    _task_destroy(taskb_id);
    _task_destroy(taskc_id);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_10_main_task
* Comments     : TEST #10: Testing function _time_dequeue_td
*
* Requirement  : TIME020,TIME021,TIME022
*
*END*---------------------------------------------------------------------*/
void tc_10_main_task(void)
{
    _task_id        taskb_id, taskc_id;
    TD_STRUCT_PTR   taskb_td;

    delay_task_run = 0;
    taskb_id = _task_create (0, DELAY_TASK, DELAY_TICKS);
    taskb_td = _task_get_td(taskb_id);

    /* Remove taskb_id from timeout queue */
    _time_dequeue_td(taskb_td);
    /* Suspend the current task to allow others task run */
    _time_delay_ticks(DELAY_TICKS);

    /* Verify the taskb_id is still not run */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (delay_task_run == 0), "Test #10 Testing: 10.00: Testing _time_dequeue_td operation");
    /* Verify the taskb_id is not in ready state */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, taskb_td->STATE != READY, "Test #10 Testing: 10.01: Testing _time_dequeue_td operation");
    /* Need to call task_ready to make taskb_td ready */
    _task_ready(taskb_td);

    /* Just suspend current task for 1 tick */
    _time_delay_ticks(1);
    /* Verify the taskb_id was run */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (delay_task_run > 0), "Test #10 Testing: 10.02: Testing _time_dequeue_td operation");

    delay_task_run = 0;
    taskc_id = _task_create(0, DELAY_TASK, DELAY_TICKS);

    /* TODO:[ENGR00270086] The _time_dequeue_td function should has internal check NULL parameter */
    /* Call _time_dequeu_td with invalid task description pointer */
    _time_dequeue_td(NULL);

    /* Suspend the current task to allow others task run */
    _time_delay_ticks(DELAY_TICKS);
    /* Verify _time_dequeue_td with invalid task description will do nothing */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (delay_task_run > 0), "Test #10 Testing: 10.03: Testing _time_dequeue_td operation");

    _task_destroy(taskb_id);
    _task_destroy(taskc_id);
}

/*******  Add test cases */
 EU_TEST_SUITE_BEGIN(suite_getset2)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1: Testing functions _time_get_hwticks_per_tick and _time_set_hwticks_per_tick "),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2: Testing functions _time_get_resolution and _time_set_resolution"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST #3: Testing function _time_get_ticks_per_sec and _time_set_ticks_per_sec"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST #4: Testing function _time_get_ticks and _time_set_ticks "),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST #5: Testing function _time_get_hwticks "),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST #6: Testing function _time_get_nanoseconds "),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST #7: Testing function _time_set_timer_vector and _time_get_timer_vector "),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST #8: Testing function _time_notify_kernel"),
    EU_TEST_CASE_ADD(tc_9_main_task, " TEST #9: Testing function _time_dequeue"),
    EU_TEST_CASE_ADD(tc_10_main_task, " TEST #10: Testing function _time_dequeue_td"),
 EU_TEST_SUITE_END(suite_getset2)

/*******  Add test suites */
 EU_TEST_REGISTRY_BEGIN()
     EU_TEST_SUITE_ADD(suite_getset2, " Test of time module")
 EU_TEST_REGISTRY_END()

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   Just read and display the time.
*
*END*----------------------------------------------------------------------*/
 void main_task
    (
       uint32_t parameter
    )
 {
    _time_init_ticks(&zero_tick_struct, 0);

    _int_install_unexpected_isr();

    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
 } /* Endbody */

 /*TASK*-------------------------------------------------------------------
 *
 * Task Name    : main_task
 * Comments     : Just read and display the time.
 *
 *END*----------------------------------------------------------------------*/
 void delay_task
    (
       uint32_t param
    )
{
     _time_delay_ticks(param);

     delay_task_run++;

     _task_block();
}

/* EOF */
