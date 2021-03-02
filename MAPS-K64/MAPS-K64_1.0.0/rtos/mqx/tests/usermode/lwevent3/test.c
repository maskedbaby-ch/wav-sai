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
* $Date    : Apr-13-2012$
*
* Comments:
*
*   This file tests the light weight event functions.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <lwevent.h>
#include <lwevent_prv.h>
#include <bsp.h>
#include <util.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test

#define SUPER_TASK      9
#define MAIN_TASK       10
#define PRIV_TASK       11

#define MAIN_PRIO       10

#define PRIV_EVENT_IN    0x100
#define PRIV_EVENT_OUT   0x200

void super_task(uint32_t parameter);
void priv_task(uint32_t parameter);
void main_task(uint32_t parameter);

void tc_1_main_task(void);// Test #1 - destruction of uninitialized event....
void tc_2_main_task(void);// Test #2 - create lwevent....


TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { SUPER_TASK,   super_task, 1000,   MAIN_PRIO, "super", MQX_AUTO_START_TASK},
   { MAIN_TASK,    main_task,  1000,   MAIN_PRIO,     "main",     MQX_USER_TASK},
   { PRIV_TASK,    priv_task,  1000,   MAIN_PRIO, "ISR",     0},
   { 0,           0,          0,      0,             0,          0}
};

USER_RO_ACCESS LWEVENT_STRUCT usr_lwevent, usr_lwevent2, priv_lwevent;
USER_NO_ACCESS LWEVENT_STRUCT priv_lwevent_no;
USER_RW_ACCESS LWEVENT_STRUCT priv_lwevent_rw;

USER_RW_ACCESS volatile _mqx_uint priv_result, error;
/******************** Begin Testcases **********************/

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Attempts to use "no acces" or privilege lwevents, should return MQX_LWEVENT_INVALID
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   _mqx_uint               result;    
   MQX_TICK_STRUCT         ticks, start_ticks;

   result = _usr_lwevent_create(&priv_lwevent_no,0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.1: Creation of 'no access' event ")

   result = _lwevent_create(&priv_lwevent_no,0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.2: Creation of 'no access' event from user task using privilege API")

   result = _usr_lwevent_create(&priv_lwevent_rw,0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.1: Creation of 'rw access' event ")

   result = _lwevent_create(&priv_lwevent_rw,0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.2: Creation of 'rw access' event from user task using privilege API")

   result = _usr_lwevent_set(&priv_lwevent, 1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.5: Set privilege event from user task")

   result = _lwevent_set(&priv_lwevent, 1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.6: Set privilege event from user task using privilege API")
    
   result = _usr_lwevent_clear(&priv_lwevent, -1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.3: Clear of privilege event from user task")

   result = _lwevent_clear(&priv_lwevent, -1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.4: Clear of privilege event from user task using privilege API")

   result = _usr_lwevent_set_auto_clear(&priv_lwevent, -1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.7: Destruction of privilege event from user task")

   result = _lwevent_set_auto_clear(&priv_lwevent, -1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.8: Destruction of privilege event from user task using privilege API")
    
   result = _usr_lwevent_wait_ticks(&priv_lwevent, 1, TRUE, 2);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.9: Destruction of privilege event from user task")

   result = _lwevent_wait_ticks(&priv_lwevent, 1, TRUE, 2);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.10: Destruction of privilege event from user task using privilege API")

   ticks = _mqx_zero_tick_struct;
   _time_add_msec_to_ticks(&ticks, 100);
   result = _usr_lwevent_wait_for(&priv_lwevent, 1, TRUE, &ticks);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.11: Destruction of privilege event from user task")

   result = _lwevent_wait_for(&priv_lwevent, 1, TRUE, &ticks);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.12: Destruction of privilege event from user task using privilege API")
        
   _time_get_elapsed_ticks(&start_ticks);
   _time_add_msec_to_ticks(&start_ticks, 100);
   result = _usr_lwevent_wait_until(&priv_lwevent, 1, TRUE, &ticks);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.13: Destruction of privilege event from user task")

   _time_get_elapsed_ticks(&start_ticks);
   _time_add_msec_to_ticks(&start_ticks, 100);
   result = _lwevent_wait_until(&priv_lwevent, 1, TRUE, &ticks);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.14: Destruction of privilege event from user task using privilege API")

   result = _usr_lwevent_destroy(&priv_lwevent);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.15: Destruction of privilege event from user task")

   result = _lwevent_destroy(&priv_lwevent);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.16: Destruction of privilege event from user task using privilege API")

}
/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Create the lwevent, return code should be MQX_OK, 0.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
   _mqx_uint               result;

   result = _usr_lwevent_create(&usr_lwevent2,0);   
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.1: Create the lwevent return code should be MQX_OK")   

   result = _usr_lwevent_set(&usr_lwevent, PRIV_EVENT_IN);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.2: Setting the lwevent -> run privilege task return code should be MQX_OK")   

   result = _usr_lwevent_wait_for(&usr_lwevent, PRIV_EVENT_OUT, TRUE, NULL);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.3: Waiting from priv task to set user event using user API; return code should be MQX_OK")   

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, priv_result == MQX_OK, "Test #2: Testing 2.4: Priv task aswered properly; return code should be MQX_OK")   

   priv_result = -1;
   result = _usr_lwevent_set(&usr_lwevent2, PRIV_EVENT_IN);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.2: Setting the lwevent -> run privilege task; return code should be MQX_OK")   

   result = _usr_lwevent_wait_until(&usr_lwevent2, PRIV_EVENT_OUT, TRUE, NULL);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.3: Waiting from priv task to set user event using priv API; return code should be MQX_OK")   

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, priv_result == MQX_OK, "Test #2: Testing 2.4: Priv task aswered properly; return code should be MQX_OK")   
}



//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_usermode)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - attempts to reach 'no access' or privilege lwevents...."),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - synchronizig with privilege task...."),
EU_TEST_SUITE_END(suite_usermode)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_usermode, " - Test of Lightweight events #3")
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
   
   _mem_zero(&usr_lwevent, sizeof(usr_lwevent));
   _mem_zero(&usr_lwevent2, sizeof(usr_lwevent2));
   _mem_zero(&priv_lwevent, sizeof(priv_lwevent));
   _mem_zero(&priv_lwevent_no, sizeof(priv_lwevent_no));
  
   result = _lwevent_create(&priv_lwevent,0);
   if(result != MQX_OK){
      printf("ERROR: Cannot create privilege lwevent -> app terminated!");
      return;
   }
   result = _usr_lwevent_create(&usr_lwevent,0);   
   if(result != MQX_OK){
      printf("ERROR: Cannot create user lwevent -> app terminated!");
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
   
   result = _usr_lwevent_wait_for(&usr_lwevent, PRIV_EVENT_IN, TRUE, NULL);
   if(result != MQX_OK){
      priv_result = -1;
      _task_destroy(0);
   }
   priv_result = MQX_OK;
   result = _usr_lwevent_set(&usr_lwevent, PRIV_EVENT_OUT);
   
   result = _lwevent_wait_for(&usr_lwevent2, PRIV_EVENT_IN, TRUE, NULL);
   if(result != MQX_OK){
      priv_result = -1;
      _task_destroy(0);
   }
   priv_result = MQX_OK;
   result = _lwevent_set(&usr_lwevent2, PRIV_EVENT_OUT);
}
