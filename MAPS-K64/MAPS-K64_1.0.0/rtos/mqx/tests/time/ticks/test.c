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
* $Date    : Oct-2-2012$
*
* Comments:
*
*   This file contains the source functions for testing the accuracy
*   of the timer.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
//#include <EUnit_Api.h>
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define MAIN_TASK      10
#define FILE_BASENAME test

#define TEST_TICK_DURATION 10


void main_task(uint32_t parameter);

void tc_1_main_task(void); /* TEST #1: Test the accuracy of timer. */

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK, main_task, 2000, 8, "Main", MQX_AUTO_START_TASK},
   { 0,         0,         0,    0, 0,      0 }
};

/*TASK--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test the accuracy of timer
*
*END*--------------------------------------------------------------------*/

void tc_1_main_task(void)
{
   TIME_STRUCT  time;
   uint32_t      result = 1;
   uint16_t      deltaT = 0, current_time = 0, old_time = 0, count = 0;

   //align test execution with BSP TIMER
   _time_delay_ticks(1);
   _time_get(&time);
   old_time = time.SECONDS * 1000 + time.MILLISECONDS;

   while (count < 10)
   {
      /* Tick every second */
      _time_delay_ticks(BSP_ALARM_FREQUENCY);

      /* Gets the current time */
      _time_get(&time);

      current_time = time.SECONDS * 1000 + time.MILLISECONDS;
      deltaT = current_time - old_time;
      old_time = current_time;

      count++;

      if ((deltaT <= 990) || (deltaT >= 1050))
      {
        result = 0;
        break;
      }
   } /* Endwhile */

   EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == 1, "Test #1 : Checking time operation\n")

}
/*TASK--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #1: Test if system time is still continuously increasing(Cannot go backwards)
*
*END*--------------------------------------------------------------------*/

void tc_2_main_task(void)
{
    MQX_TICK_STRUCT mqx_time;
    uint32_t tmp_tick, tmp_hwtick, test_duration;
    bool check = TRUE;

    /* Store actual time of MQX */
    _time_get_elapsed_ticks(&mqx_time);
    tmp_tick =  PSP_GET_ELEMENT_FROM_TICK_STRUCT(&mqx_time.TICKS, 0);
    tmp_hwtick = mqx_time.HW_TICKS;

    /* Compute duration of test */
    test_duration = tmp_tick + TEST_TICK_DURATION;

    while(tmp_tick < test_duration)
    {
        /* Get actual time of MQX */
        _time_get_elapsed_ticks(&mqx_time);
        /* Check if TICK are more than last time */
        if (tmp_tick > PSP_GET_ELEMENT_FROM_TICK_STRUCT(&mqx_time.TICKS, 0))
        {
            check = FALSE;
            break;
        }
        /* If ticks are the same , check subticks */
        if (tmp_tick == PSP_GET_ELEMENT_FROM_TICK_STRUCT(&mqx_time.TICKS, 0))
        {
            if (tmp_hwtick > mqx_time.HW_TICKS)
            {
                check = FALSE;
                break;
            }
        }
        tmp_tick = PSP_GET_ELEMENT_FROM_TICK_STRUCT(&mqx_time.TICKS, 0);
        tmp_hwtick = mqx_time.HW_TICKS;
    }

    EU_ASSERT_REF_TC_MSG( tc_2_main_task, check, "Test #2 : System time continuously increasing Test\n")

}

/* Define Test Suite*/
 EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test # 1: Test the accuracy of timer"),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test # 2: System time continuously increasing Test ")
 EU_TEST_SUITE_END(suite_1)

/* Add test suites*/
 EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, " ticks - Test the accuracy of timer")
 EU_TEST_REGISTRY_END()

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   Just read and display the time.
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t parameter
   )
{
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();

} /* Endbody */

/* EOF */
