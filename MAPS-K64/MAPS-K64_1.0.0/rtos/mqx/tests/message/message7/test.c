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
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file contains code for the MQX message verification
*   program.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <message.h>
#include <msg_prv.h>
#include "test.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"


/*------------------------------------------------------------------------
 * Required user_config settings
 */
#if !MQX_CHECK_ERRORS
    #error "Required MQX_CHECK_ERRORS"
#endif
#if !MQX_CHECK_VALIDITY
    #error "Required MQX_CHECK_VALIDITY"
#endif
#if !MQXCFG_ENABLE_MSG_TIMEOUT_ERROR
    #error "Required MQXCFG_ENABLE_MSG_TIMEOUT_ERROR"
#endif
#if !MQX_USE_MESSAGES
    #error "Required MQX_USE_MESSAGES"
#endif


/*------------------------------------------------------------------------
 * Test suite function prototype
 */
void tc_1_main_task(void);  /* TEST # 1 - Testing _msgq_receive function with timeout */
void tc_2_main_task(void);  /* TEST # 2 - Testing _msgq_receive_ticks function with timeout */
void tc_3_main_task(void);  /* TEST # 3 - Testing _msgq_receive_for function with timeout */
void tc_4_main_task(void);  /* TEST # 4 - Testing _msgq_receive_until function with timeout */

/*------------------------------------------------------------------------
 * MQX task template
 */
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK,      main_task,      3000,  9, "main",    MQX_AUTO_START_TASK},
   {  SUB_TASK,       sub_task,      1000,  8,  "sub",                      0},
   {         0,              0,         0,  0,      0,                      0}
};

/*------------------------------------------------------------------------
 * MQX initialization structure
 */
MQX_INITIALIZATION_STRUCT  MQX_init_struct =
{
   /* PROCESSOR_NUMBER                */  BSP_DEFAULT_PROCESSOR_NUMBER,
   /* START_OF_KERNEL_MEMORY          */  BSP_DEFAULT_START_OF_KERNEL_MEMORY,
   /* END_OF_KERNEL_MEMORY            */  BSP_DEFAULT_END_OF_KERNEL_MEMORY,
   /* INTERRUPT_STACK_SIZE            */  BSP_DEFAULT_INTERRUPT_STACK_SIZE,
   /* TASK_TEMPLATE_LIST              */  (void *)MQX_template_list,
   /* MQX_HARDWARE_INTERRUPT_LEVEL_MAX*/  BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX,
   /* MAX_MSGPOOLS                    */  TEST_MAX_MSG_POOLS,
   /* MAX_MSGQS                       */  TEST_MAX_MSG_QUEUES,
   /* IO_CHANNEL                      */  BSP_DEFAULT_IO_CHANNEL,
#if MQX_USE_IO_OLD
   /* IO_OPEN_MODE                    */  BSP_DEFAULT_IO_OPEN_MODE
#else
   /* IO_OPEN_MODE                    */  0
#endif
};

_mqx_uint                  task_error;      /* Hold the task's error code */
void                      *msg_cmpnt;       /* Hold the message component info */
bool                    bool_result;     /* Hold the functions' result */

TEST_MSG_STRUCT_PTR        temp_msg_ptr;
TEST_MSG_STRUCT_PTR        msg_ptr1, msg_ptr2;
_queue_id                  private_qid, invalid_qid, temp_qid;
_task_id                   sub_tid;
uint32_t                    delay;

MQX_TICK_STRUCT            ticks;
MQX_TICK_STRUCT            start_ticks;
MQX_TICK_STRUCT            end_ticks;
MQX_TICK_STRUCT            diff_ticks;
TIME_STRUCT                time;


/******************** Begin Testcases **********************/

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
* The function _msgq_receive is to wait for a message to be available
* in the queue for the number of milliseconds.This test case is to verify
* the function shall work properly in normal operation.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    /* Set delay time in ticks */
    delay = 40;

    /* Create a system message pool successfully */
    bool_result = _msgpool_create_system(sizeof(TEST_MSG_STRUCT),1,1,0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, bool_result == TRUE, "Test #1: Testing 1.01: Create a system message pool successfully");

    /* Open a private message queue successfully */
    private_qid = _msgq_open(QUEUE_VALID_NUM,NUM_OF_MSG);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, private_qid != 0, "Test #1: Testing 1.02: Open a private message queue successfully");

    /* Allocate 2 system messages successfully */
    msg_ptr1 = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr1 != NULL, "Test #1: Testing 1.03: Allocate first message successfully");
    msg_ptr1->DATA1 = 0x00001111;

    msg_ptr2 = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr2 != NULL, "Test #1: Testing 1.04: Allocate second message successfully");
    msg_ptr2->DATA1 = 0x00002222;

    /* Send messages to queue successfully */
    bool_result = _msgq_send_queue(msg_ptr1,private_qid);
    bool_result &= _msgq_send_queue(msg_ptr2,private_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, bool_result == TRUE, "Test #1: Testing 1.05: Send message to queue successfully");

    /*[BEGIN] Receive messages successfully*/
    msg_ptr1 = _msgq_receive(private_qid,delay);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr1 != NULL, "Test #1: Testing 1.06: Receive messages from queue successfully");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr1->DATA1 == 0x00001111, "Test #1: Testing 1.07: Verify the message's data is correct");
    _task_set_error(MQX_OK);
    /*[END] Receive messages successfully*/

    /*[BEGIN] Try to receive message when system message component is invalid */
    invalidate_msg_component(&msg_cmpnt);
    temp_msg_ptr = _msgq_receive(private_qid,delay);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, temp_msg_ptr == NULL, "Test #1: Testing 1.08: Failed to receive message when message component is invalid");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MQX_COMPONENT_DOES_NOT_EXIST, "Test #1: Testing 1.09: The task's error code must be MQX_COMPONENT_DOES_NOT_EXIST");
    _task_set_error(MQX_OK);
    restore_msg_component(msg_cmpnt);
    /*[END] Try to receive message when system message component is invalid */

    /* Create another task and run */
    sub_tid = _task_create(0, SUB_TASK, 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, sub_tid != 0, "Test #1: Testing 1.10: Create task successfully");

    /*[BEGIN] Try to receive message from an invalid queue */
    invalid_qid = BUILD_QID(INVALID_PROCESSOR, QUEUE_VALID_NUM);
    temp_msg_ptr = _msgq_receive(invalid_qid,delay);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, temp_msg_ptr == NULL, "Test #1: Testing 1.11: Failed to receive message from an invalid queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MSGQ_INVALID_QUEUE_ID, "Test #1: Testing 1.12: The task's error code must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);
    /*[END] Try to receive message from an invalid queue */

    /*[BEGIN] Try to receive message from queue that task doesn't own*/
    temp_msg_ptr = _msgq_receive(temp_qid,delay);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, temp_msg_ptr == NULL, "Test #1: Testing 1.13: Failed to receive message from queue that task doesn't own");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MSGQ_NOT_QUEUE_OWNER, "Test #1: Testing 1.14: The task's error code must be MSGQ_NOT_QUEUE_OWNER");
    _task_set_error(MQX_OK);
    /*[END] Try to receive message from an invalid queue task doesn't own*/

    /* Receive the remaining message on private queue */
    msg_ptr2 = _msgq_receive(private_qid,delay);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr2 != NULL, "Test #1: Testing 1.15: Receive messages from queue successfully");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr2->DATA1 == 0x00002222, "Test #1: Testing 1.16: Verify the message's data is correct");

    /*[BEGIN] Try to receive message from empty queue*/
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    temp_msg_ptr = _msgq_receive(private_qid, delay);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();

    EU_ASSERT_REF_TC_MSG(tc_1_main_task, temp_msg_ptr == NULL, "Test #1: Testing 1.17: Failed to receive message from empty queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MSGQ_MESSAGE_NOT_AVAILABLE, "Test #1: Testing 1.18: The task's error code must be MSGQ_MESSAGE_NOT_AVAILABLE");
    _time_diff_ticks(&end_ticks, &start_ticks, &diff_ticks);
    _ticks_to_time(&diff_ticks, &time);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, time_check_struct(time, 0, delay, 1), "Test #1: Testing 1.19: Verify the waiting time must be correct");
    _task_set_error(MQX_OK);
    /*[END] Try to receive message from empty queue*/

    /* Free all messages */
    _msg_free(msg_ptr1);
    _msg_free(msg_ptr2);
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (task_error == MQX_OK), "Test #1: Testing 1.20: Free all messages successfully");

    /* Close all queues */
    bool_result = _msgq_close(private_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, bool_result == TRUE, "Test #1: Testing 1.21: Close all queues successfully");

    /* Destroy sub task */
    task_error = _task_destroy(sub_tid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MQX_OK, "Test #1: Testing 1.22: Destroy sub task successfully");

    /*[BEGIN] Try to receive message from closed queue*/
    temp_msg_ptr = _msgq_receive(private_qid, delay);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, temp_msg_ptr == NULL, "Test #1: Testing 1.23: Failed to receive message from closed queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MSGQ_QUEUE_IS_NOT_OPEN, "Test #1: Testing 1.24: The task's error code must be MSGQ_QUEUE_IS_NOT_OPEN");
    /*[END] Try to receive message from closed queue*/
} /* End tc_1_main_task */

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
* The function _msgq_receive_ticks is to wait for a message to be available
* in the queue for the number of ticks (in tick time). This test case is to
* verify the function shall work properly in normal operation.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    /* Set delay time in ticks */
    delay = 40;
    uint32_t start_time, end_time;

    /* Create a system message pool successfully */
    bool_result = _msgpool_create_system(sizeof(TEST_MSG_STRUCT),1,1,0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == TRUE, "Test #2: Testing 2.01: Create a system message pool successfully");

    /* Open a private message queue successfully */
    private_qid = _msgq_open(QUEUE_VALID_NUM,NUM_OF_MSG);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, private_qid != 0, "Test #2: Testing 2.02: Open a private message queue successfully");

    /* Allocate 2 system messages successfully */
    msg_ptr1 = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr1 != NULL, "Test #2: Testing 2.03: Allocate first message successfully");
    msg_ptr1->DATA1 = 0x00001111;

    msg_ptr2 = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr2 != NULL, "Test #2: Testing 2.04: Allocate second message successfully");
    msg_ptr2->DATA1 = 0x00002222;

    /* Send messages to queue successfully */
    bool_result = _msgq_send_queue(msg_ptr1,private_qid);
    bool_result &= _msgq_send_queue(msg_ptr2,private_qid);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == TRUE, "Test #2: Testing 2.05: Send message to queue successfully");

    /*[BEGIN] Receive messages successfully*/
    msg_ptr1 = _msgq_receive_ticks(private_qid,delay);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr1 != NULL, "Test #2: Testing 2.06: Receive messages from queue successfully");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr1->DATA1 == 0x00001111, "Test #2: Testing 2.07: Verify the message's data is correct");
    _task_set_error(MQX_OK);
    /*[END] Receive messages successfully*/

    /*[BEGIN] Try to receive message when system message component is invalid */
    invalidate_msg_component(&msg_cmpnt);
    temp_msg_ptr = _msgq_receive_ticks(private_qid,delay);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, temp_msg_ptr == NULL, "Test #2: Testing 2.08: Failed to receive message when message component is invalid");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, task_error == MQX_COMPONENT_DOES_NOT_EXIST, "Test #2: Testing 2.09: The task's error code must be MQX_COMPONENT_DOES_NOT_EXIST");
    _task_set_error(MQX_OK);
    restore_msg_component(msg_cmpnt);
    /*[END] Try to receive message when system message component is invalid */

    /* Create another task and run */
    sub_tid = _task_create(0, SUB_TASK, 0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, sub_tid != 0, "Test #2: Testing 2.10: Create task successfully");

    /*[BEGIN] Try to receive message from an invalid queue */
    invalid_qid = BUILD_QID(INVALID_PROCESSOR, QUEUE_VALID_NUM);
    temp_msg_ptr = _msgq_receive_ticks(invalid_qid,delay);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, temp_msg_ptr == NULL, "Test #2: Testing 2.11: Failed to receive message from an invalid queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, task_error == MSGQ_INVALID_QUEUE_ID, "Test #2: Testing 2.12: The task's error code must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);
    /*[END] Try to receive message from an invalid queue */

    /*[BEGIN] Try to receive message from queue that task doesn't own*/
    temp_msg_ptr = _msgq_receive_ticks(temp_qid,delay);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, temp_msg_ptr == NULL, "Test #2: Testing 2.13: Failed to receive message from queue that task doesn't own");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, task_error == MSGQ_NOT_QUEUE_OWNER, "Test #2: Testing 2.14: The task's error code must be MSGQ_NOT_QUEUE_OWNER");
    _task_set_error(MQX_OK);
    /*[END] Try to receive message from an invalid queue task doesn't own*/

    /* Receive the remaining message on private queue */
    msg_ptr2 = _msgq_receive_ticks(private_qid,delay);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr2 != NULL, "Test #2: Testing 2.15: Receive messages from queue successfully");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr2->DATA1 == 0x00002222, "Test #2: Testing 2.16: Verify the message's data is correct");

    /*[BEGIN] Try to receive message from empty queue*/
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    temp_msg_ptr = _msgq_receive_ticks(private_qid, delay);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();

    EU_ASSERT_REF_TC_MSG(tc_2_main_task, temp_msg_ptr == NULL, "Test #1: Testing 2.17: Failed to receive message from empty queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (task_error == MSGQ_MESSAGE_NOT_AVAILABLE), "Test #2: Testing 2.18: The task's error code must be MSGQ_MESSAGE_NOT_AVAILABLE");
    start_time = PSP_GET_ELEMENT_FROM_TICK_STRUCT(&start_ticks,0);
    end_time = PSP_GET_ELEMENT_FROM_TICK_STRUCT(&end_ticks,0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, delay == (end_time - start_time), "Test #2: Testing 2.3: Verify the waiting time must be correct");
    _task_set_error(MQX_OK);
    /*[END] Try to receive message from empty queue*/

    /* Free all messages */
    _msg_free(msg_ptr1);
    _msg_free(msg_ptr2);
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (task_error == MQX_OK), "Test #2: Testing 2.20: Free all messages successfully");

    /* Close all queues */
    bool_result = _msgq_close(private_qid);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == TRUE, "Test #2: Testing 2.21: Close all queues successfully");

    /* Destroy sub task */
    task_error = _task_destroy(sub_tid);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, task_error == MQX_OK, "Test #2: Testing 2.22: Destroy sub task successfully");

    /*[BEGIN] Try to receive message from closed queue*/
    temp_msg_ptr = _msgq_receive_ticks(private_qid, delay);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, temp_msg_ptr == NULL, "Test #2: Testing 2.23: Failed to receive message from closed queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (task_error == MSGQ_QUEUE_IS_NOT_OPEN), "Test #2: Testing 2.24: The task's error code must be MSGQ_QUEUE_IS_NOT_OPEN");
    /*[END] Try to receive message from closed queue*/
} /* End tc_2_main_task */

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
* The function _msgq_receive_for is to wait for a message to be available
* in the queue for the number of ticks (in tick time). This test case is to
* verify the function shall work properly in normal operation.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    /* Set time delay */
    uint32_t start_time, end_time;
    
    delay = 40;
    _time_init_ticks(&ticks, delay);

    /* Create a system message pool successfully */
    bool_result = _msgpool_create_system(sizeof(TEST_MSG_STRUCT),1,1,0);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == TRUE, "Test #3: Testing 3.01: Create a system message pool successfully");

    /* Open a private message queue successfully */
    private_qid = _msgq_open(QUEUE_VALID_NUM,NUM_OF_MSG);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, private_qid != 0, "Test #3: Testing 3.02: Open a private message queue successfully");

    /* Allocate 2 system messages successfully */
    msg_ptr1 = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr1 != NULL, "Test #3: Testing 3.03: Allocate first message successfully");
    msg_ptr1->DATA1 = 0x00001111;

    msg_ptr2 = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr2 != NULL, "Test #3: Testing 3.04: Allocate second message successfully");
    msg_ptr2->DATA1 = 0x00002222;

    /* Send messages to queue successfully */
    bool_result = _msgq_send_queue(msg_ptr1,private_qid);
    bool_result &= _msgq_send_queue(msg_ptr2,private_qid);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == TRUE, "Test #3: Testing 3.05: Send message to queue successfully");

    /*[BEGIN] Receive messages successfully*/
    msg_ptr1 = _msgq_receive_for(private_qid, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr1 != NULL, "Test #3: Testing 3.06: Receive messages from queue successfully");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr1->DATA1 == 0x00001111, "Test #3: Testing 3.07: Verify the message's data is correct");
    _task_set_error(MQX_OK);
    /*[END] Receive messages successfully*/

    /*[BEGIN] Try to receive message when system message component is invalid */
    invalidate_msg_component(&msg_cmpnt);
    temp_msg_ptr = _msgq_receive_for(private_qid, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, temp_msg_ptr == NULL, "Test #3: Testing 3.08: Failed to receive message when message component is invalid");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, task_error == MQX_COMPONENT_DOES_NOT_EXIST, "Test #3: Testing 3.09: The task's error code must be MQX_COMPONENT_DOES_NOT_EXIST");
    _task_set_error(MQX_OK);
    restore_msg_component(msg_cmpnt);
    /*[END] Try to receive message when system message component is invalid */

    /* Create another task and run */
    sub_tid = _task_create(0, SUB_TASK, 0);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, sub_tid != 0, "Test #3: Testing 3.10: Create task successfully");

    /*[BEGIN] Try to receive message from an invalid queue */
    invalid_qid = BUILD_QID(INVALID_PROCESSOR, QUEUE_VALID_NUM);
    temp_msg_ptr = _msgq_receive_for(invalid_qid, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, temp_msg_ptr == NULL, "Test #3: Testing 3.11: Failed to receive message from an invalid queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, task_error == MSGQ_INVALID_QUEUE_ID, "Test #3: Testing 3.12: The task's error code must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);
    /*[END] Try to receive message from an invalid queue */

    /*[BEGIN] Try to receive message from queue that task doesn't own*/
    temp_msg_ptr = _msgq_receive_for(temp_qid, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, temp_msg_ptr == NULL, "Test #3: Testing 3.13: Failed to receive message from queue that task doesn't own");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, task_error == MSGQ_NOT_QUEUE_OWNER, "Test #3: Testing 3.14: The task's error code must be MSGQ_NOT_QUEUE_OWNER");
    _task_set_error(MQX_OK);
    /*[END] Try to receive message from an invalid queue task doesn't own*/

    /* Receive the remaining message on private queue */
    msg_ptr2 = _msgq_receive_for(private_qid, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr2 != NULL, "Test #3: Testing 3.15: Receive messages from queue successfully");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr2->DATA1 == 0x00002222, "Test #3: Testing 3.16: Verify the message's data is correct");

    /*[BEGIN] Try to receive message from empty queue*/
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    temp_msg_ptr = _msgq_receive_for(private_qid, &ticks);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();

    EU_ASSERT_REF_TC_MSG(tc_3_main_task, temp_msg_ptr == NULL, "Test #3: Testing 3.17: Failed to receive message from empty queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, task_error == MSGQ_MESSAGE_NOT_AVAILABLE, "Test #3: Testing 3.18: The task's error code must be MSGQ_MESSAGE_NOT_AVAILABLE");
    start_time = PSP_GET_ELEMENT_FROM_TICK_STRUCT(&start_ticks,0);
    end_time = PSP_GET_ELEMENT_FROM_TICK_STRUCT(&end_ticks,0);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, delay == (end_time - start_time), "Test #3: Testing 3.19: Verify the waiting time must be correct");
    _task_set_error(MQX_OK);
    /*[END] Try to receive message from empty queue*/

    /* Free all messages */
    _msg_free(msg_ptr1);
    _msg_free(msg_ptr2);
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (task_error == MQX_OK), "Test #3: Testing 3.20: Free all messages successfully");

    /* Close all queues */
    bool_result = _msgq_close(private_qid);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == TRUE, "Test #3: Testing 3.21: Close all queues successfully");

    /* Destroy sub task */
    task_error = _task_destroy(sub_tid);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, task_error == MQX_OK, "Test #3: Testing 3.22: Destroy sub task successfully");

    /*[BEGIN] Try to receive message from closed queue*/
    temp_msg_ptr = _msgq_receive_for(private_qid, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, temp_msg_ptr == NULL, "Test #3: Testing 3.23: Failed to receive message from closed queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (task_error == MSGQ_QUEUE_IS_NOT_OPEN), "Test #3: Testing 3.24: The task's error code must be MSGQ_QUEUE_IS_NOT_OPEN");
    /*[END] Try to receive message from closed queue*/
} /* End tc_3_main_task */

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
* The function _msgq_receive_until is to wait for a message to be available
* in the queue until the specified time (in tick time). This test case is to
* verify the function shall work properly in normal operation.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    /* Set time delay */
    _time_get_elapsed_ticks(&start_ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&start_ticks, 1000, &ticks);

    /* Create a system message pool successfully */
    bool_result = _msgpool_create_system(sizeof(TEST_MSG_STRUCT),1,1,0);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result == TRUE, "Test #4: Testing 4.01: Create a system message pool successfully");

    /* Open a private message queue successfully */
    private_qid = _msgq_open(QUEUE_VALID_NUM,NUM_OF_MSG);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, private_qid != 0, "Test #4: Testing 4.02: Open a private message queue successfully");

    /* Allocate 2 system messages successfully */
    msg_ptr1 = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, msg_ptr1 != NULL, "Test #4: Testing 4.03: Allocate first message successfully");
    msg_ptr1->DATA1 = 0x00001111;

    msg_ptr2 = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, msg_ptr2 != NULL, "Test #4: Testing 4.04: Allocate second message successfully");
    msg_ptr2->DATA1 = 0x00002222;

    /* Send messages to queue successfully */
    bool_result = _msgq_send_queue(msg_ptr1,private_qid);
    bool_result &= _msgq_send_queue(msg_ptr2,private_qid);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result == TRUE, "Test #4: Testing 4.05: Send message to queue successfully");

    /*[BEGIN] Receive messages successfully*/
    msg_ptr1 = _msgq_receive_until(private_qid, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, msg_ptr1 != NULL, "Test #4: Testing 4.06: Receive messages from queue successfully");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, msg_ptr1->DATA1 == 0x00001111, "Test #4: Testing 4.07: Verify the message's data is correct");
    _task_set_error(MQX_OK);
    /*[END] Receive messages successfully*/

    /*[BEGIN] Try to receive message when system message component is invalid */
    invalidate_msg_component(&msg_cmpnt);
    temp_msg_ptr = _msgq_receive_until(private_qid, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, temp_msg_ptr == NULL, "Test #4: Testing 4.08: Failed to receive message when message component is invalid");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error == MQX_COMPONENT_DOES_NOT_EXIST, "Test #4: Testing 4.09: The task's error code must be MQX_COMPONENT_DOES_NOT_EXIST");
    _task_set_error(MQX_OK);
    restore_msg_component(msg_cmpnt);
    /*[END] Try to receive message when system message component is invalid */

    /* Create another task and run */
    sub_tid = _task_create(0, SUB_TASK, 0);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, sub_tid != 0, "Test #4: Testing 4.10: Create task successfully");

    /*[BEGIN] Try to receive message from an invalid queue */
    invalid_qid = BUILD_QID(INVALID_PROCESSOR, QUEUE_VALID_NUM);
    temp_msg_ptr = _msgq_receive_until(invalid_qid, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, temp_msg_ptr == NULL, "Test #4: Testing 4.11: Failed to receive message from an invalid queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error == MSGQ_INVALID_QUEUE_ID, "Test #4: Testing 4.12: The task's error code must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);
    /*[END] Try to receive message from an invalid queue */

    /*[BEGIN] Try to receive message from queue that task doesn't own*/
    temp_msg_ptr = _msgq_receive_until(temp_qid, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, temp_msg_ptr == NULL, "Test #4: Testing 4.13: Failed to receive message from queue that task doesn't own");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error == MSGQ_NOT_QUEUE_OWNER, "Test #4: Testing 4.14: The task's error code must be MSGQ_NOT_QUEUE_OWNER");
    _task_set_error(MQX_OK);
    /*[END] Try to receive message from an invalid queue task doesn't own*/

    /* Receive the remaining message on private queue */
    msg_ptr2 = _msgq_receive_until(private_qid, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, msg_ptr2 != NULL, "Test #4: Testing 4.15: Receive messages from queue successfully");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, msg_ptr2->DATA1 == 0x00002222, "Test #4: Testing 4.16: Verify the message's data is correct");

    /*[BEGIN] Try to receive message from empty queue*/
    _int_disable();
    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    PSP_ADD_TICKS_TO_TICK_STRUCT(&start_ticks, delay, &ticks);
    temp_msg_ptr = _msgq_receive_until(private_qid, &ticks);
    _time_get_elapsed_ticks(&end_ticks);
    _int_enable();

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, temp_msg_ptr == NULL, "Test #4: Testing 4.17: Failed to receive message from empty queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error == MSGQ_MESSAGE_NOT_AVAILABLE, "Test #4: Testing 4.18: The task's error code must be MSGQ_MESSAGE_NOT_AVAILABLE");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, 0 == PSP_CMP_TICKS(&end_ticks, &ticks), "Test #4: Testing 4.19: Verify the waiting time must be correct");
    _task_set_error(MQX_OK);
    /*[END] Try to receive message from empty queue*/

    /* Free all messages */
    _msg_free(msg_ptr1);
    _msg_free(msg_ptr2);
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (task_error == MQX_OK), "Test #4: Testing 4.20: Free all messages successfully");

    /* Close all queues */
    bool_result = _msgq_close(private_qid);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result == TRUE, "Test #4: Testing 4.21: Close all queues successfully");

    /* Destroy sub task */
    task_error = _task_destroy(sub_tid);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error == MQX_OK, "Test #4: Testing 4.22: Destroy sub task successfully");

    /*[BEGIN] Try to receive message from closed queue*/
    temp_msg_ptr = _msgq_receive_until(private_qid, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, temp_msg_ptr == NULL, "Test #4: Testing 4.23: Failed to receive message from closed queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (task_error == MSGQ_QUEUE_IS_NOT_OPEN), "Test #4: Testing 4.24: The task's error code must be MSGQ_QUEUE_IS_NOT_OPEN");
    /*[END] Try to receive message from closed queue*/
} /* End tc_4_main_task */

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_7)
    EU_TEST_CASE_ADD(tc_1_main_task, "TEST # 1 - Testing _msgq_receive function with timeout"),
    EU_TEST_CASE_ADD(tc_2_main_task, "TEST # 2 - Testing _msgq_receive_ticks function with timeout"),
    EU_TEST_CASE_ADD(tc_3_main_task, "TEST # 3 - Testing _msgq_receive_for function with timeout"),
    EU_TEST_CASE_ADD(tc_4_main_task, "TEST # 4 - Testing _msgq_receive_until function with timeout")
EU_TEST_SUITE_END(suite_7)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_7, " - MQX Message Component Test Suite, 4 Tests")
EU_TEST_REGISTRY_END()
/*------------------------------------------------------------------------*/

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : sub_task
* Comments     : 
*    This task is used to create another private message queue
*
*END*----------------------------------------------------------------------*/
void sub_task
   (
      uint32_t param
      /* [IN] The MQX task parameter */
   )
{ /* Body */
    temp_qid = _msgq_open(QUEUE_VALID_NUM_2,NUM_OF_MSG);
    _task_block();
} /* endbody */

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
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();

    _mqx_exit(0);
} /* endbody */

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : main_task
* Comments     : 
*    This is the main task to the MQX Kernel program
*
*END*----------------------------------------------------------------------*/

/* EOF */
