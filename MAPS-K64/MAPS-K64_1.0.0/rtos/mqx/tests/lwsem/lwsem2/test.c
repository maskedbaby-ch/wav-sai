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
*   This file contains the source functions for the demo of the
*   time slicing functions of the kernel.
* Requirements:
*   LWSEM016, LWSEM017, LWSEM021, LWSEM022, LWSEM026, LWSEM027
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <util.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test


#define MAIN_TASK       10
#define LWSEM_TASK       11

extern char   *_mqx_get_task_error_code_string(uint32_t);

extern void main_task(uint32_t);
extern void lwsem_task(uint32_t);

//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);// Test #1 - Testing _lwsem_create
void tc_2_main_task(void);// Test #2 - Testing _lwsem_wait_ticks
void tc_3_main_task(void);// Test #3 - Testing _lwsem_wait_for
void tc_4_main_task(void);// Test #4 - Testing _lwsem_wait_until
void tc_5_main_task(void);// Test #5 - Testing waiting in task
//------------------------------------------------------------------------

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
{ MAIN_TASK,       main_task,       2000,  7, "Main",       MQX_AUTO_START_TASK},
{ LWSEM_TASK,      lwsem_task,      2000,  6, "LWS",        0},
{ 0,               0,               0,     0, 0,            0}
};

LWSEM_STRUCT lwsem;

volatile _mqx_uint lwsem_count = 0;
volatile _mqx_uint lwsem_timeout = 0;

/*TASK*-------------------------------------------------------------------
*
* Task Name : lwsem_task
* Comments  : This task waits on a lwsem.
*
*END*----------------------------------------------------------------------*/

void lwsem_task
   (
      uint32_t dummy
   )
{ /* Body */
    _mqx_uint result;

    while (TRUE) {
        result = _lwsem_wait_ticks(&lwsem, 5);
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, ((result == MQX_OK) || (result == MQX_LWSEM_WAIT_TIMEOUT) || (result == MQX_INVALID_LWSEM)), "Test #5 Testing: _lwsem_wait_ticks should be either MQX_OK or MQX_LWSEM_WAIT_TIMEOUT or MQX_INVALID_LWSEM")
        if (result == MQX_OK) {
            lwsem_count++;
        } else if (result == MQX_LWSEM_WAIT_TIMEOUT) {
            lwsem_timeout++;
        } else if (result == MQX_INVALID_LWSEM) {
            _task_destroy(0);
        } else {
        } /* Endif */
    } /* Endwhile */

} /* Endbody */


//----------------- Begin Testcases --------------------------------------
LWSEM_STRUCT_PTR lwsem_ptr = NULL;

//TASK--------------------------------------------------------------------
//
// Task Name    : tc_1_main_task
// Comments     : TEST #1: Testing _lwsem_create
//
//END---------------------------------------------------------------------

void tc_1_main_task(void)
{
    _mqx_uint       result;

    result = _lwsem_create(&lwsem, 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: _lwsem_create operation")
    _task_set_error(MQX_OK);
}

/*TASK--------------------------------------------------------------------
 Task Name    : tc_2_main_task
 Comments     : TEST #2: Testing _lwsem_wait_ticks
 Requirements : LWSEM021, LWSEM022
END---------------------------------------------------------------------*/

void tc_2_main_task(void)
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;
    MQX_TICK_STRUCT diff_ticks;
    _mqx_uint       result, delay;
    _mqx_uint in_isr_value;
    _mqx_uint valid_value;
    /* Test _lwsem_wait_ticks() which will fail if calling from ISR */
    _int_disable();
    in_isr_value = get_in_isr();
    set_in_isr(1);
    result=_lwsem_wait_ticks(&lwsem,0);
    set_in_isr(in_isr_value);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #2: Testing 2.1: _lwsem_wait_ticks() fails and returns MQX_CANNOT_CALL_FUNCTION_FROM_ISR");
    /* Test _lwsem_wait_ticks() which will fail if the lightweight semaphore pointed by lwsem_ptr is not valid */
    valid_value=lwsem.VALID;
    lwsem.VALID=valid_value+1;
    result=_lwsem_wait_ticks(&lwsem,0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_INVALID_LWSEM, "Test #2: Testing 2.2: _lwsem_wait_ticks() fails and returns MQX_INVALID_LWSEM");
    lwsem.VALID=valid_value;
    /* Test _lwsem_wait_ticks() which fail if timeout expired before the task could get the lightweight semaphore */
    delay = 5;
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    result = _lwsem_wait_ticks(lwsem_ptr, delay);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_LWSEM_WAIT_TIMEOUT), "Test #2 Testing: 2.3: _time_get_elapsed_ticks should have returned MQX_LWSEM_WAIT_TIMEOUT")

    _task_set_error(MQX_OK);

    _time_diff_ticks(&end_ticks, &start_ticks, &diff_ticks);

    result = PSP_GET_ELEMENT_FROM_TICK_STRUCT(&diff_ticks,0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == delay), "Test #2 Testing: 2.4: Lwsem wait ticks time is correct")


}

/*TASK--------------------------------------------------------------------
 Task Name    : tc_3_main_task
 Comments     : TEST #3: Testing _lwsem_wait_for
 Requirements : LWSEM016, LWSEM017
END---------------------------------------------------------------------*/

void tc_3_main_task(void)
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;
    MQX_TICK_STRUCT diff_ticks;
    MQX_TICK_STRUCT ticks;
    _mqx_uint       result, delay;
    _mqx_uint in_isr_value;
    _mqx_uint valid_value;
    /* Test _lwsem_wait_for() which will fail if calling from ISR */
    _int_disable();
    in_isr_value = get_in_isr();
    set_in_isr(1);
    result=_lwsem_wait_for(&lwsem,NULL);
    set_in_isr(in_isr_value);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #3: Testing 3.1: _lwsem_wait_for() fails and returns MQX_CANNOT_CALL_FUNCTION_FROM_ISR");
    /* Test _lwsem_wait_for() which will fail if the lightweight semaphore pointed by lwsem_ptr is not valid */
    valid_value=lwsem.VALID;
    lwsem.VALID=valid_value+1;
    result=_lwsem_wait_for(&lwsem,NULL);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_INVALID_LWSEM , "Test #3: Testing 3.2: _lwsem_wait_for() fails and returns MQX_INVALID_LWSEM");
    lwsem.VALID=valid_value;
    /* Test _lwsem_wait_for() which fail if timeout expired before the task could get the lightweight semaphore */
    ticks = _mqx_zero_tick_struct;
    delay = 20;
    _time_add_msec_to_ticks(&ticks, delay);
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    result = _lwsem_wait_for(lwsem_ptr, &ticks);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_LWSEM_WAIT_TIMEOUT), "Test #3 Testing: 3.3: _lwsem_wait_for operation")

    _task_set_error(MQX_OK);

    _time_diff_ticks(&end_ticks, &start_ticks, &diff_ticks);

    result = (unsigned)PSP_CMP_TICKS(&diff_ticks, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == 0), "Test #3 Testing: 3.4: Lwsem wait for is correct")
}

/*TASK--------------------------------------------------------------------
 Task Name    : tc_4_main_task
 Comments     : TEST #4: Testing _lwsem_wait_until
 Requirements : LWSEM026, LWSEM027
END---------------------------------------------------------------------*/

void tc_4_main_task(void)
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;
    _mqx_uint       result, delay;
    _mqx_uint in_isr_value;
    _mqx_uint valid_value;
    /* Test _lwsem_wait_until() which will fail if calling from ISR */
    _int_disable();
    in_isr_value = get_in_isr();
    set_in_isr(1);
    result=_lwsem_wait_until(&lwsem,NULL);
    set_in_isr(in_isr_value);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #4: Testing 4.1: _lwsem_wait_until() fails and returns MQX_CANNOT_CALL_FUNCTION_FROM_ISR");
    /* Test _lwsem_wait_until() which will fail if the lightweight semaphore pointed by lwsem_ptr is not valid */
    valid_value=lwsem.VALID;
    lwsem.VALID=valid_value+1;
    result=_lwsem_wait_until(&lwsem,NULL);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_INVALID_LWSEM, "Test #4: Testing 4.2: _lwsem_wait_until() fails and returns MQX_INVALID_LWSEM");
    lwsem.VALID=valid_value;
    /* Test _lwsem_wait_until() which fail if timeout expired before the task could get the lightweight semaphore */
   delay = 20;
   _int_disable();
   _time_delay_ticks(1);
   _time_get_elapsed_ticks(&start_ticks);
   _time_add_msec_to_ticks(&start_ticks, delay);
   result = _lwsem_wait_until(lwsem_ptr, &start_ticks);
   _time_get_elapsed_ticks(&end_ticks);
   _int_enable();
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_LWSEM_WAIT_TIMEOUT), "Test #4 Testing: 4.3: _lwsem_wait_until operation")

   _task_set_error(MQX_OK);

   result = PSP_CMP_TICKS(&start_ticks, &end_ticks);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == 0), "Test #4 Testing: 4.4: _lwsem_wait_until time is correct")
}

//TASK--------------------------------------------------------------------
//
// Task Name    : tc_5_main_task
// Comments     : TEST #5: Testing waiting in task
//
//END---------------------------------------------------------------------

void tc_5_main_task(void)
{
    _task_id        tid;
    void           *td;

    _time_delay_ticks(1);
    tid = _task_create(0, LWSEM_TASK, 0);
    _lwsem_post(lwsem_ptr);
    _lwsem_post(lwsem_ptr);

    _time_delay_ticks(14);

    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (lwsem_count == 2), "Test #5 Testing: 5.1: Lwsem post is correct")
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (lwsem_timeout != 0), "Test #5 Testing: 5.2: Lwsem wait timeout is correct")

    _lwsem_destroy(lwsem_ptr);

    td = _task_get_td(tid);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (td == NULL), "Test #5 Testing: 5.3: ")
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_2)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Testing _lwsem_create"),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Testing _lwsem_wait_ticks"),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Testing _lwsem_wait_for"),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - Testing _lwsem_wait_until"),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - Testing waiting in task")
EU_TEST_SUITE_END(suite_2)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_2, " - Lightweight semaphore Time Specific Tests")
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
    lwsem_ptr = &lwsem;

    // marks@5/20/03 Mask skew in tick differences that are caused by partial ticks
    /* Start CR 1148 */
    _time_set_hwtick_function(0, (void *)NULL);
    /* End   CR 1148 */

    _int_install_unexpected_isr();

   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* Endbody */

/* EOF */

