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
* $Version : 4.0.2.0$
* $Date    : Jul-22-2013$
*
* Comments:
*
*   This file contains the source for the lwtimer testing.
*
*END************************************************************************/

/************************* Attention *****************************************
*        printf operation can affect lwtimer's counter and lead to test failure.
* These test cases here can only work well with printf disabled.
****************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <lwtimer.h>
#include <lwtimer_prv.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"


#define FILE_BASENAME   test
/* General defines */
#define MAIN_TASK  10
#define TEST_TASK  11

extern void    test_task(uint32_t);
extern void    main_task(uint32_t);

void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
void tc_4_main_task(void);
void tc_5_main_task(void);


TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK, main_task,  2000,   7, "Main_Task", MQX_AUTO_START_TASK},
   { TEST_TASK, test_task,  1000,   7, "Test_Task", 0                  },
   { 0,         0,          0,      0, 0,           0                  }
};

volatile uint32_t cnt1 = 0;
volatile uint32_t cnt2 = 0;
volatile uint32_t cnt3 = 0;
volatile uint32_t cnt4 = 0;

/*FUNCTION*---------------------------------------------------------------
*
* Function Name: timer_function
* Comments     :
*
*END*----------------------------------------------------------------------*/
void timer_function
(
void *param
)
{ /* Body */

   (*(uint32_t *)param)++;

} /* Endbody */

/*TASK*-------------------------------------------------------------------
*
* Task Name    : test_task
* Comments     :
*
*END*----------------------------------------------------------------------*/
void test_task
(
uint32_t dummy
)
{
   _task_block();

} /* Endbody */

/******************** Begin Testcases **********************/
LWTIMER_PERIOD_STRUCT  period1;
LWTIMER_STRUCT         timer1;
LWTIMER_STRUCT         timer2;
LWTIMER_STRUCT         timer3;
LWTIMER_STRUCT         timer4;

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Testing one timer
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   _mqx_uint              result;

   _time_delay_ticks(1);
   result = _lwtimer_create_periodic_queue(&period1, 2, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.1: _lwmtimer_create_periodic_queue() operation")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   result = _lwtimer_add_timer_to_queue(&period1, &timer1, 3, timer_function,
   (void *)&cnt1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.2: _lwtimer_add_timer_to_queue operation should have returned failed")
   if (result == MQX_OK) {
      _task_block();
   } /* Endif */
   result = _lwtimer_add_timer_to_queue(&period1, &timer1, 1, timer_function,
   (void *)&cnt1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.3: _lwmtimer_add_timer_to_queue() operation should be OK")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   _time_delay_ticks(1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, cnt1 == 0, "Test #1: Testing 1.4: Count should be zero")
   _time_delay_ticks(1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, cnt1 == 1, "Test #1: Testing 1.5: Count should be 1")
   _time_delay_ticks(1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, cnt1 == 1, "Test #1: Testing 1.6: Count should be 1")
   _time_delay_ticks(1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, cnt1 == 2, "Test #1: Testing 1.7: Count should be 2")
   result=_lwtimer_cancel_timer(&timer1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.8: _lwtimer_cancel_timer() operation")
   result=_lwtimer_cancel_period(&period1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.9: _lwtimer_cancel_period() operation")
   cnt1 = 0;
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Testing four timers
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
   _mqx_uint              result;

   _time_delay_ticks(1);
   result = _lwtimer_create_periodic_queue(&period1, 4, 0);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.1: Create a periodic timer queue")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   result = _lwtimer_add_timer_to_queue(&period1, &timer1, 1, timer_function,
   (void *)&cnt1);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.2: Add lightweight timer1 to the periodic queue")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   result = _lwtimer_add_timer_to_queue(&period1, &timer2, 0, timer_function,
   (void *)&cnt2);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.3: Add lightweight timer2 to the periodic queue")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   result = _lwtimer_add_timer_to_queue(&period1, &timer3, 2, timer_function,
   (void *)&cnt3);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.4: Add lightweight timer3 to the periodic queue")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   result = _lwtimer_add_timer_to_queue(&period1, &timer4, 1, timer_function,
   (void *)&cnt4);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.5: Add lightweight timer4 to the periodic queue")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   _time_delay_ticks(1);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ((cnt1 == 0) && (cnt2 == 1) && (cnt3 == 0) && (cnt4 == 0)), "Test #2: Testing 2.6: Check all count")

   _time_delay_ticks(1);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ((cnt1 == 1) && (cnt2 == 1) && (cnt3 == 0) && (cnt4 == 1)), "Test #2: Testing 2.7: Check all count")

   _time_delay_ticks(1);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ((cnt1 == 1) && (cnt2 == 1) && (cnt3 == 1) && (cnt4 == 1)), "Test #2: Testing 2.8: Check all count")

   _time_delay_ticks(1);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ((cnt1 == 1) && (cnt2 == 1) && (cnt3 == 1) && (cnt4 == 1)), "Test #2: Testing 2.9: Check all count")

   _time_delay_ticks(1);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, ((cnt1 == 1) && (cnt2 == 2) && (cnt3 == 1) && (cnt4 == 1)), "Test #2: Testing 2.10: Check all count")

   _lwtimer_cancel_period(&period1);
   cnt1 = cnt2 = cnt3 = cnt4 = 0;
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Testing four timers, 2 periods
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
   _mqx_uint              result;
   LWTIMER_PERIOD_STRUCT  period2;

   _time_delay_ticks(1);
   result = _lwtimer_create_periodic_queue(&period1, 4, 0);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.1: Create a periodic timer queue")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   result = _lwtimer_add_timer_to_queue(&period1, &timer1, 1, timer_function,
   (void *)&cnt1);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.2: Add lightweight timer1 to the periodic queue")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   result = _lwtimer_add_timer_to_queue(&period1, &timer2, 2, timer_function,
   (void *)&cnt2);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.3: Add lightweight timer2 to the periodic queue")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   result = _lwtimer_create_periodic_queue(&period2, 2, 0);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.4: Create a periodic timer queue")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   result = _lwtimer_add_timer_to_queue(&period2, &timer3, 0, timer_function,
   (void *)&cnt3);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.5: Add lightweight timer3 to the periodic queue")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   result = _lwtimer_add_timer_to_queue(&period2, &timer4, 1, timer_function,
   (void *)&cnt4);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.6: Add lightweight timer4 to the periodic queue")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   _time_delay_ticks(1);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((cnt1 == 0) && (cnt2 == 0) && (cnt3 == 1) && (cnt4 == 0)), "Test #3: Testing 3.7: Check all count")

   _time_delay_ticks(1);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((cnt1 == 1) && (cnt2 == 0) && (cnt3 == 1) && (cnt4 == 1)), "Test #3: Testing 3.8: Check all count")

   _time_delay_ticks(1);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((cnt1 == 1) && (cnt2 == 1) && (cnt3 == 2) && (cnt4 == 1)), "Test #3: Testing 3.9: Check all count")

   _time_delay_ticks(1);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((cnt1 == 1) && (cnt2 == 1) && (cnt3 == 2) && (cnt4 == 2)), "Test #3: Testing 3.10: Check all count")

   _time_delay_ticks(1);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((cnt1 == 1) && (cnt2 == 1) && (cnt3 == 3) && (cnt4 == 2)), "Test #3: Testing 3.11: Check all count")

   _lwtimer_cancel_period(&period1);
   _lwtimer_cancel_period(&period2);
   cnt1 = cnt2 = cnt3 = cnt4 = 0;
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Testing validity checking
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
   _mqx_uint              result;

   result = _lwtimer_add_timer_to_queue(&period1, &timer1, 1, timer_function,
   (void *)&cnt1);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result != MQX_OK, "Test #4: Testing 4.1: Add lightweight timer1 to the periodic queue")
   if (result == MQX_OK) {
      _task_block();
   } /* Endif */
   result = _lwtimer_create_periodic_queue(&period1, 4, 0);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.2: Create a periodic timer queue")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   period1.VALID = 0;
   result = _lwtimer_add_timer_to_queue(&period1, &timer1, 1, timer_function,
   (void *)&cnt1);
   period1.VALID = LWTIMER_VALID;
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result != MQX_OK, "Test #4: Testing 4.3: Add lightweight timer1 to the periodic queue")
   if (result == MQX_OK) {
      _task_block();
   } /* Endif */
   timer1.VALID=0;
   result=_lwtimer_cancel_timer(&timer1);
   timer1.VALID = LWTIMER_VALID;
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_LWTIMER_INVALID, "Test #4: Testing 4.4: cancel lightweight timer1 from the periodic queue")
   period1.VALID=0;
   result=_lwtimer_cancel_timer(&timer1);
   period1.VALID = LWTIMER_VALID;
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_LWTIMER_INVALID, "Test #4: Testing 4.5: cancel lightweight timer1 from the periodic queue")
   period1.VALID = 0;
   result = _lwtimer_cancel_period(&period1);
   period1.VALID = LWTIMER_VALID;
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result != MQX_OK, "Test #4: Testing 4.6: _lwmtimer_cancel_period operation")
   if (result == MQX_OK) {
      _task_block();
   } /* Endif */
   _lwtimer_cancel_period(&period1);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Testing lwtimer test function
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
   _mqx_uint              result;
   void                  *period_error;
   void                  *timer_error;   

   result = _lwtimer_create_periodic_queue(&period1, 4, 0);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.1: Create a periodic timer queue")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   result = _lwtimer_test(&period_error, &timer_error);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.2: lwtimer test operation")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   result = _lwtimer_add_timer_to_queue(&period1, &timer1, 1, timer_function,
   (void *)&cnt1);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.3: Add lightweight timer1 to the periodic queue")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   result = _lwtimer_test(&period_error, &timer_error);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.4: lwtimer test operation")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   period1.VALID = 0;
   result = _lwtimer_test(&period_error, &timer_error);
   period1.VALID = LWTIMER_VALID;
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, !((result == MQX_OK) || (period_error != (void *)&period1)), "Test #5: Testing 5.5: lwtimer test operation")
   if ((result == MQX_OK) || (period_error != (void *)&period1)) {
      _task_block();
   } /* Endif */
   timer1.VALID = 0;
   result = _lwtimer_test(&period_error, &timer_error);
   timer1.VALID = LWTIMER_VALID;
   EU_ASSERT_REF_TC_MSG(tc_5_main_task,!((result == MQX_OK) || (period_error != (void *)&period1) || (timer_error != (void *)&timer1 )) , "Test #5: Testing 5.6: lwtimer test operation")
   if ((result == MQX_OK) || (period_error != (void *)&period1) ||
         (timer_error != (void *)&timer1 ))
   {
      _task_block();
   } /* Endif */
}

/******************** End Testcases ********************/

/******************** Register tests *******************/
EU_TEST_SUITE_BEGIN(suite_lwtimer)
EU_TEST_CASE_ADD(tc_1_main_task, "- Test # 1 - Testing one timer"),
EU_TEST_CASE_ADD(tc_2_main_task, "- Test # 2 - Testing four timers"),
EU_TEST_CASE_ADD(tc_3_main_task, "- Test # 3 - Testing four timers, 2 periods"),
EU_TEST_CASE_ADD(tc_4_main_task, "- Test # 4 - Testing validity checking"),
EU_TEST_CASE_ADD(tc_5_main_task, "- Test # 5 - Testing lwtimer test function")
EU_TEST_SUITE_END(suite_lwtimer)

EU_TEST_REGISTRY_BEGIN()
EU_TEST_SUITE_ADD(suite_lwtimer, "Test of the Light Weight Timers")
EU_TEST_REGISTRY_END()
/******************** End Register *********************/


/*TASK*--------------------------------------------------------------
*
* Task Name : main_task
* Comments  :
*
*END*--------------------------------------------------------------*/

void main_task
(
uint32_t dummy
)
{ /* Body */
   _int_install_unexpected_isr();

   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();

} /* Endbody */

/* EOF */


