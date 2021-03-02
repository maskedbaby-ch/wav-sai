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
* $Version : 3.8.5.0$
* $Date    : Apr-13-2012$
*
* Comments:
*
*   This file contains the kernel test functions for the watchdog
*   component.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <watchdog.h>
#include <watchdog_prv.h>
//#include <test_module.h>

#include "EUnit.h"
#include "eunit_crit_secs.h"

#include "util.h"

#define FILE_BASENAME test
#define MAIN_TASK 10
#define WATCHDOG_TIME 10

extern void       main_task(uint32_t);
extern void       work_ticks(_mqx_uint);
extern void       Watchdog_expired(void *);
void tc_1_main_task(void);

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
{ MAIN_TASK, main_task, 5000,  8, "Main",  MQX_AUTO_START_TASK,  0},
{ 0,         0,         0,     0, 0,       0,                    0}
};

volatile bool expired = FALSE;
volatile void   *expired_td_ptr = NULL;

volatile MQX_TICK_STRUCT start_ticks;
volatile MQX_TICK_STRUCT end_ticks;
volatile MQX_TICK_STRUCT diff_ticks;




/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: _watchdog_create_component.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{       
   _mqx_uint                      task_error_code;
   
   task_error_code = _watchdog_create_component(BSP_TIMER_INTERRUPT_VECTOR,
      Watchdog_expired);
   EU_ASSERT_REF_TC_MSG( tc_1_main_task, task_error_code == MQX_OK, "Test #1 Testing: 1.1: Create the watchdog component\n")

}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_2_main_task
* Comments     : TEST #2: _watchdog_start_ticks/_watchdog_stop.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{ 
   KERNEL_DATA_STRUCT_PTR          kernel_data;
   bool                         result; 
   _mqx_uint                       i; 
   WATCHDOG_COMPONENT_STRUCT_PTR   watchdog_component_ptr;
   WATCHDOG_ALARM_TABLE_STRUCT_PTR table_ptr;
   MQX_TICK_STRUCT                 ticks;
   bool                         failed = FALSE;
   _mqx_uint                       elapsed_ticks;
   
   _time_init_ticks(&ticks, WATCHDOG_TIME);
   for (i = 0; i < 50; i++) {
      
      _int_disable();
      _time_get_elapsed_ticks((MQX_TICK_STRUCT_PTR)&start_ticks);
      result = _watchdog_start_ticks(&ticks);
      _int_enable();
      EU_ASSERT_REF_TC_MSG( tc_2_main_task, result, "Test #2 Testing: 2.1: Start the watchdog\n")
      if (!result) {
         failed = TRUE;
      }/* Endif */
      work_ticks(4);
      EU_ASSERT_REF_TC_MSG( tc_2_main_task, !expired, "Test #2 Testing: 2.2: Test the watchdog expired\n")
      if (expired) {
         expired = FALSE;
         failed = TRUE;
      }/* Endif */

      /* Wait for watchdog to expire */
      while(!expired);
      expired = FALSE;
      _int_disable();
      _time_get_elapsed_ticks((MQX_TICK_STRUCT_PTR)&end_ticks);
      result = _watchdog_stop();
      _int_enable();
      EU_ASSERT_REF_TC_MSG( tc_2_main_task, result, "Test #2 Testing: 2.3: Stop the watchdog\n")
      if (!result) {
         failed = TRUE;
      }/* Endif */

      // Begin CR1113
      start_ticks.HW_TICKS = 0; // correct for elapsed time between 1st get & 2nd
      // End CR1113
      _time_diff_ticks((MQX_TICK_STRUCT_PTR)&end_ticks, 
         (MQX_TICK_STRUCT_PTR)&start_ticks, (MQX_TICK_STRUCT_PTR)&diff_ticks);
#if PSP_ENDIAN == MQX_LITTLE_ENDIAN
      elapsed_ticks = diff_ticks.TICKS[0];
#else
      elapsed_ticks = diff_ticks.TICKS[MQX_NUM_TICK_FIELDS-1];
#endif
      EU_ASSERT_REF_TC_MSG( tc_2_main_task, elapsed_ticks == WATCHDOG_TIME, "Test #2 Testing: 2.4: Test the watchdog expired\n")
     } /* Endwhile */

   /* Check to see if multiple watchdog entries have been made */
   _GET_KERNEL_DATA(kernel_data);
   watchdog_component_ptr = (WATCHDOG_COMPONENT_STRUCT_PTR)
      kernel_data->KERNEL_COMPONENTS[KERNEL_WATCHDOG];

   table_ptr = &watchdog_component_ptr->ALARM_ENTRIES;
   result = !(table_ptr->NEXT_TABLE_PTR);
   EU_ASSERT_REF_TC_MSG( tc_2_main_task, result, "Test #2 Testing: 2.5: Extra watchdog table")
   EU_ASSERT_REF_TC_MSG( tc_2_main_task, !(table_ptr->TD_PTRS[1]),"Test #2 Testing: 2.6: Extra watchdog entry")
    
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

} /* Endbody */

 /******   Define Test Suite      *******/  
 
 EU_TEST_SUITE_BEGIN(suite_Watchdog2)
    EU_TEST_CASE_ADD(tc_1_main_task, " Testing _watchdog_create_component"),
    EU_TEST_CASE_ADD(tc_2_main_task, " Testing _watchdog_start_ticks/_watchdog_stop")
 EU_TEST_SUITE_END(suite_Watchdog2)

/******   Add test suites ******/
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_Watchdog2, "Test of Watchdog Component, Part 2")
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
