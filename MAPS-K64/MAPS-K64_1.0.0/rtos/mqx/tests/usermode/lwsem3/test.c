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
* $Version : 3.8.3.0$
* $Date    : Sep-26-2011$
*
* Comments:
*
*   CR 1223 - An aborted task was left on a lightweight semaphore
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
#define LWSEM_TASK      11

void super_task(uint32_t parameter);
extern void main_task(uint32_t);
extern void lwsem_task(uint32_t);

//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);// Lightweight sem _abort_task
//------------------------------------------------------------------------

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
{ SUPER_TASK,   super_task,   1000, 10, "Super", MQX_AUTO_START_TASK},
{ MAIN_TASK,     main_task,   2000, 10, "Main", MQX_USER_TASK},
{ LWSEM_TASK,    lwsem_task,  2000,  9, "LWS",  MQX_USER_TASK},
{ 0,             0,           0,     0, 0,      0}
};

USER_RO_ACCESS LWSEM_STRUCT lwsem;
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
   _usr_lwsem_wait(&lwsem);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "lwsem_task didnt block on semaphore")
} /* Endbody */


//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST #1: Lightweight sem _abort_task
//
//END---------------------------------------------------------------------

void tc_1_main_task(void)
{
   _mqx_uint       result;
   _task_id        tid, tid2;

   result = _usr_lwsem_create(&lwsem, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.1: _usr_lwsem_create operation")
   if (result != MQX_OK) {
      return;
   } /* Endif */

   // Nobody should be waiting on the semaphore
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (lwsem.TD_QUEUE.SIZE == 0), "Test #1 Testing: 1.1: lwsem.TD_QUEUE.SIZE should be 0")
   if (lwsem.TD_QUEUE.SIZE != 0) {
      return;
   } /* Endif */

   tid = _usr_task_create(0, LWSEM_TASK, 0);

   // Our lwsem task should be waiting on the semaphore
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (lwsem.TD_QUEUE.SIZE == 1), "Test #1 Testing: 1.2: lwsem.TD_QUEUE.SIZE should be 1")
   if (lwsem.TD_QUEUE.SIZE != 1) {
      return;
   } /* Endif */

   tid2 = _usr_task_create(0, LWSEM_TASK, 0);

   // Our lwsem task should be waiting on the semaphore
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (lwsem.TD_QUEUE.SIZE == 2), "Test #1 Testing: 1.3: lwsem.TD_QUEUE.SIZE should be 2")
   if (lwsem.TD_QUEUE.SIZE != 2) {
      return;
   } /* Endif */

   result = _usr_task_abort(tid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: _task_abort operation")
   if (result != MQX_OK) {
      return;
   } /* Endif */

   // _task_abort should remove our lwsem task from the semaphore
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (lwsem.TD_QUEUE.SIZE == 1), "Test #1 Testing: 1.4: lwsem.TD_QUEUE.SIZE should be 1")
   if (lwsem.TD_QUEUE.SIZE != 1) {
      return;
   } /* Endif */

   result = _usr_task_destroy(tid2);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.5: _task_destroy operation")
   if (result != MQX_OK) {
      return;
   }

   // _task_destroy should remove our lwsem task from the semaphore
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (lwsem.TD_QUEUE.SIZE == 0), "Test #1 Testing: 1.5: lwsem.TD_QUEUE.SIZE should be 0")
   if (lwsem.TD_QUEUE.SIZE != 0) {
      return;
   } /* Endif */
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_usermode)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Lightweight sem _abort_task")
EU_TEST_SUITE_END(suite_usermode)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_usermode, " - Test Lightweight semaphores #3 Abort")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*-------------------------------------------------------------------
* 
* Task Name : main_task
* Comments  : This task test light weight semaphores abort.
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
      uint32_t parameter
   )
{
   _task_id tid;
   _mem_zero(&lwsem, sizeof(lwsem)); //must be in PRIV mode, lwsem must be USER_RO_ACCESS

   _int_install_unexpected_isr();

   tid = _task_create(0, MAIN_TASK, 0);
   if(tid == MQX_NULL_TASK_ID){
     printf("Unable to create test application");
     return;
   }
   _task_block();
}
