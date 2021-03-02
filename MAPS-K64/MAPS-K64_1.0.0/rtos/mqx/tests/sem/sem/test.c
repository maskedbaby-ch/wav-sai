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
* $Version : 3.8.10.1$
* $Date    : Aug-29-2012$
*
* Comments:
*
*   This file contains the MQX sempahore test.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <name.h>
#include <name_prv.h>
#include <sem.h>
#include <sem_prv.h>
#include "test.h"
#include "util.h"
//#include <test_module.h>

#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME test

#define _PRINTING_ALLOWED_

#if PSP_MQX_CPU_IS_KINETIS_L
#define STACK_SIZE  1000
#else
#define STACK_SIZE  2000
#endif
//------------------------------------------------------------------------
// Test suite function prototipe
extern void tc_1_main_task(void);//TEST # 1 - Testing _sem_create_component function
extern void tc_2_main_task(void);//TEST # 2 - Testing _sem_create function
extern void tc_3_main_task(void);//TEST # 3 - Testing _sem_create_fast function
extern void tc_4_main_task(void);//TEST # 4 - Testing _sem_open function
extern void tc_5_main_task(void);//TEST # 5 - Testing _sem_open_fast function
extern void tc_6_main_task(void);//TEST # 6 - Testing _sem_close function
extern void tc_7_main_task(void);//TEST # 7 - Testing _sem_destroy_fast function
extern void tc_8_main_task(void);//TEST # 8 - Testing _sem_destroy function, expect W1.3 to be printed
extern void tc_9_main_task(void);//TEST # 9 - Testing _sem_get_value function
extern void tc_10_main_task(void);//TEST # 10 - Testing _sem_get_wait_count function
extern void tc_11_main_task(void);//TEST # 11 - Testing _sem_test function
extern void tc_12_main_task(void);//TEST # 12 - Testing _sem_wait _sem_wait_ticks _sem_wait_for _sem_wait_until and _sem_post functions expect one W1.3 msg to be printed
extern void tc_13_main_task(void);//TEST # 13 - Mixing semaphores by name and index
//------------------------------------------------------------------------
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
{ MAIN_TASK,            main_task,              3000,        8, "Main",      MQX_AUTO_START_TASK},
{ TEST_DESTROY_SEM_TASK,test_destroy_sem_task,  STACK_SIZE,  6, "Test",      0                 },
{ PRIO_TEST_TASK1,      prio_test_task,         STACK_SIZE,  6, "Prio1",     0                 },
{ PRIO_TEST_TASK2,      prio_test_task,         STACK_SIZE,  5, "Prio2",     0                 },
{ WAIT_TIME_TASK,       wait_time_task,         STACK_SIZE,  7, "Wait",      0                 },
{ 0,                    0,                      0,           0, 0,           0                 },
};

_task_id main_id = 0;
char     sem_name[128] = "";

void         *sem_ptr;
void         *sem2_ptr;
void         *sem_error_ptr;
void           *mem;
_mqx_uint       index;
TD_STRUCT_PTR           td_ptr;
DESTROY_SETTINGS_STRUCT settings;
MEMORY_ALLOC_INFO_STRUCT   memory_alloc_info;
SEM_COMPONENT_STRUCT_PTR   sem_component_ptr;
uint16_t test_index=0;
uint16_t   isr_value;
_mqx_uint sem_valid;
_mqx_uint valid_value;
_mqx_uint handle_field;
bool   delayed_destroy_value;
_mqx_uint       w13_count = 0;

/******   Test cases body definition      *******/

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST # 1 - Testing _sem_create_component function 
//
//END---------------------------------------------------------------------
 void tc_1_main_task(void)
 {
    _mqx_uint       result;

    /* check for MQX memory error
    ** Return code - MQX_OUT_OF_MEMORY = 0X4
    */
    mem = _mem_alloc(sizeof(SEM_COMPONENT_STRUCT));
    memory_alloc_all(&memory_alloc_info);
    result = _sem_create_component( 2, 1,4 );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task,result == MQX_OUT_OF_MEMORY,"Test #1 Testing: 1.0:Check for MQX memory error")

    _task_set_error(MQX_OK);

    memory_free_all(&memory_alloc_info);
    //printf("Highest memory used: 0x%P \n", _mem_get_highwater());

    /* test the normal creation
    ** Return code - MQX_OK = 0
    */
    result = _sem_create_component( 2, 1,4 );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task,result == MQX_OK,"Test #1 Testing: 1.2:Test the normal creation")

    _task_set_error(MQX_OK);

    /* Try again result should be OK */
    result = _sem_create_component( 2, 1,4 );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task,result == MQX_OK,"Test #1 Testing: 1.3: Try again _sem_create_component operation")

    _task_set_error(MQX_OK);
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST # 2 - Testing _sem_create function
//
//END---------------------------------------------------------------------
 void tc_2_main_task(void)
 {
    _mqx_uint       result;

   /* check for MQX memory error
   ** Return code - MQX_OUT_OF_MEMORY = 0X4
   */
   memory_alloc_all(&memory_alloc_info);
   result =  _sem_create( sem_name, 1, 0 );
   memory_free_all(&memory_alloc_info);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == MQX_OUT_OF_MEMORY,"Test #2 Testing: 2.0: Check for MQX memory error, return value should 0X4")

   _task_set_error(MQX_OK);

   /* test MQX_CANOT_CALL_FUNCTION_FROM_ISR
   ** Return code - MQX_CANNOT_CALL_FUNCTION_FROM_ISR = 0X0C
   */
   _int_disable();
   isr_value = get_in_isr();
   set_in_isr(1);
   result =  _sem_create( sem_name, 1, 0 );
   set_in_isr(isr_value);
   _int_enable();
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR,"Test #2 Testing: 2.1:Test MQX_CANOT_CALL_FUNCTION_FROM_ISR")

   _task_set_error(MQX_OK);

   /* test for the  MQX component exist
   ** Return code - MQX_OUT_OF_MEMORY = 0x04
   */
   sem_component_ptr = get_sem_component_ptr();
   set_sem_component_ptr(NULL);
   
   /* by default, if the component doesn't exist
      a default component will be created. So we need to cause 
      the component to fail by creating a memory error */
   memory_alloc_all(&memory_alloc_info);
   result =  _sem_create( sem_name, 1, 0 );
   memory_free_all(&memory_alloc_info);
   set_sem_component_ptr(sem_component_ptr);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == MQX_OUT_OF_MEMORY,"Test #2 Testing: 2.2:Test for the  MQX component exist")

   _task_set_error(MQX_OK);

   /* test for the invalid component base
   ** Return code - MQX_INVALID_COMPONENT_BASE = 0x63
   */
   sem_valid = get_sem_component_valid_field();
   set_sem_component_valid_field(0);
   result =  _sem_create( sem_name, 1, 0 );
   set_sem_component_valid_field(sem_valid);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == MQX_INVALID_COMPONENT_BASE,"Test #2 Testing: 2.3:Test for the invalid component base")

   _task_set_error(MQX_OK);

   /* test the normal creation
   ** Return code - MQX_OK = 0
   */
   result =  _sem_create( sem_name, 1, 0 );
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == MQX_OK,"Test #2 Testing: 2.4:Test the normal creation")

   _task_set_error(MQX_OK);

   /* test for sem exists error
   ** Return code - SEM_SEMAPHORE_EXISTS = 0x1008
   */
   result =  _sem_create( sem_name, 1, 0 );
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == SEM_SEMAPHORE_EXISTS,"Test #2 Testing: 2.5:Create sem with name sem.one")

   _task_set_error(MQX_OK);

   /* test for sem table full error
   ** Return code - SEM_SEMAPHORE_TABLE_FULL = 0x1009   
   */
   result =  _sem_create( "sem.two", 1, 0 );
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == MQX_OK,"Test #2 Testing: 2.6:Create sem with name sem.two")

   _task_set_error(MQX_OK);

   result =  _sem_create( "sem.three", 1, 0 );
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == MQX_OK,"Test #2 Testing: 2.7: Create sem with name sem.three ")

   _task_set_error(MQX_OK);

   result =  _sem_create( "sem.four", 1, 0 );
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == MQX_OK,"Test #2 Testing: 2.8:Create sem with name sem.four")

   _task_set_error(MQX_OK);
  
   result =  _sem_create( "sem.five", 1, 0 );
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == SEM_SEMAPHORE_TABLE_FULL,"Test #2 Testing: 2.9:Create sem with name sem.five")

   _task_set_error(MQX_OK);

   /* destroy all the sems that were just created */
   result =  _sem_destroy( sem_name, TRUE );
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == MQX_OK,"Test #2 Testing: 2.10:Destroy sem.one")

   _task_set_error(MQX_OK);

   result =  _sem_destroy( "sem.two", TRUE );
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == MQX_OK,"Test #2 Testing: 2.11: Destroy sem.two")

   _task_set_error(MQX_OK);

   result =  _sem_destroy( "sem.three", TRUE );
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == MQX_OK,"Test #2 Testing: 2.12:Destroy sem.three")

   _task_set_error(MQX_OK);

   result =  _sem_destroy( "sem.four", TRUE );
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == MQX_OK,"Test #2 Testing: 2.13:Destroy sem.four")

   _task_set_error(MQX_OK);
   
   /* test SEM_STRICT, PRIORITY INHERITANCE and, PRIORITY QUEUEING. */
    
   /* test for SEM_INVALID_POLICY
   ** return - SEM_INVALID_POLICY = 0x100B
   */

   result =  _sem_create( sem_name, 1, SEM_PRIORITY_INHERITANCE );
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == SEM_INVALID_POLICY,"Test #2 Testing: 2.14: Create sem.one should return SEM_INVALID_POLICY value")

   _task_set_error(MQX_OK);
   
   /* test normal creation */
   result =  _sem_create( sem_name, 1, SEM_PRIORITY_QUEUEING | SEM_PRIORITY_INHERITANCE | SEM_STRICT );
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == MQX_OK,"Test #2 Testing: 2.15: Create sem.one operation")

   _task_set_error(MQX_OK);

   result =  _sem_destroy( sem_name, TRUE );
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == MQX_OK,"Test #2 Testing: 2.16: Destroy sem.one")

   _task_set_error(MQX_OK);
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST # 3 - Testing _sem_create_fast function 
//
//END---------------------------------------------------------------------
 void tc_3_main_task(void)
 {
    _mqx_uint       result;

   index=0;

   /* check for MQX memory error
   ** Return code - MQX_OUT_OF_MEMORY = 0X4
   */
   memory_alloc_all(&memory_alloc_info);
   result =  _sem_create_fast(index, 1, 0);
   memory_free_all(&memory_alloc_info);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,result == MQX_OUT_OF_MEMORY,"Test #3 Testing: 3.0:Check for MQX memory error")

   _task_set_error(MQX_OK);
   
   /* test for the invalid component base
   ** Return code - MQX_INVALID_COMPONENT_BASE = 0x63
   */
   sem_valid = get_sem_component_valid_field();
   set_sem_component_valid_field(0);
   result =  _sem_create_fast(index, 1, 0);
   set_sem_component_valid_field(sem_valid);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,result == MQX_INVALID_COMPONENT_BASE,"Test #3 Testing: 3.1:Test for the invalid component base")

   _task_set_error(MQX_OK);

   /* test for the  MQX component exist
   ** Return code - MQX_OUT_OF_MEMORY = 0x04
   */
   sem_component_ptr = get_sem_component_ptr();
   set_sem_component_ptr(NULL);
   /* by default if the component doesn't exist
      it will create a default component, so we need to cause
      the creation to fail by creating a memory error */
   memory_alloc_all(&memory_alloc_info);
   result =  _sem_create_fast(index, 1, 0);
   memory_free_all(&memory_alloc_info);
   set_sem_component_ptr(sem_component_ptr);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,result == MQX_OUT_OF_MEMORY,"Test #3 Testing: 3.2:Test for the  MQX component exist")

   _task_set_error(MQX_OK);

   /* test MQX_CANOT_CALL_FUNCTION_FROM_ISR
   ** Return code - MQX_CANNOT_CALL_FUNCTION_FROM_ISR = 0X0C
   */
   _int_disable();
   isr_value = get_in_isr();
   set_in_isr(1);
   result =  _sem_create_fast(index, 1, 0);
   set_in_isr(isr_value);
   _int_enable();
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR,"Test #3 Testing: 3.3:Test MQX_CANOT_CALL_FUNCTION_FROM_ISR")

   _task_set_error(MQX_OK);

   /* test the normal creation
   ** Return code - MQX_OK = 0
   */
   result =  _sem_create_fast(index, 1, 0);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,result == MQX_OK,"Test #3 Testing: 3.4:Test the normal creation")

   _task_set_error(MQX_OK);

   /* test for sem exists error
   ** Return code - SEM_SEMAPHORE_EXISTS = 0X1008
   */
   result =  _sem_create_fast(index, 1, 0);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,result == SEM_SEMAPHORE_EXISTS,"Test #3 Testing: 3.5:Test for sem exists error")

   _task_set_error(MQX_OK);
    
   /* test for sem table full error
   ** Return code - SEM_SEMAPHORE_TABLE_FULL = 0X1009
   */
         
   for ( index = 1; index < 5; index++ ) {
      result =  _sem_create_fast(index, 1, 0);
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, (index != 4 || result == SEM_SEMAPHORE_TABLE_FULL),"Test #3 Testing: 3.6:Test for sem table full error")

      EU_ASSERT_REF_TC_MSG(tc_3_main_task, (index >= 4 || result == MQX_OK),"Test #3 Testing: 3.7:test for sem table full error")

      _task_set_error(MQX_OK);
   }
   
   /* destroy the semaphores that were created */
   for ( index = 0; index < 4; index++ ) {
      result =  _sem_destroy_fast(index, TRUE);
      EU_ASSERT_REF_TC_MSG(tc_3_main_task,result == MQX_OK,"Test #3 Testing: 3.8:Destroy the semaphores that were created")

      _task_set_error(MQX_OK);
   }

   /* test SEM_STRICT, PRIORIYT INHERITANCE and, PRIORITY QUEUEING. */
   index = 0;
    
   /* test for SEM_INVALID_POLICY
   ** return - SEM_INVALID_POLICY = 0x100B
   */
   result =  _sem_create_fast( index, 1, SEM_PRIORITY_INHERITANCE );
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,result == SEM_INVALID_POLICY,"Test #3 Testing: 3.9:Test for SEM_INVALID_POLICY")

   _task_set_error(MQX_OK);

   /* test for normal creation */
   result =  _sem_create_fast( index, 1, SEM_PRIORITY_QUEUEING | SEM_PRIORITY_INHERITANCE | SEM_STRICT );
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,result == MQX_OK,"Test #3 Testing: 3.10:Test for normal creation ")

   _task_set_error(MQX_OK);

   result =  _sem_destroy_fast( index, TRUE );
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,result == MQX_OK,"Test #3 Testing: 3.11: Destroy sem just created")

   _task_set_error(MQX_OK);
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST # 4 - Testing _sem_open function 
//
//END---------------------------------------------------------------------
 void tc_4_main_task(void)
 {
    _mqx_uint       result;

   /* initial setup */
    
   result =  _sem_create( sem_name, 1, 0 );
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,result == MQX_OK,"Test #4 Testing: 4.0: Create sem.one")

   _task_set_error(MQX_OK);

   /* check for MQX memory error
   ** Return code - MQX_OUT_OF_MEMORY
   */
   memory_alloc_all(&memory_alloc_info);
   result = _sem_open( sem_name, &sem_ptr );
   memory_free_all(&memory_alloc_info);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,result == MQX_OUT_OF_MEMORY,"Test #4 Testing: 4.1:Check for MQX memory error with sem.one")

   _task_set_error(MQX_OK);

   /* test for the invalid component base
   ** Return code - MQX_INVALID_COMPONENT_BASE
   */
   sem_valid = get_sem_component_valid_field();
   set_sem_component_valid_field(0);
   result = _sem_open( sem_name, &sem_ptr );
   set_sem_component_valid_field(sem_valid);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,result == MQX_INVALID_COMPONENT_BASE,"Test #4 Testing: 4.2:Test the invalid component base with sem.one")

   _task_set_error(MQX_OK);

   /* test for the  MQX component does not exist
   ** Return code - MQX_COMPONENT_DOES_NOT_EXIST
   */
   sem_component_ptr = get_sem_component_ptr();
   set_sem_component_ptr(NULL);
   result = _sem_open( sem_name, &sem_ptr );
   set_sem_component_ptr(sem_component_ptr);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,result == MQX_COMPONENT_DOES_NOT_EXIST,"Test #4 Testing: 4.3:Test the  MQX component does not exist with sem.one")

   _task_set_error(MQX_OK);

   /* test for Invalid sem
   ** Return code - SEM_INVALID_SEMAPHORE
   */
   valid_value= get_sem_valid_field(sem_name);
   set_sem_valid_field(sem_name, 0);
   result = _sem_open( sem_name, &sem_ptr );
   set_sem_valid_field(sem_name, valid_value);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,result == SEM_INVALID_SEMAPHORE,"Test #4 Testing: 4.4:Test for Invalid sem")

   _task_set_error(MQX_OK);

   /* test the normal creation
   ** Return code - MQX_OK = 0
   */
   result = _sem_open( sem_name, &sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,result == MQX_OK,"Test #4 Testing: 4.5:Test the normal creation for sem.one")

   _task_set_error(MQX_OK);

   /* test for delayed destroy
   ** Return code - SEM_SEMAPHORE_DELETED
   */
   delayed_destroy_value = get_sem_delayed_destroy_field(sem_name);
   set_sem_delayed_destroy_field(sem_name, TRUE);
   result = _sem_open( sem_name, &sem2_ptr );
   set_sem_delayed_destroy_field(sem_name, delayed_destroy_value);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,result == SEM_SEMAPHORE_DELETED,"Test #4 Testing: 4.6:Test for delayed destroy")

   _task_set_error(MQX_OK);
  
    /* opening sem2_ptr will not return no error
   ** Return code - MQX_OK = 0
   */
   result = _sem_open( sem_name, &sem2_ptr );
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,result == MQX_OK,"Test #4 Testing: 4.7:Opening sem2_ptr should return no error")

   _task_set_error(MQX_OK);

   result = _sem_close( sem2_ptr );
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,result == MQX_OK,"Test #4 Testing: 4.8: Close sem.one with sem2_ptr parameter")

   _task_set_error(MQX_OK);

   result = _sem_close( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,result == MQX_OK,"Test #4 Testing: 4.9:Close sem.one with sem_ptr parameter")

   _task_set_error(MQX_OK);

   /* test for Sem not found
   ** Return code - SEM_SEMAPHORE_NOT_FOUND = 0X100A
   */
   result =  _sem_destroy(sem_name, TRUE);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,result == MQX_OK,"Test #4 Testing: 4.10: Destroy sem.one")

   _task_set_error(MQX_OK);
   result = _sem_open( sem_name, &sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,result == SEM_SEMAPHORE_NOT_FOUND,"Test #4 Testing: 4.11:Test for Sem not found")

   _task_set_error(MQX_OK);
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_5_main_task
// Comments     : TEST # 5 - Testing _sem_open_fast function 
//
//END---------------------------------------------------------------------
 void tc_5_main_task(void)
 {
   _mqx_uint       result;  

   /* initial setup */
   index=0;
   result = _sem_create_fast(index, 1, 0);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task,result == MQX_OK,"Test #5 Testing: 5.0: Create sem")

   _task_set_error(MQX_OK);

   /* check for MQX memory error
   ** Return code - MQX_OUT_OF_MEMORY = 0X4
   */
   memory_alloc_all(&memory_alloc_info);
   result = _sem_open_fast( index, &sem_ptr );
   memory_free_all(&memory_alloc_info);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task,result == MQX_OUT_OF_MEMORY,"Test #5 Testing: 5.1:Check MQX memory error for sem just created")

   _task_set_error(MQX_OK);
   
   /* test for the invalid component base
   ** Return code - MQX_INVALID_COMPONENT_BASE = 0x63
   */
   sem_valid = get_sem_component_valid_field();
   set_sem_component_valid_field(0);
   result = _sem_open_fast( index, &sem_ptr );
   set_sem_component_valid_field(sem_valid);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task,result == MQX_INVALID_COMPONENT_BASE,"Test #5 Testing: 5.2:Test the invalid component base for sem just created")

   _task_set_error(MQX_OK);

   /* test for the  MQX component does not exist
   ** Return code - MQX_COMPONENT_DOES_NOT_EXIST = 0x61
   */
   sem_component_ptr = get_sem_component_ptr();
   set_sem_component_ptr(NULL);
   result = _sem_open_fast( index, &sem_ptr );
   set_sem_component_ptr(sem_component_ptr);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task,result == MQX_COMPONENT_DOES_NOT_EXIST,"Test #5 Testing: 5.3:Test for the  MQX component does not exist")

   _task_set_error(MQX_OK);

   /* test for Invalid sem
   ** Return code - SEM_INVALID_SEMAPHORE = 0X100C
   */
   valid_value= get_sem_valid_field_fast(index);
   set_sem_valid_field_fast(index, 0);
   result = _sem_open_fast( index, &sem_ptr );
   set_sem_valid_field_fast(index, valid_value);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task,result == SEM_INVALID_SEMAPHORE,"Test #5 Testing: 5.4:Test for Invalid sem")

   _task_set_error(MQX_OK);

   /* test the normal creation
   ** Return code - MQX_OK = 0
   */
   result = _sem_open_fast( index, &sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_5_main_task,result == MQX_OK,"Test #5 Testing: 5.5:Test the normal creation")

   _task_set_error(MQX_OK);
     
   /* opening again will not return an error
   ** Return code - MQX_OK = 0
   */
   result = _sem_open_fast( index, &sem2_ptr );
   EU_ASSERT_REF_TC_MSG(tc_5_main_task,result == MQX_OK,"Test #5 Testing: 5.6:Opening sem again")

   _task_set_error(MQX_OK);

   result = _sem_close( sem2_ptr );
   EU_ASSERT_REF_TC_MSG(tc_5_main_task,result == MQX_OK,"Test #5 Testing: 5.7: Close sem")

   _task_set_error(MQX_OK);

   /* test for delayed destroy
   ** Return code - SEM_SEMAPHORE_DELETED = 0x1001
   */
   delayed_destroy_value = get_sem_delayed_destroy_field_fast( index );
   set_sem_delayed_destroy_field_fast( index, TRUE);
   result = _sem_open_fast( index, &sem2_ptr );
   set_sem_delayed_destroy_field_fast( index, delayed_destroy_value );
   EU_ASSERT_REF_TC_MSG(tc_5_main_task,result == SEM_SEMAPHORE_DELETED,"Test #5 Testing: 5.8:Test for delayed destroy")

   _task_set_error(MQX_OK);
   /* test for sem not found
   ** Return code - SEM_SEMAPHORE_NOT_FOUND = 0X100A
   */
   /* destroy the sem */
   
   result =  _sem_close( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_5_main_task,result == MQX_OK,"Test #5 Testing: 5.9:Close sem")

   _task_set_error(MQX_OK);

   result =  _sem_destroy_fast(index, TRUE);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task,result == MQX_OK,"Test #5 Testing: 5.10: Destroy sem")

   _task_set_error(MQX_OK);

   result = _sem_open_fast( index, &sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_5_main_task,result == SEM_SEMAPHORE_NOT_FOUND,"Test #5 Testing: 5.11:Test for sem not found")

   _task_set_error(MQX_OK);
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_6_main_task
// Comments     : TEST # 6 - Testing _sem_close function 
//
//END---------------------------------------------------------------------
 void tc_6_main_task(void)
 {
   _mqx_uint       result;

   /* initial setup */
   index=0;
   result = _sem_create_fast(index, 1, 0);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task,result == MQX_OK,"Test #6 Testing: 6.0: Create sem")

   _task_set_error(MQX_OK);

   result = _sem_open_fast( index, &sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_6_main_task,result == MQX_OK,"Test #6 Testing: 6.1: Open sem")

   _task_set_error(MQX_OK);
   
   /* test MQX_CANOT_CALL_FUNCTION_FROM_ISR
   ** Return code - MQX_CANNOT_CALL_FUNCTION_FROM_ISR
   */
   _int_disable();
   isr_value = get_in_isr();
   set_in_isr(1);
   result = _sem_close( sem_ptr );
   set_in_isr(isr_value);
   _int_enable();
   EU_ASSERT_REF_TC_MSG(tc_6_main_task,result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR,"Test #6 Testing: 6.2:Test MQX_CANOT_CALL_FUNCTION_FROM_ISR")

   _task_set_error(MQX_OK);


   /* test for Invalid sem
   ** Return code - SEMT_INVALID_SEMAPHORE_HANDLE
   */
   handle_field = get_sem_handle_valid_field(sem_ptr);
   set_sem_handle_valid_field(sem_ptr, 0);
   result = _sem_close( sem_ptr );
   set_sem_handle_valid_field(sem_ptr,handle_field);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task,result == SEM_INVALID_SEMAPHORE_HANDLE,"Test #6 Testing: 6.3:Test for Invalid sem")

   _task_set_error(MQX_OK);

   /* test for normal close
   ** Return code - MQX_OK = 0
   */
   result = _sem_close( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_6_main_task,result == MQX_OK,"Test #6 Testing: 6.4:Test for normal close")

   _task_set_error(MQX_OK);

    /* destroy the sem */
   result =  _sem_destroy_fast(index, TRUE);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task,result == MQX_OK,"Test #6 Testing: 6.5:Destroy the sem")

   _task_set_error(MQX_OK);
 }
 //TASK--------------------------------------------------------------------
// 
// Task Name    : tc_7_main_task
// Comments     : TEST # 7 - Testing _sem_destroy_fast function 
//
//END---------------------------------------------------------------------
 void tc_7_main_task(void)
 {
   _mqx_uint       result;  

   /* initial setup */
   index=0;
   result = _sem_create_fast(index, 1, 0);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == MQX_OK,"Test #7 Testing: 7.0: create sem")

   _task_set_error(MQX_OK);
 
   /* test MQX_CANOT_CALL_FUNCTION_FROM_ISR
   ** Return code - MQX_OK = 0X0C
   */
   _int_disable();
   isr_value = get_in_isr();
   set_in_isr(1);
   result =  _sem_destroy_fast(index, TRUE);
   set_in_isr(isr_value);
   _int_enable();
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR,"Test #7 Testing: 7.1:Test MQX_CANOT_CALL_FUNCTION_FROM_ISR")

   _task_set_error(MQX_OK);

   /* test MQX_COMPONENT_DOES_NOT_EXIST
   ** Return code - 
   */
   sem_component_ptr = get_sem_component_ptr();
   set_sem_component_ptr(NULL);
   result =  _sem_destroy_fast(index, TRUE);
   set_sem_component_ptr(sem_component_ptr);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == MQX_COMPONENT_DOES_NOT_EXIST,"Test #7 Testing: 7.2:Test MQX_COMPONENT_DOES_NOT_EXIST")

   _task_set_error(MQX_OK);

   /* test MQX_INVALID_COMPONENT_BASE
   ** Return code - 
   */
   sem_valid = get_sem_component_valid_field();
   set_sem_component_valid_field(0);
   result =  _sem_destroy_fast(index, TRUE);
   set_sem_component_valid_field(sem_valid);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == MQX_INVALID_COMPONENT_BASE,"Test #7 Testing: 7.3:Test MQX_INVALID_COMPONENT_BASE")

   _task_set_error(MQX_OK);

   /* test for Invalid sem
   ** Return code - SEM_INVALID_SEMAPHORE
   */
   valid_value= get_sem_valid_field_fast(index);
   set_sem_valid_field_fast(index, 0);
   result =  _sem_destroy_fast(index, TRUE);
   set_sem_valid_field_fast(index, valid_value);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == SEM_INVALID_SEMAPHORE,"Test #7 Testing: 7.4:Test for Invalid sem")

   _task_set_error(MQX_OK);

   /* test the normal operation
   ** Return code - MQX_OK = 0
   */
   result =  _sem_destroy_fast(index, TRUE);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == MQX_OK,"Test #7 Testing: 7.5:Destroy sem")

   _task_set_error(MQX_OK);

   /* test for Invalid sem
   ** Return code - SEM_SEMAPHORE_NOT_FOUND
   */
   result =  _sem_destroy_fast(index, TRUE);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == SEM_SEMAPHORE_NOT_FOUND,"Test #7 Testing: 7.6:Test for Invalid sem")

   _task_set_error(MQX_OK);
   result = _sem_test(&sem_error_ptr);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == MQX_OK,"Test #7 Testing: 7.7:_sem_test operation")
   if (result != MQX_OK) {
      //printf("Corrupt semaphore. Result is 0x%X, error is 0x%X.\n", result, sem_error_ptr );
      eunit_test_stop();
      _mqx_exit(0);
   }
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_8_main_task
// Comments     : TEST # 8 - Testing _sem_destroy function, expect W1.3 to be printed
//
//END---------------------------------------------------------------------
 void tc_8_main_task(void)
 {
   _mqx_uint       result;
   
    test_index = 8;
    w13_count = 0;

   /* initial setup */
   result =  _sem_create( sem_name, 1, 0 );
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == MQX_OK,"Test #8 Testing: 8.0: Create sem")

   _task_set_error(MQX_OK);
   
   /* test MQX_CANNOT_CALL_FUNCTION_FROM_ISR
   ** Return code - MQX_OK
   */
   _int_disable();
   isr_value = get_in_isr();
   set_in_isr(1);
   result =  _sem_destroy( sem_name, TRUE );
   set_in_isr(isr_value);
   _int_enable();
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR,"Test #8 Testing: 8.1:Test MQX_CANNOT_CALL_FUNCTION_FROM_ISR")

   _task_set_error(MQX_OK);

   /* test MQX_COMPONENT_DOES_NOT_EXIST
   ** Return code - 
   */
   sem_component_ptr = get_sem_component_ptr();
   set_sem_component_ptr(NULL);
   result =  _sem_destroy( sem_name, TRUE );
   set_sem_component_ptr(sem_component_ptr);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == MQX_COMPONENT_DOES_NOT_EXIST,"Test #8 Testing: 8.2:Test MQX_COMPONENT_DOES_NOT_EXIST")

   _task_set_error(MQX_OK);

   /* test MQX_INVALID_COMPONENT_BASE
   ** Return code - 
   */
   sem_valid = get_sem_component_valid_field();
   set_sem_component_valid_field(0);
   result =  _sem_destroy( sem_name, TRUE );
   set_sem_component_valid_field(sem_valid);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == MQX_INVALID_COMPONENT_BASE,"Test #8 Testing: 8.3:Test MQX_INVALID_COMPONENT_BASE")

   _task_set_error(MQX_OK);

   /* test for Invalid sem
   ** Return code - SEM_INVALID_SEMAPHORE
   */
   valid_value= get_sem_valid_field(sem_name);
   set_sem_valid_field(sem_name, 0);
   result =  _sem_destroy( sem_name, TRUE );
   set_sem_valid_field(sem_name, valid_value);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == SEM_INVALID_SEMAPHORE,"Test #8 Testing: 8.4:Test for Invalid sem")

   _task_set_error(MQX_OK);


   /* test the normal operation
   ** Return code - MQX_OK = 0
   */
   result =  _sem_destroy( sem_name, TRUE );
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == MQX_OK,"Test #8 Testing: 8.5: Destroy sem")

   _task_set_error(MQX_OK);

   /* test for Invalid sem
   ** Return code - SEM_SEMAPHORE_NOT_FOUND
   */
   result =  _sem_destroy( sem_name, TRUE );
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == SEM_SEMAPHORE_NOT_FOUND,"Test #8 Testing: 8.6:Test for Invalid sem")

   _task_set_error(MQX_OK);

   /* Test delayed destroy */
     
   result =  _sem_create( sem_name, 1, 0 );
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == MQX_OK,"Test #8 Testing: 8.7:Test delayed destroy")

   _task_set_error(MQX_OK);
   result = _sem_open( sem_name, &sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == MQX_OK,"Test #8 Testing: 8.8: Open sem.one")

   _task_set_error(MQX_OK);
   result = _sem_wait( sem_ptr, 0 );
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == MQX_OK,"Test #8 Testing: 8.9:Wait sem.one")

   _task_set_error(MQX_OK);

   result = _task_create(0, WAIT_TIME_TASK, 0);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result != MQX_NULL_TASK_ID,"Test #8 Testing: 8.9.0:Create wait_time_task")

   _task_set_error(MQX_OK);

   settings.force_destroy = FALSE;
   settings.name = sem_name;
   _task_create(0, TEST_DESTROY_SEM_TASK, (uint32_t) &settings );
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,_task_get_error() == MQX_OK,"Test #8 Testing: 8.9.1: Test destroy sem task")

   _task_set_error(MQX_OK);

   /* semaphore should be deleted on first post */
   result = _sem_post( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == MQX_OK,"Test #8 Testing: 8.9.2:Semaphore should be deleted on first post ")

   _task_set_error(MQX_OK);

   result = _sem_post( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == MQX_OK,"Test #8 Testing: 8.9.3:Sem post sem.one")

   _task_set_error(MQX_OK);

   result =  _sem_create( sem_name, 0, 0 );
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == MQX_OK,"Test #8 Testing: 8.9.4: Create sem.one")

   _task_set_error(MQX_OK);
   result = _sem_open( sem_name, &sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == MQX_OK,"Test #8 Testing: 8.9.5:Open sem.one")

   _task_set_error(MQX_OK);
   
   result = _task_create(0, WAIT_TIME_TASK, 0);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result != MQX_NULL_TASK_ID,"Test #8 Testing: 8.9.7:Create wait_time_task")

   _task_set_error(MQX_OK);

   settings.force_destroy = TRUE;
   settings.name = sem_name;

   _task_create (0,TEST_DESTROY_SEM_TASK, (uint32_t) &settings );
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,_task_get_error() == MQX_OK,"Test #8 Testing: 8.9.8:Test destroy sem task")

   _task_set_error(MQX_OK);

   result = _sem_post( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == MQX_OK,"Test #8 Testing: 8.9.9:Sem post sem.one")

   _task_set_error(MQX_OK);
   
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (w13_count == 1), "Test #8 Testing: 8.10: W1.3 should be printed 1 time")
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_9_main_task
// Comments     : TEST # 9 - Testing _sem_get_value function 
//
//END---------------------------------------------------------------------
 void tc_9_main_task(void)
 {
   _mqx_uint       result;

   /* initial setup */
   result =  _sem_create( sem_name,1,0 );
   EU_ASSERT_REF_TC_MSG(tc_9_main_task,result == MQX_OK,"Test #9 Testing: 9.0: Create sem.one")

   _task_set_error(MQX_OK);
   result = _sem_open( sem_name, &sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_9_main_task,result == MQX_OK,"Test #9 Testing: 9.1: Open sem.one")

   _task_set_error(MQX_OK);

   /* test for Invalid sem
   ** Return code - SEM_INVALID_SEMAPHORE_HANDLE
   */
   handle_field = get_sem_handle_valid_field(sem_ptr);
   set_sem_handle_valid_field( sem_ptr, 0 );
   result = _sem_get_value( sem_ptr );
   set_sem_handle_valid_field( sem_ptr, handle_field );
   EU_ASSERT_REF_TC_MSG(tc_9_main_task,((result == MAX_MQX_UINT) && (_task_get_error() == SEM_INVALID_SEMAPHORE_HANDLE)),"Test #9 Testing: 9.2: Test for Invalid sem")

   _task_set_error(MQX_OK);

   /* test the normal operation
   ** Return code - MQX_OK = 0
   */
   result = _sem_get_value( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, ((result == 1) && (_task_get_error() == MQX_OK)),"Test #9 Testing: 9.3: Test _sem_get_value")

   _task_set_error(MQX_OK);

   /* test for Invalid sem
   ** Return code - SEM_INVALID_SEMAPHORE
   */
   
   result =  _sem_close( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_9_main_task,result == MQX_OK,"Test #9 Testing: 9.4: Close sem.one")

   _task_set_error(MQX_OK);

   result =  _sem_destroy( sem_name, TRUE );
   EU_ASSERT_REF_TC_MSG(tc_9_main_task,result == MQX_OK,"Test #9 Testing: 9.5: Destroy sem.one")

   _task_set_error(MQX_OK);

   result = _sem_get_value( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, ((result == MAX_MQX_UINT) && (_task_get_error() == SEM_INVALID_SEMAPHORE_HANDLE)),"Test #9 Testing: 9.6:Test _sem_get_value with sem_ptr parameter")

   _task_set_error(MQX_OK);
   result = _sem_test(&sem_error_ptr);
   EU_ASSERT_REF_TC_MSG(tc_9_main_task,result == MQX_OK,"Test #9 Testing: 9.7: _sem_test operation")
   if (result != MQX_OK) {
      //printf("Corrupt semaphore. Result is 0x%X, error is 0x%X.\n", result, sem_error_ptr );
      eunit_test_stop();
      _mqx_exit(0);
   }
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_10_main_task
// Comments     : TEST # 10 - Testing _sem_get_wait_count function
//
//END---------------------------------------------------------------------
 void tc_10_main_task(void)
 {
   _mqx_uint       result;
   
   test_index = 10;

   /* initial setup */
   result =  _sem_create( sem_name,1,0 );
   EU_ASSERT_REF_TC_MSG(tc_10_main_task,result == MQX_OK,"Test #10 Testing: 10.0: Create sem.one")

   _task_set_error(MQX_OK);

   result = _sem_open( sem_name, &sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_10_main_task,result == MQX_OK,"Test #10 Testing: 10.1: Open sem.one")

   _task_set_error(MQX_OK);

   /* test for Invalid sem
   ** Return code - SEM_INVALID_SEMAPHORE_HANDLE
   */
   handle_field = get_sem_handle_valid_field(sem_ptr);
   set_sem_handle_valid_field(sem_ptr, 0);
   result = _sem_get_wait_count( sem_ptr );
   set_sem_handle_valid_field(sem_ptr,handle_field);
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, ((result == MAX_MQX_UINT) && (_task_get_error() == SEM_INVALID_SEMAPHORE_HANDLE)),"Test #10 Testing: 10.2: Test for Invalid sem")

   _task_set_error(MQX_OK);

   /* test the normal operation
   ** Return code - MQX_OK = 0
   */
   result = _sem_get_wait_count( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == 0 && (_task_get_error() == MQX_OK)),"Test #10 Testing: 10.3: _sem_get_wait_count operation")

   _task_set_error(MQX_OK);

   result = _sem_wait( sem_ptr, 0 );
   EU_ASSERT_REF_TC_MSG(tc_10_main_task,result == MQX_OK,"Test #10 Testing: 10.4: _sem_wait operation")

   _task_set_error(MQX_OK);

   result = _task_create(0, WAIT_TIME_TASK, 0);
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, result != MQX_NULL_TASK_ID,"Test #10 Testing: 10.4.0:Create task with WAIT_TIME_TASK")

   _task_set_error(MQX_OK);

   result = _sem_get_wait_count( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, ((result == 1) && (_task_get_error() == MQX_OK)),"Test #10 Testing: 10.5:_sem_get_wait_count operation")

   _task_set_error(MQX_OK);

   result = _sem_post( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10 Testing: 10.6: Sem post sem.one")

   _task_set_error(MQX_OK);

   /* test for Invalid sem
   ** Return code - SEM_INVALID_SEMAPHORE
   */
   result =  _sem_test(&sem_error_ptr);
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10 Testing: 10.6.1: Test for Invalid sem")

   _task_set_error(MQX_OK);

   result =  _sem_close( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_10_main_task,result == MQX_OK,"Test #10 Testing: 10.7: Close sem.one")

   _task_set_error(MQX_OK);
   
   result =  _sem_destroy( sem_name, TRUE );
   EU_ASSERT_REF_TC_MSG(tc_10_main_task,result == MQX_OK,"Test #10 Testing: 10.8: Destroy sem.one")

   _task_set_error(MQX_OK);
   
   result = _sem_get_wait_count( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, ((result == MAX_MQX_UINT) && (_task_get_error() == SEM_INVALID_SEMAPHORE_HANDLE)),"Test #10 Testing: 10.9: _sem_get_wait_count operation")

   _task_set_error(MQX_OK);
   result = _sem_test(&sem_error_ptr);
   EU_ASSERT_REF_TC_MSG(tc_10_main_task,result == MQX_OK,"Test #10 Testing: 10.10:_sem_test operation" )
   if (result != MQX_OK) {
     // printf("Corrupt semaphore. Result is 0x%X, error is 0x%X.\n", result, sem_error_ptr );
      eunit_test_stop();
      _mqx_exit(0);
   }
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_11_main_task
// Comments     : TEST # 11 - Testing _sem_test function 
//
//END---------------------------------------------------------------------
 void tc_11_main_task(void)
 {
   _mqx_uint       result;  

   /* initial setup */
   result =  _sem_create( sem_name, 1, 0 );
   EU_ASSERT_REF_TC_MSG(tc_11_main_task,result == MQX_OK,"Test #11 Testing: 11.1: Create sem.one")

   _task_set_error(MQX_OK);

   result = _sem_open( sem_name, &sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_11_main_task,result == MQX_OK,"Test #11 Testing: 11.2: Open sem.one")

   _task_set_error(MQX_OK);

   /* test for normal operation
   ** Return code -
   */
   result = _sem_test( &sem_error_ptr );
   EU_ASSERT_REF_TC_MSG(tc_11_main_task,result == MQX_OK,"Test #11 Testing: 11.3:_sem_test operation")

   _task_set_error(MQX_OK);


   /* test MQX_INVALID_COMPONENT_BASE
   ** Return code - 
   */
   sem_valid = get_sem_component_valid_field();
   set_sem_component_valid_field(0);
   result = _sem_test(&sem_error_ptr);
   set_sem_component_valid_field(sem_valid);
   EU_ASSERT_REF_TC_MSG(tc_11_main_task,result == MQX_INVALID_COMPONENT_BASE,"Test #11 Testing: 11.4:Test MQX_INVALID_COMPONENT_BASE")

   _task_set_error(MQX_OK);

   /* test for Invalid sem
   ** Return code - SEM_INVALID_SEMAPHORE = 0X100C
   */
   valid_value= get_sem_valid_field(sem_name);
   set_sem_valid_field(sem_name, 0);
   result = _sem_test(&sem_error_ptr);
   set_sem_valid_field(sem_name, valid_value);
   EU_ASSERT_REF_TC_MSG(tc_11_main_task,result == SEM_INVALID_SEMAPHORE,"Test #11 Testing: 11.5:Test for Invalid sem")

   _task_set_error(MQX_OK);

   /* Delete all the sems created */
   result =  _sem_close( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_11_main_task,result == MQX_OK,"Test #11 Testing: 11.6: Close sem.one")

   _task_set_error(MQX_OK);

   result =  _sem_destroy( sem_name, TRUE );
   EU_ASSERT_REF_TC_MSG(tc_11_main_task,result == MQX_OK,"Test #11 Testing: 11.7:Destroy sem.one")

   _task_set_error(MQX_OK);
   result = _sem_test(&sem_error_ptr);
   EU_ASSERT_REF_TC_MSG(tc_11_main_task,result == MQX_OK,"Test #11 Testing: 11.8:_sem_test operation")
   if (result != MQX_OK) {
      //printf("Corrupt semaphore. Result is 0x%X, error is 0x%P.\n", result, sem_error_ptr );
      eunit_test_stop();
      _mqx_exit(0);
   }
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_12_main_task
// Comments     : TEST # 12 - Testing _sem_wait _sem_wait_ticks _sem_wait_for _sem_wait_until and _sem_post functions expect one W1.3 msg to be printed 
//
//END---------------------------------------------------------------------
 void tc_12_main_task(void)
 {
   _mqx_uint       result,sem_count=0;
   _mqx_uint       i,j;
   MQX_TICK_STRUCT ticks;

   test_index = 12;
   w13_count = 0;
   
#if PRINT_ON
   printf("\n     expect one W1.3 msg to be printed\n\n");
#endif
   /* initial setup */
   result =  _sem_create( sem_name, 1, SEM_STRICT );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.0: Create sem.one")

   _task_set_error(MQX_OK);

   result = _sem_open( sem_name, &sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.1: Open sem.one")
 //b23362  
   sem_count = _sem_get_value(sem_ptr );
 //b23362  
   _task_set_error(MQX_OK);

   /* test for SEM_CANNOT_POST */
   result = _sem_post( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == SEM_CANNOT_POST,"Test #12 Testing: 12.2:Test for SEM_CANNOT_POST")
//b23362  
   sem_count = _sem_get_value(sem_ptr );
 //b23362  

   _task_set_error(MQX_OK);

   /* test MQX_CANNOT_CALL_FUNCTION_FROM_ISR
   ** Return code - MQX_OK = 0X0C
   */
   _int_disable();
   isr_value = get_in_isr();
   set_in_isr(1);


   result =  _sem_wait( sem_ptr, 0 );

//b23362  
   sem_count = _sem_get_value(sem_ptr );
 //b23362  

   set_in_isr(isr_value);
   _int_enable();
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR,"Test #12 Testing: 12.3:Test MQX_CANNOT_CALL_FUNCTION_FROM_ISR")

   _task_set_error(MQX_OK);

   /* test for Invalid sem
   ** Return code - SEM_INVALID_SEMAPHORE_HANDLE = 0X1003
   */
   handle_field = get_sem_handle_valid_field(sem_ptr);
   set_sem_handle_valid_field(sem_ptr, 0);
   result = _sem_wait( sem_ptr, 0 );
   set_sem_handle_valid_field(sem_ptr,handle_field);
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == SEM_INVALID_SEMAPHORE_HANDLE,"Test #12 Testing: 12.4:Test for Invalid sem")

   _task_set_error(MQX_OK);
   
   /* test for Invalid sem
   ** Return code - SEM_INVALID_SEMAPHORE = 0X100C
   */
   valid_value= get_sem_valid_field(sem_name);
   set_sem_valid_field(sem_name, 0);
   result =  _sem_wait( sem_ptr, 0 );
   set_sem_valid_field(sem_name, valid_value);
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == SEM_INVALID_SEMAPHORE,"Test #12 Testing: 12.5:Test for Invalid sem")

   _task_set_error(MQX_OK);

   /* test for deadlock
   ** Return code - 
   */
   result =  _sem_wait( sem_ptr, 0 );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.6:Test for deadlock")

   _task_set_error(MQX_OK);

   /* should produce error 0x809 MQX_EDEADLK */
   result =  _sem_wait( sem_ptr, 0 );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_EDEADLK,"Test #12 Testing: 12.7:_sem_wait operation should return MQX_EDEADLK")

   _task_set_error(MQX_OK);

   result =  _sem_post( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.7.1: Sem post sem")

   _task_set_error(MQX_OK);
   
   /* test sem timeout */
 //  result = _task_create(0, WAIT_TIME_TASK, 10);
 //  EU_ASSERT_REF_TC_MSG(tc_12_main_task,result != MQX_NULL_TASK_ID,"Test #12 Testing: 12.7.2:Test sem timeout")
 //  _task_set_error(MQX_OK);
 //b23362  
   sem_count = _sem_get_value(sem_ptr );
 //b23362 
   result = _task_create(0, WAIT_TIME_TASK, 10);
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result != MQX_NULL_TASK_ID,"Test #12 Testing: 12.7.2:Test sem timeout")

   result = _task_create(0, WAIT_TIME_TASK, 0);
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result != MQX_NULL_TASK_ID,"Test #12 Testing: 12.7.2:Test sem timeout")

   _task_set_error(MQX_OK);
   
 //b23362  
   sem_count = _sem_get_value(sem_ptr );
 //b23362  

   result = _sem_wait( sem_ptr, 10 );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == SEM_WAIT_TIMEOUT,"Test #12 Testing: 12.7.3: _sem_wait operation")

 //b23362  
   sem_count = _sem_get_value(sem_ptr );
 //b23362  

   _task_set_error(MQX_OK);
   
   //result = _sem_wait_ticks( sem_ptr, 2 );
   //EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == SEM_WAIT_TIMEOUT,"Test #12 Testing: 12.7.4:_sem_wait_ticks operation")

   //_task_set_error(MQX_OK);

   ticks = _mqx_zero_tick_struct;
   _time_add_msec_to_ticks(&ticks, 10);

   result = _sem_wait_for( sem_ptr, &ticks );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == SEM_WAIT_TIMEOUT,"Test #12 Testing: 12.7.5: Wait for sem.one")
//b23362  
   sem_count = _sem_get_value(sem_ptr );
//b23362  



   _task_set_error(MQX_OK);

   _int_disable();
   _time_get_elapsed_ticks(&ticks);
   _time_add_msec_to_ticks(&ticks, 10);
   
   result = _sem_wait_until( sem_ptr, &ticks );
   _int_enable();
   EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == SEM_WAIT_TIMEOUT, "Test #12 Testing: 12.7.6:Sem wait until sem.one")
//b23362  
   sem_count = _sem_get_value(sem_ptr );
 //b23362  

   _task_set_error(MQX_OK);
    
   result = _sem_close( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.8: Close sem.one")

   _task_set_error(MQX_OK);

   result = _sem_destroy( sem_name, TRUE );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.9:Destroy sem.one")

   _task_set_error(MQX_OK);

   _time_delay_ticks(10);

   /* PRIORITY INVERSION TEST */
   //printf("\nPriority inversion test.\n");

   result =  _sem_create( sem_name, 1, SEM_STRICT |  SEM_PRIORITY_INHERITANCE );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.10:Create sem.one for priority inversion test")

   _task_set_error(MQX_OK);

   result = _sem_open( sem_name, &sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.11: Open sem.one")

   _task_set_error(MQX_OK);

   _task_get_priority( _task_get_id(), &i );
   
   result =  _sem_wait( sem_ptr, 0 );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.13:_sem_wait operation")

   _task_set_error(MQX_OK);

   _task_create(0,PRIO_TEST_TASK1, 0);
   
   td_ptr = _task_get_td( _task_get_id() );
   j = td_ptr->MY_QUEUE->PRIORITY;

   result =  _sem_post( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.15:Sem post sem.one")

   _task_set_error(MQX_OK);
   EU_ASSERT_REF_TC_MSG(tc_12_main_task, ( j < i ),"Test #12 Testing: 12.16: Test priority inheritance")

   result = _sem_close( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.17:Close sem.one")

   _task_set_error(MQX_OK);

   result = _sem_destroy( sem_name, TRUE );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.18:Destroy sem.one")

   _task_set_error(MQX_OK);

   /* PRIORITY QUEUEING TEST */
#if PRINT_ON
   printf("Priority queueing test.\n");
   printf("The following output should be seen:\n");
   printf("P1.0 Entered Task 1.\nP1.0 Entered Task 2.\n");
   printf("P1.3 Exited Task 2.\nP1.3 Exited Task 1.\n\n");
#endif

   result =  _sem_create( sem_name, 1,  SEM_STRICT | SEM_PRIORITY_QUEUEING );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.19:Create sem.one for priority queueing test")

   _task_set_error(MQX_OK);

   result = _sem_open( sem_name, &sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.20:Open sem.one")

   _task_set_error(MQX_OK);

   result =  _sem_wait( sem_ptr, 0 );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.21:_sem_wait operation")

   _task_set_error(MQX_OK);

   _task_create(0,PRIO_TEST_TASK1, 1);
   _task_create(0,PRIO_TEST_TASK2, 2);
   
   /* On post PRIO_TEST_TASK2 should run */
   result =  _sem_post( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.22:Sem post sem.one")

   _task_set_error(MQX_OK);

   result =  _sem_close( sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.23:Close sem.one")

   _task_set_error(MQX_OK);

   result =  _sem_destroy( sem_name, TRUE );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.24: Destroy sem.one")

   _task_set_error(MQX_OK);
   
   EU_ASSERT_REF_TC_MSG(tc_12_main_task, (w13_count == 1), "Test #12 Testing: 12.25: W1.3 should be printed 1 time")
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_13_main_task
// Comments     : TEST # 13 - Mixing semaphores by name and index 
//
//END---------------------------------------------------------------------
  void tc_13_main_task(void)
 {
   _mqx_uint       result;  

   index = 0;

   result =  _sem_create( sem_name, 1, 0 );
   EU_ASSERT_REF_TC_MSG(tc_13_main_task,result == MQX_OK,"Test #13 Testing: 13.0: Create sem.one")

   _task_set_error(MQX_OK);

   result =  _sem_create( "sem.two", 1, 0 );
   EU_ASSERT_REF_TC_MSG(tc_13_main_task,result == MQX_OK,"Test #13 Testing: 13.1: Create sem.two")

   _task_set_error(MQX_OK);
    
   for (index = 0; index < 4; index++) {
      result =  _sem_create_fast( index, 1, 0 );
      EU_ASSERT_REF_TC_MSG(tc_13_main_task, (index >= 2 || result == SEM_SEMAPHORE_EXISTS),"Test #13 Testing: 13.2:_sem_create_fast operation")

      _task_set_error(MQX_OK);
      EU_ASSERT_REF_TC_MSG(tc_13_main_task, (index < 2 || result == MQX_OK),"Test #13 Testing: 13.3:_sem_create_fast operation")

      _task_set_error(MQX_OK);
   } /* End For */
   
   /* destroy the semaphores that were created */
   /* Semaphores created by name do not get destroyed */
   for (index = 0; index < 4 ; index++) {
      result = _sem_destroy_fast(index, TRUE);
      EU_ASSERT_REF_TC_MSG(tc_13_main_task, (index < 2 || result == MQX_OK),"Test #13 Testing: 13.4: _sem_destroy_fast operation")

      EU_ASSERT_REF_TC_MSG(tc_13_main_task, (index >= 2 || result == SEM_SEMAPHORE_NOT_FOUND),"Test #13 Testing: 13.4: _sem_destroy_fast operation")

      _task_set_error(MQX_OK);
   } /* End For */
  
   //printf("\nGood bye.");

   _time_delay_ticks(10);  // waiting for all loggings to be printed out
 }
 /*END
 -----------------------------------*/
 
//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_sem)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1 - Testing _sem_create_component function"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2 - Testing _sem_create function"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3 - Testing _sem_create_fast function"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4 - Testing _sem_open function"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST # 5 - Testing _sem_open_fast function"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST # 6 - Testing _sem_close function"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST # 7 - Testing _sem_destroy_fast function"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST # 8 - Testing _sem_destroy function, expect W1.3 to be printed"),
    EU_TEST_CASE_ADD(tc_9_main_task, " TEST # 9 - Testing _sem_get_value function"),
    EU_TEST_CASE_ADD(tc_10_main_task," TEST # 10 - Testing _sem_get_wait_count function"),
    EU_TEST_CASE_ADD(tc_11_main_task," TEST # 11 - Testing _sem_test function"),
    EU_TEST_CASE_ADD(tc_12_main_task," TEST # 12 - Testing _sem_wait _sem_wait_ticks _sem_wait_for _sem_wait_until and _sem_post functions expect one W1.3 msg to be printed"),
    EU_TEST_CASE_ADD(tc_13_main_task," TEST # 13 - Mixing semaphores by name and index"),
 EU_TEST_SUITE_END(suite_sem)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_sem, " - Test sem suite ")
 EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : test_destroy_sem_task 
* Comments     : This task test the delay destroy feature for semaphores.
*
*END*----------------------------------------------------------------------*/

void test_destroy_sem_task 
   (
      uint32_t  dummy
   )
{
    _mqx_uint                     result;
    DESTROY_SETTINGS_STRUCT_PTR   settings;

    settings = (DESTROY_SETTINGS_STRUCT_PTR) dummy; 
    result =  _sem_destroy(settings->name, settings->force_destroy);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8 Testing: D1.0 Sem destroy")

    if ( settings->force_destroy == FALSE ) {
        result = get_sem_delayed_destroy_field( settings->name );
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == TRUE, "Test #8 Testing: D1.1 Semaphore Delayed destroy")
    } /* Endif */      
} /* End Body */


/*TASK*-------------------------------------------------------------------
* 
* Task Name : prio_test_task
* Comments  : This task opens a connection to a semaphore named sem_name,
*             and waits on that semaphore.
*
*END*----------------------------------------------------------------------*/

void prio_test_task
   (
      uint32_t dummy
   )
{
   _mqx_uint result;
   void     *sem_ptr;
#if PRINT_ON
   printf("P1.0 Entered Task %ld\n", dummy);
#endif
   
   result = _sem_open( sem_name, &sem_ptr );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "Test #12 Testing: P1.1 Sem open sem.one")
   //if (result != MQX_OK) {
     // printf("P1.1 Sem open sem.one Result is 0x%X TEC 0x%X\n",
       //  result, _task_get_error() );
   //} /* Endif */
   _task_set_error(MQX_OK);

   result =  _sem_wait( sem_ptr, 0 );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: P1.2 Sem wait sem.one")

   _task_set_error(MQX_OK);
#if PRINT_ON
   printf("P1.3 Exited Task %ld\n", dummy);
#endif
} /* Endbody */


/*TASK*--------------------------------------------------------------------
* 
* Task Name : wait_time_task
* Comments  : This task opens a connection to the semaphore named sem_name,
*             waits on that semaphore, and then does a time delay for dummy
*             amout of time
*
*END*----------------------------------------------------------------------*/

void wait_time_task
   (
      uint32_t dummy
   )
{
    _mqx_uint result,sem_count=0;
    void     *sem_ptr;

     
    result = _sem_open( sem_name, &sem_ptr );
    switch (test_index){
    case 8:
        EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == MQX_OK, "W1.1 Sem open sem.one")
        break;
    case 10:
        EU_ASSERT_REF_TC_MSG(tc_10_main_task,result == MQX_OK, "W1.1 Sem open sem.one")
        break;
    case 12:
        EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK, "W1.1 Sem open sem.one")
        break;
    }

    _task_set_error(MQX_OK);
    
 //b23362  
   sem_count = _sem_get_value(sem_ptr );
 //b23362 

    result =  _sem_wait( sem_ptr, 0 );
 //b23362  
   sem_count = _sem_get_value(sem_ptr );
 //b23362 

    switch (test_index){
    case 8:
        // W1.3 should be printed 1 time for test #8
        // W1.3 Sem wait sem.one
        if (result != MQX_OK)
            w13_count++;
        break;
    case 10: 
        EU_ASSERT_REF_TC_MSG(tc_10_main_task,result == MQX_OK,"W1.3 Sem wait sem.one")
        break;
    case 12:
        // W1.3 should be printed 1 time for test #12
        // W1.3 Sem wait sem.one
        if (result != MQX_OK)
            w13_count++;
        break;
    }

    _task_set_error(MQX_OK);
//while(1)
//  {
        _time_delay_ticks(dummy);
//      sem_count++;
//  }

} /* Endbody */


/*TASK*-------------------------------------------------------------------
* 
* Task Name : main_task
* Comments  : This task test the semaphore component.
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t dummy
   )
{
    _int_install_unexpected_isr();

    main_id = _task_get_id();
    sprintf( sem_name, "sem.one" );

   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();

    _mqx_exit(0);

} /* Endbody */

        
/*------------------------------------------------------------------------*/
/* 
** Semaphore Utility Functions
*/

SEM_COMPONENT_STRUCT_PTR get_sem_component_ptr
   (
      void
   )
{
   KERNEL_DATA_STRUCT_PTR     kernel_data;
   SEM_COMPONENT_STRUCT_PTR   sem_component_ptr;
   
   _GET_KERNEL_DATA(kernel_data);                                         
   sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
   return  sem_component_ptr; 

}

void set_sem_component_ptr
   (
      SEM_COMPONENT_STRUCT_PTR sem_ptr
   )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;
   
   _GET_KERNEL_DATA(kernel_data);                                         
   kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES] = sem_ptr;

}

_mqx_uint get_sem_component_valid_field
   (
      void
   )
{
   KERNEL_DATA_STRUCT_PTR     kernel_data;
   SEM_COMPONENT_STRUCT_PTR   sem_component_ptr;


   _GET_KERNEL_DATA(kernel_data);                                         
   sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
   return sem_component_ptr->VALID;

}

void set_sem_component_valid_field
   (
      _mqx_uint sem_valid
   )
{
   KERNEL_DATA_STRUCT_PTR     kernel_data;
   SEM_COMPONENT_STRUCT_PTR   sem_component_ptr;
   
   _GET_KERNEL_DATA(kernel_data);                                         
   sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
   sem_component_ptr->VALID = sem_valid;

}



_mqx_uint get_sem_valid_field
   (
      char  *name_ptr
   )
{
   _mqx_max_type              tmp;
   KERNEL_DATA_STRUCT_PTR     kernel_data;
   SEM_COMPONENT_STRUCT_PTR   sem_component_ptr;
   SEM_STRUCT_PTR             sem_ptr;
   _mqx_uint                  result;

   _GET_KERNEL_DATA(kernel_data);                                         
   sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
   result = _name_find_internal(sem_component_ptr->NAME_TABLE_HANDLE, 
      name_ptr,&tmp);
   if (result == MQX_OK) {
      sem_ptr = (SEM_STRUCT_PTR)tmp;
      return sem_ptr->VALID ;
   } else {
      return 0;
   } /* Endif */

}

void set_sem_valid_field
   (
      char  *name_ptr,
      _mqx_uint  valid_value
   )
{
   _mqx_max_type              tmp;
   KERNEL_DATA_STRUCT_PTR     kernel_data;
   SEM_COMPONENT_STRUCT_PTR   sem_component_ptr;
   SEM_STRUCT_PTR             sem_ptr;
   _mqx_uint                  result;

   _GET_KERNEL_DATA(kernel_data);                                         

   sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
   result = _name_find_internal(sem_component_ptr->NAME_TABLE_HANDLE, 
      name_ptr,&tmp);
   if (result == MQX_OK) {      
      sem_ptr = (SEM_STRUCT_PTR)tmp;
      sem_ptr->VALID = valid_value; 
   } /* Endif */

}

bool get_sem_delayed_destroy_field
   (
      char  *name_ptr
   )
{
   _mqx_max_type              tmp;
   KERNEL_DATA_STRUCT_PTR     kernel_data;
   SEM_COMPONENT_STRUCT_PTR   sem_component_ptr;
   SEM_STRUCT_PTR             sem_ptr;
   _mqx_uint                  result;

   _GET_KERNEL_DATA(kernel_data);                                         
   sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
   result = _name_find_internal(sem_component_ptr->NAME_TABLE_HANDLE, 
      name_ptr,&tmp);
   if (result == MQX_OK) {
      sem_ptr = (SEM_STRUCT_PTR)tmp;
      return sem_ptr->DELAYED_DESTROY ;
   } else {
      return FALSE;
   } /* Endif */

}

void set_sem_delayed_destroy_field
   (
      char  *name_ptr,
      bool delayed_destroy_value
   )
{
   _mqx_max_type              tmp;
   KERNEL_DATA_STRUCT_PTR     kernel_data;
   SEM_COMPONENT_STRUCT_PTR   sem_component_ptr;
   SEM_STRUCT_PTR             sem_ptr;
   _mqx_uint                  result;

   _GET_KERNEL_DATA(kernel_data);                                         

   sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
   result = _name_find_internal(sem_component_ptr->NAME_TABLE_HANDLE, 
      name_ptr, &tmp);
   if (result == MQX_OK) {      
      sem_ptr = (SEM_STRUCT_PTR)tmp;
      sem_ptr->DELAYED_DESTROY = delayed_destroy_value; 
   } /* Endif */

}

bool get_sem_delayed_destroy_field_fast
   (
      _mqx_uint sem_index
   )
{
   _mqx_max_type              tmp;
   KERNEL_DATA_STRUCT_PTR     kernel_data;
   SEM_COMPONENT_STRUCT_PTR   sem_component_ptr;
   SEM_STRUCT_PTR             sem_ptr;
   _mqx_uint                  result;

   _GET_KERNEL_DATA(kernel_data);                                         

   sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
   result = _name_find_internal_by_index(sem_component_ptr->NAME_TABLE_HANDLE, 
      sem_index,&tmp);
   if (result == MQX_OK) {
      sem_ptr = (SEM_STRUCT_PTR)tmp;
      return sem_ptr->DELAYED_DESTROY;
   } else {
      return 0;
   } /* Endif */

}

void set_sem_delayed_destroy_field_fast
   (
      _mqx_uint sem_index,
      bool   delayed_destroy_value
   )
{
   _mqx_max_type              tmp;
   KERNEL_DATA_STRUCT_PTR     kernel_data;
   SEM_COMPONENT_STRUCT_PTR   sem_component_ptr;
   SEM_STRUCT_PTR             sem_ptr;
   _mqx_uint                  result;

   _GET_KERNEL_DATA(kernel_data);                                         

   sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
   result = _name_find_internal_by_index(sem_component_ptr->NAME_TABLE_HANDLE, 
      sem_index,&tmp);
   if (result == MQX_OK) {
      sem_ptr = (SEM_STRUCT_PTR)tmp;
      sem_ptr->DELAYED_DESTROY = delayed_destroy_value;
   } else {
      result = 1;
   } /* Endif */

}

_mqx_uint get_sem_valid_field_fast
   (
      _mqx_uint sem_index
   )
{
   _mqx_max_type              tmp;
   KERNEL_DATA_STRUCT_PTR     kernel_data;
   SEM_COMPONENT_STRUCT_PTR   sem_component_ptr;
   SEM_STRUCT_PTR             sem_ptr;
   _mqx_uint                  result;

   _GET_KERNEL_DATA(kernel_data);                                         

   sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
   result = _name_find_internal_by_index(sem_component_ptr->NAME_TABLE_HANDLE, 
      sem_index,&tmp);
   if (result == MQX_OK) {
      sem_ptr = (SEM_STRUCT_PTR)tmp;
      return sem_ptr->VALID;
   } else {
      return 0;
   } /* Endif */

}

void set_sem_valid_field_fast
   (
      _mqx_uint sem_index,
      _mqx_uint valid_value
   )
{
   _mqx_max_type              tmp;
   KERNEL_DATA_STRUCT_PTR     kernel_data;
   SEM_COMPONENT_STRUCT_PTR   sem_component_ptr;
   SEM_STRUCT_PTR             sem_ptr;
   _mqx_uint                  result;

   _GET_KERNEL_DATA(kernel_data);                                         

   sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
   result = _name_find_internal_by_index(sem_component_ptr->NAME_TABLE_HANDLE, 
      sem_index,&tmp);
   if (result == MQX_OK) {
      sem_ptr = (SEM_STRUCT_PTR)tmp;
      sem_ptr->VALID = valid_value; 
   } /* Endif */

}


_mqx_uint get_sem_handle_valid_field
   (
      void   *users_sem_ptr 
   )
{ 
   SEM_CONNECTION_STRUCT_PTR sem_connection_ptr = users_sem_ptr;

   return sem_connection_ptr->VALID;  

}

void set_sem_handle_valid_field
   (
      void     *users_sem_ptr,
      _mqx_uint handle_field 
   )
{ 
   SEM_CONNECTION_STRUCT_PTR sem_connection_ptr = users_sem_ptr;

   sem_connection_ptr->VALID = handle_field;    

}

/* EOF */

