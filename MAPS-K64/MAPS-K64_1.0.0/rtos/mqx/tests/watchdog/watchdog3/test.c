/**HEADER********************************************************************
*
* Copyright (c) 2008-2013 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International
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
* $Version : 4.0.1.1$
* $Date    : Apr-8-2013$
*
* Comments   : This file contains the kernel test functions for the watchdog
*              component.
* Requirement: WATCHDOG001,WATCHDOG002,WATCHDOG003,WATCHDOG004,WATCHDOG005,WATCHDOG006,
*              WATCHDOG007,WATCHDOG008,WATCHDOG009,WATCHDOG010,WATCHDOG011,WATCHDOG012,
*              WATCHDOG013,WATCHDOG014,WATCHDOG016
*
*END***********************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <watchdog.h>
#include <watchdog_prv.h>

#include "EUnit.h"
#include "eunit_crit_secs.h"
#include "util.h"

#define MAIN_TASK         10
#define WATCHDOG_INVALID  WATCHDOG_VALID + 1

void           main_task(uint32_t);
void           Watchdog_expired(void *);
void           set_watchdog_component_valid_field(_mqx_uint );
void           set_watchdog_component_ptr(void *);
_mqx_uint      get_watchdog_component_valid_field( );
void          *get_watchdog_component_ptr( );

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    { MAIN_TASK, main_task, 1500,  8, "Main",  MQX_AUTO_START_TASK,  0},
    { 0,         0,         0,     0,       0,       0,              0}
};

bool    expired = FALSE;

void tc_1_main_task(void);/* TEST #1: Testing function _watchdog_create_component with failure cases */
void tc_2_main_task(void);/* TEST #2: Testing function _watchdog_test in special case */
void tc_3_main_task(void);/* TEST #3: Testing function _watchdog_start_ticks in special cases */
void tc_4_main_task(void);/* TEST #4: Testing function _watchdog_start in special cases */
void tc_5_main_task(void);/* TEST #5: Testing function _watchdog_stop in special cases */

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Testing function _watchdog_create_component with failure cases
* Requirement  : WATCHDOG001,WATCHDOG002,WATCHDOG003,WATCHDOG004
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint                       temp;
    _mqx_uint                       result;
    _mqx_uint                       invalid_interrupt_vector;
    KERNEL_DATA_STRUCT_PTR          kernel_data;
    MEMORY_ALLOC_INFO_STRUCT        memory_alloc_info;

    kernel_data = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);
    _task_set_error(MQX_OK);

    /* Create a watchdog component when the MQX cannot allocate memory for the watchdog component structures */
    result = _watchdog_create_component(BSP_TIMER_INTERRUPT_VECTOR, Watchdog_expired);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == MQX_OUT_OF_MEMORY , "Test #1 Testing: 1.00: Create a watchdog component with MQX memory error should have returned MQX_OUT_OF_MEMORY ");
    _task_set_error(MQX_OK);

    memory_free_all(&memory_alloc_info);

    /* Create a watchdog component when the expiry function is unavailable */
    result = _watchdog_create_component(BSP_TIMER_INTERRUPT_VECTOR, NULL);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == WATCHDOG_INVALID_ERROR_FUNCTION , "Test #1 Testing: 1.01: Create a watchdog component with expiry_function is NULL ");
    _task_set_error(MQX_OK);

    _int_disable();
    temp = kernel_data->IN_ISR;
    kernel_data->IN_ISR = 1;
    /*Calling this function in ISR*/
    result = _watchdog_create_component(BSP_TIMER_INTERRUPT_VECTOR, Watchdog_expired);

    kernel_data->IN_ISR = temp;
    _int_enable();

    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR , "Test #1 Testing: 1.02: Calling _watchdog_create_component in the ISR ");
    _task_set_error(MQX_OK);

    invalid_interrupt_vector = kernel_data->LAST_USER_ISR_VECTOR + 1;

    /* Create a watchdog component when MQX cannot install the periodic timer interrupt vector */
    result = _watchdog_create_component(invalid_interrupt_vector, Watchdog_expired);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == WATCHDOG_INVALID_INTERRUPT_VECTOR , "Test #1 Testing: 1.03: Create a watchdog component with MQX cannot install the periodic timer interrupt vector ");
    _task_set_error(MQX_OK);

    result = _watchdog_create_component(BSP_TIMER_INTERRUPT_VECTOR, Watchdog_expired);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK ,"Test #1 Testing: 1.04: Create watchdog component with all valid parameter ");

    /* check the watchdog component was created */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, kernel_data->KERNEL_COMPONENTS[KERNEL_WATCHDOG] != NULL ,"Test #1 Testing: 1.05: The watchdog component was created ");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Testing function _watchdog_test in special case
* Requirement  : WATCHDOG016
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _mqx_uint         temp;
    _mqx_uint         task_error_code;
    void             *watchdog_error_ptr, *watchdog_table_error_ptr;

    /* Save the VALID field value */
    temp = (_mqx_uint)get_watchdog_component_valid_field();
    /* Makes the watchdog component to invalid */
    set_watchdog_component_valid_field(WATCHDOG_INVALID);

    /* Test function watchdog_test with watchdog component is invalid*/
    task_error_code = _watchdog_test(&watchdog_error_ptr, &watchdog_table_error_ptr);

    /* restore watchdog valid field to original value */
    set_watchdog_component_valid_field(temp);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, task_error_code == MQX_INVALID_COMPONENT_BASE, "Test #2 Testing: 2.00: Test function _watchdog_test when watchdog component is not valid ");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Testing _watchdog_start_ticks in special cases
* Requirement  : WATCHDOG008,WATCHDOG009,WATCHDOG010,WATCHDOG011
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    bool            result;
    _mqx_uint          temp;
    MQX_TICK_STRUCT    ticks;
    TD_STRUCT_PTR      td_ptr;
    void              *temp_ptr;

    td_ptr = _task_get_td( _task_get_id() );

    /* Clear WATCHDOG flags before test */
    _watchdog_cleanup(td_ptr);

    /* Initialize for variable ticks */
    _time_init_ticks(&ticks, 4);

    /* The watchdog components was previously created
     * so we need to zero it before doing this test */

    /* Save pointer to watchdog component */
    temp_ptr = get_watchdog_component_ptr();
    /* test for the watchdog component does not exist */
    set_watchdog_component_ptr(NULL);

    /*start watchdog but it was not created*/
    result = _watchdog_start_ticks(&ticks);

    /* restore kernel watchdog component to original value */
    set_watchdog_component_ptr(temp_ptr);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, (!result) , "Test #3 Testing: 3.00: Test function _watchdog_start_ticks with watchdog component was not previously created ");
    _task_set_error(MQX_OK);

    /* Save the VALID field value */
    temp = (_mqx_uint)get_watchdog_component_valid_field();
    /* Makes the watchdog component to invalid */
    set_watchdog_component_valid_field(WATCHDOG_INVALID);

    /* starts watchdog with watchdog component is invalid*/
    result = _watchdog_start_ticks(&ticks);

    /* restore watchdog valid field to original value */
    set_watchdog_component_valid_field(temp);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, (!result) , "Test #3 Testing: 3.01: Test function _watchdog_start_ticks with watchdog component data is not valid ");

    /* start watchdog in normal operation */
    result = _watchdog_start_ticks(&ticks);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result , "Test #3 Testing: 3.02: Test function _watchdog_start_ticks in normal operation ");
    _task_set_error(MQX_OK);

    /* stop watchdog */
    result = _watchdog_stop();
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result , "Test #3 Testing: 3.03: Test function _watchdog_start_ticks in normal operation ");
    _task_set_error(MQX_OK);

    /* Clear WATCHDOG flags */
    _watchdog_cleanup(td_ptr);

}
/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Testing _watchdog_start in special cases
* Requirement  : WATCHDOG005,WATCHDOG006,WATCHDOG007,WATCHDOG011
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    bool               result;
    _mqx_uint             temp;
    TD_STRUCT_PTR         td_ptr;
    void                 *temp_ptr;

    td_ptr = _task_get_td( _task_get_id() );

    /* Clear WATCHDOG flags before test */
    _watchdog_cleanup(td_ptr);

    /* The watchdog components was previously created
     * so we need to zero it before doing this test */

    /* Save pointer to watchdog component */
    temp_ptr = get_watchdog_component_ptr();
    /* test for the watchdog component does not exist */
    set_watchdog_component_ptr(NULL);

    /*start watchdog but it was not created*/
    result = _watchdog_start(30);

    /* restore kernel watchdog component to original value */
    set_watchdog_component_ptr(temp_ptr);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, (!result) , "Test #4 Testing: 4.00: Test function _watchdog_start with watchdog component was not previously created ");
    _task_set_error(MQX_OK);

    /* Save the VALID field value */
    temp = (_mqx_uint)get_watchdog_component_valid_field();
    /* Makes the watchdog component to invalid */
    set_watchdog_component_valid_field(WATCHDOG_INVALID);

    /* starts watchdog when the watchdog component is invalid*/
    result = _watchdog_start(30);

    /* restore watchdog valid field to original value */
    set_watchdog_component_valid_field(temp);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, (!result) , "Test #4 Testing: 4.01: Test function _watchdog_start with watchdog component data is not valid ");
    _task_set_error(MQX_OK);

    /* start watchdog in normal operation */
    result = _watchdog_start(30);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result , "Test #4 Testing: 4.02: Test function _watchdog_start in normal operation ");
    _task_set_error(MQX_OK);

    /* stop watchdog */
    result = _watchdog_stop();
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result , "Test #4 Testing: 4.03: Test function _watchdog_start in normal operation ");
    _task_set_error(MQX_OK);

    /* Clear WATCHDOG flags */
    _watchdog_cleanup(td_ptr);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Testing _watchdog_stop in special cases
* Requirement  : WATCHDOG012,WATCHDOG013,WATCHDOG014
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    bool          result;
    _mqx_uint        temp;
    void            *temp_ptr;
    TD_STRUCT_PTR    td_ptr;

    td_ptr = _task_get_td( _task_get_id() );

    /* Clear WATCHDOG flags before test */
    _watchdog_cleanup(td_ptr);

    _int_disable();
    /* The watchdog components was previously created
     * so we need to zero it before doing this test */

    /* Save pointer to watchdog component */
    temp_ptr = get_watchdog_component_ptr();
    /* test for the watchdog component does not exist */
    set_watchdog_component_ptr(NULL);

    /*stops watchdog but the watchdog compenent was not created*/
    result = _watchdog_stop();

    /* restore kernel watchdog component to original value */
    set_watchdog_component_ptr(temp_ptr);

    _int_enable();

    EU_ASSERT_REF_TC_MSG( tc_5_main_task, (!result) , "Test #5 Testing: 5.00: Test function _watchdog_stop with watchdog component was not previously created ");
    _task_set_error(MQX_OK);

    /* Save the VALID field value */
    temp = (_mqx_uint)get_watchdog_component_valid_field();
    /* Makes the watchdog component to invalid */
    set_watchdog_component_valid_field(WATCHDOG_INVALID);

    /* stops watchdog with watchdog component is invalid*/
    result = _watchdog_stop();

    /* restore watchdog valid field to original value */
    set_watchdog_component_valid_field(temp);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, (!result) , "Test #5 Testing: 5.01: Test function _watchdog_stop with watchdog component data is not valid ");
    _task_set_error(MQX_OK);

    /* stops watchdog when it was not started */
    result = _watchdog_stop();
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, (!result) , "Test #5 Testing: 5.02: Test function _watchdog_stop when watchdog was not started ");
    _task_set_error(MQX_OK);

    /* Clear WATCHDOG flags */
    _watchdog_cleanup(td_ptr);
}

 /******  Define Test Suite      *******/

 EU_TEST_SUITE_BEGIN(suite_watchdog3)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1: Testing _watchdog_create_component with failure cases "),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2: Testing function _watchdog_test in special case "),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3: Testing function _watchdog_start_ticks in special cases "),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4: Testing function _watchdog_start in special cases "),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST # 5: Testing function _watchdog_stop in special cases "),
 EU_TEST_SUITE_END(suite_watchdog3)

/*******  Add test suites */
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_watchdog3, " Test of watchdog component")
 EU_TEST_REGISTRY_END()

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   starts up the watchdog and tests it.
*
*END*----------------------------------------------------------------------*/
void main_task
   (
      uint32_t param
   )
{/* Body */

   _int_install_unexpected_isr();
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();

} /* Endbody */

 /*FUNCTION*-----------------------------------------------------------------
 *
 * Function Name : Watchdog_expired
 * Return Value  : void
 * Comments      :
 *   Print out the watchdog expiry information
 *
 *END*----------------------------------------------------------------------*/
void Watchdog_expired
   (
       void   *td_ptr
   )
{ /* Body */
    expired = TRUE;
} /* Endbody */

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_watchdog_component_valid_field
* Return Value  : void
* Comments      : Gets the VALID field value of watchdog component structure
*
*END*----------------------------------------------------------------------*/
_mqx_uint get_watchdog_component_valid_field
   (
      void
   )
{
    KERNEL_DATA_STRUCT_PTR        kernel_data;
    WATCHDOG_COMPONENT_STRUCT_PTR watchdog_component_ptr;

    _GET_KERNEL_DATA(kernel_data);
    watchdog_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_WATCHDOG];
    return watchdog_component_ptr->VALID ;

}
/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_watchdog_component_valid_field
* Return Value  : void
* Comments      : set the VALID field value of watchdog component structure
*
*END*----------------------------------------------------------------------*/
void set_watchdog_component_valid_field
   (
      _mqx_uint wat_valid
   )
{
    KERNEL_DATA_STRUCT_PTR        kernel_data;
    WATCHDOG_COMPONENT_STRUCT_PTR watchdog_component_ptr;

    _GET_KERNEL_DATA(kernel_data);

    /* Disable all interrupt */
    _int_disable();
    watchdog_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_WATCHDOG];
    watchdog_component_ptr->VALID = wat_valid;

    /* Enable all interrupt */
    _int_enable();
}
/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_watchdog_component_ptr
* Return Value  : void
* Comments      : gets the pointer to kernel component
*
*END*----------------------------------------------------------------------*/
void *get_watchdog_component_ptr
   (
      void
   )
{
    KERNEL_DATA_STRUCT_PTR        kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    return  (void *)kernel_data->KERNEL_COMPONENTS[KERNEL_WATCHDOG];

}
/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_watchdog_component_ptr
* Return Value  : void
* Comments      : set the kernel component pointer.
*
*END*----------------------------------------------------------------------*/
void set_watchdog_component_ptr
   (
      void   *wat_ptr
   )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    /* Disable all interrupt */
    _int_disable();

    kernel_data->KERNEL_COMPONENTS[KERNEL_WATCHDOG] = (WATCHDOG_COMPONENT_STRUCT_PTR)wat_ptr;

    /* Enable all interrupt */
    _int_enable();
}
/* EOF */
