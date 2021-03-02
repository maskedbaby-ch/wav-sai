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
* $Version : 3.7.7.0$
* $Date    : Mar-24-2011$
*
* Comments:
*
*   This file tests the generic kernel interrupt support functions.
*
*END************************************************************************/

#include <mqx_inc.h>
//#include <mqx.h>
#include <bsp.h>
#include <util.h>

//#include <test_module.h>

#include "EUnit.h"
#include "eunit_crit_secs.h"


#define FILE_BASENAME   test

#define MAIN_TASK  10

#define MAIN_PRIO   9

extern void main_task(uint32_t parameter);
//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);// TEST #1 - Testing _int_set_exception_handler
void tc_2_main_task(void);// TEST #2 - Testing _int_get_exception_handler
void tc_3_main_task(void);// TEST #3 - Testing _int_get_default_isr
void tc_4_main_task(void);// TEST #4 - Testing _int_install_default_isr
void tc_5_main_task(void);// TEST #5 - Testing _int_install_exception_isr
void tc_6_main_task(void);// TEST #6 - Testing _int_install_unexpected_isr
void tc_7_main_task(void);// TEST #7 - Testing _int_get_isr
void tc_8_main_task(void);// TEST #8 - Testing _int_set_isr_dat
void tc_9_main_task(void);// TEST #9 - Testing _int_get_isr_data
void tc_10_main_task(void);// TEST #10 - Testing _int_install_isr
//------------------------------------------------------------------------
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK,  main_task,  1000, MAIN_PRIO,  "main",  MQX_AUTO_START_TASK},
   { 0,         0,         0,     0, 0,      0}
};


/*FUNCTION*-----------------------------------------------------------------
* 
* Task Name    : exception_handler
* Comments     :
*
*END*----------------------------------------------------------------------*/

void exception_handler
   (
      _mqx_uint vector_number,
      _mqx_uint exception_vector_number,
      void     *isr_data,
      void     *frame_ptr
   )
{ /* Body */


} /* Endbody */


/*ISR*-----------------------------------------------------------------
* 
* Task Name    : my_default_isr
* Comments     :
*
*END*----------------------------------------------------------------------*/

void my_default_isr
   (
      void   *vector_number
   )
{ /* Body */

} /* Endbody */


/*ISR*-----------------------------------------------------------------
* 
* Task Name    : my_isr
* Comments     :
*
*END*----------------------------------------------------------------------*/

void my_isr
   (
      void   *isr_data
   )
{ /* Body */

} /* Endbody */

//----------------- Begin Testcases --------------------------------------
KERNEL_DATA_STRUCT_PTR kernel_data;
extern void _bsp_timer_isr(void *);
void   *my_isr_data = (void *)0xABCDEF01;

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST #1: Testing _int_set_exception_handler
//
//END---------------------------------------------------------------------

void tc_1_main_task(void)
{
    _mqx_uint vector;
    void (*old_exception_handler)(_mqx_uint, _mqx_uint, void *, void *);
    bool br;
//  INTERRUPT_TABLE_STRUCT_PTR table_ptr;

#if MQX_CHECK_ERRORS
    vector = kernel_data->FIRST_USER_ISR_VECTOR - 1;
    old_exception_handler = _int_set_exception_handler(vector,
        exception_handler);
    /* Set exception handler with too small vector number */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (_task_errno == MQX_INVALID_VECTORED_INTERRUPT), "Test #1 Testing: _int_set_exception_handler with too small vector number should have returned MQX_INVALID_VECTORED_INTERRUPT")

    _task_set_error(MQX_OK);

    vector = kernel_data->LAST_USER_ISR_VECTOR + 1;
    old_exception_handler = _int_set_exception_handler(vector,
        exception_handler);
    /* Set exception handler with too large vector number */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (_task_errno == MQX_INVALID_VECTORED_INTERRUPT), "Test #1 Testing: _int_set_exception_handler with too large vector number should have returned MQX_INVALID_VECTORED_INTERRUPT")

    _task_set_error(MQX_OK);
#endif
             
    vector = kernel_data->LAST_USER_ISR_VECTOR;
    old_exception_handler = _int_set_exception_handler(vector,
        exception_handler);
    /* Set exception handler with ok vector number */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (_task_errno == MQX_OK), "Test #1 Testing: _int_set_exception_handler operation")

    _task_set_error(MQX_OK);
    

    br = ((void *)exception_handler == (void *)(kernel_data->INTERRUPT_TABLE_PTR[vector - kernel_data->FIRST_USER_ISR_VECTOR].APP_ISR_EXCEPTION_HANDLER));
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (br == TRUE), "Test #1 Testing: check if interrupt exception handler was set correctly")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST #2: Testing _int_get_exception_handler
//
//END---------------------------------------------------------------------

void tc_2_main_task(void)
{
    _mqx_uint vector;
    void (*old_exception_handler)(_mqx_uint, _mqx_uint, void *, void *);

    // printf("TEST # 2 - Testing _int_get_exception_handler\n");

#if MQX_CHECK_ERRORS
    vector = kernel_data->FIRST_USER_ISR_VECTOR - 1;
    old_exception_handler = _int_get_exception_handler(vector);
    /* Get exception handler with too small vector number */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (_task_errno == MQX_INVALID_VECTORED_INTERRUPT), "Test #2 Testing: _int_get_exception_handler with too small vector number should have returned MQX_INVALID_VECTORED_INTERRUPT")

    _task_set_error(MQX_OK);

    vector = kernel_data->LAST_USER_ISR_VECTOR + 1;
    old_exception_handler = _int_get_exception_handler(vector);
    /* Get exception handler with too large vector number */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (_task_errno == MQX_INVALID_VECTORED_INTERRUPT), "Test #2 Testing:  _int_get_exception_handler with too large vector number should have returned MQX_INVALID_VECTORED_INTERRUPT")

    _task_set_error(MQX_OK);
#endif

    vector = kernel_data->LAST_USER_ISR_VECTOR;
    old_exception_handler = _int_get_exception_handler(vector);
    /* Get exception handler with ok vector number */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (_task_errno == MQX_OK), "Test #2 Testing: _int_get_exception_handler operation")

    _task_set_error(MQX_OK);
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST #3: Testing _int_get_default_isr
//
//END---------------------------------------------------------------------

void tc_3_main_task(void)
{
    void (*old_default_isr)(void *);

    // printf("TEST # 3 - Testing _int_get_default_isr\n");
    old_default_isr = _int_get_default_isr();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (old_default_isr == kernel_data->DEFAULT_ISR), "Test #3 Testing: _int_get_default_isr operation")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST #4: Testing _int_install_default_isr
//
//END---------------------------------------------------------------------

void tc_4_main_task(void)
{
    void (*old_default_isr)(void *);

    // printf("TEST # 4 - Testing _int_install_default_isr\n");
    old_default_isr = _int_install_default_isr(my_default_isr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (my_default_isr == kernel_data->DEFAULT_ISR), "Test #4 Testing: _int_install_default_isr operation")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_5_main_task
// Comments     : TEST #5: Testing _int_install_exception_isr
//
//END---------------------------------------------------------------------

void tc_5_main_task(void)
{
    void (*old_default_isr)(void *);

    // printf("TEST # 5 - Testing _int_install_exception_isr\n");
    old_default_isr = _int_install_exception_isr();
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (_int_exception_isr == kernel_data->DEFAULT_ISR), "Test #5 Testing: _int_install_exception_isr operation")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_6_main_task
// Comments     : TEST #6: Testing _int_install_unexpected_isr
//
//END---------------------------------------------------------------------

void tc_6_main_task(void)
{
    void (*old_default_isr)(void *);

    // printf("TEST # 6 - Testing _int_install_unexpected_isr\n");
    old_default_isr = _int_install_unexpected_isr();
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (_int_unexpected_isr == kernel_data->DEFAULT_ISR), "Test #6 Testing: _int_install_unexpected_isr operation")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_7_main_task
// Comments     : TEST #7: Testing _int_get_isr
//
//END---------------------------------------------------------------------

void tc_7_main_task(void)
{
    _mqx_uint vector;
    void (*old_isr)(void *);

    // printf("TEST # 7 - Testing _int_get_isr\n");
#if MQX_CHECK_ERRORS
    vector = kernel_data->FIRST_USER_ISR_VECTOR - 1;
    old_isr = _int_get_isr(vector);
    /* Get isr with too small vector number */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (_task_errno == MQX_INVALID_VECTORED_INTERRUPT), "Test #7 Testing: _int_get_isr with too small vector number should have returned MQX_INVALID_VECTORED_INTERRUPT")

    _task_set_error(MQX_OK);

    vector = kernel_data->LAST_USER_ISR_VECTOR + 1;
    old_isr = _int_get_isr(vector);
    /* Get isr with too large vector number */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (_task_errno == MQX_INVALID_VECTORED_INTERRUPT), "Test #7 Testing: _int_get_isr with too large vector number should have returned MQX_INVALID_VECTORED_INTERRUPT")

    _task_set_error(MQX_OK);
#endif

    vector = BSP_TIMER_INTERRUPT_VECTOR;
    old_isr = _int_get_isr(vector);
    /* Get isr with ok vector number */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (_task_errno == MQX_OK), "Test #7 Testing: _int_get_isr operation")

    _task_set_error(MQX_OK);

    _int_disable();
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_8_main_task
// Comments     : TEST #8: Testing _int_set_isr_data
//
//END---------------------------------------------------------------------

void tc_8_main_task(void)
{
    _mqx_uint vector;
    void   *old_isr_data;
    bool br;

    // printf("TEST # 8 - Testing _int_set_isr_data\n");
#if MQX_CHECK_ERRORS
    vector = kernel_data->FIRST_USER_ISR_VECTOR - 1;
    old_isr_data = _int_set_isr_data(vector, my_isr_data);
    /* Set isr data with too small vector number */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (_task_errno == MQX_INVALID_VECTORED_INTERRUPT), "Test #8 Testing: _int_set_isr_data with too small vector number should have returned MQX_INVALID_VECTORED_INTERRUPT")

    _task_set_error(MQX_OK);

    vector = kernel_data->LAST_USER_ISR_VECTOR + 1;
    old_isr_data = _int_set_isr_data(vector, my_isr_data);
    /* Set isr data with too large vector number */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (_task_errno == MQX_INVALID_VECTORED_INTERRUPT), "Test #8 Testing: _int_set_isr_data with too large vector number should have returned MQX_INVALID_VECTORED_INTERRUPT")

    _task_set_error(MQX_OK);
#endif

    vector = BSP_TIMER_INTERRUPT_VECTOR;
    old_isr_data = _int_set_isr_data(vector, my_isr_data);
    /* Set isr data with ok vector number */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (_task_errno == MQX_OK), "Test #8 Testing: _int_set_isr_data operation")

    _task_set_error(MQX_OK);

    br = (my_isr_data == kernel_data->INTERRUPT_TABLE_PTR[vector - kernel_data->FIRST_USER_ISR_VECTOR].APP_ISR_DATA);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (br == TRUE), "Test #8 Testing: check if isr data was set correctly")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_9_main_task
// Comments     : TEST #9: TEST # 9 - Testing _int_get_isr_data
//
//END---------------------------------------------------------------------

void tc_9_main_task(void)
{
    _mqx_uint vector;
    void   *old_isr_data;
    bool br;

    // printf("TEST # 9 - Testing _int_get_isr_data\n");
#if MQX_CHECK_ERRORS
    vector = kernel_data->FIRST_USER_ISR_VECTOR - 1;
    old_isr_data = _int_get_isr_data(vector);
    /* Get isr data with too small vector number */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_task_errno == MQX_INVALID_VECTORED_INTERRUPT), "Test #9 Testing: _int_get_isr_data with too small vector number should have returned MQX_INVALID_VECTORED_INTERRUPT")

    _task_set_error(MQX_OK);

    vector = kernel_data->LAST_USER_ISR_VECTOR + 1;
    old_isr_data = _int_get_isr_data(vector);
    /* Get isr data with too large vector number */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_task_errno == MQX_INVALID_VECTORED_INTERRUPT), "Test #9 Testing: _int_get_isr_data with too large vector number should have returned MQX_INVALID_VECTORED_INTERRUPT")

    _task_set_error(MQX_OK);
#endif

    vector = BSP_TIMER_INTERRUPT_VECTOR;
    old_isr_data = _int_get_isr_data(vector);
    /* Get isr data with ok vector number */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (_task_errno == MQX_OK), "Test #9 Testing: _int_get_isr_data operation")

    _task_set_error(MQX_OK);

    br = (my_isr_data == kernel_data->INTERRUPT_TABLE_PTR[vector - kernel_data->FIRST_USER_ISR_VECTOR].APP_ISR_DATA);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (br == TRUE), "Test #9 Testing: check isr data operation")
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_10_main_task
// Comments     : TEST #10: Testing _int_install_isr
//
//END---------------------------------------------------------------------

void tc_10_main_task(void)
{
    _mqx_uint vector;
    void (*old_isr)(void *);
    bool br;

    // printf("TEST # 10 - Testing _int_install_isr\n");
#if MQX_CHECK_ERRORS
    vector = kernel_data->FIRST_USER_ISR_VECTOR - 1;
    old_isr = _int_install_isr(vector, my_isr, my_isr_data);
    /* Get isr data with too small vector number */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (_task_errno == MQX_INVALID_VECTORED_INTERRUPT), "Test #10 Testing: _int_install_isr with too small vector number should have returned MQX_INVALID_VECTORED_INTERRUPT")

    _task_set_error(MQX_OK);

    vector = kernel_data->LAST_USER_ISR_VECTOR + 1;
    old_isr = _int_install_isr(vector, my_isr, my_isr_data);
    /* Get isr data with too larget vector number */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (_task_errno == MQX_INVALID_VECTORED_INTERRUPT), "Test #10 Testing: _int_install_isr with too large vector number should have returned MQX_INVALID_VECTORED_INTERRUPT")

    _task_set_error(MQX_OK);
#endif

    vector = BSP_TIMER_INTERRUPT_VECTOR;
    old_isr = _int_install_isr(vector, my_isr, my_isr_data);
    /* Get isr data with ok vector number */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (_task_errno == MQX_OK), "Test #10 Testing: _int_install_isr operation")

    _task_set_error(MQX_OK);

    br = (my_isr == kernel_data->INTERRUPT_TABLE_PTR[ vector - kernel_data->FIRST_USER_ISR_VECTOR].APP_ISR);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (br == TRUE), "Test #10 Testing: check if isr installed correctly")

    br = (my_isr_data == kernel_data->INTERRUPT_TABLE_PTR[vector - kernel_data->FIRST_USER_ISR_VECTOR].APP_ISR_DATA);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (br == TRUE), "Test #10 Testing: check if isr data installed correctly")

    //kernel_data->INTERRUPT_TABLE_PTR[vector - kernel_data->FIRST_USER_ISR_VECTOR].APP_ISR_DATA = 0;
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_int)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Testing _int_set_exception_handler"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Testing _int_get_exception_handler"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST #3 - Testing _int_get_default_isr"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST #4 - Testing _int_install_default_isr"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST #5 - Testing _int_install_exception_isr"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST #6 - Testing _int_install_unexpected_isr"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST #7 - Testing _int_get_isr"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST #8 - Testing _int_set_isr_data"),
    EU_TEST_CASE_ADD(tc_9_main_task, " TEST #9 - Testing _int_get_isr_data"),
    EU_TEST_CASE_ADD(tc_10_main_task, " TEST #10 - Testing _int_install_isr")
EU_TEST_SUITE_END(suite_int)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_int, " - Test of generic interrupt support functions")
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
    kernel_data = _mqx_get_kernel_data();
    _task_set_error(MQX_OK);
    _int_install_unexpected_isr();
  //test_initialize();
  //EU_RUN_ALL_TESTS(test_result_array);
  //test_stop();
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();	

} /* Endbody */

/* EOF */
