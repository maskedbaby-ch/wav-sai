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
* $Date    : Apr-13-2012$
*
* Comments:
*
*   This file tests the light weight event functions.
*
*END************************************************************************/

#include <mqx_inc.h>
//#include <lwsem.h>
//#include <lwevent_prv.h>
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
#define LWSEM_TASK    11
#define WAIT_TASK       12
#define ISR_TASK        13

#define MAIN_PRIO       10

#define NUMBER_OF_TASKS 5

void super_task(uint32_t parameter);
void isr_task(uint32_t parameter);
void fault_isr(void *parameter);
extern void       main_task(uint32_t parameter);
extern void       lwsem_task(uint32_t index);
extern void       wait_task(uint32_t index);
extern char   *_mqx_get_task_error_code_string(uint32_t);

void tc_1_main_task(void);// Test #1 - destruction of uninitialized event....
void tc_2_main_task(void);// Test #2 - create lwsem....
void tc_3_main_task(void);// Test #3 - wait on lwsem with IN_ISR == 1...
void tc_4_main_task(void);// Test #4 - tasks wait on lwsem.....
void tc_5_main_task(void);// Test #5 - lwsem testing error codes...
void tc_6_main_task(void);// Test #6 - test of lwsem normal operation....
void tc_7_main_task(void);// Test #7 - testing task priorities....
                         
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { SUPER_TASK,   super_task, 1000,   MAIN_PRIO, "super", MQX_AUTO_START_TASK},
   { MAIN_TASK,    main_task,  1000,   MAIN_PRIO,     "main",     MQX_USER_TASK},
   { LWSEM_TASK, lwsem_task, 1000,   MAIN_PRIO - 1, "lwsem",    MQX_USER_TASK},
   { WAIT_TASK,    wait_task,  1000,   MAIN_PRIO - 2, "wait",     MQX_USER_TASK},
   { ISR_TASK,    isr_task,  1000,   MAIN_PRIO - 1, "ISR",     0},
   { 0,           0,          0,      0,             0,          0}
};

#define MULTI_NOP _ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();

// For Keil these structs must be volatile, or they will be placed in RW area
#if defined(__CC_ARM)
USER_RO_ACCESS volatile LWSEM_STRUCT lwsem, isr_lwsem;
#else
USER_RO_ACCESS LWSEM_STRUCT lwsem, isr_lwsem;
#endif

USER_RW_ACCESS volatile _mqx_uint order_of_exec, counter, count[NUMBER_OF_TASKS];
USER_RW_ACCESS volatile _mqx_uint isr_result, error;
bool tc_7_is_testing = 0;

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : lwsem_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void lwsem_task
   (
      uint32_t index
   )
{ /* Body */
   _mqx_uint  result;


   result = _usr_lwsem_wait(&lwsem);
   if (tc_7_is_testing != 0)
      EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #9 Testing: _usr_lwsem_wait_ticks operation")
   else
      EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #8 Testing: _usr_lwsem_wait_ticks operation")

   count[index] = order_of_exec++;

   result = _usr_lwsem_post(&lwsem);
   if (tc_7_is_testing != 0)
      EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #9 Testing: _usr_lwsem_set operation")
   else
      EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #8 Testing: _usr_lwsem_set operation") 
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
      result = _usr_lwsem_wait_ticks(&lwsem, 0);
      if (result == MQX_INVALID_LWSEM) {
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
* Comments     : TEST #1: Destroy uninitialized semaphore, returns MQX_INVALID_LWSEM, 0x72
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   _mqx_uint               result;    

   result = _usr_lwsem_destroy(&lwsem);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.1: Destruction of uninitialized event should have returned MQX_INVALID_LWSEM")
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

   result = _usr_lwsem_create(&lwsem, 2);   
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.1: Create the lwsem return code should be MQX_OK")         
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Wait on lwsem with IN_ISR == 1 ,
*                         expected error, MQX CANNOT CALL FUNCTION FROM ISR, 0x0C
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{ 
   _mqx_uint     result;

   result = _usr_lwsem_post(&isr_lwsem);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.1: Posting semaphore to run privilage task")
    
   result = _usr_lwsem_wait_until(&isr_lwsem, NULL);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.1: Waiting for privilage task")
    
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, isr_result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #3: Testing 3.1: lwsem_wait operation should have returned MQX_CANNOT_CALL_FUNCTION_FROM_ISR")    
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
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, tid != MQX_NULL_TASK_ID, "Test #4: Testing 4.1.1: Create instance of wait_task")    

   tid = _usr_task_create(0, WAIT_TASK, 1);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, tid != MQX_NULL_TASK_ID, "Test #4: Testing 4.1.2: Create instance of wait_task")
     
   tid = _usr_task_create(0, WAIT_TASK, 2);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, tid != MQX_NULL_TASK_ID, "Test #4: Testing 4.1.3: Create instance of wait_task")
     
   tid = _usr_task_create(0, WAIT_TASK, 3);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, tid != MQX_NULL_TASK_ID, "Test #4: Testing 4.1.4: Create instance of wait_task")
    
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, lwsem.VALUE == 0, "Test #4: Testing 4.2: Check lwsem.VALUE should be 0")
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, lwsem.TD_QUEUE.SIZE == 4, "Test #4: Testing 4.3: Check lwsem.WAITING_TASKS.SIZE should be 4")
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, lwsem.TD_QUEUE.SIZE == 4, "Test #4: Testing 4.4: task 0 should obtain sem twice")

   counter = 0;
   /* Destroy the event */
   result = _usr_lwsem_destroy(&lwsem);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.4: Destroy the lwsem operation")
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, counter == 4, "Test #4: Testing 4.5: Check all task were readied after sem destroyed")

   result = _usr_lwsem_create(&lwsem,1);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.6: Create the lwsem operation")
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_5_main_task
* Comments     : TEST #5: lwsem test, invalidating lwsem struct by direct write
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
   _mqx_uint               valid_value;
   KERNEL_DATA_STRUCT_PTR  kernel_data = _mqx_get_kernel_data();

   error = 0;    
   valid_value = lwsem.VALID;
   lwsem.VALID = 0;
   MULTI_NOP
   lwsem.VALID = valid_value;
   MULTI_NOP
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, error != 0, "Test #5: Testing 5.1: lwsem corruption test ")

   error = 0;
   valid_value = kernel_data->LWSEM.SIZE;
   kernel_data->LWSEM.SIZE = 99;
   MULTI_NOP
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, error != 0, "Test #5: Testing 5.2: lwsem corruption test ")

   error = 0;
   valid_value = lwsem.TD_QUEUE.SIZE;
   lwsem.TD_QUEUE.SIZE = 99;
   MULTI_NOP
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, error != 0, "Test #5: Testing 5.3: lwsem corruption test ")
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
    
   result = _usr_lwsem_wait(&lwsem);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.1: _usr_lwsem_clear operation")

   order_of_exec = 0;
   for (i = 0; i < NUMBER_OF_TASKS; i++) {
      id[i] = _usr_task_create(0, LWSEM_TASK, i);
      count[i] = 0;
   } /* Endfor */

   result = _usr_lwsem_post(&lwsem);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.2: _usr_lwsem_set operation")

   for (i = 0; i < NUMBER_OF_TASKS; i++) {
      EU_ASSERT_REF_TC_MSG(tc_6_main_task, count[i] == i, "Test #6: Testing 6.3: Count operation")    
   } /* Endfor */
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_7_main_task
* Comments     : TEST #9: different priorities
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
   _mqx_uint               i, result, old_prio;
   _task_id                id[NUMBER_OF_TASKS];

   tc_7_is_testing = 1;
   result = _usr_lwsem_wait(&lwsem);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #9: Testing 9.1: lwsem clear operation") 

   order_of_exec = 0;
   for (i = 0; i < NUMBER_OF_TASKS; i++) {
      id[i] = _usr_task_create(0, LWSEM_TASK, i);
      _task_set_priority(id[i], MAIN_PRIO - i - 1, &old_prio);
      count[i] = 0;
   } /* Endfor */
   result = _usr_lwsem_post(&lwsem);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #9: Testing 9.2: lwsem set operation")

   for (i = 0; i < NUMBER_OF_TASKS; i++) {
      EU_ASSERT_REF_TC_MSG(tc_7_main_task, count[i] == i, "Test #9: Testing 9.3: Count operation")
   } /* Endfor */
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_usermode)
   EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - destruction of uninitialized semaphore...."),
   EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - create lwsem...."),
   EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - wait on lwsem with IN_ISR == 1..."),
   EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - tasks wait on lwsem....."),
   EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - lwsem testing error codes..."),
   EU_TEST_CASE_ADD(tc_6_main_task, " Test #6 - test of lwsem normal operation...."),
   EU_TEST_CASE_ADD(tc_7_main_task, " Test #7 - testing task priorities....")
EU_TEST_SUITE_END(suite_usermode)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_usermode, " - Test of Lightweight semaphore #1")
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
   _mem_zero(&lwsem, sizeof(lwsem)); //must be in PRIV mode, lwsem must be USER_RO_ACCESS
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

   _task_block();
}

void isr_task
   (
      uint32_t parameter
   )
{
   _mqx_uint   in_isr_value, result;
   
   result = _usr_lwsem_create(&isr_lwsem,0);
   result = _usr_lwsem_wait_ticks(&isr_lwsem, 0);
   
   _int_disable();
   in_isr_value = get_in_isr();   
   set_in_isr(1);
   isr_result = _usr_lwsem_wait_for(&lwsem, NULL);
   set_in_isr(in_isr_value);
   _int_enable();

   _lwsem_post(&isr_lwsem);
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
