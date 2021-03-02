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
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file tests the light weight semaphore functions.
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
#define PRIV_TASK       13

#define MAIN_PRIO       10

#define PRIV_EVENT_IN   0x100
#define PRIV_EVENT_OUT  0x200
#define NUMBER_OF_TASKS 5

void super_task(uint32_t parameter);
void priv_task(uint32_t parameter);
void main_task(uint32_t parameter);

void tc_1_main_task(void);// Test #1 - destruction of uninitialized semaphore....
void tc_2_main_task(void);// Test #2 - create lwsem....

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { SUPER_TASK,   super_task, 1000,   MAIN_PRIO+1, "super", MQX_AUTO_START_TASK},
   { MAIN_TASK,    main_task,  1000,   MAIN_PRIO,     "main",     MQX_USER_TASK},
   { PRIV_TASK,    priv_task,  1000,   MAIN_PRIO, "privilege",     0},
   { 0,           0,          0,      0,             0,          0}
};

#define MULTI_NOP _ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();

USER_RO_ACCESS LWSEM_STRUCT usr_lwsem, usr_lwsem2, priv_lwsem;
USER_NO_ACCESS LWSEM_STRUCT priv_lwsem_no;
USER_RW_ACCESS LWSEM_STRUCT priv_lwsem_rw;
USER_RW_ACCESS volatile _mqx_uint order_of_exec, counter, count[NUMBER_OF_TASKS];
USER_RW_ACCESS volatile _mqx_uint priv_result, error;
bool tc_9_is_testing = 0;

/******************** Begin Testcases **********************/

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Attempts to use "no acces" or privilege lwsems, should return MQX_LWSEM_INVALID
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   _mqx_uint               result;    
   MQX_TICK_STRUCT         ticks;

   result = _usr_lwsem_create(&priv_lwsem_no,0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.1: Creation of 'no access' semaphore ")

   result = _lwsem_create(&priv_lwsem_no,0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.2: Creation of 'no access' semaphore from user task using privilege API")

   result = _usr_lwsem_create(&priv_lwsem_rw,0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.1: Creation of 'no access' semaphore ")

   result = _lwsem_create(&priv_lwsem_rw,0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.2: Creation of 'no access' semaphore from user task using privilege API")

   result = _usr_lwsem_post(&priv_lwsem);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.3: Posting of privilege semaphore from user task")

   result = _lwsem_post(&priv_lwsem);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.4: Posting of privilege semaphore from user task using privilege API")

   result = _usr_lwsem_poll(&priv_lwsem);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.5: Polling on privilege semaphore from user task")

   result = _lwsem_poll(&priv_lwsem);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.6: Polling on privilege semaphore from user task using privilege API")

   result = _usr_lwsem_wait_ticks(&priv_lwsem, 2);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.7: Waiting (ticks) on privilege semaphore from user task")

   result = _lwsem_wait_ticks(&priv_lwsem, 2);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.8: Waiting (ticks) on privilege semaphore from user task using privilege API")

   ticks = _mqx_zero_tick_struct;
   _time_add_msec_to_ticks(&ticks, 100);
   result = _usr_lwsem_wait_for(&priv_lwsem, &ticks);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.9: Waiting (for) on privilege semaphore from user task")

   result = _lwsem_wait_for(&priv_lwsem, &ticks);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.10: Waiting (for) on privilege semaphore from user task using privilege API")

   _time_get_elapsed_ticks(&ticks);
   _time_add_msec_to_ticks(&ticks, 100);    
   result = _usr_lwsem_wait_until(&priv_lwsem, &ticks);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.11: Waiting (until) on  privilege semaphore from user task")

   result = _lwsem_wait_until(&priv_lwsem, &ticks);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.12: Waiting (until) on  privilege semaphore from user task using privilege API")

   result = _usr_lwsem_wait(&priv_lwsem);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.13: Waiting on  privilege semaphore from user task")

   result = _lwsem_wait(&priv_lwsem);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.14: Waiting on privilege semaphore from user task using privilege API")

   result = _usr_lwsem_destroy(&priv_lwsem);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.15: Destruction of privilege semaphore from user task")

   result = _lwsem_destroy(&priv_lwsem);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.16: Destruction of privilege semaphore from user task using privilege API")
}
/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Create the lwsem, return code should be MQX_OK, 0.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _mqx_uint               result;
    
   result = _usr_lwsem_create(&usr_lwsem2,0);   
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.1: Create the lwsem return code should be MQX_OK")   

   result = _usr_lwsem_post(&usr_lwsem);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.2: Setting the lwsem -> run privilege task return code should be MQX_OK")   

   result = _usr_lwsem_wait_for(&usr_lwsem2, NULL);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.3: Waiting from priv task to set user semaphore using user API; return code should be MQX_OK")   
    
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, priv_result == MQX_OK, "Test #2: Testing 2.4: Priv task aswered properly; return code should be MQX_OK")   
   priv_result = -1;
   result = _usr_lwsem_post(&usr_lwsem);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.5: Setting the lwsem -> run privilege task; return code should be MQX_OK")   

   result = _usr_lwsem_wait_until(&usr_lwsem2, NULL);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.6: Waiting from priv task to set user semaphore using priv API; return code should be MQX_OK")   
    
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, priv_result == MQX_OK, "Test #2: Testing 2.7: Priv task aswered properly; return code should be MQX_OK")   
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_usermode)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - attempts to reach 'no access' or privilege lwsems...."),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - synchronizig with privilege task...."),
EU_TEST_SUITE_END(suite_usermode)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_usermode, " - Test of Lightweight semaphore #4")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------


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
{ /* Body */
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* Endbody */
/* EOF */

void super_task
   (
      uint32_t parameter
   )
{
   _task_id    tid;
   _mqx_uint   result;
   
   _int_install_unexpected_isr();
   
   _mem_zero(&usr_lwsem, sizeof(usr_lwsem));
   _mem_zero(&usr_lwsem2, sizeof(usr_lwsem2));
   _mem_zero(&priv_lwsem, sizeof(priv_lwsem));
   _mem_zero(&priv_lwsem_no, sizeof(priv_lwsem_no));

   result = _lwsem_create(&priv_lwsem,0);
   if(result != MQX_OK){
      printf("ERROR: Cannot create privilege lwsem -> app terminated!");
      return;
   }
   result = _usr_lwsem_create(&usr_lwsem,0);   
   if(result != MQX_OK){
      printf("ERROR: Cannot create user lwsem -> app terminated!");
      return;
   }
   tid = _task_create(0, PRIV_TASK, 0);
   if(tid == MQX_NULL_TASK_ID){
     printf("Unable to create test application");
     return;
   }
   
   tid = _task_create(0, MAIN_TASK, 0);
   if(tid == MQX_NULL_TASK_ID){
     printf("Unable to create test application");
     return;
   }
}

void priv_task
   (
      uint32_t parameter
   )
{
   _mqx_uint   result;
   
   result = _usr_lwsem_wait(&usr_lwsem);
   if(result != MQX_OK){
      priv_result = -1;
      _task_destroy(0);
   }
   priv_result = MQX_OK;
   result = _usr_lwsem_post(&usr_lwsem2);
   
   result = _lwsem_wait(&usr_lwsem);
   if(result != MQX_OK){
      priv_result = -1;
      _task_destroy(0);
   }
   priv_result = MQX_OK;
   result = _lwsem_post(&usr_lwsem2);
}
