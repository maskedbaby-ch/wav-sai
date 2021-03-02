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
* $Version : 3.7.3.0$
* $Date    : Mar-24-2011$
*
* Comments:
*
*   This file tests the Test and Set function.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "util.h"
//#include <test_module.h>

#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test

#define MAIN_TASK    10
#define TESTN_TASK   11

#define NUM_TEST_TASKS  8
#define TEST_LOC_SIZE   4

extern void main_task(uint32_t parameter);
extern void test_task(uint32_t index);
//------------------------------------------------------------------------
// Test suite function
void tc_1_main_task(void);//Test of the function _mem_test_and_set
//------------------------------------------------------------------------
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK,   main_task, 1000, 7, "Main", MQX_AUTO_START_TASK, 0, 0},
   { TESTN_TASK,  test_task, 1000, 8, "Test", MQX_TIME_SLICE_TASK, 0, 2},
   { 0,           0,               0,  0,     0,                   0, 0}
};

unsigned char test_loc[TEST_LOC_SIZE];
uint32_t taskblock = 0;
/*TASK*-------------------------------------------------------------------
* 
* Task Name    : test_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void test_task
   (
      uint32_t index
   )
{ /* Body */
   volatile _mqx_uint i;
   char     c = 'A' + (char)index;

   while (1) {
      if(taskblock)
      {
        _task_block();
      }
      if (_mem_test_and_set(&test_loc[1]) == 0) {
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (test_loc[1] == 0x80), "Test #1 Testing: 1.5: test_loc[1] should be 0x80")
         _task_stop_preemption();
         _task_start_preemption();
         test_loc[1] = 0;
         for (i = 0; i < 0xfff; i++) {
         } /* Endfor */
      } else {
         _sched_yield();
      } /* Endif */

   } /* Endfor */

}/* Endbody */


/******************** Begin Testcases **********************/

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : Test of the function _mem_test_and_set
//
//END---------------------------------------------------------------------
void tc_1_main_task(void)
{
   _mqx_uint result;
   _mqx_uint i;
    
   for ( i = 0; i < TEST_LOC_SIZE; i++) {
      test_loc[i] = 0;
   } /* Endfor */

   for ( i = 0; i < TEST_LOC_SIZE; i++) {
      result = _mem_test_and_set(&test_loc[i]);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == 0), "Test #1 Testing: 1.1: _mem_test_and_set operation should write LOC properly")
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (test_loc[i] == 0x80), "Test #1 Testing: 1.2: test_loc[i] should be 0x80")
   } /* Endfor */

   for ( i = 0; i < TEST_LOC_SIZE; i++) {
      result = _mem_test_and_set(&test_loc[i]);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == 0x80), "Test #1 Testing: 1.3: _mem_test_and_set operation should return 0x80...memory is already set")
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (test_loc[i] == 0x80), "Test #1 Testing: 1.4: test_loc should be 0x80")
   } /* Endfor */

   for ( i = 0; i < TEST_LOC_SIZE; i++) {
      test_loc[i] = 0;
   } /* Endfor */

   for ( i = NUM_TEST_TASKS; i >= 1; i--) {
      _task_create(0,TESTN_TASK, i);
   } /* Endfor */
    _time_delay_ticks(200);
   taskblock = 1;   
}

/******************** End Testcases ********************/

/******************** Register tests *******************/
EU_TEST_SUITE_BEGIN(suite_tstnset)
    EU_TEST_CASE_ADD(tc_1_main_task, "- Test of the function _mem_test_and_set")
EU_TEST_SUITE_END(suite_tstnset)

EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_tstnset, "Test of the function _mem_test_and_set")
EU_TEST_REGISTRY_END()
/******************** End Register *********************/

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
   
}/* Endbody */

/* EOF */
