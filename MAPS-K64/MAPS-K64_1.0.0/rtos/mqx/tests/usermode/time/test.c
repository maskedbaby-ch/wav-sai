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
* $Version : 4.0.2.1$
* $Date    : Jun-19-2013$
*
* Comments:
*
*   This file contains code for the MQX Usermode verification program,
*   includes all remaining test cases, which haven't been verified yet,
*   of Time functions in usermode.
*
* Requirements:
*
*   USERMODE071, USERMODE072, USERMODE073.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include "test.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

/*------------------------------------------------------------------------
 * Test suite function prototype
 */
void tc_1_main_task(void);  /* TEST # 1 - Testing function _usr_time_delay */
void tc_2_main_task(void);  /* TEST # 2 - Testing function _usr_time_delay_ticks */
void tc_3_main_task(void);  /* TEST # 3 - Testing function _usr_time_get_elapsed_ticks */

/*------------------------------------------------------------------------
 * MQX task template
 */
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { SUPER_TASK, super_task, 1500, 10, "super", MQX_AUTO_START_TASK },
   {  MAIN_TASK,  main_task, 3000,  9,  "main",       MQX_USER_TASK },
   {          0,          0,    0,  0,       0,                   0 }
};

/******************** Begin Test cases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*   This test case is to verify function _usr_time_delay in the case of:
*   - Suspending task for number of miliseconds.
* Requirements:
*   USERMODE071.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;
    MQX_TICK_STRUCT diff_ticks;
    TIME_STRUCT time;

    /* Get the tick time before delay, the ticks value is stored in start_ticks */
    _usr_time_get_elapsed_ticks(&start_ticks);
    /* Suspend main task for DELAY_MS milliseconds by calling function _usr_time_delay */
    _usr_time_delay(DELAY_MS);
    /* Get the tick time after delay, the ticks value is stored in end_ticks */
    _usr_time_get_elapsed_ticks(&end_ticks);
    /* Calculate the different ticks */
    PSP_SUB_TICKS(&end_ticks, &start_ticks, &diff_ticks);
    /* Convert from ticks to time */
    PSP_TICKS_TO_TIME(&diff_ticks, &time);

    /* Check the different milliseconds must be equal to DELAY_MS to verify accurate delay time */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, time_check_struct(time, 0, DELAY_MS, 1), "TEST #1: Testing 1.01: Verify the delay time must be accurate");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   This test case is to verify function _usr_time_delay_ticks in the case of:
*   - Suspending task for number of ticks.
* Requirements:
*   USERMODE072.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;
    MQX_TICK_STRUCT ticks;

    /* Get the tick time before delay, the ticks value is stored in start_ticks */
    _usr_time_get_elapsed_ticks(&start_ticks);
    /* Suspend main task for DELAY_TICKS ticks by calling function _usr_time_delay_ticks */
    _usr_time_delay_ticks(DELAY_TICKS);
    /* Get the tick time after delay, the ticks value is stored in end_ticks */
    _usr_time_get_elapsed_ticks(&end_ticks);

    PSP_ADD_TICKS_TO_TICK_STRUCT(&start_ticks, DELAY_TICKS, &ticks);

    /* Check the different ticks must be equal to DELAY_TICKS to verify accurate delay time */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, PSP_CMP_TICKS(&ticks, &end_ticks) == 0, "TEST #2: Testing 2.01: Verify the delay time must be accurate");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   This test case is to verify function _usr_time_get_elapsed_ticks in
*   the case of:
*   - Getting elapsed time (in ticks) since MQX started.
* Requirements:
*   USERMODE073.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    MQX_TICK_STRUCT start_ticks;
    MQX_TICK_STRUCT end_ticks;

    /* Get the tick time before delay, the ticks value is stored in start_ticks */
    _usr_time_get_elapsed_ticks(&start_ticks);
    /* Suspend main task for 1 ticks by calling function _usr_time_delay_ticks */
    _usr_time_delay_ticks(1);
    /* Get the tick time after delay, the ticks value is stored in end_ticks */
    _usr_time_get_elapsed_ticks(&end_ticks);

    /* Check the ticks of end_ticks must be bigger than start_ticks to verify correct elapsed time */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, PSP_CMP_TICKS(&end_ticks, &start_ticks) == 1, "TEST #3: Testing 3.01: Verify correct elapsed time");
}

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_usr_time)
    EU_TEST_CASE_ADD(tc_1_main_task, "TEST # 1 - Testing function _usr_time_delay"),
    EU_TEST_CASE_ADD(tc_2_main_task, "TEST # 2 - Testing function _usr_time_delay_ticks"),
    EU_TEST_CASE_ADD(tc_3_main_task, "TEST # 3 - Testing function _usr_time_get_elapsed_ticks"),
EU_TEST_SUITE_END(suite_usr_time)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_usr_time, "MQX Usermode Component Test Suite, 3 Tests")
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

    /* Create the MAIN_TASK with user access rights and make it ready */
    tid = _task_create(0, MAIN_TASK, 0);
    /* Check if the MAIN_TASK was created */
    if(tid == MQX_NULL_TASK_ID)
    {
        printf("Unable to create test application");
        return;
    }
}
