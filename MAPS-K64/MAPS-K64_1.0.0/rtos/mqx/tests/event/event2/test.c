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
* $Version : 3.8.7.0$
* $Date    : Aug-14-2012$
*
* Comments:
*
*   This file contains the source functions for the demo of the
*   time slicing functions of the kernel.
*
*END************************************************************************/

#include <mqx.h>
#include <mqx_inc.h>
#include <bsp.h>
#include <name.h>
#include <name_prv.h>
#include <event.h>
#include <event_prv.h>
#include <util.h>

#include "util.h"
#include <string.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#if !MQX_CHECK_ERRORS
    #error "MQX_CHECK_ERRORS have to be enabled"
#endif

#if !MQX_CHECK_VALIDITY
    #error "MQX_CHECK_VALIDITY have to be enabled"
#endif

#if !MQX_USE_EVENTS
    #error "MQX_USE_EVENTS have to be enabled"
#endif

#define TIME_DELAY_TOLERANCE (1000/BSP_ALARM_FREQUENCY)

#define FILE_BASENAME   test
#define MAIN_TASK       10

extern void main_task(uint32_t);

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
{ MAIN_TASK,       main_task,       2000,  7, "Main",       MQX_AUTO_START_TASK},
{ 0,               0,               0,     0, 0,            0}
};

_task_id   main_id = 0;
char       string[128];
char       event_name[] = "event.one";
_mqx_uint  event_timeout   = 20;

//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);// Test #1 - Creating and opening events
void tc_2_main_task(void);// Test #2 - Testing event_wait_any_ticks
void tc_3_main_task(void);// Test #3 - Testing event_wait_all_ticks
void tc_4_main_task(void);// Test #4 - Testing event_wait_any_for
void tc_5_main_task(void);// Test #5 - Testing event_wait_all_for
void tc_6_main_task(void);// Test #6 - Testing event_wait_any_until
void tc_7_main_task(void);// Test #7 - Testing event_wait_all_until
void tc_8_main_task(void);// Test #8 - Testing event_wait_any
void tc_9_main_task(void);// Test #9 - Testing event_wait_all
//------------------------------------------------------------------------

/*******************    Variables for tasks */
void       *event_ptr;
_mqx_uint   delay;
   
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST #1: Creating and opening events
//
//END---------------------------------------------------------------------

void tc_1_main_task(void)
{
    _mqx_uint       result;
    _int_install_unexpected_isr();

    main_id = _task_get_id();

    result = _event_create_component( 2, 1,4 );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.1: create_event_component operation")
    _task_set_error(MQX_OK);

    result =  _event_create( event_name );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.2: _event_create operation")
    _task_set_error(MQX_OK);

    result = _event_open( event_name, &event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.3: _event_open operation")
    _task_set_error(MQX_OK);
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST #2: Testing event_wait_any_ticks
//
//END---------------------------------------------------------------------

void tc_2_main_task()
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;
    MQX_TICK_STRUCT diff_ticks;

    _mqx_uint       result;
    delay = 5;
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    result = _event_wait_any_ticks(event_ptr, 0x1, delay);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == EVENT_WAIT_TIMEOUT), "Test #2 Testing: 2.1: _event_wait_any_ticks should have returned EVENT_WAIT_TIMEOUT")
    _task_set_error(MQX_OK);

    _time_diff_ticks(&end_ticks, &start_ticks, &diff_ticks);

    result = PSP_GET_ELEMENT_FROM_TICK_STRUCT(&diff_ticks,0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == delay), "Test #2 Testing: 2.2: _event_wait_any_ticks time should be correct")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST #3: Testing event_wait_all_ticks
//
//END---------------------------------------------------------------------

void tc_3_main_task(void)
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;
    MQX_TICK_STRUCT diff_ticks;

    _mqx_uint       result;
    delay = 5;
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    result = _event_wait_all_ticks(event_ptr, 0x1, delay);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == EVENT_WAIT_TIMEOUT), "Test #3 Testing: 3.1: _event_wait_all_ticks should have returned EVENT_WAIT_TIMEOUT")
    _task_set_error(MQX_OK);

    _time_diff_ticks(&end_ticks, &start_ticks, &diff_ticks);

    result = PSP_GET_ELEMENT_FROM_TICK_STRUCT(&diff_ticks,0);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == delay), "Test #3 Testing: 3.2: _event_wait_all_ticks time should be correct")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST #4: Testing event_wait_any_for
//
//END---------------------------------------------------------------------

void tc_4_main_task(void)
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;
    MQX_TICK_STRUCT diff_ticks;
    MQX_TICK_STRUCT ticks;
    _mqx_uint       result;
    ticks = _mqx_zero_tick_struct;
    delay = 20;
    _time_add_msec_to_ticks(&ticks, delay);

    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    result = _event_wait_any_for(event_ptr, 0x1, &ticks);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == EVENT_WAIT_TIMEOUT), "Test #4 Testing: 4.1: _event_wait_any_for should have returned EVENT_WAIT_TIMEOUT")
    _task_set_error(MQX_OK);

    _time_diff_ticks(&end_ticks, &start_ticks, &diff_ticks);

    result = (unsigned)PSP_CMP_TICKS(&diff_ticks, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == 0), "Test #4 Testing: 4.2: _event_wait_any_for time should be correct")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_5_main_task
// Comments     : TEST #5: Testing event_wait_all_for
//
//END---------------------------------------------------------------------

void tc_5_main_task(void)
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;
    MQX_TICK_STRUCT diff_ticks;
    MQX_TICK_STRUCT ticks;
    _mqx_uint       result;
    ticks = _mqx_zero_tick_struct;
    _time_add_msec_to_ticks(&ticks, delay);

    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    result = _event_wait_all_for(event_ptr, 0x1, &ticks);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == EVENT_WAIT_TIMEOUT), "Test #5 Testing: 5.1: _event_wait_all_for should have returned EVENT_WAIT_TIMEOUT")
    _task_set_error(MQX_OK);

    _time_diff_ticks(&end_ticks, &start_ticks, &diff_ticks);

    result = PSP_CMP_TICKS(&diff_ticks, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == 0), "Test #5 Testing: 5.2: _event_wait_all_for time should be correct")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_6_main_task
// Comments     : TEST #6: Testing event_wait_any_until
//
//END---------------------------------------------------------------------

void tc_6_main_task(void)
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;
    _mqx_uint       result;
    delay = 20;
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    _time_add_msec_to_ticks(&start_ticks, delay);
    result = _event_wait_any_until(event_ptr, 0x1, &start_ticks);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == EVENT_WAIT_TIMEOUT), "Test #6 Testing: 6.1: _event_wait_any_until should have returned EVENT_WAIT_TIMEOUT")
    _task_set_error(MQX_OK);

    result = PSP_CMP_TICKS(&start_ticks, &end_ticks);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == 0), "Test #6 Testing: 6.2: _event_wait_any_until time should be correct")
    if (result != 0) {
        _psp_print_ticks((PSP_TICK_STRUCT_PTR)&start_ticks);
        _psp_print_ticks((PSP_TICK_STRUCT_PTR)&end_ticks);
    } /* Endif */
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_7_main_task
// Comments     : TEST #7: Testing event_wait_all_until
//
//END---------------------------------------------------------------------

void tc_7_main_task(void)
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;
    _mqx_uint       result;
    delay = 20;
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    _time_add_msec_to_ticks(&start_ticks, delay);
    result = _event_wait_all_until(event_ptr, 0x1, &start_ticks);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == EVENT_WAIT_TIMEOUT), "Test #7 Testing: 7.1: _event_wait_all_until should have returned EVENT_WAIT_TIMEOUT")
    _task_set_error(MQX_OK);

    result = PSP_CMP_TICKS(&start_ticks, &end_ticks);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == 0), "Test #7 Testing: 7.2: _event_wait_all_until time should be correct")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_8_main_task
// Comments     : TEST #8: Testing event_wait_any
//
//END---------------------------------------------------------------------

void tc_8_main_task(void)
{
    TIME_STRUCT start_time;
    TIME_STRUCT end_time;
    TIME_STRUCT diff_time;
    _mqx_uint   result;
    delay = 20;
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed(&start_time);
    result = _event_wait_any(event_ptr, 0x1, (uint32_t)delay);
    _time_get_elapsed(&end_time);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == EVENT_WAIT_TIMEOUT), "Test #8 Testing: 8.1: _event_wait_any should have returned EVENT_WAIT_TIMEOUT")
    _task_set_error(MQX_OK);

    _time_diff(&start_time, &end_time, &diff_time);

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, ( (diff_time.MILLISECONDS >= (uint32_t)delay) && (diff_time.MILLISECONDS <= ((uint32_t)delay + TIME_DELAY_TOLERANCE )) && (diff_time.SECONDS == 0) ), "Test #8 Testing: 8.2: _event_wait_any time should be correct")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_9_main_task
// Comments     : TEST #9: Testing event_wait_all
//
//END---------------------------------------------------------------------

void tc_9_main_task(void)
{
    TIME_STRUCT start_time;
    TIME_STRUCT end_time;
    TIME_STRUCT diff_time;
    _mqx_uint       result;
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed(&start_time);
    result = _event_wait_all(event_ptr, 0x1, (uint32_t)delay);
    _time_get_elapsed(&end_time);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == EVENT_WAIT_TIMEOUT), "Test #9 Testing: 9.1: _event_wait_all should have returned EVENT_WAIT_TIMEOUT")
    _task_set_error(MQX_OK);

    _time_diff(&start_time, &end_time, &diff_time);

    EU_ASSERT_REF_TC_MSG(tc_9_main_task, ( (diff_time.MILLISECONDS >= (uint32_t)delay) && (diff_time.MILLISECONDS <= ((uint32_t)delay + TIME_DELAY_TOLERANCE )) && (diff_time.SECONDS == 0) ), "Test #9 Testing: 9.2: _event_wait_all time should be correct")
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_2)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Testing create and open events"),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Testing event_wait_any_ticks"),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Testing event_wait_all_ticks"),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - Testing event_wait_any_for"),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - Testing event_wait_all_for"),
    EU_TEST_CASE_ADD(tc_6_main_task, " Test #6 - Testing event_wait_any_until"),
    EU_TEST_CASE_ADD(tc_7_main_task, " Test #7 - Testing event_wait_all_until"),
    EU_TEST_CASE_ADD(tc_8_main_task, " Test #8 - Testing event_wait_any"),
    EU_TEST_CASE_ADD(tc_9_main_task, " Test #9 - Testing event_wait_all")
EU_TEST_SUITE_END(suite_2)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_2, " - Event Component Time Specific Tests")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*-------------------------------------------------------------------
* 
* Task Name : main_task
* Comments  : This task test the event component.
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t dummy
   )
{
    // Don't include HW_TICKS
    _time_set_hwtick_function(0,0);
    //test_initialize();
    eunit_mutex_init();
    //EU_RUN_ALL_TESTS(test_result_array);
    EU_RUN_ALL_TESTS(eunit_tres_array);
    //test_stop();
    eunit_test_stop();
}


/* EOF */
