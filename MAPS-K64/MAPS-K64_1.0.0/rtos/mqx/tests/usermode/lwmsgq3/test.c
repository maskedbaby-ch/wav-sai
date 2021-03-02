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
* $Version : 4.0.2.0$
* $Date    : Jun-14-2013$
*
* Comments:
*
*   This file contains code for the MQX Usermode verification program,
*   includes all remaining test cases, which haven't been verified yet,
*   of Lightweight Message Queue functions in usermode.
*
* Requirements:
*
*   USERMODE008, USERMODE016, USERMODE073, USERMODE074, USERMODE075,
*   USERMODE077, USERMODE078, USERMODE081, USERMODE083, USERMODE086,
*   USERMODE087.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <lwmsgq.h>
#include <lwmsgq_prv.h>
#include "test.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

/*------------------------------------------------------------------------
 * Test suite function prototype
 */
void tc_1_main_task(void);  /* TEST #1 - Testing function _usr_lwmsgq_init in special cases */
void tc_2_main_task(void);  /* TEST #2 - Testing function _usr_lwmsgq_receive in special cases */
void tc_3_main_task(void);  /* TEST #3 - Testing function _usr_lwmsgq_send in special cases */

/*------------------------------------------------------------------------
 * MQX task template
 */
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    { SUPER_TASK, super_task, 1500, 10, "super", MQX_AUTO_START_TASK },
    {  MAIN_TASK,  main_task, 3000,  9,  "main",       MQX_USER_TASK },
    {   ISR_TASK,   isr_task, 1500,  9,   "isr",                   0 },
    {  PRIV_TASK,  priv_task, 1500,  9,  "priv",                   0 },
    {         0,           0,    0,  0,       0,                   0 }
};

/******************** Begin Test cases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*   This test case is to verify function _usr_lwmsgq_init in the cases of:
*   - Initializing a lightweight message queue which is already initialized
*   - Accessing a lightweight message queue without access rights.
* Requirements:
*   USERMODE074, USERMODE075, USERMODE086
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    tc_number++;

    /* Initialize lightweight message queue usr_lwmsgq_ro by calling function _usr_lwmsgq_init */
    result = _usr_lwmsgq_init(usr_lwmsgq_ro, NUM_OF_MSG, MSG_SIZE);
    /* Check the returned result must be MQX_OK to verify successfully initializing */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "TEST #1: Testing 1.01: Initialize lightweight message queue");

    /* Try to initialize lightweight message queue usr_lwmsgq_ro again by calling function _usr_lwmsgq_init */
    result = _usr_lwmsgq_init(usr_lwmsgq_ro, NUM_OF_MSG, MSG_SIZE);
    /*
    ** Check the returned result must be MQX_EINVAL to verify initializing the lightweight message queue,
    ** which is already initialized, should fail
    */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_EINVAL, "TEST #1: Testing 1.02: Failed to initialize duplicate lightweight message queue");

    /* Try to initialize lightweight message queue usr_lwmsgq_rw by calling function _usr_lwmsgq_init */
    result = _usr_lwmsgq_init(usr_lwmsgq_rw, NUM_OF_MSG, MSG_SIZE);
    /*
    ** Check the returned result must be MQX_EINVAL to verify accessing a lightweight message queue
    ** without access rights should fail
    */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_EINVAL, "TEST #1: Testing 1.03: Failed to access a lightweight message queue without access rights");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   This test case is to verify function _usr_lwmsgq_receive in the cases of:
*   - Receiving lightweight message from an invalid message queue.
*   - Waiting lightweight message for specified time.
* Requirements:
*   USERMODE008, USERMODE016, USERMODE073, USERMODE081, USERMODE083.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;

    tc_number++;

    /* Set bit EVENT_MASK_PRIV of isr_lwevent by calling function _usr_lwevent_set. The PRIV_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_PRIV);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret_val == MQX_OK, "TEST #2: Testing 2.01: Set event bit EVENT_MASK_PRIV");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in PRIV_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret_val == MQX_OK, "TEST #2: Testing 2.02: Wait for event bit EVENT_MASK_MAIN");
    /* Try to receive message from invalid lightweight message queue by calling function _usr_lwmsgq_receive */
    result = _usr_lwmsgq_receive(usr_lwmsgq_ro, temp_msg, 0, 0, 0);
    /* Check the returned result must be LWMSGQ_INVALID to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == LWMSGQ_INVALID, "TEST #2: Testing 2.03: Failed to receive message from invalid lightweight message queue");
    /* Set bit EVENT_MASK_PRIV isr_lwevent by calling function _usr_lwevent_set. The PRIV_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_PRIV);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret_val == MQX_OK, "TEST #2: Testing 2.04: Set event bit EVENT_MASK_PRIV");
    /* Wait bit EVENT_MASK_MAIN for isr_lwevent to be set in PRIV_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ret_val == MQX_OK, "TEST #2: Testing 2.05: Wait for event bit EVENT_MASK_MAIN");

    /* Get the tick time before receiving lightweight message, the ticks value is stored in start_ticks */
    _usr_time_get_elapsed_ticks(&start_ticks);
    /* Wait to receive lightweight message for DELAY_TICKS ticks by calling function _usr_lwmsgq_receive */
    result = _usr_lwmsgq_receive(usr_lwmsgq_ro, temp_msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, DELAY_TICKS, NULL);
    /* Get the tick time after waiting for lightweight event, the ticks value is stored in end_ticks */
    _usr_time_get_elapsed_ticks(&end_ticks);
    /* Check the returned result must be LWMSGQ_TIMEOUT to verify no message was returned */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == LWMSGQ_TIMEOUT, "TEST #2: Testing 2.06: Wait lightweight message for DELAY_TICKS ticks");
    /* Check the different ticks must be equal to DELAY_TICKS to verify accurate waiting time */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, end_ticks.TICKS[0] - start_ticks.TICKS[0] == DELAY_TICKS, "TEST #2: Testing 2.07: Verify the waiting time must be accurate");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   This test case is to verify function _usr_lwmsgq_send in the cases of:
*   - Sending lightweight message to an invalid message queue.
*   - Sending lightweight message to a full message queue.
    - Sending lightweight message in ISR.
* Requirements:
*   USERMODE008, USERMODE016, USERMODE077, USERMODE078, USERMODE087.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    _mqx_uint i;

    tc_number++;

    /* Set bit EVENT_MASK_PRIV of isr_lwevent by calling function _usr_lwevent_set. The PRIV_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_PRIV);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret_val == MQX_OK, "TEST #3: Testing 3.01: Set event bit EVENT_MASK_PRIV");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in PRIV_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret_val == MQX_OK, "TEST #3: Testing 3.02: Wait for event bit EVENT_MASK_MAIN");
    /* Try to send message to invalid lightweight message queue by calling function _usr_lwmsgq_send */
    result = _usr_lwmsgq_send(usr_lwmsgq_ro, temp_msg, 0);
    /* Check the result must be LWMSGQ_INVALID to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == LWMSGQ_INVALID, "TEST #3: Testing 3.03: Failed to send message to invalid message queue");
    /* Set bit EVENT_MASK_PRIV of isr_lwevent by calling function _usr_lwevent_set. The PRIV_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_PRIV);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret_val == MQX_OK, "TEST #3: Testing 3.04: Set event bit EVENT_MASK_PRIV");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in PRIV_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret_val == MQX_OK, "TEST #3: Testing 3.05: Wait for event bit EVENT_MASK_MAIN");

    /* Send message to lightweight message queue usr_lwmsgq_ro until it's full by calling function _usr_lwmsgq_send */
    result = MQX_OK;
    for(i = 0; i<NUM_OF_MSG; i++)
    {
        result |= _usr_lwmsgq_send(usr_lwmsgq_ro, msg[NUM_OF_MSG], 0);
    }
    /* Check the returned result must be MQX_OK to verify successfully sending */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "TEST #3: Testing 3.06: Send message to message queue until it's full");
    /* Try to send message to a full queue by calling function _usr_lwmsgq_send */
    result = _usr_lwmsgq_send(usr_lwmsgq_ro, temp_msg, 0);
    /* Check the returned result must be LWMSGQ_FULL to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == LWMSGQ_FULL, "TEST #3: Testing 3.07: Failed to send message to a full queue");

    /* Set bit EVENT_MASK_ISR of isr_lwevent by calling function _usr_lwevent_set. The PRIV_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_ISR);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret_val == MQX_OK, "TEST #3: Testing 3.08: Set event bit EVENT_MASK_ISR");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in ISR_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret_val == MQX_OK, "TEST #3: Testing 3.09: Wait for event bit EVENT_MASK_MAIN");
    /*
    ** Check the returned result must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR to verify sending
    ** lightweight message in ISR should fail
    */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "TEST #3: Testing 3.10: Failed to send lightweight message in ISR");
}

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_usr_lwmsgq3)
    EU_TEST_CASE_ADD(tc_1_main_task, "TEST # 1 - Testing function _usr_lwmsgq_init in special cases"),
    EU_TEST_CASE_ADD(tc_2_main_task, "TEST # 2 - Testing function _usr_lwmsgq_receive in special cases"),
    EU_TEST_CASE_ADD(tc_3_main_task, "TEST # 3 - Testing function _usr_lwmsgq_send in special cases"),
EU_TEST_SUITE_END(suite_usr_lwmsgq3)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_usr_lwmsgq3, "MQX Usermode Component Test Suite, 3 Tests")
EU_TEST_REGISTRY_END()
/*------------------------------------------------------------------------*/

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   This is the main task with user access rights, this task creates
*   all defined test tasks and lets them run continuously.
*
*END*----------------------------------------------------------------------*/
void main_task(uint32_t param)
{
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : super_task
* Comments     :
*   This is the privilege task to the MQX Kernel program, this task creates
*   main task with user access rights.
*
*END*----------------------------------------------------------------------*/
void super_task(uint32_t param)
{
    _task_id tid;

    /* Install all unexpected interrupt */
    _int_install_unexpected_isr();
    /* Init test case number */
    tc_number = 0;

    /* Create ISR task. With higher priority, this task run immediately */
    tid = _task_create(0, ISR_TASK, 0);
    if(tid == MQX_NULL_TASK_ID)
    {
        printf("Unable to create test application");
        return;
    }

    /* Create PRIV task. With higher priority, this task run immediately */
    tid = _task_create(0, PRIV_TASK, 0);
    if(tid == MQX_NULL_TASK_ID)
    {
        printf("Unable to create test application");
        return;
    }

    /* Create the MAIN_TASK with user access rights and make it ready */
    tid = _task_create(0, MAIN_TASK, 0);
    /* Check if the MAIN_TASK was created */
    if(tid == MQX_NULL_TASK_ID)
    {
        printf("Unable to create test application");
        return;
    }
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : isr_task
* Comments     :
*    This task is to test calling function from interrupt service routine.
*
*END*----------------------------------------------------------------------*/
void isr_task(uint32_t param)
{
    _mqx_uint isr_backup;

    /* Create the isr_lwevent with auto clear bits by calling function _usr_lwevent_create */
    result = _usr_lwevent_create(&isr_lwevent, LWEVENT_AUTO_CLEAR);
    if(result != MQX_OK)
    {
        printf("Unable to create test component");
    }

    /* Wait for isr_lwevent to be set in MAIN_TASK by calling function _usr_lwevent_wait_ticks */
    _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_ISR, TRUE, 0);

    /* Disable all interrupt by calling function _int_disable */
    _int_disable();
    /* Save the current isr counting value to variable isr_backup */
    isr_backup = get_in_isr();
    /* Set the current task as an isr */
    set_in_isr(1);

    /* Call the function _usr_lwmsgq_send to send lightweight message in interrupt service routine */
    result = _usr_lwmsgq_send(usr_lwmsgq_ro, temp_msg, LWMSGQ_SEND_BLOCK_ON_FULL);

    /* Restore the isr counting value */
    set_in_isr(isr_backup);
    /* Enable all interrupt by calling function _int_enable */
    _int_enable();

    /* Set the isr_lwevent by calling function _usr_lwevent_set. The MAIN_TASK runs after this */
    _usr_lwevent_set(&isr_lwevent, EVENT_MASK_MAIN);
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : priv_task
* Comments     :
*   This task is to change a lightweight message queue between valid and
*   invalid state.
*
*END*----------------------------------------------------------------------*/
void priv_task(uint32_t param)
{
    LWMSGQ_STRUCT_PTR lwmsgq_ptr;

    /* Get the pointer of Lightweight message queue struct */
    lwmsgq_ptr = (LWMSGQ_STRUCT_PTR)usr_lwmsgq_ro;
    /* Wait for isr_lwevent to be set in MAIN_TASK by calling function _usr_lwevent_wait_ticks */
    _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_PRIV, TRUE, 0);

    while(TRUE)
    {
        /* Make the message queue usr_lwmsgq_ro to be invalid by changing its VALID field */
        lwmsgq_ptr->VALID = LWMSGQ_VALID + INVALID_VALUE;
        /* Set the isr_lwevent by calling function _usr_lwevent_set. The MAIN_TASK runs after this */
        _usr_lwevent_set(&isr_lwevent, EVENT_MASK_MAIN);
        /* Wait for isr_lwevent to be set in MAIN_TASK by calling function _usr_lwevent_wait_ticks */
        _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_PRIV, TRUE, 0);

        /* Make the semaphore usr_lwmsgq_ro to be valid by restoring its VALID field */
        lwmsgq_ptr->VALID = LWMSGQ_VALID;
        /* Set the isr_lwevent by calling function _usr_lwevent_set. The MAIN_TASK runs after this */
        _usr_lwevent_set(&isr_lwevent, EVENT_MASK_MAIN);
        /* Check if this is the last test case using this task */
        if(tc_number == 3)
        {
            break;
        }
        /* Wait for isr_lwevent to be set in MAIN_TASK by calling function _usr_lwevent_wait_ticks */
        _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_PRIV, TRUE, 0);
    }
}
