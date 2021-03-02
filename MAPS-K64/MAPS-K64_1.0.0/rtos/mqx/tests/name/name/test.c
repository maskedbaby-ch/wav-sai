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
*   This file contains the source functions for the name management
*   component tests.
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <name.h>
#include <mem_prv.h>
#include <mqx_prv.h>
// #include <log.h>
// #include <klog.h>
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"


#define FILE_BASENAME   test
#define PRINT_ON        0

#define MAIN_TASK 10

extern void main_task(uint32_t dummy);

//------------------------------------------------------------------------
// Test suite function prototipe
extern void tc_1_main_task(void);//TEST #1  Auto-Component Creation
extern void tc_2_main_task(void);//TEST #2  Component growth
extern void tc_3_main_task(void);//TEST #3  Adding and dleting names
extern void tc_4_main_task(void);//TEST #4  Finding names
extern void tc_5_main_task(void);//TEST #5  Calling from an ISR
extern void tc_6_main_task(void);//TEST #6  Name length
extern void tc_7_main_task(void);//TEST #7  Component creation and growth
//------------------------------------------------------------------------

bool name_test_fn(bool, _mqx_uint, _mqx_uint, _mqx_uint, _mqx_uint);
extern void name_test_print_result(_mqx_uint, _mqx_uint, char *);
extern void name_test_ISR(void *user_isr_ptr);

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK, main_task, 2000, 8, "Main",  MQX_AUTO_START_TASK},
   { 0,         0,            0, 0, 0,       0 }
};



volatile bool  ISR_test;
_mqx_uint ISR_test_result;
void     *Old_ISR_data;
void     (_CODE_PTR_ Old_ISR)(void *);
KERNEL_DATA_STRUCT_PTR  mqx_ptr;
char                    name[NAME_MAX_NAME_SIZE+1];
char                    found_name[NAME_MAX_NAME_SIZE+1];
_mqx_max_type           value;
_mqx_uint               i,j, num, max_num;
bool                 passed, all_passed;

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST #1: Auto-Component Creation
//
//END---------------------------------------------------------------------

 void tc_1_main_task(void)
 {
       passed = TRUE;
       value = 0;
       passed = name_test_fn(passed,1,1,_name_delete("Test"), MQX_COMPONENT_DOES_NOT_EXIST);
       EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed != FALSE), "Test #1 Testing: 1.1: _name_delete operation")
       passed = name_test_fn(passed,1,2,_name_find("Test", &value), MQX_COMPONENT_DOES_NOT_EXIST);
       EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed != FALSE), "Test #1 Testing: 1.2: _name_find operation")
       passed = name_test_fn(passed,1,3,_name_find_by_number(value, name), MQX_COMPONENT_DOES_NOT_EXIST);
       EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed != FALSE), "Test #1 Testing: 1.3: _name_find_by_number operation")
       passed = name_test_fn(passed,1,4,_name_add("0", 0), MQX_OK);
       EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed != FALSE), "Test #1 Testing: 1.4: _name_add operation")
       passed = name_test_fn(passed,1,5,_name_create_component(1,2,3), MQX_OK);
       EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed != FALSE), "Test #1 Testing: 1.5: _name_create_component with parameters 1, 2, 3 operation")
       passed = name_test_fn(passed,1,6,_name_create_component(4,5,6), MQX_OK);
       EU_ASSERT_REF_TC_MSG(tc_1_main_task, (passed != FALSE), "Test #1 Testing: 1.6: _name_create_component with parameters 4, 5, 6 operation")
       all_passed = all_passed && passed;
 }

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST #2: Component growth
//
//END---------------------------------------------------------------------

  void tc_2_main_task(void)
 {
       passed = TRUE;
       num = 1;
       for (i = 1; i < (NAME_DEFAULT_INITIAL_NUMBER - 1); i++) {
          sprintf(name,"%d",num++);
          passed = name_test_fn(passed,2,1,_name_add(name, i), MQX_OK);
          EU_ASSERT_REF_TC_MSG(tc_2_main_task, (passed != FALSE), "Test #2 Testing: 2.1: _name_add operation")
       }
       for (j = 0; j < 3; j++) {
          for (i = 1; i < NAME_DEFAULT_GROW_NUMBER; i++) {
             sprintf(name,"%d",num++);
             passed = name_test_fn(passed,2,2,_name_add(name, i), MQX_OK);
             EU_ASSERT_REF_TC_MSG(tc_2_main_task, (passed != FALSE), "Test #2 Testing: 2.2: _name_add operation")
          }
       }
       all_passed = all_passed && passed;
 }

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST #3: Adding and dleting names
//
//END---------------------------------------------------------------------

  void tc_3_main_task(void)
 {
       max_num = num;
       passed = TRUE;
       for (num = 0; num < max_num; num++) {
          sprintf(name,"%d",num);
          passed = name_test_fn(passed,3,1,_name_add(name, i), NAME_EXISTS);
          EU_ASSERT_REF_TC_MSG(tc_3_main_task, (passed != FALSE), "Test #3 Testing: 3.1: _name_add operation")
       }
       for (num = 0; num < max_num; num++) {
          sprintf(name,"%d",num);
          passed = name_test_fn(passed,3,2,_name_delete(name), MQX_OK);
          EU_ASSERT_REF_TC_MSG(tc_3_main_task, (passed != FALSE), "Test #3 Testing: 3.2: _name_delete operation")
       }
       for (num = 0; num < max_num; num++) {
          sprintf(name,"%d",num);
          passed = name_test_fn(passed,3,3,_name_delete(name), NAME_NOT_FOUND);
          EU_ASSERT_REF_TC_MSG(tc_3_main_task, (passed != FALSE), "Test #3 Testing: 3.3: _name_delete operation")
       }
       
       for (num = 0; num < max_num; num += 2) {
          sprintf(name,"%d",num);
          passed = name_test_fn(passed,3,4,_name_add(name, i), MQX_OK);
          EU_ASSERT_REF_TC_MSG(tc_3_main_task, (passed != FALSE), "Test #3 Testing: 3.4: _name_add operation")
       }
       for (num = 1; num < max_num; num += 2) {
          sprintf(name,"%d",num);
          passed = name_test_fn(passed,3,5,_name_add(name, i), MQX_OK);
          EU_ASSERT_REF_TC_MSG(tc_3_main_task, (passed != FALSE), "Test #3 Testing: 3.5: _name_add operation")
       }
       for (num = 0; num < max_num; num++) {
          sprintf(name,"%d",num);
          passed = name_test_fn(passed,3,6,_name_add(name, i), NAME_EXISTS);
          EU_ASSERT_REF_TC_MSG(tc_3_main_task, (passed != FALSE), "Test #3 Testing: 3.6: _name_add operation")
       }
       for (num = 0; num < max_num; num += 2) {
          sprintf(name,"%d",num);
          passed = name_test_fn(passed,3,7,_name_delete(name), MQX_OK);
          EU_ASSERT_REF_TC_MSG(tc_3_main_task, (passed != FALSE), "Test #3 Testing: 3.7: _name_delete operation")
       }
       for (num = 1; num < max_num; num += 2) {
          sprintf(name,"%d",num);
          passed = name_test_fn(passed,3,8,_name_delete(name), MQX_OK);
          EU_ASSERT_REF_TC_MSG(tc_3_main_task, (passed != FALSE), "Test #3 Testing: 3.8: _name_delete operation")
       }
       all_passed = all_passed && passed;   
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST #4: Finding names
//
//END---------------------------------------------------------------------

  void tc_4_main_task(void)
 {
       passed = TRUE;
       for (num = 0; num < max_num; num += 2) {
          sprintf(name,"%d",num);
          passed = name_test_fn(passed,4,1,_name_add(name,num), MQX_OK);
          EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed != FALSE), "Test #4 Testing: 4.1: _name_add operation")
       }
       for (num = 0; num < max_num; num += 2) {
          sprintf(name,"%d",num);
          passed = name_test_fn(passed,4,2,_name_find(name,&value), MQX_OK);
          EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed != FALSE), "Test #4 Testing: 4.2: _name_find operation")
          passed = name_test_fn(passed,4,3,(_mqx_uint)value, num );
          EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed != FALSE), "Test #4 Testing: 4.3: _name_find operation")
       }
       for (num = 1; num < max_num; num += 2) {
          sprintf(name,"%d",num);
          passed = name_test_fn(passed,4,4,_name_find(name,&value), NAME_NOT_FOUND);
          EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed != FALSE), "Test #4 Testing: 4.4: _name_find operation")
       }
       for (num = 0; num < max_num; num += 2) {
          sprintf(name,"%d",num);
          _mem_zero(found_name, NAME_MAX_NAME_SIZE);
          passed = name_test_fn(passed,4,5,_name_find_by_number(num, found_name), MQX_OK);
          EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed != FALSE), "Test #4 Testing: 4.5: _name_find_by_number operation")
          passed = name_test_fn(passed,4,6,strcmp(name, found_name ), 0);
          EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed != FALSE), "Test #4 Testing: 4.6: _name_find_by_number operation")
       }
       for (num = 1; num < max_num; num += 2) {
          sprintf(name,"%d",num);
          passed = name_test_fn(passed,4,7,_name_find_by_number(num, found_name), NAME_NOT_FOUND);
          EU_ASSERT_REF_TC_MSG(tc_4_main_task, (passed != FALSE), "Test #4 Testing: 4.7: _name_find_by_number operation")
       }
       all_passed = all_passed && passed;
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_5_main_task
// Comments     : TEST #5: Calling from an ISR
//
//END---------------------------------------------------------------------

  void tc_5_main_task(void)
 {
       passed = TRUE;
       ISR_test_result = FALSE;
       Old_ISR_data = _int_get_isr_data(BSP_TIMER_INTERRUPT_VECTOR);
       Old_ISR      = _int_get_isr(BSP_TIMER_INTERRUPT_VECTOR);

       _int_disable();
       ISR_test = TRUE;
       _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, name_test_ISR, 0 );

       _time_delay_ticks(2);
       
       _int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, Old_ISR, Old_ISR_data );
       _int_enable();

       passed = passed && ISR_test_result;
       EU_ASSERT_REF_TC_MSG(tc_5_main_task, (passed != FALSE), "Test #5 Testing: Calling from ISR operation")
       all_passed = all_passed && passed;
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_6_main_task
// Comments     : TEST #6: Name length
//
//END---------------------------------------------------------------------

  void tc_6_main_task(void)
 {
       passed = TRUE;
       /* Delete names to clean up name database for next test */
       for (num = 0; num < max_num; num += 2) {
          sprintf(name,"%d",num);
          passed = name_test_fn(passed,6,1,_name_delete(name), MQX_OK);
          EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed != FALSE), "Test #6 Testing: 6.1: _name_delete operation")
       }

       /* First test adding names of various lengths */
       /* Make sure we can't add the null name */
       num = 0;
       _mem_zero(name, NAME_MAX_NAME_SIZE+1);
       passed = name_test_fn(passed,6,2,_name_add(name,num), NAME_TOO_SHORT);
       EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed != FALSE), "Test #6 Testing: 6.2: _name_add operation")
       
       /* Make sure we can add names with valid length name */
       for (num = 0; num < (NAME_MAX_NAME_SIZE - 1); num++) {
          name[num] = 'X';
          passed = name_test_fn(passed,6,3,_name_add(name,num), MQX_OK);
          EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed != FALSE), "Test #6 Testing: 6.3: _name_add operation")
       }
       
       name[num] = 'X';
       passed = name_test_fn(passed,6,4,_name_add(name,num), NAME_TOO_LONG);
       EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed != FALSE), "Test #6 Testing: 6.4: _name_add operation")
       
       /* Now test finding names of various lengths */
       
       num = 0;
       _mem_zero(name, NAME_MAX_NAME_SIZE + 1);
       
       /* Make sure we can't find the null name */
       passed = name_test_fn(passed,6,5,_name_find(name,&value), NAME_TOO_SHORT);
       EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed != FALSE), "Test #6 Testing: 6.5: _name_find operation")
       
       /* Make sure we can find names with valid length name */
       for (num = 0; num < (NAME_MAX_NAME_SIZE - 1); num++) {
          name[num] = 'X';
          passed = name_test_fn(passed,6,6,_name_find(name,&value), MQX_OK);
          EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed != FALSE), "Test #6 Testing: 6.6: _name_find operation")
          passed = name_test_fn(passed,6,7,value, num );
          EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed != FALSE), "Test #6 Testing: 6.7: _name_find operation")
          
          _mem_zero(found_name, NAME_MAX_NAME_SIZE+1);
          passed = name_test_fn(passed,6,8,_name_find_by_number(num, found_name), MQX_OK);
          EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed != FALSE), "Test #6 Testing: 6.8: _name_find_by_number operation")
          passed = name_test_fn(passed,6,9,strcmp(name, found_name), 0);
          EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed != FALSE), "Test #6 Testing: 6.9: _name_find_by_number operation")
       }
       
       name[num] = 'X';
       passed = name_test_fn(passed,6,10,_name_find(name,&value), NAME_TOO_LONG);
       EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed != FALSE), "Test #6 Testing: 6.10: _name_find operation")
       
       /* Now test deleting names of various lengths */
       
       num = 0;
       _mem_zero(name, NAME_MAX_NAME_SIZE + 1);
       
       /* Make sure we can't delete the null name */
       passed = name_test_fn(passed,6,11,_name_delete(name), NAME_TOO_SHORT);
       EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed != FALSE), "Test #6 Testing: 6.11: _name_delete operation")
       
       /* Make sure we can find names with valid length name */
       for (num = 0; num < (NAME_MAX_NAME_SIZE - 1); num++) {
          name[num] = 'X';
          passed = name_test_fn(passed,6,12,_name_delete(name), MQX_OK);
          EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed != FALSE), "Test #6 Testing: 6.12: _name_delete operation")
       }
       
       name[num] = 'X';
       passed = name_test_fn(passed,6,13,_name_delete(name), NAME_TOO_LONG);
       EU_ASSERT_REF_TC_MSG(tc_6_main_task, (passed != FALSE), "Test #6 Testing: 6.13: _name_delete operation")
       all_passed = all_passed && passed;
 }

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_7_main_task
// Comments     : TEST #7: Component creation and growth
//
//END---------------------------------------------------------------------

  void tc_7_main_task(void)
 {
       passed = TRUE;
       mqx_ptr = _mqx_get_kernel_data();
       mqx_ptr->KERNEL_COMPONENTS[KERNEL_NAME_MANAGEMENT] = NULL;

       passed = name_test_fn(passed,7,1,_name_delete("Test"), MQX_COMPONENT_DOES_NOT_EXIST);
       EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed != FALSE), "Test #7 Testing: 7.1: _name_delete operation")
       passed = name_test_fn(passed,7,2,_name_create_component(2,2,8), MQX_OK);
       EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed != FALSE), "Test #7 Testing: 7.2: _name_create_component operation")
       for (num = 0; num < 8; num++) {
          sprintf(name,"%d",num);
          passed = name_test_fn(passed,7,3,_name_add(name,num), MQX_OK);
          EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed != FALSE), "Test #7 Testing: 7.3: _name_add operation")
       }
       num++;
       sprintf(name,"%d",num);
       passed = name_test_fn(passed,7,4,_name_add(name,num), NAME_TABLE_FULL);
       EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed != FALSE), "Test #7 Testing: 7.4: _name_add operation")

       passed = name_test_fn(passed,7,5,_name_create_component(4,4,16), MQX_OK);
       EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed != FALSE), "Test #7 Testing: 7.5: _name_create_component operation")
       for (num = 8; num < 16; num++) {
          sprintf(name,"%d",num);
          passed = name_test_fn(passed,7,6,_name_add(name,num), MQX_OK);
          EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed != FALSE), "Test #7 Testing: 7.6: _name_add operation")
       }
       num++;
       sprintf(name,"%d",num);
       passed = name_test_fn(passed,7,7,_name_add(name,num), NAME_TABLE_FULL);
       EU_ASSERT_REF_TC_MSG(tc_7_main_task, (passed != FALSE), "Test #7 Testing: 7.7: _name_add operation")
       all_passed = all_passed && passed;
}
 /*END-----------------------------------*/

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : name_test_fn
* Returned Value   : bool result
* Comments         :
*
*  This function is used to compare the result of a function call with the
* desired result, and print an error if they don't match.
*
*  This function also calls  the _name_test function to ensure that the 
* name component is not corrupt.
*
*  This function returns FALSE if an error was detected, otherwise it returns 
* the previous result.
*
*END*-------------------------------------------------------------------------*/


bool name_test_fn
(
   bool     overall_result,
   _mqx_uint   test,
   _mqx_uint   sub_test,
   _mqx_uint   result,
   _mqx_uint   desired_result
)
{
   void   *p1, *p2;
   
   if (result != desired_result ) {
      // printf("Test #%d.%d Failed\n", test, sub_test );
      overall_result = FALSE;
   }
   if (!ISR_test) {
      if (_name_test( &p1, &p2) != MQX_OK ) {
#if PRINT_ON
         printf("Test #%d.%d _name_test Failed, error_ptr = %x, extension_ptr = %x\n", 
            test, sub_test, p1, p2 );
#endif
         overall_result = FALSE;
      }
   }

   return(overall_result);
}



/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : name_test_print_result
* Returned Value   : void 
* Comments         :
*
*  This function prints the test name and number, and the result of the test.
*
*END*-------------------------------------------------------------------------*/

void name_test_print_result
(
   _mqx_uint test,
   _mqx_uint result,
   char  *test_name
)
{
#if PRINT_ON
   printf("Test #%d, %s %s", test, test_name,(result?"Passed\n":"Failed\n"));
#endif
}

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : name_test_ISR
* Returned Value   : void 
* Comments         :
*
*  This function is used to test calling name functions from an ISR.  This 
* function executes when a tick interrupt occurs, and attempts to call 
* the name component functions.
*
*END*-------------------------------------------------------------------------*/

void name_test_ISR(void *user_isr_ptr)
{
   _mqx_max_type value;
   char          name[NAME_MAX_NAME_SIZE+1];
   
   
   if (ISR_test) {
      ISR_test_result = TRUE;
      ISR_test_result = name_test_fn(ISR_test_result,5,1,_name_add("0", 0), MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
      EU_ASSERT_REF_TC_MSG(tc_5_main_task, (ISR_test_result != FALSE), "Test #5 Testing: 5.1: _name_add operation")
      ISR_test_result = name_test_fn(ISR_test_result,5,2,_name_delete("Test"), MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
      EU_ASSERT_REF_TC_MSG(tc_5_main_task, (ISR_test_result != FALSE), "Test #5 Testing: 5.2: _name_delete operation")
      ISR_test_result = name_test_fn(ISR_test_result,5,3,_name_find("1", &value), NAME_NOT_FOUND);
      EU_ASSERT_REF_TC_MSG(tc_5_main_task, (ISR_test_result != FALSE), "Test #5 Testing: 5.3: _name_find operation")
      ISR_test_result = name_test_fn(ISR_test_result,5,4,_name_find("2", &value), MQX_OK);
      EU_ASSERT_REF_TC_MSG(tc_5_main_task, (ISR_test_result != FALSE), "Test #5 Testing: 5.4: _name_find operation")
      ISR_test_result = name_test_fn(ISR_test_result,5,5,value, 2 );
      EU_ASSERT_REF_TC_MSG(tc_5_main_task, (ISR_test_result != FALSE), "Test #5 Testing: 5.5: _name_find operation")
      ISR_test_result = name_test_fn(ISR_test_result,5,6,_name_find_by_number(2, name), MQX_OK);
      EU_ASSERT_REF_TC_MSG(tc_5_main_task, (ISR_test_result != FALSE), "Test #5 Testing: 5.6: _name_find_by_number operation")
      ISR_test_result = name_test_fn(ISR_test_result,5,7,strcmp(name, "2" ), 0);
      EU_ASSERT_REF_TC_MSG(tc_5_main_task, (ISR_test_result != FALSE), "Test #5 Testing: 5.7: _name_find_by_number operation")
   
      ISR_test = FALSE;
   }

   /* Chain to previous notifier */
   (*Old_ISR)(Old_ISR_data);
}

//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_name)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Auto-Component Creation"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Component growth"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST #3 - Adding and dleting names"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST #4 - Finding names"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST #5 - Calling from an ISR"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST #6 - Name length"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST #7 - Component creation and growth")
 EU_TEST_SUITE_END(suite_name)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_name, " - Tests the name component")
 EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : Main_task
* Comments     :
*   This task tests the name component.  
* The following tests are conducted:
*
*     1  Auto-Component Creation
*     2  Component growth
*     3  Adding and dleting names
*     4  Finding names
*     5  Calling from an ISR
*     6  Name length
*     7  Component creation and growth
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t dummy
   )
{ /* Body */
    all_passed = TRUE;
    
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
   
} /* Endbody */

/* EOF */
