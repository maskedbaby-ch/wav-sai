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
* $Version : 3.8.2.0$
* $Date    : Sep-9-2011$
*
* Comments:
*
*   This file contains the source functions for the demo of the
*   time slicing functions of the kernel.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <util.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test

#define SUPER_TASK      9
#define MAIN_TASK       10
#define LWSEM_TASK       11

extern void main_task(uint32_t);
extern void lwsem_task(uint32_t);
extern void super_task(uint32_t);

//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);// Test #1 - Testing _usr_lwsem_create
void tc_2_main_task(void);// Test #2 - Testing _usr_lwsem_wait_ticks
void tc_3_main_task(void);// Test #3 - Testing _usr_lwsem_wait_for
void tc_4_main_task(void);// Test #4 - Testing _usr_lwsem_wait_until
void tc_5_main_task(void);// Test #5 - Testing waiting in task
//------------------------------------------------------------------------

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
{ SUPER_TASK,      super_task,      2000,  7, "Super",      MQX_AUTO_START_TASK},
{ MAIN_TASK,       main_task,       2000,  7, "Main",       MQX_USER_TASK},
{ LWSEM_TASK,      lwsem_task,      2000,  6, "LWS",        MQX_USER_TASK},
{ 0,               0,               0,     0, 0,            0}
};

USER_RO_ACCESS LWSEM_STRUCT lwsem;

volatile _mqx_uint lwsem_count = 0;
volatile _mqx_uint lwsem_timeout = 0;
USER_RW_ACCESS volatile _mqx_uint error;
/*TASK*-------------------------------------------------------------------
* 
* Task Name : lwsem_task
* Comments  : This task waits on a lwsem.
*
*END*----------------------------------------------------------------------*/

void lwsem_task
   (
      uint32_t dummy
   )
{ /* Body */
   _mqx_uint result;

   while (TRUE) {
      result = _usr_lwsem_wait_ticks(&lwsem, 5);
      EU_ASSERT_REF_TC_MSG(tc_5_main_task, ((result == MQX_OK) || (result == MQX_LWSEM_WAIT_TIMEOUT) || (result == MQX_INVALID_LWSEM)), "Test #5 Testing: _usr_lwsem_wait_ticks should be either MQX_OK or MQX_LWSEM_WAIT_TIMEOUT or MQX_INVALID_LWSEM")
      if (result == MQX_OK) {
         lwsem_count++;
      } else if (result == MQX_LWSEM_WAIT_TIMEOUT) {
         lwsem_timeout++;
      } else if (result == MQX_INVALID_LWSEM) { 
         _task_destroy(0);
      } else {
      } /* Endif */
    } /* Endwhile */

} /* Endbody */

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST #1: Testing _usr_lwsem_create
//
//END---------------------------------------------------------------------

void tc_1_main_task(void)
{
   _mqx_uint       result;

   result = _usr_lwsem_create(&lwsem, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: _usr_lwsem_create operation")
   _usr_task_set_error(MQX_OK);
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST #2: Testing _usr_lwsem_wait_ticks
//
//END---------------------------------------------------------------------

void tc_2_main_task(void)
{
   MQX_TICK_STRUCT start_ticks;
   MQX_TICK_STRUCT end_ticks;
   MQX_TICK_STRUCT diff_ticks;
   _mqx_uint       result, delay;

   delay = 5;
   _time_delay_ticks(1);
   _time_get_elapsed_ticks(&start_ticks);
   result = _usr_lwsem_wait_ticks(&lwsem, delay);
   _time_get_elapsed_ticks(&end_ticks);

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_LWSEM_WAIT_TIMEOUT), "Test #2 Testing: 2.1: _time_get_elapsed_ticks should have returned MQX_LWSEM_WAIT_TIMEOUT")
   _usr_task_set_error(MQX_OK);

   _time_diff_ticks(&end_ticks, &start_ticks, &diff_ticks);

   result = PSP_GET_ELEMENT_FROM_TICK_STRUCT(&diff_ticks,0);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == delay), "Test #2 Testing: 2.2: Lwsem wait ticks time is correct")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST #3: Testing _usr_lwsem_wait_for
//
//END---------------------------------------------------------------------

void tc_3_main_task(void)
{
   MQX_TICK_STRUCT start_ticks;
   MQX_TICK_STRUCT end_ticks;
   MQX_TICK_STRUCT diff_ticks;
   MQX_TICK_STRUCT ticks;
   _mqx_uint       result, delay;

   ticks = _mqx_zero_tick_struct;
   delay = 20;
   _time_add_msec_to_ticks(&ticks, delay);
   _time_delay_ticks(1);
   _time_get_elapsed_ticks(&start_ticks);
   result = _usr_lwsem_wait_for(&lwsem, &ticks);
   _time_get_elapsed_ticks(&end_ticks);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_LWSEM_WAIT_TIMEOUT), "Test #3 Testing: 3.1: _usr_lwsem_wait_for operation")
   _usr_task_set_error(MQX_OK);

   _time_diff_ticks(&end_ticks, &start_ticks, &diff_ticks);
   result = (unsigned)PSP_CMP_TICKS(&diff_ticks, &ticks);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == 0), "Test #3 Testing: 3.2: Lwsem wait for is correct")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST #4: Testing _usr_lwsem_wait_until
//
//END---------------------------------------------------------------------

void tc_4_main_task(void)
{
   MQX_TICK_STRUCT start_ticks;
   MQX_TICK_STRUCT end_ticks;
   _mqx_uint       result, delay;

   delay = 20;
   _time_delay_ticks(1);
   _time_get_elapsed_ticks(&start_ticks);
   _time_add_msec_to_ticks(&start_ticks, delay);
   result = _usr_lwsem_wait_until(&lwsem, &start_ticks);
   _time_get_elapsed_ticks(&end_ticks);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_LWSEM_WAIT_TIMEOUT), "Test #4 Testing: 4.1: _usr_lwsem_wait_until operation")
   _usr_task_set_error(MQX_OK);

   result = PSP_CMP_TICKS(&start_ticks, &end_ticks);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == 0), "Test #4 Testing: 4.2: _usr_lwsem_wait_until time is correct")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_5_main_task
// Comments     : TEST #5: Testing waiting in task
//
//END---------------------------------------------------------------------

void tc_5_main_task(void)
{
   _task_id        tid;
   void           *td;

   _time_delay_ticks(1);
   tid = _task_create(0, LWSEM_TASK, 0);
   _usr_lwsem_post(&lwsem);
   _usr_lwsem_post(&lwsem);

   _time_delay_ticks(14);

   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (lwsem_count == 2), "Test #5 Testing: 5.1: Lwsem post is correct")
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (lwsem_timeout != 0), "Test #5 Testing: 5.2: Lwsem wait timeout is correct")

   _usr_lwsem_destroy(&lwsem);

   td = _task_get_td(tid);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (td == NULL), "Test #5 Testing: 5.3: ")
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_usermode)
   EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Testing _usr_lwsem_create"),
   EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Testing _usr_lwsem_wait_ticks"),
   EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Testing _usr_lwsem_wait_for"),
   EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - Testing _usr_lwsem_wait_until"),
   EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - Testing waiting in task")
EU_TEST_SUITE_END(suite_usermode)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_usermode, " - Test of Lightweight semaphore #2 Time Specific Tests")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------


/*TASK*-------------------------------------------------------------------
* 
* Task Name : main_task
* Comments  : This task test the light weight semaphores.
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t dummy
   )
{ /* Body */
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* Endbody */

void super_task
   (
      uint32_t dummy
   )
{
   // marks@5/20/03 Mask skew in tick differences that are caused by partial ticks
   /* Start CR 1148 */
   _time_set_hwtick_function(0, (void *)NULL);
   /* End   CR 1148 */

   _int_install_unexpected_isr();
   _usr_task_create(0, MAIN_TASK, 0);
}

/* EOF */
