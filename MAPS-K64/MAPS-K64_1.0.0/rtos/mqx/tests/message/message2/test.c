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
*   This file contains code for the MQX message verification
*   program.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <message.h>
#include "test.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME test

//------------------------------------------------------------------------
// Test suite function prototype
extern void tc_0_main_task(void);// TEST # 0 - Create message component, message pool, message queue
extern void tc_1_main_task(void);// TEST # 1 - Testing _msgq_receive() primitive with timeout
extern void tc_2_main_task(void);// TEST # 2 - Testing _msgq_receive_ticks() primitive with timeout
extern void tc_3_main_task(void);// TEST # 3 - Testing _msgq_receive_for() primitive with timeout
extern void tc_4_main_task(void);// TEST # 4 - Testing _msgq_receive_until() primitive with timeout
//------------------------------------------------------------------------                                
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK,      main_task,      2000,  9, "main",    MQX_AUTO_START_TASK},
   { 0,              0,              0,     0, 0,         0}
};

 MQX_TICK_STRUCT      ticks;
 MQX_TICK_STRUCT      start_ticks;
 MQX_TICK_STRUCT      end_ticks;
 MQX_TICK_STRUCT      diff_ticks;
 TEST_MSG_STRUCT_PTR  msg_ptr;
 _queue_id queue;
//b23362
// uint32_t delay;
 uint64_t delay;
//b23362 
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_0_main_task
// Comments     : TEST #0: Create message component, message pool, message queue
//
//END---------------------------------------------------------------------

void tc_0_main_task(void) {
    _mqx_uint   result;
    _pool_id    pool;
    
    /*
    ** create the message component
    ** verify the integrity of msg pools and msg queues
    */
    result = _msg_create_component();
    EU_ASSERT_REF_TC_MSG(tc_0_main_task, (result == MQX_OK), "Test #0 Testing: 0.1: create message component operation")
    if (result != MQX_OK) {
        eunit_test_stop();
        _mqx_exit(1);
    } 

    /* 
    ** create a msg pool, create a msg queue
    */
    pool  = _msgpool_create(sizeof(TEST_MSG_STRUCT),1,0,0);
    EU_ASSERT_REF_TC_MSG(tc_0_main_task, (pool != MSGPOOL_NULL_POOL_ID), "Test #0 Testing: 0.2: create message pool operation")
    if (pool == MSGPOOL_NULL_POOL_ID) {
        eunit_test_stop();
        _mqx_exit(1);
    } 
    queue = _msgq_open(0,0);
    EU_ASSERT_REF_TC_MSG(tc_0_main_task, (queue != MSGQ_NULL_QUEUE_ID), "Test #0 Testing: 0.3: create message queue operation")
    if (queue == MSGQ_NULL_QUEUE_ID) {
        eunit_test_stop();
        _mqx_exit(1);
    }
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST #1: Testing _msgq_receive() primitive with timeout
//
//END---------------------------------------------------------------------

void tc_1_main_task(void)
{
    _mqx_int        result;
    TIME_STRUCT     time;
    delay = 40;
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    msg_ptr = _msgq_receive(queue, delay);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();
    result = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MSGQ_MESSAGE_NOT_AVAILABLE), "Test #1 Testing: 1.1: _msgq_receive should be failed and the error should be MSGQ_MESSAGE_NOT_AVAILABLE")
    _task_set_error(MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (msg_ptr == NULL), "Test #1 Testing: 1.2: _msgq_receive should return a NULL msg pointer")
    if (msg_ptr) {
        eunit_test_stop();
        _mqx_exit(1);  
    }

    _time_diff_ticks(&end_ticks, &start_ticks, &diff_ticks);
    _ticks_to_time(&diff_ticks, &time);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, time_check_struct(time, 0, delay, 1), "Test #1 Testing: 1.3: _msgq_receive should return correct time")
    if (! time_check_struct(time, 0, delay, 1)) {
        eunit_test_stop();
        _mqx_exit(1);  
    }
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST #2: Testing _msgq_receive_ticks() primitive with timeout
//
//END---------------------------------------------------------------------

void tc_2_main_task(void)
{
    _mqx_int result;
    delay = 40;
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    msg_ptr = _msgq_receive_ticks(queue, (_mqx_uint)delay);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();
    result = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MSGQ_MESSAGE_NOT_AVAILABLE), "Test #2 Testing: 2.1: _msgq_receive_ticks should be failed and error should be MSGQ_MESSAGE_NOT_AVAILABLE")
    if (result != MSGQ_MESSAGE_NOT_AVAILABLE) {
        eunit_test_stop();
        _mqx_exit(1);  
    }
    _task_set_error(MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (msg_ptr == NULL), "Test #2 Testing: 2.2: _msgq_receive_ticks should return a NULL msg pointer")
    if (msg_ptr) {
        eunit_test_stop();
        _mqx_exit(1);  
    }

    _time_diff_ticks(&end_ticks, &start_ticks, &diff_ticks);
    result = PSP_GET_ELEMENT_FROM_TICK_STRUCT(&diff_ticks,0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == delay), "Test #2 Testing: 2.3: _msgq_receive_ticks should return correct time")
    if (result != delay) {
        eunit_test_stop();
        _mqx_exit(1);  
    }
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST #3: Testing _msgq_receive_for() primitive with timeout
//
//END---------------------------------------------------------------------

void tc_3_main_task(void)
{
    _mqx_int result;
    ticks = diff_ticks;
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    msg_ptr = _msgq_receive_for(queue, &ticks);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();
    result = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MSGQ_MESSAGE_NOT_AVAILABLE), "Test #3 Testing: 3.1: _msgq_receive_for should be failed and error should be MSGQ_MESSAGE_NOT_AVAILABLE")
    if (result != MSGQ_MESSAGE_NOT_AVAILABLE) {
        eunit_test_stop();
        _mqx_exit(1);  
    }
    _task_set_error(MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (msg_ptr == NULL), "Test #3 Testing: 3.2: _msgq_receive_for should return a NULL msg pointer")
    if (msg_ptr) {
        eunit_test_stop();
        _mqx_exit(1);  
    }

    _time_diff_ticks(&end_ticks, &start_ticks, &diff_ticks);
    result = PSP_CMP_TICKS(&diff_ticks, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == 0), "Test #3 Testing: 3.3: _msgq_receive_for should return correc time")
    if (result != 0) {
        eunit_test_stop();
        _mqx_exit(1);  
    }
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST #4: Testing _msgq_receive_until() primitive with timeout
//
//END---------------------------------------------------------------------

void tc_4_main_task(void)
{
    _mqx_int result;
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&start_ticks, delay, &ticks);
    msg_ptr = _msgq_receive_until(queue, &ticks);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();
    result = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MSGQ_MESSAGE_NOT_AVAILABLE), "Test #4 Testing: 4.1: _msgq_receive_until should be failed and error should be MSGQ_MESSAGE_NOT_AVAILABLE")
    if (result != MSGQ_MESSAGE_NOT_AVAILABLE) {
        eunit_test_stop();
        _mqx_exit(1);  
    }
    _task_set_error(MQX_OK);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (msg_ptr == NULL), "Test #4 Testing: 4.2: _msgq_receive_until should return a NULL msg pointer")
    if (msg_ptr) {
        eunit_test_stop();
        _mqx_exit(1);  
    }

    result = PSP_CMP_TICKS(&end_ticks, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == 0), "Test #4 Testing: 4.3: _msgq_receive_until should return correct time")
    if ((_mqx_int)result != 0) {
        eunit_test_stop();
        _mqx_exit(1);  
    }
}

//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_2)
    EU_TEST_CASE_ADD(tc_0_main_task, " TEST # 0 - Creating message component, message pool, message queue"),
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1 - Testing _msgq_receive() primitive with timeout"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2 - Testing _msgq_receive_ticks() primitive with timeout"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3 - Testing _msgq_receive_for() primitive with timeout"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4 - Testing _msgq_receive_until() primitive with timeout")
EU_TEST_SUITE_END(suite_2)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_2, " - Testing message 2")
 EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------
 
/*TASK*-------------------------------------------------------------------
* 
* Task Name    : main_task
* Comments     : 
*    This is the main task to the MQX Kernel program
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t param
      /* [IN] The MQX task parameter */
   )
{ /* Body */
    _time_set_hwtick_function(0,0);
    
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* endbody */



/* EOF */
