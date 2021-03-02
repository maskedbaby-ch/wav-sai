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
* $Date    : Jun-22-2012$
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
// #include <log.h>
// #include <klog.h>
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test

#define SUPER_TASK      9
#define MAIN_TASK       10
#define LWEVENT_TASK    11
#define WAIT_TASK       12
#define ISR_TASK        13

#define MAIN_PRIO       10
#define ISR_EVENT_IN    0x100
#define ISR_EVENT_OUT   0x200
#define NUMBER_OF_TASKS 5

void super_task(uint32_t parameter);
void isr_task(uint32_t parameter);
void fault_isr(void *parameter);
extern void       main_task(uint32_t parameter);
extern void       lwevent_task(uint32_t index);
extern void       wait_task(uint32_t index);
extern char   *_mqx_get_task_error_code_string(uint32_t);

void tc_1_main_task(void);// Test #1 - destruction of uninitialized event....
void tc_2_main_task(void);// Test #2 - create lwevent....
void tc_3_main_task(void);// Test #3 - wait on lwevent with IN_ISR == 1...
void tc_4_main_task(void);// TEST #4 - tasks wait on lwevent.....
void tc_5_main_task(void);// Test #5 - attempts to invalidate lwevent ...
void tc_6_main_task(void);// TEST #6 - test of lwevent normal operation....
void tc_7_main_task(void);// Test #7 - testing task priorities....
                         
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { SUPER_TASK,   super_task, 1000,   MAIN_PRIO+1, "super", MQX_AUTO_START_TASK},
   { MAIN_TASK,    main_task,  1000,   MAIN_PRIO,     "main",     MQX_USER_TASK},
   { LWEVENT_TASK, lwevent_task, 1000,   MAIN_PRIO - 1, "lwevent",    MQX_USER_TASK},
   { WAIT_TASK,    wait_task,  1000,   MAIN_PRIO - 2, "wait",     MQX_USER_TASK},
   { ISR_TASK,    isr_task,  1000,   MAIN_PRIO - 1, "ISR",     0},
   { 0,           0,          0,      0,             0,          0}
};

#define MULTI_NOP _ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();

// For Keil these structs must be volatile, or they will be placed in RW area
#if defined(__CC_ARM)
USER_RO_ACCESS volatile LWEVENT_STRUCT lwevent, isr_lwevent;
#else
USER_RO_ACCESS LWEVENT_STRUCT lwevent, isr_lwevent;
#endif

USER_RW_ACCESS volatile _mqx_uint order_of_exec, counter, count[NUMBER_OF_TASKS];
USER_RW_ACCESS volatile _mqx_uint isr_result, error;
bool tc_7_is_testing = 0;

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : lwevent_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void lwevent_task
   (
      uint32_t index
   )
{ /* Body */
    _mqx_uint  result, unblock_value;


   result = _usr_lwevent_wait_ticks(&lwevent,1<<index,TRUE,0);
   if (tc_7_is_testing != 0)
      EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: _usr_lwevent_wait_ticks operation")
   else
      EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "TEST #6 Testing: _usr_lwevent_wait_ticks operation")

    
    unblock_value = _usr_lwevent_get_signalled();
   if (tc_7_is_testing != 0)
      EU_ASSERT_REF_TC_MSG(tc_7_main_task, (unblock_value == (1<<index) ), "Test #7 Testing: _usr_lwevent_get_signalled operation")
   else
      EU_ASSERT_REF_TC_MSG(tc_6_main_task, (unblock_value == (1<<index) ), "TEST #6 Testing: _usr_lwevent_get_signalled operation")
    
   result = (lwevent.VALUE & unblock_value);
   if(index%2){//every even bit should by autocleared
      if(result == 0) result = MQX_OK;
      else result = 1; //ERROR;     
   } else { //manual clear needed
      if(result != 0) result = MQX_OK;
      else result = 1; //ERROR;       
   }
   if (tc_7_is_testing != 0)
      EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: _usr_lwevent_auto_clear operation")
   else
      EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "TEST #6 Testing: _usr_lwevent_auto_clear operation")

   count[index] = order_of_exec++;

   result = _usr_lwevent_set(&lwevent, 1<<(index+1));
   if (tc_7_is_testing != 0)
      EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: _usr_lwevent_set operation")
   else
      EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "TEST #6 Testing: _usr_lwevent_set operation") 

} /* Endbody */


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : wait_task
* Comments     :
*
*END*----------------------------------------------------------------------*/
void wait_task
   (
      uint32_t index
   )
{ /* Body */
   _mqx_uint result;

   while (1) {
   result = _usr_lwevent_wait_ticks(&lwevent,1,TRUE,0);
      if (result == MQX_LWEVENT_INVALID) {
         break;
      } else {
         if (index == 0)
             counter++;
      } /* Endif */
   } /* Endwhile */
   counter++;
} /* Endbody */

/******************** Begin Testcases **********************/

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Destroy uninitialized semaphore, returns MQX_LWEVENT_INVALID, 0x72
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint               result;    

   result = _usr_lwevent_destroy(&lwevent);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.1: Destruction of uninitialized event should have returned MQX_LWEVENT_INVALID")
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

   result = _usr_lwevent_create(&lwevent,0);   
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.1: Create the lwevent return code should be MQX_OK")   
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Wait on lwevent with IN_ISR == 1 ,
*                         expected error, MQX CANNOT CALL FUNCTION FROM ISR, 0x0C
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{ 
   _mqx_uint     result;

   result = _usr_lwevent_set(&isr_lwevent,1);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.1: Starting RRIV task to call fn from ISR")

   result = _usr_lwevent_wait_ticks(&isr_lwevent, 2, TRUE, 0);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.2: Waiting on PRIV task")

   EU_ASSERT_REF_TC_MSG(tc_3_main_task, isr_result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #3: Testing 3.3: lwevent_wait operation should have returned MQX_CANNOT_CALL_FUNCTION_FROM_ISR")
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Create 4 tasks waiting on the semapohre 
*                         then destroy event, tasks should detect destruction.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    _mqx_uint               result;
    _task_id                tid;

    
   counter = 0;
   tid = _usr_task_create(0, WAIT_TASK, 0);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, tid != MQX_NULL_TASK_ID, "TEST #4: Testing 4.1.1: Create instance of wait_task")    

   tid = _usr_task_create(0, WAIT_TASK, 1);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, tid != MQX_NULL_TASK_ID, "TEST #4: Testing 4.1.2: Create instance of wait_task")

   tid = _usr_task_create(0, WAIT_TASK, 2);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, tid != MQX_NULL_TASK_ID, "TEST #4: Testing 4.1.3: Create instance of wait_task")

   tid = _usr_task_create(0, WAIT_TASK, 3);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, tid != MQX_NULL_TASK_ID, "TEST #4: Testing 4.1.4: Create instance of wait_task")

   EU_ASSERT_REF_TC_MSG(tc_4_main_task, lwevent.VALUE == 0, "TEST #4: Testing 4.2: Check lwevent.VALUE should be 0")
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, lwevent.WAITING_TASKS.SIZE == 4, "TEST #4: Testing 4.3: Check lwevent.WAITING_TASKS.SIZE should be 4")

   counter = 0;
   /* Destroy the event */
   result = _usr_lwevent_destroy(&lwevent);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "TEST #4: Testing 4.4: Destroy the lwevent operation")
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, counter == 4, "TEST #4: Testing 4.5: Check all task were readied after sem destroyed")

   result = _usr_lwevent_create(&lwevent,0);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "TEST #4: Testing 4.6: Create the lwevent operation")
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_5_main_task
* Comments     : Test #5: attempts to invalidete lwevent struct
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
   _mqx_uint               valid_value;
   KERNEL_DATA_STRUCT_PTR  kernel_data = _mqx_get_kernel_data();

   error = 0;    
   valid_value = lwevent.VALID;
   lwevent.VALID = 0;
   MULTI_NOP
   lwevent.VALID = valid_value;
   MULTI_NOP
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, error != 0, "Test #5: Testing 5.1: lwevent corruption test ")

   error = 0;
   valid_value = kernel_data->LWEVENTS.SIZE;
   kernel_data->LWEVENTS.SIZE = 99;
   MULTI_NOP
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, error != 0, "Test #5: Testing 5.2: lwevent corruption test ")

   error = 0;
   valid_value = lwevent.WAITING_TASKS.SIZE;
   lwevent.WAITING_TASKS.SIZE = 99;
   MULTI_NOP
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, error != 0, "Test #5: Testing 5.3: lwevent corruption test ")

}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Normal operation
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
   _mqx_uint               i, result;
   _task_id                id[NUMBER_OF_TASKS];

   result = _usr_lwevent_set_auto_clear(&lwevent, 0xAAAA); //every even bit is set to AutoClear
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "TEST #6: Testing 6.1: _usr_lwevent_set_auto_clear operation")

   result = _usr_lwevent_clear(&lwevent, -1);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "TEST #6: Testing 6.1: _usr_lwevent_clear operation")

   order_of_exec = 0;
   for (i = 0; i < NUMBER_OF_TASKS; i++) {
      id[i] = _usr_task_create(0, LWEVENT_TASK, i);
      count[i] = 0;
   } /* Endfor */

   result = _usr_lwevent_set(&lwevent,1);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "TEST #6: Testing 6.2: _usr_lwevent_set operation")

   for (i = 0; i < NUMBER_OF_TASKS; i++) {
      EU_ASSERT_REF_TC_MSG(tc_6_main_task, count[i] == i, "TEST #6: Testing 6.3: Count operation")    
   } /* Endfor */
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_7_main_task
* Comments     : Test #7: different priorities
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
   _mqx_uint               i, result, old_prio;
   _task_id                id[NUMBER_OF_TASKS];

   tc_7_is_testing = 1;
   result = _usr_lwevent_clear(&lwevent, -1);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.1: Lwevent clear operation") 

   order_of_exec = 0;
   for (i = 0; i < NUMBER_OF_TASKS; i++) {
      id[i] = _usr_task_create(0, LWEVENT_TASK, i);
      _task_set_priority(id[i], MAIN_PRIO - i - 1, &old_prio);
      count[i] = 0;
   } /* Endfor */
   result = _usr_lwevent_set(&lwevent,1);
   _time_delay_ticks(10);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.2: lwevent set operation")

   for (i = 0; i < NUMBER_OF_TASKS; i++) {
     EU_ASSERT_REF_TC_MSG(tc_7_main_task, count[i] == i, "Test #7: Testing 7.3: Count operation")
   } /* Endfor */
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_usermode)
   EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - destruction of uninitialized event...."),
   EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - create lwevent...."),
   EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - wait on lwevent with IN_ISR == 1..."),
   EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - tasks wait on lwevent....."),
   EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - attempts to invalidate lwevent ..."),
   EU_TEST_CASE_ADD(tc_6_main_task, " TEST #6 - test of lwevent normal operation...."),
   EU_TEST_CASE_ADD(tc_7_main_task, " Test #7 - testing task priorities....")
EU_TEST_SUITE_END(suite_usermode)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
   EU_TEST_SUITE_ADD(suite_usermode, " - Test of Lightweight events #1")
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
   _task_id tid;
   _mem_zero(&lwevent, sizeof(lwevent));
   _int_install_unexpected_isr();

   _int_install_isr(4, fault_isr, NULL);
   _int_install_isr(5, fault_isr, NULL);    
   SCB_SHCSR |= SCB_SHCSR_MEMFAULTENA_MASK | SCB_SHCSR_BUSFAULTENA_MASK;
  
   tid = _task_create(0, ISR_TASK, 0);
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

void isr_task
   (
      uint32_t parameter
   )
{
   _mqx_uint   in_isr_value, result;
   
   result = _usr_lwevent_create(&isr_lwevent,0);
   result = _usr_lwevent_wait_ticks(&isr_lwevent, 1, TRUE, 0);
   
   _int_disable();
   in_isr_value = get_in_isr();   
   set_in_isr(1);
   isr_result = _usr_lwevent_wait_ticks(&lwevent, 1, TRUE,0);
   set_in_isr(in_isr_value);
   _int_enable();
   
   _lwevent_set(&isr_lwevent, 2);
   _task_block();
}

void fault_isr
(
  /*  [IN]  the parameter passed by the kernel */
  void   *parameter
)
{
   uint32_t *p;
    
   error++;
    
   if (SCB_CFSR & SCB_CFSR_PRECISERR_MASK) {
      p = (uint32_t*)__get_PSP();
      p[6] += 2;      // PC in stack, temporary solution, estimate 16bit instruction
   }
    
   SCB_CFSR |= (SCB_CFSR_PRECISERR_MASK | SCB_CFSR_IMPRECISERR_MASK);
}
