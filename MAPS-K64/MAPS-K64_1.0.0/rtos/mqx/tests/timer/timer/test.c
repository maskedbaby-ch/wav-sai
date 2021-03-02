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
* $Version : 3.8.6.0$
* $Date    : Sep-23-2011$
*
* Comments:
*
*   This file contains code for the MQX timer verification
*   program.
*   Revision History:
*   Date          Version        Changes
*   Apr, 1998      2.40          Original version
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <timer.h>
#include <log.h>
#include <klog.h>
#include "util.h"
//#include <test_module.h>

#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test

#define MAIN_TASK 1

/* #define KLOG_ON */

void main_task(uint32_t);

//------------------------------------------------------------------------
// Test suite function prototipe
void tc_1_main_task();
void tc_2_main_task();
void tc_3_main_task();
void tc_4_main_task();
void tc_5_main_task();
void tc_6_main_task();
void tc_7_main_task();
void tc_8_main_task();
//------------------------------------------------------------------------

volatile uint32_t count  = 0;
volatile uint32_t count1 = 0;
volatile uint32_t count2 = 0;
volatile uint32_t count3 = 0;
volatile uint32_t tick_diff_ms = 3*1000/BSP_ALARM_FREQUENCY;
void   *main_td;

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK,  main_task, 4000,   8,   "Main",  MQX_AUTO_START_TASK, 0},
   { 0,          0,         0,      0,   0,       0,                   0}
};


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


/*FUNCTION*-----------------------------------------------------------------
* 
* Function Name : time_adjust
* Return Value  : void
* Comments      : Adds a specified number of ms to the time
*
*END*----------------------------------------------------------------------*/

void time_adjust
   (
      TIME_STRUCT_PTR time_ptr,
      uint32_t         ms
   )
{/* Body */
   time_ptr->MILLISECONDS += ms;
   while (time_ptr->MILLISECONDS >= 1000) {
      time_ptr->SECONDS++;
      time_ptr->MILLISECONDS -= 1000;
   } /* Endwhile */

} /* Endbody */

//------------------------------------------------------------------------
   KERNEL_DATA_STRUCT_PTR  kernel_data_ptr;
   _timer_id               id;
   MQX_TICK_STRUCT         ticks, ticks2;

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST #1: _timer_create_component
//
//END---------------------------------------------------------------------

void tc_1_main_task() {
    _task_id    timer_task_id;
    _mqx_uint   task_error_code;
    void       *timer_error_ptr;

    /* issue _timer_create_component with priority of 0, 0 stack size
    ** this should succeed
    */
    task_error_code = _timer_create_component(0, 0);   

    EU_ASSERT_REF_TC_MSG( tc_1_main_task, task_error_code == MQX_OK, "Test #1: Test 1.2 Create the timer component")

    /* uninstall timer task */
    _int_disable();
    timer_task_id = _task_get_id_from_name("Timer Task");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, timer_task_id != MQX_NULL_TASK_ID, "Test #1: Test 1.3 Get the task ID is associated with Timer task")
    task_error_code = _task_destroy(timer_task_id);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, task_error_code == MQX_OK, "Test #1: Test 1.4 Destroy the Timer task operation")
    kernel_data_ptr->KERNEL_COMPONENTS[KERNEL_TIMER] = 0;
    _int_enable();

    /* issue _timer_create_component with TIMER_DEFAULT_TASK_PRIORITY and
    ** TIMER_DEFAULT_STACK_SIZE.  Verify success.
    */
    task_error_code = _timer_create_component(TIMER_DEFAULT_TASK_PRIORITY,
        TIMER_DEFAULT_STACK_SIZE);

    EU_ASSERT_REF_TC_MSG( tc_1_main_task, task_error_code == MQX_OK, "Test #1: Test 1.5 Create the timer component")

    task_error_code = _timer_test(&timer_error_ptr);

    EU_ASSERT_REF_TC_MSG( tc_1_main_task, task_error_code == MQX_OK, "Test #1: Test 1.6 Test the timer component")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST #2: oneshot_after
//
//END---------------------------------------------------------------------

void tc_2_main_task() {
    _mqx_uint   task_error_code;
    void       *timer_error_ptr;

    /* start timer with NULL for notification routine. should fail */
    _time_init_ticks(&ticks, 2);
    id = _timer_start_oneshot_after_ticks(0, (void *)&count, 
        TIMER_ELAPSED_TIME_MODE, &ticks);

    EU_ASSERT_REF_TC_MSG( tc_2_main_task, id == TIMER_NULL_ID, "Test #2: Test 2.1 Start a timer that expires the mumber of ticks")
    if (id == TIMER_NULL_ID) {
        task_error_code = _task_get_error();
        EU_ASSERT_REF_TC_MSG( tc_2_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #2: Test 2.2 Get task error code string should have returned MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start timer with 0 for milliseconds. should fail */
    ticks = _mqx_zero_tick_struct;
    id = _timer_start_oneshot_after_ticks(increment_counter_ticks, 
        (void *)&count, TIMER_ELAPSED_TIME_MODE, &ticks);

    EU_ASSERT_REF_TC_MSG( tc_2_main_task, id == TIMER_NULL_ID, "Test #2: Test 2.3 Start a timer that expires the mumber of ticks")
    if (id == TIMER_NULL_ID) {
        task_error_code = _task_get_error();
        EU_ASSERT_REF_TC_MSG( tc_2_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #2: Test 2.4 Get task error code string should have returned MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* 
    ** Start timer with value other than TIMER_ELAPSED_TIME_MODE or
    ** TIMER_KERNEL_TIME_MODE.  should fail 
    */
    _time_init_ticks(&ticks, 2);
    id = _timer_start_oneshot_after_ticks(increment_counter_ticks, 
        (void *)&count, 0, &ticks);

    EU_ASSERT_REF_TC_MSG( tc_2_main_task, id == TIMER_NULL_ID, "Test #2: Test 2.5 Start a timer that expires the mumber of ticks")
    if (id == TIMER_NULL_ID) {
        task_error_code = _task_get_error();
        EU_ASSERT_REF_TC_MSG( tc_2_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #2: Test 2.6 Get task error code string should have returned MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start a valid timer using TIMER_ELAPSED_TIME_MODE
    ** verify that it worked, i.e. count is 1
    */
    _time_init_ticks(&ticks, 1);
    id = _timer_start_oneshot_after_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks);

    EU_ASSERT_REF_TC_MSG( tc_2_main_task, id != TIMER_NULL_ID, "Test #2: Test 2.7 Start a timer that expires the mumber of ticks")

    _time_delay_ticks(4);

    EU_ASSERT_REF_TC_MSG( tc_2_main_task, count == 1, "Test #2: Test 2.8 Timer_start_oneshot_after count should be 1.")

    /* reset global counter. */
    count = 0;

    /* start a valid timer using TIMER_KERNEL_TIME_MODE
    ** verify that it worked. i.e. count is 1
    */
    _time_init_ticks(&ticks, 1);
    id = _timer_start_oneshot_after_ticks(increment_counter_ticks, (void *)&count,
        TIMER_KERNEL_TIME_MODE, &ticks);

    EU_ASSERT_REF_TC_MSG( tc_2_main_task, id != TIMER_NULL_ID, "Test #2: Test 2.9 Start a timer that expires the mumber of ticks.")

    _time_delay_ticks(4);

    EU_ASSERT_REF_TC_MSG( tc_2_main_task, count == 1, "Test #2: Test 2.10 Timer_start_oneshot_after count should be 1.")

    /* reset global counter. */
    count = 0;

    /* start a valid timer */
    _time_init_ticks(&ticks, 4);
    id = _timer_start_oneshot_after_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks);

    EU_ASSERT_REF_TC_MSG( tc_2_main_task, id != TIMER_NULL_ID, "Test #2: Test 2.11 Start a timer that expires the mumber of ticks.")

    /* cancel timer before it is scheduled to fire */
    task_error_code = _timer_cancel(id);

    EU_ASSERT_REF_TC_MSG( tc_2_main_task, task_error_code == MQX_OK, "Test #2: Test 2.12 Cancel an outstanding timer request")

    /* give it time to fire */
    _time_delay_ticks(5);

    /* verify that it did not occur */
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, count == 0, "Test #2: Test 2.13 Timer_start_oneshot_after count should be 0.")

    /* start a valid timer */
    _time_init_ticks(&ticks, 1);
    id = _timer_start_oneshot_after_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks);

    EU_ASSERT_REF_TC_MSG( tc_2_main_task, id != TIMER_NULL_ID, "Test #2: Test 2.14 Start a timer that expires the mumber of ticks.")

    /* give it time to fire */
    _time_delay_ticks(4);

    /* cancel it after it fires */
    task_error_code = _timer_cancel(id);

    /* verify that cancel failed */
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, task_error_code != MQX_OK, "Test #2: Test 2.15 Cancel timer should have failed.")

    task_error_code = _timer_test(&timer_error_ptr);

    EU_ASSERT_REF_TC_MSG(tc_2_main_task, task_error_code == MQX_OK, "Test #2: Test 2.16 Test the timer component")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST #3: oneshot at
//
//END---------------------------------------------------------------------

void tc_3_main_task() {
    _mqx_uint   task_error_code;
    void       *timer_error_ptr;

    /* set the alarm */
    _time_get_elapsed_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 4, &ticks);

    /* start timer with NULL for notification routine. should fail */
    id = _timer_start_oneshot_at_ticks(0, (void *)&count, TIMER_ELAPSED_TIME_MODE, 
        &ticks);

    EU_ASSERT_REF_TC_MSG(tc_3_main_task, id == TIMER_NULL_ID, "Test #3: Test 3.1 Timer_start_oneshot_at_ticks should have failed.")
    if (id == TIMER_NULL_ID) {
        task_error_code = _task_get_error();
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #3: Test 3.2 Timer_start_oneshot_at get error code should have returned MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start timer with NULL time pointer. should fail */
    id = _timer_start_oneshot_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, NULL);

    EU_ASSERT_REF_TC_MSG(tc_3_main_task, id == TIMER_NULL_ID, "Test #3: Test 3.3 Timer_start_oneshot_at_ticks should have failed.")
    if (id == TIMER_NULL_ID) {
        task_error_code = _task_get_error();
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #3: Test 3.4 Timer_start_oneshot_at get error code should have returned MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start timer with value other than TIMER_ELAPSED_TIME_MODE or
    ** TIMER_KERNEL_TIME_MODE.  should fail */
    id = _timer_start_oneshot_at_ticks(increment_counter_ticks, (void *)&count, 0, &ticks);

    EU_ASSERT_REF_TC_MSG(tc_3_main_task, id == TIMER_NULL_ID, "Test #3: Test 3.5 Timer_start_oneshot_at_ticks should have failed.")
    if (id == TIMER_NULL_ID) {
        task_error_code = _task_get_error();
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #3: Test 3.6 Timer_start_oneshot_at get error code should have returned MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* reset global counter. */
    count = 0;

    /* start a valid timer using TIMER_ELAPSED_TIME_MODE
    ** verify that it worked, i.e. count is 1
    */

    /* set the alarm */
    _time_get_elapsed_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 4, &ticks);

    id = _timer_start_oneshot_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks);

    EU_ASSERT_REF_TC_MSG(tc_3_main_task, id != TIMER_NULL_ID, "Test #3: Test 3.7 Timer_start_oneshot_at")
    if (id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    /* wait for the timer to fire */
    _time_delay_ticks(6);

    EU_ASSERT_REF_TC_MSG(tc_3_main_task, count == 1, "Test #3: Test 3.8 Timer_start_oneshot_at count should be 1.")

    /* reset global counter. */
    count = 0;

    /* set the alarm */
    _time_get_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 4, &ticks);

    /* start a valid timer using TIMER_KERNEL_TIME_MODE
    **   verify that it worked. i.e. count is 1
    */

    id = _timer_start_oneshot_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_KERNEL_TIME_MODE, &ticks);

    EU_ASSERT_REF_TC_MSG(tc_3_main_task, id != TIMER_NULL_ID, "Test #3: Test 3.9 Timer_start_oneshot_at")
    if (id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    /* wait for the timer to fire */
    _time_delay_ticks(6);

    EU_ASSERT_REF_TC_MSG(tc_3_main_task, count == 1, "Test #3: Test 3.10 Timer_start_oneshot_at count should be 1.")

    /* reset global counter. */
    count = 0;

    /* set the alarm */
    _time_get_elapsed_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 6, &ticks);

    /* start a valid timer */
    id = _timer_start_oneshot_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks);

    EU_ASSERT_REF_TC_MSG(tc_3_main_task, id != TIMER_NULL_ID, "Test #3: Test 3.11 Timer_start_oneshot_at")
    if (id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    /* cancel timer before it is scheduled to fire */
    task_error_code = _timer_cancel(id);

    EU_ASSERT_REF_TC_MSG(tc_3_main_task, task_error_code == MQX_OK, "Test #3: Test 3.12 Cancel an outstanding timer.")

    /* give the timer time to fire */
    _time_delay_ticks(2);

    /* verify that it did not occur */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, count == 0, "Test #3: Test 3.13 Timer_start_oneshot_at count should be 0.")

    /* set the alarm */
    _time_get_elapsed_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 4, &ticks);

    /* start a valid timer */
    id = _timer_start_oneshot_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks);

    EU_ASSERT_REF_TC_MSG(tc_3_main_task, id != TIMER_NULL_ID, "Test #3: Test 3.14 Timer_start_oneshot_at")
    if (id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    /* give the timer time to fire */
    _time_delay_ticks(6);

    /* cancel it after it fires */
    task_error_code = _timer_cancel(id);

    /* verify that cancel failed */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, task_error_code != MQX_OK, "Test #3: Test 3.15 Cancel timer should have failed.")

    task_error_code = _timer_test(&timer_error_ptr);

    EU_ASSERT_REF_TC_MSG(tc_3_main_task, task_error_code == MQX_OK, "Test #3: Test 3.16 Test timer")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST #4: periodic at 
//
//END---------------------------------------------------------------------

void tc_4_main_task() {
    _mqx_uint   task_error_code;
    void       *timer_error_ptr;
    uint32_t     i;

    /* set the alarm */
    _time_init_ticks(&ticks2, 2);
    _time_get_elapsed_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 2, &ticks);

    /* start timer with NULL for notification routine. should fail */
    id = _timer_start_periodic_at_ticks(0, (void *)&count, 
        TIMER_ELAPSED_TIME_MODE, &ticks, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, id == TIMER_NULL_ID, "Test #4: Test 4.1 _timer_start_periodic_at_ticks should have failed.")
    if (id == TIMER_NULL_ID) {
        task_error_code = _task_get_error();
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #4: Test 4.1.1 _timer_start_periodic_at_ticks get error code should have returned MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* set the alarm */
    _time_get_elapsed_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 2, &ticks);

    /* start timer with NULL time pointer. should fail */
    id = _timer_start_periodic_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, NULL, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, id == TIMER_NULL_ID, "Test #4: Test 4.2 _timer_start_periodic_at_ticks should have failed.")
    if (id == TIMER_NULL_ID) {
        task_error_code = _task_get_error();
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #4: Test 4.3 _timer_start_periodic_at_ticks get error code should have returned MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* set the alarm */
    _time_get_elapsed_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 2, &ticks);

    /* start timer with value other than TIMER_ELAPSED_TIME_MODE or
    ** TIMER_KERNEL_TIME_MODE.  should fail */
    id = _timer_start_periodic_at_ticks(increment_counter_ticks, (void *)&count,
        0, &ticks, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, id == TIMER_NULL_ID, "Test #4: Test 4.4 _timer_start_periodic_at_ticks should have failed.")
    if (id == TIMER_NULL_ID) {
        task_error_code = _task_get_error();
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #4: Test 4.5 _timer_start_periodic_at_ticks get error code should have returned MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* set the alarm */
    _time_get_elapsed_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 2, &ticks);

    /* start timer with 0 for milliseconds. should fail */
    id = _timer_start_periodic_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks, (MQX_TICK_STRUCT_PTR)&_mqx_zero_tick_struct);

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, id == TIMER_NULL_ID, "Test #4: Test 4.6 _timer_start_periodic_at_ticks should have failed.")
    if (id == TIMER_NULL_ID) {
        task_error_code = _task_get_error();
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #4: Test 4.7 _timer_start_periodic_at_ticks get error code should have returned MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* reset global counter. */
    count = 0;

    _time_delay_ticks(2);

    /* set the alarm */
    _time_init_ticks(&ticks2, 10);
    _time_get_elapsed_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 10, &ticks);

    /* start a valid timer using TIMER_ELAPSED_TIME_MODE to fire every
    ** 20 milliseconds. verify that it worked.
    */
    id = _timer_start_periodic_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, id != TIMER_NULL_ID, "Test #4: Test 4.8 Start a valid timer using TIMER_ELAPSED_TIME_MODE to fire every 20 milliseconds.")
    if (id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    /* check the count every 1/50th of a second */
    for (i = 0; i < 10; i++) {
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, i == count, "Test #4: Test 4.9 Check the count every 1/50th of a second")
        _time_delay_ticks(10);
    } /* Endfor */

    /* cancel the timer */
    task_error_code = _timer_cancel(id);

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error_code == MQX_OK, "Test #4: Test 4.10 Cancel the timer")

    /* reset global counter. */
    count = 0;

    _time_delay_ticks(2);

    /* set the alarm */
    _time_get_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 10, &ticks);

    /* start a valid timer using TIMER_KERNEL_TIME_MODE to fire
    ** every 1/50th of a second. verify that it worked. 
    */
    id = _timer_start_periodic_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_KERNEL_TIME_MODE, &ticks, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, id != TIMER_NULL_ID, "Test #4: Test 4.11 Start a valid timer using TIMER_KERNEL_TIME_MODE to fire every 1/50th of a second.")
    if (id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    /* check the count every 1/50th of a second */
    for (i = 0; i < 10; i++) {
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, i == count, "Test #4: Test 4.12 Check the count every 1/50th of a second")
        _time_delay_ticks(10);
    } /* Endfor */


    /* cancel the timer */
    task_error_code = _timer_cancel(id);

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error_code == MQX_OK, "Test #4: Test 4.13 Cancle the timer")

    /* reset global counter. */
    count = 0;

    _time_delay_ticks(2);

    /* set the alarm */
    _time_get_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 10, &ticks);

    /* start a valid timer to fire every 1/50th of a second */
    id = _timer_start_periodic_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, id != TIMER_NULL_ID, "Test #4: Test 4.14 Start a valid timer to fire every 1/50th of a second")

    /* cancel timer before it is scheduled to fire */
    task_error_code = _timer_cancel(id);

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error_code == MQX_OK, "Test #4: Test 4.14.1 Cancel the timer")

    /* give the timer time to fire */
    _time_delay_ticks(10);

    /* verify that it did not occur */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, count == 0, "Test #4: Test 4.14.2 Timer_start_periodic_at count should be 0")

    /* Issue timer test */
    task_error_code = _timer_test(&timer_error_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error_code == MQX_OK, "Test #4: Test 4.14.3 timer test")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_5_main_task
// Comments     : TEST #5: periodic every  
//
//END---------------------------------------------------------------------

void tc_5_main_task() {
    _mqx_uint   task_error_code;
    void       *timer_error_ptr;
    uint32_t     i;

    /* start timer with NULL for notification routine. should fail */
    _time_init_ticks(&ticks2, 4);
    id = _timer_start_periodic_every_ticks(0, (void *)&count, 
        TIMER_ELAPSED_TIME_MODE, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, id == TIMER_NULL_ID, " Test #5: Test 5.1 Start timer with NULL for notification routine should have failed")
    if (id == TIMER_NULL_ID) {
        task_error_code = _task_get_error();
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, task_error_code == MQX_INVALID_PARAMETER, " Test #5: Test 5.2 Timer_start_periodic_every get error code should have returned MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start timer with value other than TIMER_ELAPSED_TIME_MODE or
    ** TIMER_KERNEL_TIME_MODE.  should fail */
    id = _timer_start_periodic_every_ticks(increment_counter_ticks, 
        (void *)&count, 0, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, id == TIMER_NULL_ID, " Test #5: Test 5.3 start timer with value other than TIMER_ELAPSED_TIME_MODE or TIMER_KERNEL_TIME_MODE should have failed")
    if (id == TIMER_NULL_ID) {
        task_error_code = _task_get_error();
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, task_error_code == MQX_INVALID_PARAMETER, " Test #5: Test 5.4 Timer_start_periodic_every get error code should have returned MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start timer with 0 for ticks. should fail */
    id = _timer_start_periodic_every_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, (MQX_TICK_STRUCT_PTR)&_mqx_zero_tick_struct);

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, id == TIMER_NULL_ID, " Test #5: Test 5.5 Start timer with 0 for ticks should have failed")
    if (id == TIMER_NULL_ID) {
        task_error_code = _task_get_error();
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, task_error_code == MQX_INVALID_PARAMETER, " Test #5: Test 5.6 Timer_start_periodic_every get error code should have returned MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* reset global counter. */
    count = 0;

    _time_delay_ticks(2);

    /* start a valid timer using TIMER_ELAPSED_TIME_MODE to fire every
    ** 1/50th of a second. verify that it worked.
    */
    id = _timer_start_periodic_every_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, id != TIMER_NULL_ID, " Test #5: Test 5.7 Start a valid timer using TIMER_ELAPSED_TIME_MODE to fire every 1/50th of a second")

    /* stagger timer and verification */
    _time_delay_ticks(1);


    /* check the count every 1/50th of a second */
    for (i = 0; i < 10; i++) {
        //      putchar('!');
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, i == count, " Test #5: Test 5.8 Check the count every 1/50th of a second")
        _time_delay_ticks(4);
    } /* Endfor */

    /* cancel the timer */
    task_error_code = _timer_cancel(id);

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, task_error_code == MQX_OK, " Test #5: Test 5.9 Cancel the timer")

    /* reset global counter. */
    count = 0;

    _time_delay_ticks(2);

    /* start a valid timer using TIMER_KERNEL_TIME_MODE to fire
    ** every 1/50th of a second. verify that it worked. 
    */
    id = _timer_start_periodic_every_ticks(increment_counter_ticks, (void *)&count,
        TIMER_KERNEL_TIME_MODE, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, id != TIMER_NULL_ID, " Test #5: Test 5.10 Start a valid timer using TIMER_KERNEL_TIME_MODE to fire every 1/50th of a second.")

    /* stagger the timer and verification */
    _time_delay_ticks(1);

    /* check the count every 1/100th of a second */
    for (i = 0; i < 10; i++) {
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, i == count, " Test #5: Test 5.11 Check the count every 1/100th of a second")
        _time_delay_ticks(4);
    } /* Endfor */

    /* cancel the timer */
    task_error_code = _timer_cancel(id);

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, task_error_code == MQX_OK, " Test #5: Test 5.12 Cancel the timer")

    /* reset global counter. */
    count = 0;

    _time_delay_ticks(2);

    /* start a valid timer to fire every 1/50th of a second */
    id = _timer_start_periodic_every_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, id != TIMER_NULL_ID, " Test #5: Test 5.13 Start a valid timer to fire every 1/50th of a second")

    /* cancel timer before it is scheduled to fire */
    task_error_code = _timer_cancel(id);

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, task_error_code == MQX_OK, " Test #5: Test 5.14 Cancel the timer")

    /* give the timer time to fire */
    _time_delay_ticks(4);

    /* verify that it did not occur */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, count == 0, " Test #5: Test 5.15 Timer_start_periodic_every test")

    /* Issue timer test */
    task_error_code = _timer_test(&timer_error_ptr);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, task_error_code == MQX_OK, " Test #5: Test 5.16 Timer test")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_6_main_task
// Comments     : TEST #6: Verify that elapsed and kernel time mode operate
//                  correctly and differently
//
//END---------------------------------------------------------------------

void tc_6_main_task() {
    _timer_id   timer1_id, timer2_id;
    _mqx_uint   task_error_code;
    uint32_t     i;

    /* reset global counter. */
    count = 0;

    _time_delay_ticks(2);

    /* set the alarm */
    _time_get_elapsed_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 4, &ticks);

    /* start a valid timer using TIMER_ELAPSED_TIME_MODE to fire every
    ** 1/50th of a second. verify that it worked.
    */
    timer1_id = _timer_start_periodic_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_6_main_task, timer1_id != TIMER_NULL_ID, "Test #6: Test 6.1 Start a valid timer using TIMER_ELAPSED_TIME_MODE to fire every 1/50th of a second")

    /* start a valid timer using TIMER_KERNEL_TIME_MODE to fire
    ** every 1/50th of a second. verify that it worked. 
    */
    timer2_id = _timer_start_periodic_at_ticks(increment_counter_ticks, (void *)&count2,
        TIMER_KERNEL_TIME_MODE, &ticks, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_6_main_task, timer2_id != TIMER_NULL_ID, "Test #6: Test 6.2 Start a valid timer using TIMER_KERNEL_TIME_MODE to fire every 1/50th of a second")

    /* check the count every 1/50th of a second */
    for (i = 0; i < 10; i++) {
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, i == count, "Test #6: Test 6.3 Check the count every 1/50th of a second")
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, i == count2, "Test #6: Test 6.4 Check the count every 1/50th of a second")
        _time_delay_ticks(4);
    } /* Endfor */

    /* cancel timer 1 */
    task_error_code = _timer_cancel(timer1_id);

    EU_ASSERT_REF_TC_MSG(tc_6_main_task, task_error_code == MQX_OK, "Test #6: Test 6.5 Cancel the timer 1 operation")

    /* cancel timer 2 */
    task_error_code = _timer_cancel(timer2_id);

    EU_ASSERT_REF_TC_MSG(tc_6_main_task, task_error_code == MQX_OK, "Test #6: Test 6.6 Cancel the timer 1 operation")

    count    = 0;
    count2   = 0;

    _time_delay_ticks(2);

    /* set the time back so the timer using KERNEL_TIME_MODE is delayed */
    _time_get_ticks(&ticks);
    PSP_SUB_TICKS_FROM_TICK_STRUCT(&ticks, 4, &ticks);
    _time_set_ticks(&ticks);

    _time_get_elapsed_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 4, &ticks);

    /* start a valid timer using TIMER_ELAPSED_TIME_MODE to fire
    ** every 1/100th of a second. verify that it worked. 
    */
    timer1_id = _timer_start_periodic_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_6_main_task, timer1_id != TIMER_NULL_ID, "Test #6: Test 6.7 start a valid timer using TIMER_ELAPSED_TIME_MODE to fire every 1/100th of a second")
    if (timer1_id == TIMER_NULL_ID)
        _task_set_error(MQX_OK);

    /* start a valid timer using TIMER_KERNEL_TIME_MODE to fire
    ** every 1/50th of a second. verify that it worked. 
    */
    timer2_id = _timer_start_periodic_at_ticks(increment_counter_ticks, (void *)&count2,
        TIMER_KERNEL_TIME_MODE, &ticks, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_6_main_task, timer2_id != TIMER_NULL_ID, "Test #6: Test 6.8 start a valid timer using TIMER_ELAPSED_TIME_MODE to fire every 1/50th of a second")
    if (timer2_id == TIMER_NULL_ID)
        _task_set_error(MQX_OK);

    _time_delay_ticks(1);

    /* Check the count every 20 milliseconds. Should be index - 1. */
    for (i = 0; i < 10; i++) {
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, i == count, "Test #6: Test 6.9 Check the count every 20ms")
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (((i != 0) || (count == 0)) && ((i == 0) || (i == count2 + 1))), "Test #6: Test 6.10 Check the count every 20ms")
        _time_delay_ticks(4);
    } /* Endfor */

    /* cancel timer 1 */
    task_error_code = _timer_cancel(timer1_id);

    EU_ASSERT_REF_TC_MSG(tc_6_main_task, task_error_code == MQX_OK, "Test #6: Test 6.11 Cancel the timer 1 operation")

    /* cancel timer 2 */
    task_error_code = _timer_cancel(timer2_id);

    EU_ASSERT_REF_TC_MSG(tc_6_main_task, task_error_code == MQX_OK, "Test #6: Test 6.12 Cancel the timer 2 operation")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_7_main_task
// Comments     : TEST #7: Combine different types of timers and verify correct operation
//
//END---------------------------------------------------------------------

void tc_7_main_task() {
    _timer_id   timer1_id, timer2_id, timer3_id;
    _mqx_uint   task_error_code;
    void       *timer_error_ptr;
    uint32_t     i;

    /* reset global counter. */
    count1   = 0;
    count2   = 0;
    count3   = 0;

    _time_delay_ticks(2);
    _time_init_ticks(&ticks2, 8);

    /* set the alarm */
    _time_get_elapsed_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 4, &ticks);

    /* start a valid timer to fire every 40 milliseconds.
    ** verify that it worked.
    */
    timer1_id = _timer_start_periodic_at_ticks(increment_counter_ticks, (void *)&count1,
        TIMER_ELAPSED_TIME_MODE, &ticks, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, timer1_id != TIMER_NULL_ID, "Test #7: Test 7.1 Start a valid timer to fire every 40 milliseconds")
    if (timer1_id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start a valid timer to fire every 60 milliseconds. 
    ** verify that it worked.              
    */
    _time_init_ticks(&ticks2, 12);
    timer2_id = _timer_start_periodic_at_ticks(increment_counter_ticks, (void *)&count2,
        TIMER_ELAPSED_TIME_MODE, &ticks, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, timer2_id != TIMER_NULL_ID, "Test #7: Test 7.2 Start a valid timer to fire every 60 milliseconds")
    if (timer2_id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start a valid timer to fire every 120 milliseconds.
    ** verify that it worked.
    */
    _time_init_ticks(&ticks2, 24);
    timer3_id = _timer_start_periodic_at_ticks(increment_counter_ticks, (void *)&count3,
        TIMER_ELAPSED_TIME_MODE, &ticks, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, timer3_id != TIMER_NULL_ID, "Test #7: Test 7.3 Start a valid timer to fire every 120 milliseconds")
    if (timer3_id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    _time_delay_ticks(1);

    /* Check the count. */
    _int_disable();
    for (i = 0; i < 10; i++) {
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, i * 3 == count1, "Test #7: Test 7.4 Check the count1")
        if (i * 3 != count1) {
            break;
        } /* Endif */
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, i * 2 == count2, "Test #7: Test 7.5 Check the count2")
        if (i * 2 != count2) {
            break;
        } /* Endif */
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, i == count3, "Test #7: Test 7.6 Check the count3")
        if (i != count3) {
            break;
        } /* Endif */
        _time_delay_ticks(24);
    } /* Endfor */
    _int_enable();

    task_error_code = _timer_cancel(timer1_id);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, task_error_code == MQX_OK, "Test #7: Test 7.7 Cancel the timer 1 operation")

    task_error_code = _timer_cancel(timer2_id);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, task_error_code == MQX_OK, "Test #7: Test 7.8 Cancel the timer 1 operation")

    task_error_code = _timer_cancel(timer3_id);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, task_error_code == MQX_OK, "Test #7: Test 7.9 Cancel the timer 1 operation")

    /* reset global counter. */
    count1   = 0;
    count2   = 0;
    count3   = 0;

    _time_init_ticks(&ticks2, 8);
    _time_delay_ticks(2);

    /* start a valid timer to fire every 40 milliseconds.
    ** verify that it worked.
    */
    timer1_id = _timer_start_periodic_every_ticks(increment_counter_ticks, (void *)&count1,
        TIMER_ELAPSED_TIME_MODE, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, timer1_id != TIMER_NULL_ID, "Test #7: Test 7.10 _timer_start_periodic_every_ticks operation start a valid timer to fire every 40 milliseconds")
    if (timer1_id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start a valid timer to fire every 60 milliseconds. 
    ** verify that it worked.              
    */
    _time_init_ticks(&ticks2, 12);
    timer2_id = _timer_start_periodic_every_ticks(increment_counter_ticks, (void *)&count2,
        TIMER_ELAPSED_TIME_MODE, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, timer2_id != TIMER_NULL_ID, "Test #7: Test 7.11 Start a valid timer to fire every 60 milliseconds")
    if (timer2_id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start a valid timer to fire every 120 milliseconds.
    ** verify that it worked.
    */
    _time_init_ticks(&ticks2, 24);
    timer3_id = _timer_start_periodic_every_ticks(increment_counter_ticks, (void *)&count3,
        TIMER_ELAPSED_TIME_MODE, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, timer3_id != TIMER_NULL_ID, "Test #7: Test 7.12 Start a valid timer to fire every 120 milliseconds")
    if (timer3_id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    _time_delay_ticks(1);

    /* Check the count. */
    _int_disable();
    for (i = 0; i < 10; i++) {
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, i * 3 == count1, "Test #7: Test 7.13 Check the count1")
        if (i * 3 != count1) {
            break;
        } /* Endif */
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, i * 2 == count2, "Test #7: Test 7.14 Check the count2")
        if (i * 2 != count2) {
            break;
        } /* Endif */
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, i == count3, "Test #7: Test 7.15 Check the count3")
        if (i != count3) {
            break;
        } /* Endif */
        _time_delay_ticks(24);
    } /* Endfor */
    _int_enable();

    task_error_code = _timer_cancel(timer1_id);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, task_error_code == MQX_OK, "Test #7: Test 7.16 Cancel the timer 1")

    task_error_code = _timer_cancel(timer2_id);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, task_error_code == MQX_OK, "Test #7: Test 7.17 Cancel the timer 2")

    task_error_code = _timer_cancel(timer3_id);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, task_error_code == MQX_OK, "Test #7: Test 7.18 Cancel the timer 3")

    /* reset global counter. */
    count1   = 0;
    count2   = 0;
    count3   = 0;

    _time_delay_ticks(2);

    /* set the alarm */
    _time_get_elapsed_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 4, &ticks);

    /* start a valid timer to fire every 40 milliseconds.
    ** verify that it worked.
    */
    _time_init_ticks(&ticks2, 8);
    timer1_id = _timer_start_periodic_at_ticks(increment_counter_ticks, (void *)&count1,
        TIMER_ELAPSED_TIME_MODE, &ticks, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, timer1_id != TIMER_NULL_ID, "Test #7: Test 7.19 Start a valid timer to fire every 40 milliseconds")
    if (timer1_id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start a valid timer to fire every 60 milliseconds. 
    ** verify that it worked.              
    */
    _time_init_ticks(&ticks2, 12);
    timer2_id = _timer_start_periodic_every_ticks(increment_counter_ticks, (void *)&count2,
        TIMER_ELAPSED_TIME_MODE, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, timer2_id != TIMER_NULL_ID, "Test #7: Test 7.20 Start a valid timer to fire every 60 milliseconds")
    if (timer2_id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    /* Check the count. */
    _int_disable();
    for (i = 0; i < 10; i++) {
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, i * 3 == count1, "Test #7: Test 7.21 Check the count1")
        if (i * 3 != count1) {
            break;
        } /* Endif */
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, i * 2 == count2, "Test #7: Test 7.22 Check the count2")
        if (i * 2 != count2) {
            break;
        } /* Endif */
        _time_delay_ticks(24);
    } /* Endfor */
    _int_enable();

    /* reset global counter. */
    count = 0;

    _time_delay_ticks(2);

    /* start a valid timer. Verify that it worked. i.e. count is 1
    */
    _time_init_ticks(&ticks2, 4);
    id = _timer_start_oneshot_after_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks2);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, id != TIMER_NULL_ID, "Test #7: Test 7.23 _timer_start_oneshot_after_ticks operation, start a valid timer")
    if (id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    _time_delay_ticks(8);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, count == 1, "Test #7: Test 7.24 Check the count should be 1")

    /* set the alarm */
    _time_get_elapsed_ticks(&ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 4, &ticks);

    /* start a valid timer using TIMER_ELAPSED_TIME_MODE
    ** verify that it worked. i.e. count is 2
    */

    id = _timer_start_oneshot_at_ticks(increment_counter_ticks, (void *)&count,
        TIMER_ELAPSED_TIME_MODE, &ticks);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, id != TIMER_NULL_ID, "Test #7: Test 7.25 Start a valid timer using TIMER_ELAPSED_TIME_MODE")
    if (id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    /* wait for the timer to fire */
    _time_delay_ticks(8);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, count == 2, "Test #7: Test 7.26 Check the count should be 2")

    task_error_code = _timer_cancel(timer1_id);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, task_error_code == MQX_OK, "Test #7: Test 7.27 Cancel the timer 1")

    task_error_code = _timer_cancel(timer2_id);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, task_error_code == MQX_OK, "Test #7: Test 7.28 Cancel the timer 2")

    task_error_code = _timer_test(&timer_error_ptr);

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, task_error_code == MQX_OK, "Test #7: Test 7.29 Test the timer") 

    _time_delay_ticks(20);  /* waiting for all loggings to be printed out */
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_8_main_task
// Comments     : TEST #8: Program timers using milliseconds interface functions
//
//END---------------------------------------------------------------------

void tc_8_main_task() {
    TIME_STRUCT     time;
    int32_t          diff;
    bool         overflow;
    _mqx_uint       task_error_code;
    uint32_t         i;

    /* start timer with NULL for notification routine. should fail */
    id = _timer_start_oneshot_after(0, (void *)&count, 
        TIMER_ELAPSED_TIME_MODE, 10);

    task_error_code = _task_get_error();

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id == TIMER_NULL_ID, "Test #8: Test 8.1 Start timer with NULL for notification routine should have failed")
    if (id == TIMER_NULL_ID) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #8: Test 8.2 Timer_start_oneshot_after get error code should have returned MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start timer with 0 ms. should fail */
    id = _timer_start_oneshot_after(capture_fcn, (void *)&count, 
        TIMER_ELAPSED_TIME_MODE, 0);
    task_error_code = _task_get_error();

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id == TIMER_NULL_ID, "Test #8: Test 8.3 Start timer with 0 ms should have failed")
    if (id == TIMER_NULL_ID) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #8: Test 8.4 Timer_start_oneshot_after get error code should have returned MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start timer with incorrect mode. should fail */
    id = _timer_start_oneshot_after(capture_fcn, (void *)&count, 0, 10);
    task_error_code = _task_get_error();

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id == TIMER_NULL_ID, "Test #8: Test 8.5 Start timer with incorrect mode should have failed")
    if (id == TIMER_NULL_ID) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #8: Test 8.6 Timer_start_oneshot_after get error code should have returned MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    // marks@5/20/03 Mask skew in tick differences that are caused by partial ticks
    /* Start CR 1148 */
    _time_set_hwtick_function(0, (void *)NULL);
    /* End   CR 1148 */

    /* Start a valid timer. Should pass */
    _int_disable();
    _time_delay(1);
    _time_get_elapsed_ticks(&ticks);
    _int_enable();
    id = _timer_start_oneshot_after(capture_fcn, (void *)&ticks2, 
        TIMER_ELAPSED_TIME_MODE, tick_diff_ms);// 15 =3 tick for K3  3*1000/BSP_ALARM_FREQUENCY

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id != TIMER_NULL_ID, "Test #8: Test 8.7 Start a valid timer should pass")
    if (id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    /* Wait for timer notification fcn to unblock us */
    _task_block();

    diff = _time_diff_milliseconds(&ticks2, &ticks, &overflow);

    task_error_code = _task_get_error();

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, !overflow, "Test #8: Test 8.8 Wait for timer notification fcn to unblock us")
    if (!overflow) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, diff == tick_diff_ms, "Test #8: Test 8.9 Timer_start_oneshot_after fired at correct time diff should be 15")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start timer with NULL for notification routine. should fail */
    id = _timer_start_oneshot_at(0, (void *)&count, 
        TIMER_ELAPSED_TIME_MODE, &time);

    task_error_code = _task_get_error();

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id == TIMER_NULL_ID, "Test #8: Test 8.10 Start timer with NULL for notification routine should have failed")
    if (id == TIMER_NULL_ID) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #8: Test 8.11 Timer_start_oneshot_at set error code must be MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start timer with NULL time pointer. should fail */
    id = _timer_start_oneshot_at(capture_fcn, (void *)&count, 
        TIMER_ELAPSED_TIME_MODE, NULL);
    task_error_code = _task_get_error();

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id == TIMER_NULL_ID, "Test #8: Test 8.12 Start timer with NULL time void *should have failed")
    if (id == TIMER_NULL_ID) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #8: Test 8.13 Timer_start_oneshot_at set error code must be MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start timer with incorrect mode. should fail */
    id = _timer_start_oneshot_at(capture_fcn, (void *)&count, 0, &time);
    task_error_code = _task_get_error();

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id == TIMER_NULL_ID, "Test #8: Test 8.14 Start timer with incorrect mode should have failed")
    if (id == TIMER_NULL_ID) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #8: Test 8.15 Timer_start_oneshot_at set error code must be MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* Start a valid timer. Should pass */
    _int_disable();
    _time_delay(1);
    _time_get_elapsed_ticks(&ticks);
    _time_get_elapsed(&time);
    _int_enable();
    time_adjust(&time, tick_diff_ms);
    id = _timer_start_oneshot_at(capture_fcn, (void *)&ticks2, 
        TIMER_ELAPSED_TIME_MODE, &time);

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id != TIMER_NULL_ID, "Test #8: Test 8.16 Start a valid timer should pass")
    if (id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    /* Wait for timer notification fcn to unblock us */
    _task_block();

    diff = _time_diff_milliseconds(&ticks2, &ticks, &overflow);

    task_error_code = _task_get_error();

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, !overflow, "Test #8: Test 8.17 Wait for timer notification fcn to unblock")
    if (!overflow) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, diff == tick_diff_ms, "Test #8: Test 8.18 Timer_start_oneshot_at fired at correct time diff should be 15")
        _task_set_error(MQX_OK);
    } /* Endif */


    /* start timer with NULL for notification routine. should fail */
    id = _timer_start_periodic_at(0, (void *)&count, 
        TIMER_ELAPSED_TIME_MODE, &time, 15);

    task_error_code = _task_get_error();

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id == TIMER_NULL_ID, "Test #8: Test 8.19 Start timer with NULL for notification routine should have failed")
    if (id == TIMER_NULL_ID) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #8: Test 8.20 Timer_start_oneshot_at set error code must be MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start timer with 0 ms period. should fail */
    id = _timer_start_periodic_at(capture_fcn, (void *)&count, 
        TIMER_ELAPSED_TIME_MODE, &time, 0);
    task_error_code = _task_get_error();

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id == TIMER_NULL_ID, "Test #8: Test 8.21 Start timer with 0 ms period should have failed")
    if (id == TIMER_NULL_ID) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #8: Test 8.22  Timer_start_oneshot_at set error code must be MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */


    /* start timer with NULL start time pointer. should fail */
    id = _timer_start_periodic_at(capture_fcn, (void *)&count, 
        TIMER_ELAPSED_TIME_MODE, NULL, 15);
    task_error_code = _task_get_error();

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id == TIMER_NULL_ID, "Test #8: Test 8.23 Start timer with NULL start time void *should have failed")
    if (id == TIMER_NULL_ID) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #8: Test 8.24 Timer_start_oneshot_at set error code must be MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start timer with incorrect mode. should fail */
    id = _timer_start_periodic_at(capture_fcn, (void *)&count, 0, &time, 10);
    task_error_code = _task_get_error();

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id == TIMER_NULL_ID, "Test #8: Test 8.25 Start timer with incorrect mode should have failed")
    if (id == TIMER_NULL_ID) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #8: Test 8.26 Timer_start_oneshot_at set error code must be MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* Start a valid timer. Should pass */
    _int_disable();
    _time_delay(1);
    _time_get_elapsed_ticks(&ticks);
    _time_get_elapsed(&time);
    time_adjust(&time, tick_diff_ms+1000/BSP_ALARM_FREQUENCY);//+minimal time in ticks
    id = _timer_start_periodic_at(capture_fcn, (void *)&ticks2, 
        TIMER_ELAPSED_TIME_MODE, &time, tick_diff_ms);

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id != TIMER_NULL_ID, "Test #8: Test 8.27 Start a valid timer should pass")
    if (id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    /* First wait for timer notification fcn to unblock us */
    _task_block();

    diff = _time_diff_milliseconds(&ticks2, &ticks, &overflow);

    task_error_code = _task_get_error();

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, !overflow, "Test #8: Test 8.28 First wait for timer notification fcn to unblock us")
    if (!overflow) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, (diff == (tick_diff_ms+1000/BSP_ALARM_FREQUENCY)), "Test #8: Test 8.29 Timer_start_periodic_at fired at correct time")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* Now loop a few times making sure timer notification function keeps firing */
    for (i = 0; i < 5; i++) {
        _time_get_elapsed_ticks(&ticks);
        _task_block();

        diff = _time_diff_milliseconds(&ticks2, &ticks, &overflow);

        task_error_code = _task_get_error();

        EU_ASSERT_REF_TC_MSG(tc_8_main_task, overflow == FALSE, "Test #8 Testing: 8.30: _time_diff_milliseconds should not be overflowed")
        if (!overflow) {
            EU_ASSERT_REF_TC_MSG(tc_8_main_task, diff == tick_diff_ms, "Test #8 Testing: 8.31: diff should be 15")
            _task_set_error(MQX_OK);
        } /* Endif */
    } /* Endfor */

    _timer_cancel(id);

    _int_enable();


    /* start timer with NULL for notification routine. should fail */
    id = _timer_start_periodic_every(0, (void *)&count, 
        TIMER_ELAPSED_TIME_MODE, 15);

    task_error_code = _task_get_error();

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id == TIMER_NULL_ID, "Test #8: Test 8.32 Start timer with NULL for notification routine should have failed")
    if (id == TIMER_NULL_ID) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #8: Test 8.33 Timer_start_oneshot_at set error code must be MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* start timer with 0 ms period. should fail */
    id = _timer_start_periodic_every(capture_fcn, (void *)&count, 
        TIMER_ELAPSED_TIME_MODE, 0);
    task_error_code = _task_get_error();

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id == TIMER_NULL_ID, "Test #8: Test 8.34 Start timer with 0 ms period should have failed")
    if (id == TIMER_NULL_ID) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #8: Test 8.35 Timer_start_oneshot_at set error code must be MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */


    /* start timer with incorrect mode. should fail */
    id = _timer_start_periodic_every(capture_fcn, (void *)&count, 0, 10);
    task_error_code = _task_get_error();

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id == TIMER_NULL_ID, "Test #8: Test 8.36 Start timer with incorrect mode should have failed")
    if (id == TIMER_NULL_ID) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, task_error_code == MQX_INVALID_PARAMETER, "Test #8: Test 8.37 Timer_start_oneshot_at set error code must be MQX_INVALID_PARAMETER")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* Start a valid timer. Should pass */
    _int_disable();
    _time_delay(1);
    _time_get_elapsed_ticks(&ticks);
    id = _timer_start_periodic_every(capture_fcn, (void *)&ticks2, 
        TIMER_ELAPSED_TIME_MODE, tick_diff_ms);

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, id != TIMER_NULL_ID, "Test #8: Test 8.38 Start a valid timer")
    if (id == TIMER_NULL_ID) {
        _task_set_error(MQX_OK);
    } /* Endif */

    /* First wait for timer notification fcn to unblock us */
    _task_block();

    diff = _time_diff_milliseconds(&ticks2, &ticks, &overflow);

    task_error_code = _task_get_error();

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, !overflow, "Test #8: Test 8.39 First wait for timer notification fcn to unblock us")
    if (!overflow) {
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, diff == tick_diff_ms, "Test #8: Test 8.40 Timer_start_periodic_every operation")
        _task_set_error(MQX_OK);
    } /* Endif */

    /* Now loop a few times making sure timer notification function keeps firing */
    for (i = 0; i < 5; i++) {
        _time_get_elapsed_ticks(&ticks);
        _task_block();

        diff = _time_diff_milliseconds(&ticks2, &ticks, &overflow);

        task_error_code = _task_get_error();

        EU_ASSERT_REF_TC_MSG(tc_8_main_task, !overflow, "Test #8: Test 8.41 check _time_diff_milliseconds overflowed")
        if (!overflow) {
            EU_ASSERT_REF_TC_MSG(tc_8_main_task, diff == tick_diff_ms, "Test #8: Test 8.42 Check timer_start_periodic_every fired at correct time")
            _task_set_error(MQX_OK);
        } /* Endif */
    } /* Endfor */

    _timer_cancel(id);
    _int_enable();
}

//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_timer)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1: _timer_create_component"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2: _timer_start_oneshot_after*"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3: _timer_start_oneshot_at*"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4: _timer_start_periodic_at*"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST # 5: _timer_start_periodic_every*"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST # 6: elapsed and kernel time operation"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST # 7: test with differnt timer types"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST # 8: test timers using milliseconds interface functions")
 EU_TEST_SUITE_END(suite_timer)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_timer, " - Test of kernel Timer Component")
 EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

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
    count = count1 = count2 = count3 = 0;

    main_td = _task_get_td(0);

    kernel_data_ptr = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();
    _time_get_elapsed_ticks(&ticks);
    _time_set_ticks(&ticks);
//    _time_get_ticks(&ticks);
    

   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* Endbody */

/* EOF */
