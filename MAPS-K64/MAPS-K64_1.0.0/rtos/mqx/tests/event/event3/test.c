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
* Comments   : This file contains the kernel test functions for the event
*              component.
*
* Requirement: EVENT006,EVENT020,EVENT021,EVENT022,EVENT023,EVENT024,EVENT025,
*              EVENT026,EVENT027,EVENT033,EVENT035,EVENT038,EVENT041,EVENT042,
*              EVENT043,EVENT044,EVENT051,EVENT052,EVENT053,EVENT054,
*              EVENT060,EVENT066,EVENT068,EVENT067,EVENT069,EVENT070,
*              EVENT071,EVENT072,EVENT100,EVENT103,EVENT117.
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
#define WAIT_ALL_TASK1    11

void      main_task(uint32_t);
void      wait_all_task(uint32_t);

char      event_name[128] = "";

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    { MAIN_TASK     ,      main_task, 1500, 10, "Main"      ,  MQX_AUTO_START_TASK,  0},
    { WAIT_ALL_TASK1,  wait_all_task,  700,  9, "Responder1",                    0    },
    {              0,              0,    0,  0,            0,                    0,  0}
};

void tc_1_main_task(void);/* TEST #1: Testing function _event_create_component in special cases   */
void tc_2_main_task(void);/* TEST #2: Testing operation of function _event_create_auto_clear      */
void tc_3_main_task(void);/* TEST #3: Testing operation of function _event_create_fast_auto_clear */
void tc_4_main_task(void);/* TEST #4: Testing function _event_set in special cases                */
void tc_5_main_task(void);/* TEST #5: Testing function _event_clear in special cases              */
void tc_6_main_task(void);/* TEST #6: Testing function _event_destroy_fast in special cases       */
void tc_7_main_task(void);/* TEST #7: Testing function _event_test in special cases               */
/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Testing function _event_create_component in special cases
*
* Requirement  : EVENT042,EVENT043,EVENT044.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint  temp;
    _mqx_uint  result;

    /* Save the current isr counting value */
    temp = get_in_isr();
    /* Set the current task as an isr */
    set_in_isr(1);
    /* Calling function _event_create_component to verify the function must return MQX_CANNOT_CALL_FUNCTION_FROM_ISR
    because the function cannot be called from an ISR */
    result = _event_create_component( 2, 1, 4 );
    /* Restore the isr counting value */
    set_in_isr(temp);
    /* Verify the result of creating */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #1 Testing 1.00: Calling _event_create_component in ISR should be failed");
    _task_set_error(MQX_OK);

    /* Calling function _partition_create_component to create partition component. Checking the returned value must be
    MQX_OK to verify partition component was created */
    result = _event_create_component( 2, 1, 4 );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing 1.01: Create event component operation");
    _task_set_error(MQX_OK);

    /* Calling function get_event_component_capacity() to get the maximum number of event groups */
    temp = get_event_component_capacity();

    /* Create event component when maximum_number is now greater that what was previously specified */
    result = _event_create_component( 2, 1, temp + 1);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing 1.02: Create event component operation");

    /* Calling function get_event_component_capacity() to get the maximum number of event groups */
    result = get_event_component_capacity();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == (temp + 1)), "Test #1 Testing 1.03: Create event component operation");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Testing function _event_create_auto_clear operation
*
* Requirement  : EVENT006,EVENT026,EVENT041,EVENT051,EVENT052,EVENT053,EVENT054,EVENT060.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _mqx_uint                temp;
    _mqx_uint                result;
    void                    *temp_ptr;
    MEMORY_ALLOC_INFO_STRUCT memory_alloc_info;

    /* Allocate all free memory */
    memory_alloc_all(&memory_alloc_info);

    /* Calling function _event_create_auto_clear when memory is insufficient
    to verify the returned value must be MQX_OUT_OF_MEMORY */
    result =  _event_create_auto_clear( "event.one" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OUT_OF_MEMORY), "Test #2 Testing 2.00: Create event when memory is full ");
    _task_set_error(MQX_OK);

    /* Free all memory that was allocated temporary */
    memory_free_all(&memory_alloc_info);

    /* Save the current isr counting value */
    temp = get_in_isr();
    /* Set the current task as an isr */
    set_in_isr(1);
    /* Calling function _event_create_auto_clear to verify the function must return
    MQX_CANNOT_CALL_FUNCTION_FROM_ISR because the function cannot be called from an ISR */
    result = _event_create_auto_clear( "event.one" );
    /* Restore the isr counting value */
    set_in_isr(temp);
    /* Verify the result of creating */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #2 Testing 2.01: Calling _event_create_auto_clear in ISR should be failed");
    _task_set_error(MQX_OK);

    /* Calling function get_event_component_valid_field to backup value of VALID field of event component */
    temp = get_event_component_valid_field();
    /* Calling function set_event_component_valid_field to make event component to invalid */
    set_event_component_valid_field(temp + 1);

    /* Calling function _event_create_auto_clear when event component is not valid to
    verify the function must return MQX_INVALID_COMPONENT_BASE */
    result = _event_create_auto_clear( "event.one" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_INVALID_COMPONENT_BASE), "Test #2 Testing 2.02: Create event when event component is not valid ");
    _task_set_error(MQX_OK);

    /* Calling function set_event_component_valid_field to restore event component to original value */
    set_event_component_valid_field(temp);

    /* Calling function get_event_component_ptr backup pointer to event component */
    temp_ptr = get_event_component_ptr();
    /* Calling function set_event_component_ptr to set pointer to event component to NULL */
    set_event_component_ptr(NULL);
    /* Allocate all free memory */
    memory_alloc_all(&memory_alloc_info);

    /* Calling function _event_create_auto_clear with the component doesn't exist and memory is full to verify
    the function must return MQX_OUT_OF_MEMORY while trying to create the component with default values */
    result = _event_create_auto_clear( "event.one" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OUT_OF_MEMORY), "Test #2 Testing 2.03: _event_create_fast should have returned MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

    /* Free memory that was allocated temporary */
    memory_free_all(&memory_alloc_info);
    /* Calling function set_event_component_ptr to restore pointer to event component to original value */
    set_event_component_ptr(temp_ptr);

    /* Calling function _event_create_auto_clear to create "event.one". Checking the
    returned value must be MQX_OK to verify the event group was created */
    result = _event_create_auto_clear( "event.one" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing 2.04: create event.one operation");
    _task_set_error(MQX_OK);

    /* Calling function _event_create_auto_clear to create "event.two". Checking the
    returned value must be MQX_OK to verify the event group was created */
    result = _event_create_auto_clear( "event.two" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing 2.05: create event.two operation");
    _task_set_error(MQX_OK);

    /* Calling function _event_create_auto_clear to create "event.three". Checking the
    returned value must be MQX_OK to verify the event group was created */
    result = _event_create_auto_clear( "event.three" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing 2.06: create event.three operation");
    _task_set_error(MQX_OK);

    /* Calling function _event_create_auto_clear to create "event.four". Checking the
    returned value must be MQX_OK to verify the event group was created */
    result = _event_create_auto_clear( "event.four" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing 2.07: create event.four operation");
    _task_set_error(MQX_OK);

    /* Calling function _event_create_auto_clear when event table is full
    to verify the returned value must be EVENT_TABLE_FULL */
    result =  _event_create_auto_clear( "event.five" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == EVENT_TABLE_FULL), "Test #2 Testing 2.08: Tests function _event_create_auto_clear when event table is full");
    _task_set_error(MQX_OK);

    /* Calling function _event_create_auto_clear to create event that was exist
    to verify the function must return EVENT_EXISTS */
    result = _event_create_auto_clear( "event.one" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == EVENT_EXISTS), "Test #2 Testing 2.09: Create event.one that is exist ");
    _task_set_error(MQX_OK);

    /* Calling function _event_destroy to destroy event group "event.one" to verify the function must return MQX_OK */
    result = _event_destroy( "event.one" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing 2.10: destroy event.two operation");
    _task_set_error(MQX_OK);
    /* Calling function _event_destroy to destroy event group "event.two" to verify the function must return MQX_OK */
    result = _event_destroy( "event.two" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing 2.11: destroy event.two operation");
    _task_set_error(MQX_OK);
    /* Calling function _event_destroy to destroy event group "event.three" to verify the function must return MQX_OK */
    result = _event_destroy( "event.three" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing 2.12: destroy event.three operation");
    _task_set_error(MQX_OK);
    /* Calling function _event_destroy to destroy event group "event.four" to verify the function must return MQX_OK */
    result = _event_destroy( "event.four" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing 2.13: destroy event.four operation");
    _task_set_error(MQX_OK);

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Testing function _event_create_fast_auto_clear operation
*
* Requirement  : EVENT025,EVENT066,EVENT068,EVENT067,EVENT069,EVENT070,EVENT071,EVENT072.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    _mqx_uint                temp;
    _mqx_uint                result;
    _mqx_uint                index = 0;
    void                    *temp_ptr;
    MEMORY_ALLOC_INFO_STRUCT memory_alloc_info;

    /* Allocate all free memory */
    memory_alloc_all(&memory_alloc_info);

    /* Calling function _event_create_fast_auto_clear to create event when memory is insufficient
    to verify the returned value must be MQX_OUT_OF_MEMORY */
    result = _event_create_fast_auto_clear(index);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OUT_OF_MEMORY), "Test #3 Testing 3.00: Call _event_create_fast_auto_clear to create event when memory is full ");
    _task_set_error(MQX_OK);

    /* Free all memory that was allocated temporary */
    memory_free_all(&memory_alloc_info);

    /* Save the current isr counting value */
    temp = get_in_isr();
    /* Set the current task as an isr */
    set_in_isr(1);

    /* Calling function _event_create_fast_auto_clear to verify the function must return
    MQX_CANNOT_CALL_FUNCTION_FROM_ISR because the function cannot be called from an ISR */
    result = _event_create_fast_auto_clear(index);
    /* Restore the isr counting value */
    set_in_isr(temp);
    /* Verify the result of creating */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #3 Testing 3.01: Calling _event_create_fast_auto_clear in ISR should be failed");
    _task_set_error(MQX_OK);

    /* Backup value of VALID field of event component */
    temp = get_event_component_valid_field();
    /* Make event component to invalid */
    set_event_component_valid_field(temp + 1);

    /* Calling function _event_create_fast_auto_clear when event component is not valid
    to verify the function must return MQX_INVALID_COMPONENT_BASE */
    result = _event_create_fast_auto_clear(index);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_INVALID_COMPONENT_BASE, "Test #3 Testing 3.02: Create event when event component is not valid ");
    _task_set_error(MQX_OK);

    /* Restore event component to original value */
    set_event_component_valid_field(temp);

    /* Calling function get_event_component_ptr to backup pointer to event component */
    temp_ptr = get_event_component_ptr();
    /* Calling function set_event_component_ptr to set pointer to event component to NULL */
    set_event_component_ptr(NULL);
    /* Allocate all free memory */
    memory_alloc_all(&memory_alloc_info);

    /* Calling function _event_create_fast_auto_clear with the component doesn't exist and memory is full to verify
    the function must return MQX_OUT_OF_MEMORY while trying to create the component with default values */
    result = _event_create_fast_auto_clear(index);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OUT_OF_MEMORY), "Test #3 Testing 3.03: _event_create_fast should have returned MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

    /* Free memory that was allocated temporary */
    memory_free_all(&memory_alloc_info);
    /* Calling function set_event_component_ptr to restores pointer to event component to original value */
    set_event_component_ptr(temp_ptr);

    for(index=0; index < 4; index++)
    {
        /* Calling function _event_create_fast_auto_clear to create the fast event group
        to verify the returned value must be MQX_OK */
        result = _event_create_fast_auto_clear(index);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK , "Test #3 Testing 3.04: Test operation of function _event_create_fast_auto_clear");
    }

    /* Calling function _event_create_fast_auto_clear when event table is full
    to verify the function must return EVENT_TABLE_FULL */
    result = _event_create_fast_auto_clear(index + 1);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == EVENT_TABLE_FULL, "Test #3 Testing 3.05: Create event when event table is full ");
    _task_set_error(MQX_OK);

    index = 0;
    /* Calling function _event_create_fast_auto_clear to create event when it is exits
    to verify the returned value must be EVENT_EXISTS */
    result = _event_create_fast_auto_clear(index);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == EVENT_EXISTS, "Test #3 Testing 3.06: Create event when it is exist ");
    _task_set_error(MQX_OK);

    for(index=0; index < 4 ; index++){
        /* Calling function _event_destroy_fast to destroy the events we created.
        Checking the returned value must be MQX_OK to verify the event was destroyed */
        result =  _event_destroy_fast(index);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing 3.07: destroy all fast events created operation");
        _task_set_error(MQX_OK);
    }
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Testing functions _event_set in special cases
*
* Requirement  : EVENT020,EVENT022,EVENT026,EVENT027,EVENT100.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    _mqx_uint                   result;
    void                       *event_ptr;

#if MQX_IS_MULTI_PROCESSOR
    EVENT_CONNECTION_STRUCT_PTR event_connection_ptr;
    _mqx_uint                   temp;
#endif

    /* Calling function _event_create to create the named event group.
    Checking the returned value must be MQX_OK to verify the event group was created */
    result = _event_create(event_name);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing 4.00: Test operation of function _event_create");
    _task_set_error(MQX_OK);

    /* Calling function _event_open to opens a connection to the event group
    to verify the returned value must be MQX_OK */
    result = _event_open(event_name, &event_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing 4.01: Test operation of function _event_open");
    _task_set_error(MQX_OK);

#if MQX_IS_MULTI_PROCESSOR

    event_connection_ptr = (EVENT_CONNECTION_STRUCT_PTR)event_ptr;

    /* Backup value of REMOTE_CPU field of event */
    temp = event_connection_ptr->REMOTE_CPU;
    /* Set value of REMOTE_CPU to 1 */
    event_connection_ptr->REMOTE_CPU = 1;

    /* Set event when event group is not in table to verify the function must return EVENT_NOT_FOUND */
    result = _event_set(event_ptr, 0x04);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == EVENT_NOT_FOUND), "Test #4 Testing 4.02: Sets the specified event bits when it is not in table");

    /* Restore value of REMOTE_CPU to original value */
    event_connection_ptr->REMOTE_CPU = temp;

#endif /* MQX_IS_MULTI_PROCESSOR */

    /* Calling function _event_set to set the specified event bits in the event group
    with all parameter are valid to verify the returned value must be MQX_OK */
    result = _event_set(event_ptr, 0x04);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing 4.03: Test operation of function _event_set");
    _task_set_error(MQX_OK);

    /* Calling function _event_destroy to destroy event group that was created to verify the function must return MQX_OK */
    result =  _event_destroy(event_name);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing 4.04: Test operation of function _event_destroy");
    _task_set_error(MQX_OK);

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Testing functions _event_clear in special cases
*
* Requirement  : EVENT020,EVENT022,EVENT026,EVENT038.
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    _mqx_uint  temp;
    _mqx_uint  result;
    void      *event_ptr;

    /* Calling function _event_create to create the named event group.
    Checking the returned value must be MQX_OK to verify the event group was created */
    result = _event_create(event_name);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing 5.00: Test operation of function _event_create");
    _task_set_error(MQX_OK);

    /* Calling function _event_open to opens a connection to the event group
    to verify the returned value must be MQX_OK */
    result = _event_open(event_name, &event_ptr);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing 5.01: Test operation of function _event_open");
    _task_set_error(MQX_OK);

    /* Calling function get_event_valid_field to backup value of VALID field of event */
    temp = get_event_valid_field(event_ptr);
    /* Calling function set_event_valid_field to make event group to invalid */
    set_event_valid_field(event_ptr, 0);

    /* Calling function _event_clear when event group is not valid to verify the returned value must be EVENT_INVALID_EVENT */
    result = _event_clear(event_ptr, 4);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == EVENT_INVALID_EVENT), "Test #5 Testing 5.02: Clear the specified event bits in the event group when event group is not valid");
    _task_set_error(MQX_OK);

    /* Calling function set_event_valid_field to restores event group to original value */
    set_event_valid_field(event_ptr,temp);

    /* Calling function _event_close in normal operation to verify the function must return MQX_OK */
    result = _event_close(event_ptr);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing 5.03: Closes the connection to the event group was destroyed");
    _task_set_error(MQX_OK);

    /* Calling function _event_clear when event group is closed to verify the returned value must be EVENT_INVALID_EVENT_HANDLE */
    result = _event_clear(event_ptr, 4);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == EVENT_INVALID_EVENT_HANDLE), "Test #5 Testing 5.04: Clear event group when event group is not valid");
    _task_set_error(MQX_OK);

    /* Calling function _event_destroy to destroy event group that was created to verify the function must return MQX_OK */
    result =  _event_destroy(event_name);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing 5.05: Test operation of function _event_destroy");
    _task_set_error(MQX_OK);

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Testing function _event_destroy_fast in special cases
*
* Requirement  : EVENT021,EVENT023,EVENT035,EVENT117.
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    _mqx_uint result;
    _task_id  tid;
    uint32_t   index;

    index = 0;

    /* Calling function _event_create_fast to creates the fast event group.
    Checking the returned value must be MQX_OK to verify the event group was created */
    result = _event_create_fast(index);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing 6.00: Test operation of function _event_create");
    _task_set_error(MQX_OK);

    /* Calling function _task_create to creates WAIT_ALL_TASK1 that will wait the event */
    tid = _task_create(0, WAIT_ALL_TASK1, index);

    /* Calling function _event_destroy_fast to destroys the fast event group
    while other task waited to verify the function must return MQX_OK */
    result =  _event_destroy_fast(index);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing 6.02: Destroys the fast event group while other task waited");

    /* Destroy WAIT_ALL_TASK1 */
    _task_destroy(tid);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7: Testing function _event_test in special cases
*
* Requirement  : EVENT020,EVENT022,EVENT026,EVENT033,EVENT103.
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    _mqx_uint        result;
    void            *temp_ptr;
    void            *event_ptr;
    void            *event_error_ptr;

    event_error_ptr = NULL;

    /* Calling function _event_create to create the named event group.
    Checking the returned value must be MQX_OK to verify the event group was created */
    result = _event_create(event_name);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing 7.00: Test operation of function _event_create");
    _task_set_error(MQX_OK);

    /* Calling function _event_open to opens a connection to the event group
    to verify the returned value must be MQX_OK */
    result = _event_open(event_name, &event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing 7.01: Test operation of function _event_open");
    _task_set_error(MQX_OK);

    /* Calling function _event_test to tests the event component.
    Checking the returned value must be MQX_OK to verify the operation of function successful.*/
    result = _event_test(&event_error_ptr);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing 7.02: Test operation of function _event_test");
    _task_set_error(MQX_OK);

    /* Calling function get_event_queue_ptr to backup pointer to event queue */
    temp_ptr = get_event_queue_ptr(event_ptr);
    /* Calling function set_event_queue_ptr to make pointer to event queue is NULL */
    set_event_queue_ptr(event_ptr, NULL);

    /* Calling function _event_test when event queue is invalid to verify the function must return MQX_CORRUPT_QUEUE */
    result = _event_test(&event_error_ptr);

    /* Calling function set_event_queue_ptr to restore event queue to original value */
    set_event_queue_ptr(event_ptr, temp_ptr);
    /* Verify the result of creating to MQX_CORRUPT_QUEUE */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_CORRUPT_QUEUE), "Test #7 Testing 7.03: Tests function _event_test when event queue is invalid");
    _task_set_error(MQX_OK);

    /* Calling function _event_test to tests the event component.
    Checking the returned value must be MQX_OK to verify the operation of function successful.*/
    result = _event_test(&event_error_ptr);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing 7.04: Test operation of function _event_test");
    _task_set_error(MQX_OK);

    /* Calling function _event_destroy to destroy event group that was created to verify the function must return MQX_OK */
    result =  _event_destroy(event_name);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing 7.05: Test operation of function _event_destroy");
    _task_set_error(MQX_OK);
}

 /******  Define Test Suite      *******/

 EU_TEST_SUITE_BEGIN(suite_event3)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1: Testing function _event_create_component in special cases "),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2: Testing operation of function _event_create_auto_clear"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3: Testing operation of function _event_create_fast_auto_clear"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4: Testing function _event_set in special cases"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST # 5: Testing function _event_clear in special cases"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST # 6: Testing function _event_destroy_fast in special cases"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST # 7: Testing function _event_test in special cases"),
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

   _int_install_unexpected_isr();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();

} /* Endbody */

/*TASK*---------------------------------------------------------------------
*
* Task Name    : wait_all_task
* Comments     :
*
*END*----------------------------------------------------------------------*/
void wait_all_task
   (
      uint32_t index
   )
{
    _mqx_uint   result;
    void       *event_ptr;

    /* Calling function _event_open_fast to opens a connection to the fast event group. Checking the returned value must be MQX_OK */
    result = _event_open_fast(index, &event_ptr);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing 6.01: Test operation of function _event_open");
    _task_set_error(MQX_OK);

    /* Calling function _event_wait_all_ticks to wait for all the specified event bits
    when it was destroyed by other task to verify the function must return EVENT_DELETED */
    result = _event_wait_all_ticks(event_ptr, 4, 50);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == EVENT_DELETED), "Test #6 Testing 6.03: Wait for all the specified event bits when it was destroyed by other task");
    _task_set_error(MQX_OK);

    _task_block();

}/* EOF */
