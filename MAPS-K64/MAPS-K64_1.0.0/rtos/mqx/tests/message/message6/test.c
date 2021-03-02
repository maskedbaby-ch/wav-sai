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
* $Version : 4.0.1.2$
* $Date    : Apr-09-2013$
*
* Comments:
*
*   This file contains code for the MQX Message verification program,
*   includes all remaining test cases, which haven't been verified yet,
*   of functions validating and deallocating messages' components.
*
* Requirements:
*
*   MESSAGE001, MESSAGE015, MESSAGE016, MESSAGE017, MESSAGE018, MESSAGE021,
*   MESSAGE033, MESSAGE034, MESSAGE035, MESSAGE036, MESSAGE038, MESSAGE039,
*   MESSAGE040, MESSAGE041, MESSAGE042, MESSAGE043, MESSAGE060, MESSAGE133,
*   MESSAGE152, MESSAGE153, TASK006, TASK019.
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
#if !MQX_USE_MESSAGES
    #error "Required MQX_USE_MESSAGES"
#endif

/*------------------------------------------------------------------------
 * Test suite function prototype
 */
void tc_1_main_task(void); /* TEST # 1 - Testing function _msgpool_test */
void tc_2_main_task(void); /* TEST # 2 - Testing function _msgq_test */
void tc_3_main_task(void); /* TEST # 3 - Testing function _msg_free */
void tc_4_main_task(void); /* TEST # 4 - Testing function _msgpool_destroy */
void tc_5_main_task(void); /* TEST # 5 - Testing function _msgq_close */

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
_queue_id global_queue;

/******************** Begin Test cases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*   The function _msgpool_test is used to test all the system message pools
*   and private message pools.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE001, MESSAGE015, MESSAGE021, MESSAGE033, MESSAGE038, MESSAGE039.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    /* Message pointer */
    void   *msg_ptr;
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Error pool pointer, error message pointer */
    void   *error_pool_ptr, *error_msg_ptr;
    /* Private pool ID */
    _pool_id prv_pool;
    /* Returned value of functions */
    _mqx_uint result;

    /* Create a private message pool */
    prv_pool = _msgpool_create(sizeof(MESSAGE_HEADER_STRUCT), 0, 1, 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, prv_pool != NULL, "Test #1: Testing 1.01: Create a private message pool");

    /* Allocate message on private pool */
    msg_ptr = _msg_alloc(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, msg_ptr != NULL, "Test #1: Testing 1.02: Allocate message on private pool");

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgpool_test] Test all pools while message component is invalid */
    result = _msgpool_test(&error_pool_ptr, &error_msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #1: Testing 1.03: Failed to test pools while message component is invalid");
    _task_set_error(MQX_OK);
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);

    /* Invalidate message */
    set_msg_valid(msg_ptr, INVALID_VALUE);
    /* [_msgpool_test] Test all pools while message component is invalid */
    result = _msgpool_test(&error_pool_ptr, &error_msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MSGQ_INVALID_MESSAGE, "Test #1: Testing 1.04: Failed to test all pools while an invalid message exists");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, error_pool_ptr == prv_pool, "Test #1: Testing 1.05: Verify error pool");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, error_msg_ptr == GET_INTERNAL_MESSAGE_PTR(msg_ptr), "Test #1: Testing 1.06: Verify error message");
    _task_set_error(MQX_OK);
    /* Restore message */
    set_msg_valid(msg_ptr, MSG_VALID);

    /* Free the allocated message */
    _msg_free(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MQX_OK, "Test #1: Testing 1.07: Free the allocated message");

    /* Destroy the private pool */
    result = _msgpool_destroy(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.08: Destroy the private pool");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   The function _msgq_test is used to test all messages on all the system 
*   message queues and private message queues.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE001, MESSAGE021, MESSAGE033, MESSAGE040, MESSAGE060, MESSAGE133,
*   MESSAGE152, MESSAGE153.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    /* Message pointer */
    void   *msg_ptr;
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Error queue pointer, error message pointer */
    void   *error_queue_ptr, *error_msg_ptr;
    /* Private pool ID */
    _pool_id prv_pool;
    /* Private queue ID */
    _queue_id prv_queue;
    /* Returned value of functions */
    _mqx_uint result;
    /* Result */
    bool bool_result;
    /* Queue number */
    _queue_number queue;
    /* Message queue pointer */
    MSGQ_STRUCT_PTR msgq_ptr;

    /* Create a private message pool */
    prv_pool = _msgpool_create(sizeof(MESSAGE_HEADER_STRUCT), 0, 1, 0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, prv_pool != NULL, "Test #2: Testing 2.01: Create a private message pool");

    /* Open a private queue */
    prv_queue = _msgq_open(QUEUE_NUMBER_1, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, prv_queue != 0, "Test #2: Testing 2.02: Open a private queue");

    /* Allocate message on private pool */
    msg_ptr = _msg_alloc(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, msg_ptr != NULL, "Test #2: Testing 2.03: Allocate message on private pool");

    /* Send message to queue */
    bool_result = _msgq_send_queue(msg_ptr, prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == TRUE, "Test #2: Testing 2.04: Send message to queue");

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgq_test] Test all queues while message component is invalid */
    result = _msgq_test(&error_queue_ptr, &error_msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #2: Testing 2.05: Failed to test all queues while message component is invalid");
    _task_set_error(MQX_OK);
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);

    /* Invalidate message */
    set_msg_valid(msg_ptr, INVALID_VALUE);
    /* TODO: [CR253679]
     * The returned error code is not correct
     * The error queue pointer and error message pointer are not correct
     */
    /* [_msgq_test] Test all queues while an invalid message exists */
    result = _msgq_test(&error_queue_ptr, &error_msg_ptr);
    /* Get queue number from queu ID */
    queue = QUEUE_FROM_QID(prv_queue);
    /* Get message queue pointer from queue number */
    msgq_ptr = &(((MSG_COMPONENT_STRUCT_PTR)msg_comp_ptr)->MSGQS_PTR[queue]);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MSGQ_INVALID_MESSAGE, "Test #2: Testing 2.06: Failed to test all queues while an invalid message exists");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, error_queue_ptr == msgq_ptr, "Test #2: Testing 2.07: Verify error queue");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, error_msg_ptr == GET_INTERNAL_MESSAGE_PTR(msg_ptr), "Test #2: Testing 2.08: Verify error message");
    _task_set_error(MQX_OK);
    /* Restore message */
    set_msg_valid(msg_ptr, MSG_VALID);

    /* Close the private queue */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == TRUE, "Test #2: Testing 2.09: Close the private queue");

    /* Destroy the private pool */
    result = _msgpool_destroy(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.10: Destroy the private pool");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   The function _msg_free is used to deallocate a message on pool.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE001, MESSAGE015, MESSAGE016, MESSAGE017, MESSAGE018, MESSAGE021,
*   MESSAGE033, MESSAGE040, MESSAGE060, MESSAGE133.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    /* Message pointer */
    void   *msg_ptr;
    /* Private pool ID */
    _pool_id prv_pool;
    /* Private queue ID */
    _queue_id prv_queue;
    /* Returned value of functions */
    _mqx_uint result;
    /* Result */
    bool bool_result;

    /* Create a private message pool */
    prv_pool = _msgpool_create(sizeof(MESSAGE_HEADER_STRUCT), 0, 1, 0);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, prv_pool != NULL, "Test #3: Testing 3.01: Create a private message pool");

    /* Open a private queue */
    prv_queue = _msgq_open(QUEUE_NUMBER_1, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, prv_queue != 0, "Test #3: Testing 3.02: Open a private queue");

    /* Allocate message on private pool */
    msg_ptr = _msg_alloc(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr != NULL, "Test #3: Testing 3.03: Allocate message on private pool");

    /* Send message to queue */
    bool_result = _msgq_send_queue(msg_ptr, prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == TRUE, "Test #3: Testing 3.04: Send message to queue");
    /* [_msg_free] Free the message that is currently in a queue*/
    _msg_free(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MSGQ_MESSAGE_IS_QUEUED, "Test #3: Testing 3.05: Failed to free the message that is currently in a queue");
    _task_set_error(MQX_OK);
    /* Get message from queue */
    msg_ptr = _msgq_poll(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr != NULL, "Test #3: Testing 3.06: Get message from queue");

    /* Invalidate message */
    set_msg_valid(msg_ptr, INVALID_VALUE);
    /* [_msg_free] Free an invalid message */
    _msg_free(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_INVALID_POINTER, "Test #3: Testing 3.07: Failed to free an invalid message");
    _task_set_error(MQX_OK);
    /* Restore message */
    set_msg_valid(msg_ptr, MSG_VALID);

    /* Free the message, then message is no longer task's resource */
    _msg_free(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_OK, "Test #3: Testing 3.08: Free the message");

    /* [_msg_free] Free the message that task doesn't own */
    _msg_free(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_NOT_RESOURCE_OWNER, "Test #3: Testing 3.09: Failed to the message that task doesn't own");
    _task_set_error(MQX_OK);

    /* Close the private queue */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == TRUE, "Test #3: Testing 3.10: Close the private queue");

    /* Destroy the private pool */
    result = _msgpool_destroy(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.11: Destroy the private pool");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*   The function _msgpool_destroy is used to destroy the private message pool.
*   This test case is to verify all the uncovered cases of these functions.
* Requirements :
*   MESSAGE001, MESSAGE015, MESSAGE021, MESSAGE033, MESSAGE034, MESSAGE035,
*   MESSAGE036.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    /* Message pointer */
    void   *msg_ptr;
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Private pool ID */
    _pool_id prv_pool;
    /* Returned value of functions */
    _mqx_uint result;

    /* Create a private message pool */
    prv_pool = _msgpool_create(sizeof(MESSAGE_HEADER_STRUCT), 0, 1, 0);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, prv_pool != NULL, "Test #4: Testing 4.01: Create a private message pool");

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgpool_destroy] Destroy the private pool while message component is invalid */
    result = _msgpool_destroy(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #4: Testing 4.02: Failed to destroy the private pool while message component is invalid");
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);

    /* Invalidate message pool */
    set_pool_valid(prv_pool, INVALID_VALUE);
    /* [_msgpool_destroy] Destroy an invalid pool */
    result = _msgpool_destroy(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MSGPOOL_INVALID_POOL_ID, "Test #4: Testing 4.03: Failed to an invalid pool");
    /* Restore message pool */
    set_pool_valid(prv_pool, MSG_VALID);

    /* Allocate message on private pool */
    msg_ptr = _msg_alloc(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, msg_ptr != NULL, "Test #4: Testing 4.04: Allocate message on private pool");
    result = _msgpool_destroy(prv_pool);
    /* [_msgpool_destroy] Destroy non-empty pool */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MSGPOOL_ALL_MESSAGES_NOT_FREE, "Test #4: Testing 4.05: Destroy non-empty pool");
    /* Free the allocated message */
    _msg_free(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MQX_OK, "Test #4: Testing 4.06: Free the allocated message");

    /* Destroy the private pool */
    result = _msgpool_destroy(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.07: Destroy the private pool");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5:
*   The function _msgq_close is used to close a message queue.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE001, MESSAGE015, MESSAGE021, MESSAGE033, MESSAGE041, MESSAGE042,
*   MESSAGE043, TASK006, TASK019.
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Message pointer */
    void   *msg_ptr;
    /* Private pool ID */
    _pool_id prv_pool;
    /* Queue ID */
    _queue_id prv_queue, inv_queue;
    /* Test task ID */
    _task_id test_tid;
    /* Returned value of functions */
    _mqx_uint result;
    /* Result */
    bool bool_result;

    /* Create a private message pool */
    prv_pool = _msgpool_create(sizeof(MESSAGE_HEADER_STRUCT), 0, 1, 0);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, prv_pool != NULL, "Test #5: Testing 5.01: Create a private message pool");

    /* Open a private queue */
    prv_queue = _msgq_open(QUEUE_NUMBER_1, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, prv_queue != 0, "Test #5: Testing 5.02: Open a private queue");

    /* Allocate message on private pool */
    msg_ptr = _msg_alloc(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, msg_ptr != NULL, "Test #5: Testing 5.03: Allocate message on private pool");

    /* Send message to queue */
    bool_result = _msgq_send_queue(msg_ptr, prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, bool_result == TRUE, "Test #5: Testing 5.04: Send message to queue");

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgq_close] Close queue when system message component is invalid */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, bool_result == FALSE, "Test #5: Testing 5.05: Failed to close queue when message component is invalid");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == MQX_COMPONENT_DOES_NOT_EXIST, "Test #5: Testing 5.06: The task's error code must be MQX_COMPONENT_DOES_NOT_EXIST");
    _task_set_error(MQX_OK);
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);

    /* Build an invalid queue ID */
    inv_queue = BUILD_QID(INVALID_PROCESSOR, QUEUE_NUMBER_1);
    /* [_msgq_close] Close invalid queue */
    bool_result = _msgq_close(inv_queue);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, bool_result == FALSE, "Test #5: Testing 5.07: Failed to close invalid queue");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == MSGQ_INVALID_QUEUE_ID, "Test #5: Testing 5.08: The task's error code must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);

    /* Create the test task and it will open queue global_queue */
    test_tid = _task_create(0, TEST_TASK, 0);
    /* [_msgq_close] Close the queue that task doesn't own */
    bool_result = _msgq_close(global_queue);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, bool_result == FALSE, "Test #5: Testing 5.09: Failed to close the queue that task doesn't own ");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == MSGQ_NOT_QUEUE_OWNER, "Test #5: Testing 5.10: The task's error code must be MSGQ_NOT_QUEUE_OWNER");
    _task_set_error(MQX_OK);

    /* Invalidate message */
    set_msg_valid(msg_ptr, INVALID_VALUE);
    /* [_msgq_close] Close the queue that contains an invalid message */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, bool_result == TRUE, "Test #5: Testing 5.11: Failed to close the queue that contains an invalid message");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == MQX_INVALID_POINTER, "Test #5: Testing 5.12: The task's error code must be MQX_INVALID_POINTER");
    _task_set_error(MQX_OK);
    /* Restore message */
    set_msg_valid(msg_ptr, MSG_VALID);

    /* [_msgq_close] Close the queue that has been closed */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, bool_result == FALSE, "Test #5: Testing 5.13: Failed to close the private queue that has been closed");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == MSGQ_QUEUE_IS_NOT_OPEN, "Test #5: Testing 5.14: The task's error code must be MSGQ_QUEUE_IS_NOT_OPEN");
    _task_set_error(MQX_OK);

    /* Destroy test task and deallocate all of its resources */
    result = _task_destroy(test_tid);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.15: Destroy test task");

    /* Free the allocated message */
    _msg_free(msg_ptr);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == MQX_OK, "Test #5: Testing 5.16: Free the allocated message");

    /* Destroy the private pool */
    result = _msgpool_destroy(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.17: Destroy the private pool");
}

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_6)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1 - Testing function _msgpool_test"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2 - Testing function _msgq_test"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3 - Testing function _msg_free"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4 - Testing function _msgpool_destroy"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST # 5 - Testing function _msgq_close")
EU_TEST_SUITE_END(suite_6)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_6, " - MQX Message Component Test Suite, 5 Tests")
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
