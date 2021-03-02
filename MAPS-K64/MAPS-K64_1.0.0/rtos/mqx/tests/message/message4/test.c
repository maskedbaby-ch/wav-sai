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
*   of functions creating, allocating, getting and modifying messages'
*   components.
*
* Requirements:
*
*   MESSAGE002, MESSAGE003, MESSAGE005, MESSAGE007, MESSAGE008, MESSAGE010,
*   MESSAGE011, MESSAGE012, MESSAGE013, MESSAGE014, MESSAGE021, MESSAGE022,
*   MESSAGE023, MESSAGE025, MESSAGE026, MESSAGE027, MESSAGE028, MESSAGE029,
*   MESSAGE030, MESSAGE031, MESSAGE032, MESSAGE033, MESSAGE040, MESSAGE047,
*   MESSAGE048, MESSAGE049, MESSAGE051, MESSAGE053, MESSAGE054, MESSAGE055,
*   MESSAGE057, MESSAGE058, MESSAGE059, MESSAGE060, MESSAGE061, MESSAGE062,
*   MESSAGE063, MESSAGE064, MESSAGE065, MESSAGE066, MESSAGE148, MESSAGE149,
*   MESSAGE150, MESSAGE163, MESSAGE164, MESSAGE165, MESSAGE170.
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
void tc_1_main_task(void); /* TEST # 1 - Testing function _msg_create_component */
void tc_2_main_task(void); /* TEST # 2 - Testing function _msgpool_create_system, _msgpool_create */
void tc_3_main_task(void); /* TEST # 3 - Testing function _msg_alloc_system, _msg_alloc */
void tc_4_main_task(void); /* TEST # 4 - Testing function _msgq_open_system, _msgq_open */
void tc_5_main_task(void); /* TEST # 5 - Testing function _msg_available */
void tc_6_main_task(void); /* TEST # 6 - Testing function _msgq_get_count */
void tc_7_main_task(void); /* TEST # 7 - Testing function _msgq_get_notification_function */
void tc_8_main_task(void); /* TEST # 8 - Testing function _msgq_get_owner */
void tc_9_main_task(void); /* TEST # 9 - Testing function _msgq_get_id */
void tc_10_main_task(void); /* TEST # 10 - Testing function _msgq_set_notification_function */

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

/******************** Begin Test cases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*   The function _msg_create_component is used to create message component
*   of the MQX kernel.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE013, MESSAGE014, MESSAGE170
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    /* Memory allocation list */
    MEMORY_ALLOC_INFO_STRUCT mem_alloc_info;
    /* Memory block pointer */
    void   *mem_ptr;
    /* Returned value of function */
    _mqx_uint result;

    /* Allocate all memory, the allocated pointers are stored in mem_alloc_info */
    memory_alloc_all(&mem_alloc_info);
    _task_set_error(MQX_OK);
    /* [_msg_create_component] Create message component when memory is insufficient */
    result = _msg_create_component();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OUT_OF_MEMORY, "Test #1: Testing 1.01: Failed to create message component");
    /* Free all memory that their pointers are stored in mem_alloc_info */
    memory_free_all(&mem_alloc_info);

    /* Allocate a memory block that its size is equal to size of all message pools */
    mem_ptr = _mem_alloc_system(sizeof(MSGPOOL_STRUCT)*TEST_MAX_MSG_POOLS);
    /* Allocate all memory, the allocated pointers are stored in mem_alloc_info */
    memory_alloc_all(&mem_alloc_info);
    _task_set_error(MQX_OK);
    /* Free the mem_ptr, this free block will be used to create message component */
    _mem_free(mem_ptr);
    /* [_msg_create_component] Create message component when memory is insufficient */
    result = _msg_create_component();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MSGPOOL_POOL_NOT_CREATED, "Test #1: Testing 1.02: Failed to create message component");
    /* Free all memory that their pointers are stored in mem_alloc_info */
    memory_free_all(&mem_alloc_info);

    /* Allocate a memory block that its size is equal to size of all message pools */
    mem_ptr = _mem_alloc_system(sizeof(MSGQ_STRUCT)*TEST_MAX_MSG_QUEUES);
    /* Allocate all memory, the allocated pointers are stored in mem_alloc_info */
    memory_alloc_all(&mem_alloc_info);
    _task_set_error(MQX_OK);
    /* Free the mem_ptr, this free block will be used to create message component */
    _mem_free(mem_ptr);
    /* [_msg_create_componenet] Create message component when could not allocate mem for message queues */
    result = _msg_create_component();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MSGQ_TOO_MANY_QUEUES, "Test #1: Testing 1.03: Failed to create message component");
    /* Free all memory that their pointers are stored in mem_alloc_info */
    memory_free_all(&mem_alloc_info);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   The functions _msgpool_create_system and _msgpool_create are used to
*   create system message pool and private message pool.
*   This test case is to verify all the uncovered cases of these functions.
* Requirements :
*   MESSAGE012, MESSAGE021, MESSAGE022, MESSAGE023, MESSAGE025, MESSAGE026,
*   MESSAGE028, MESSAGE029, MESSAGE030, MESSAGE031, MESSAGE032, MESSAGE033
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    /* Memory allocation list */
    MEMORY_ALLOC_INFO_STRUCT mem_alloc_info;
    /* Private pool IDs */
    _pool_id prv_pool[TEST_MAX_MSG_POOLS];
    /* Iteration */
    _mqx_uint i;
    /* Returned value of function */
    _mqx_uint result;
    /* Functions' result */
    bool bool_result;

    /* Allocate all memory, the allocated pointers are stored in mem_alloc_info */
    memory_alloc_all(&mem_alloc_info);
    _task_set_error(MQX_OK);
    /* [_msgpool_create_system] Create pool when system memory is insufficient for message component */
    bool_result = _msgpool_create_system(POOL_VALID_SIZE, 2, 2, 0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == FALSE, "Test #2: Testing 2.01: Failed to allocate memory for message component");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #2: Testing 2.02: Task's error is MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);
    /* [_msgpool_create] Create pool when system memory is insufficient for message component */
    prv_pool[0] = _msgpool_create(POOL_VALID_SIZE, 0, 2, 8);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, prv_pool[0] == NULL, "Test #2: Testing 2.03: Failed to allocate memory for message component");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #2: Testing 2.04: Task's error is MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);
    /* Free all memory that their pointers are stored in mem_alloc_info */
    memory_free_all(&mem_alloc_info);

    /* Create message component for system */
    result = _msg_create_component();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.05: Create message component");

    /* Allocate all memory, the allocated pointers are stored in mem_alloc_info */
    memory_alloc_all(&mem_alloc_info);
    _task_set_error(MQX_OK);
    /* [_msgpool_create_system] Create pool when when could not allocate mem for initial messages */
    bool_result = _msgpool_create_system(POOL_VALID_SIZE, 1, 2, 0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == FALSE, "Test #2: Testing 2.06: Failed to allocate memory for initial messages in _msgpool_create_system");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #2: Testing 2.07: Task error is MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);
    /* [_msgpool_create] Create pool when when could not allocate mem for initial messages */
    prv_pool[0] = _msgpool_create(POOL_VALID_SIZE, 1, 2, 8);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, prv_pool[0] == NULL, "Test #2: Testing 2.08: Failed to allocate memory for initial messages in _msgpool_create");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #2: Testing 2.09: Task error is MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);
    /* Free all memory that their pointers are stored in mem_alloc_info */
    memory_free_all(&mem_alloc_info);

    /* [_msgpool_create_system] Create pool with too small messages' size */
    bool_result = _msgpool_create_system(POOL_INVALID_SIZE, 0, 4, 0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == FALSE, "Test #2: Testing 2.10: Failed to create system message pool with too small messages' size");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MSGPOOL_MESSAGE_SIZE_TOO_SMALL, "Test #2: Testing 2.11: Task error is MSGPOOL_MESSAGE_SIZE_TOO_SMALL");
    _task_set_error(MQX_OK);
    /* [_msgpool_create] Create pool with too small messages' size */
    prv_pool[0] = _msgpool_create(POOL_INVALID_SIZE, 0, 2, 8);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, prv_pool[0] == NULL, "Test #2: Testing 2.12: Failed to create private message pool with too small messages' size");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MSGPOOL_MESSAGE_SIZE_TOO_SMALL, "Test #2: Testing 2.13: Task error is MSGPOOL_MESSAGE_SIZE_TOO_SMALL");
    _task_set_error(MQX_OK);

    /* Create message pools until reach the maximum pool number */
    for(i=0; i<TEST_MAX_MSG_POOLS; i++)
    {
        prv_pool[i] = _msgpool_create(POOL_VALID_SIZE, 0, 0, 0);
    }
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_OK, "Test #2: Testing 2.14: Create message pools");
    /* [_msgpool_create_system] Try to create more message pools when number of pools are limited */
    bool_result = _msgpool_create_system(POOL_VALID_SIZE, 0, 4, 0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == FALSE, "Test #2: Testing 2.15: Failed to create system message pool when all pools have been created");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MSGPOOL_OUT_OF_POOLS, "Test #2: Testing 2.16: Task error is MSGPOOL_OUT_OF_POOLS");
    _task_set_error(MQX_OK);
    /* Destroy all created pools */
    result = MQX_OK;
    for(i=0; i<TEST_MAX_MSG_POOLS; i++)
    {
        result |= _msgpool_destroy(prv_pool[i]);
    }
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.17: Destroy private pools");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   The functions _msg_alloc_system and _msg_alloc are used to allocate
*   messages from system message pool and private message pool.
*   This test case is to verify all the uncovered cases of these functions.
* Requirements :
*   MESSAGE002, MESSAGE003, MESSAGE005, MESSAGE007, MESSAGE008, MESSAGE021,
*   MESSAGE027, MESSAGE033.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    /* Memory allocation list */
    MEMORY_ALLOC_INFO_STRUCT mem_alloc_info;
    /* Allocated message pointer */
    void   *msg_ptr;
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Private pool ID */
    _pool_id prv_pool;
    /* Returned value of functions */
    _mqx_uint result;
    /* Functions' result */
    bool bool_result;

    /* Create a private pool and a system pool */
    prv_pool = _msgpool_create(sizeof(MESSAGE_HEADER_STRUCT), 0, 1, 4);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, prv_pool != NULL, "Test #3: Testing 3.01: Create private pool");
    bool_result = _msgpool_create_system(sizeof(MESSAGE_HEADER_STRUCT), 0, 1, 4);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == TRUE, "Test #3: Testing 3.02: Create system pool");

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msg_alloc_system] Allocate message when message component is invalid */
    msg_ptr = _msg_alloc_system(sizeof(MESSAGE_HEADER_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr == NULL, "Test #3: Testing 3.03: Failed to allocate system message when message component is invalid");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_COMPONENT_DOES_NOT_EXIST, "Test #3: Testing 3.04: Task error must be MQX_COMPONENT_DOES_NOT_EXIST");
    _task_set_error(MQX_OK);
    /* [_msg_alloc] Allocate message when message component is invalid */
    msg_ptr = _msg_alloc(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr == NULL, "Test #3: Testing 3.05: Failed to allocate private message when message component is invalid");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_COMPONENT_DOES_NOT_EXIST, "Test #3: Testing 3.06: Task error must be MQX_COMPONENT_DOES_NOT_EXIST");
    _task_set_error(MQX_OK);
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);

    /* Allocate all memory, the allocated pointers are stored in mem_alloc_info */
    memory_alloc_all(&mem_alloc_info);
    _task_set_error(MQX_OK);
    /* [_msg_alloc_system] Allocate more messages when memory is currently full */
    msg_ptr = _msg_alloc_system(sizeof(MESSAGE_HEADER_STRUCT));
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr == NULL, "Test #3: Testing 3.07: Failed to allocate more messages when memory is currently full");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #3: Testing 3.08: Task error must be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);
    /* [_msg_alloc] Allocate more messages when memory is currently full */
    msg_ptr = _msg_alloc(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr == NULL, "Test #3: Testing 3.09: Failed to allocate more messages when memory is currently full");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #3: Testing 3.10: Task error must be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);
    /* Free all memory that their pointers are stored in mem_alloc_info */
    memory_free_all(&mem_alloc_info);

    /* Invalidate message pool */
    set_pool_valid(prv_pool, INVALID_VALUE);
    /* [_msg_alloc] Allocate message from invalid pool */
    msg_ptr = _msg_alloc(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr == NULL, "Test #3: Testing 3.11: Failed to allocate private message from invalid pool");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MSGPOOL_INVALID_POOL_ID, "Test #3: Testing 3.12: Task error must be MSGPOOL_INVALID_POOL_ID");
    _task_set_error(MQX_OK);
    /* Restore message pool */
    set_pool_valid(prv_pool, MSG_VALID);

    /* Destroy the private message pools */
    result = _msgpool_destroy(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.13: Destroy private message pool");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*   The functions _msgq_open_system and _msgq_open are used to open a
*   system message queue and private message queue.
*   This test case is to verify all the uncovered cases of these functions.
* Requirements :
*   MESSAGE040, MESSAGE061, MESSAGE062, MESSAGE063, MESSAGE064, MESSAGE065,
*   MESSAGE066, MESSAGE163, MESSAGE164, MESSAGE165.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    /* Memory allocation list */
    MEMORY_ALLOC_INFO_STRUCT mem_alloc_info;
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* System queue ID */
    _queue_id sys_queue;
    /* Temporary queue ID */
    _queue_id tmp_queue;
    /* Functions's result */
    bool bool_result;

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* Allocate all memory, the allocated pointers are stored in mem_alloc_info */
    memory_alloc_all(&mem_alloc_info);
    _task_set_error(MQX_OK);
    /* [_msgq_open_system] Open message queue when there's not enough memory for message component */
    tmp_queue = _msgq_open_system(QUEUE_VALID_NUM, NUM_OF_MSGS, NULL, NULL);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, tmp_queue == 0, "Test #4: Testing 4.01: Failed to allocate memory for creating component");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #4: Testing 4.02: Task error must be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);
    /* [_msgq_open] Open message queue when there's not enough memory for message component */
    tmp_queue = _msgq_open(QUEUE_VALID_NUM, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, tmp_queue == 0, "Test #4: Testing 4.03: Failed to allocate memory for creating component");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #4: Testing 4.04: Task error must be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);
    /* Free all memory that their pointers are stored in mem_alloc_info */
    memory_free_all(&mem_alloc_info);
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);

    /* Open a system queue */
    sys_queue = _msgq_open_system(QUEUE_VALID_NUM, NUM_OF_MSGS, NULL, NULL);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, sys_queue != 0, "Test #4: Testing 4.05: Open system message queue");

    /* [_msgq_open_system] Open the message queue which is currently opened */
    tmp_queue = _msgq_open_system(QUEUE_VALID_NUM, NUM_OF_MSGS, NULL, NULL);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, tmp_queue == 0, "Test #4: Testing 4.06: Failed to open a system message queue twice");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MSGQ_QUEUE_IN_USE, "Test #4: Testing 4.07: Task error must be MSGQ_QUEUE_IN_USE");
    _task_set_error(MQX_OK);
    /* [_msgq_open] Open the message queue which is currently opened */
    tmp_queue = _msgq_open(QUEUE_VALID_NUM, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, tmp_queue == 0, "Test #4: Testing 4.08: Failed to open a private message queue twice");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MSGQ_QUEUE_IN_USE, "Test #4: Testing 4.09: Task error must be MSGQ_QUEUE_IN_USE");
    _task_set_error(MQX_OK);

    /* [_msgq_open_system] Open the message queue with invalid queue number */
    tmp_queue = _msgq_open_system(QUEUE_INVALID_NUM, NUM_OF_MSGS, NULL, NULL);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, tmp_queue == 0, "Test #4: Testing 4.10: Failed to open a system message queue with invalid queue number");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MSGQ_INVALID_QUEUE_NUMBER, "Test #4: Testing 4.11: Task error must be MSGQ_INVALID_QUEUE_NUMBER");
    _task_set_error(MQX_OK);
    /* [_msgq_open] Open the message queue with invalid queue number */
    tmp_queue = _msgq_open(QUEUE_INVALID_NUM, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, tmp_queue == 0, "Test #4: Testing 4.12: Failed to open a private message queue with invalid queue number");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MSGQ_INVALID_QUEUE_NUMBER, "Test #4: Testing 4.13: Task error must be MSGQ_INVALID_QUEUE_NUMBER");
    _task_set_error(MQX_OK);

    /* TODO: [ENGR255978] This function's error code can't be reached */
    /* [_msgq_open_system] Open the message queue with invalid queue ID */
    tmp_queue = _msgq_open_system(QUEUE_INVALID_ID, NUM_OF_MSGS, NULL, NULL);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, tmp_queue == 0, "Test #4: Testing 4.14: Failed to open a system message queue with invalid queue id");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MSGQ_INVALID_QUEUE_NUMBER, "Test #4: Testing 4.15: Task error must be MSGQ_INVALID_QUEUE_NUMBER");
    _task_set_error(MQX_OK);

    /* TODO: [ENGR255978] This function's error code can't be reached */
    /* [_msgq_open] Open the message queue with invalid queue ID */
    tmp_queue = _msgq_open(QUEUE_INVALID_ID, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, tmp_queue == 0, "Test #4: Testing 4.16: Failed to open a private message queue with invalid queue id");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MSGQ_INVALID_QUEUE_NUMBER, "Test #4: Testing 4.17: Task error must be MSGQ_INVALID_QUEUE_NUMBER");
    _task_set_error(MQX_OK);

    /* Close all message queues */
    bool_result = _msgq_close(sys_queue);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result == TRUE, "Test #4: Testing 4.18: Close the system message queue");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5:
*   The function _msg_available is used to get number of free messages
*   in the message pool.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE010, MESSAGE011, MESSAGE021, MESSAGE033.
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Private pool pointer */
    _pool_id prv_pool;
    /* Number of available message */
    _mqx_uint count;
    /* Returned value of functions */
    _mqx_uint result;

    /* Create private message pool */
    prv_pool = _msgpool_create(sizeof(MESSAGE_HEADER_STRUCT), 2, 2, 4);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, prv_pool != NULL, "Test #5: Testing 5.01: Create private pool");

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msg_available] Get number of free messages of pool when message component is invalid */
    count = _msg_available(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, count == 0, "Test #5: Testing 5.02: Failed to get number of free messages of an invalid pool");
    _task_set_error(MQX_OK);
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);

    /* Invalidate message pool */
    set_pool_valid(prv_pool, INVALID_VALUE);
    /* [_msg_available] Get number of free messages of an invalid pool */
    count = _msg_available(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, count == 0, "Test #5: Testing 5.03: Failed to get number of free messages of an invalid pool");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == MSGPOOL_INVALID_POOL_ID, "Test #5: Testing 5.04: Task's error must be MSGPOOL_INVALID_POOL_ID");
    _task_set_error(MQX_OK);
    /* Restore message pool */
    set_pool_valid(prv_pool, MSG_VALID);

    /* Destroy private pool */
    result = _msgpool_destroy(prv_pool);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.05: Destroy private pool");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6:
*   The function _msgq_get_count is used to get the number of messages
*   in the message queue
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*    MESSAGE040, MESSAGE047, MESSAGE048, MESSAGE049, MESSAGE060.
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Private queue ID */
    _queue_id prv_queue;
    /* Invalid queue ID */
    _queue_id inv_queue;
    /* Number of available message */
    _mqx_uint count;
    /* Functions' result */
    bool bool_result;

    /* Open a private message queue */
    prv_queue = _msgq_open(QUEUE_VALID_NUM, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, prv_queue != 0, "Test #6: Testing 6.01: Create private message queue");

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgq_get_count] Get number of messages of queue when message component is invalid */
    count = _msgq_get_count(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, count == 0, "Test #6: Testing 6.02: Failed to get queue's free messages number when message component is invalid");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == MQX_COMPONENT_DOES_NOT_EXIST, "Test #6: Testing 6.03: The task's error must be MQX_COMPONENT_DOES_NOT_EXIST");
    _task_set_error(MQX_OK);
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);

    /* Build an invalid queue ID */
    inv_queue = BUILD_QID(INVALID_PROCESSOR, QUEUE_VALID_NUM);
    /* [_msgq_get_count] Get number of free messages of invalid queue */
    count = _msgq_get_count(inv_queue);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, count == 0, "Test #6: Testing 6.04: Failed to get number of messages of invalid queue");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == MSGQ_INVALID_QUEUE_ID, "Test #6: Testing 6.05: The task's error must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);

    /* Close the private message queue */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, bool_result == TRUE, "Test #6: Testing 6.06: Close the message queue");

    /* [_msgq_get_count] Get number of free messages of closed queue */
    count = _msgq_get_count(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, count == 0, "Test #6: Testing 6.07: Failed to get number of free messages of closed queue");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == MSGQ_QUEUE_IS_NOT_OPEN, "Test #6: Testing 6.08: The task's error must be MSGQ_QUEUE_IS_NOT_OPEN");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7:
*   The function _msgq_get_notification_function is used to get the
*   notification function and its data that are associated with the
*   private or the system message queue.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE040, MESSAGE053, MESSAGE054, MESSAGE055, MESSAGE060.
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Private queue ID */
    _queue_id prv_queue;
    /* Invalid queue ID */
    _queue_id inv_queue;
    /* Returned value of functions */
    _mqx_uint result;
    /* Functions' result */
    bool bool_result;
    /* Notification data */
    void   *noti_data_ptr;
    /* Notification function */
    void (_CODE_PTR_ noti_func_ptr)(void *);

    /* Open a private message queue */
    prv_queue = _msgq_open(QUEUE_VALID_NUM, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, prv_queue != 0, "Test #7: Testing 7.01: Create private message queue");

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgq_get_notification_function] Get notification of queue when message component is invalid */
    result = _msgq_get_notification_function(prv_queue, &noti_func_ptr, &noti_data_ptr);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #7: Testing 7.02: Failed to get the notification of queue when message component is invalid");
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);

    /* Build an invalid queue ID */
    inv_queue = BUILD_QID(INVALID_PROCESSOR, QUEUE_VALID_NUM);
    /* [_msgq_get_notification_function] Get notification of invalid queue */
    result = _msgq_get_notification_function(inv_queue, &noti_func_ptr, &noti_data_ptr);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MSGQ_INVALID_QUEUE_ID, "Test #7: Testing 7.03: Failed to get the notification of invalid queue");

    /* Close the private message queue */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, bool_result == TRUE, "Test #7: Testing 7.04: Close the message queue");

    /* [_msgq_get_notification_function] Get notification of closed queue */
    result = _msgq_get_notification_function(prv_queue, &noti_func_ptr, &noti_data_ptr);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MSGQ_QUEUE_IS_NOT_OPEN, "Test #7: Testing 7.05: Failed to get the notification of closed queue");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8:
*   The function _msgq_get_owner is used to get the task ID of the task
*   that owns the message queue.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE040, MESSAGE057, MESSAGE058, MESSAGE059, MESSAGE060.
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* Private queue ID */
    _queue_id prv_queue;
    /* Invalid queue ID */
    _queue_id inv_queue;
    /* Task ID */
    _task_id tid;
    /* Functions' result */
    bool bool_result;

    /* Open a private message queue */
    prv_queue = _msgq_open(QUEUE_VALID_NUM, NUM_OF_MSGS);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, prv_queue != 0, "Test #8: Testing 8.01: Create private message queue");

    /* [_msgq_get_owner] Get the owner of a message queue */
    tid = _msgq_get_owner(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, tid == _task_get_id(), "Test #8: Testing 8.02: The owner should be the current task");

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgq_get_owner] Get the owner of a message queue when message component is invalid */
    tid = _msgq_get_owner(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, tid == MQX_NULL_TASK_ID, "Test #8: Testing 8.03: Failed to get the owner of a message queue when message component is invalid");
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _task_errno == MQX_COMPONENT_DOES_NOT_EXIST, "Test #8: Testing 8.04: The task's error must be MQX_COMPONENT_DOES_NOT_EXIST");
    _task_set_error(MQX_OK);
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);

    /* Build an invalid queue ID */
    inv_queue = BUILD_QID(INVALID_PROCESSOR, QUEUE_VALID_NUM);
    /* [_msgq_get_owner] Get the owner of an invalid message queue */
    tid = _msgq_get_owner(inv_queue);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, tid == MQX_NULL_TASK_ID, "Test #8: Testing 8.05: Failed to get the owner of an invalid message queue");
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _task_errno == MQX_INVALID_PROCESSOR_NUMBER, "Test #8: Testing 8.06: The task's error must be MQX_INVALID_PROCESSOR_NUMBER");
    _task_set_error(MQX_OK);

    /* Close the private message queue */
    bool_result = _msgq_close(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, bool_result == TRUE, "Test #8: Testing 8.07: Close the message queue");

    /* [_msgq_get_owner] Get the owner of closed message queue */
    tid = _msgq_get_owner(prv_queue);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, tid == MQX_NULL_TASK_ID, "Test #8: Testing 8.08: Failed to get the owner of closed message queue");
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _task_errno == MSGQ_QUEUE_IS_NOT_OPEN, "Test #8: Testing 8.09: The task's error must be MSGQ_QUEUE_IS_NOT_OPEN");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : TEST #9:
*   The function _msgq_get_id is used to convert a message-queue number
*   and processor number to a queue ID.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE051.
*
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
    /* Private queue ID */
    _queue_id prv_queue;

    /* [_msgq_get_id] Convert an invalid processor number to queue ID */
    prv_queue = _msgq_get_id(INVALID_PROCESSOR, QUEUE_VALID_NUM);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, prv_queue == MSGQ_NULL_QUEUE_ID, "Test #9: Testing 9.01: Failed to convert an invalid processor number to queue ID");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_10_main_task
* Comments     : TEST #10:
*   The function _msgq_set_notification_function is used to set the
*   notification function for the private or the system message queue.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MESSAGE040, MESSAGE064, MESSAGE148, MESSAGE149, MESSAGE150.
*
*END*---------------------------------------------------------------------*/
void tc_10_main_task(void)
{
    /* Address of kernel message component */
    void   *msg_comp_ptr;
    /* System queue ID */
    _queue_id sys_queue;
    /* Invalid queue ID */
    _queue_id inv_queue;
    /* Functions' result */
    bool bool_result;
    /* Function address */
    void   *func_ptr;

    /* Open a private message queue */
    sys_queue = _msgq_open_system(QUEUE_VALID_NUM, NUM_OF_MSGS, queue_not_function, NULL);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, sys_queue != 0, "Test #10: Testing 10.01: Create system message queue with notification function");

    /* Invalidate the system message component */
    msg_comp_ptr = get_msg_component();
    set_msg_component(NULL);
    /* [_msgq_set_notification_function] Change notification function when msg component is invalid */
    func_ptr = (void *)_msgq_set_notification_function(sys_queue, NULL, NULL);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, func_ptr == NULL, "Test #10: Testing 10.02: Failed to change notification function when msg component is invalid");
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, _task_errno == MQX_COMPONENT_DOES_NOT_EXIST, "Test #10: Testing 10.03: Task error must be MQX_COMPONENT_DOES_NOT_EXIST");
    _task_set_error(MQX_OK);
    /* Restore the system message component */
    set_msg_component(msg_comp_ptr);

    /* Build an invalid queue ID */
    inv_queue = BUILD_QID(INVALID_PROCESSOR, QUEUE_VALID_NUM);
    /* [_msgq_set_notification_function] Change notification function of an invalid message queue */
    func_ptr = (void *)_msgq_set_notification_function(inv_queue, NULL, NULL);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, func_ptr == NULL, "Test #10: Testing 10.04: Failed to change the notification function an invalid message queue");
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, _task_errno == MSGQ_INVALID_QUEUE_ID, "Test #10: Testing 10.05: Task error must be MSGQ_INVALID_QUEUE_ID");
    _task_set_error(MQX_OK);

    /* Close the private message queue */
    bool_result = _msgq_close(sys_queue);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, bool_result == TRUE, "Test #10: Testing 10.06: Close the message queue");

    /* [_msgq_set_notification_function] Change notification function of closed message queue */
    func_ptr = (void *)_msgq_set_notification_function(sys_queue, NULL, NULL);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, func_ptr == NULL, "Test #10: Testing 10.07: Failed to change the notification function of closed message queue");
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, _task_errno == MSGQ_QUEUE_IS_NOT_OPEN, "Test #10: Testing 10.08: Task error must be MSGQ_QUEUE_IS_NOT_OPEN");
    _task_set_error(MQX_OK);
}

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_4)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1 - Testing function _msg_create_component"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2 - Testing function _msgpool_create_system, _msgpool_create"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3 - Testing function _msg_alloc_system, _msg_alloc"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4 - Testing function _msgq_open_system, _msgq_open"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST # 5 - Testing function _msg_available"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST # 6 - Testing function _msgq_get_count"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST # 7 - Testing function _msgq_get_notification_function"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST # 8 - Testing function _msgq_get_owner"),
    EU_TEST_CASE_ADD(tc_9_main_task, " TEST # 9 - Testing function _msgq_get_id"),
    EU_TEST_CASE_ADD(tc_10_main_task, " TEST # 10 - Testing function _msgq_set_notification_function"),
EU_TEST_SUITE_END(suite_4)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_4, " - MQX Message Component Test Suite, 10 Tests")
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

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : queue_not_function
* Returned Value   : None
* Comments         :
*   This function is used to test get/set notification function of a queue
*
*END*--------------------------------------------------------------------*/
void queue_not_function(void *param)
{
    /* Does nothing */
}
