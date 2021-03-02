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
* $Version : 3.8.12.0$
* $Date    : Jul-20-2012$
*
* Comments:
*
*   This file contains the source functions for the demo of the
*   time slicing functions of the kernel.
*
*END*******************************fh*****************************************/


#include <mqx_inc.h>
#include <bsp.h>
#include <name.h>
#include <name_prv.h>
#include <event.h>
#include <event_prv.h>

#include "test.h"
#include "util.h"
//#include <test_module.h>
//#include <EUnit_Api.h>
//#include "EUnit_Assertion_Ids.h"

#include <string.h>
#include <mqx.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test

#define PRINT_KLOG

//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);// TEST #1 - Testing _event_create_component function
void tc_2_main_task(void);// TEST #2 - Testing _event_create function
void tc_3_main_task(void);// TEST #3 - Testing _event_create_fast function
void tc_4_main_task(void);// TEST #4 - Testing _event_open function
void tc_5_main_task(void);// TEST #5 - Testing _event_open_fast function
void tc_6_main_task(void);// TEST #6 - Testing _event_close function
void tc_7_main_task(void);// TEST #7 - Testing _event_destroy_fast function
void tc_8_main_task(void);// TEST #8 - Testing _event_destroy function
void tc_9_main_task(void);// TEST #9 - Testing _event_set function
void tc_10_main_task(void);// TEST #10 - Testing _event_wait_any_ticks function
void tc_11_main_task(void);// TEST #11 - Testing _event_wait_all_ticks function
void tc_12_main_task(void);// TEST #12 - Testing _event_get_value function
void tc_13_main_task(void);// TEST #13 - Testing _event_get_wait_count function
void tc_14_main_task(void);// TEST #14 - Testing _event_clear function
void tc_15_main_task(void);// TEST #15 - Testing _event_test function
void tc_16_main_task(void);// TEST #16 - Mixing events by name and index
void tc_17_main_task(void);// TEST #17 - Test the issue destroying the waiting task while waiting for events
void tc_18_main_task(void);// TEST #18 - Normal operation testing on events
//------------------------------------------------------------------------
                                       //3000
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
{ MAIN_TASK,       main_task,       1700,  7, "Main",       MQX_AUTO_START_TASK},
{ WAIT_ALL_TASK1,  wait_all_task,   700,  8, "Responder1", 0},
{ WAIT_ALL_TASK2,  wait_all_task,   700,  9, "Responder2", 0},
{ WAIT_ALL_TASK3,  wait_all_task,   600, 10, "Responder3", 0},
{ WAIT_ANY_TASK,   wait_any_task,   700,  7, "Taska",      0},
{ 0,               0,               0,     0, 0,            0}
};

_task_id   main_id = 0;
char       string[128];
char       event_name[128] = "";
_mqx_uint  event_timeout   = 20;
_mqx_uint  event_got[17];

MEMORY_ALLOC_INFO_STRUCT   memory_alloc_info;
EVENT_COMPONENT_STRUCT_PTR event_component_ptr;

uint16_t   isr_value;
_mqx_uint evt_valid;
_mqx_uint valid_value;
_mqx_uint handle_field;

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
    void       *event_ptr;
    _mqx_uint   result;
    _mqx_uint   i;

    result = _event_open( event_name, &event_ptr );
    if (index == 0)
        EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: WA1.0 _event_open operation")
    else
        EU_ASSERT_REF_TC_MSG(tc_18_main_task, (result == MQX_OK), "Test #18 Testing: WA1.0 _event_open operation")

    while(1) {
        i = 1;
        while( 1 ) {
            result = _event_wait_all_ticks( event_ptr, (_mqx_uint)(1 << index),
            event_timeout+8*index );
            if( result == EVENT_WAIT_TIMEOUT ) {
                _time_delay_ticks( 20+(2*index*i) );
                i++;
            } else if (result == EVENT_DELETED) {
                _event_close( &event_ptr );
                _task_destroy(0);
                break;
            } else {
                if ( result != MQX_OK ) {
                    _task_destroy(0);
                }
            }
        }
        
        event_got[index] =  result == MQX_OK;
        _time_delay_ticks( 20+2*index );
    }

}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : wait_any_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void wait_any_task
   (
      uint32_t  initial_data
   )
{
   void      *event_ptr;
   _mqx_uint  result;

    result = _event_open( "event.global", &event_ptr);
    if (initial_data == 1)
        EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: W1.0 _event_open operation")
    else if (initial_data == 0)
        EU_ASSERT_REF_TC_MSG(tc_17_main_task, (result == MQX_OK), "Test #17 Testing: W1.0 _event_open operation")

    while(TRUE) {
        result = _event_wait_any_ticks(event_ptr, 0x01, 0);
        if (initial_data == 0)
            EU_ASSERT_REF_TC_MSG(tc_17_main_task, (result == MQX_OK), "Test #17 Testing: W1.1 _event_wait_any_ticks operation")
        if( result != MQX_OK && initial_data != 1 ) {
            break;
        } /* Endif */
        
        if (initial_data == 1)
            EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == EVENT_DELETED), "Test #10 Testing: W1.2 _event_wait_any_ticks should have returned EVENT_DELETED")
        if( result != EVENT_DELETED && initial_data == 1 ) {
            break;
        } /* Endif */
        
        if( result == EVENT_DELETED && initial_data == 1) {
            break;
        }/* Endif */
        result = _event_clear(event_ptr, 0x01);
        if (initial_data == 1)
            EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: _event_clear operation")
        else if (initial_data == 0)
            EU_ASSERT_REF_TC_MSG(tc_17_main_task, (result == MQX_OK), "Test #17 Testing: _event_clear operation")
        if (result != MQX_OK) {
            //test_stop();
            eunit_test_stop();
			_mqx_exit(0);
        }
    }
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST #1: Test the _event_create_component function
//
//END---------------------------------------------------------------------

void tc_1_main_task(void)
{
    _mqx_uint result;
    void     *mem;
   
    /*
    ** The following code tests the event_create_component function  
    */

    /* check for MQX memory error
    ** Return code - MQX_OUT_OF_MEMORY = 0X4
    */
    mem = _mem_alloc(sizeof(EVENT_COMPONENT_STRUCT));
    memory_alloc_all(&memory_alloc_info);
    result = _event_create_component( 2, 1,4 );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OUT_OF_MEMORY), "Test #1 Testing: 1.0: _event_create_component with MQX memory error operation should have returned MQX_OUT_OF_MEMORY")
    _task_set_error(MQX_OK);

    memory_free_all(&memory_alloc_info);
  
    /* test the normal creation
    ** Return code - MQX_OK = 0
    */
    result = _event_create_component( 2, 1,4 );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.1: create event component operation")
    _task_set_error(MQX_OK);

    /* Try again result should be OK */
    result = _event_create_component( 2, 1,4 );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.2: create event component operation")
    _task_set_error(MQX_OK);
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST #2: Test the _event_create function
//
//END---------------------------------------------------------------------

void tc_2_main_task(void)
{
    _mqx_uint result;

    /*
    ** The following code tests the event_create function          
    */

    /* check for MQX memory error
    ** Return code - MQX_OUT_OF_MEMORY = 0X4
    */
    memory_alloc_all(&memory_alloc_info);
    result =  _event_create( "event.one" );
    memory_free_all(&memory_alloc_info);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OUT_OF_MEMORY), "Test #2 Testing: 2.0: create event.one with MQX memory error should have returned MQX_OUT_OF_MEMORY")
    _task_set_error(MQX_OK);

    /* test MQX_CANNOT_CALL_FUNCTION_FROM_ISR
    ** Return code - MQX_CANNOT_CALL_FUNCTION_FROM_IST = 0X0C
    */
    _int_disable();
    isr_value = get_in_isr();
    set_in_isr(1);
    result =  _event_create( "event.one" );
    set_in_isr(isr_value);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR), "Test #2 Testing: 2.1: event should have returned MQX_CANNOT_CALL_FUNCTION_FROM_ISR when called from ISR")
    _task_set_error(MQX_OK);

    /* test for the  MQX component exist
    ** Return code - MQX_OUT_OF_MEMORY = 0x61
    */
    event_component_ptr = get_event_component_ptr();
    set_event_component_ptr(NULL);

    /* by if the default if the component doesn't exist
      it will create a default component, so we need to fail
      the creation by creating a memory error */
    memory_alloc_all(&memory_alloc_info);
    result =  _event_create( "event.one" );
    memory_free_all(&memory_alloc_info);
    set_event_component_ptr(event_component_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OUT_OF_MEMORY), "Test #2 Testing: 2.2: create event.one should have returned MQX_OUT_OF_MEMORY")
    _task_set_error(MQX_OK);

    /* test for the invalid component base
    ** Return code - MQX_INVALID_COMPONENT_BASE = 0x63
    */
    evt_valid = get_event_component_valid_field();
    set_event_component_valid_field(0);
    result =  _event_create( "event.one" );
    set_event_component_valid_field(evt_valid);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_INVALID_COMPONENT_BASE), "Test #2 Testing: 2.3: create event.one with invalid component base should have returned MQX_INVALID_COMPONENT_BASE")
    _task_set_error(MQX_OK);

    /* test the normal creation
    ** Return code - MQX_OK = 0
    */
    result =  _event_create( "event.one" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.4: create event.one operation")
    _task_set_error(MQX_OK);

    /* test for event exists error
    ** Return code - EVENT_EXISTS = 0X2008
    */
    result =  _event_create( "event.one" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == EVENT_EXISTS), "Test #2 Testing: 2.5: test for event exists error - create event.one should have returned EVENT_EXISTS")
    _task_set_error(MQX_OK);

    /* test for event table full error
    ** Return code - EVENT_TABLE_FULL = 0X2009
    */
    result =  _event_create( "event.two" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.6: create event.two operation")
    _task_set_error(MQX_OK);

    result =  _event_create( "event.three" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.7: create event.three operation")
    _task_set_error(MQX_OK);

    result =  _event_create( "event.four" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.8: create event.four operation")
    _task_set_error(MQX_OK);

    result =  _event_create( "event.five" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == EVENT_TABLE_FULL), "Test #2 Testing: 2.9: test for event table full error - try to create event.five should have returned EVENT_TABLE_FULL")
    _task_set_error(MQX_OK);

    /* destroy all the events we just created */
    result =  _event_destroy( "event.one" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.10: destroy event.one operation")
    _task_set_error(MQX_OK);
    result =  _event_destroy( "event.two" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.11: destroy event.two operation")
    _task_set_error(MQX_OK);
    result =  _event_destroy( "event.three" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.12: destroy event.three operation")
    _task_set_error(MQX_OK);
    result =  _event_destroy( "event.four" );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.13: destroy event.four operation")
    _task_set_error(MQX_OK);
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST #3: Test the _event_create_fast function
//
//END---------------------------------------------------------------------

void tc_3_main_task(void)
{
    _mqx_uint result;
    _mqx_uint index;

    /*
    ** The following code tests the event_create_fast function  
    */

    index=0;

    /* check for MQX memory error
    ** Return code - MQX_OUT_OF_MEMORY = 0X4
    */
    memory_alloc_all(&memory_alloc_info);
    result =  _event_create_fast(index);
    memory_free_all(&memory_alloc_info);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OUT_OF_MEMORY), "Test #3 Testing: 3.0: _event_create_fast with MQX memory error should have returned MQX_OUT_OF_MEMORY")
    _task_set_error(MQX_OK);

    /* test for the invalid component base
    ** Return code - MQX_INVALID_COMPONENT_BASE = 0x63
    */
    evt_valid = get_event_component_valid_field();
    set_event_component_valid_field(0);
    result =  _event_create_fast(index);
    set_event_component_valid_field(evt_valid);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_INVALID_COMPONENT_BASE), "Test #3 Testing: 3.1: _event_create_fast with invalid component base should have returned MQX_INVALID_COMPONENT_BASE");
    _task_set_error(MQX_OK);

    /* test for the  MQX component exist
    ** Return code - MQX_OUT_OF_MEMORY = 0x61
    */
    event_component_ptr = get_event_component_ptr();
    set_event_component_ptr(NULL);
    /* by if the default if the component doesn't exist
      it will create a default component, so we need to fail
      the creation by creating a memory error */
    memory_alloc_all(&memory_alloc_info);
    result =  _event_create_fast(index);
    memory_free_all(&memory_alloc_info);
    set_event_component_ptr(event_component_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OUT_OF_MEMORY), "Test #3 Testing: 3.2: _event_create_fast should have returned MQX_OUT_OF_MEMORY")
    _task_set_error(MQX_OK);

    /* test MQX_CANNOT_CALL_FUNCTION_FROM_ISR
    ** Return code - MQX_CANNOT_CALL_FUNCTION_FROM_ISR = 0X0C
    */
    _int_disable();
    isr_value = get_in_isr();
    set_in_isr(1);
    result =  _event_create_fast(index);
    set_in_isr(isr_value);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR), "Test #3 Testing: 3.3: _event_create_fast should have returned MQX_CANNOT_CALL_FUNCTION_FROM_ISR when called from ISR")
    _task_set_error(MQX_OK);

    /* test the normal creation
    ** Return code - MQX_OK = 0
    */
    result =  _event_create_fast(index);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing: 3.4: _event_create_fast operation")
    _task_set_error(MQX_OK);

    /* test for event exists error
    ** Return code - EVENT_EXISTS = 0X2008
    */
    result =  _event_create_fast(index);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == EVENT_EXISTS), "Test #3 Testing: 3.5: test for event exists error - create fast event should have returned EVENT_EXISTS")
    _task_set_error(MQX_OK);

    /* test for event table full error
    ** Return code - EVENT_TABLE_FULL = 0X2009
    */
         
    for(index=1; index < 5 ; index++){
        result =  _event_create_fast(index);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((index >= 4 || result == MQX_OK) && (index != 4 || result == EVENT_TABLE_FULL)), "Test #3 Testing: 3.6: test for event table full error - create 5 fast events should have returned EVENT_TABLE_FULL when trying to create 5th event")
      _task_set_error(MQX_OK);
    }

    /* destroy the events we created */
    for(index=0; index < 4 ; index++){
        result =  _event_destroy_fast(index);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing: 3.7: destroy all fast events created operation")
      _task_set_error(MQX_OK);
    }
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST #4: Test the event_open function
//
//END---------------------------------------------------------------------

void tc_4_main_task(void)
{
    _mqx_uint result;
    void     *event_ptr;
    void     *event2_ptr;

    /*
    ** The following code tests the event_open function  
    */

    /* initial setup */
    result =  _event_create( event_name );
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: 4.0: _event_create operation")
    _task_set_error(MQX_OK);

    /* check for MQX memory error
    ** Return code - MQX_OUT_OF_MEMORY = 0X4
    */
    memory_alloc_all(&memory_alloc_info);
    result = _event_open( event_name, &event_ptr );
    memory_free_all(&memory_alloc_info);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OUT_OF_MEMORY), "Test #4 Testing: 4.1: _event_open with MQX memory error should have returned MQX_OUT_OF_MEMORY")
    _task_set_error(MQX_OK);

    /* test for the invalid component base
    ** Return code - MQX_INVALID_COMPONENT_BASE = 0x63
    */
    evt_valid = get_event_component_valid_field();
    set_event_component_valid_field(0);
    result = _event_open( event_name, &event_ptr );
    set_event_component_valid_field(evt_valid);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_INVALID_COMPONENT_BASE), "Test #4 Testing: 4.2: _event_open with invalid component base should have returned MQX_INVALID_COMPONENT_BASE")
    _task_set_error(MQX_OK);

    /* test for the  MQX component does not exist
    ** Return code - MQX_COMPONENT_DOES_NOT_EXIST = 0x61
    */
    event_component_ptr = get_event_component_ptr();
    set_event_component_ptr(NULL);
    result = _event_open( event_name, &event_ptr );
    set_event_component_ptr(event_component_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_COMPONENT_DOES_NOT_EXIST), "Test #4 Testing: 4.3: test for error MQX_COMPONENT_DOES_NOT_EXIST operation")
    _task_set_error(MQX_OK);

    /* test for Invalid event
    ** Return code - EVENT_INVALID_EVENT = 0X200B
    */
    valid_value= get_event_valid_field(event_name);
    set_event_valid_field(event_name, 0);
    result = _event_open( event_name, &event_ptr );
    set_event_valid_field(event_name, valid_value);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == EVENT_INVALID_EVENT), "Test #4 Testing: 4.4: _event_open with invalid event should have returned EVENT_INVALID_EVENT")
    _task_set_error(MQX_OK);

    /* test the normal creation
    ** Return code - MQX_OK = 0
    */
    result = _event_open( event_name, &event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: 4.5: _event_open operation")
    _task_set_error(MQX_OK);

    /* opening second connection will not return an error
    ** Return code - MQX_OK = 0
    */
    result = _event_open( event_name, &event2_ptr );
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: 4.6: _event_open operation")
    _task_set_error(MQX_OK);

    result = _event_close( event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: 4.7: _event_close operation")
    _task_set_error(MQX_OK);

    result = _event_close( event2_ptr );
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: 4.8: _event_close operation")
    _task_set_error(MQX_OK);

    /* test for Event not found
    ** Return code - EVENT_NOT_FOUND = 0X200A
    */
    result =  _event_destroy(event_name);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: 4.9: _event_destroy operation")
    _task_set_error(MQX_OK);
    result = _event_open( event_name, &event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == EVENT_NOT_FOUND), "Test #4 Testing: 4.10: _event_open should have returned EVENT_NOT_FOUND")
    _task_set_error(MQX_OK);
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_5_main_task
// Comments     : TEST #5: Test the event_open_fast function
//
//END---------------------------------------------------------------------

void tc_5_main_task(void)
{
    _mqx_uint result;
    _mqx_uint index;
    void     *event_ptr;
    void     *event2_ptr;

    /*
    ** The following code tests the event_open_fast function  
    */

    /* initial setup */
    index=1;
    result = _event_create_fast(index);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.0: event create fast operation")
    _task_set_error(MQX_OK);

    /* check for MQX memory error
    ** Return code - MQX_OUT_OF_MEMORY = 0X4
    */
    memory_alloc_all(&memory_alloc_info);
    result = _event_open_fast( index, &event_ptr );
    memory_free_all(&memory_alloc_info);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OUT_OF_MEMORY), "Test #5 Testing: 5.1: _event_open_fast with MQX memory error should have returned MQX_OUT_OF_MEMORY")
    _task_set_error(MQX_OK);

    /* test for the invalid component base
    ** Return code - MQX_INVALID_COMPONENT_BASE = 0x63
    */
    evt_valid = get_event_component_valid_field();
    set_event_component_valid_field(0);
    result = _event_open_fast( index, &event_ptr );
    set_event_component_valid_field(evt_valid);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_INVALID_COMPONENT_BASE), "Test #5 Testing: 5.2: _event_open_fast with invalid component base should have returned MQX_INVALID_COMPONENT_BASE")
    _task_set_error(MQX_OK);

    /* test for the  MQX component does not exist
    ** Return code - MQX_COMPONENT_DOES_NOT_EXIST = 0x61
    */
    event_component_ptr = get_event_component_ptr();
    set_event_component_ptr(NULL);
    result = _event_open_fast( index, &event_ptr );
    set_event_component_ptr(event_component_ptr);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_COMPONENT_DOES_NOT_EXIST), "Test #5 Testing: 5.3: _event_open_fast should have returned MQX_COMPONENT_DOES_NOT_EXIST")
    _task_set_error(MQX_OK);

    /* test for Invalid event
    ** Return code - EVENT_INVALID_EVENT = 0X200B
    */
    valid_value= get_event_valid_field_fast(index);
    set_event_valid_field_fast(index, 0);
    result = _event_open_fast( index, &event_ptr );
    set_event_valid_field_fast(index, valid_value);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == EVENT_INVALID_EVENT), "Test #5 Testing: 5.4: _event_open_fast with invalid event should have returned EVENT_INVALID_EVENT")
    _task_set_error(MQX_OK);

    /* test the normal creation
    ** Return code - MQX_OK = 0
    */
    result = _event_open_fast( index, &event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.6: _event_open_fast operation")
    _task_set_error(MQX_OK);  

    /* opening second connection will not produce error
    ** Return code - MQX_OK = 0
    */
    result = _event_open_fast( index, &event2_ptr );
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.7: _event_open_fast operation")
    _task_set_error(MQX_OK);

    result = _event_close( event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.8: _event_close operation")
    _task_set_error(MQX_OK);

    result = _event_close( event2_ptr );
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.9: _event_close operation")
    _task_set_error(MQX_OK);

    /* test for Event not found
    ** Return code - EVENT_NOT_FOUND = 0X200A
    */
    /* destroy the event */
    result =  _event_destroy_fast(index);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.10: _event_destroy_fast operation")
    _task_set_error(MQX_OK);
    result = _event_open_fast( index, &event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == EVENT_NOT_FOUND), "Test #5 Testing: 5.11: _event_open_fast with destroyed event should have returned EVENT_NOT_FOUND")
    _task_set_error(MQX_OK);
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_6_main_task
// Comments     : TEST #6: Test the _event_close function
//
//END---------------------------------------------------------------------

void tc_6_main_task(void)
{
    _mqx_uint result;
    _mqx_uint index;
    void     *event_ptr;

    /*
    ** The following code tests the _event_close function  
    */

    /* initial setup */
    index=1;
    result = _event_create_fast(index);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing: 6.0: _event_create_fast operation")
    _task_set_error(MQX_OK);
    result = _event_open_fast( index, &event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing: 6.1: _event_open_fast operation")
    _task_set_error(MQX_OK);

    /* test MQX_CANNOT_CALL_FUNCTION_FROM_ISR
    ** Return code - MQX_CANNOT_CALL_FUNCTION_FROM_ISR = 0X0C
    */
    _int_disable();
    isr_value = get_in_isr();
    set_in_isr(1);
    result = _event_close( event_ptr );
    set_in_isr(isr_value);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR), "Test #6 Testing: 6.2: _event_close should have returned MQX_CANNOT_CALL_FROM_ISR when called from ISR")
    _task_set_error(MQX_OK);

    /* test for Invalid event
    ** Return code - EVENT_INVALID_EVENT_HANDLE = 0X2003
    */
    handle_field = get_event_handle_valid_field(event_ptr);
    set_event_handle_valid_field(event_ptr, 0);
    result = _event_close( event_ptr );
    set_event_handle_valid_field(event_ptr,handle_field);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == EVENT_INVALID_EVENT_HANDLE), "Test #6 Testing: 6.3: _event_close with invalid event should have returned EVENT_INVALID_EVENT")
    _task_set_error(MQX_OK);

    /* test for normal close
    ** Return code - MQX-OK = 0
    */
    result = _event_close( event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing: 6.4: _event_close operation")
    _task_set_error(MQX_OK);

    /* destroy the event */
    result =  _event_destroy_fast(index);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing: 6.5: _event_destroy_fast operation")
    _task_set_error(MQX_OK);
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_7_main_task
// Comments     : TEST #7: Test the _event_destroy_fast function
//
//END---------------------------------------------------------------------

void tc_7_main_task(void)
{
    _mqx_uint result;
    _mqx_uint index;
    void     *event_error_ptr;

    /*
    ** The following code tests the _event_destroy_fast function  
    */

    /* initial setup */
    index=1;
    result = _event_create_fast(index);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: 7.0: _event_create_fast operation")
    _task_set_error(MQX_OK);

    /* test MQX_CANNOT_CALL_FUNCTION_FROM_ISR
    ** Return code - MQX_CANNOT_CALL_FUNCTION_FROM_ISR = 0X0C
    */
    _int_disable();
    isr_value = get_in_isr();
    set_in_isr(1);
    result =  _event_destroy_fast(index);
    set_in_isr(isr_value);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR), "Test #7 Testing: 7.1: _event_destroy_fast should have returned MQX_CANNOT_CALL_FROM_ISR when called from ISR")
    _task_set_error(MQX_OK);

    /* test MQX_COMPONENT_DOES_NOT_EXIST
    ** Return code - 
    */
    event_component_ptr = get_event_component_ptr();
    set_event_component_ptr(NULL);
    result =  _event_destroy_fast(index);
    set_event_component_ptr(event_component_ptr);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_COMPONENT_DOES_NOT_EXIST), "Test #7 Testing: 7.2: _event_destroy_fast should have returned MQX_COMPONENT_DOES_NOT_EXIST")
    _task_set_error(MQX_OK);

    /* test MQX_INVALID_COMPONENT_BASE
    ** Return code - 
    */
    evt_valid = get_event_component_valid_field();
    set_event_component_valid_field(0);
    result =  _event_destroy_fast(index);
    set_event_component_valid_field(evt_valid);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_INVALID_COMPONENT_BASE), "Test #7 Testing: 7.3: _event_destroy_fast with invalid component base should have returned MQX_INVALID_COMPONENT_BASE")
    _task_set_error(MQX_OK);

    /* test for Invalid event
    ** Return code - EVENT_INVALID_EVENT = 0X200B
    */
    valid_value= get_event_valid_field_fast(index);
    set_event_valid_field_fast(index, 0);
    result =  _event_destroy_fast(index);
    set_event_valid_field_fast(index, valid_value);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == EVENT_INVALID_EVENT), "Test #7 Testing: 7.4: _event_destroy_fast with invalid event should have returned EVENT_INVALID_EVENT")
    _task_set_error(MQX_OK);

    /* test the normal operation
    ** Return code - MQX_OK = 0
    */
    result =  _event_destroy_fast(index);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: 7.5: _event_destroy_fast operation")
    _task_set_error(MQX_OK);

    /* test for Invalid event
    ** Return code - EVENT_NOT_FOUND = 0X200A
    */
    result =  _event_destroy_fast(index);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == EVENT_NOT_FOUND), "Test #7 Testing: 7.6: _event_destroy_fast with invalid event should have returned EVENT_NOT_FOUND")
    _task_set_error(MQX_OK);

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: 7.7: _event_test operation")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_8_main_task
// Comments     : TEST #8: Test the _event_destroy function
//
//END---------------------------------------------------------------------

void tc_8_main_task(void)
{
    _mqx_uint result;

    /*
    ** The following code tests the _event_destroy function  
    */

    /* initial setup */
    result =  _event_create( event_name );
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: 8.0: _event_create operation")
    _task_set_error(MQX_OK);

    /* test MQX_CANNOT_CALL_FUNCTION_FROM_ISR
    ** Return code - MQX_CANNOT_CALL_FUNCTION_FROM_ISR = 0X0C
    */
    _int_disable();
    isr_value = get_in_isr();
    set_in_isr(1);
    result =  _event_destroy( event_name );
    set_in_isr(isr_value);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR), "Test #8 Testing: 8.1: _event_destroy should have returned MQX_CANNOT_CALL_FROM_ISR when called from ISR")
    _task_set_error(MQX_OK);

    /* test MQX_COMPONENT_DOES_NOT_EXIST
    ** Return code - 
    */
    event_component_ptr = get_event_component_ptr();
    set_event_component_ptr(NULL);
    result =  _event_destroy( event_name );
    set_event_component_ptr(event_component_ptr);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_COMPONENT_DOES_NOT_EXIST), "Test #8 Testing: 8.2: _event_destroy should have returned MQX_COMPONENT_DOES_NOT_EXIST")
    _task_set_error(MQX_OK);

    /* test MQX_INVALID_COMPONENT_BASE
    ** Return code - 
    */
    evt_valid = get_event_component_valid_field();
    set_event_component_valid_field(0);
    result =  _event_destroy( event_name );
    set_event_component_valid_field(evt_valid);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_INVALID_COMPONENT_BASE), "Test #8 Testing: 8.3: _event_destroy with invalid component base should have returned MQX_INVALID_COMPONENT_BASE")
    _task_set_error(MQX_OK);

    /* test for Invalid event
    ** Return code - EVENT_INVALID_EVENT = 0X200B
    */
    valid_value= get_event_valid_field(event_name);
    set_event_valid_field(event_name, 0);
    result =  _event_destroy( event_name );
    set_event_valid_field(event_name, valid_value);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == EVENT_INVALID_EVENT), "Test #8 Testing: 8.4: _event_destroy with invalid event should have returned EVENT_INVALID_EVENT")
    _task_set_error(MQX_OK);

    /* test the normal operation
    ** Return code - MQX_OK = 0
    */
    result =  _event_destroy( event_name );
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: 8.5: _event_destroy operation")
    _task_set_error(MQX_OK);

    /* test for Invalid event
    ** Return code - EVENT_NOT_FOUND = 0X200A
    */
    result =  _event_destroy( event_name );
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == EVENT_NOT_FOUND), "Test #8 Testing: 8.6: _event_destroy with destroyed event should have returned EVENT_NOT_FOUND")
    _task_set_error(MQX_OK);
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_9_main_task
// Comments     : TEST #9: Test the _event_set function
//
//END---------------------------------------------------------------------

void tc_9_main_task(void)
{
    _mqx_uint result;
    void     *event_ptr;
    void     *event_error_ptr;

    /*
    ** The following code tests the _event_set function  
    */

    /* initial setup */
    result =  _event_create( event_name );
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: 9.0: _event_create operation")
    _task_set_error(MQX_OK);
    result = _event_open( event_name, &event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: 9.1: _event_open operation")
    _task_set_error(MQX_OK);

    /* test MQX_COMPONENT_DOES_NOT_EXIST
    ** Return code - 
    */
    event_component_ptr = get_event_component_ptr();
    set_event_component_ptr(NULL);
    result = _event_set( event_ptr, 1 );
    set_event_component_ptr(event_component_ptr);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_COMPONENT_DOES_NOT_EXIST), "Test #9 Testing: 9.2: _event_set should have returned MQX_COMPONENT_DOES_NOT_EXIST")
    _task_set_error(MQX_OK);

    /* test MQX_INVALID_COMPONENT_BASE
    ** Return code - 
    */
    evt_valid = get_event_component_valid_field();
    set_event_component_valid_field(0);
    result = _event_set( event_ptr, 1 );
    set_event_component_valid_field(evt_valid);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_INVALID_COMPONENT_BASE), "Test #9 Testing: 9.3: _event_set with invalid component base should have returned MQX_INVALID_COMPONENT_BASE")
    _task_set_error(MQX_OK);

    /* test for Invalid event
    ** Return code - EVENT_INVALID_EVENT_HANDLE = 0X2003
    */
    handle_field = get_event_handle_valid_field(event_ptr);
    set_event_handle_valid_field(event_ptr, 0);
    result = _event_set( event_ptr, 1 );
    set_event_handle_valid_field(event_ptr,handle_field);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == EVENT_INVALID_EVENT_HANDLE), "Test #9 Testing: 9.4: _event_set with invalid event handle should have returned EVENT_INVALID_EVENT_HANDLE")
    _task_set_error(MQX_OK);

    /* test the normal operation
    ** Return code - MQX_OK = 0
    ** NOTE: seting all 32 bits are tested in the unified test.
    */
    result = _event_set( event_ptr, 1 );
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: 9.5: _event_set operation")
    _task_set_error(MQX_OK);

    /* test for Invalid event
    ** Return code - EVENT_INVALID_EVENT = 0X200B
    */
    result =  _event_destroy( event_name );
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: 9.6: _event_destroy operation")
    _task_set_error(MQX_OK);
    result = _event_set( event_ptr, 1 );
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == EVENT_INVALID_EVENT), "Test #9 Testing: 9.7: _event_set with invalid event should have returned EVENT_INVALID_EVENT")
    _task_set_error(MQX_OK);

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: 9.8: _event_test operation")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_10_main_task
// Comments     : TEST #10: Test the _event_wait_any_ticks function 
//
//END---------------------------------------------------------------------

void tc_10_main_task(void)
{
    _mqx_uint result;
    void     *event_ptr;
    void     *event_error_ptr;
    _task_id  tid;

    /*
    ** The following code tests the _event_wait_any_ticks function  
    */

    /* initial setup */
    result =  _event_create( event_name );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.0: _event_create operation")
    _task_set_error(MQX_OK);
    result = _event_open( event_name, &event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.1: _event_open operation")
    _task_set_error(MQX_OK);

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.2: _event_test operation")

    /* test MQX_CANNOT_CALL_FUNCTION_FROM_ISR
    ** Return code - 
    */
    _int_disable();
    isr_value = get_in_isr();
    set_in_isr(1);
    result = _event_wait_any_ticks( event_ptr, 0, 20 );
    set_in_isr(isr_value);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR), "Test #10 Testing: 10.3: _event_wait_any_ticks should have returned MQX_CANNOT_CALL_FROM_ISR when called from ISR")
    _task_set_error(MQX_OK);

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.4: _event_test operation")

    /* test for Invalid event
    ** Return code - EVENT_INVALID_EVENT_HANDLE = 0X2003
    */
    handle_field = get_event_handle_valid_field(event_ptr);
    set_event_handle_valid_field(event_ptr, 0);
    result = _event_wait_any_ticks( event_ptr, 0, 20 );
    set_event_handle_valid_field(event_ptr,handle_field);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == EVENT_INVALID_EVENT_HANDLE), "Test #10 Testing: 10.5: _event_wait_any_ticks with invalid event handle should have returned EVENT_INVALID_EVENT_HANDLE")
    _task_set_error(MQX_OK);

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.6: _event_test operation")

    /* test for wait time out
    ** Return code - EVENT_WAIT_TIME_OUT = 0x2007
    */
    result = _event_wait_any_ticks( event_ptr, 0, 2 );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == EVENT_WAIT_TIMEOUT), "Test #10 Testing: 10.7: _event_wait_any_ticks should have returned EVENT_WAIT_TIMEOUT")
    _task_set_error(MQX_OK);

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.8: _event_test operation")

    /* test the normal operation
    ** Return code - MQX_OK = 0
    ** NOTE: seting all 32 bits are tested in the unified test.
    */
    result = _event_set( event_ptr, 1 );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.9: _event_set operation")
    _task_set_error(MQX_OK);
    result = _event_wait_any_ticks( event_ptr, 0x01, event_timeout );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.10: _event_wait_any_ticks operation")
    _task_set_error(MQX_OK);

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.11: _event_test operation")

    /* test for event deleted while waiting */

    result =  _event_create( "event.global" );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.12: create event.global operation")
    _task_set_error(MQX_OK);

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.13: _event_test operation")

    tid = _task_create(0, WAIT_ANY_TASK, 1);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (tid != MQX_NULL_TASK_ID), "Test #10 Testing: 10.14: create wait task operation")
    _task_set_error(MQX_OK);
    _time_delay_ticks(3); 

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.15: _event_test operation")

    result =  _event_destroy( "event.global" );
    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.16: _event_test operation")
    _time_delay_ticks(2);

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.17: _event_test operation")
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.18: destroy event.global operation")
    _task_set_error(MQX_OK);

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.19: _event_test operation")

    /* test for Invalid event
    ** Return code - EVENT_INVALID_EVENT = 0X200B
    */
    result =  _event_destroy( event_name );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.20: _event_destroy operation")
    _task_set_error(MQX_OK);

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.21: _event_test operation")

    result = _event_set( event_ptr, 1 );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == EVENT_INVALID_EVENT), "Test #10 Testing: 10.22: _event_set with invalid event should have returned EVENT_INVALID_EVENT")
    _task_set_error(MQX_OK);

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.23: _event_test operation")

    result = _event_wait_any_ticks( event_ptr, 0, 20 );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == EVENT_INVALID_EVENT), "Test #10 Testing: 10.24: _event_wait_any_ticks with invalid event should have returned EVENT_INVALID_EVENT")

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.25: _event_test operation")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_11_main_task
// Comments     : TEST #11: Test the _event_wait_all_ticks function
//
//END---------------------------------------------------------------------

void tc_11_main_task(void)
{
    _mqx_uint result;
    void     *event_ptr;
    void     *event_error_ptr;

    /*
    ** The following code tests the _event_wait_all_ticks function  
    */

    /* initial setup */
    result =  _event_create( event_name );
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.0: _event_create operation")
    _task_set_error(MQX_OK);
    result = _event_open( event_name, &event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.1: _event_open operation")
    _task_set_error(MQX_OK);

    /* test MQX_CANNOT_CALL_FUNCTION_FROM_ISR
    ** Return code - 
    */
    _int_disable();
    isr_value = get_in_isr();
    set_in_isr(1);
    result = _event_wait_all_ticks( event_ptr, 4, 20 );
    set_in_isr(isr_value);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR), "Test #11 Testing: 11.2 _event_wait_all_ticks should have returned MQX_CANNOT_CALL_FROM_ISR when called from ISR")
    _task_set_error(MQX_OK);

    /* test for Invalid event
    ** Return code - EVENT_INVALID_EVENT_HANDLE = 0X2003
    */
    handle_field = get_event_handle_valid_field(event_ptr);
    set_event_handle_valid_field(event_ptr, 0);
    result = _event_wait_all_ticks( event_ptr, 4, 20 );
    set_event_handle_valid_field(event_ptr,handle_field);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == EVENT_INVALID_EVENT_HANDLE), "Test #11 Testing: 11.3: _event_wait_all_ticks with invalid event handle should have returned EVENT_INVALID_EVENT_HANDLE")
    _task_set_error(MQX_OK);

    /* test for wait time out
    ** Return code - EVENT_WAIT_TIME_OUT = 0x2007
    */
    result = _event_wait_all_ticks( event_ptr, 4, 20 );
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == EVENT_WAIT_TIMEOUT), "Test #11 Testing: 11.4: _event_wait_all_ticks should have returned EVENT_WAIT_TIMEOUT")
    _task_set_error(MQX_OK);

    /* test the normal operation
    ** Return code - MQX_OK = 0
    ** NOTE: seting all 32 bits are tested in the unified test.
    */
    result = _event_set( event_ptr, 0x04 );
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.5: _event_set operation")
    _task_set_error(MQX_OK);
    result = _event_wait_all_ticks( event_ptr, 0x04, 20 );
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.6: _event_wait_all_ticks operation")
    _task_set_error(MQX_OK);

    /* test for event deleted while waiting */

    result =  _event_create( "event.global" );
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.7: create event.global operation")
    _task_set_error(MQX_OK);

    _task_create(0, WAIT_ALL_TASK1, 0);
    _time_delay_ticks(2);

    result =  _event_destroy( event_name );
    _time_delay_ticks(2);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.8: _event_destroy operation")
    _task_set_error(MQX_OK);

    result =  _event_destroy( "event.global" );
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.8.2: destroy event.global operation")

    /* test for Invalid event
    ** Return code - EVENT_INVALID_EVENT = 0X200B
    */
    result =  _event_destroy( event_name );
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == EVENT_NOT_FOUND), "Test #11 Testing: 11.9: _event_destroy with invalid event should have returned EVENT_NOT_FOUND")
    _task_set_error(MQX_OK);
    result = _event_set( event_ptr,0x04 );
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == EVENT_INVALID_EVENT), "Test #11 Testing: 11.10: _event_set with invalid event should have returned EVENT_INVALID_EVENT")
    _task_set_error(MQX_OK);
    result = _event_wait_all_ticks( event_ptr, 0x04, 20 );
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == EVENT_INVALID_EVENT), "Test #11 Testing: 11.11: _event_wait_all_ticks with invalid event should have returned EVENT_INVALID_EVENT")
    _task_set_error(MQX_OK);

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.12: _event_test operation")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_12_main_task
// Comments     : TEST #12: Test the _event_get_value function
//
//END---------------------------------------------------------------------

void tc_12_main_task(void)
{
    _mqx_uint result;
    void     *event_ptr;
    void     *event_error_ptr;
    _mqx_uint event_value;

    /*
    ** The following code tests the _event_get_value function  
    */

    /* initial setup */
    result =  _event_create( event_name );
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, (result == MQX_OK), "Test #12 Testing: 12.0: _event_create operation")
    _task_set_error(MQX_OK);
    result = _event_open( event_name, &event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, (result == MQX_OK), "Test #12 Testing: 12.1: _event_open operation")
    _task_set_error(MQX_OK);

    /* test for Invalid event
    ** Return code - EVENT_INVALID_EVENT_HANDLE = 0X2003
    */

    handle_field = get_event_handle_valid_field(event_ptr);
    set_event_handle_valid_field(event_ptr, 0);
    result = _event_get_value(event_ptr, &event_value);
    set_event_handle_valid_field(event_ptr,handle_field);
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, (result == EVENT_INVALID_EVENT_HANDLE), "Test #12 Testing: 12.2, 12.3: _event_get_value with invalid event should have returned EVENT_INVALID_EVENT_HANDLE")
    _task_set_error(MQX_OK);
         
    /* test the normal operation
    ** Return code - MQX_OK = 0
    ** 
    */
    result = _event_set( event_ptr, 0x04 );
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, (result == MQX_OK), "Test #12 Testing: 12.4: _event_set operation")
    _task_set_error(MQX_OK);
    result = _event_wait_all_ticks( event_ptr, 0x04, 20 );
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, (result == MQX_OK), "Test #12 Testing: 12.5: _event_wait_all_ticks operation")
    _task_set_error(MQX_OK);

    result = _event_get_value(event_ptr, &event_value);
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, (result == MQX_OK), "Test #12 Testing: 12.6, 12.7: _event_get_value operation")
    _task_set_error(MQX_OK);

    /* test for Invalid event
    ** Return code - EVENT_INVALID_EVENT = 0X200B
    */
    _task_set_error(MQX_OK);
    result =  _event_destroy( event_name );
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, (result == MQX_OK), "Test #12 Testing: 12.9: _event_destroy operation")
    _task_set_error(MQX_OK);
    result = _event_get_value(event_ptr, &event_value);
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, (result == EVENT_INVALID_EVENT), "Test #12 Testing: 12.10, 12.11: _event_get_value with invalid event should have returned EVENT_INVALID_EVENT")
    _task_set_error(MQX_OK);

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, (result == MQX_OK), "Test #12 Testing: 12.12: _event_test operation")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_13_main_task
// Comments     : TEST #13: Test the function _event_get_wait_count function
//
//END---------------------------------------------------------------------

void tc_13_main_task(void)
{
    _mqx_uint result;
    void     *event_ptr;
    void     *event_error_ptr;
    _mqx_uint event_value;
    _mqx_int  tasks_waiting;

    /*
    ** The following code tests the _event_get_wait_count function  
    */

    /* initial setup */
    result =  _event_create( event_name );
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, (result == MQX_OK), "Test #13 Testing: 13.0: _event_create operation")
    _task_set_error(MQX_OK);
    result = _event_open( event_name, &event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, (result == MQX_OK), "Test #13 Testing: 13.1: _event_open operation")
    _task_set_error(MQX_OK);

    /* test the normal operation
    ** Return code - MQX_OK = 0
    ** 
    ** Note: multiple tasks waiting are tested in the unified testing program.
    */
    result = _event_set( event_ptr, 0x04 );
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, (result == MQX_OK), "Test #13 Testing: 13.3: _event_set operation")
    _task_set_error(MQX_OK);
    result = _event_wait_all_ticks( event_ptr, 0x04, 20 );
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, (result == MQX_OK), "Test #13 Testing: 13.4: _event_wait_all_ticks operation")
    _task_set_error(MQX_OK);

    result = _event_get_value(event_ptr, &event_value);
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, (result == MQX_OK), "Test #13 Testing: 13.4.5: _event_get_value operation")
    tasks_waiting = _event_get_wait_count(event_ptr);
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, (tasks_waiting == 0), "Test #13 Testing: 13.5: _event_get_wait_count should have returned value of 0")

    /* test for Invalid event handle
    ** Return code - -1
    */
    handle_field = get_event_handle_valid_field(event_ptr);
    set_event_handle_valid_field(event_ptr, 0);
    tasks_waiting=_event_get_wait_count(event_ptr);
    set_event_handle_valid_field(event_ptr,handle_field);
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, (tasks_waiting == -1), "Test #13 Testing: 13.6: _event_get_wait_count should have returned value of -1")

    /* test for Invalid event
    ** Return code - -1
    */
    result =  _event_destroy( event_name );
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, (result == MQX_OK), "Test #13 Testing: 13.7: _event_destroy operation")
    _task_set_error(MQX_OK);
    tasks_waiting=_event_get_wait_count(event_ptr);
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, (tasks_waiting == -1), "Test #13 Testing: 13.8: _event_get_wait_count should have returned value of -1")

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, (result == MQX_OK), "Test #13 Testing: 13.9: _event_test operation")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_14_main_task
// Comments     : TEST #14: Test the _event_clear function
//
//END---------------------------------------------------------------------

void tc_14_main_task(void)
{
    _mqx_uint result;
    void     *event_ptr;

    /*
    ** The following code tests the _event_clear function  
    */

    /* initial setup */
    result =  _event_create( event_name );
    EU_ASSERT_REF_TC_MSG(tc_14_main_task, (result == MQX_OK), "Test #14 Testing: 14.0: _event_create operation")
    _task_set_error(MQX_OK);
    result = _event_open( event_name, &event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_14_main_task, (result == MQX_OK), "Test #14 Testing: 14.1: _event_open operation")
    _task_set_error(MQX_OK);

    /* test for EVENT_INVALID_EVENT_HANDLE
    ** Return code -
    */
    handle_field = get_event_handle_valid_field(event_ptr);
    set_event_handle_valid_field(event_ptr, 0);
    result = _event_clear( event_ptr, 0x04);
    EU_ASSERT_REF_TC_MSG(tc_14_main_task, (result == EVENT_INVALID_EVENT_HANDLE), "Test #14 Testing: 14.2: _event_clear with invalid event handle should have returned EVENT_INVALID_EVENT_HANDLE")
    _task_set_error(MQX_OK);
    set_event_handle_valid_field(event_ptr,handle_field);

    /* test the normal operation
    ** Return code - MQX_OK = 0
    ** 
    ** Note: more clearing and setting is tested by TaskA..
    */
            
    result = _event_set( event_ptr, 0x04 );
    EU_ASSERT_REF_TC_MSG(tc_14_main_task, (result == MQX_OK), "Test #14 Testing: 14.3: _event_set operation")
    _task_set_error(MQX_OK);
    result = _event_wait_all_ticks( event_ptr, 0x04, 20 );
    EU_ASSERT_REF_TC_MSG(tc_14_main_task, (result == MQX_OK), "Test #14 Testing: 14.4: _event_wait_all_ticks operation")
    _task_set_error(MQX_OK);
    result = _event_clear( event_ptr, 0x04);
    EU_ASSERT_REF_TC_MSG(tc_14_main_task, (result == MQX_OK), "Test #14 Testing: 14.5: _event_clear operation")
    _task_set_error(MQX_OK);

    /* test for EVENT_INVALID_EVENT
    ** Return code -  EVENT_INVALID_EVENT = 0X200B
    */
    result =  _event_destroy( event_name );
    EU_ASSERT_REF_TC_MSG(tc_14_main_task, (result == MQX_OK), "Test #14 Testing: 14.6: _event_destroy operation")
    _task_set_error(MQX_OK);
    result = _event_clear( event_ptr, 0x04);
    EU_ASSERT_REF_TC_MSG(tc_14_main_task, (result == EVENT_INVALID_EVENT), "Test #14 Testing: 14.7: _event_clear with invalid event should have returned EVENT_INVALID_EVENT")
    _task_set_error(MQX_OK);
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_15_main_task
// Comments     : TEST #15: Test the _event_test function
//
//END---------------------------------------------------------------------

void tc_15_main_task(void)
{
    _mqx_uint result;
    void     *event_ptr;
    void     *event_error_ptr;

    /*
    ** The following code tests the _event_test function  
    */
    /* initial setup */
    result =  _event_create( event_name );
    EU_ASSERT_REF_TC_MSG(tc_15_main_task, (result == MQX_OK), "Test #15 Testing: 15.0: _event_create operation")
    _task_set_error(MQX_OK);
    result = _event_open( event_name, &event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_15_main_task, (result == MQX_OK), "Test #15 Testing: 15.1: _event_open operation")
    _task_set_error(MQX_OK);
    result =  _event_create( "event.one" );
    EU_ASSERT_REF_TC_MSG(tc_15_main_task, (result == MQX_OK), "Test #15 Testing: 15.2: create event.one operation")
    _task_set_error(MQX_OK);
    result = _event_open( "event.one", &event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_15_main_task, (result == MQX_OK), "Test #15 Testing: 15.3: open event.one operation")
    _task_set_error(MQX_OK);

    /* test for normal operation
    ** Return code - MQX_OK
    */
    result = _event_test(&event_error_ptr);
    EU_ASSERT_REF_TC_MSG(tc_15_main_task, (result == MQX_OK), "Test #15 Testing: 15.4: _event_test operation")
    _task_set_error(MQX_OK);

    /* test for INVALID_COMPONENT_HANDLE
    ** Return code - MQX_OK
    */
    event_component_ptr = get_event_component_ptr();
    set_event_component_ptr(NULL);
    result = _event_test(&event_error_ptr);
    set_event_component_ptr(event_component_ptr);
    EU_ASSERT_REF_TC_MSG(tc_15_main_task, (result == MQX_OK), "Test #15 Testing: 15.5: _event_test operation")
    _task_set_error(MQX_OK);

    /* test MQX_INVALID_COMPONENT_BASE
    ** Return code - MQX_INVALID_COMPONENT_BASE
    */
    evt_valid = get_event_component_valid_field();
    set_event_component_valid_field(0);
    result = _event_test(&event_error_ptr);
    set_event_component_valid_field(evt_valid);
    EU_ASSERT_REF_TC_MSG(tc_15_main_task, (result == MQX_INVALID_COMPONENT_BASE), "Test #15 Testing: 15.6: _event_test with invalid component base should have returned MQX_INVALID_COMPONENT_BASE")
    _task_set_error(MQX_OK);

    /* test for Invalid event
    ** Return code - EVENT_INVALID_EVENT = 0X200B
    */
    valid_value= get_event_valid_field(event_name);
    set_event_valid_field(event_name, 0);
    result = _event_test(&event_error_ptr);
    set_event_valid_field(event_name, valid_value);
    EU_ASSERT_REF_TC_MSG(tc_15_main_task, (result == EVENT_INVALID_EVENT), "Test #15 Testing: 15.7: _event_test with invalid event should have returned EVENT_INVALID_EVENT")
    _task_set_error(MQX_OK);

    /*
    ** Delete all the events created
    */
    result =  _event_destroy( event_name );
    EU_ASSERT_REF_TC_MSG(tc_15_main_task, (result == MQX_OK), "Test #15 Testing: 15.8: _event_destroy operation")
    _task_set_error(MQX_OK);
    result =  _event_destroy( "event.one" );
    EU_ASSERT_REF_TC_MSG(tc_15_main_task, (result == MQX_OK), "Test #15 Testing: 15.9: destroy event.one operation")
    _task_set_error(MQX_OK);

    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_15_main_task, (result == MQX_OK), "Test #15 Testing: 15.10: _event_test operation")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_16_main_task
// Comments     : TEST #16: Test the feature of mixing events by name and index
//
//END---------------------------------------------------------------------

void tc_16_main_task(void)
{
    _mqx_uint result;
    _mqx_uint index;

    /*
    ** The following code tests the feature of mixing events by name and index  
    */

    /* create event groups by name */
    result =  _event_create( "event.one" );
    EU_ASSERT_REF_TC_MSG(tc_16_main_task, (result == MQX_OK), "Test #16 Testing: 16.0: create event.one operation")
    _task_set_error(MQX_OK);

    result =  _event_create( "event.two" );
    EU_ASSERT_REF_TC_MSG(tc_16_main_task, (result == MQX_OK), "Test #16 Testing: 16.1: create event.two operation")
    _task_set_error(MQX_OK);

	/* Create fast event groups by index
	** note: index starts at zero 
	*/
    for (index = 0; index < 4 ; index++){
        result =  _event_create_fast(index);
        EU_ASSERT_REF_TC_MSG(tc_16_main_task, ((index >= 2 || result == EVENT_EXISTS) && (index < 2 || result == MQX_OK)), "Test #16 Testing: 16.2: create fast events operation")
        _task_set_error(MQX_OK);
    }

    /* destroy the events we created */
    for (index = 0; index < 4 ; index++){
        result =  _event_destroy_fast(index);
        EU_ASSERT_REF_TC_MSG(tc_16_main_task, ((index >= 2 || result == EVENT_NOT_FOUND) && (index < 2 || result == MQX_OK)), "Test #16 Testing: 16.3: destroy all fast events created")
        _task_set_error(MQX_OK);
    }
	
	/* Events created by name also get destroyed */
    result =  _event_destroy( "event.one" );
    EU_ASSERT_REF_TC_MSG(tc_16_main_task, (result == MQX_OK), "Test #16 Testing: 16.4: destroy event.one operation")
    _task_set_error(MQX_OK);

    result =  _event_destroy( "event.two" );
    EU_ASSERT_REF_TC_MSG(tc_16_main_task, (result == MQX_OK), "Test #16 Testing: 16.5: destroy event.two operation")
    _task_set_error(MQX_OK);
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_17_main_task
// Comments     : TEST #17: Test the issue destroying the waiting task
//                  while waiting for events
//
//END---------------------------------------------------------------------

void tc_17_main_task(void)
{
    _mqx_uint result;
    void     *event_ptr;
    void     *event_error_ptr;
    _task_id  task;

    /*
    ** The following code tests the issue destroying the waiting task
    ** while waiting for events  
    */
    result =  _event_create( "event.global" );
    EU_ASSERT_REF_TC_MSG(tc_17_main_task, (result == MQX_OK), "Test #17 Testing: 17.0: create event.one operation")
    _task_set_error(MQX_OK);
    result = _event_open( "event.global", &event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_17_main_task, (result == MQX_OK), "Test #17 Testing: 17.1: create event.global operation")
    _task_set_error(MQX_OK);

	/* create a task waiting for the event */
    task =_task_create(0, WAIT_ANY_TASK, 0);
    EU_ASSERT_REF_TC_MSG(tc_17_main_task, (task != MQX_NULL_TASK_ID), "Test #17 Testing: 17.1.5: create wait task operation")
    _time_delay_ticks(2);
	
	/* set specified event bit to unblock the waiting task */
    result=_event_set(event_ptr, 0x01);
    EU_ASSERT_REF_TC_MSG(tc_17_main_task, (result == MQX_OK), "Test #17 Testing: 17.2: _event_set operation")
    _task_set_error(MQX_OK);
    _time_delay_ticks(2);
	
	/* set specified event bit to unblock the waiting task again */
    result=_event_set(event_ptr, 0x01);
    EU_ASSERT_REF_TC_MSG(tc_17_main_task, (result == MQX_OK), "Test #17 Testing: 17.3: _event_set operation")
    _task_set_error(MQX_OK);
    _time_delay_ticks(2);
	
	/* destroy the waiting task while waiting for the event */
    result = _task_destroy(task);
    EU_ASSERT_REF_TC_MSG(tc_17_main_task, (result == MQX_OK), "Test #17 Testing: 17.4: destroy wait task operation")
    _task_set_error(MQX_OK);
	
	/* close the event connection */
    result =  _event_close( event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_17_main_task, (result == MQX_OK), "Test #17 Testing: 17.4.0: _event_close operation")
    _task_set_error(MQX_OK);
	
	/* destroy the event we created */
    result =  _event_destroy( "event.global" );
    EU_ASSERT_REF_TC_MSG(tc_17_main_task, (result == MQX_OK), "Test #17 Testing: 17.5: destroy event.global operation")
    _task_set_error(MQX_OK);
           
    result = _event_test( &event_error_ptr );
    EU_ASSERT_REF_TC_MSG(tc_17_main_task, (result == MQX_OK), "Test #17 Testing: 17.6: _event_test operation")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_18_main_task
// Comments     : TEST #18: Test multiple tasks waiting for events while
//                  waiting for events
//
//END---------------------------------------------------------------------

void tc_18_main_task(void)
{
    _mqx_uint result;
    void     *event_ptr;
    _mqx_uint event_value;
    _mqx_int  tasks_waiting;
    _mqx_uint i, j;

    /*
    ** The following code tests multiple tasks waiting for events
    ** while waiting for events  
    */

	/* creat an event group and open a connection data for the event */
    result =  _event_create( event_name );
    EU_ASSERT_REF_TC_MSG(tc_18_main_task, (result == MQX_OK), "Test #18 Testing: _event_create operation")
    _task_set_error(MQX_OK);
    result = _event_open( event_name, &event_ptr );
    EU_ASSERT_REF_TC_MSG(tc_18_main_task, (result == MQX_OK), "Test #18 Testing: _event_open operation")
    _task_set_error(MQX_OK);

	/* create tasks waiting for events */
    _task_create(0,WAIT_ALL_TASK3, 3);
    _time_delay_ticks(2);
    _task_create(0,WAIT_ALL_TASK2, 2);
    _task_create(0,WAIT_ALL_TASK1, 1);
    _task_create(0,WAIT_ALL_TASK1, 4);
    _task_create(0,WAIT_ALL_TASK2, 5);
    _task_create(0,WAIT_ALL_TASK3, 6);
    _task_create(0,WAIT_ALL_TASK1, 7);
    _task_create(0,WAIT_ALL_TASK2, 8);
    _task_create(0,WAIT_ALL_TASK3, 9);

    i = ((_mqx_uint)(0x1 << (MQX_INT_SIZE_IN_BITS - 1)));
    for(j = 0; j < 128; ++j) {
        if( i == 0 ) {
            i = ((_mqx_uint)(0x1 << (MQX_INT_SIZE_IN_BITS - 1)));
            _event_clear( event_ptr, MAX_MQX_UINT );
            _time_delay_ticks(20);
        }
        _time_delay_ticks(10);
        _int_disable();
		
		/* get the current value of the event and the number of waiting tasks */
        result = _event_get_value( event_ptr, &event_value );   
        tasks_waiting = _event_get_wait_count( event_ptr );   
        _int_enable();
		
		/* print out the event value and number of waiting tasks */
        printf("Event Val %08lx, Tasks %d\r", (uint32_t)event_value, tasks_waiting );
		
		/* getting event value and number of task should be successful */
		if ((result != MQX_OK) || (tasks_waiting < 0)) {
			break;
		}
		
		/* each bits in the event group are set sequentially */
        _event_set( event_ptr, i );
        i = i >> 1;
    }
	EU_ASSERT_REF_TC_MSG(tc_18_main_task, ((result == MQX_OK) && (tasks_waiting >= 0)), "Test #18 Testing: _event_get_value operation and _event_get_wait_count operation ")        
}

//------------------------------------------------------------------------
// Define Test Suite    
EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Testing _event_create_component function"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Testing _event_create function"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST #3 - Testing _event_create_fast function"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST #4 - Testing _event_open function"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST #5 - Testing _event_open_fast function"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST #6 - Testing _event_close function"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST #7 - Testing _event_destroy_fast function"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST #8 - Testing _event_destroy function"),
    EU_TEST_CASE_ADD(tc_9_main_task, " TEST #9 - Testing _event_set function"),
    EU_TEST_CASE_ADD(tc_10_main_task, " TEST #10 - Testing _event_wait_any_ticks function"),
    EU_TEST_CASE_ADD(tc_11_main_task, " TEST #11 - Testing _event_wait_all_ticks function"),
    EU_TEST_CASE_ADD(tc_12_main_task, " TEST #12 - Testing _event_get_value function"),
    EU_TEST_CASE_ADD(tc_13_main_task, " TEST #13 - Testing _event_get_wait_count function"),
    EU_TEST_CASE_ADD(tc_14_main_task, " TEST #14 - Testing _event_clear function"),
    EU_TEST_CASE_ADD(tc_15_main_task, " TEST #15 - Testing _event_test function"),
    EU_TEST_CASE_ADD(tc_16_main_task, " TEST #16 - Mixing events by name and index"),
    EU_TEST_CASE_ADD(tc_17_main_task, " TEST #17 - Test the issue destroying the waiting task while waiting for events"),
    EU_TEST_CASE_ADD(tc_18_main_task, " TEST #18 - Normal operation testing on events")
EU_TEST_SUITE_END(suite_1)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, " - Test of events for kernel")
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

    sprintf( event_name, "event.three" );

    _int_install_unexpected_isr();

    main_id = _task_get_id();
    //test_initialize();
    //EU_RUN_ALL_TESTS(test_result_array);
	EU_RUN_ALL_TESTS(eunit_tres_array);
    //test_stop();
	eunit_test_stop();

}/* Endbody */

/*------------------------------------------------------------------------*/
/* 
** Event Utility Functions
*/

EVENT_COMPONENT_STRUCT_PTR get_event_component_ptr
   (
      void
   )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    EVENT_COMPONENT_STRUCT_PTR event_component_ptr;

    _GET_KERNEL_DATA(kernel_data);                                         
    event_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
    return  event_component_ptr; 

}

void set_event_component_ptr
   (
      EVENT_COMPONENT_STRUCT_PTR evt_ptr
   )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;

    _GET_KERNEL_DATA(kernel_data);                                         
    kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS] = evt_ptr;

}

_mqx_uint get_event_component_valid_field
   (
      void
   )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    EVENT_COMPONENT_STRUCT_PTR event_component_ptr;

    _GET_KERNEL_DATA(kernel_data);                                         
    event_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
    return event_component_ptr->VALID ;

}

void set_event_component_valid_field
   (
      _mqx_uint evt_valid
   )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    EVENT_COMPONENT_STRUCT_PTR event_component_ptr;

    _GET_KERNEL_DATA(kernel_data);                                         
    event_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
    event_component_ptr->VALID = evt_valid;

}



_mqx_uint get_event_valid_field
   (
      char  *name_ptr
   )
{
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    EVENT_COMPONENT_STRUCT_PTR  event_component_ptr;
    EVENT_STRUCT_PTR            event_ptr;
    _mqx_uint                   result;
    _mqx_max_type               tmp;

    _GET_KERNEL_DATA(kernel_data);                                         
    event_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
    result = _name_find_internal(event_component_ptr->NAME_TABLE_HANDLE, 
        name_ptr,&tmp);
    if (result == MQX_OK) {
        event_ptr = (EVENT_STRUCT_PTR)tmp;
        return event_ptr->VALID ;
    } else {
        return 0;
    } /* Endif */

}

void set_event_valid_field
   (
      char  *name_ptr,
      _mqx_uint  valid_value
   )
{
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    EVENT_COMPONENT_STRUCT_PTR  event_component_ptr;
    EVENT_STRUCT_PTR            event_ptr;
    _mqx_uint                   result;
    _mqx_max_type               tmp;

    _GET_KERNEL_DATA(kernel_data);                                         

    event_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
    result = _name_find_internal(event_component_ptr->NAME_TABLE_HANDLE, 
        name_ptr,&tmp);
    if (result == MQX_OK) {      
        event_ptr = (EVENT_STRUCT_PTR)tmp;
        event_ptr->VALID = valid_value; 
    } else {
        result = 1;
    } /* Endif */

}

_mqx_uint get_event_valid_field_fast
   (
      _mqx_uint event_index
   )
{
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    EVENT_COMPONENT_STRUCT_PTR  event_component_ptr;
    EVENT_STRUCT_PTR            event_ptr;
    _mqx_uint                   result;
    _mqx_max_type               tmp;

    _GET_KERNEL_DATA(kernel_data);                                         

    event_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
    result = _name_find_internal_by_index(event_component_ptr->NAME_TABLE_HANDLE, 
        event_index,&tmp);
    if (result == MQX_OK) {
        event_ptr = (EVENT_STRUCT_PTR)tmp;
        return event_ptr->VALID;
    } else {
        return 0;
    } /* Endif */

}

void set_event_valid_field_fast
   (
      _mqx_uint event_index,
      _mqx_uint valid_value
   )
{
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    EVENT_COMPONENT_STRUCT_PTR  event_component_ptr;
    EVENT_STRUCT_PTR            event_ptr;
    _mqx_uint                   result;
    _mqx_max_type               tmp;

    _GET_KERNEL_DATA(kernel_data);                                         

    event_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
    result = _name_find_internal_by_index(event_component_ptr->NAME_TABLE_HANDLE, 
        event_index,&tmp);
    if (result == MQX_OK) {
        event_ptr = (EVENT_STRUCT_PTR)tmp;
        event_ptr->VALID = valid_value; 
    } else {
        result = 1;
    } /* Endif */

}


_mqx_uint get_event_handle_valid_field
   (
      void   *users_event_ptr 
   )
{ 
    EVENT_CONNECTION_STRUCT_PTR event_connection_ptr = users_event_ptr;

    return event_connection_ptr->VALID;  

}

void set_event_handle_valid_field
   (
      void      *users_event_ptr,
      _mqx_uint  handle_field 
   )
{ 
    EVENT_CONNECTION_STRUCT_PTR event_connection_ptr = users_event_ptr;

    event_connection_ptr->VALID = handle_field;    

}

/* EOF */
