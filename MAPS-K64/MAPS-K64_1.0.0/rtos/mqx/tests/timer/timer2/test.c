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
* $Version : 4.0.1.0$
* $Date    : May-16-2013$
*
* Comments    : This file contains code for the MQX timer verification program.
* Requirements:
*               TIMER001,TIMER002,TIMER003,TIMER004,TIMER005,TIMER006,TIMER007,
*               TIMER008,TIMER010,TIMER011,TIMER012,TIMER014,TIMER015,TIMER016,
*               TIMER018,TIMER019,TIMER020,TIMER022,TIMER023,TIMER024,TIMER026,
*               TIMER027,TIMER028,TIMER030,TIMER031,TIMER032,TIMER034,TIMER035,
*               TIMER036,TIMER038,TIMER039,TIMER040,TIMER041.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <timer.h>
#include "timer_prv.h"
#include <log.h>
#include <klog.h>
#include "util.h"
#include "EUnit.h"
#include "timer_util.h"
#include "eunit_crit_secs.h"


#define MAIN_TASK     (11)
#define TICK_DIFF_MS  (3*1000/BSP_ALARM_FREQUENCY)

void main_task(uint32_t);
void capture_fcn(_timer_id, void *, uint32_t, uint32_t);
void increment_counter_ticks(_timer_id, void *, MQX_TICK_STRUCT_PTR);

/*----------------------------------------------------------------------*/
/* Test suite function prototype */
void tc_1_main_task();/* TEST # 1: Tests function _timer_create_component in special cases           */
void tc_2_main_task();/* TEST # 2: Tests function _timer_start_oneshot_after_ticks in special cases  */
void tc_3_main_task();/* TEST # 3: Tests function _timer_start_oneshot_at_ticks in special cases     */
void tc_4_main_task();/* TEST # 4: Tests function _timer_start_periodic_at_ticks in special cases    */
void tc_5_main_task();/* TEST # 5: Tests function _timer_start_periodic_every_ticks in special cases */
void tc_6_main_task();/* TEST # 6: Tests function _timer_start_oneshot_after in special cases        */
void tc_7_main_task();/* TEST # 7: Tests function _timer_start_oneshot_at in special cases           */
void tc_8_main_task();/* TEST # 8: Tests function _timer_start_periodic_at in special cases          */
void tc_9_main_task();/* TEST # 9: Tests function _timer_start_periodic_at in special cases          */
void tc_10_main_task();/* TEST # 10: Tests function _timer_cancel in special cases                   */
void tc_11_main_task();/* TEST # 11: Tests function _timer_test in special cases                     */
/*----------------------------------------------------------------------*/

void    *main_td;
volatile uint32_t count  = 0;

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK,  main_task, 2000,   9,   "Main",  MQX_AUTO_START_TASK, 0},
   {         0,          0,    0,   0,        0,                    0, 0}
};

/******************** Begin Testcases **********************/
/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : Testing function _timer_create_component
* Requirements :
*                TIMER005,TIMER006.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint                 temp;
    _mqx_uint                 result;
    KERNEL_DATA_STRUCT_PTR    kernel_data;
    MEMORY_ALLOC_INFO_STRUCT  memory_alloc_info;

    /* Get the current kernel data */
    _GET_KERNEL_DATA(kernel_data);

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);

    /* Creates timer component when the memory is insufficient */
    result = _timer_create_component(0,0);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == MQX_OUT_OF_MEMORY, "Test #1 Testing 1.00: Test function _timer_create_component with memory is insufficient");
    _task_set_error(MQX_OK);

    /* Frees all of memory that was temporary allocated */
    memory_free_all(&memory_alloc_info);

    /* Save the current isr counting value */
    temp = get_in_isr();
    /* Set the current task as an isr */
    set_in_isr(1);
    /* [_timer_create_component] Create a timer component in ISR */
    result = _timer_create_component(0,0);
    /* Restore the isr counting value */
    set_in_isr(temp);

    /* Verify the result of creating */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #1 Testing 1.01: Calling _timer_create_component in ISR should be failed");
    _task_set_error(MQX_OK);

    /* Creates timer component with all of parameters are valid */
    result = _timer_create_component(TIMER_DEFAULT_TASK_PRIORITY,
        TIMER_DEFAULT_STACK_SIZE);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == MQX_OK, "Test #1 Testing 1.02: Create the timer component");
    /* Checks the timer component was created */
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER] != NULL, "Test #1 Testing 1.03: Checking the timer component was created");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : Testing function _timer_start_oneshot_after_ticks in special cases.
* Requirements :
*                TIMER001,TIMER011,TIMER012,TIMER014.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _timer_id                 id;
    _mqx_uint                 temp;
    MQX_TICK_STRUCT           ticks;
    MEMORY_ALLOC_INFO_STRUCT  memory_alloc_info;

    /* Set the alarm */
    _time_init_ticks(&ticks, 2);

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);

    /* Test function _timer_start_oneshot_after_ticks with memory is insufficient */
    id = _timer_start_oneshot_after_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, id == TIMER_NULL_ID, "Test #2 Testing 2.00: Starts timer with memory is insufficient");
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #2 Testing 2.01: The task's error code should be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

    /* Frees all of memory that was temporary allocated */
    memory_free_all(&memory_alloc_info);

    /* The timer components was previously created */

    /* Backups the value of VALID field */
    temp = get_timer_component_valid_field();
    /* Makes the timer component to invalid */
    set_timer_component_valid_field(temp + 1);

    /* Test function _timer_start_oneshot_after_ticks with timer component is not valid */
    id = _timer_start_oneshot_after_ticks(increment_counter_ticks, (void *)&count,
            TIMER_ELAPSED_TIME_MODE, &ticks);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, id == TIMER_NULL_ID, "Test #2 Testing 2.02: Start timer with timer component is not valid");
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, _task_errno == MQX_INVALID_COMPONENT_BASE, "Test #2 Testing 2.03: The task's error code should be MQX_INVALID_COMPONENT_BASE");
    _task_set_error(MQX_OK);

    /* Restore the value of VALID field to original value */
    set_timer_component_valid_field(temp);

    /* Reset global counter */
    count = 0;

    /* Start timer with all of parameter are valid */
    id = _timer_start_oneshot_after_ticks(increment_counter_ticks, (void *)&count,
        TIMER_KERNEL_TIME_MODE, &ticks);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, id != TIMER_NULL_ID, "Test #2 Testing 2.04: Start a timer that expires the mumber of ticks.");

    /* Wait for the timer to fire */
    _time_delay_ticks(4);
    /* The function increment_counter_ticks was called when the timer expires */
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, count == 1, "Test #2 Testing 2.05: Timer_start_oneshot_after count should be 1");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : Testing function _timer_start_oneshot_at_ticks in special cases.
* Requirements :
*                TMER019,TIMER020,TIMER022.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    _timer_id                 id;
    _mqx_uint                 temp;
    MQX_TICK_STRUCT           ticks;
    MEMORY_ALLOC_INFO_STRUCT  memory_alloc_info;

    /* Set the alarm */
    _time_init_ticks(&ticks, 2);

    /* The timer components was previously created */

    /* Backups the value of VALID field */
    temp = get_timer_component_valid_field();
    /* Makes the timer component to invalid */
    set_timer_component_valid_field(temp + 1);

    /* Tests function _timer_start_oneshot_at_ticks when the timer component is not valid */
    id = _timer_start_oneshot_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, id == TIMER_NULL_ID, "Test #3 Testing 3.00: Tests function _timer_start_oneshot_at_ticks when the timer component is not valid");
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, _task_errno == MQX_INVALID_COMPONENT_BASE, "Test #3 Testing 3.01: The task's error code should be MQX_INVALID_COMPONENT_BASE");
    _task_set_error(MQX_OK);

    /* Restore the value of VALID field to original value */
    set_timer_component_valid_field(temp);

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);

    /* Tests function _timer_start_oneshot_at_ticks when memory is insufficient */
    id = _timer_start_oneshot_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, id == TIMER_NULL_ID, "Test #3 Testing 3.02: Tests function _timer_start_oneshot_at_ticks when memory is insufficient");
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #3 Testing 3.03: The task's error code should be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

    /* Frees all of memory that was temporary allocated */
    memory_free_all(&memory_alloc_info);

    /* Set the alarm */
    _time_get_elapsed_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 4, &ticks);
    /* reset global counter. */
    count = 0;

    /* Start timer*/
    id = _timer_start_oneshot_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, id != TIMER_NULL_ID, "Test #3 Testing 3.04: Timer_start_oneshot_at");

    /* wait for the timer to fire */
    _time_delay_ticks(6);
    /* The function increment_counter_ticks was called when the timer expires */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, count == 1, "Test #3 Testing 3.05: Timer_start_oneshot_at count should be 1.");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : Testing function _timer_start_periodic_at_ticks in special cases.
* Requirements :
*                TIMER001,TIMER027,TIMER028,TIMER030.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    _timer_id                 id;
    _mqx_uint                 temp;
    _mqx_uint                 result;
    MQX_TICK_STRUCT           ticks;
    MQX_TICK_STRUCT           ticks2;
    MEMORY_ALLOC_INFO_STRUCT  memory_alloc_info;

    /* Set the alarm */
    _time_init_ticks(&ticks2, 10);

    /* The timer components was previously created */

    /* Backups the value of VALID field */
    temp = get_timer_component_valid_field();
    /* Makes the timer component to invalid */
    set_timer_component_valid_field(temp + 1);

    /* Tests function _timer_start_periodic_at_ticks when the timer component is not valid */
    id = _timer_start_periodic_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks, &ticks2);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, id == TIMER_NULL_ID, "Test #4 Testing 4.00: Tests function _timer_start_periodic_at_ticks when the timer component is not valid");
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, _task_errno == MQX_INVALID_COMPONENT_BASE, "Test #4 Testing 4.01: The task's error code should be MQX_INVALID_COMPONENT_BASE");
    _task_set_error(MQX_OK);

    /* Restore the value of VALID field to original value */
    set_timer_component_valid_field(temp);

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);

    /* Tests function _timer_start_periodic_at_ticks when memory is insufficient */
    id = _timer_start_periodic_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks, &ticks2);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, id == TIMER_NULL_ID, "Test #4 Testing 4.02: Tests function _timer_start_periodic_at_ticks when memory is insufficient");
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #4 Testing 4.03: The task's error code should be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

    /* Frees all of memory that was temporary allocated */
    memory_free_all(&memory_alloc_info);

    /* set the alarm */
    _time_get_elapsed_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 10, &ticks);
    /* reset global counter. */
    count = 0;

    /* Tests function _timer_start_periodic_at_ticks operation */
    id = _timer_start_periodic_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks, &ticks2);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, id != TIMER_NULL_ID, "Test #4 Testing 4.04: Start a valid timer using TIMER_ELAPSED_TIME_MODE to fire every 20 milliseconds.");

    /* Wait for the timer to fire */
    _time_delay_ticks(50);
    /* The function increment_counter_ticks was called when the timer expires */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, count == 5, "Test #4 Testing 4.05: Timer_start_oneshot_at count should be 1.");

    /* Cancel timer */
    result = _timer_cancel(id);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_OK, "Test #4 Testing 4.06: Tests function _timer_cancel operation");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : Testing function _timer_start_periodic_every_ticks in special cases.
* Requirements :
*                TIMER001,TIMER035,TIMER036,TIMER038.
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    _timer_id                 id;
    _mqx_uint                 temp;
    _mqx_uint                 result;
    MQX_TICK_STRUCT           ticks2;
    MEMORY_ALLOC_INFO_STRUCT  memory_alloc_info;

    /* Set the alarm */
    _time_init_ticks(&ticks2, 4);

    /* The timer components was previously created */

    /* Backups the value of VALID field */
    temp = get_timer_component_valid_field();
    /* Makes the timer component to invalid */
    set_timer_component_valid_field(temp + 1);

    /* Tests function _timer_start_periodic_every_ticks when the timer component is not valid */
    id = _timer_start_periodic_every_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks2);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, id == TIMER_NULL_ID, "Test #5 Testing 5.00: Tests function _timer_start_periodic_every_ticks when the timer component is not valid");
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, _task_errno == MQX_INVALID_COMPONENT_BASE, "Test #5 Testing 5.01: The task's error code should be MQX_INVALID_COMPONENT_BASE");
    _task_set_error(MQX_OK);

    /* Restore the value of VALID field to original value */
    set_timer_component_valid_field(temp);

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);

    /* Tests function _timer_start_periodic_every_ticks when memory is insufficient */
    id = _timer_start_periodic_every_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks2);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, id == TIMER_NULL_ID, "Test #5 Testing 5.02: Tests function _timer_start_periodic_every_ticks when memory is insufficient");
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #5 Testing 5.03: The task's error code should be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

    /* Frees all of memory that was temporary allocated */
    memory_free_all(&memory_alloc_info);

    /* Reset global counter */
    count = 0;

    /* Tests function _timer_start_periodic_every_ticks operation */
    id = _timer_start_periodic_every_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks2);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, id != TIMER_NULL_ID, "Test #5 Testing 5.04: Start a valid timer using TIMER_ELAPSED_TIME_MODE to fire every 1/50th of a second");

    /* Stagger timer and verification */
    _time_delay_ticks(16);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, count == 4, "Test #5 Testing 5.05: Check the count every 1/50th of a second");

    /* Cancel the timer */
    result = _timer_cancel(id);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5 Testing 5.06: Cancel the timer");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : Testing function _timer_start_oneshot_after in special cases.
* Requirements :
*                TIMER007,TIMER008,TIMER010.
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    _timer_id                 id;
    _mqx_uint                 temp;
    _mqx_uint                 result;
    MQX_TICK_STRUCT           ticks2;
    MEMORY_ALLOC_INFO_STRUCT  memory_alloc_info;

    /* Set the alarm */
    _time_init_ticks(&ticks2, 2);

    /* The timer components was previously created */

    /* Backups the value of VALID field */
    temp = get_timer_component_valid_field();
    /* Makes the timer component to invalid */
    set_timer_component_valid_field(temp + 1);

    /* Tests function _timer_start_oneshot_after when the timer component is not valid */
    id = _timer_start_oneshot_after(capture_fcn, (void *)&ticks2,
        TIMER_ELAPSED_TIME_MODE, TICK_DIFF_MS);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, id == TIMER_NULL_ID, "Test #6 Testing 6.00: Tests function _timer_start_oneshot_after when the timer component is not valid");
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, _task_errno == MQX_INVALID_COMPONENT_BASE, "Test #6 Testing 6.01: The task's error code should be MQX_INVALID_COMPONENT_BASE");
    _task_set_error(MQX_OK);

    /* Restore the value of VALID field to original value */
    set_timer_component_valid_field(temp);

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);

    /* Tests function _timer_start_oneshot_after when memory is insufficient */
    id = _timer_start_oneshot_after(capture_fcn, (void *)&ticks2,
        TIMER_ELAPSED_TIME_MODE, TICK_DIFF_MS);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, id == TIMER_NULL_ID, "Test #6 Testing 6.02: Tests function _timer_start_oneshot_after when memory is insufficient");
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #6 Testing 6.03: The task's error code should be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

    /* Frees all of memory that was temporary allocated */
    memory_free_all(&memory_alloc_info);

    /* Tests function _timer_start_oneshot_after operation */
    id = _timer_start_oneshot_after(capture_fcn, (void *)&ticks2,
        TIMER_ELAPSED_TIME_MODE, TICK_DIFF_MS);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, id != TIMER_NULL_ID, "Test #6 Testing 6.04: Start a valid timer should pass");

    /* Wait for timer notification fcn to unblock us */
    _task_block();

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : Testing function _timer_start_oneshot_at in special cases.
* Requirements :
*                TIMER015,TIMER016,TIMER018.
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    _timer_id                 id;
    _mqx_uint                 temp;
    TIME_STRUCT               time;
    MQX_TICK_STRUCT           ticks2;
    MEMORY_ALLOC_INFO_STRUCT  memory_alloc_info;

    /* Set the alarm */
    _time_init_ticks(&ticks2, 2);

    /* Gets the number of seconds and milliseconds since the processor started */
    _int_disable();
    _time_delay(1);
    _time_get_elapsed(&time);
    _int_enable();
    time_adjust(&time, TICK_DIFF_MS);

    /* The timer components was previously created */

    /* Backups the value of VALID field */
    temp = get_timer_component_valid_field();
    /* Makes the timer component to invalid */
    set_timer_component_valid_field(temp + 1);

    /* Tests function _timer_start_oneshot_at when the timer component is not valid */
    id = _timer_start_oneshot_at(capture_fcn, (void *)&ticks2,
        TIMER_ELAPSED_TIME_MODE, &time);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, id == TIMER_NULL_ID, "Test #7 Testing 7.00: Tests function _timer_start_oneshot_at when the timer component is not valid");
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, _task_errno == MQX_INVALID_COMPONENT_BASE, "Test #7 Testing 7.01: The task's error code should be MQX_INVALID_COMPONENT_BASE");
    _task_set_error(MQX_OK);

    /* Restore the value of VALID field to original value */
    set_timer_component_valid_field(temp);

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);

    /* Tests function _timer_start_oneshot_at when memory is insufficient */
    id = _timer_start_oneshot_at(capture_fcn, (void *)&ticks2,
        TIMER_ELAPSED_TIME_MODE, &time);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, id == TIMER_NULL_ID, "Test #7 Testing 7.02: Tests function _timer_start_oneshot_at when memory is insufficient");
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #7 Testing 7.03: The task's error code should be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

    /* Frees all of memory that was temporary allocated */
    memory_free_all(&memory_alloc_info);

    /* Start a valid timer. Should pass */
    _int_disable();
    _time_delay(1);
    _time_get_elapsed(&time);
    _int_enable();
    time_adjust(&time, TICK_DIFF_MS);

    /* Tests function _timer_start_oneshot_at operation */
    id = _timer_start_oneshot_at(capture_fcn, (void *)&ticks2,
        TIMER_ELAPSED_TIME_MODE, &time);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, id != TIMER_NULL_ID, "Test #7 Testing 7.04: Start a valid timer should pass");

    /* Wait for timer notification fcn to unblock us */
    _task_block();
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : Testing function _timer_start_periodic_at in special cases.
* Requirements :
*                TIMER023,TIMER024,TIMER026.
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
    _timer_id                 id;
    _mqx_uint                 temp;
    _mqx_uint                 result;
    TIME_STRUCT               time;
    MQX_TICK_STRUCT           ticks2;
    MEMORY_ALLOC_INFO_STRUCT  memory_alloc_info;

    /* Set the alarm */
    _time_init_ticks(&ticks2, 2);

    /* Gets the number of seconds and milliseconds since the processor started */
    _int_disable();
    _time_delay(1);
    _time_get_elapsed(&time);
    _int_enable();
    time_adjust(&time, TICK_DIFF_MS+1000/BSP_ALARM_FREQUENCY);/*+minimal time in ticks*/

    /* The timer components was previously created */

    /* Backups the value of VALID field */
    temp = get_timer_component_valid_field();
    /* Makes the timer component to invalid */
    set_timer_component_valid_field(temp + 1);

    /* Tests function _timer_start_periodic_at when the timer component is not valid */
    id = _timer_start_periodic_at(capture_fcn, (void *)&ticks2,
        TIMER_ELAPSED_TIME_MODE, &time, TICK_DIFF_MS);
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, id == TIMER_NULL_ID, "Test #8 Testing 8.00: Tests function _timer_start_periodic_at when the timer component is not valid");
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, _task_errno == MQX_INVALID_COMPONENT_BASE, "Test #8 Testing 8.01: The task's error code should be MQX_INVALID_COMPONENT_BASE");
    _task_set_error(MQX_OK);

    /* Restore the value of VALID field to original value */
    set_timer_component_valid_field(temp);

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);

    /* Tests function _timer_start_periodic_at when memory is insufficient */
    id = _timer_start_periodic_at(capture_fcn, (void *)&ticks2,
        TIMER_ELAPSED_TIME_MODE, &time, TICK_DIFF_MS);
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, id == TIMER_NULL_ID, "Test #8 Testing 8.02: Tests function _timer_start_periodic_at when memory is insufficient");
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #8 Testing 8.03: The task's error code should be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

    /* Frees all of memory that was temporary allocated */
    memory_free_all(&memory_alloc_info);

    /* Start a valid timer. Should pass */
    _int_disable();
    _time_delay(1);
    _time_get_elapsed(&time);
    time_adjust(&time, TICK_DIFF_MS+1000/BSP_ALARM_FREQUENCY);//+minimal time in ticks

    /* Tests function _timer_start_periodic_at operation */
    id = _timer_start_periodic_at(capture_fcn, (void *)&ticks2,
        TIMER_ELAPSED_TIME_MODE, &time, TICK_DIFF_MS);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id != TIMER_NULL_ID, "Test #8 Testing 8.04: Start a valid timer should pass");

    /* First wait for timer notification fcn to unblock us */
    _task_block();

    /* Cancel the timer */
    result = _timer_cancel(id);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8 Testing 8.05: Cancel the timer");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : Testing function _timer_start_periodic_every in special cases.
* Requirements :
*                TIMER031,TIMER032,TIMER034.
*
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
    _timer_id                 id;
    _mqx_uint                 temp;
    _mqx_uint                 result;
    MQX_TICK_STRUCT           ticks2;
    MEMORY_ALLOC_INFO_STRUCT  memory_alloc_info;

    /* Set the alarm */
    _time_init_ticks(&ticks2, 2);

    /* The timer components was previously created */

    /* Backups the value of VALID field */
    temp = get_timer_component_valid_field();
    /* Makes the timer component to invalid */
    set_timer_component_valid_field(temp + 1);

    /* Tests function _timer_start_periodic_every when the timer component is not valid */
    id = _timer_start_periodic_every(capture_fcn, (void *)&ticks2,
        TIMER_ELAPSED_TIME_MODE, TICK_DIFF_MS);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, id == TIMER_NULL_ID, "Test #9 Testing 9.00: Tests function _timer_start_periodic_every when the timer component is not valid");
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, _task_errno == MQX_INVALID_COMPONENT_BASE, "Test #9 Testing 9.01: The task's error code should be MQX_INVALID_COMPONENT_BASE");
    _task_set_error(MQX_OK);

    /* Restore the value of VALID field to original value */
    set_timer_component_valid_field(temp);

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);

    /* Tests function _timer_start_periodic_every when memory is insufficient */
    id = _timer_start_periodic_every(capture_fcn, (void *)&ticks2,
        TIMER_ELAPSED_TIME_MODE, TICK_DIFF_MS);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, id == TIMER_NULL_ID, "Test #9 Testing 9.02: Tests function _timer_start_periodic_every when memory is insufficient");
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #9 Testing 9.03: The task's error code should be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

    /* Frees all of memory that was temporary allocated */
    memory_free_all(&memory_alloc_info);

    /* Tests function _timer_start_periodic_every operation */
    id = _timer_start_periodic_every(capture_fcn, (void *)&ticks2,
        TIMER_ELAPSED_TIME_MODE, TICK_DIFF_MS);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, id != TIMER_NULL_ID, "Test #9 Testing 9.04: Start a valid timer");

    /* First wait for timer notification fcn to unblock us */
    _task_block();

    /* Cancel the timer */
    result = _timer_cancel(id);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing 9.05: Cancel the timer");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_10_main_task
* Comments     : Testing function _timer_cancel in special cases.
* Requirements :
*                TIMER001,TIMER002,TIMER003,TIMER004.
*
*END*---------------------------------------------------------------------*/
void tc_10_main_task(void)
{
    _timer_id                 id;
    _mqx_uint                 temp;
    _mqx_uint                 result;
    MQX_TICK_STRUCT           ticks;
    void                     *temp_ptr;

    /* Set the alarm */
    _time_init_ticks(&ticks, 10);

    /* Start a timer that expires the number of ticks */
    id = _timer_start_oneshot_after_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks);
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, id != TIMER_NULL_ID, "Test #10 Testing 10.00: Start a timer that expires the number of ticks.");

    /* Backup pointer to timer component */
    temp_ptr = get_timer_component_ptr();
    /* Sets pointer to timer component to NULL */
    set_timer_component_ptr( NULL);

    /* Cancel timer when the pointer to timer component is NULL */
    result = _timer_cancel(id);
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #10 Testing 10.01: Tests function _timer_cancel when timer component was not created");
    _task_set_error(MQX_OK);

    /* Restore pointer to timer component to original value */
    set_timer_component_ptr( temp_ptr);

    /* Save the current isr counting value */
    temp = get_in_isr();
    /* Set the current task as an isr */
    set_in_isr(1);
    /* [_timer_cancel] Cancel timer in ISR */
    result = _timer_cancel(id);
    /* Restore the isr counting value */
    set_in_isr(temp);

    /* Verify the result of creating */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #10 Testing 10.02: Calling _timer_cancel in ISR should be failed");
    _task_set_error(MQX_OK);

    /* Backups the value of VALID field */
    temp = get_timer_component_valid_field();
    /* Makes the timer component to invalid */
    set_timer_component_valid_field(temp + 1);

    /* Cancel timer when the timer component is not valid */
    result = _timer_cancel(id);
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, result == MQX_INVALID_COMPONENT_BASE, "Test #10 Testing 10.03: Tests function _timer_cancel when timer component is not valid");
    _task_set_error(MQX_OK);

    /* Restore the value of VALID field to original value */
    set_timer_component_valid_field(temp);

    /* Cancel timer when timer id is 0 */
    result = _timer_cancel(0);
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, result == MQX_INVALID_PARAMETER, "Test #10 Testing 10.04: Tests function _timer_cancel when timer id is 0");
    _task_set_error(MQX_OK);

    /* Start a timer that expires the number of ticks */
    id = _timer_start_oneshot_after_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks);
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, id != TIMER_NULL_ID, "Test #10 Testing 10.05: Start a timer that expires the number of ticks.");

    /* Cancel timer when timer is not valid */
    result = _timer_cancel(id);
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, result == MQX_OK, "Test #10 Testing 10.06: Tests function _timer_cancel operation");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_11_main_task
* Comments     : Testing function _timer_test in special cases.
* Requirements :
*                TIMER039,TIMER040,TIMER041.
*
*END*---------------------------------------------------------------------*/
void tc_11_main_task(void)
{
    _timer_id        id;
    _mqx_uint        temp;
    _mqx_uint        result;
    void            *temp_ptr;
    void            *timer_error_ptr;
    MQX_TICK_STRUCT  ticks;

    /* Backups queue of timer */
    temp_ptr = get_timer_queue_ptr();
    /* Makes queue of timer to invalid */
    set_timer_queue_ptr(NULL);

    /* Tests the timer component when the queue of timers is not valid */
    result = _timer_test(&timer_error_ptr);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_CORRUPT_QUEUE, "Test #11 Testing 11.00: Tests function _timer_test when the queue of timers is not valid");
    _task_set_error(MQX_OK);

    /* Restore queue of timer to original value */
    set_timer_queue_ptr(temp_ptr);

    /* Set the alarm */
    _time_init_ticks(&ticks, 1);

    /* Start a timer that expires the number of ticks */
    id = _timer_start_oneshot_after_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks);
    EU_ASSERT_REF_TC_MSG( tc_11_main_task, id != TIMER_NULL_ID, "Test #11 Testing 11.01: Start a timer that expires the number of ticks");

    /* Backups value of VALID field of the timer entry in the timer queue */
    temp = get_timer_valid_field();
    /* Makes value of VALID field of the timer entry to invalid */
    set_timer_valid_field(temp + 1);

    /* Tests all of timer when the timer entry in the timer queue is not valid*/
    result = _timer_test(&timer_error_ptr);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_INVALID_COMPONENT_HANDLE, "Test #11 Testing 11.02: Tests function _timer_test when the timer entry in the timer queue is not valid");
    _task_set_error(MQX_OK);

    /* Restore the timer entry in the timer queue to original value */
    set_timer_valid_field(temp);

    /* Tests all of timer */
    result = _timer_test(&timer_error_ptr);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "Test #11 Testing 11.03: Tests operation of function _timer_test");
    _task_set_error(MQX_OK);

}

/*----------------------------------------------------------------------*/
/* Define Test Suite */
 EU_TEST_SUITE_BEGIN(suite_timer2)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1: Tests function _timer_create_component in special cases"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2: Tests function _timer_start_oneshot_after_ticks in special cases"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3: Tests function _timer_start_oneshot_at_ticks in special cases"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4: Tests function _timer_start_periodic_at_ticks in special cases"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST # 5: Tests function _timer_start_periodic_every_ticks in special cases"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST # 6: Tests function _timer_start_oneshot_after in special cases"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST # 7: Tests function _timer_start_oneshot_at in special cases"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST # 8: Tests function _timer_start_periodic_at in special cases"),
    EU_TEST_CASE_ADD(tc_9_main_task, " TEST # 9: Tests function _timer_start_periodic_at in special cases"),
    EU_TEST_CASE_ADD(tc_10_main_task, " TEST # 10: Tests function _timer_cancel in special cases"),
    EU_TEST_CASE_ADD(tc_11_main_task, " TEST # 11: Tests function _timer_test in special cases")
 EU_TEST_SUITE_END(suite_timer2)
/* Add test suites */
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_timer2, " - Test of kernel Timer Component")
 EU_TEST_REGISTRY_END()
/*----------------------------------------------------------------------*/

/*TASK*-------------------------------------------------------------------
*
* Task Name : main_task
* Comments  :
*
*END*-------------------------------------------------------------------*/
void main_task
   (
       uint32_t param
   )
{ /* Body */
    count = 0;
    main_td = _task_get_td(0);

    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
} /* Endbody */

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : increment_counter
* Return Value  : void
* Comments      : increments the counter
*
*END*----------------------------------------------------------------------*/
void capture_fcn
   (
       _timer_id id, void *tick_ptr, uint32_t seconds, uint32_t milliseconds
   )
{ /* Body */

    _time_get_elapsed_ticks((MQX_TICK_STRUCT_PTR)tick_ptr);

    _task_ready(main_td);

} /* Endbody */

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : increment_counter_ticks
* Return Value  : void
* Comments      : increments the counter
*
*END*----------------------------------------------------------------------*/
void increment_counter_ticks
   (
       _timer_id           id,
       void               *count_ptr,
       MQX_TICK_STRUCT_PTR tick_ptr
   )
{ /* Body */
    uint32_t *increment_ptr = (uint32_t *)count_ptr;

    (*increment_ptr)++;

} /* Endbody */

/* EOF */
