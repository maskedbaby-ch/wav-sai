/**HEADER********************************************************************
*
* Copyright (c) 2008-2013 Freescale Semiconductor;
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
* $Version : 4.0.2.0$
* $Date    : July-15-2013$
*
* Comments:
*
*   This file contains the source functions for testing light weight
*   events
* Requirements:
*   LWEVENT018, LWEVENT019, LWEVENT026, LWEVENT027
*END************************************************************************/

#include <mqx_inc.h>
#include <lwevent.h>
#include <lwevent_prv.h>
#include <bsp.h>
#include <util.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test

#define MAIN_TASK       10
#define LWEVENT_TASK    11

extern char   *_mqx_get_task_error_code_string(uint32_t);

extern void main_task(uint32_t);
extern void lwevent_task(uint32_t);

void tc_1_main_task(void);// Test #1 - Testing _lwevent_create
void tc_2_main_task(void);// Test #2 - Testing _lwevent_wait_ticks
void tc_3_main_task(void);// Test #3 - Testing _lwevent_wait_for
void tc_4_main_task(void);// Test #4 - Testing _lwevent_wait_until
void tc_5_main_task(void);// Test #5 - Testing waiting in task
void tc_6_main_task(void);// Test #6 - Testing _lwevent_clear

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
{ MAIN_TASK,       main_task,       2000,  7, "Main",       MQX_AUTO_START_TASK},
{ LWEVENT_TASK,    lwevent_task,      2000,  6, "LWS",        0},
{ 0,               0,               0,     0, 0,            0}
};

LWEVENT_STRUCT lwevent;

volatile _mqx_uint lwevent_count = 0;
volatile _mqx_uint lwevent_timeout = 0;

/*TASK*-------------------------------------------------------------------
*
* Task Name : lwevent_task
* Comments  : This task waits on a lwevent.
*
*END*----------------------------------------------------------------------*/

void lwevent_task
   (
      uint32_t dummy
   )
{ /* Body */
    _mqx_uint result;

    while (TRUE) {
        result = _lwevent_wait_ticks(&lwevent, 1, TRUE, 5);
        _lwevent_clear(&lwevent,1);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK || result == LWEVENT_WAIT_TIMEOUT || result == MQX_LWEVENT_INVALID), "Test #5 Testing: lwevent_wait_ticks should have returned either MQX_OK or LWEVENT_WAIT_TIMEOUT or MQX_LWEVENT_INVALID")
        if (result == MQX_OK) {
            lwevent_count++;
        } else if (result == LWEVENT_WAIT_TIMEOUT) {
            lwevent_timeout++;
        } else if (result == MQX_LWEVENT_INVALID) {
            _task_destroy(0);
        }
    } /* Endwhile */

} /* Endbody */

/******************** Begin Testcases **********************/
LWEVENT_STRUCT_PTR lwevent_ptr;

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test _lwevent_create function
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   _mqx_uint       result;

   _time_set_hwtick_function(0,0);
   result = _lwevent_create(&lwevent, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.1: Create the lwevent operation")
   _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: _lwevent_wait_ticks
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
   MQX_TICK_STRUCT start_ticks;
   MQX_TICK_STRUCT end_ticks;
   MQX_TICK_STRUCT diff_ticks;
   _mqx_uint       result, delay;

   delay = 5;
   _int_disable();
   _time_delay_ticks(1);
   _time_get_elapsed_ticks(&start_ticks);

   result = _lwevent_wait_ticks(lwevent_ptr, 1, TRUE, delay);
   _time_get_elapsed_ticks(&end_ticks);
   _int_enable();
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == LWEVENT_WAIT_TIMEOUT, "Test #2: Testing 2.1: wait lwevent operation result should be LWEVENT_WAIT_TIMEOUT")
   _task_set_error(MQX_OK);

   _time_diff_ticks(&end_ticks, &start_ticks, &diff_ticks);
   result = PSP_GET_ELEMENT_FROM_TICK_STRUCT(&diff_ticks,0);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == delay, "Test #2: Testing 2.2: Lwevent wait ticks")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: _lwevent_wait_for
* Requirements :
*   LWEVENT018, LWEVENT019
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;
    MQX_TICK_STRUCT diff_ticks;
    MQX_TICK_STRUCT ticks;
    _mqx_uint       result, delay;
    /* Test _lwevent_wait_for() which will fail if calling from ISR */
    _mqx_uint in_isr_value;
    _mqx_uint valid_value;
    _int_disable();
    in_isr_value = get_in_isr();
    set_in_isr(1);
    result=_lwevent_wait_for(&lwevent,1, TRUE, NULL);
    set_in_isr(in_isr_value);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #3: Testing 3.1: _lwevent_wait_for() fails and returns MQX_CANNOT_CALL_FUNCTION_FROM_ISR");
    /* Test _lwevent_wait_for() which shall return the error MQX_LWEVENT_INVALID if lightweight event group is no longer valid or was never valid */
    valid_value=lwevent.VALID;
    lwevent.VALID=valid_value+1;
    result=_lwevent_wait_for(&lwevent,1, TRUE, NULL);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_LWEVENT_INVALID, "Test #3: Testing 3.2: _lwevent_wait_for() fails and returns MQX_LWEVENT_INVALID");
    lwevent.VALID=valid_value;

    ticks = _mqx_zero_tick_struct;
    delay = 20;
    _time_add_msec_to_ticks(&ticks, delay);
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    result = _lwevent_wait_for(lwevent_ptr, 1, TRUE, &ticks);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == LWEVENT_WAIT_TIMEOUT, "Test #3: Testing 3.3: _lwevent_wait_for operation result should be LWEVENT_WAIT_TIMEOUT")
    _task_set_error(MQX_OK);
    _time_diff_ticks(&end_ticks, &start_ticks, &diff_ticks);
    result=time_check(diff_ticks.TICKS[0],ticks.TICKS[0],1);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == TRUE, "Test #3: Testing 3.4: _lwevent_wait_for operation")
    if (result != TRUE) {
        _psp_print_ticks((PSP_TICK_STRUCT_PTR)&start_ticks);
        _psp_print_ticks((PSP_TICK_STRUCT_PTR)&end_ticks);
    } /* Endif */
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4: _lwevent_wait_until
* Requirements :
*   LWEVENT026, LWEVENT027
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;
    _mqx_uint       result, delay;
    /* Test _lwevent_wait_until() which will fail if calling from ISR */
    _mqx_uint in_isr_value;
    _mqx_uint valid_value;
    _int_disable();
    in_isr_value = get_in_isr();
    set_in_isr(1);
    result=_lwevent_wait_until(&lwevent,1, TRUE, NULL);
    set_in_isr(in_isr_value);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #4: Testing 4.1: _lwevent_wait_until() fails and returns MQX_CANNOT_CALL_FUNCTION_FROM_ISR");
    /* Test _lwevent_wait_until() which shall return the error MQX_LWEVENT_INVALID if lightweight event group is no longer valid or was never valid */
    valid_value=lwevent.VALID;
    lwevent.VALID=valid_value+1;
    result=_lwevent_wait_until(&lwevent,1, TRUE, NULL);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_LWEVENT_INVALID, "Test #4: Testing 4.2: _lwevent_wait_until() fails and returns MQX_LWEVENT_INVALID");
    lwevent.VALID=valid_value;

    delay = 20;
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    _time_add_msec_to_ticks(&start_ticks, delay);
    result = _lwevent_wait_until(lwevent_ptr, 1, TRUE, &start_ticks);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == LWEVENT_WAIT_TIMEOUT, "Test #4: Testing 4.3: _lwevent_wait_until operation Result should be LWEVENT_WAIT_TIMEOUT")
    _task_set_error(MQX_OK);

    result = PSP_CMP_TICKS(&start_ticks, &end_ticks);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == 0, "Test #4: Testing 4.4: Check lwevent wait until")

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5: waiting in task
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    _task_id        tid;

    _time_delay_ticks(1);
    tid = _task_create(0, LWEVENT_TASK, 0);
    _lwevent_set(lwevent_ptr,1);
    _time_delay_ticks(14);

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, lwevent_count == 1, "Test #5: Testing 5.1: Lwevent set incorrect")
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, lwevent_timeout != 0, "Test #5: Testing 5.2: Lwevent wait timeout incorrect")

    _lwevent_destroy(lwevent_ptr);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_get_td(tid) == NULL, "Test #5: Testing 5.3: Lwevent destroy incorrect operation")

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6: _lwevent_clear
* Requirements :
*   LWEVENT001, LWEVENT002
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
   _mqx_uint result;
   _mqx_uint valid_value;
   _lwevent_create(&lwevent, 0);
   result=_lwevent_clear(lwevent_ptr,1);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.1: Lwevent Clears the specified event bits")
   valid_value=lwevent.VALID;
   lwevent.VALID=valid_value+1;
   result=_lwevent_clear(lwevent_ptr,1);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_LWEVENT_INVALID, "Test #6: Testing 6.2: _lwevent_clear() fails and returns MQX_LWEVENT_INVALID")
   valid_value=lwevent.VALID;
   lwevent.VALID=valid_value-1;
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_2)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Testing _lwevent_create"),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Testing _lwevent_wait_ticks"),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Testing _lwevent_wait_for"),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - Testing _lwevent_wait_until"),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - Testing waiting in task"),
    EU_TEST_CASE_ADD(tc_6_main_task, " Test #6 - Testing _lwevent_create")
EU_TEST_SUITE_END(suite_2)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_2, " - Lightweight event Time Specific Tests.")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*-------------------------------------------------------------------
*
* Task Name : main_task
* Comments  : This task test the light weight semaphores.
*
*END*----------------------------------------------------------------------*/
void main_task
   (
      uint32_t dummy
   )
{ /* Body */
    lwevent_ptr = &lwevent;
   _int_install_unexpected_isr();
    //test_initialize();
    //EU_RUN_ALL_TESTS(test_result_array);
    //test_stop();
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
} /* Endbody */

/* EOF */


