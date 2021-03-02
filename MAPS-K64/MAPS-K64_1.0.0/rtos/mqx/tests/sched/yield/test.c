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
* $Version : 3.8.3.1$
* $Date    : Aug-10-2012$
*
* Comments:
*
*   This file tests the scheduler yield function.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "util.h"
//#include <test_module.h>

#include "EUnit.h"
#include "eunit_crit_secs.h"

#define MAIN_TASK    10
#define YIELD_TASK   11
#define FILE_BASENAME test

#if PSP_MQX_CPU_IS_KINETIS_L
#define NUM_YIELD_TASKS 4
#else
#define NUM_YIELD_TASKS 8
#endif


extern void main_task(uint32_t parameter);
extern void tc_1_main_task(void);
extern void yield_task(uint32_t index);

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK,   main_task,  1000, 7, "Main",   MQX_AUTO_START_TASK},
   { YIELD_TASK,  yield_task, 1000, 8, "Yield",  0                  },
   { 0,           0,          0,    0, 0,        0                  }
};
/******   Test cases body definition      *******/
/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*
*END*---------------------------------------------------------------------*/
 void tc_1_main_task(void)
 {
      _mqx_uint  i, result;   
      for ( i = NUM_YIELD_TASKS; i >= 1; i--) {
            result = _task_create(0,YIELD_TASK, i);
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_NULL_TASK_ID, "Test #1: Testing 1.1: Check task create operation")
   } /*Endif */
 }
 
/******     Define Test Suite      *******/ 
 EU_TEST_SUITE_BEGIN(suite_yield)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test of _sched_yield ")
 EU_TEST_SUITE_END(suite_yield)
/******      Add test suites      *******/
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_yield, " Test yield suite")
 EU_TEST_REGISTRY_END()

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : yield_task
* Comments     :
*
*END*----------------------------------------------------------------------*/
uint32_t task_block = 0;
void yield_task
   (
      uint32_t index
   )
{/* Body */
   char c = (char)((uint32_t)'A' + index);

   while (TRUE) {
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, !task_block, "Test #1: Testing 1.2: Check task block")
      if(task_block)
         _task_block();
      _sched_yield();
      
   } /* Endwhile */

}/* Endbody */

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : main_task
* Comments     :
*
*END*----------------------------------------------------------------------*/
void main_task
   (
      uint32_t parameter
   )
{/* Body */
   _int_install_unexpected_isr();  

   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();

   _time_delay_ticks(200);
   task_block = 1;
} /* Endbody */

/* EOF */
