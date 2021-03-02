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
* $Version : 3.8.4.0$
* $Date    : Sep-6-2011$
*
* Comments:
*
*   This file contains the kernel test functions for the watchdog
*   component.
*
*END***********************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <watchdog.h>
//#include <test_module.h>

#include "EUnit.h"
#include "eunit_crit_secs.h"

#include "util.h"

/* #define KLOG_ON */
#define FILE_BASENAME test
#define MAIN_TASK 10

#define TIME_DELAY_TOLERANCE (1000/BSP_ALARM_FREQUENCY)

extern void       main_task(uint32_t);
extern void       work_ticks(_mqx_uint);
extern void       Watchdog_expired(void *);

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
{ MAIN_TASK, main_task, 4000,  8, "Main",  MQX_AUTO_START_TASK,  0},
{ 0,         0,         0,     0, 0,       0,                    0}
};

volatile bool ms_test = FALSE;
volatile bool expired = FALSE;
volatile void   *expired_td_ptr = NULL;

volatile MQX_TICK_STRUCT start_ticks;
volatile MQX_TICK_STRUCT end_ticks;
void tc_1_main_task(void);  
void tc_2_main_task(void);
void tc_3_main_task(void);
void tc_4_main_task(void);
void tc_5_main_task(void);
   
/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: _watchdog_create_component.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{    
     _mqx_uint task_error_code;
     
     task_error_code = _watchdog_create_component(BSP_TIMER_INTERRUPT_VECTOR,
      Watchdog_expired);
     EU_ASSERT_REF_TC_MSG(tc_1_main_task, task_error_code == MQX_OK ,"Test #1: Testing 1.1: Create watchdog component")
         
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_2_main_task
* Comments     : TEST #2: _watchdog_test.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{    
   _mqx_uint task_error_code; 
   void           *watchdog_error_ptr, *watchdog_table_error_ptr;
   
   task_error_code = _watchdog_test(&watchdog_error_ptr, &watchdog_table_error_ptr);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, task_error_code == MQX_OK, "Test #2: Testing 2.1: Test watchdog")
   
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_3_main_task
* Comments     : TEST #3: _watchdog_start_ticks.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{    
    bool         failed = FALSE;
    MQX_TICK_STRUCT ticks;
    _mqx_uint i, result;
    
   _time_init_ticks(&ticks, 4);
   for (i = 0; i < 3; ++i) {
      
      result = _watchdog_start_ticks(&ticks);
      EU_ASSERT_REF_TC_MSG( tc_3_main_task, result, "Test #3 Testing: 3.1: Start the watchdog\n")
      if (!result) {
         failed = TRUE;
      }/* Endif */
      work_ticks(2);
      EU_ASSERT_REF_TC_MSG( tc_3_main_task, !expired, "Test #3 Testing: 3.2: Check watchdog expired")
      if (expired) {
         expired = FALSE;
         failed = TRUE;
      }/* Endif */

      result = _watchdog_start_ticks(&ticks);
      EU_ASSERT_REF_TC_MSG( tc_3_main_task, result, "Test #3 Testing: 3.3: Start the watchdog\n")
      if (!result) {
         failed = TRUE;
      }/* Endif */
      work_ticks(3);
      EU_ASSERT_REF_TC_MSG( tc_3_main_task, !expired, "Test #3 Testing: 3.4: Check watchdog expired")
      if (expired) {
         expired = FALSE;
         failed = TRUE;
      }/* Endif */

      result = _watchdog_start_ticks(&ticks);
      EU_ASSERT_REF_TC_MSG( tc_3_main_task, result, "Test #3 Testing: 3.5: Start the watchdog\n")
      if (!result) {
         failed = TRUE;
      }/* Endif */
      work_ticks(4);
      EU_ASSERT_REF_TC_MSG( tc_3_main_task, expired, "Test #3 Testing: 3.6: Check watchdog expired")
      if (expired) {
         expired = FALSE;
      } else {
         failed = TRUE;
      }/* Endif */

      result = _watchdog_start_ticks(&ticks);
      EU_ASSERT_REF_TC_MSG( tc_3_main_task, result, "Test #3 Testing: 3.7: Start the watchdog\n")
      if (!result) {
         failed = TRUE;
      }/* Endif */
      work_ticks(5);
      EU_ASSERT_REF_TC_MSG( tc_3_main_task, expired, "Test #3 Testing: 3.8: Check watchdog expired")
      if (expired) {
         expired = FALSE;
      } else {
         failed = TRUE;
      }/* Endif */

      result = _watchdog_stop();
      EU_ASSERT_REF_TC_MSG( tc_3_main_task, result, "Test #3 Testing: 3.9: Stop the watchdog\n")
      if (!result) {
         failed = TRUE;
      }/* Endif */
      _time_delay_ticks(4);
      EU_ASSERT_REF_TC_MSG( tc_3_main_task, !expired, "Test #3 Testing: 3.10: Test watchdog expired\n")
      if (expired) {
         expired = FALSE;
         failed = TRUE;
      }/* Endif */

   } /* Endwhile */
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_4_main_task
* Comments     : TEST #4: _watchdog_start.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{    
   bool         failed = FALSE;
   int32_t          ms;
   bool         result, overflow;
   
   /* Test using milliseconds interface */
   ms_test = TRUE;
   _int_disable();
   _time_delay_ticks(1);
   _time_get_elapsed_ticks((MQX_TICK_STRUCT_PTR)&start_ticks);
   result = _watchdog_start(30);
   EU_ASSERT_REF_TC_MSG( tc_4_main_task, result, "Test #4 Testing: 4.1: Start the watchdog\n")
   if (!result) {
      failed = TRUE;
   }/* Endif */

   /* Wait for WDOG to wake us */
   _task_block();
   _time_get_elapsed_ticks((MQX_TICK_STRUCT_PTR)&end_ticks);
   _int_enable();

   ms = _time_diff_milliseconds((MQX_TICK_STRUCT_PTR)&end_ticks, 
      (MQX_TICK_STRUCT_PTR)&start_ticks, &overflow);
   EU_ASSERT_REF_TC_MSG( tc_4_main_task, !overflow, "Test #4 Testing: 4.2: _time_diff should not have overflowed")
   if (overflow) {
      //ktest_error("4.2 _time_diff overflowed.\n");
      failed = TRUE;
   } /* Endif */
   EU_ASSERT_REF_TC_MSG( tc_4_main_task, ( (ms >= 30) && (ms <= (30 + TIME_DELAY_TOLERANCE)) ), "Test #4 Testing: 4.3: Check watchdog fire at correct time")
   if (ms != 30) {
      failed = TRUE;
   }/* Endif */
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_5_main_task
* Comments     : TEST #5: _watchdog_test again.
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{    
   bool         failed = FALSE;
   void           *watchdog_error_ptr, *watchdog_table_error_ptr;
   _mqx_uint       task_error_code;
   
   task_error_code = _watchdog_test(&watchdog_error_ptr, &watchdog_table_error_ptr);
   EU_ASSERT(task_error_code == MQX_OK);
   if (task_error_code != MQX_OK) {
      failed = TRUE;
   }/* Endif */
}
/*FUNCTION*-----------------------------------------------------------------
* 
* Function Name : Watchdog_expired
* Return Value  : void
* Comments      :
*   Print out the watchdog expiry information
*
*END*----------------------------------------------------------------------*/

void Watchdog_expired
   (
      void   *td_ptr
   )
{ /* Body */
   expired = TRUE;
   expired_td_ptr = td_ptr;

   if (ms_test) {
      _task_ready(td_ptr);
   } /* Endif */

} /* Endbody */

 /******  Define Test Suite      *******/   
 
 EU_TEST_SUITE_BEGIN(suite_watchdog1)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1: Testing _watchdog_create_component"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2: Testing _watchdog_test"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3: Testing _watchdog_start_ticks"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4: Testing _watchdog_start"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST # 5: Testing _watchdog_test again ")
 EU_TEST_SUITE_END(suite_watchdog1)

/*******  Add test suites */
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_watchdog1, " Test of Watchdog Component")
 EU_TEST_REGISTRY_END()
 
/*TASK*-------------------------------------------------------------------
* 
* Task Name    : main_task
* Comments     :
*   starts up the watchdog and tests it.
*
*END*----------------------------------------------------------------------*/


void main_task
   (
      uint32_t param
   )
{/* Body */ 
   
   _int_install_unexpected_isr();
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
   
} /* Endbody */

/* EOF */
