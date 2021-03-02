/**HEADER********************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
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
* $Version : 4.0.2$
* $Date    : Sep-12-2013$
*
* Comments:
*
*   This file contains code for MQX main functions verification program,
*   includes all remaining test cases, which haven't been verified yet,
*   of following functions:
*   _mqx_exit, _mqx_fatal_error, _mqx_get_tad_data, _mqx_set_tad_data
*   and constants:
*   _mqx_version, _mqx_date, _mqx_zero_tick_struct.
*
* Requirements:
*
*   MQX011, MQX013, MQX021, MQX022, MQX029, MQX030.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include "test.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

/*------------------------------------------------------------------------
** Test suite function prototype
*/
void tc_1_main_task(void); /* TEST #1 - Testing function _mqx_exit */
void tc_2_main_task(void); /* TEST #2 - Testing function _mqx_fatal_error */
void tc_3_main_task(void); /* TEST #3 - Testing functions _mqx_get_tad_data and _mqx_set_tad_data */
void tc_4_main_task(void); /* TEST #4 - Testing constants _mqx_version, _mqx_date, _mqx_zero_tick_struct */

/*------------------------------------------------------------------------
** MQX task template
*/
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    { MAIN_TASK,  main_task, 3000,   9,  "main", MQX_AUTO_START_TASK },
    {         0,          0,    0,   0,       0,                   0 }
};

/*------------------------------------------------------------------------
** Global variables
*/
_mqx_uint global_value;

/******************** Begin Test cases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*   This test case is to verify function _mqx_exit in the case of:
*   - Indicating MQX error and making a jump to the startup environment.
* Requirements:
*   MQX012, MQX017, MQX028
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    void (_CODE_PTR_ func_ptr)(void);
    jmp_buf jump_buffer_backup;
    jmp_buf jump_buffer_current;
    _mqx_uint error_code;
    _mqx_uint result;

    /**********************************************************************************************
    **                                                                                           **
    **                    Success: _mqx_exit - Terminates the MQX application                    **
    **                                                                                           **
    **********************************************************************************************/

    /* Set the value of global_value as TEST_VALUE_0 */
    global_value = TEST_VALUE_0;

    /* Backup the startup environment of MQX by copying data from _mqx_exit_jump_buffer_internal to jump_buffer_backup */
    memcpy(jump_buffer_backup, _mqx_exit_jump_buffer_internal, sizeof(jmp_buf));

    /* Set the MQX exit handler as exit_handler_func by calling function _mqx_set_exit_handler */
    _mqx_set_exit_handler(exit_handler_func);
    /* Get the MQX exit handler by calling function _mqx_get_exit_handler, the returned pointer is stored in func_ptr */
    func_ptr = _mqx_get_exit_handler();
    /* Check the func_ptr must be exit_handler_func to verify handler was set successfully */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, func_ptr == exit_handler_func, "TEST #1: Testing 1.01: Verify exit handler was set successfully");

    /* Get the current environment and store in jump_buffer_current by calling function MQX_SETJMP. Here is TEST_STATE */
    result = MQX_SETJMP(jump_buffer_current);
    /* The result of MQX_SETJMP should be 0 */
    if(result == 0)
    {
        /* Get the current kernel's user error and store in error_code by calling utility get_kernel_user_error */
        error_code = get_kernel_user_error();
        /* Check the error_code must be MQX_OK to verify the kernel's error hasn't been changed yet */
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, error_code == MQX_OK, "TEST #1: Testing 1.02: Check the original error code must be MQX_OK");
        /* Check the global_value must be TEST_VALUE_0 to verify it hasn't been changed yet */
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, global_value == TEST_VALUE_0, "TEST #1: Testing 1.03: Verify the initial value");

        /* Modify the startup environment by copying data from jump_buffer_current to _mqx_exit_jump_buffer_internal */
        memcpy(_mqx_exit_jump_buffer_internal, jump_buffer_current, sizeof(jmp_buf));

        /* Calling function _mqx_exit with input error code is MQX_INVALID_COMPONENT_HANDLE */
        _mqx_exit(MQX_INVALID_COMPONENT_HANDLE);
    }

    /*
    ** After calling _mqx_exit, the system will jump to the state which is described in _mqx_exit_jump_buffer_internal.
    ** The _mqx_exit_jump_buffer_internal is now at TEST_STATE so the system will run again from here.
    ** But result of MQX_SETJMP has been set to 1 since MQX_LONGJMP was called in _mqx_exit, the if checking should fail
    ** and system will run to next following steps.
    */

    /* Get the current kernel's user error and store in error_code by calling utility get_kernel_user_error */
    error_code = get_kernel_user_error();
    /* Check the error_code must be MQX_INVALID_COMPONENT_HANDLE to verify the kernel's error has been changed by _mqx_exit */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, error_code == MQX_INVALID_COMPONENT_HANDLE, "TEST #1: Testing 1.04: Check the changed error code must be MQX_INVALID_COMPONENT_HANDLE");
    /* Check the global_value must be TEST_VALUE_1 to verify exit handler function ran and changed its value */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, global_value == TEST_VALUE_1, "TEST #1: Testing 1.05: Verify the value has been changed");

    /* Reset the startup environment of MQX by copying data from jump_buffer_backup to _mqx_exit_jump_buffer_internal */
    memcpy(_mqx_exit_jump_buffer_internal, jump_buffer_backup, sizeof(jmp_buf));

    /* Reset the kernel's user error to MQX_OK by calling utility set_kernel_user_error */
    set_kernel_user_error(MQX_OK);
    /* Get the current kernel's user error and store in error_code by calling utility get_kernel_user_error */
    error_code = get_kernel_user_error();
        /* Check the error_code must be MQX_OK to verify the kernel's error has been reset successfully */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, error_code == MQX_OK, "TEST #1: Testing 1.06: Verify the error code has been restored");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   This test case is to verify function _mqx_fatal_error in the case of:
*   - Indicating MQX error and making a jump to the startup environment.
* Requirements:
*   MQX013
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    void (_CODE_PTR_ func_ptr)(void);
    jmp_buf jump_buffer_backup;
    jmp_buf jump_buffer_current;
    _mqx_uint error_code;
    _mqx_uint result;

    /**********************************************************************************************
    **                                                                                           **
    **                  Success: _mqx_fatal_error - Indicates an error occurred                  **
    **                                                                                           **
    **********************************************************************************************/

    /* Set the value of global_value as TEST_VALUE_0 */
    global_value = TEST_VALUE_0;

    /* Backup the startup environment of MQX by copying data from _mqx_exit_jump_buffer_internal to jump_buffer_backup */
    memcpy(jump_buffer_backup, _mqx_exit_jump_buffer_internal, sizeof(jmp_buf));

    /* Set the MQX exit handler as exit_handler_func by calling function _mqx_set_exit_handler */
    _mqx_set_exit_handler(exit_handler_func);
    /* Get the MQX exit handler by calling function _mqx_get_exit_handler, the returned pointer is stored in func_ptr */
    func_ptr = _mqx_get_exit_handler();
    /* Check the func_ptr must be exit_handler_func to verify handler was set successfully */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, func_ptr == exit_handler_func, "TEST #2: Testing 2.01: Verify exit handler was set successfully");

    /* Get the current environment and store in jump_buffer_current by calling function MQX_SETJMP. Here is TEST_STATE */
    result = MQX_SETJMP(jump_buffer_current);
    /* The result of MQX_SETJMP should be 0 */
    if(result == 0)
    {
        /* Get the current kernel's user error and store in error_code by calling utility get_kernel_user_error */
        error_code = get_kernel_user_error();
        /* Check the error_code must be MQX_OK to verify the kernel's error hasn't been changed yet */
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, error_code == MQX_OK, "TEST #2: Testing 2.02: Check the original error code must be MQX_OK");
        /* Check the global_value must be TEST_VALUE_0 to verify it hasn't been changed yet */
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, global_value == TEST_VALUE_0, "TEST #2: Testing 2.03: Verify the initial value");

        /* Modify the startup environment by copying data from jump_buffer_current to _mqx_exit_jump_buffer_internal */
        memcpy(_mqx_exit_jump_buffer_internal, jump_buffer_current, sizeof(jmp_buf));

        /* Calling function _mqx_fatal_error with input error code is MQX_INVALID_HANDLE */
        _mqx_fatal_error(MQX_INVALID_HANDLE);
    }

    /*
    ** After calling _mqx_fatal_error, the system will jump to the state described in _mqx_exit_jump_buffer_internal.
    ** The _mqx_exit_jump_buffer_internal is now at TEST_STATE so the system will run again from here.
    ** But result of MQX_SETJMP has been set to 1 since MQX_LONGJMP was called in _mqx_fatal_error, the if checking
    ** should fail and system will run to next following steps.
    */

    /* Get the current kernel's user error and store in error_code by calling utility get_kernel_user_error */
    error_code = get_kernel_user_error();
    /* Check the error_code must be MQX_INVALID_HANDLE to verify the kernel's error has been changed by _mqx_fatal_error */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, error_code == MQX_INVALID_HANDLE, "TEST #2: Testing 2.04: Check the changed error code must be MQX_INVALID_HANDLE");
    /* Check the global_value must be TEST_VALUE_1 to verify exit handler function ran and changed its value */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, global_value == TEST_VALUE_1, "TEST #2: Testing 2.05: Verify the value has been changed");

    /* Reset the startup environment of MQX by copying data from jump_buffer_backup to _mqx_exit_jump_buffer_internal */
    memcpy(_mqx_exit_jump_buffer_internal, jump_buffer_backup, sizeof(jmp_buf));

    /* Reset the kernel's user error to MQX_OK by calling utility set_kernel_user_error */
    set_kernel_user_error(MQX_OK);
    /* Get the current kernel's user error and store in error_code by calling utility get_kernel_user_error */
    error_code = get_kernel_user_error();
        /* Check the error_code must be MQX_OK to verify the kernel's error has been reset successfully */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, error_code == MQX_OK, "TEST #2: Testing 2.06: Verify the error code has been restored");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   This test case is to verify two functions _mqx_get_tad_data and
*   _mqx_set_tad_data in the cases of:
*   - Getting TAD data of active task and system task
*   - Setting TAD data of active task and system task
* Requirements:
*   MQX021, MQX022
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    void *tad_ptr;
    void *task_td_ptr;

    /**********************************************************************************************
    **                                                                                           **
    **             Success: _mqx_get_tad_data - Gets the value of TAD_RESERVED field             **
    **                                       (Active task)                                       **
    **                                                                                           **
    **********************************************************************************************/

    /* Get the task descriptor of active task by calling function _task_get_td */
    task_td_ptr = _task_get_td(MQX_NULL_TASK_ID);
    /* The returned pointer is stored in task_td_ptr, check it must be NOT NULL */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, task_td_ptr != NULL, "TEST #3: Testing 3.01: Get the task descriptor of active task");

    /* Get the TAD data of active task by calling function _mqx_get_tad_data with input descriptor task_td_ptr */
    tad_ptr = _mqx_get_tad_data(task_td_ptr);
    /* The returned TAD pointer is stored in tad_ptr, check its value must be NULL to verify getting successfully */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, tad_ptr == NULL, "TEST #3: Testing 3.02: Get the original TAD of current task");

    /**********************************************************************************************
    **                                                                                           **
    **           Success: _mqx_set_tad_data - Sets the value of the TAD_RESERVED field           **
    **                                       (Active task)                                       **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the TAD pointer of active task to TEST_ADDRESS by calling function _mqx_set_tad_data input descriptor task_td_ptr */
    _mqx_set_tad_data(task_td_ptr, (void*)TEST_ADDRESS);
    /* Get the TAD data of active task by calling function _mqx_get_tad_data */
    tad_ptr = _mqx_get_tad_data(task_td_ptr);
    /* The returned TAD pointer is stored in tad_ptr, check its value must be TEST_ADDRESS to verify setting successfully */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, tad_ptr == (void*)TEST_ADDRESS, "TEST #3: Testing 3.03: Change the TAD of current task");

    /* Restore the TAD pointer of active task to original value by calling function _mqx_set_tad_data */
    _mqx_set_tad_data(task_td_ptr, NULL);
    /* Get the TAD data of active task by calling function _mqx_get_tad_data */
    tad_ptr = _mqx_get_tad_data(task_td_ptr);
    /* The returned TAD pointer is stored in tad_ptr, check its value must be NULL */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, tad_ptr == NULL, "TEST #3: Testing 3.04: Restore the TAD of current task");

    /**********************************************************************************************
    **                                                                                           **
    **             Success: _mqx_get_tad_data - Gets the value of TAD_RESERVED field             **
    **                                       (System task)                                       **
    **                                                                                           **
    **********************************************************************************************/

    /* Get the task descriptor of system task by calling function _task_get_td */
    task_td_ptr = _task_get_td(_mqx_get_system_task_id());
    /* The returned pointer is stored in task_td_ptr, check it must be NOT NULL */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, task_td_ptr != NULL, "TEST #3: Testing 3.05: Get the task descriptor of system task");

    /* Get the TAD data of system task by calling function _mqx_get_tad_data with input descriptor task_td_ptr */
    tad_ptr = _mqx_get_tad_data(task_td_ptr);
    /* The returned TAD pointer is stored in tad_ptr, check its value must be NULL to verify getting successfully */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, tad_ptr == NULL, "TEST #3: Testing 3.06: Get the original TAD of system task");

    /**********************************************************************************************
    **                                                                                           **
    **           Success: _mqx_set_tad_data - Sets the value of the TAD_RESERVED field           **
    **                                       (System task)                                       **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the TAD pointer of system task to TEST_ADDRESS by calling function _mqx_set_tad_data input descriptor task_td_ptr */
    _mqx_set_tad_data(task_td_ptr, (void*)TEST_ADDRESS);
    /* Get the TAD data of system task by calling function _mqx_get_tad_data */
    tad_ptr = _mqx_get_tad_data(task_td_ptr);
    /* The returned TAD pointer is stored in tad_ptr, check its value must be TEST_ADDRESS to verify setting successfully */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, tad_ptr == (void*)TEST_ADDRESS, "TEST #3: Testing 3.07: Change the TAD of system task");

    /* Restore the TAD pointer of system task to original value by calling function _mqx_set_tad_data */
    _mqx_set_tad_data(task_td_ptr, NULL);
    /* Get the TAD data of system task by calling function _mqx_get_tad_data */
    tad_ptr = _mqx_get_tad_data(task_td_ptr);
    /* The returned TAD pointer is stored in tad_ptr, check its value must be NULL */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, tad_ptr == NULL, "TEST #3: Testing 3.08: Restore the TAD of system task");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*   This test case is to verify three constants _mqx_version, _mqx_date and
*   _mqx_zero_tick_struct.
* Requirements:
*   MQX011, MQX029, MQX030
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    /* Declare zero_ticks as a zero MQX_TICK_STRUCT */
    MQX_TICK_STRUCT zero_ticks = {0};
    _mqx_uint result;

    /**********************************************************************************************
    **                                                                                           **
    **        Success: _mqx_zero_tick_struct - A constant zero-initialized tick structure        **
    **                                                                                           **
    **********************************************************************************************/

    /* Compare memory of two structures zero_ticks and _mqx_zero_tick_struct */
    result = memcmp(&_mqx_zero_tick_struct, &zero_ticks, sizeof(MQX_TICK_STRUCT));
    /* Check the result must be 0 to verify _mqx_zero_tick_struct was initialized correctly */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == 0, "TEST #4: Testing 4.01: Verify value of _mqx_zero_tick_struct");

    /**********************************************************************************************
    **                                                                                           **
    **                   Success: _mqx_version - Indicates the version of MQX                    **
    **                                                                                           **
    **********************************************************************************************/

    /* Verify the version string of MQX must be NOT NULL */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _mqx_version != NULL, "TEST #4: Testing 4.02: Verify value of _mqx_version");

    /**********************************************************************************************
    **                                                                                           **
    **        Success: _mqx_date - Indicates the date and time when MQX library was built        **
    **                                                                                           **
    **********************************************************************************************/

    /* Verify the date/time string of MQX must be NOT NULL */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _mqx_date != NULL, "TEST #4: Testing 4.03: Verify value of _mqx_date");
}

/*------------------------------------------------------------------------
** Define Test Suite
*/
EU_TEST_SUITE_BEGIN(suite_mqx2)
    EU_TEST_CASE_ADD(tc_1_main_task, "TEST #1 - Testing function _mqx_exit"),
    EU_TEST_CASE_ADD(tc_2_main_task, "TEST #2 - Testing function _mqx_fatal_error"),
    EU_TEST_CASE_ADD(tc_3_main_task, "TEST #3 - Testing functions _mqx_get_tad_data and _mqx_set_tad_data"),
    EU_TEST_CASE_ADD(tc_4_main_task, "TEST #4 - Testing constants _mqx_version, _mqx_date, _mqx_zero_tick_struct"),
EU_TEST_SUITE_END(suite_mqx2)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_mqx2, " - MQX Main functions Test Suite, 4 Tests")
EU_TEST_REGISTRY_END()
/*------------------------------------------------------------------------*/

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   This task creates all defined test tasks and lets them run continuously.
*
*END*----------------------------------------------------------------------*/
void main_task(uint32_t param)
{
    /* Create test tasks */
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : exit_handler_func
* Returned Value   : None
* Comments         :
*   Change the value of global_value to TEST_VALUE_1
*
*END*--------------------------------------------------------------------*/
void exit_handler_func(void)
{
    global_value = TEST_VALUE_1;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_kernel_user_error
* Returned Value   : _mqx_uint (Error value)
* Comments         :
*   Get the kernel's user error code which is set by function _mqx_exit.
*
*END*--------------------------------------------------------------------*/
_mqx_uint get_kernel_user_error(void)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    _mqx_uint error_code;

    /* Get current kernel structure */
    _GET_KERNEL_DATA(kernel_data);

    /* Prevent other processes from changing kernel */
    _int_disable();

    /* Get the error value */
    error_code = kernel_data->USERS_ERROR;

    /* Re-enable interrupts */
    _int_enable();

    return error_code;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_kernel_user_error
* Returned Value   : None
* Comments         :
*   Set the kernel's user error code.
*   - [IN] error_code: The error value to be set
*
*END*--------------------------------------------------------------------*/
void set_kernel_user_error(_mqx_uint error_code)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;

    /* Get current kernel structure */
    _GET_KERNEL_DATA(kernel_data);

    /* Prevent other processes from accessing kernel */
    _int_disable();

    /* Set the error value */
     kernel_data->USERS_ERROR = error_code;

    /* Re-enable interrupts */
    _int_enable();
}
