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
void tc_1_main_task(void);  /* TEST # 1 - Testing peek and poll message queue */
void tc_2_main_task(void);  /* TEST # 2 - Testing swap message endian */
void tc_3_main_task(void);  /* TEST # 3 - Testing validate messages */
void tc_4_main_task(void);  /* TEST # 4 - Testing deallocate messages */

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
   /* IO_OPEN_MODE                    */  BSP_DEFAULT_IO_OPEN_MODE
};

/*------------------------------------------------------------------------
 * Global variables
 */
_mqx_uint           task_error;     /* Hold the task's error code */
void               *msg_cmpnt;      /* Hold the message component info */
bool             bool_result;    /* Hold the functions' result */
_queue_id           temp_qid;       /* Global message queue */

/******************** Begin Test cases **********************/

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
* The function _msgq_peek is used to get a pointer to the message that is
* at the start of the message queue, but do not remove the message.
* The function _msgq_poll is used to poll the message queue for a message,
* but do not wait if a message is not in the queue.
* This test case is to verify those function shall work properly.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    TEST_MSG_STRUCT_PTR msg_ptr1, msg_ptr2;
    TEST_MSG_STRUCT_PTR temp_msg_ptr;
    _queue_id           private_qid, system_qid;
    _queue_id           invalid_qid;
    _task_id            sub_tid;

    /* Create a system message pool */
    bool_result = _msgpool_create_system(sizeof(TEST_MSG_STRUCT),1,1,0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, bool_result == TRUE, "Test #1: Testing 1.01: Create a system message pool successfully");

    /* Open a private message queue */
    private_qid = _msgq_open(QUEUE_VALID_NUM,NUM_OF_MSG);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, private_qid != 0, "Test #1: Testing 1.02: Open a private message queue successfully");

    /* Open a system message queue */
    system_qid = _msgq_open_system(QUEUE_VALID_NUM_2,NUM_OF_MSG,NULL,NULL);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, private_qid != 0, "Test #1: Testing 1.03: Open a system message queue successfully");

    /* Allocate 2 system messages */
    msg_ptr1 = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr1 != NULL, "Test #1: Testing 1.04: Allocate first message successfully");
    msg_ptr1->DATA2 = 0x0011;

    msg_ptr2 = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr2 != NULL, "Test #1: Testing 1.05: Allocate second message successfully");
    msg_ptr2->DATA2 = 0x0022;

    /* Send messages to private queue and system queue */
    bool_result = _msgq_send_queue(msg_ptr1,private_qid);
    bool_result &= _msgq_send_queue(msg_ptr2,system_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, bool_result == TRUE, "Test #1: Testing 1.06: Send messages to queues successfully");

    /*[BEGIN] Peek and poll message from queue successfully */
    msg_ptr1 = _msgq_peek(private_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr1 != NULL, "Test #1: Testing 1.07: Peek message from private queue successfully");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr1->DATA2 == 0x0011, "Test #1: Testing 1.08: Verify message data");

    msg_ptr2 = _msgq_peek(system_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr2 != NULL, "Test #1: Testing 1.09: Peek message from system queue successfully");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr2->DATA2 == 0x0022, "Test #1: Testing 1.10: Verify message data");

    msg_ptr1 = _msgq_poll(private_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr1 != NULL, "Test #1: Testing 1.11: Poll message from private queue successfully");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr1->DATA2 == 0x0011, "Test #1: Testing 1.12: Verify message data");

    msg_ptr2 = _msgq_poll(system_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr2 != NULL, "Test #1: Testing 1.13: Poll message from system queue successfully");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr2->DATA2 == 0x0022, "Test #1: Testing 1.14: Verify message data");
    /*[END] Peek and poll message from queue successfully */

    /*[BEGIN] Try to peek and poll message when system message component is invalid */
    invalidate_msg_component(&msg_cmpnt);

    temp_msg_ptr = _msgq_peek(private_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, temp_msg_ptr == NULL, "Test #1: Testing 1.15: Failed to peek message when message component is invalid");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MQX_COMPONENT_DOES_NOT_EXIST, "Test #1: Testing 1.16: The task's error code must be MQX_COMPONENT_DOES_NOT_EXIST");
    _task_set_error(MQX_OK);
    
    temp_msg_ptr = _msgq_poll(system_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, temp_msg_ptr == NULL, "Test #1: Testing 1.17: Failed to poll message when message component is invalid");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MQX_COMPONENT_DOES_NOT_EXIST, "Test #1: Testing 1.18: The task's error code must be MQX_COMPONENT_DOES_NOT_EXIST");
    _task_set_error(MQX_OK);

    restore_msg_component(msg_cmpnt);
    /*[END] Try to peek and poll message when system message component is invalid */

    /*[BEGIN] Try to peek and poll message from empty queues */
    temp_msg_ptr = _msgq_peek(private_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, temp_msg_ptr == NULL, "Test #1: Testing 1.19: Failed to peek message from empty queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MSGQ_MESSAGE_NOT_AVAILABLE, "Test #1: Testing 1.20:  The task's error code must be MSGQ_MESSAGE_NOT_AVAILABLE");
    _task_set_error(MQX_OK);

    temp_msg_ptr = _msgq_poll(system_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, temp_msg_ptr == NULL, "Test #1: Testing 1.21: Failed to poll message from empty queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MSGQ_MESSAGE_NOT_AVAILABLE, "Test #1: Testing 1.22: The task's error code must be MSGQ_MESSAGE_NOT_AVAILABLE");
    _task_set_error(MQX_OK);
    /*[END] Try to peek and poll message from empty queues */

    /*[BEGIN] Try to peek and poll message from invalid queues */
    invalid_qid = BUILD_QID(INVALID_PROCESSOR, QUEUE_VALID_NUM);
    temp_msg_ptr = _msgq_peek(invalid_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, temp_msg_ptr == NULL, "Test #1: Testing 1.23: Failed to peek message from invalid queues");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MSGQ_INVALID_QUEUE_ID, "Test #1: Testing 1.24: The task's error code must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);

    temp_msg_ptr = _msgq_poll(invalid_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, temp_msg_ptr == NULL, "Test #1: Testing 1.25: Failed to poll message from invalid queues");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MSGQ_INVALID_QUEUE_ID, "Test #1: Testing 1.26: The task's error code must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);
    /*[END] Try to peek and poll message from invalid queues */

    /*[BEGIN] Try to peek and poll message from the queue that task doesn't own */
    sub_tid = _task_create(0, SUB_TASK, 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, sub_tid != 0, "Test #1: Testing 1.27: Create sub task successfully");

    invalid_qid = BUILD_QID(INVALID_PROCESSOR, QUEUE_VALID_NUM);
    temp_msg_ptr = _msgq_peek(temp_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, temp_msg_ptr == NULL, "Test #1: Testing 1.28: Failed to peek message from the queue that task doesn't own");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MSGQ_NOT_QUEUE_OWNER, "Test #1: Testing 1.29: The task's error code must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);

    temp_msg_ptr = _msgq_poll(temp_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, temp_msg_ptr == NULL, "Test #1: Testing 1.30: Failed to poll message from the queue that task doesn't own");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MSGQ_NOT_QUEUE_OWNER, "Test #1: Testing 1.31: The task's error code must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);
    /*[END] Try to peek and poll message from the queue that task doesn't own */

    /* Free all messages */
    _msg_free(msg_ptr1);
    _msg_free(msg_ptr2);
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MQX_OK, "Test #1: Testing 1.32: Free all messages successfully");

    /* Close all queues */
    bool_result = _msgq_close(private_qid);
    bool_result &= _msgq_close(system_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, bool_result == TRUE, "Test #1: Testing 1.33: Close all queues successfully");

    /* Destroy sub task */
    task_error = _task_destroy(sub_tid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MQX_OK, "Test #1: Testing 1.34: Destroy sub task successfully");

    /*[BEGIN] Try to peek and poll message from closed queue*/
    temp_msg_ptr = _msgq_peek(private_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, temp_msg_ptr == NULL, "Test #1: Testing 1.35: Failed to peek message from closed queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MSGQ_QUEUE_IS_NOT_OPEN, "Test #1: Testing 1.36: The task's error code must be MSGQ_QUEUE_IS_NOT_OPEN");
    _task_set_error(MQX_OK);
    
    temp_msg_ptr = _msgq_poll(system_qid);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, temp_msg_ptr == NULL, "Test #1: Testing 1.37: Failed to poll message from closed queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error == MSGQ_QUEUE_IS_NOT_OPEN, "Test #1: Testing 1.38: The task's error code must be MSGQ_QUEUE_IS_NOT_OPEN");
    _task_set_error(MQX_OK);
    /*[END] Try to peek and poll message from closed queue*/
} /* End tc_1_main_task */

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
* The function _msg_swap_endian_header and _msg_swap_endian_data are used to
* convert the messages' header and messages' data to the other endian format.
* This test case is to verify those function shall work properly.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    TEST_MSG_STRUCT_PTR msg_ptr;

    /* Message data size structure */
    unsigned char data_type[] = {4,2,1,2,4,0};

    /* Allocate message and assign its data */
    msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr != NULL, "Test #2: Testing 2.01: Allocate first message successfully");
    msg_ptr->HEADER.SOURCE_QID = 0xAABB;
    msg_ptr->HEADER.TARGET_QID = 0xCCDD;
    msg_ptr->DATA1 = 0x11223344;
    msg_ptr->DATA2 = 0x1122;
    msg_ptr->DATA3 = 0x12;
    msg_ptr->DATA4 = 0x5566;
    msg_ptr->DATA5 = 0x55667788;

    /* Swap header and data of message */
    _msg_swap_endian_header((MESSAGE_HEADER_STRUCT_PTR)msg_ptr);
    _msg_swap_endian_data(data_type, (MESSAGE_HEADER_STRUCT_PTR)msg_ptr);

    /* Verify results of swapping operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr->HEADER.SOURCE_QID == 0xBBAA, "Test #2: Testing 2.02: Verify header");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr->HEADER.TARGET_QID == 0xDDCC, "Test #2: Testing 2.03: Verify header");
    /* TODO:
     * Handle the byte alignment when swapping message data */ 
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr->DATA1 == 0x44332211, "Test #2: Testing 2.04: Verify data");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr->DATA2 == 0x2211, "Test #2: Testing 2.05: Verify data");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr->DATA3 == 0x12, "Test #2: Testing 2.06: Verify data");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr->DATA4 == 0x6655, "Test #2: Testing 2.07: Verify data");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr->DATA5 == 0x88776655, "Test #2: Testing 2.08: Verify data");

    /* Free the message */
    _msg_free(msg_ptr);
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, task_error == MQX_OK, "Test #2: Testing 2.09: Free all messages successfully");
} /* End tc_2_main_task */

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
* The function _msgpool_test and _msgq_test are used to validate 
* all messages in all message pools and all message queues.
* This test case is to verify those function shall work properly.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    TEST_MSG_STRUCT_PTR prv_msg[NUM_OF_MSG], sys_msg[NUM_OF_MSG];
    _queue_id           private_qid, system_qid;
    _pool_id            prv_pool;
    _mqx_uint           result, i;
    void               *pool_error_ptr, *queue_error_ptr, *msg_error_ptr;

    /* Create a private message pool */
    prv_pool = _msgpool_create(sizeof(TEST_MSG_STRUCT),1,1,0);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, prv_pool != 0, "Test #3: Testing 3.01: Create a private message pool successfully");

    /* Allocate private and system messages */
    bool_result = TRUE;
    for(i=0; i<NUM_OF_MSG; i++)
    {
        prv_msg[i] = _msg_alloc(prv_pool);
        sys_msg[i] = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
        if((prv_msg[i] == NULL) || (prv_msg[i] == NULL))
        {
            bool_result = FALSE;
            break;
        }
    }
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == TRUE, "Test #3: Testing 3.02: Create a system message pool successfully");

    /* Open a private message queue */
    private_qid = _msgq_open(QUEUE_VALID_NUM,NUM_OF_MSG);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, private_qid != 0, "Test #3: Testing 3.03: Open a private message queue successfully");

    /* Open a system message queue */
    system_qid = _msgq_open_system(QUEUE_VALID_NUM_2,NUM_OF_MSG,NULL,NULL);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, private_qid != 0, "Test #3: Testing 3.04: Open a system message queue successfully");

    /* Send messages to queues */
    bool_result = TRUE;
    for(i=0; i<NUM_OF_MSG; i++)
    {
        bool_result &= _msgq_send_queue(prv_msg[i], private_qid);
        bool_result &= _msgq_send_queue(sys_msg[i], system_qid);
    }
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == TRUE, "Test #3: Testing 3.05: Send messages to queue successfully");

    /*[BEGIN] Test all queues and pools */
    result = _msgpool_test(&pool_error_ptr, &msg_error_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.06: Test all pools successfully");

    result = _msgq_test(&queue_error_ptr, &msg_error_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.07: Test all queues successfully");
    /*[END] Test all queues and pools */

    /*[BEGIN] Try to test all queues and pools while message component is invalid */
    invalidate_msg_component(&msg_cmpnt);
    
    result = _msgpool_test(&pool_error_ptr, &msg_error_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #3: Testing 3.08: Failed to test pools while message component is invalid");

    result = _msgq_test(&queue_error_ptr, &msg_error_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #3: Testing 3.09: Failed to test queues while message component is invalid");
    
    restore_msg_component(msg_cmpnt);
    /*[END] Try to test all queues and pools while message component is invalid */

    /*[BEGIN] Test all queues and pools while an invalid message exists */
    invalidate_msg(prv_msg[0]);

    /* TODO: 
     * The error pool pointer and error message pointer are not correct */
    result = _msgpool_test(&pool_error_ptr, &msg_error_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MSGQ_INVALID_MESSAGE, "Test #3: Testing 3.10: Test pools and detect error correctly");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, pool_error_ptr == &prv_pool, "Test #3: Testing 3.11: Verify error pool");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_error_ptr == prv_msg, "Test #3: Testing 3.12: Verify error message");

    /* TODO: 
     * The error code is not correct
     * The error queue pointer and error message pointer are not correct */
    result = _msgq_test(&queue_error_ptr, &msg_error_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MSGQ_INVALID_MESSAGE, "Test #3: Testing 3.13: Test queues and detect error correctly");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, queue_error_ptr == &private_qid, "Test #3: Testing 3.14: Verify error queue");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_error_ptr == prv_msg, "Test #3: Testing 3.15: Verify error message");

    restore_msg(prv_msg[0]);
    /*[END] Test all queues and pools while an invalid message exists */

    /* Close all message queues */
    bool_result = _msgq_close(private_qid);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == TRUE, "Test #3: Testing 3.16: Close private message queue successfully");
    bool_result = _msgq_close(system_qid);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == TRUE, "Test #3: Testing 3.17: Close system message queue successfully");

    /* Destroy private pool */
    result = _msgpool_destroy(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.18: Destroy private pool successfully");
} /* End tc_3_main_task */

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
* The function _msg_free, _msgq_close and _msgpool_destroy are used to
* deallocate messages, message queues and message pools.
* This test case is to verify those function shall work properly.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    TEST_MSG_STRUCT_PTR msg_ptr1, msg_ptr2;
    _pool_id            prv_pool;
    _queue_id           private_qid, invalid_qid;
    _task_id            sub_tid;
    _mqx_uint           result;

    /* Create a private message pool */
    prv_pool = _msgpool_create(sizeof(TEST_MSG_STRUCT),1,1,0);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, prv_pool != 0, "Test #4: Testing 4.01: Create a private message pool successfully");

    /* Open a private message queue */
    private_qid = _msgq_open(QUEUE_VALID_NUM,NUM_OF_MSG);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, private_qid != 0, "Test #4: Testing 4.02: Open a private message queue successfully");

    /* Create message on private pool */
    msg_ptr1 = _msg_alloc(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, msg_ptr1 != NULL, "Test #4: Testing 4.03: Create first message on private pool successfully");
    msg_ptr2 = _msg_alloc(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, msg_ptr2 != NULL, "Test #4: Testing 4.04: Create second message on private pool successfully");

    /* Send message to private queue */
    bool_result = _msgq_send_queue(msg_ptr1, private_qid);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result == TRUE, "Test #4: Testing 4.05: Send message to private queue successfully");
    _task_set_error(MQX_OK);

    /*[BEGIN] Try deallocate queue, pool while message component is invalid*/
    invalidate_msg_component(&msg_cmpnt);

    bool_result = _msgq_close(private_qid);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result == FALSE, "Test #4: Testing 4.06: Failed to close queue while message component is invalid");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error == MQX_COMPONENT_DOES_NOT_EXIST, "Test #4: Testing 4.07: The task's error code must be MQX_COMPONENT_DOES_NOT_EXIST");
    _task_set_error(MQX_OK);

    result = _msgpool_destroy(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #4: Testing 4.08: Failed to destroy pool while message component is invalid");

    restore_msg_component(msg_cmpnt);
    /*[END] Try deallocate queue, pool while message component is invalid*/

    /*[BEGIN] Try to free message in the queue */
    _msg_free(msg_ptr1);
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error == MSGQ_MESSAGE_IS_QUEUED, "Test #4: Testing 4.09: Failed to free message in the queue");
    _task_set_error(MQX_OK);
    /*[END] Try to free message in the queue */

    /*[BEGIN] Try to free an invalid message */
    invalidate_msg(msg_ptr2);
    _msg_free(msg_ptr2);
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error == MQX_INVALID_POINTER, "Test #4: Testing 4.10: Failed to free an invalid message");
    _task_set_error(MQX_OK);
    restore_msg(msg_ptr2);
    /*[END] Try to free an invalid message */

    /*[BEGIN] Free message successfully */
    _msg_free(msg_ptr2);
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error == MQX_OK, "Test #4: Testing 4.11: Free message successfully");
    _task_set_error(MQX_OK);
    /*[END] Free message successfully */

    /*[BEGIN] Try to free message that task doesn't own */
    _msg_free(msg_ptr2);
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error == MQX_NOT_RESOURCE_OWNER, "Test #4: Testing 4.12: Failed to free message that task doesn't own");
    _task_set_error(MQX_OK);
    /*[END] Try to free message that task doesn't own */

    /*[BEGIN] Try to close queue that task doesn't own */
    sub_tid = _task_create(0, SUB_TASK, 0);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, sub_tid != 0, "Test #4: Testing 4.13: Create sub task successfully");

    bool_result = _msgq_close(temp_qid);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result == FALSE, "Test #4: Testing 4.14: Failed to close queue that task doesn't own");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error == MSGQ_NOT_QUEUE_OWNER, "Test #4: Testing 4.15: The task's error code must be MSGQ_NOT_QUEUE_OWNER");
    _task_set_error(MQX_OK);
    /*[END] Try to close queue that task doesn't own */

    /*[BEGIN] Try to close an invalid queue */
    invalid_qid = BUILD_QID(INVALID_PROCESSOR, QUEUE_VALID_NUM);
    bool_result = _msgq_close(invalid_qid);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result == FALSE, "Test #4: Testing 4.16: Failed to close an invalid queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error == MSGQ_INVALID_QUEUE_ID, "Test #4: Testing 4.17: The task's error code must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);
    /*[END] Try to close an invalid queue */

    /*[BEGIN] Try to close queue with invalid message */
    invalidate_msg(msg_ptr1);
    bool_result = _msgq_close(private_qid);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result == TRUE, "Test #4: Testing 4.18: Close queue with invalid message successfully");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error == MQX_INVALID_POINTER, "Test #4: Testing 4.19: The task's error code must be MQX_INVALID_POINTER");
    _task_set_error(MQX_OK);
    restore_msg(msg_ptr1);
    /*[END] Try to close queue with invalid message */

    /*[BEGIN] Try to close a closed queue */
    bool_result = _msgq_close(private_qid);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result == FALSE, "Test #4: Testing 4.20: Failed to close a closed queue");
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error == MSGQ_QUEUE_IS_NOT_OPEN, "Test #4: Testing 4.21: The task's error code must be MSGQ_QUEUE_IS_NOT_OPEN");
    _task_set_error(MQX_OK);
    /*[END] Try to close a closed queue */

    /*[BEGIN] Try to destroy the message pool which currently stores message */
    result = _msgpool_destroy(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MSGPOOL_ALL_MESSAGES_NOT_FREE, "Test #4: Testing 4.22: Failed to destroy the message pool which currently stores message");
    /*[END] Try to destroy the message pool which currently stores message */

    /*[BEGIN] Try to destroy an invalid message pool */
    invalidate_msg_pool(prv_pool);
    result = _msgpool_destroy(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MSGPOOL_INVALID_POOL_ID, "Test #4: Testing 4.23: Failed to destroy an invalid message pool");
    restore_msg_pool(prv_pool);
    /*[END] Try to destroy an invalid message pool */

    /* Free remaining message */
    _msg_free(msg_ptr1);
    task_error = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, task_error == MQX_OK, "Test #4: Testing 4.24: Free remaining message successfully");
    _task_set_error(MQX_OK);

    /*[BEGIN] Destroy message pool */
    result = _msgpool_destroy(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.25: Destroy message pool successfully");
    /*[END] Destroy message pool  */
    
    /* Destroy sub task and free its resources */ 
    result = _task_destroy(sub_tid);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.26: Destroy sub task successfully");
} /* End tc_4_main_task */

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_8)
    EU_TEST_CASE_ADD(tc_1_main_task, "TEST # 1 - Testing peek and poll message queue"),
    EU_TEST_CASE_ADD(tc_2_main_task, "TEST # 2 - Testing swap message endian"),
    EU_TEST_CASE_ADD(tc_3_main_task, "TEST # 3 - Testing validate messages"),
    EU_TEST_CASE_ADD(tc_4_main_task, "TEST # 4 - Testing deallocate messages")
EU_TEST_SUITE_END(suite_8)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_8, " - MQX Message Component Test Suite, 4 Tests")
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
    temp_qid = _msgq_open(QUEUE_VALID_NUM_3,NUM_OF_MSG);
    _task_block();
} /* End body */

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
} /* End body */

/* EOF */
