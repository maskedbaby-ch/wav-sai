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
* $Version : 4.0.1.0$
* $Date    : Jun-1-2013$
*
* Comments   : This file contains the kernel test functions for the event
*              component.
*
* Requirement: EVENT020,EVENT022,EVENT027,EVENT028,EVENT029,EVENT045,EVENT105,
*              EVENT106,EVENT107,EVENT109,EVENT110,EVENT111,EVENT112,EVENT113,
*              EVENT119,EVENT123,EVENT124,EVENT125,EVENT126,EVENT128,EVENT130,
*              EVENT131,EVENT134,EVENT135,EVENT136,EVENT137,EVENT138,EVENT140,
*              EVENT142,EVENT144.EVENT147,EVENT148,EVENT149,EVENT150,EVENT152.
*
*END***********************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <name.h>
#include <name_prv.h>
#include <event.h>
#include <event_prv.h>
#include <string.h>
#include "event_util.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define MAIN_TASK         10
#define TEST_TASK         11

void    main_task(uint32_t);
void    wait_task(uint32_t);

void   *event_global_ptr;
char    event_name[128] = "";
char    event_global[128] = "";

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    { MAIN_TASK,  main_task, 3000,  9, "Main",  MQX_AUTO_START_TASK,  0},
    { TEST_TASK,  wait_task, 2000, 10, "wait",                    0    },
    {         0,          0,    0,  0,      0,                    0,  0}
};

void tc_1_main_task(void);/* TEST #1: Creating and opening events                             */
void tc_2_main_task(void);/* TEST #2: Testing function _event_wait_all in special cases       */
void tc_3_main_task(void);/* TEST #3: Testing function _event_wait_all_for in special cases   */
void tc_4_main_task(void);/* TEST #4: Testing function _event_wait_all_until in special cases */
void tc_5_main_task(void);/* TEST #5: Testing function _event_wait_any in special cases       */
void tc_6_main_task(void);/* TEST #6: Testing function _event_wait_any_for in special cases   */
void tc_7_main_task(void);/* TEST #7: Testing function _event_wait_any_ticks in special cases */
void tc_8_main_task(void);/* TEST #8: Testing function _event_wait_any_until in special cases */
/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Creating and opening events
*
* Requirement  : EVENT020,EVENT022,EVENT045.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint result;

    /* Calling function _event_create_component tocCreate event component with unlimited number of event groups.
    Checking the returned value must be MQX_OK to verify the event component was created */
    result = _event_create_component(2, 1, 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing 1.00: create event component operation");
    _task_set_error(MQX_OK);

    /* Calling function _event_create to create the named event group.
    Checking the returned value must be MQX_OK to verify the event group was created */
    result =  _event_create(event_global);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing 1.01: _event_create operation");
    _task_set_error(MQX_OK);

    /* Calling function _event_open to opens a connection to the event group
    to verify the returned value must be MQX_OK */
    result = _event_open(event_global, &event_global_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing 1.02: _event_open operation");
    _task_set_error(MQX_OK);

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Testing function _event_wait_all in special cases
*
* Requirement  : EVENT020,EVENT022,EVENT027,EVENT029,EVENT105,EVENT106,EVENT107,EVENT110.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _task_id  tid;
    _mqx_uint temp;
    _mqx_uint result;
    void     *event_ptr;

    /* Save the current isr counting value */
    temp = get_in_isr();
    /* Set the current task as an isr */
    set_in_isr(1);
    /* Calling function _event_wait_all to verify the function must return MQX_CANNOT_CALL_FUNCTION_FROM_ISR
    because the function cannot be called from an ISR */
    result = _event_wait_all(event_global_ptr, 4, 20);
    set_in_isr(temp);
    _int_enable();
    /* Verify result must be set to MQX_CANNOT_CALL_FUNCTION_FROM_ISR */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR), "Test #2 Testing 2.00: Calling _event_wait_all in ISR should be failed");
    _task_set_error(MQX_OK);

    /* Calling function get_event_handle_valid_field to backup value of VALID field of event group handle */
    temp = get_event_handle_valid_field(event_global_ptr);
    /* Calling function set_event_handle_valid_field to make event group handle to invalid */
    set_event_handle_valid_field(event_global_ptr, 0);

    /* Calling function _event_wait_all when the event group handle is not valid
    to verify the function must return EVENT_INVALID_EVENT_HANDLE */
    result = _event_wait_all( event_global_ptr, 4, 20 );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == EVENT_INVALID_EVENT_HANDLE), "Test #2 Testing 2.01: Tests function _event_wait_all when handle is not a valid event group handle");
    _task_set_error(MQX_OK);

    /* Calling function set_event_handle_valid_field to restore value of VALID field of event group handle to original value */
    set_event_handle_valid_field(event_global_ptr,temp);

    /* Calling function get_event_valid_field to backup value of VALID field of event group */
    temp = get_event_valid_field(event_global_ptr);
    /* Calling function set_event_valid_field to make event group to invalid */
    set_event_valid_field(event_global_ptr, 0);

    /* Calling function _event_wait_all when event group is not valid
    to verify the returned value must be EVENT_INVALID_EVENT */
    result = _event_wait_all(event_global_ptr, 4, 20);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == EVENT_INVALID_EVENT), "Test #2 Testing 2.02: Tests function _event_wait_all when event group is not valid");
    _task_set_error(MQX_OK);

    /* Calling function set_event_valid_field to restore event group to original value */
    set_event_valid_field(event_global_ptr,temp);

    /* Calling function _event_set to set the specified event bits in the event group
    to verify the function must return MQX_OK */
    result = _event_set(event_global_ptr, 0x04);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing 2.03: Tests operation of function _event_set");
    _task_set_error(MQX_OK);

    /* Calling function _event_wait_all with all of parameter is valid
    to verify the returned value must be MQX_OK */
    result = _event_wait_all(event_global_ptr, 0x04, 20);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing 2.04: Tests operation of function _event_wait_all");
    _task_set_error(MQX_OK);

    /* Calling function _event_create to create the named event group.
    Checking the returned value must be MQX_OK to verify the event group was created */
    result = _event_create(event_name);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing 2.05: Tests operation of function _event_create");
    _task_set_error(MQX_OK);

    /* Calling function _event_open to opens a connection to the event group
    to verify the returned value must be MQX_OK */
    result = _event_open(event_name, &event_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing 2.06: Tests operation of function _event_open");
    _task_set_error(MQX_OK);

    /* Creates task that will destroy the event group */
    tid = _task_create(0, TEST_TASK, 0);

    /* Calling function _event_wait_all when event name will be destroyed while waiting
    to verify the function must return EVENT_DELETED */
    result = _event_wait_all(event_ptr, 4, 20);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == EVENT_DELETED), "Test #2 Testing 2.07: Tests function _event_wait_all when event name will be destroyed while waiting");
    _task_set_error(MQX_OK);

    /* Destroys TEST_TASK */
    _task_destroy(tid);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Testing function _event_wait_all_for operation
*
* Requirement  : EVENT020,EVENT022,EVENT027,EVENT109,EVENT111,EVENT112,EVENT113,EVENT119.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    _task_id         tid;
    _mqx_uint        temp;
    _mqx_uint        result;
    void            *event_ptr;
    MQX_TICK_STRUCT  ticks;

    /* Set the alarm */
    _time_init_ticks(&ticks, 10);

    /* Save the current isr counting value */
    temp = get_in_isr();
    /* Set the current task as an isr */
    set_in_isr(1);
    /* Calling function _event_wait_all to verify the function must return MQX_CANNOT_CALL_FUNCTION_FROM_ISR
    because the function cannot be called from an ISR */
    result = _event_wait_all_for( event_global_ptr, 4, &ticks );
    set_in_isr(temp);
    _int_enable();
    /* Verify result must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR), "Test #3 Testing 3.00: Calling _event_wait_all_for in ISR should be failed");
    _task_set_error(MQX_OK);

    /* Calling function to backup value of VALID field of event group handle */
    temp = get_event_handle_valid_field(event_global_ptr);
    /* Calling function set_event_handle_valid_field to make event group handle to invalid */
    set_event_handle_valid_field(event_global_ptr, 0);

    /* Calling function _event_wait_all_for when unable the event group handle is not valid
    to verify the returned value must be EVENT_INVALID_EVENT_HANDLE */
    result = _event_wait_all_for( event_global_ptr, 4, &ticks );
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == EVENT_INVALID_EVENT_HANDLE), "Test #3 Testing 3.01: Tests function _event_wait_all_for when unable to get the event group handle");
    _task_set_error(MQX_OK);

    /* Calling set_event_handle_valid_field to restore value of VALID field of event group handle to original value */
    set_event_handle_valid_field(event_global_ptr,temp);

    /* Calling function get_event_valid_field to backup value of VALID field of event group */
    temp = get_event_valid_field(event_global_ptr);
    /* Calling function set_event_valid_field to make event group to invalid */
    set_event_valid_field(event_global_ptr, 0);

    /* Calling function _event_wait_all_for when event group is not valid
    to verify the function must return EVENT_INVALID_EVENT */
    result = _event_wait_all_for( event_global_ptr, 4, &ticks );
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == EVENT_INVALID_EVENT), "Test #3 Testing 3.02: _event_wait_all_ticks with invalid event should have returned EVENT_INVALID_EVENT");
    _task_set_error(MQX_OK);

    /* Calling function set_event_valid_field to restore event group to original value */
    set_event_valid_field(event_global_ptr,temp);
    _task_set_error(MQX_OK);

    /* Calling function _event_set to set the specified event bits in the event group
    to verify the function must return MQX_OK */
    result = _event_set( event_global_ptr, 0x04 );
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing 3.03: Tests operation of function _event_set");
    _task_set_error(MQX_OK);

    /* Calling function _event_wait_all_for with all of parameter is valid
    to verify the returned value must be MQX_OK */
    result = _event_wait_all_for( event_global_ptr, 4, &ticks );
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing 3.04: Tests operation of function _event_wait_all_for");
    _task_set_error(MQX_OK);

    /* Calling function _event_create to create the named event group.
    Checking the returned value must be MQX_OK to verify the event group was created */
    result = _event_create(event_name);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing 3.05: Tests operation of function _event_create");
    _task_set_error(MQX_OK);

    /* Calling function _event_open to opens a connection to the event group
    to verify the returned value must be MQX_OK */
    result = _event_open(event_name, &event_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing 3.06: Tests operation of function _event_open");
    _task_set_error(MQX_OK);

    /* Creates task that will destroy the event group */
    tid = _task_create(0, TEST_TASK, 0);

    /* Calling function _event_wait_all_for when event name will be destroyed while waiting
    to verify the function must return EVENT_DELETED */
    result = _event_wait_all_for( event_ptr, 4, &ticks  );
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == EVENT_DELETED), "Test #3 Testing 3.07: Wait for all the specified event bits when event name will be destroyed while waiting");
    _task_set_error(MQX_OK);

    /* Destroy TEST_TASK */
    _task_destroy(tid);

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Testing function _event_wait_all_until operation
*
* Requirement  : EVENT020,EVENT022,EVENT027,EVENT123,EVENT124,EVENT125,EVENT126,EVENT128.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    _task_id         tid;
    _mqx_uint        temp;
    _mqx_uint        result;
    void            *event_ptr;
    MQX_TICK_STRUCT  ticks;

    /* Set the alarm */
    _time_init_ticks(&ticks, 10);

    /* Save the current isr counting value */
    temp = get_in_isr();
    /* Set the current task as an isr */
    set_in_isr(1);
    /* Calling function _event_wait_all to verify the function must return MQX_CANNOT_CALL_FUNCTION_FROM_ISR
    because the function cannot be called from an ISR */
    result = _event_wait_all_until( event_global_ptr, 4, &ticks );
    set_in_isr(temp);
    _int_enable();
    /* Verify result must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR), "Test #4 Testing 4.00: Calling _event_wait_all_until in ISR should be failed");
    _task_set_error(MQX_OK);

    /* Calling function get_event_handle_valid_field to backup value of VALID field of event group handle */
    temp = get_event_handle_valid_field(event_global_ptr);
    /* Calling function set_event_handle_valid_field to make event group handle to invalid */
    set_event_handle_valid_field(event_global_ptr, 0);

    /* Calling function _event_wait_all_until when the event group handle is not valid
    to verify the function must return EVENT_INVALID_EVENT_HANDLE */
    result = _event_wait_all_until( event_global_ptr, 4, &ticks );
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == EVENT_INVALID_EVENT_HANDLE), "Test #4 Testing 4.01: Tests function _event_wait_all_until when unable to get the event group handle");
    _task_set_error(MQX_OK);

    /* Calling function set_event_handle_valid_field to restore value of VALID field of event group handle to original value */
    set_event_handle_valid_field(event_global_ptr,temp);

    /* Calling function get_event_valid_field to backup value of VALID field of event group */
    temp = get_event_valid_field(event_global_ptr);
    /* Calling function set_event_valid_field to make event group to invalid */
    set_event_valid_field(event_global_ptr, 0);

    /* Calling function _event_wait_all_until when event group is not valid
    to verify the returned value must be EVENT_INVALID_EVENT */
    result = _event_wait_all_until( event_global_ptr, 4, &ticks );
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == EVENT_INVALID_EVENT), "Test #4 Testing 4.02: _event_wait_all_ticks with invalid event should have returned EVENT_INVALID_EVENT");
    _task_set_error(MQX_OK);

    /* Calling function set_event_valid_field to restore event group to original value */
    set_event_valid_field(event_global_ptr,temp);
    _task_set_error(MQX_OK);

    /* Calling function _event_set to set the specified event bits in the event group
    to verify the function must return MQX_OK */
    result = _event_set( event_global_ptr, 0x04 );
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing 4.03: Tests operation of function _event_set");
    _task_set_error(MQX_OK);

    /* Calling function _event_wait_all_until with all of parameter is valid
    to verify the function must return MQX_OK */
    result = _event_wait_all_until( event_global_ptr, 4, &ticks );
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing 4.04: Tests operation of function _event_wait_all_until");
    _task_set_error(MQX_OK);

    /* Calling function _event_create to create the named event group.
    Checking the returned value must be MQX_OK to verify the event group was created */
    result = _event_create(event_name);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing 4.05: Tests operation of function _event_create");
    _task_set_error(MQX_OK);

    /* Calling function _event_open to opens a connection to the event group
    to verify the returned value must be MQX_OK */
    result = _event_open(event_name, &event_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing 4.06: Tests operation of function _event_open");
    _task_set_error(MQX_OK);

    /* Creates task that will destroy the event group */
    tid = _task_create(0, TEST_TASK, 0);

    /* CAlling function _event_wait_all_until when event name will be destroyed while waiting
    to verify the returned value must be EVENT_DELETED */
    result = _event_wait_all_until( event_ptr, 4, &ticks );
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == EVENT_DELETED), "Test #4 Testing 4.07: Wait for all the specified event bits when event name will be destroyed while waiting");
    _task_set_error(MQX_OK);

    /* Destroy TEST_TASK */
    _task_destroy(tid);

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Testing function _event_wait_any operation
*
* Requirement  : EVENT020,EVENT022,EVENT027,EVENT028,EVENT130,EVENT131,EVENT134.
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    _task_id         tid;
    _mqx_uint        temp;
    _mqx_uint        result;
    void            *event_ptr;

    /* Save the current isr counting value */
    temp = get_in_isr();
    /* Set the current task as an isr */
    set_in_isr(1);

    /* Calling function _event_wait_all to verify the function must return MQX_CANNOT_CALL_FUNCTION_FROM_ISR
    because the function cannot be called from an ISR */
    result = _event_wait_any( event_global_ptr, 4, 20 );
    set_in_isr(temp);
    _int_enable();
    /* Verify result must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR), "Test #5 Testing 5.00: Calling _event_wait_any in ISR should be failed");
    _task_set_error(MQX_OK);

    /* Calling function get_event_valid_field to backup value of VALID field of event group */
    temp = get_event_valid_field(event_global_ptr);
    /* Calling function set_event_valid_field to make event group to invalid */
    set_event_valid_field(event_global_ptr, 0);

    /* Calling function _event_wait_any when event group is not valid
    to verify the returned value must be EVENT_INVALID_EVENT */
    result = _event_wait_any( event_global_ptr, 4, 20 );
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == EVENT_INVALID_EVENT), "Test #5 Testing 5.01: _event_wait_all_ticks with invalid event should have returned EVENT_INVALID_EVENT");
    _task_set_error(MQX_OK);

    /* Calling function set_event_valid_field to restore event group to original value */
    set_event_valid_field(event_global_ptr,temp);

    /* Calling function _event_set to set the specified event bits in the event group
    to verify the function must return MQX_OK */
    result = _event_set( event_global_ptr, 0x04 );
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing 5.02: Tests operation of function _event_set");
    _task_set_error(MQX_OK);

    /* Calling function _event_wait_any with all of parameter is valid
    to verify the returned value must be MQX_OK */
    result = _event_wait_any( event_global_ptr, 4, 20 );
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing 5.03: Tests operation of function _event_wait_any");
    _task_set_error(MQX_OK);

    /* Calling function _event_create to create the named event group.
    Checking the returned value must be MQX_OK to verify the event group was created */
    result = _event_create(event_name);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing 5.04: Tests operation of function _event_create");
    _task_set_error(MQX_OK);

    /* Calling function _event_open to opens a connection to the event group
    to verify the returned value must be MQX_OK */
    result = _event_open(event_name, &event_ptr);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing 5.05: Tests operation of function _event_open");
    _task_set_error(MQX_OK);

    /* Creates task that will destroy the event group */
    tid = _task_create(0, TEST_TASK, 0);

    /* Calling function _event_wait_any when event name will be destroyed while waiting
    to verify the function must return EVENT_DELETED */
    result = _event_wait_any( event_ptr, 4, 20 );
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == EVENT_DELETED), "Test #5 Testing 5.06: Wait for all the specified event bits when event name will be destroyed while waiting");
    _task_set_error(MQX_OK);

    /* Destroy TEST_TASK */
    _task_destroy(tid);

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Testing function _event_wait_any_for operation
*
* Requirement  : EVENT020,EVENT022,EVENT027,EVENT135,EVENT136,EVENT137,EVENT138,EVENT140.
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    _task_id         tid;
    _mqx_uint        temp;
    _mqx_uint        result;
    void            *event_ptr;
    MQX_TICK_STRUCT  ticks;

    /* Set the alarm */
    _time_init_ticks(&ticks, 10);

    /* Save the current isr counting value */
    temp = get_in_isr();
    /* Set the current task as an isr */
    set_in_isr(1);

    /* Calling function _event_wait_all to verify the function must return MQX_CANNOT_CALL_FUNCTION_FROM_ISR
    because the function cannot be called from an ISR */
    result = _event_wait_any_for( event_global_ptr, 4, &ticks );
    set_in_isr(temp);
    _int_enable();
    /* Verify result must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR), "Test #6 Testing 6.00: Calling _event_wait_any_for in ISR should be failed");
    _task_set_error(MQX_OK);

    /* Calling function get_event_handle_valid_field to backup value of VALID field of event group handle */
    temp = get_event_handle_valid_field(event_global_ptr);
    /* Calling function set_event_handle_valid_field to make event group handle to invalid */
    set_event_handle_valid_field(event_global_ptr, 0);

    /* Calling function _event_wait_any_for when the event group handle is not valid
    to verify the returned value must be EVENT_INVALID_EVENT_HANDLE */
    result = _event_wait_any_for( event_global_ptr, 4, &ticks );
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == EVENT_INVALID_EVENT_HANDLE), "Test #6 Testing 6.01: Tests function _event_wait_any_for when unable to get the event group handle");
    _task_set_error(MQX_OK);

    /* Calling function set_event_handle_valid_field to restore the event group handle to original value */
    set_event_handle_valid_field(event_global_ptr,temp);

    /* Calling function get_event_valid_field to backup value of VALID field of event group */
    temp = get_event_valid_field(event_global_ptr);
    /* Calling function set_event_valid_field make event group to invalid */
    set_event_valid_field(event_global_ptr, 0);

    /* Calling function _event_wait_any_for when event group is not valid
    to verify the returned value must be EVENT_INVALID_EVENT */
    result = _event_wait_any_for( event_global_ptr, 4, &ticks );
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == EVENT_INVALID_EVENT), "Test #6 Testing 6.02: _event_wait_all_ticks with invalid event should have returned EVENT_INVALID_EVENT");
    _task_set_error(MQX_OK);

    /* Calling function set_event_valid_field to restore event group to original value */
    set_event_valid_field(event_global_ptr,temp);

    /* Calling function _event_set to set the specified event bits in the event group
    to verify the function must return MQX_OK */
    result = _event_set( event_global_ptr, 0x04 );
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing 6.03: Tests operation of function _event_set");
    _task_set_error(MQX_OK);

    /* Calling function _event_wait_any_for with all of parameter is valid
    to verify the returned value must be MQX_OK */
    result = _event_wait_any_for( event_global_ptr, 4, &ticks );
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing 6.04: Tests operation of function _event_wait_any_for");
    _task_set_error(MQX_OK);

    /* Calling function _event_create to create the named event group.
    Checking the returned value must be MQX_OK to verify the event group was created */
    result = _event_create(event_name);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing 6.05: Tests operation of function _event_create");
    _task_set_error(MQX_OK);

    /* Calling function _event_open to opens a connection to the event group
    to verify the returned value must be MQX_OK */
    result = _event_open(event_name, &event_ptr);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing 6.06: Tests operation of function _event_open");
    _task_set_error(MQX_OK);

    /* Creates task that will destroy the event group */
    tid = _task_create(0, TEST_TASK, 0);

    /* Calling function _event_wait_any_for when event name will be destroyed while waiting
    to veerify the returned value must be MQX_OK */
    result = _event_wait_any_for( event_ptr, 4, &ticks );
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == EVENT_DELETED), "Test #6 Testing 6.07: Wait for all the specified event bits when event name will be destroyed while waiting");
    _task_set_error(MQX_OK);

    /* Destroy TEST_TASK */
    _task_destroy(tid);

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7: Testing function _event_wait_any_ticks in special cases
*
* Requirement  : EVENT020,EVENT022,EVENT142,EVENT144.
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    _task_id  tid;
    _mqx_uint temp;
    _mqx_uint result;
    void     *event_ptr;

    /* Calling function get_event_handle_valid_field to backup value of VALID field of event group handle */
    temp = get_event_handle_valid_field(event_global_ptr);
    /* Calling function set_event_handle_valid_field to make the event group handle to invalid */
    set_event_handle_valid_field(event_global_ptr, 0);

    /* Calling function _event_wait_any_ticks when the event group handle is not valid
    to verify the function must return EVENT_INVALID_EVENT_HANDLE */
    result = _event_wait_any_ticks(event_global_ptr, 0x1, 10);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == EVENT_INVALID_EVENT_HANDLE), "Test #7 Testing 7.00: Tests function _event_wait_any_ticks when unable to get the event group handle");
    _task_set_error(MQX_OK);

    /* Calling function set_event_handle_valid_field to restore the event group handle to original value */
    set_event_handle_valid_field(event_global_ptr,temp);

    /* Calling function _event_create to create the named event group.
    Checking the returned value must be MQX_OK to verify the event group was created */
    result = _event_create(event_name);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing 7.01: Tests operation of function _event_create");
    _task_set_error(MQX_OK);

    /* Calling function _event_open to opens a connection to the event group
    to verify the returned value must be MQX_OK */
    result = _event_open(event_name, &event_ptr);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing 7.02: Tests operation of function _event_open");
    _task_set_error(MQX_OK);

    /* Creates task that will destroy the event group */
    tid = _task_create(0, TEST_TASK, 0);

    /* Calling function _event_wait_any_ticks when event name will be destroyed while waiting
    to verify the returned value must be EVENT_DELETED */
    result = _event_wait_any_ticks(event_ptr, 4, 20);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == EVENT_DELETED), "Test #7 Testing 7.03: Wait for all the specified event bits when event name will be destroyed while waiting");
    _task_set_error(MQX_OK);

    /* Destroy TEST_TASK */
    _task_destroy(tid);

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8: Testing function _event_wait_any_until operation
*
* Requirement  : EVENT020,EVENT022,EVENT027,EVENT147,EVENT148,EVENT149,EVENT150,EVENT152.
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
    _task_id         tid;
    _mqx_uint        temp;
    _mqx_uint        result;
    void            *event_ptr;
    MQX_TICK_STRUCT  ticks;

    /* Set the alarm */
    _time_init_ticks(&ticks, 10);

    /* Save the current isr counting value */
    temp = get_in_isr();
    /* Set the current task as an isr */
    set_in_isr(1);
    /* Calling function _event_wait_all to verify the function must return MQX_CANNOT_CALL_FUNCTION_FROM_ISR
    because the function cannot be called from an ISR */
    result = _event_wait_any_until( event_global_ptr, 4, &ticks );
    set_in_isr(temp);
    _int_enable();
    /* Verify result must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR), "Test #8 Testing 8.00: Calling _event_wait_any_until in ISR should be failed");
    _task_set_error(MQX_OK);

    /* Calling function get_event_handle_valid_field to backup value of VALID field of event group handle */
    temp = get_event_handle_valid_field(event_global_ptr);
    /* Calling function set_event_handle_valid_field to make event group handle to invalid */
    set_event_handle_valid_field(event_global_ptr, 0);

    /* Calling function _event_wait_any_until when the event group handle is not valid
    to verify the function must return EVENT_INVALID_EVENT_HANDLE */
    result = _event_wait_any_until( event_global_ptr, 4, &ticks );
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == EVENT_INVALID_EVENT_HANDLE), "Test #8 Testing 8.01: Tests function _event_wait_any_until when unable to get the event group handle");
    _task_set_error(MQX_OK);

    /* Calling function set_event_handle_valid_field to restore the event group handle to original value */
    set_event_handle_valid_field(event_global_ptr,temp);

    /* Calling function get_event_valid_field to backup value of VALID field of event group */
    temp = get_event_valid_field(event_global_ptr);
    /* Calling function set_event_valid_field to make event group to invalid */
    set_event_valid_field(event_global_ptr, 0);

    /* Calling function _event_wait_any_until when event group is not valid
    to verify the returned value must be EVENT_INVALID_EVENT */
    result = _event_wait_any_until( event_global_ptr, 4, &ticks );
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == EVENT_INVALID_EVENT), "Test #8 Testing 8.02: Tests function _event_wait_any_until when event group is not valid");
    _task_set_error(MQX_OK);

    /* Calling function set_event_valid_field to restore event group to original value */
    set_event_valid_field(event_global_ptr, temp);

    /* Calling function _event_set to set the specified event bits in the event group
    to verify the returned value must be MQX_OK */
    result = _event_set( event_global_ptr, 0x04 );
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing 8.03: Tests operation of function _event_set");
    _task_set_error(MQX_OK);

    /* Calling function _event_wait_any_until with all of parameter is valid
    to verify the function must return MQX_OK */
    result = _event_wait_any_until( event_global_ptr, 4, &ticks );
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing 8.04: Tests operation of function _event_wait_any_until");
    _task_set_error(MQX_OK);

    /* Calling function _event_create to create the named event group.
    Checking the returned value must be MQX_OK to verify the event group was created */
    result = _event_create(event_name);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing 8.05: Tests operation of function _event_create");
    _task_set_error(MQX_OK);

    /* Calling function _event_open to opens a connection to the event group
    to verify the returned value must be MQX_OK */
    result = _event_open(event_name, &event_ptr);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing 8.06: Tests operation of function _event_open");
    _task_set_error(MQX_OK);

    /* Creates task that will destroy the event group */
    tid = _task_create(0, TEST_TASK, 0);

    /* Calling function _event_wait_any_until when event name will be destroyed while waiting
    to verify the function must return EVENT_DELETED */
    result = _event_wait_any_until( event_ptr, 4, &ticks );
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == EVENT_DELETED), "Test #8 Testing 8.07: Wait for all the specified event bits when it was destroyed by other task");
    _task_set_error(MQX_OK);

    /* Destroy TEST_TASK */
    _task_destroy(tid);

}

 /******  Define Test Suite      *******/

 EU_TEST_SUITE_BEGIN(suite_event3)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1: Creating and opening events"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2: Testing function _event_wait_all in special cases"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3: Testing function _event_wait_all_for in special cases"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4: Testing function _event_wait_all_until in special cases"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST # 5: Testing function _event_wait_any in special cases"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST # 6: Testing function _event_wait_any_for in special cases"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST # 7: Testing function _event_wait_any_ticks in special cases"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST # 8: Testing function _event_wait_any_until in special cases")
 EU_TEST_SUITE_END(suite_event3)

/*******  Add test suites */
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_event3, " Test of event component")
 EU_TEST_REGISTRY_END()

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   starts up the event and tests it.
*
*END*----------------------------------------------------------------------*/
void main_task
   (
      uint32_t param
   )
{/* Body */

   sprintf( event_name, "event.test" );
   sprintf( event_global, "event.global" );
   _int_install_unexpected_isr();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();

} /* Endbody */

/*TASK*---------------------------------------------------------------------
*
* Task Name    : wait_task
* Comments     :
*
*END*----------------------------------------------------------------------*/
void wait_task
   (
      uint32_t param
   )
{
    /* Destroys the fast event group while other task waited */
    _event_destroy(event_name);

    _task_block();
}/* EOF */
