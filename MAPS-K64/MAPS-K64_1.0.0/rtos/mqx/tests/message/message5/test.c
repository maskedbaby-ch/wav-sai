/**HEADER********************************************************************
*
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
* $Version : 4.0.1.2$
* $Date    : Apr-09-2013$
*
* Comments:

*   This file contains code for the MQX Message verification program,
*   includes all remaining test cases, which haven't been verified yet,
*   of functions sending, receiving, peeking and polling messages.
*
* Requirements:
*
*   MESSAGE006, MESSAGE015, MESSAGE021, MESSAGE027, MESSAGE033, MESSAGE040,
*   MESSAGE060, MESSAGE069, MESSAGE070, MESSAGE071, MESSAGE072, MESSAGE073,
*   MESSAGE075, MESSAGE076, MESSAGE077, MESSAGE078, MESSAGE079, MESSAGE081,
*   MESSAGE082, MESSAGE083, MESSAGE085, MESSAGE086, MESSAGE104, MESSAGE105,
*   MESSAGE106, MESSAGE107, MESSAGE109, MESSAGE110, MESSAGE113, MESSAGE114,
*   MESSAGE115, MESSAGE117, MESSAGE120, MESSAGE121, MESSAGE122, MESSAGE124,
*   MESSAGE126, MESSAGE127, MESSAGE128, MESSAGE129, MESSAGE130, MESSAGE132,
*   MESSAGE133, MESSAGE134, MESSAGE135, MESSAGE136, MESSAGE137, MESSAGE138,
*   TASK006, TASK019.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <message.h>
#include <msg_prv.h>
#include "test.h"
#include "msg_util.h"
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
void tc_0_main_task(void); /* TEST # 0 - Create system message pool */
void tc_1_main_task(void); /* TEST # 1 - Testing function _msgq_send */
void tc_2_main_task(void); /* TEST # 2 - Testing function _msgq_send_priority */
void tc_3_main_task(void); /* TEST # 3 - Testing function _msgq_send_urgent */
void tc_4_main_task(void); /* TEST # 4 - Testing function _msgq_send_queue */
void tc_5_main_task(void); /* TEST # 5 - Testing function _msgq_send_broadcast */
void tc_6_main_task(void); /* TEST # 6 - Testing function _msgq_receive */
void tc_7_main_task(void); /* TEST # 7 - Testing function _msgq_receive_ticks */
void tc_8_main_task(void); /* TEST # 8 - Testing function _msgq_receive_for */
void tc_9_main_task(void); /* TEST # 9 - Testing function _msgq_receive_until */
void tc_10_main_task(void); /* TEST # 10 - Testing function _msgq_peek */
void tc_11_main_task(void); /* TEST # 11 - Testing function _msgq_poll */

/*------------------------------------------------------------------------
 * MQX task template
 */
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
#if PSP_MQX_CPU_IS_KINETIS_L
    { MAIN_TASK,      main_task,      1500,  9, "main",    MQX_AUTO_START_TASK},
#else
    { MAIN_TASK,      main_task,      3000,  9, "main",    MQX_AUTO_START_TASK},
#endif
    { TEST_TASK,      test_task,      1000,  8, "test",                      0},
    {         0,              0,         0,  0,      0,                      0}
};

/*------------------------------------------------------------------------
 * Global variables
 */
/* This message queue is to test accessing private resource of other task */
_queue_id   global_queue;
_mqx_uint   test_number;
_mqx_uint   task_error;

/******************** Begin Test cases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_0_main_task
* Comments     : TEST #0:
*   This task creates a system message pool that will be used to
*   allocate testing messages in another test cases.
* Requirements :
*   MESSAGE027.
*
*END*---------------------------------------------------------------------*/
void tc_0_main_task(void)
{
    /* Functions' result */
    bool bool_result;

    test_number = 0;

    /* Create a system message pool with infinite number of messages */
    bool_result = _msgpool_create_system(sizeof(TEST_MSG_STRUCT), 0, 1, 0);
    EU_ASSERT_REF_TC_MSG(tc_0_main_task, bool_result == TRUE, "Test #0: Testing 0.01: Create a system message pool");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*   The function _msgq_send is used to send the message to the message queue.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE006, MESSAGE040, MESSAGE060, MESSAGE113, MESSAGE114, MESSAGE115
*   MESSAGE117.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    /* Message pointer */
    TEST_MSG_STRUCT_PTR msg_ptr;
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Queue IDS */
    _queue_id prv_queue, inv_queue;
    /* Functions' result */
    bool bool_result;

    test_number++;

    /* Open a private message queue */
    prv_queue = _msgq_open(QUEUE_NUMBER_1, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, prv_queue != 0, "Test #1: Testing 1.01: Create private message queue");
    /* Allocate a message */
    msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr != NULL, "Test #1: Testing 1.02: Allocate message");
    /* Assign a valid queue to message's target */
    msg_ptr->HEADER.TARGET_QID = prv_queue;

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgq_send] Send message when message component is invalid */
    bool_result = _msgq_send(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, bool_result == FALSE, "Test #1: Testing 1.03: Failed to send message when message component is invalid");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MQX_COMPONENT_DOES_NOT_EXIST, "Test #1: Testing 1.04: The task's error must be MQX_COMPONENT_DOES_NOT_EXIST");
    _task_set_error(MQX_OK);
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);

    /* Invalidate message */
    set_msg_valid(&msg_ptr->HEADER, INVALID_VALUE);
    /* [_msgq_send] Send an invalid message */
    bool_result = _msgq_send(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, bool_result == FALSE, "Test #1: Testing 1.05: Failed to send an invalid message");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MSGQ_INVALID_MESSAGE, "Test #1: Testing 1.06: The task's error must be MSGQ_INVALID_MESSAGE");
    _task_set_error(MQX_OK);
    /* Restore message */
    set_msg_valid(&msg_ptr->HEADER, MSG_VALID);

    /* Build an invalid queue ID */
    inv_queue = BUILD_QID(INVALID_PROCESSOR, QUEUE_NUMBER_1);
    /* Assign an invalid queue to message's target */
    msg_ptr->HEADER.TARGET_QID = inv_queue;
    /* [_msgq_send] Send message to an invalid queue */
    bool_result = _msgq_send(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, bool_result == FALSE, "Test #1: Testing 1.07: Failed to send message to an invalid queue");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MSGQ_INVALID_QUEUE_ID, "Test #1: Testing 1.08: The task's error must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);

    /* Allocate message again because after sending failed, message was deallocated */
    msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr != NULL, "Test #1: Testing 1.09: Allocate message");
    /* Assign a valid queue to message's target */
    msg_ptr->HEADER.TARGET_QID = prv_queue;

    /* Close the private message queue and free all of its message */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, bool_result == TRUE, "Test #1: Testing 1.10: Close the message queue");
    _task_set_error(MQX_OK);
    /* [_msgq_send] Send message to closed queue */
    bool_result = _msgq_send(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, bool_result == FALSE, "Test #1: Testing 1.11: Failed to send message to closed queue");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MSGQ_QUEUE_IS_NOT_OPEN, "Test #1: Testing 1.12: The task's error must be MSGQ_QUEUE_IS_NOT_OPEN");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   The function _msgq_send_priority is used to send priority message to
*   the message queue.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE006, MESSAGE040, MESSAGE060, MESSAGE126, MESSAGE127, MESSAGE128,
*   MESSAGE129, MESSAGE130, MESSAGE132.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    /* Message pointer */
    TEST_MSG_STRUCT_PTR msg_ptr, tmp_msg_ptr[NUM_OF_MSGS];
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Queue IDS */
    _queue_id prv_queue, inv_queue;
    /* Iteration */
    _mqx_uint i;
    /* Functions' result */
    bool bool_result;

    test_number++;

    /* Open a private message queue */
    prv_queue = _msgq_open(QUEUE_NUMBER_1, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, prv_queue != 0, "Test #2: Testing 2.01: Create private message queue");

    /* Allocate a message */
    msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr != NULL, "Test #2: Testing 2.02: Allocate message");
    /* Assign a valid queue to message's target */
    msg_ptr->HEADER.TARGET_QID = prv_queue;

    /* [_msgq_send_priority] Send message with invalid priority */
    bool_result = _msgq_send_priority(msg_ptr, INVALID_PRIORITY);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == FALSE, "Test #2: Testing 2.03: Failed to send message with invalid priority");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MSGQ_INVALID_MESSAGE_PRIORITY, "Test #2: Testing 2.04: The task's error must be MSGQ_INVALID_MESSAGE_PRIORITY");
    _task_set_error(MQX_OK);

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgq_send_priority] Send message when message component is invalid */
    bool_result = _msgq_send_priority(msg_ptr, VALID_PRIORITY);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == FALSE, "Test #2: Testing 2.05: Failed to send message when message component is invalid");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_COMPONENT_DOES_NOT_EXIST, "Test #2: Testing 2.06: The task's error must be MQX_COMPONENT_DOES_NOT_EXIST");
    _task_set_error(MQX_OK);
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);

    /* Invalidate message */
    set_msg_valid(&msg_ptr->HEADER, INVALID_VALUE);
    /* [_msgq_send_priority] Send an invalid message */
    bool_result = _msgq_send_priority(msg_ptr, VALID_PRIORITY);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == FALSE, "Test #2: Testing 2.07: Failed to send an invalid message");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MSGQ_INVALID_MESSAGE, "Test #2: Testing 2.08: The task's error must be MSGQ_INVALID_MESSAGE");
    _task_set_error(MQX_OK);
    /* Restore message */
    set_msg_valid(&msg_ptr->HEADER, MSG_VALID);

    /* Build an invalid queue ID */
    inv_queue = BUILD_QID(INVALID_PROCESSOR, QUEUE_NUMBER_1);
    /* Assign an invalid queue to message's target */
    msg_ptr->HEADER.TARGET_QID = inv_queue;
    /* [_msgq_send_priority] Send message to an invalid queue */
    bool_result = _msgq_send_priority(msg_ptr, VALID_PRIORITY);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == FALSE, "Test #2: Testing 2.09: Failed to send message to an invalid queue");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MSGQ_INVALID_QUEUE_ID, "Test #2: Testing 2.10: The task's error must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);

    /* Allocate message again because after sending failed, message was deallocated */
    msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr != NULL, "Test #2: Testing 2.11: Allocate message");
    /* Assign a valid queue to message's target */
    msg_ptr->HEADER.TARGET_QID = prv_queue;

    /* Allocate message and send until the queue is full */
    bool_result = TRUE;
    for(i = 0; i < NUM_OF_MSGS; i++)
    {
        tmp_msg_ptr[i] = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
        if(tmp_msg_ptr[i] != NULL)
        {
            tmp_msg_ptr[i]->HEADER.TARGET_QID = prv_queue;
            bool_result &= _msgq_send(tmp_msg_ptr[i]);
        }
        else
        {
            bool_result = FALSE;
            break;
        }
    }
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == TRUE, "Test #2: Testing 2.12: Send message to fill the queue");

    /* [_msgq_send_priority] Send message to a full queue */
    bool_result = _msgq_send_priority(msg_ptr, VALID_PRIORITY);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == FALSE, "Test #2: Testing 2.13: Failed to send message to a full queue");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MSGQ_QUEUE_FULL, "Test #2: Testing 2.14: The task's error must be MSGQ_QUEUE_FULL");
    _task_set_error(MQX_OK);

    /* Allocate message again */
    msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr != NULL, "Test #2: Testing 2.15: Allocate message");
    /* Assign a valid queue to message's target */
    msg_ptr->HEADER.TARGET_QID = prv_queue;

    /* Close the private message queue and free all of its message */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == TRUE, "Test #2: Testing 2.16: Close the message queue");
    _task_set_error(MQX_OK);

    /* [_msgq_send_priority] Send message to closed queue */
    bool_result = _msgq_send_priority(msg_ptr, VALID_PRIORITY);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == FALSE, "Test #2: Testing 2.17: Failed to send message to closed queue");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MSGQ_QUEUE_IS_NOT_OPEN, "Test #2: Testing 2.18: The task's error must be MSGQ_QUEUE_IS_NOT_OPEN");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   The function _msgq_send_urgent is used to send urgent message to
*   the message queue.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE006, MESSAGE040, MESSAGE060, MESSAGE141, MESSAGE142, MESSAGE143,
*   MESSAGE144, MESSAGE145.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    /* Message pointer */
    TEST_MSG_STRUCT_PTR msg_ptr, tmp_msg_ptr[NUM_OF_MSGS];
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Private message queue */
    /* Queue IDS */
    _queue_id prv_queue, inv_queue;
    /* Iteration */
    _mqx_uint i;
    /* Functions' result */
    bool bool_result;

    test_number++;

    /* Open a private message queue */
    prv_queue = _msgq_open(QUEUE_NUMBER_1, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, prv_queue != 0, "Test #3: Testing 3.01: Create private message queue");

    /* Allocate a message */
    msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr != NULL, "Test #3: Testing 3.02: Allocate message");
    /* Assign a valid queue to message's target */
    msg_ptr->HEADER.TARGET_QID = prv_queue;

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgq_send_urgent] Send message when message component is invalid */
    bool_result = _msgq_send_urgent(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == FALSE, "Test #3: Testing 3.03: Failed to send message when message component is invalid");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_COMPONENT_DOES_NOT_EXIST, "Test #3: Testing 3.04: The task's error must be MQX_COMPONENT_DOES_NOT_EXIST");
    _task_set_error(MQX_OK);
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);

    /* Invalidate message */
    set_msg_valid(&msg_ptr->HEADER, INVALID_VALUE);
    /* [_msgq_send_urgent] Send an invalid message */
    bool_result = _msgq_send_urgent(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == FALSE, "Test #3: Testing 3.05: Failed to send an invalid message");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MSGQ_INVALID_MESSAGE, "Test #3: Testing 3.06: The task's error must be MSGQ_INVALID_MESSAGE");
    _task_set_error(MQX_OK);
    /* Restore message */
    set_msg_valid(&msg_ptr->HEADER, MSG_VALID);

    /* Build an invalid queue ID */
    inv_queue = BUILD_QID(INVALID_PROCESSOR, QUEUE_NUMBER_1);
    /* Assign an invalid queue to message's target */
    msg_ptr->HEADER.TARGET_QID = inv_queue;
    /* [_msgq_send_urgent] Send message to an invalid queue */
    bool_result = _msgq_send_urgent(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == FALSE, "Test #3: Testing 3.07: Failed to send message to an invalid queue");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MSGQ_INVALID_QUEUE_ID, "Test #3: Testing 3.08: The task's error must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);

    /* Allocate message again because after sending failed, message was deallocated */
    msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr != NULL, "Test #3: Testing 3.09: Allocate message");
    /* Assign a valid queue to message's target */
    msg_ptr->HEADER.TARGET_QID = prv_queue;

    /* Allocate message and send until the queue is full */
    bool_result = TRUE;
    for(i = 0; i < NUM_OF_MSGS; i++)
    {
        tmp_msg_ptr[i] = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
        if(tmp_msg_ptr[i] != NULL)
        {
            tmp_msg_ptr[i]->HEADER.TARGET_QID = prv_queue;
            bool_result &= _msgq_send(tmp_msg_ptr[i]);
        }
        else
        {
            bool_result = FALSE;
            break;
        }
    }
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == TRUE, "Test #3: Testing 3.10: Send message to fill the queue");

    /* [_msgq_send_urgent] Send message to a full queue */
    bool_result = _msgq_send_urgent(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == FALSE, "Test #3: Testing 3.11: Failed to send message to a full queue");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MSGQ_QUEUE_FULL, "Test #3: Testing 3.12: The task's error must be MSGQ_QUEUE_FULL");
    _task_set_error(MQX_OK);

    /* Allocate message again */
    msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr != NULL, "Test #3: Testing 3.13: Allocate message");
    /* Assign a valid queue to message's target */
    msg_ptr->HEADER.TARGET_QID = prv_queue;

    /* Close the private message queue and free all of its message */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == TRUE, "Test #3: Testing 3.14: Close the message queue");
    _task_set_error(MQX_OK);

    /* [_msgq_send_urgent] Send message to closed queue */
    bool_result = _msgq_send_urgent(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == FALSE, "Test #3: Testing 3.15: Failed to send message to closed queue");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MSGQ_QUEUE_IS_NOT_OPEN, "Test #3: Testing 3.16: The task's error must be MSGQ_QUEUE_IS_NOT_OPEN");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*   The function _msgq_send_queue is used to send the message directly to
*   the private or system message queue.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE006, MESSAGE040, MESSAGE060, MESSAGE134, MESSAGE135, MESSAGE136,
*   MESSAGE137, MESSAGE138.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    /* Message pointer */
    TEST_MSG_STRUCT_PTR msg_ptr, tmp_msg_ptr[NUM_OF_MSGS];
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Queue IDS */
    _queue_id prv_queue, inv_queue;
    /* Iteration */
    _mqx_uint i;
    /* Functions' result */
    bool bool_result;

    test_number++;

    /* Open a private message queue */
    prv_queue = _msgq_open(QUEUE_NUMBER_1, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, prv_queue != 0, "Test #4: Testing 4.01: Create private message queue");

    /* Allocate a message */
    msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, msg_ptr != NULL, "Test #4: Testing 4.02: Allocate message");

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgq_send_queue] Send message when message component is invalid */
    bool_result = _msgq_send_queue(msg_ptr, prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result == FALSE, "Test #4: Testing 4.03: Failed to send message when message component is invalid");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MQX_COMPONENT_DOES_NOT_EXIST, "Test #4: Testing 4.04: The task's error must be MQX_COMPONENT_DOES_NOT_EXIST");
    _task_set_error(MQX_OK);
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);

    /* Invalidate message */
    set_msg_valid(&msg_ptr->HEADER, INVALID_VALUE);
    /* [_msgq_send_queue] Send an invalid message */
    bool_result = _msgq_send_queue(msg_ptr, prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result == FALSE, "Test #4: Testing 4.05: Failed to send an invalid message");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MSGQ_INVALID_MESSAGE, "Test #4: Testing 4.06: The task's error must be MSGQ_INVALID_MESSAGE");
    _task_set_error(MQX_OK);
    /* Restore message */
    set_msg_valid(&msg_ptr->HEADER, MSG_VALID);

    /* Build an invalid queue ID */
    inv_queue = BUILD_QID(INVALID_PROCESSOR, QUEUE_NUMBER_1);
    /* [_msgq_send_queue] Send message to an invalid queue */
    bool_result = _msgq_send_queue(msg_ptr, inv_queue);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result == FALSE, "Test #4: Testing 4.07: Failed to send message to an invalid queue");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MSGQ_INVALID_QUEUE_ID, "Test #4: Testing 4.08: The task's error must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);

    /* Allocate message again because after sending failed, message was deallocated */
    msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, msg_ptr != NULL, "Test #4: Testing 4.09: Allocate message");
    /* Assign a valid queue to message's target */
    msg_ptr->HEADER.TARGET_QID = prv_queue;

    /* Allocate message and send until the queue is full */
    bool_result = TRUE;
    for(i = 0; i < NUM_OF_MSGS; i++)
    {
        tmp_msg_ptr[i] = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
        if(tmp_msg_ptr[i] != NULL)
        {
            tmp_msg_ptr[i]->HEADER.TARGET_QID = prv_queue;
            bool_result &= _msgq_send(tmp_msg_ptr[i]);
        }
        else
        {
            bool_result = FALSE;
            break;
        }
    }
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result == TRUE, "Test #4: Testing 4.10: Send message to fill the queue");

    /* [_msgq_send_queue] Send message to a full queue */
    bool_result = _msgq_send_queue(msg_ptr, prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result == FALSE, "Test #4: Testing 4.11: Failed to send message to a full queue");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MSGQ_QUEUE_FULL, "Test #4: Testing 4.12: The task's error must be MSGQ_QUEUE_FULL");
    _task_set_error(MQX_OK);

    /* Allocate message again */
    msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, msg_ptr != NULL, "Test #4: Testing 4.13: Allocate message");

    /* Close the private message queue and free all of its message */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result == TRUE, "Test #4: Testing 4.14: Close the message queue");
    _task_set_error(MQX_OK);

    /* [_msgq_send_queue] Send message to closed queue */
    bool_result = _msgq_send_queue(msg_ptr, prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result == FALSE, "Test #4: Testing 4.15: Failed to send message to closed queue");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MSGQ_QUEUE_IS_NOT_OPEN, "Test #4: Testing 4.16: The task's error must be MSGQ_QUEUE_IS_NOT_OPEN");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5:
*   The function _msgq_send_broadcast is used to send the message to
*   multiple message queues.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE006, MESSAGE015, MESSAGE021, MESSAGE033, MESSAGE040, MESSAGE060,
*   MESSAGE120, MESSAGE121, MESSAGE122, MESSAGE124.
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    /* Memory allocation list */
    MEMORY_ALLOC_INFO_STRUCT mem_alloc_info;
    /* Message pointers */
    TEST_MSG_STRUCT_PTR msg_ptr, tmp_msg_ptr;
    /* Private pool ID */
    _pool_id prv_pool;
    /* Queue array */
    _queue_id prv_queue[NUM_OF_QUEUES];
    /* Number of messages that were sent */
    _mqx_uint count;
    /* Iteration */
    _mqx_uint i;
    /* Result */
    bool bool_result;

    test_number++;

    /* Create a private message pool with size 1 */
    prv_pool = _msgpool_create(sizeof(TEST_MSG_STRUCT), 0, 1, 1);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, prv_pool != NULL, "Test #5: Testing 5.01: Create a private message pool");

    /* Open an array of private message queues */
    bool_result = TRUE;
    for(i = 0; i < NUM_OF_QUEUES - 1; i++)
    {
        prv_queue[i] = _msgq_open(QUEUE_NUMBER_1 + i, NUM_OF_MSGS);
        if(prv_queue[i] == 0)
        {
            bool_result = FALSE;
            break;
        }
    }
    prv_queue[NUM_OF_QUEUES - 1] = MSGQ_NULL_QUEUE_ID;
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, bool_result == TRUE, "Test #5: Testing 5.02: Open an array of private message queues");
    _task_set_error(MQX_OK);

    /* Allocate message to send */
    msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, msg_ptr != NULL, "Test #5: Testing 5.03: Allocate a system message");

    /* [_msgq_send_broadcast] Send an invalid message to queue array */
    count = _msgq_send_broadcast(NULL, prv_queue, MSGPOOL_NULL_POOL_ID);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, count == 0, "Test #5: Testing 5.04: Failed to an invalid message to queue array");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == MSGQ_INVALID_MESSAGE, "Test #5: Testing 5.05: The task's error code must be MSGQ_INVALID_MESSAGE");
    _task_set_error(MQX_OK);

    /* [_msgq_send_broadcast] Send message to an invalid queue array */
    count = _msgq_send_broadcast(msg_ptr, NULL, MSGPOOL_NULL_POOL_ID);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, count == 0, "Test #5: Testing 5.06: Failed to send message to an invalid queue array");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == MQX_INVALID_PARAMETER, "Test #5: Testing 5.07: The task's error code must be MQX_INVALID_PARAMETER");
    _task_set_error(MQX_OK);

    /* Allocate all memory, the allocated pointers are stored in mem_alloc_info */
    memory_alloc_all(&mem_alloc_info);
    _task_set_error(MQX_OK);
//    /* TODO: [CR253651] Loop forever, should disable this case for testing the following cases */
//    /* [_msgq_send_broadcast] Send message broadcast when memory is currently full */
//    count = _msgq_send_broadcast(msg_ptr, prv_queue, prv_pool);
//    EU_ASSERT_REF_TC_MSG(tc_5_main_task, count == 1, "Test #5: Testing 5.08: Failed to send message broadcast when memory is currently full");
//    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #5: Testing 5.09: The task's error code must be MQX_OUT_OF_MEMORY");
    /* Free all memory that their pointers are stored in mem_alloc_info */
    memory_free_all(&mem_alloc_info);
    _task_set_error(MQX_OK);

    /* Allocate message on private pool, then this pool will be full */
    tmp_msg_ptr = _msg_alloc(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, tmp_msg_ptr != NULL, "Test #5: Testing 5.10: Allocate message on private pool");
//    /* TODO: [CR253651] Loop forever, should disable this case for testing the following cases */
//    /*[_msgq_send_broadcast] Send message broadcast when pool is currently full */
//    count = _msgq_send_broadcast(msg_ptr, prv_queue, prv_pool);
//    EU_ASSERT_REF_TC_MSG(tc_5_main_task, count == 0, "Test #5: Testing 5.11: Failed to send message broadcast when pool is currently full");
//    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == MSGPOOL_OUT_OF_MESSAGES, "Test #5: Testing 5.12: The task's error code must be MSGPOOL_OUT_OF_MESSAGES");
    _task_set_error(MQX_OK);

    /* Close queue array */
    bool_result = TRUE;
    for(i = 0; i < NUM_OF_QUEUES - 1; i++)
    {
        bool_result &= _msgq_close(prv_queue[i]);
    }
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, bool_result == TRUE, "Test #5: Testing 5.13: Close queue array");

    /* Free the allocated message */
    _msg_free(tmp_msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == MQX_OK, "Test #5: Testing 5.14: Free the allocated message");

    /* Destroy the private pool */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _msgpool_destroy(prv_pool) == MQX_OK, "Test #5: Testing 5.15: Destroy the private pool");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6:
*   The function _msgq_receive is used to wait for a message from the
*   private message queue for the number of milliseconds.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE006, MESSAGE040, MESSAGE060, MESSAGE081, MESSAGE082, MESSAGE083,
*   MESSAGE085, MESSAGE086, MESSAGE133, TASK006, TASK019.
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    /* Message pointers */
    TEST_MSG_STRUCT_PTR msg_ptr, tmp_msg_ptr;
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Queue IDs */
    _queue_id prv_queue, inv_queue;
    /* Task ID */
    _task_id test_tid;
    /* Result */
    bool bool_result;

    test_number++;

    /* Open a private message queue */
    prv_queue = _msgq_open(QUEUE_NUMBER_1, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, prv_queue != 0, "Test #6: Testing 6.01: Open a private message queue");

    /* Allocate a message */
    tmp_msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, tmp_msg_ptr != NULL, "Test #6: Testing 6.02: Allocate a message");

    /* Send messages to queue */
    bool_result = _msgq_send_queue(tmp_msg_ptr, prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_result == TRUE, "Test #6: Testing 6.03: Send message to queue");

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgq_receive] Receive message when system message component is invalid */
    msg_ptr = _msgq_receive(prv_queue, DELAY_MS);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, msg_ptr == NULL, "Test #6: Testing 6.04: Failed to receive message when message component is invalid");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == MQX_COMPONENT_DOES_NOT_EXIST, "Test #6: Testing 6.05: The task's error code must be MQX_COMPONENT_DOES_NOT_EXIST");
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);
    _task_set_error(MQX_OK);

    /* Build an invalid queue ID */
    inv_queue = BUILD_QID(INVALID_PROCESSOR, QUEUE_NUMBER_1);
    /* [_msgq_receive] Receive message from an invalid queue */
    msg_ptr = _msgq_receive(inv_queue, DELAY_MS);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, msg_ptr == NULL, "Test #6: Testing 6.06: Failed to receive message from an invalid queue");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == MSGQ_INVALID_QUEUE_ID, "Test #6: Testing 6.07: The task's error code must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);

    /* Create the test task and it will open queue global_queue */
    test_tid = _task_create(0, TEST_TASK, 0);
    /* [_msgq_receive] Receive message from queue that task doesn't own*/
    msg_ptr = _msgq_receive(global_queue, DELAY_MS);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, msg_ptr == NULL, "Test #6: Testing 6.08: Failed to receive message from queue that task doesn't own");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == MSGQ_NOT_QUEUE_OWNER, "Test #6: Testing 6.09: The task's error code must be MSGQ_NOT_QUEUE_OWNER");
    _task_set_error(MQX_OK);

    msg_ptr = test_msg_in_isr(prv_queue, test_number, &task_error);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, msg_ptr == NULL, "Test #6: Testing 6.10: Failed to receive message in ISR");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, task_error == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #6: Testing 6.11: The task's error code must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR");

    /* Close the private queue and free all of its message */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_result == TRUE, "Test #6: Testing 6.12: Close the private queue");

    /* Destroy test task and deallocate all of its resources */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_destroy(test_tid) == MQX_OK, "Test #6: Testing 6.13: Destroy test task");

    /* [_msgq_receive] Receive message from closed queue*/
    msg_ptr = _msgq_receive(prv_queue, DELAY_MS);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, msg_ptr == NULL, "Test #6: Testing 6.14: Failed to receive message from closed queue");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == MSGQ_QUEUE_IS_NOT_OPEN, "Test #6: Testing 6.15: The task's error code must be MSGQ_QUEUE_IS_NOT_OPEN");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7:
*   The function _msgq_receive_ticks is used to wait for a message from the
*   private message queue for the number of ticks (in tick time).
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE006, MESSAGE040, MESSAGE060, MESSAGE097, MESSAGE098, MESSAGE099,
*   MESSAGE101, MESSAGE102, MESSAGE133, TASK006, TASK019.
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    /* Message pointers */
    TEST_MSG_STRUCT_PTR msg_ptr, tmp_msg_ptr;
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Queue IDs */
    _queue_id prv_queue, inv_queue;
    /* Task ID */
    _task_id test_tid;
    /* Result */
    bool bool_result;

    test_number++;

    /* Open a private message queue */
    prv_queue = _msgq_open(QUEUE_NUMBER_1, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, prv_queue != 0, "Test #7: Testing 7.01: Open a private message queue");

    /* Allocate a message */
    tmp_msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, tmp_msg_ptr != NULL, "Test #7: Testing 7.02: Allocate a message");

    /* Send messages to queue */
    bool_result = _msgq_send_queue(tmp_msg_ptr, prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_result == TRUE, "Test #7: Testing 7.03: Send message to queue");

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgq_receive_ticks] Receive message when system message component is invalid */
    msg_ptr = _msgq_receive_ticks(prv_queue, DELAY_TICKS);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, msg_ptr == NULL, "Test #7: Testing 7.04: Failed to receive message when message component is invalid");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, _task_errno == MQX_COMPONENT_DOES_NOT_EXIST, "Test #7: Testing 7.05: The task's error code must be MQX_COMPONENT_DOES_NOT_EXIST");
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);
    _task_set_error(MQX_OK);

    /* Build an invalid queue ID */
    inv_queue = BUILD_QID(INVALID_PROCESSOR, QUEUE_NUMBER_1);
    /* [_msgq_receive_ticks] Receive message from an invalid queue */
    msg_ptr = _msgq_receive_ticks(inv_queue, DELAY_TICKS);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, msg_ptr == NULL, "Test #7: Testing 7.06: Failed to receive message from an invalid queue");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, _task_errno == MSGQ_INVALID_QUEUE_ID, "Test #7: Testing 7.07: The task's error code must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);

    /* Create the test task and it will open queue global_queue */
    test_tid = _task_create(0, TEST_TASK, 0);
    /* [_msgq_receive_ticks] Receive message from queue that task doesn't own*/
    msg_ptr = _msgq_receive_ticks(global_queue, DELAY_TICKS);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, msg_ptr == NULL, "Test #7: Testing 7.08: Failed to receive message from queue that task doesn't own");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, _task_errno == MSGQ_NOT_QUEUE_OWNER, "Test #7: Testing 7.09: The task's error code must be MSGQ_NOT_QUEUE_OWNER");
    _task_set_error(MQX_OK);

    msg_ptr = test_msg_in_isr(prv_queue, test_number, &task_error);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, msg_ptr == NULL, "Test #7: Testing 7.10: Failed to receive message in ISR");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, task_error == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #7: Testing 7.11: The task's error code must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR");

    /* Close the private queue and free all of its message */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_result == TRUE, "Test #7: Testing 7.12: Close the private queue");

    /* Destroy test task and deallocate all of its resources */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, _task_destroy(test_tid) == MQX_OK, "Test #7: Testing 7.13: Destroy test task");

    /* [_msgq_receive_ticks] Receive message from closed queue*/
    msg_ptr = _msgq_receive_ticks(prv_queue, DELAY_TICKS);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, msg_ptr == NULL, "Test #7: Testing 7.14: Failed to receive message from closed queue");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, _task_errno == MSGQ_QUEUE_IS_NOT_OPEN, "Test #7: Testing 7.15: The task's error code must be MSGQ_QUEUE_IS_NOT_OPEN");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8:
*   The function _msgq_receive_for is used to wait for a message from the
*   private message queue for the number of ticks.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE006, MESSAGE015, MESSAGE040, MESSAGE060, MESSAGE088, MESSAGE089,
*   MESSAGE090, MESSAGE091, MESSAGE093, MESSAGE094, MESSAGE133, TASK006, TASK019.
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
    /* Delay ticks */
    MQX_TICK_STRUCT ticks;
    /* Message pointers */
    TEST_MSG_STRUCT_PTR msg_ptr, tmp_msg_ptr;
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Queue IDs */
    _queue_id prv_queue, inv_queue;
    /* Task ID */
    _task_id test_tid;
    /* Result */
    bool bool_result;

    test_number++;

    /* Open a private message queue */
    prv_queue = _msgq_open(QUEUE_NUMBER_1, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, prv_queue != 0, "Test #8: Testing 8.01: Open a private message queue");

    /* Allocate a message */
    tmp_msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, tmp_msg_ptr != NULL, "Test #8: Testing 8.02: Allocate a message");

    /* Send messages to queue */
    bool_result = _msgq_send_queue(tmp_msg_ptr, prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, bool_result == TRUE, "Test #8: Testing 8.03: Send message to queue");

    /* Convert delay time to tick structure */
    ticks.TICKS[0] = 0;
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, DELAY_TICKS, &ticks);

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgq_receive_for] Receive message when system message component is invalid */
    msg_ptr = _msgq_receive_for(prv_queue, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, msg_ptr == NULL, "Test #8: Testing 8.04: Failed to receive message when message component is invalid");
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _task_errno == MQX_COMPONENT_DOES_NOT_EXIST, "Test #8: Testing 8.05: The task's error code must be MQX_COMPONENT_DOES_NOT_EXIST");
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);
    _task_set_error(MQX_OK);

    /* Build an invalid queue ID */
    inv_queue = BUILD_QID(INVALID_PROCESSOR, QUEUE_NUMBER_1);
    /* [_msgq_receive_for] Receive message from an invalid queue */
    msg_ptr = _msgq_receive_for(inv_queue, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, msg_ptr == NULL, "Test #8: Testing 8.06: Failed to receive message from an invalid queue");
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _task_errno == MSGQ_INVALID_QUEUE_ID, "Test #8: Testing 8.07: The task's error code must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);

    /* Create the test task and it will open queue global_queue */
    test_tid = _task_create(0, TEST_TASK, 0);
    /* [_msgq_receive_for] Receive message from queue that task doesn't own*/
    msg_ptr = _msgq_receive_for(global_queue, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, msg_ptr == NULL, "Test #8: Testing 8.08: Failed to receive message from queue that task doesn't own");
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _task_errno == MSGQ_NOT_QUEUE_OWNER, "Test #8: Testing 8.09: The task's error code must be MSGQ_NOT_QUEUE_OWNER");
    _task_set_error(MQX_OK);

    msg_ptr = test_msg_in_isr(prv_queue, test_number, &task_error);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, msg_ptr == NULL, "Test #8: Testing 8.10: Failed to receive message in ISR");
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, task_error == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #8: Testing 8.11: The task's error code must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR");

    /* [_msgq_receive_for] Receive message normally */
    msg_ptr = _msgq_receive_for(prv_queue, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, msg_ptr == tmp_msg_ptr, "Test #8: Testing 8.12: Receive message normally");

    /* Free the received message */
    _msg_free(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _task_errno == MQX_OK, "Test #8: Testing 8.13: Free the received message");

    /* Close the private queue and free all of its message */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, bool_result == TRUE, "Test #8: Testing 8.14: Close the private queue");

    /* Destroy test task and deallocate all of its resources */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _task_destroy(test_tid) == MQX_OK, "Test #8: Testing 8.15: Destroy test task");

    /* [_msgq_receive_for] Receive message from closed queue*/
    msg_ptr = _msgq_receive_for(prv_queue, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, msg_ptr == NULL, "Test #8: Testing 8.16: Failed to receive message from closed queue");
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _task_errno == MSGQ_QUEUE_IS_NOT_OPEN, "Test #8: Testing 8.17: The task's error code must be MSGQ_QUEUE_IS_NOT_OPEN");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : TEST #9:
*   The function _msgq_receive_until is used to wait for a message from the
*   private message queue until the specified moment (in tick time).
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE006, MESSAGE015, MESSAGE040, MESSAGE060, MESSAGE104, MESSAGE105,
*   MESSAGE106, MESSAGE107, MESSAGE109, MESSAGE110, MESSAGE133, TASK006, TASK019.
*
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
    /* Delay ticks */
    MQX_TICK_STRUCT ticks;
    /* Message pointers */
    TEST_MSG_STRUCT_PTR msg_ptr, tmp_msg_ptr;
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Queue IDs */
    _queue_id prv_queue, inv_queue;
    /* Task ID */
    _task_id test_tid;
    /* Result */
    bool bool_result;

    test_number++;

    /* Open a private message queue */
    prv_queue = _msgq_open(QUEUE_NUMBER_1, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, prv_queue != 0, "Test #9: Testing 9.01: Open a private message queue");

    /* Allocate a message */
    tmp_msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, tmp_msg_ptr != NULL, "Test #9: Testing 9.02: Allocate a message");

    /* Send messages to queue */
    bool_result = _msgq_send_queue(tmp_msg_ptr, prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, bool_result == TRUE, "Test #9: Testing 9.03: Send message to queue");

    /* Get the current tick time */
    _time_get_elapsed_ticks(&ticks);
    /* Calculate the moment to wait until (long time delay) */
    PSP_ADD_TICKS_TO_TICK_STRUCT(&ticks, 100 * DELAY_TICKS, &ticks);

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgq_receive_until] Receive message when system message component is invalid */
    msg_ptr = _msgq_receive_until(prv_queue, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, msg_ptr == NULL, "Test #9: Testing 9.04: Failed to receive message when message component is invalid");
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, _task_errno == MQX_COMPONENT_DOES_NOT_EXIST, "Test #9: Testing 9.05: The task's error code must be MQX_COMPONENT_DOES_NOT_EXIST");
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);
    _task_set_error(MQX_OK);

    /* Build an invalid queue ID */
    inv_queue = BUILD_QID(INVALID_PROCESSOR, QUEUE_NUMBER_1);
    /* [_msgq_receive_until] Receive message from an invalid queue */
    msg_ptr = _msgq_receive_until(inv_queue, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, msg_ptr == NULL, "Test #9: Testing 9.06: Failed to receive message from an invalid queue");
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, _task_errno == MSGQ_INVALID_QUEUE_ID, "Test #9: Testing 9.07: The task's error code must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);

    /* Create the test task and it will open queue global_queue */
    test_tid = _task_create(0, TEST_TASK, 0);
    /* [_msgq_receive_until] Receive message from queue that task doesn't own*/
    msg_ptr = _msgq_receive_until(global_queue, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, msg_ptr == NULL, "Test #9: Testing 9.08: Failed to receive message from queue that task doesn't own");
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, _task_errno == MSGQ_NOT_QUEUE_OWNER, "Test #9: Testing 9.09: The task's error code must be MSGQ_NOT_QUEUE_OWNER");
    _task_set_error(MQX_OK);

    msg_ptr = test_msg_in_isr(prv_queue, test_number, &task_error);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, msg_ptr == NULL, "Test #9: Testing 9.10: Failed to receive message in ISR");
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, task_error == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #9: Testing 9.11: The task's error code must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR");

    /* [_msgq_receive_until] Receive message normally */
    msg_ptr = _msgq_receive_until(prv_queue, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, msg_ptr == tmp_msg_ptr, "Test #9: Testing 9.12: Receive message normally");

    /* Free the received message */
    _msg_free(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, _task_errno == MQX_OK, "Test #9: Testing 9.13: Close the private queue");

    /* Close the private queue and free all of its message */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, bool_result == TRUE, "Test #9: Testing 9.14: Close the private queue");

    /* Destroy test task and deallocate all of its resources */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, _task_destroy(test_tid) == MQX_OK, "Test #9: Testing 9.15: Destroy test task");

    /* [_msgq_receive_until] Receive message from closed queue*/
    msg_ptr = _msgq_receive_until(prv_queue, &ticks);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, msg_ptr == NULL, "Test #9: Testing 9.16: Failed to receive message from closed queue");
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, _task_errno == MSGQ_QUEUE_IS_NOT_OPEN, "Test #9: Testing 9.17: The task's error code must be MSGQ_QUEUE_IS_NOT_OPEN");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_10_main_task
* Comments     : TEST #10:
*   The function _msgq_peek is used to get a pointer to the message that
*   is at the start of the message queue, but do not remove the message.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE006, MESSAGE040, MESSAGE060, MESSAGE069, MESSAGE070, MESSAGE071,
*   MESSAGE072, MESSAGE073, MESSAGE133, TASK006, TASK019.
*
*END*---------------------------------------------------------------------*/
void tc_10_main_task(void)
{
    /* Message pointers */
    TEST_MSG_STRUCT_PTR msg_ptr, tmp_msg_ptr;
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Queue IDs */
    _queue_id prv_queue, inv_queue;
    /* Task ID */
    _task_id test_tid;
    /* Result */
    bool bool_result;

    test_number++;

    /* Open a private message queue */
    prv_queue = _msgq_open(QUEUE_NUMBER_1, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, prv_queue != 0, "Test #10: Testing 10.01: Open a private message queue");

    /* Allocate a message */
    tmp_msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, tmp_msg_ptr != NULL, "Test #10: Testing 10.02: Allocate a message");

    /* Send messages to queue */
    bool_result = _msgq_send_queue(tmp_msg_ptr, prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, bool_result == TRUE, "Test #10: Testing 10.03: Send message to queue");

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgq_peek] Peek message when system message component is invalid */
    msg_ptr = _msgq_peek(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, msg_ptr == NULL, "Test #10: Testing 10.04: Failed to peek message when message component is invalid");
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, _task_errno == MQX_COMPONENT_DOES_NOT_EXIST, "Test #10: Testing 10.05: The task's error code must be MQX_COMPONENT_DOES_NOT_EXIST");
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);
    _task_set_error(MQX_OK);

    /* Build an invalid queue ID */
    inv_queue = BUILD_QID(INVALID_PROCESSOR, QUEUE_NUMBER_1);
    /* [_msgq_peek] Peek message from an invalid queue */
    msg_ptr = _msgq_peek(inv_queue);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, msg_ptr == NULL, "Test #10: Testing 10.06: Failed to peek message from an invalid queue");
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, _task_errno == MSGQ_INVALID_QUEUE_ID, "Test #10: Testing 10.07: The task's error code must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);

    /* Create the test task and it will open queue global_queue */
    test_tid = _task_create(0, TEST_TASK, 0);
    /* [_msgq_peek] Peek message from queue that task doesn't own*/
    msg_ptr = _msgq_peek(global_queue);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, msg_ptr == NULL, "Test #10: Testing 10.08: Failed to peek message from queue that task doesn't own");
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, _task_errno == MSGQ_NOT_QUEUE_OWNER, "Test #10: Testing 10.09: The task's error code must be MSGQ_NOT_QUEUE_OWNER");
    _task_set_error(MQX_OK);

    /* Close the private queue and free all of its message */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, bool_result == TRUE, "Test #10: Testing 10.10: Close the private queue");

    /* Destroy test task and deallocate all of its resources */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, _task_destroy(test_tid) == MQX_OK, "Test #10: Testing 10.11: Destroy test task");

    /* [_msgq_peek] Peek message from closed queue*/
    msg_ptr = _msgq_peek(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, msg_ptr == NULL, "Test #10: Testing 10.12: Failed to peek message from closed queue");
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, _task_errno == MSGQ_QUEUE_IS_NOT_OPEN, "Test #10: Testing 10.13: The task's error code must be MSGQ_QUEUE_IS_NOT_OPEN");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_11_main_task
* Comments     : TEST #11:
*   The function _msgq_poll is used to get a pointer to the message that
*   is at the start of the message queue, but do not remove the message.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE006, MESSAGE040, MESSAGE060, MESSAGE075, MESSAGE076, MESSAGE077,
*   MESSAGE078, MESSAGE079, MESSAGE133, TASK006, TASK019.
*
*END*---------------------------------------------------------------------*/
void tc_11_main_task(void)
{
    /* Message pointers */
    TEST_MSG_STRUCT_PTR msg_ptr, tmp_msg_ptr;
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Queue IDs */
    _queue_id prv_queue, inv_queue;
    /* Task ID */
    _task_id test_tid;
    /* Result */
    bool bool_result;

    test_number++;

    /* Open a private message queue */
    prv_queue = _msgq_open(QUEUE_NUMBER_1, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, prv_queue != 0, "Test #11: Testing 11.01: Open a private message queue");

    /* Allocate a message */
    tmp_msg_ptr = _msg_alloc_system(sizeof(TEST_MSG_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, tmp_msg_ptr != NULL, "Test #11: Testing 11.02: Allocate a message");

    /* Send messages to queue */
    bool_result = _msgq_send_queue(tmp_msg_ptr, prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, bool_result == TRUE, "Test #11: Testing 11.03: Send message to queue");

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgq_poll] Poll message when system message component is invalid */
    msg_ptr = _msgq_poll(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, msg_ptr == NULL, "Test #11: Testing 11.04: Failed to poll message when message component is invalid");
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, _task_errno == MQX_COMPONENT_DOES_NOT_EXIST, "Test #11: Testing 11.05: The task's error code must be MQX_COMPONENT_DOES_NOT_EXIST");
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);
    _task_set_error(MQX_OK);

    /* Build an invalid queue ID */
    inv_queue = BUILD_QID(INVALID_PROCESSOR, QUEUE_NUMBER_1);
    /* [_msgq_poll] Poll message from an invalid queue */
    msg_ptr = _msgq_poll(inv_queue);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, msg_ptr == NULL, "Test #11: Testing 11.06: Failed to poll message from an invalid queue");
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, _task_errno == MSGQ_INVALID_QUEUE_ID, "Test #11: Testing 11.07: The task's error code must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);

    /* Create the test task and it will open queue global_queue */
    test_tid = _task_create(0, TEST_TASK, 0);
    /* [_msgq_poll] Poll message from queue that task doesn't own*/
    msg_ptr = _msgq_poll(global_queue);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, msg_ptr == NULL, "Test #11: Testing 11.08: Failed to poll message from queue that task doesn't own");
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, _task_errno == MSGQ_NOT_QUEUE_OWNER, "Test #11: Testing 11.09: The task's error code must be MSGQ_NOT_QUEUE_OWNER");
    _task_set_error(MQX_OK);

    /* Close the private queue and free all of its message */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, bool_result == TRUE, "Test #11: Testing 11.10: Close the private queue");

    /* Destroy test task and deallocate all of its resources */
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, _task_destroy(test_tid) == MQX_OK, "Test #11: Testing 11.11: Destroy test task");

    /* [_msgq_poll] Poll message from closed queue*/
    msg_ptr = _msgq_poll(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, msg_ptr == NULL, "Test #11: Testing 11.12: Failed to poll message from closed queue");
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, _task_errno == MSGQ_QUEUE_IS_NOT_OPEN, "Test #11: Testing 11.13: The task's error code must be MSGQ_QUEUE_IS_NOT_OPEN");
    _task_set_error(MQX_OK);
}

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_5)
    EU_TEST_CASE_ADD(tc_0_main_task, " TEST # 0 - Create system message pool"),
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1 - Testing function _msgq_send"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2 - Testing function _msgq_send_priority"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3 - Testing function _msgq_send_urgent"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4 - Testing function _msgq_send_queue"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST # 5 - Testing function _msgq_send_broadcast"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST # 6 - Testing function _msgq_receive"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST # 7 - Testing function _msgq_receive_ticks"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST # 8 - Testing function _msgq_receive_for"),
    EU_TEST_CASE_ADD(tc_9_main_task, " TEST # 9 - Testing function _msgq_receive_until"),
    EU_TEST_CASE_ADD(tc_10_main_task, " TEST # 10 - Testing function _msgq_peek"),
    EU_TEST_CASE_ADD(tc_11_main_task, " TEST # 11 - Testing function _msgq_poll")
EU_TEST_SUITE_END(suite_5)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_5, " - MQX Message Component Test Suite, 11 Tests")
EU_TEST_REGISTRY_END()
/*------------------------------------------------------------------------*/

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   This is the main task to the MQX Kernel program, this task creates
*   all defined test tasks and lets them run continuously.
*
*END*----------------------------------------------------------------------*/
void main_task(uint32_t param)
{
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
    _mqx_exit(0);
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : test_task
* Comments     :
*   This task creates an private message queue that will be used to test
*   accessing private resource of another task.
*   After creating queue, the task will be blocked.
*
*END*----------------------------------------------------------------------*/
void test_task(uint32_t param)
{
    /* Create private queue */
    global_queue = _msgq_open(QUEUE_NUMBER_2, NUM_OF_MSGS);
    /* Block itself */
    _task_block();
}

void new_tick_isr(void *ptr)
{
    MY_ISR_STRUCT_PTR isr_ptr = (MY_ISR_STRUCT_PTR)ptr;
    MQX_TICK_STRUCT ticks;

    _task_set_error(MQX_OK);

    switch (isr_ptr->TEST_NUMBER)
    {
    case 6:
        isr_ptr->MSG_PTR = _msgq_receive(isr_ptr->MSG_QUEUE_TEST, DELAY_MS);
        break;

    case 7:
        isr_ptr->MSG_PTR = _msgq_receive_ticks(isr_ptr->MSG_QUEUE_TEST, DELAY_TICKS);
        break;

    case 8:
        _time_init_ticks(&ticks, DELAY_TICKS);
        isr_ptr->MSG_PTR = _msgq_receive_for(isr_ptr->MSG_QUEUE_TEST, &ticks);
        break;

    case 9:
        _time_get_elapsed_ticks(&ticks);
        _time_add_msec_to_ticks(&ticks, DELAY_MS);
        isr_ptr->MSG_PTR = _msgq_receive_until(isr_ptr->MSG_QUEUE_TEST, &ticks);
        break;
    }

    isr_ptr->ERROR = _task_errno;

    _task_set_error(MQX_OK);

    /* signal the main task */
    _lwsem_post(&isr_ptr->INT_EVENT);

    /* Chain to the previous notifier */
    (*isr_ptr->OLD_ISR)(isr_ptr->OLD_ISR_DATA);
}

void *test_msg_in_isr(_queue_id msg_queue, _mqx_uint test_no, _mqx_uint *error)
{
    MY_ISR_STRUCT_PTR   isr_ptr;
    void                *msg_ptr;

    isr_ptr = _mem_alloc_zero((_mem_size)sizeof(MY_ISR_STRUCT));

    isr_ptr->TEST_NUMBER = test_no;
    isr_ptr->MSG_QUEUE_TEST = msg_queue;

    isr_ptr->OLD_ISR = _int_get_isr(BSP_TIMER_INTERRUPT_VECTOR);
    isr_ptr->OLD_ISR_DATA = _int_get_isr_data(BSP_TIMER_INTERRUPT_VECTOR);
    _lwsem_create(&isr_ptr->INT_EVENT, 0);

    /* Install new isr for bsp timer */
    _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, new_tick_isr, isr_ptr);

    /* Waiting for bsp timer interrupt signal */
    *error = _lwsem_wait_ticks(&isr_ptr->INT_EVENT, 2);

    if (*error == MQX_OK)
    {
        *error = isr_ptr->ERROR;
        msg_ptr = isr_ptr->MSG_PTR;
    }

    _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, isr_ptr->OLD_ISR, isr_ptr->OLD_ISR_DATA);

    /* Free resources */
    _lwsem_destroy(&isr_ptr->INT_EVENT);
    _mem_free(isr_ptr);

    return msg_ptr;
}
