/**HEADER********************************************************************
*
* Copyright (c) 2008-2013 Freescale Semiconductor;
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
* $Version : 4.0.1.1$
* $Date    : Mar-27-2013$
*
* Comments   : This file contains the source for the task module testing.
* Requirement:
* TASK006,TASK019,TASK026,TASK027,TASK028,TASK029,TASK032,TASK033,TASK034,TASK035,
* TASK036,TASK037,TASK038,TASK039,TASK045,TASK048,TASK046,TASK049,TASK050,TASK052,
* TASK054,TASK056,TASK057,TASK058,TASK064,TASK065,TASK043,TASK044,TASK062,TASK063,
* TASK042,TASK061,TASK005,TASK060,TASK067,TASK068,TASK070
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <mqx_prv.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define MAIN_TASK        1
#define TEST_TASK        2
#define XCPT_TEST_TASK   3

#define MAIN_PRIO        7
#define TEST_PRIO        8
#define HIGHER_MAIN_PRIO 6

#define INVALID_TID          (_task_id)(-1)
#define VALID_PARAMETER      0xCAFE
#define NEW_VALID_PARAMETER  0xDEAD
#define TEST_ENVIRONMENT     (void *)0xABDC

void    main_task(uint32_t);
void    test_task(uint32_t);
void    xcpt_test_task(uint32_t);
void    exception_handler(_mqx_uint, void *);
void    exit_handler(void);
uint32_t inc_function(volatile uint32_t *);
void    test_stack_overflow(uint32_t);

/*------------------------------------------------------------------------*/
/* Test suite function prototype*/
void tc_1_main_task(void);/* TEST #1: Testing functions _task_get_environment,_task_set_environment             */
void tc_2_main_task(void);/* TEST #2: Testing functions _task_set_exception_handler,_task_get_exception_handler */
void tc_3_main_task(void);/* TEST #3: Test functions _task_set_exit_handler,_task_get_exit_handler              */
void tc_4_main_task(void);/* TEST #4: Testing functions _task_get_parameter,_task_set_parameter                 */
void tc_5_main_task(void);/* TEST #5: Testing functions _task_set_parameter_for,_task_get_parameter_for         */
void tc_6_main_task(void);/* TEST #6: Testing functions _task_get_priority,_task_set_priority                   */
void tc_7_main_task(void);/* TEST #7: Testing function _task_get_template_ptr                                   */
void tc_8_main_task(void);/* TEST #8: Testing function _task_get_index_from_id                                  */
void tc_9_main_task(void);/* TEST #9: Test function _task_get_template_index                                    */
void tc_10_main_task(void);/*TEST #10: Testing function _task_get_id_from_name with task name is not valid      */
void tc_11_main_task(void);/*TEST #11: Test function _task_get_td when task id is not valid                     */
void tc_12_main_task(void);/*TEST #12: Testing the function _task_check_stack                                   */
void tc_13_main_task(void);/*TEST #13: Testing function _task_ready in special cases                            */
void tc_14_main_task(void);/*TEST #14: Testing function _task_restart in ISR                                    */
/*------------------------------------------------------------------------*/
TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,      main_task,      1500, MAIN_PRIO,     "main",      MQX_AUTO_START_TASK},
   {TEST_TASK,      test_task,      1500, TEST_PRIO,     "test",      0},
   {XCPT_TEST_TASK, xcpt_test_task, 1500, TEST_PRIO,     "exception", 0},
   {0,              0,              0,    0,             0,           0,                  }
};

bool     is_task_running;
bool     is_task_aborted;
bool     is_exception_called;
bool     is_stack_overflow;
_mqx_uint   first_isr_vector;
_task_id    main_task_id;
uint32_t    stack_limit;
uint32_t    test_task_param, test_task_param1;

/*FUNCTION*-----------------------------------------------------
*
* Function Name : exception_handler
* Comments      : Sets global variable is_exception_called to verify
*                 function exception_handle is called
*END*-----------------------------------------------------*/
void exception_handler(_mqx_uint number, void *param)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;

    kernel_data = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();

    /* set exception called flag */
    is_exception_called = TRUE;

    /* restore FIRST_USER_ISR_VECTOR to original value */
    kernel_data->FIRST_USER_ISR_VECTOR = first_isr_vector;

    /* make the main task to be ready */
    _task_ready( _task_get_td(main_task_id) );
}

/*FUNCTION*-----------------------------------------------------
*
* Function Name : exit_handler
* Comments      : Sets the global variable is_task_aborted to verify
*                 the function exit_handle is called
*END*-----------------------------------------------------*/
void exit_handler(void)
{
    is_task_aborted = TRUE;
}

/*FUNCTION*-----------------------------------------------------
*
* Function Name : inc_function
* Comments      : this function is only preventing GCC from removing 'param' in test_stack_overflow
*
*END*-----------------------------------------------------*/
uint32_t inc_function(volatile uint32_t *in)
{
    volatile uint32_t val = 0;
    val = (*in+1);
    return val;
}
/*FUNCTION*-----------------------------------------------------
*
* Function Name : test_stack_overflow
* Comments      :
*
*END*-----------------------------------------------------*/
void test_stack_overflow(uint32_t depth)
{
    volatile uint32_t param;

    param = depth;
    param = inc_function(&param);
#if PSP_STACK_GROWS_TO_LOWER_MEM
    if ((uint32_t)&param < stack_limit)
#else
    if ((uint32_t)&param > stack_limit)
#endif
    {
        is_stack_overflow = _task_check_stack();
    }
    else test_stack_overflow(param);
}

/*FUNCTION*-----------------------------------------------------
*
* Function Name : test_task
* Comments      : The function gets the task creation parameter of TEST_TASK
*
*END*-----------------------------------------------------*/
void test_task(uint32_t initial_data)
{
    is_task_running = TRUE;
    /* Sets the task creation parameter of the TEST_TASK and old parameter is stored in variable test_task_param */
    test_task_param = _task_set_parameter(NEW_VALID_PARAMETER);
    /* Sets the task creation parameter of the TEST_TASK */
    test_task_param1 = _task_get_parameter();

    _task_block();
}

/*FUNCTION*-----------------------------------------------------
*
* Function Name : xcpt_test_task
* Comments      : The function intalls exception ISR and waits
*                 exception interrupts occur
*END*-----------------------------------------------------*/
void xcpt_test_task(uint32_t data)
{
    KERNEL_DATA_STRUCT_PTR  kernel_data;
    MQX_TICK_STRUCT         start_ticks, end_ticks;
    uint32_t                 duration_ticks;

    kernel_data = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();

    /* set exception isr as default interrupt subroutine */
    _int_install_exception_isr();

    /* backup the FIRST_USER_ISR_VECTOR */
    first_isr_vector = kernel_data->FIRST_USER_ISR_VECTOR;

    /* make the bsp timer interrupt to be an exception interrupts */
    kernel_data->FIRST_USER_ISR_VECTOR = BSP_TIMER_INTERRUPT_VECTOR + 1;

    _time_get_ticks(&start_ticks);
    duration_ticks = 0;

    /* wait for a tick interrupt */
    while (duration_ticks == 0)
    {
        _time_get_ticks(&end_ticks);
        duration_ticks = end_ticks.TICKS[0] - start_ticks.TICKS[0];
    }

    /* ensure the main task to be ready in case the exception handler function was not called */
    _task_ready( _task_get_td(main_task_id) );

}
/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Testing functions _task_get_environment and _task_set_environment
* Requirement  : TASK006,TASK019,TASK026,TASK027,TASK028,TASK029
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _task_id       test_tid;
    _mqx_uint      result;
    TD_STRUCT_PTR  td_ptr;

    /* Test function _task_get_environment when task id is not valid */
    _task_get_environment(INVALID_TID);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == MQX_INVALID_TASK_ID , "Test #1: Testing 1.00: Tests function _task_get_environment with invalid task ID");
    _task_set_error(MQX_OK);

    /* Test function _task_set_environment when task id is not valid*/
    _task_set_environment(INVALID_TID, TEST_ENVIRONMENT);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == MQX_INVALID_TASK_ID , "Test #1: Testing 1.01: Tests function _task_set_environment with invalid task ID");
    _task_set_error(MQX_OK);

    /* Creates TEST_TASK */
    test_tid = _task_create(0, TEST_TASK,0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, test_tid != MQX_NULL_TASK_ID , "Test #1: Testing 1.02: Create a task in normal operation will be successful ");

    td_ptr = _task_get_td(test_tid);

    /* Sets the address of the application-specific environment data for TEST_TASK */
    _task_set_environment(test_tid, TEST_ENVIRONMENT);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, td_ptr->ENVIRONMENT_PTR == TEST_ENVIRONMENT , "Test #1: Testing 1.03: Tests function _task_set_environment  ");

     /* Gets the address of the application-specific environment data for TEST_TASK */
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_get_environment(test_tid) == TEST_ENVIRONMENT , "Test #1: Testing 1.04: Tests function _task_get_environment  ");

    /* Destroy TEST_TASK */
    result = _task_destroy(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == MQX_OK , "Test #1: Testing 1.05: Destroys TEST_TASK that was create by main task ");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Testing functions _task_set_exception_handler and
*                _task_get_exception_handler
* Requirement  : TASK006,TASK019,TASK032,TASK033,TASK034,TASK035
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _task_id       test_tid;
    _mqx_uint      result;
    TD_STRUCT_PTR  td_ptr;

    /* Sets a pointer to the task exception handler when task id is not valid */
    _task_set_exception_handler(INVALID_TID, exception_handler);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, _task_errno == MQX_INVALID_TASK_ID , "Test #2: Testing 2.00: Tests function _task_set_exception_handler with invalid task ID");
    _task_set_error(MQX_OK);

    /* Gets a pointer to the task exception handler when task id is not valid */
    _task_get_exception_handler(INVALID_TID);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, _task_errno == MQX_INVALID_TASK_ID , "Test #2: Testing 2.01: Tests function _task_get_exception_handler with invalid task ID");
    _task_set_error(MQX_OK);

    /* Creates XCPT_TEST_TASK that install exception ISR */
    test_tid = _task_create(0, XCPT_TEST_TASK, 0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, test_tid != MQX_NULL_TASK_ID , "Test #2: Testing 2.02: Create a task in normal operation will be successful ");

    td_ptr = _task_get_td(test_tid);

    /* Sets a pointer to the task exception handler for XCPT_TEST_TASK */
    _task_set_exception_handler(test_tid, exception_handler);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, td_ptr->EXCEPTION_HANDLER_PTR == exception_handler , "Test #2: Testing 2.03: Tests function _task_set_exception_handler ");

    /* Gets a pointer to the task exception handler of XCPT_TEST_TASK */
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, _task_get_exception_handler(test_tid) == exception_handler , "Test #2: Testing 2.04: Tests function _task_get_exception_handler ");

    /* Clear exception called flag */
    is_exception_called = FALSE;

    /* block the main task to allow xcpt_test_task run */
    _task_block();

    /* destroy xcpt_test_task task */
    result = _task_destroy(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_OK , "Test #2: Testing 2.05: Destroys TEST_TASK that was create by main task ");

    /* Checks the function exception handle is called */
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, is_exception_called, "Test #2: Testing 2.06: The exception handler function should be called ");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Test functions _task_set_exit_handler,_task_get_exit_handler
* Requirement  : TASK006,TASK036,TASK037,TASK038,TASK039
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    _task_id       test_tid;
    _mqx_uint      result;
    TD_STRUCT_PTR  td_ptr;

    /* Sets the pointer to the task exit handler for the task when task id is not valid */
    _task_set_exit_handler(INVALID_TID, exit_handler);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, _task_errno == MQX_INVALID_TASK_ID , "Test #3: Testing 3.00: Tests function _task_set_exit_handler with invalid task ID");
    _task_set_error(MQX_OK);

    /* Gets the pointer to the task exit handler for the task when task id is not valid */
    _task_get_exit_handler(INVALID_TID);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, _task_errno == MQX_INVALID_TASK_ID , "Test #3: Testing 3.01: Tests function _task_get_exit_handler with invalid task ID");
    _task_set_error(MQX_OK);

    /* Creates TEST_TASK */
    test_tid = _task_create(0, TEST_TASK, 0);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, test_tid != MQX_NULL_TASK_ID , "Test #3: Testing 3.02: Create a task in normal operation will be successful ");

    td_ptr = _task_get_td(test_tid);

    /* Sets the pointer to the task exit handler for TEST_TASK */
    _task_set_exit_handler(test_tid, exit_handler);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, td_ptr->EXIT_HANDLER_PTR == exit_handler , "Test #3: Testing 3.03: Tests function _task_set_exit_handler ");

    /* Gets the pointer to the task exit handler of TEST_TASK */
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, _task_get_exit_handler(test_tid) == exit_handler , "Test #3: Testing 3.04: Tests function _task_get_exit_handler ");

    is_task_aborted = FALSE;
    /* aborts TEST_TASK */
    result = _task_abort(test_tid);

    _time_delay_ticks(1);
    /* Check function exit handle is called */
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, (result == MQX_OK) && (is_task_aborted) , "Test #3: Testing 3.05: Tests function _task_abort and _task_set_exit_handler  ");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Testing functions _task_get_parameter/_task_set_parameter
* Requirement  : TASK006,TASK019,TASK045,TASK048
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    _task_id       test_tid;
    _mqx_uint      result;

    /* Create TEST_TASK */
    test_tid = _task_create(0, TEST_TASK, VALID_PARAMETER);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, test_tid != MQX_NULL_TASK_ID , "Test #4: Testing 4.00: Create a task in normal operation will be successful ");

    is_task_running = FALSE;
    /* suspend main task and allows TEST_TASK run */
    _time_delay_ticks(1);

    /* Test function _task_set_parameter that is called by TEST_TASK */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, is_task_running && (test_task_param == VALID_PARAMETER) , "Test #4: Testing 4.01: Tests function _task_set_parameter ");

    /* Test function _task_get_parameter that was called by TEST_TASK */
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, test_task_param1 == NEW_VALID_PARAMETER , "Test #4: Testing 4.02: Tests function _task_get_parameter function");

    /* Destroy TEST_TASK */
    result = _task_destroy(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_OK , "Test #4: Testing 4.03: Destroys TEST_TASK that was create by main task ");

}
/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Testing functions _task_set_parameter_for/_task_get_parameter_for
* Requirement  : TASK006,TASK019,TASK046,TASK049,TASK050
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    _task_id       test_tid;
    _mqx_uint      result;

    /* Create TEST_TASK */
    test_tid = _task_create(0, TEST_TASK, VALID_PARAMETER);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, test_tid != MQX_NULL_TASK_ID , "Test #5: Testing 5.00: Create a task in normal operation will be successful ");

    /* Sets the task creation parameter of the TEST_TASK */
    result = _task_set_parameter_for(NEW_VALID_PARAMETER, test_tid);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == VALID_PARAMETER , "Test #5: Testing 5.01: Tests function _task_set_parameter_for ");

    /* Gets the task creation parameter of the TEST_TASK */
    result = _task_get_parameter_for(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == NEW_VALID_PARAMETER , "Test #5: Testing 5.02: Tests function _task_get_parameter_for ");

    result = _task_destroy(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == MQX_OK , "Test #5: Testing 5.03: Destroys TEST_TASK that was create by main task ");
    /* Sets the task creation parameter of the task when task id is not valid */
    result = _task_set_parameter_for(NEW_VALID_PARAMETER, test_tid);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == 0 , "Test #5: Testing 5.04: Tests function _task_set_parameter_for with task id is not valid ");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Testing functions _task_get_priority/_task_set_priority
* Requirement  : TASK052,TASK054,TASK056,TASK057,TASK058
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    _mqx_uint                result;
    _mqx_uint                invalid_prio, prio;
    KERNEL_DATA_STRUCT_PTR   kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    /* Gets priority of the task when task id is not valid */
    result = _task_get_priority(INVALID_TID, &prio);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_INVALID_TASK_ID , "Test #6: Testing 6.00: Tests function _task_get_priority with invalid task ID");

    /* Sets priority of the task when task id is not valid */
    result = _task_set_priority(INVALID_TID, MAIN_PRIO, &prio);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_INVALID_TASK_ID , "Test #6: Testing 6.01: Tests function _task_set_priority with invalid task ID");

    invalid_prio = kernel_data->LOWEST_TASK_PRIORITY + 1 ;

    /* Sets priority of the task when new priority is not valid */
    result = _task_set_priority(_task_get_id(),invalid_prio ,&prio);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_INVALID_PARAMETER , "Test #6: Testing 6.02: Tests function _task_set_priority with invalid priority ");

    /* Gets priority of the main task */
    result = _task_get_priority(MQX_NULL_TASK_ID, &prio);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, prio == MAIN_PRIO , "Test #6: Testing 6.03: Tests function _task_get_priority ");

    /* Sets priority of the main task to HIGHER_MAIN_PRIO */
    result = _task_set_priority(MQX_NULL_TASK_ID, HIGHER_MAIN_PRIO , &prio);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_OK , "Test #6: Testing 6.04: Tests function _task_set_priority ");

    /* Gets priority of the main task */
    result = _task_get_priority(MQX_NULL_TASK_ID, &prio);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, prio == HIGHER_MAIN_PRIO , "Test #6: Testing 6.05: Tests function _task_get_priority ");

}


/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7: Testing function _task_get_template_ptr
* Requirement  : TASK006,TASK019,TASK064,TASK065
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    _mqx_uint                result;
    _task_id                 test_tid;
    TD_STRUCT_PTR            td_ptr;
    TASK_TEMPLATE_STRUCT_PTR tmp_ptr;

    /* Create TEST_TASK */
    test_tid = _task_create(0, TEST_TASK, 0);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, test_tid != MQX_NULL_TASK_ID , "Test #7: Testing 7.00: Create a task in normal operation will be successful ");

    td_ptr = _task_get_td(test_tid);
    /* Gets the pointer to the task template for the task ID */
    tmp_ptr = _task_get_template_ptr(test_tid);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, tmp_ptr == td_ptr->TASK_TEMPLATE_PTR , "Test #7: Testing 7.01: Tests function _task_get_template_ptr ");

    /* Destroy TEST_TASK */
    result = _task_destroy(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_OK , "Test #7: Testing 7.02: Destroys TEST_TASK that was create by main task ");

    /* Test function _task_get_template_ptr when task id is not valid */
    tmp_ptr = _task_get_template_ptr(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, tmp_ptr == NULL , "Test #7: Testing 7.03: Tests function _task_get_template_ptr with invalid task ID");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8: Testing function _task_get_index_from_id
* Requirement  : TASK006,TASK019,TASK043,TASK044
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
    _task_id                 test_tid;
    _mqx_uint                result;
    TASK_TEMPLATE_STRUCT_PTR tmp_ptr;

    test_tid = _task_create(0, TEST_TASK, 0);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, test_tid != MQX_NULL_TASK_ID , "Test #8: Testing 8.00: Create a task in normal operation will be successful ");

    /* Gets the pointer to the task template for the task ID */
    tmp_ptr = _task_get_template_ptr(test_tid);

    /* Gets the task template index for the task ID */
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, _task_get_index_from_id(test_tid) == tmp_ptr->TASK_TEMPLATE_INDEX , "Test #8: Testing 8.01: Tests function _task_get_index_from_id ");

    result = _task_destroy(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, result == MQX_OK , "Test #8: Testing 8.02: Destroys TEST_TASK that was create by main task ");

    /* Test function _task_get_index_from_id when task id is not valid */
    result = _task_get_index_from_id(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, result == 0 , "Test #8: Testing 8.03: Tests _task_get_index_from_id with invalid task ID");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : TEST #9: Test function _task_get_template_index
* Requirement  : TASK006,TASK019,TASK062,TASK063
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
    _task_id                 test_tid;
    _mqx_uint                result;
    TASK_TEMPLATE_STRUCT_PTR tmp_ptr;

    /* Test function _task_get_template_index with task name is not valid */
    result = _task_get_template_index("invalid");
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_NULL_TASK_ID , "Test #9: Testing 9.00: Test _task_get_template_index with invalid name");
    _task_set_error(MQX_OK);

    test_tid = _task_create(0, TEST_TASK, 0);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, test_tid != MQX_NULL_TASK_ID , "Test #9: Testing 9.01: Create a task in normal operation will be successful ");

    /* Gets the pointer to the task template for the task ID */
    tmp_ptr = _task_get_template_ptr(test_tid);

    /* Gets the task ID that is associated with the task name */
    result = _task_get_template_index(tmp_ptr->TASK_NAME);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == tmp_ptr->TASK_TEMPLATE_INDEX , "Test #9: Testing 9.02: Tests function _task_get_template_index ");

    /* Destroy TEST_TASK */
    result = _task_destroy(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_OK , "Test #9: Testing 9.03: Destroys TEST_TASK that was create by main task ");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_10_main_task
* Comments     : TEST #10: Testing function _task_get_id_from_name with task name is not valid
* Requirement  : TASK042
*END*---------------------------------------------------------------------*/
void tc_10_main_task(void)
{
    _task_id                 test_tid;
    _mqx_uint                result;

     /* Test function _task_get_id_from_name with task name is not valid */
    test_tid = _task_get_id_from_name("invalid");
    EU_ASSERT_REF_TC_MSG( tc_10_main_task, test_tid == MQX_NULL_TASK_ID , "Test #10: Testing 10.00: Tests function _task_get_id_from_name with invalid name");
    _task_set_error(MQX_OK);

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_11_main_task
* Comments     : TEST #11: Test function _task_get_td when task id is not valid
* Requirement  : TASK061
*END*---------------------------------------------------------------------*/
void tc_11_main_task(void)
{
    TD_STRUCT_PTR            td_ptr;

    /* Test function _task_get_td when task id is not valid */
    td_ptr = _task_get_td(INVALID_TID);
    EU_ASSERT_REF_TC_MSG( tc_11_main_task, td_ptr == NULL , "Test #11: Testing 11.00: Tests function _task_get_td with task ID is not valid");
    _task_set_error(MQX_OK);

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_12_main_task
* Comments     : TEST #12: Testing the function _task_check_stack
* Requirement  : TASK005,TASK060
*END*---------------------------------------------------------------------*/
void tc_12_main_task(void)
{
    TD_STRUCT_PTR           td_ptr;

    /* Gets a pointer to the task descriptor for the main task */
    td_ptr = (TD_STRUCT_PTR) _task_get_td(_task_get_id());
    EU_ASSERT_REF_TC_MSG( tc_12_main_task, td_ptr != NULL , "Test #12: Testing 12.00: Tests function _task_get_td ");

    /* Store stack limitation of current task into global stack_limit */
    stack_limit = (uint32_t)td_ptr->STACK_LIMIT;

    /* Clear flag */
    is_stack_overflow = FALSE;

    /* disable interrupts to ensure no exception interrupt occurred because of stack overflow */
    _int_disable();

    test_stack_overflow(0);

    _int_enable();

    EU_ASSERT_REF_TC_MSG( tc_12_main_task, is_stack_overflow , "Test #12: Testing 12.01: Tests function _task_check_stack ");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_13_main_task
* Comments     : TEST #13: Testing function _task_ready in special cases
* Requirement  : TASK006,TASK019,TASK067,TASK068
*END*---------------------------------------------------------------------*/
void tc_13_main_task(void)
{
    _task_id       test_tid;
    _mqx_uint      result;
    TD_STRUCT_PTR  td_ptr = NULL;

    /* Test function _task_ready when pointer to the task descriptor of the task is NULL*/
    _task_ready(td_ptr);
    EU_ASSERT_REF_TC_MSG( tc_13_main_task, _task_errno == MQX_INVALID_TASK_ID , "Test #13: Testing 13.00: Tests function _task_ready with the NULL task descriptor");
    _task_set_error(MQX_OK);

    /* Create TEST_TASK */
    test_tid = _task_create(0, TEST_TASK, 0);
    EU_ASSERT_REF_TC_MSG( tc_13_main_task, test_tid != MQX_NULL_TASK_ID , "Test #13: Testing 13.01: Create a task in normal operation will be successful ");

    td_ptr = _task_get_td(test_tid);

    /* Test function _task_ready when the task is already in its ready queue */
    _task_ready(td_ptr);
    EU_ASSERT_REF_TC_MSG( tc_13_main_task, _task_errno == MQX_INVALID_TASK_STATE , "Test #13: Testing 13.02: Fail to makes the task ready when the task that are currently in ready state  ");
    _task_set_error(MQX_OK);

    /* Abort TEST_TASK */
    result = _task_abort(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_13_main_task, result == MQX_OK , "Test #13: Testing 13.03: Aborts TEST_TASK that was create by main task ");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_14_main_task
* Comments     : TEST #14: Testing function _task_restart in ISR
* Requirement  : TASK006,TASK019,TASK070
*END*---------------------------------------------------------------------*/
void tc_14_main_task(void)
{
    _task_id                   test_tid;
    _mqx_uint                  result;
    _mqx_uint                  temp;
    KERNEL_DATA_STRUCT_PTR     kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    /* Create TEST_TASK */
    test_tid = _task_create(0, TEST_TASK, 0);
    EU_ASSERT_REF_TC_MSG( tc_14_main_task, test_tid != MQX_NULL_TASK_ID , "Test #14: Testing 14.00: Create a task in normal operation will be successful ");

    _int_disable();
    /* Save the current isr counting value */
    temp = kernel_data->IN_ISR;
    /* Set the current task as an isr */
    kernel_data->IN_ISR = 1;

    /*test _task_restart function cannot be called from an ISR*/
    result = _task_restart(test_tid, NULL, TRUE); /*test _task_restart function cannot be called from an ISR*/

    /* Restore the isr counting value */
    kernel_data->IN_ISR = temp;
    /* Enable all interrupt */
    _int_enable();
    EU_ASSERT_REF_TC_MSG( tc_14_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR , "Test #14: Testing 14.01: Calling _task_reatart in ISR should return error");
    _task_set_error(MQX_OK);

    /* Destroy TEST_TASK */
    result = _task_destroy(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_14_main_task, result == MQX_OK , "Test #14: Testing 14.02: Destroys TEST_TASK that was create by main task ");
}

/******************** End Testcases ********************/

/*------------------------------------------------------------------------*/
/* Define Test Suite*/
 EU_TEST_SUITE_BEGIN(task_task)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Testing functions _task_get_environment,_task_set_environment"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Testing functions _task_set_exception_handler,_task_get_exception_handler"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST #3 - Test functions _task_set_exit_handler,_task_get_exit_handler"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST #4 - Testing functions _task_get_parameter,_task_set_parameter"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST #5 - Testing functions _task_set_parameter_for,_task_get_parameter_for"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST #6 - Testing functions _task_get_priority,_task_set_priority"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST #7 - Testing function _task_get_template_ptr"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST #8 - Testing function _task_get_index_from_id"),
    EU_TEST_CASE_ADD(tc_9_main_task, " TEST #9 - Test function _task_get_template_index"),
    EU_TEST_CASE_ADD(tc_10_main_task, " TEST #10 - Testing function _task_get_id_from_name with task name is not valid"),
    EU_TEST_CASE_ADD(tc_11_main_task, " TEST #11 - Test function _task_get_td when task id is not valid"),
    EU_TEST_CASE_ADD(tc_12_main_task, " TEST #12 - Testing the function _task_check_stack"),
    EU_TEST_CASE_ADD(tc_13_main_task, " TEST #13 - Testing function _task_ready in special cases"),
    EU_TEST_CASE_ADD(tc_14_main_task, " TEST #14 - Testing function _task_restart in ISR"),
 EU_TEST_SUITE_END(task_task)
/* Add test suites*/
    EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(task_task, " - Test task")
    EU_TEST_REGISTRY_END()
/*------------------------------------------------------------------------*/

/*TASK*--------------------------------------------------------------
*
* Task Name : main_task
* Comments  :
*
*END*--------------------------------------------------------------*/
void main_task
   (
      uint32_t dummy
   )
{
   main_task_id = _task_get_id();
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
}/* Endbody */
/* EOF */
