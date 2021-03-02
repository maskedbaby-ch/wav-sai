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
*   of Lightweight Semaphore functions in usermode.
*
* Requirements:
*
*   USERMODE008, USERMODE016, USERMODE034, USERMODE035, USERMODE038,
*   USERMODE040, USERMODE042, USERMODE043, USERMODE047, USERMODE048,
*   USERMODE050, USERMODE051, USERMODE052, USERMODE054, USERMODE055,
*   USERMODE056, USERMODE073, USERMODE088, USERMODE089.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mqx_prv.h>
#include "test.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

/*------------------------------------------------------------------------
 * Test suite function prototype
 */
void tc_1_main_task(void);  /* TEST # 1 - Testing function _usr_lwsem_create in special cases */
void tc_2_main_task(void);  /* TEST # 2 - Testing function _usr_lwsem_poll in special cases */
void tc_3_main_task(void);  /* TEST # 3 - Testing function _usr_lwsem_post in special cases */
void tc_4_main_task(void);  /* TEST # 4 - Testing function _usr_lwsem_wait in special cases */
void tc_5_main_task(void);  /* TEST # 5 - Testing function _usr_lwsem_wait_ticks in special cases */
void tc_6_main_task(void);  /* TEST # 6 - Testing function _usr_lwsem_wait_for in special cases */
void tc_7_main_task(void);  /* TEST # 7 - Testing function _usr_lwsem_wait_until in special cases */

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
*   This test case is to verify function _usr_lwsem_create in the cases of:
*   - Accessing a lightweight semaphore without access rights.
*   - Initializing a lightweight semaphore which has already been initialized.
* Requirements:
*   USERMODE034, USERMODE035, USERMODE088, USERMODE089.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    tc_number++;

    /* Initialize the lightweight semaphore usr_lwsem_ro with no initial count by calling function _usr_lwsem_create */
    result = _usr_lwsem_create(&usr_lwsem_ro, 0);
    /* Check the returned result must be MQX_OK to verify successfully initializing */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "TEST #1: Testing 1.01: Initialize a lightweight semaphore");

    /* Try to initialize the lightweight semaphore usr_lwsem_ro again by calling function _usr_lwsem_create */
    result = _usr_lwsem_create(&usr_lwsem_ro, 0);
    /* Check the returned result must be MQX_EINVAL to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_EINVAL, "TEST #1: Testing 1.02: Failed to initialize the lightweight semaphore again");

    /* Try to initialize the lightweight semaphore usr_lwsem_rw by calling function _usr_lwsem_create */
    result = _usr_lwsem_create(&usr_lwsem_rw, 0);
    /*
    ** Check the returned result must be MQX_INVALID_LWSEM to verify initializing a lightweight semaphore
    ** without access rights should fail
    */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_INVALID_LWSEM, "TEST #1: Testing 1.03: Failed to initialize the lightweight semaphore without access rights");

    /* Destroy the lightweight semaphore usr_lwsem_ro by calling function _usr_lwsem_destroy */
    result = _usr_lwsem_destroy(&usr_lwsem_ro);
    /* Check the returned value must be MQX_OK to verify successfully destroying */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "TEST #1: Testing 1.04: Destroy the initialized lightweight semaphore");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   This test case is to verify function _usr_lwsem_poll in the case of:
*   - Polling a lightweight semaphore while it's unavailable.
* Requirements:
*   USERMODE034, USERMODE035, USERMODE038.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    bool bool_result;

    tc_number++;

    /* Initialize the lightweight semaphore usr_lwsem_ro with no initial count by calling function _usr_lwsem_create */
    result = _usr_lwsem_create(&usr_lwsem_ro, 0);
    /* Check the returned result must be MQX_OK to verify successfully initializing */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "TEST #2: Testing 2.01: Initialize a lightweight semaphore");

    /* Try to poll lightweight semaphore usr_lwsem_ro when it's unavailable by calling function _usr_lwsem_poll */
    bool_result = _usr_lwsem_poll(&usr_lwsem_ro);
    /* Check the result must be FALSE to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == FALSE, "TEST #2: Testing 2.02: Failed to poll lightweight semaphore while it's unavailable");

    /* Destroy the lightweight semaphore usr_lwsem_ro by calling function _usr_lwsem_destroy */
    result = _usr_lwsem_destroy(&usr_lwsem_ro);
    /* Check the returned value must be MQX_OK to verify successfully destroying */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "TEST #2: Testing 2.03: Destroy the initialized lightweight semaphore");
}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   This test case is to verify function _usr_lwsem_post in the case of:
*   - Posting an invalid lightweight semaphore.
* Requirements:
*   USERMODE008, USERMODE016, USERMODE034, USERMODE035, USERMODE040.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    tc_number++;

    /* Initialize the lightweight semaphore usr_lwsem_ro with no initial count by calling function _usr_lwsem_create */
    result = _usr_lwsem_create(&usr_lwsem_ro, 0);
    /* Check the returned result must be MQX_OK to verify successfully initializing */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "TEST #3: Testing 3.01: Initialize a lightweight semaphore");

    /* Set bit EVENT_MASK_PRIV of isr_lwevent by calling function _usr_lwevent_set. The PRIV_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_PRIV);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret_val == MQX_OK, "TEST #3: Testing 3.02: Set event bit EVENT_MASK_PRIV");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in PRIV_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret_val == MQX_OK, "TEST #3: Testing 3.03: Wait event bit EVENT_MASK_MAIN");
    /* Try to post the invalid lightweight semaphore usr_lwsem_ro by calling function _usr_lwsem_post */
    result = _usr_lwsem_post(&usr_lwsem_ro);
    /*
    ** Check the returned result must be MQX_INVALID_LWSEM to verify accessing an invalid lightweight semaphore
    ** should fail
    */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_INVALID_LWSEM, "TEST #3: Testing 3.04: Failed to access an invalid lightweight semaphore");
    /* Set bit EVENT_MASK_PRIV of isr_lwevent by calling function _usr_lwevent_set. The PRIV_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_PRIV);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret_val == MQX_OK, "TEST #3: Testing 3.05: Set event bit EVENT_MASK_PRIV");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in PRIV_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret_val == MQX_OK, "TEST #3: Testing 3.06: Wait event bit EVENT_MASK_MAIN");

    /* Destroy the lightweight semaphore usr_lwsem_ro by calling function _usr_lwsem_destroy */
    result = _usr_lwsem_destroy(&usr_lwsem_ro);
    /* Check the returned value must be MQX_OK to verify successfully destroying */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "TEST #3: Testing 3.07: Destroy the initialized lightweight semaphore");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*   This test case is to verify function _usr_lwsem_wait in the cases of:
*   - Accessing an invalid lightweight semaphore.
*   - Waiting for a lightweight semaphore in ISR.
* Requirements:
*   USERMODE008, USERMODE016, USERMODE034, USERMODE035, USERMODE042,
*   USERMODE043.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    tc_number++;

    /* Initialize the lightweight semaphore usr_lwsem_ro with 1 initial count by calling function _usr_lwsem_create */
    result = _usr_lwsem_create(&usr_lwsem_ro, 1);
    /* Check the returned result must be MQX_OK to verify successfully initializing */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "TEST #4: Testing 4.01: Initialize a lightweight semaphore");

    /* Set bit EVENT_MASK_PRIV of isr_lwevent by calling function _usr_lwevent_set. The PRIV_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_PRIV);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ret_val == MQX_OK, "TEST #4: Testing 4.02: Set event bit EVENT_MASK_PRIV");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in PRIV_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ret_val == MQX_OK, "TEST #4: Testing 4.03: Wait event bit EVENT_MASK_MAIN");
    /* Try to wait for the invalid lightweight semaphore usr_lwsem_ro by calling function _usr_lwsem_wait */
    result = _usr_lwsem_wait(&usr_lwsem_ro);
    /*
    ** Check the returned result must be MQX_INVALID_LWSEM to verify accessing an invalid lightweight semaphore
    ** should fail.
    */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_INVALID_LWSEM, "TEST #4: Testing 4.04: Failed to wait for an invalid lightweight semaphore");
    /* Set bit EVENT_MASK_PRIV of isr_lwevent by calling function _usr_lwevent_set. The PRIV_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_PRIV);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ret_val == MQX_OK, "TEST #4: Testing 4.05: Set event bit EVENT_MASK_PRIV");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in PRIV_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ret_val == MQX_OK, "TEST #4: Testing 4.06: Wait event bit EVENT_MASK_MAIN");

    /* Set bit EVENT_MASK_ISR of isr_lwevent by calling function _usr_lwevent_set. The ISR_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_ISR);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ret_val == MQX_OK, "TEST #4: Testing 4.07: Set event bit EVENT_MASK_ISR");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in ISR_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ret_val == MQX_OK, "TEST #4: Testing 4.08: Wait event bit EVENT_MASK_MAIN");
    /*
    ** Check the returned result must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR to verify waiting
    ** lightweight semaphore in ISR should fail
    */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "TEST #4: Testing 4.09: Failed to wait for lightweight semaphore in ISR");

    /* Destroy the lightweight semaphore usr_lwsem_ro by calling function _usr_lwsem_destroy */
    result = _usr_lwsem_destroy(&usr_lwsem_ro);
    /* Check the returned value must be MQX_OK to verify successfully destroying */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "TEST #4: Testing 4.010: Destroy the initialized lightweight semaphore");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5:
*   This test case is to verify function _usr_lwsem_wait_ticks in the cases of:
*   - Accessing an invalid lightweight semaphore.
*   - Waiting for a lightweight semaphore in ISR.
*   - Waiting for a lightweight semaphore until timeout.
* Requirements:
*   USERMODE008, USERMODE016, USERMODE034, USERMODE035, USERMODE050,
*   USERMODE051, USERMODE052, USERMODE073.
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;

    tc_number++;

    /* Initialize the lightweight semaphore usr_lwsem_ro with no initial count by calling function _usr_lwsem_create */
    result = _usr_lwsem_create(&usr_lwsem_ro, 0);
    /* Check the returned result must be MQX_OK to verify successfully initializing */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "TEST #5: Testing 5.01: Initialize a lightweight semaphore");

    /* Get the tick time before waiting for lightweight semaphore, the ticks value is stored in start_ticks */
    _usr_time_get_elapsed_ticks(&start_ticks);
    /* Wait lightweight semaphore usr_lwsem_ro for DELAY_TICKS ticks by calling function _usr_lwsem_wait_ticks */
    result = _usr_lwsem_wait_ticks(&usr_lwsem_ro, DELAY_TICKS);
    /* Get the tick time after waiting for lightweight semaphore, the ticks value is stored in end_ticks */
    _usr_time_get_elapsed_ticks(&end_ticks);
    /* Check the returned result must be MQX_LWSEM_WAIT_TIMEOUT to verify there's no semaphore was posted */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_LWSEM_WAIT_TIMEOUT, "TEST #5: Testing 5.02: Wait lightweight semaphore for DELAY_TICKS ticks");
    /* Check the different ticks must be equal to DELAY_TICKS to verify accurate waiting time */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, end_ticks.TICKS[0] - start_ticks.TICKS[0] == DELAY_TICKS, "TEST #5: Testing 5.03: Verify the waiting time must be accurate");

    /* Set bit EVENT_MASK_PRIV of isr_lwevent by calling function _usr_lwevent_set. The PRIV_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_PRIV);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, ret_val == MQX_OK, "TEST #5: Testing 5.04: Set event bit EVENT_MASK_PRIV");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in PRIV_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, ret_val == MQX_OK, "TEST #5: Testing 5.05: Wait event bit EVENT_MASK_MAIN");
    /* Try to wait for the invalid lightweight semaphore usr_lwsem_ro by calling function _usr_lwsem_wait_ticks */
    result = _usr_lwsem_wait_ticks(&usr_lwsem_ro, DELAY_TICKS);
    /*
    ** Check the returned result must be MQX_INVALID_LWSEM to verify accessing an invalid lightweight semaphore
    ** should fail.
    */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_INVALID_LWSEM, "TEST #5: Testing 5.06: Failed to wait for an invalid lightweight semaphore");
    /* Set bit EVENT_MASK_PRIV of isr_lwevent by calling function _usr_lwevent_set. The PRIV_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_PRIV);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, ret_val == MQX_OK, "TEST #5: Testing 5.07: Set event bit EVENT_MASK_PRIV");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in PRIV_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, ret_val == MQX_OK, "TEST #5: Testing 5.08: Wait event bit EVENT_MASK_MAIN");

    /* Set bit EVENT_MASK_ISR of isr_lwevent by calling function _usr_lwevent_set. The ISR_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_ISR);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, ret_val == MQX_OK, "TEST #5: Testing 5.09: Set event bit EVENT_MASK_ISR");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in ISR_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, ret_val == MQX_OK, "TEST #5: Testing 5.10: Wait event bit EVENT_MASK_MAIN");
    /*
    ** Check the returned result must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR to verify waiting
    ** lightweight semaphore in ISR should fail
    */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "TEST #5: Testing 5.11: Failed to wait for lightweight semaphore in ISR");

    /* Destroy the lightweight semaphore usr_lwsem_ro by calling function _usr_lwsem_destroy */
    result = _usr_lwsem_destroy(&usr_lwsem_ro);
    /* Check the returned value must be MQX_OK to verify successfully destroying */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "TEST #5: Testing 5.12: Destroy the initialized lightweight semaphore");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6:
*   This test case is to verify function _usr_lwsem_wait_for in the cases of:
*   - Accessing an invalid lightweight semaphore.
*   - Waiting for a lightweight semaphore until timeout.
* Requirements:
*   USERMODE008, USERMODE016, USERMODE034, USERMODE035, USERMODE047,
*   USERMODE048, USERMODE073.
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;

    tc_number++;

    /* Initialize the lightweight semaphore with no initial semaphore by calling function _usr_lwsem_create */
    result = _usr_lwsem_create(&usr_lwsem_ro, 0);
    /* Check the returned result must be MQX_OK to verify successfully initializing */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "TEST #6: Testing 6.01: Initialize a lightweight semaphore");

    /* Initialize time structure with DELAY_TICKS ticks and store in variable ticks*/
    _time_init_ticks(&ticks, DELAY_TICKS);
    /* Get the tick time before waiting for lightweight semaphore, the ticks value is stored in start_ticks */
    _usr_time_get_elapsed_ticks(&start_ticks);
    /* Wait lightweight semaphore usr_lwsem_ro for DELAY_TICKS ticks by calling function _usr_lwsem_wait_for */
    result = _usr_lwsem_wait_for(&usr_lwsem_ro, &ticks);
    /* Get the tick time after waiting for lightweight semaphore, the ticks value is stored in end_ticks */
    _usr_time_get_elapsed_ticks(&end_ticks);
    /* Check the returned result must be MQX_LWSEM_WAIT_TIMEOUT to verify there's no semaphore was posted */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_LWSEM_WAIT_TIMEOUT, "TEST #6: Testing 6.02: Wait lightweight semaphore for DELAY_TICKS ticks");
    /* Check the different ticks must be equal to DELAY_TICKS to verify accurate waiting time */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, end_ticks.TICKS[0] - start_ticks.TICKS[0] == DELAY_TICKS, "TEST #6: Testing 6.03: Verify the waiting time must be accurate");

    /* Set bit EVENT_MASK_PRIV of isr_lwevent by calling function _usr_lwevent_set. The PRIV_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_PRIV);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, ret_val == MQX_OK, "TEST #6: Testing 6.04: Set event bit EVENT_MASK_PRIV");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in PRIV_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, ret_val == MQX_OK, "TEST #6: Testing 6.05: Wait event bit EVENT_MASK_MAIN");
    /* Try to wait for the invalid lightweight semaphore usr_lwsem_ro by calling function _usr_lwsem_wait_for */
    result = _usr_lwsem_wait_for(&usr_lwsem_ro, &ticks);
    /*
    ** Check the returned result must be MQX_INVALID_LWSEM to verify accessing an invalid lightweight semaphore
    ** should fail
    */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_INVALID_LWSEM, "TEST #6: Testing 6.06: Failed to wait for an invalid lightweight semaphore");
    /* Set bit EVENT_MASK_PRIV of isr_lwevent by calling function _usr_lwevent_set. The PRIV_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_PRIV);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, ret_val == MQX_OK, "TEST #6: Testing 6.07: Set event bit EVENT_MASK_PRIV");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in PRIV_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, ret_val == MQX_OK, "TEST #6: Testing 6.08: Wait event bit EVENT_MASK_MAIN");

    /* Destroy the lightweight semaphore usr_lwsem_ro by calling function _usr_lwsem_destroy */
    result = _usr_lwsem_destroy(&usr_lwsem_ro);
    /* Check the returned value must be MQX_OK to verify successfully destroying */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "TEST #6: Testing 6.09: Destroy the initialized lightweight semaphore");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7:
*   This test case is to verify function _usr_lwsem_wait_until in the cases of:
*   - Accessing an invalid lightweight semaphore.
*   - Waiting for a lightweight semaphore in ISR.
*   - Waiting for a lightweight semaphore until timeout.
* Requirements:
*   USERMODE008, USERMODE016, USERMODE034, USERMODE035, USERMODE054,
*   USERMODE055, USERMODE056, USERMODE073.
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;

    tc_number++;

    /* Initialize the lightweight semaphore with no initial semaphore by calling function _usr_lwsem_create */
    result = _usr_lwsem_create(&usr_lwsem_ro, 0);
    /* Check the returned result must be MQX_OK to verify successfully initializing */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "TEST #7: Testing 7.01: Initialize a lightweight semaphore");

    /* Get the tick time before waiting for lightweight semaphore, the ticks value is stored in start_ticks */
    _usr_time_get_elapsed_ticks(&start_ticks);
    /* Calculate the moment (in tick time) to wait until and store in variable ticks*/
    PSP_ADD_TICKS_TO_TICK_STRUCT(&start_ticks, DELAY_TICKS, &ticks);
    /* Wait lightweight semaphore usr_lwsem_ro for DELAY_TICKS ticks by calling function _usr_lwsem_wait_until */
    result = _usr_lwsem_wait_until(&usr_lwsem_ro, &ticks);
    /* Get the tick time after waiting for lightweight semaphore, the ticks value is stored in end_ticks */
    _usr_time_get_elapsed_ticks(&end_ticks);
    /* Check the returned result must be MQX_LWSEM_WAIT_TIMEOUT to verify there's no semaphore was posted */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_LWSEM_WAIT_TIMEOUT, "TEST #7: Testing 7.02: Wait lightweight semaphore for DELAY_TICKS ticks");
    /* Check the different ticks must be equal to DELAY_TICKS to verify accurate waiting time */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, end_ticks.TICKS[0] - start_ticks.TICKS[0] == DELAY_TICKS, "TEST #7: Testing 7.03: Verify the waiting time must be accurate");

    /* Calculate a far moment (in tick time) to wait until and store in variable ticks*/
    PSP_ADD_TICKS_TO_TICK_STRUCT(&end_ticks, LONG_DELAY_TICKS, &ticks);

    /* Set bit EVENT_MASK_PRIV of isr_lwevent by calling function _usr_lwevent_set. The PRIV_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_PRIV);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, ret_val == MQX_OK, "TEST #7: Testing 7.04: Set event bit EVENT_MASK_PRIV");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in PRIV_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, ret_val == MQX_OK, "TEST #7: Testing 7.05: Wait event bit EVENT_MASK_MAIN");
    /* Try to wait for the invalid lightweight semaphore usr_lwsem_ro by calling function _usr_lwsem_wait_until */
    result = _usr_lwsem_wait_until(&usr_lwsem_ro, &ticks);
    /*
    ** Check the returned result must be MQX_INVALID_LWSEM to verify accessing an invalid lightweight semaphore
    ** should fail
    */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_INVALID_LWSEM, "TEST #7: Testing 7.06: Failed to wait for an invalid lightweight semaphore");
    /* Set bit EVENT_MASK_PRIV of isr_lwevent by calling function _usr_lwevent_set. The PRIV_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_PRIV);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, ret_val == MQX_OK, "TEST #7: Testing 7.07: Set event bit EVENT_MASK_PRIV");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in PRIV_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, ret_val == MQX_OK, "TEST #7: Testing 7.08: Wait event bit EVENT_MASK_MAIN");

    /* Set bit EVENT_MASK_ISR of isr_lwevent by calling function _usr_lwevent_set. The ISR_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_ISR);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, ret_val == MQX_OK, "TEST #7: Testing 7.09: Set event bit EVENT_MASK_ISR");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in ISR_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, ret_val == MQX_OK, "TEST #7: Testing 7.10: Wait event bit EVENT_MASK_MAIN");
    /*
    ** Check the returned result must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR to verify waiting
    ** lightweight semaphore in ISR should fail
    */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "TEST #7: Testing 7.11: Failed to wait for lightweight semaphore in ISR");

    /* Destroy the lightweight semaphore usr_lwsem_ro by calling function _usr_lwsem_destroy */
    result = _usr_lwsem_destroy(&usr_lwsem_ro);
    /* Check the returned value must be MQX_OK to verify successfully destroying */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "TEST #7: Testing 7.12: Destroy the initialized lightweight semaphore");
}

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_usr_lwsem5)
    EU_TEST_CASE_ADD(tc_1_main_task, "TEST # 1 - Testing function _usr_lwsem_create in special cases"),
    EU_TEST_CASE_ADD(tc_2_main_task, "TEST # 2 - Testing function _usr_lwsem_poll in special cases"),
    EU_TEST_CASE_ADD(tc_3_main_task, "TEST # 3 - Testing function _usr_lwsem_post in special cases"),
    EU_TEST_CASE_ADD(tc_4_main_task, "TEST # 4 - Testing function _usr_lwsem_wait in special cases"),
    EU_TEST_CASE_ADD(tc_5_main_task, "TEST # 5 - Testing function _usr_lwsem_wait_ticks in special cases"),
    EU_TEST_CASE_ADD(tc_6_main_task, "TEST # 6 - Testing function _usr_lwsem_wait_for in special cases"),
    EU_TEST_CASE_ADD(tc_7_main_task, "TEST # 7 - Testing function _usr_lwsem_wait_until in special cases"),
EU_TEST_SUITE_END(suite_usr_lwsem5)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_usr_lwsem5, "MQX Usermode Component Test Suite, 7 Tests")
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
*   This task is to test calling function from interrupt service routine.
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

    while(TRUE)
    {
        /* Wait for isr_lwevent to be set in MAIN_TASK by calling function _usr_lwevent_wait_ticks */
        result = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_ISR, TRUE, 0);

        /* Disable all interrupt by calling function _int_disable */
        _int_disable();
        /* Save the current isr counting value to variable isr_backup */
        isr_backup = get_in_isr();
        /* Set the current task as an isr */
        set_in_isr(1);

        switch(tc_number)
        {
            case 4:
                /* Call the function _usr_lwsem_wait to wait for lightweight semaphore usr_lwsem_ro */
                result = _usr_lwsem_wait(&usr_lwsem_ro);
                break;
            case 5:
                /* Call the function _usr_lwsem_wait_ticks to wait for lightweight semaphore usr_lwsem_ro */
                result = _usr_lwsem_wait_ticks(&usr_lwsem_ro, DELAY_TICKS);
                break;
            case 7:
                /* Call the function _usr_lwsem_wait_until to wait for lightweight semaphore usr_lwsem_ro */
                result = _usr_lwsem_wait_until(&usr_lwsem_ro, &ticks);
                break;
            default:
                break;
        }

        /* Restore the isr counting value */
        set_in_isr(isr_backup);
        /* Enable all interrupt by calling function _int_enable */
        _int_enable();

        /* Set the isr_lwevent by calling function _usr_lwevent_set. The MAIN_TASK runs after this */
        _usr_lwevent_set(&isr_lwevent, EVENT_MASK_MAIN);
        /* Check if this is the last test case using this task */
        if(tc_number == 7)
        {
            break;
        }
    }
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : priv_task
* Comments     :
*   This task is to change a lightweight semaphore between valid and
*   invalid state.
*
*END*----------------------------------------------------------------------*/
void priv_task(uint32_t param)
{
    /* Wait for isr_lwevent to be set in MAIN_TASK by calling function _usr_lwevent_wait_ticks */
    _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_PRIV, TRUE, 0);

    while(TRUE)
    {
        /* Make the semaphore usr_lwsem_ro to be invalid by changing its VALID field */
        usr_lwsem_ro.VALID = LWSEM_VALID + INVALID_VALUE;
        /* Set the isr_lwevent by calling function _usr_lwevent_set. The MAIN_TASK runs after this */
        _usr_lwevent_set(&isr_lwevent, EVENT_MASK_MAIN);
        /* Wait for isr_lwevent to be set in MAIN_TASK by calling function _usr_lwevent_wait_ticks */
        _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_PRIV, TRUE, 0);

        /* Make the semaphore usr_lwsem_ro to be valid by restoring its VALID field */
        usr_lwsem_ro.VALID = LWSEM_VALID;
        /* Set the isr_lwevent by calling function _usr_lwevent_set. The MAIN_TASK runs after this */
        _usr_lwevent_set(&isr_lwevent, EVENT_MASK_MAIN);
        /* Check if this is the last test case using this task */
        if(tc_number == 7)
        {
            break;
        }
        /* Wait for isr_lwevent to be set in MAIN_TASK by calling function _usr_lwevent_wait_ticks */
        _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_PRIV, TRUE, 0);
    }
}
