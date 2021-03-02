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
* $Date    : Aug-12-2013$
*
* Comments:
*
*   This file contains code for MQX Semaphore component verification program,
*   includes all remaining test cases, which haven't been verified yet,
*   of following functions:
*   _sem_create_component, _sem_create_fast, _sem_create, _sem_destroy
*   _sem_open, _sem_close, _sem_test, _sem_post
*   _sem_wait, _sem_wait_ticks, _sem_wait_for, _sem_wait_until
*
* Requirements:
*
*   SEM002, SEM007, SEM010, SEM014, SEM015, SEM018, SEM019, SEM021, SEM026,
*   SEM027, SEM028, SEM029, SEM030, SEM048, SEM058, SEM059, SEM060, SEM061,
*   SEM062, SEM064, SEM068, SEM069, SEM070, SEM071, SEM072, SEM073, SEM074,
*   SEM075, SEM076, SEM077, SEM078, SEM079, SEM080, SEM081, SEM082, SEM083,
*   SEM084, SEM085, SEM086, SEM087, SEM088, SEM089, SEM090, SEM091, SEM092,
*   SEM093, SEM094.
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
#include "sem_util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

/*------------------------------------------------------------------------
** Test suite function prototype
*/
void tc_1_main_task(void);  /* TEST #1 - Testing function _sem_create_component */
void tc_2_main_task(void);  /* TEST #2 - Testing function _sem_create_fast */
void tc_3_main_task(void);  /* TEST #3 - Testing function _sem_create */
void tc_4_main_task(void);  /* TEST #4 - Testing function _sem_destroy */
void tc_5_main_task(void);  /* TEST #5 - Testing function _sem_open */
void tc_6_main_task(void);  /* TEST #6 - Testing function _sem_close */
void tc_7_main_task(void);  /* TEST #7 - Testing function _sem_wait */
void tc_8_main_task(void);  /* TEST #8 - Testing function _sem_wait_ticks */
void tc_9_main_task(void);  /* TEST #9 - Testing function _sem_wait_for */
void tc_10_main_task(void); /* TEST #10 - Testing function _sem_wait_until */
void tc_11_main_task(void); /* TEST #11 - Testing function _sem_post */
void tc_12_main_task(void); /* TEST #12 - Testing function _sem_test */

/*------------------------------------------------------------------------
** MQX task template
*/
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    {  MAIN_TASK,  main_task, 3000,   MAIN_PRIO,  "main", MQX_AUTO_START_TASK },
    {  WAIT_TASK,  wait_task, 1000,   MAIN_PRIO,  "wait",                   0 },
    { OWNER_TASK, owner_task, 1000, MAIN_PRIO-1, "owner",                   0 },
    { PRIO1_TASK,  prio_task, 1000, MAIN_PRIO-1, "prio1",                   0 },
    { PRIO2_TASK,  prio_task, 1000, MAIN_PRIO-2, "prio2",                   0 },
    {          0,          0,    0,           0,       0,                   0 }
};

/*------------------------------------------------------------------------
** Global variables
*/
LWSEM_STRUCT lwsem;
void *global_sem_ptr;
uint32_t global_val;

/******************** Begin Test cases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*   This test case is to verify function _sem_create_component in the cases of:
*   - Creating semaphore component when memory is insufficient.
*   - Creating semaphore component properly without error.
* Requirements:
*   SEM014, SEM015
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    MEMORY_ALLOC_INFO_STRUCT mem_alloc_info;
    _mqx_uint result;

    /**********************************************************************************************
    **                                                                                           **
    **                Failure: _sem_create_component - Could not allocate memory                 **
    **                                                                                           **
    **********************************************************************************************/

    /* Allocate all memory, the allocated pointers are stored in mem_alloc_info */
    memory_alloc_all(&mem_alloc_info);
    /* Try to create semaphore component by calling function _sem_create_component when memory is insufficient */
    result = _sem_create_component(0, 1, 0);
    /* Check the result must be MQX_OUT_OF_MEMORY to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OUT_OF_MEMORY, "TEST #1: Testing 1.01: Failed to create semaphore component when memory is insufficient");
    /* Free all memory whose pointers are stored in mem_alloc_info */
    memory_free_all(&mem_alloc_info);
    /* Clear error code if exists */
    _task_set_error(MQX_OK);

    /**********************************************************************************************
    **                                                                                           **
    **                           Precondition of following test cases                            **
    **                                                                                           **
    **********************************************************************************************/

    /* Create semaphore component by calling function _sem_create_component in normal case */
    result = _sem_create_component(0, 1, 0);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "TEST #1: Testing 1.02: Create semaphore component properly");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   This test case is to verify function _sem_create_fast in the cases of:
*   - Creating semaphore when name table handler is invalid
*   - Creating semaphore when semaphore component is invalid
*   - Creating strict semaphore with invalid initial count
* Requirements:
*   SEM018, SEM019, SEM021
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    void *bak_ptr;
    _mqx_uint result;

    /**********************************************************************************************
    **                                                                                           **
    **              Failure: _sem_create_fast - Semaphore component does not exist               **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the name table handler of semaphore component to invalid pointer */
    bak_ptr = get_sem_component_name_table();
    set_sem_component_name_table(NULL);
    /* Try to create semaphore by calling function _sem_create_fast when name table handler is invalid */
    result = _sem_create_fast(SEM_INDEX, 1, 0);
    /* Check the result must be MQX_COMPONENT_DOES_NOT_EXIST to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "TEST #2: Testing 2.01: Failed to create semaphore when name table handler is invalid");
    /* Restore the name table handler of semaphore component to original pointer */
    set_sem_component_name_table(bak_ptr);

    /**********************************************************************************************
    **                                                                                           **
    **                     Failure: _sem_create_fast - Component is invalid                      **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the VALID field of semaphore component to invalid value */
    set_sem_component_valid_field(SEM_VALID + INVALID_VALUE);
    /* Try to create semaphore by calling function _sem_create_fast semaphore component is invalid */
    result = _sem_create_fast(SEM_INDEX, 1, 0);
    /* Check the result must be MQX_INVALID_COMPONENT_BASE to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_INVALID_COMPONENT_BASE, "TEST #2: Testing 2.02: Failed to create semaphore when semaphore component is invalid");
    /* Restore the VALID field of semaphore component to original value */
    set_sem_component_valid_field(SEM_VALID);

    /**********************************************************************************************
    **                                                                                           **
    **                 Failure: _sem_create_fast - Initial count is not correct                  **
    **                                                                                           **
    **********************************************************************************************/

    /* Try to create a strict semaphore with invalid initial count by calling function _sem_create_fast */
    result = _sem_create_fast(SEM_INDEX, 0, SEM_STRICT);
    /* Check the result must be SEM_INCORRECT_INITIAL_COUNT to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == SEM_INCORRECT_INITIAL_COUNT, "TEST #2: Testing 2.03: Failed to create a strict semaphore with invalid initial count");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   This test case is to verify function _sem_create in the case of:
*   - Creating strict semaphore with invalid initial count
* Requirements:
*   SEM007, SEM010
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    void *bak_ptr;
    _mqx_uint result;

    /**********************************************************************************************
    **                                                                                           **
    **                 Failure: _sem_create - Semaphore component does not exist                 **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the name table handler of semaphore component to invalid pointer */
    bak_ptr = get_sem_component_name_table();
    set_sem_component_name_table(NULL);
    /* Try to create semaphore by calling function _sem_create when name table handler is invalid */
    result = _sem_create(SEM_NAME, 1, 0);
    /* Check the result must be MQX_COMPONENT_DOES_NOT_EXIST to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "TEST #3: Testing 3.01: Failed to create semaphore when name table handler is invalid");
    /* Restore the name table handler of semaphore component to original pointer */
    set_sem_component_name_table(bak_ptr);

    /**********************************************************************************************
    **                                                                                           **
    **                    Failure: _sem_create - Initial count is not correct                    **
    **                                                                                           **
    **********************************************************************************************/

    /* Try to create a strict semaphore with invalid initial count by calling function _sem_create_fast */
    result = _sem_create(SEM_NAME, 0, SEM_STRICT);
    /* Check the result must be SEM_INCORRECT_INITIAL_COUNT to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == SEM_INCORRECT_INITIAL_COUNT, "TEST #3: Testing 3.02: Failed to create a strict semaphore with invalid initial count");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*   This test case is to verify function _sem_destroy in the cases of:
*   - Destroying semaphore in ISR
*   - Destroying semaphore when semaphore component doesn't exist
*   - Destroying semaphore when semaphore component is invalid
*   - Destroying semaphore when semaphore is invalid
*   - Destroying semaphore which doesn't exist
* Requirements:
*   SEM026, SEM027, SEM028, SEM029, SEM030
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    void *bak_ptr;
    _mqx_uint result;
    uint16_t isr_value;

    /* Create semaphore SEM_NAME with initial count is 1 by calling function _sem_create */
    result = _sem_create(SEM_NAME, 1, 0);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "TEST #4: Testing 4.01: Create a semaphore named SEM_NAME");

    /**********************************************************************************************
    **                                                                                           **
    **                   Failure: _sem_destroy - Could not be called from ISR                    **
    **                                                                                           **
    **********************************************************************************************/

    /* Disable all interrupts and save the current ISR counting value to isr_value */
    _int_disable();
    isr_value = get_in_isr();
    /* Change the ISR counting value to make task become an ISR */
    set_in_isr(1);
    /* Try to destroy semaphore SEM_NAME by calling function _sem_destroy in ISR */
    result = _sem_destroy(SEM_NAME, FALSE);
    /* Check the result must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "TEST #4: Testing 4.02: Failed to destroy semaphore in ISR");
    /* Reset the ISR counting value to original value which is stored in isr_value, then enable interrupts */
    set_in_isr(isr_value);
    _int_enable();

    /**********************************************************************************************
    **                                                                                           **
    **                     Failure: _sem_destroy - Component does not exist                      **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the pointer of semaphore component to invalid pointer */
    bak_ptr = get_sem_component_ptr();
    set_sem_component_ptr(NULL);
    /* Try to destroy semaphore SEM_NAME by calling function _sem_destroy when semaphore component doesn't exist */
    result = _sem_destroy(SEM_NAME, FALSE);
    /* Check the result must be MQX_COMPONENT_DOES_NOT_EXIST to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "TEST #4: Testing 4.03: Failed to destroy semaphore when semaphore component doesn't exist");
    /* Restore the pointer of semaphore component to original pointer */
    set_sem_component_ptr(bak_ptr);

    /**********************************************************************************************
    **                                                                                           **
    **                     Failure: _sem_destroy - Component data is invalid                     **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the VALID field of semaphore component to invalid value */
    set_sem_component_valid_field(SEM_VALID + INVALID_VALUE);
    /* Try to destroy semaphore SEM_NAME by calling function _sem_destroy when semaphore component is invalid */
    result = _sem_destroy(SEM_NAME, FALSE);
    /* Check the result must be MQX_INVALID_COMPONENT_BASE to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_INVALID_COMPONENT_BASE, "TEST #4: Testing 4.04: Failed to destroy semaphore when semaphore component is invalid");
    /* Restore the VALID field of semaphore component to original value */
    set_sem_component_valid_field(SEM_VALID);

    /**********************************************************************************************
    **                                                                                           **
    **                     Failure: _sem_destroy - The semaphore is invalid                      **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the VALID field of semaphore SEM_NAME to invalid value */
    set_sem_valid_field(SEM_NAME, SEM_VALID + INVALID_VALUE);
    /* Try to destroy semaphore SEM_NAME by calling function _sem_destroy when semaphore is invalid */
    result = _sem_destroy(SEM_NAME, FALSE);
    /* Check the result must be SEM_INVALID_SEMAPHORE to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == SEM_INVALID_SEMAPHORE, "TEST #4: Testing 4.05: Failed to destroy semaphore when semaphore is invalid");
    /* Restore the VALID field of semaphore SEM_NAME to original value */
    set_sem_valid_field(SEM_NAME, SEM_VALID);

    /**********************************************************************************************
    **                                                                                           **
    **                   Failure: _sem_destroy - The semaphore does not exist                    **
    **                                                                                           **
    **********************************************************************************************/

    /* Destroy the semaphore SEM_NAME by calling function _sem_destroy */
    result = _sem_destroy(SEM_NAME, FALSE);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "TEST #4: Testing 4.06: Destroy semaphore SEM_NAME properly");

    /* Try to destroy the semaphore SEM_NAME, which has already been destroyed, by calling function _sem_destroy */
    result = _sem_destroy(SEM_NAME, FALSE);
    /* Check the result must be SEM_SEMAPHORE_NOT_FOUND to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == SEM_SEMAPHORE_NOT_FOUND, "TEST #4: Testing 4.07: Failed to destroy semaphore which has already been destroyed");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5:
*   This test case is to verify function _sem_open in the case of:
*   - Opening semaphore which is going to be destroyed
* Requirements:
*   SEM048
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    void *sem_ptr;
    _mqx_uint result;

    /**********************************************************************************************
    **                                                                                           **
    **                  Failure: _sem_open - Semaphore is going to be destroyed                  **
    **                                                                                           **
    **********************************************************************************************/

    /* Create semaphore SEM_NAME with initial count is 1 by calling function _sem_create */
    result = _sem_create(SEM_NAME, 1, 0);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "TEST #5: Testing 5.01: Create a semaphore named SEM_NAME");

    /* Change the DELAYED_DESTROY field of semaphore SEM_NAME to TRUE */
    set_sem_delayed_destroy_field(SEM_NAME, TRUE);
    /* Try to open semaphore SEM_NAME by calling function _sem_open when semaphore is going to be destroyed */
    result = _sem_open(SEM_NAME, &sem_ptr);
    /* Check the result must be SEM_SEMAPHORE_DELETED to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == SEM_SEMAPHORE_DELETED, "TEST #5: Testing 5.02: Failed to open semaphore which is going to be destroyed");
    /* Restore the DELAYED_DESTROY field of semaphore SEM_NAME to FALSE */
    set_sem_delayed_destroy_field(SEM_NAME, FALSE);

    /* Destroy the semaphore SEM_NAME by calling function _sem_destroy */
    result = _sem_destroy(SEM_NAME, TRUE);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "TEST #5: Testing 5.03: Destroy semaphore SEM_NAME");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6:
*   This test case is to verify function _sem_close in the case of:
*   - Close the semaphore connection that task doesn't own
* Requirements:
*   SEM002
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    TD_STRUCT_PTR bak_ptr;
    _task_id owner_tid;
    _mqx_uint result;

    /**********************************************************************************************
    **                                                                                           **
    **                                Init test case components                                  **
    **                                                                                           **
    **********************************************************************************************/

    /* Create semaphore SEM_NAME with initial count is 1 by calling function _sem_create */
    result = _sem_create(SEM_NAME, 1, 0);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "TEST #6: Testing 6.01: Create a semaphore named SEM_NAME");

    /* Create OWNER_TASK by calling function _task_create. With higher priority, this task runs immediately */
    owner_tid = _task_create(0, OWNER_TASK, 0);
    /* Check the returned task id must be non-zero to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, owner_tid != 0, "TEST #6: Testing 6.02: Create OWNER_TASK");

    /**********************************************************************************************
    **                                                                                           **
    **          Failure: _sem_close - The semaphore doesn't belong to the current task           **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the owner of semaphore connection global_sem_ptr from OWNER_TASK to MAIN_TASK */
    bak_ptr = get_sem_handle_owner(global_sem_ptr);
    set_sem_handle_owner(global_sem_ptr, _task_get_td(MQX_NULL_TASK_ID));
    /* Try to close the connection global_sem_ptr whose memory belongs to other task by calling function _sem_close */
    result = _sem_close(global_sem_ptr);
    /* Check the result must be MQX_NOT_RESOURCE_OWNER to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_NOT_RESOURCE_OWNER, "TEST #6: Testing 6.03: Failed to close the semaphore connection which task doesn't own");
    /* Restore the owner of semaphore connection global_sem_ptr to OWNER_TASK */
    set_sem_handle_owner(global_sem_ptr, bak_ptr);

    /* Closing failed, then reset some parameters were changed manually */
    set_sem_handle_valid_field(global_sem_ptr, SEM_VALID);
    _task_set_error(MQX_OK);

    /**********************************************************************************************
    **                                                                                           **
    **                               Deinit test case components                                 **
    **                                                                                           **
    **********************************************************************************************/

    /* Destroy the OWNER_TASK by calling function _task_destroy */
    result = _task_destroy(owner_tid);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "TEST #6: Testing 6.04: Destroy OWNER_TASK");

    /* Destroy the semaphore SEM_NAME by calling function _sem_destroy */
    result = _sem_destroy(SEM_NAME, TRUE);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "TEST #6: Testing 6.05: Destroy semaphore SEM_NAME");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7:
*   This test case is to verify function _sem_wait in the cases of:
*   - Waiting for semaphore in ISR
*   - Waiting for semaphore when connection is invalid
*   - Waiting for invalid semaphore
*   - Waiting for semaphore may cause deadlock
*   - Waiting for semaphore until timeout
*   - Waiting for semaphore which is going to be destroyed
* Requirements:
*   SEM068, SEM069, SEM070, SEM071, SEM072, SEM073.
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    MQX_TICK_STRUCT start_ticks, end_ticks, diff_ticks;
    TIME_STRUCT diff_time;
    void *sem_ptr;
    _task_id wait_tid;
    _mqx_uint result;
    uint16_t isr_value;

    /**********************************************************************************************
    **                                                                                           **
    **                                Init test case components                                  **
    **                                                                                           **
    **********************************************************************************************/

    /* Create strict semaphore SEM_NAME with initial count is 1 by calling function _sem_create */
    result = _sem_create(SEM_NAME, 1, SEM_STRICT);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "TEST #7: Testing 7.01: Create a semaphore named SEM_NAME");

    /* Open a connection to semaphore SEM_NAME by calling function _sem_open. The handler pointer is stored in sem_ptr */
    result = _sem_open(SEM_NAME, &sem_ptr);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "TEST #7: Testing 7.02: Open a connection to semaphore SEM_NAME");

    /**********************************************************************************************
    **                                                                                           **
    **                     Failure: _sem_wait - Could not be called from ISR                     **
    **                                                                                           **
    **********************************************************************************************/

    /* Disable all interrupts and save the current ISR counting value to isr_value */
    _int_disable();
    isr_value = get_in_isr();
    /* Change the ISR counting value to make task become an ISR */
    set_in_isr(1);
    /* Try to wait for semaphore SEM_NAME by calling function _sem_wait in ISR */
    result = _sem_wait(sem_ptr, 0);
    /* Check the result must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "TEST #7: Testing 7.03: Failed to wait for semaphore in ISR");
    /* Reset the ISR counting value to original value which is stored in isr_value, then enable interrupts */
    set_in_isr(isr_value);
    _int_enable();

    /**********************************************************************************************
    **                                                                                           **
    **                   Failure: _sem_wait - The semaphore handle is invalid                    **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the VALID field of semaphore connection sem_ptr to invalid value */
    set_sem_handle_valid_field(sem_ptr, SEM_VALID + INVALID_VALUE);
    /* Try to wait for semaphore SEM_NAME by calling function _sem_wait when semaphore connection is invalid */
    result = _sem_wait(sem_ptr, 0);
    /* Check the result must be SEM_INVALID_SEMAPHORE_HANDLE to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == SEM_INVALID_SEMAPHORE_HANDLE, "TEST #7: Testing 7.04: Failed to wait for semaphore when semaphore connection is invalid");
    /* Reset the VALID field of semaphore connection sem_ptr to original value */
    set_sem_handle_valid_field(sem_ptr, SEM_VALID);

    /**********************************************************************************************
    **                                                                                           **
    **                       Failure: _sem_wait - The semaphore is invalid                       **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the VALID field of semaphore SEM_NAME to invalid value */
    set_sem_valid_field(SEM_NAME, SEM_VALID + INVALID_VALUE);
    /* Try to wait for semaphore SEM_NAME by calling function _sem_wait when semaphore is invalid */
    result = _sem_wait(sem_ptr, 0);
    /* Check the result must be SEM_INVALID_SEMAPHORE to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == SEM_INVALID_SEMAPHORE, "TEST #7: Testing 7.05: Failed to wait for an invalid semaphore");
    /* Reset the VALID field of semaphore SEM_NAME to original value */
    set_sem_valid_field(SEM_NAME, SEM_VALID);

    /**********************************************************************************************
    **                                                                                           **
    **                     Failure: _sem_wait - Aborted to prevent deadlock                      **
    **                                                                                           **
    **********************************************************************************************/

    /* Wait and hold the semaphore SEM_NAME by calling function _sem_wait */
    result = _sem_wait(sem_ptr, 0);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "TEST #7: Testing 7.06: Wait and hold the semaphore");
    /* Try to wait for semaphore SEM_NAME by calling function _sem_wait while task has already hold the semaphore */
    result = _sem_wait(sem_ptr, 0);
    /* Check the result must be MQX_EDEADLK to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_EDEADLK, "TEST #7: Testing 7.07: Abort waiting to prevent deadlock");
    /* Post the semaphore SEM_NAME by calling function _sem_post */
    result = _sem_post(sem_ptr);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "TEST #7: Testing 7.08: Post the held semaphore");

    /**********************************************************************************************
    **                                                                                           **
    **                           Failure: _sem_wait - Timeout expired                            **
    **                                                                                           **
    **********************************************************************************************/

    /* Create WAIT_TASK with equal priority by calling function _task_create */
    wait_tid = _task_create(0, WAIT_TASK, 0);
    /* Check the returned task id must be non-zero to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, wait_tid != 0, "TEST #7: Testing 7.09: Create WAIT_TASK");

    /* Wait for lightweight semaphore lwsem until it is available by calling function _lwsem_wait */
    _lwsem_wait(&lwsem);
    /* MAIN_TASK suspends, WAIT_TASK runs from here */

    /* Get the tick time before delay, the ticks value is stored in start_ticks */
    _time_get_elapsed_ticks(&start_ticks);
    /* Wait for semaphore SEM_NAME in DELAY_MS milliseconds by calling function _sem_wait */
    result = _sem_wait(sem_ptr, DELAY_MS);
    /* Get the tick time after delay, the ticks value is stored in end_ticks */
    _time_get_elapsed_ticks(&end_ticks);
    /* Calculate the different ticks between start_ticks and end_ticks, then store value to diff_ticks */
    PSP_SUB_TICKS(&end_ticks, &start_ticks, &diff_ticks);
    /* Convert different time from ticks to milliseconds and store in diff_time */
    PSP_TICKS_TO_TIME(&diff_ticks, &diff_time);
    /* Check the result must be SEM_WAIT_TIMEOUT to verify task waited for but didn't get the semaphore */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == SEM_WAIT_TIMEOUT, "TEST #7: Testing 7.10: Waiting for semaphore SEM_NAME when it has already been held");
    /* Check the milliseconds value of diff_time and DELAY_MS must be equal to verify accurate waiting time */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, time_check_struct(diff_time, 0, DELAY_MS, 1), "TEST #7: Testing 7.11: Verify the timeout must be accurate");

    /**********************************************************************************************
    **                                                                                           **
    **                Failure: _sem_wait - The semaphore is going to be destroyed                **
    **                                                                                           **
    **********************************************************************************************/

    /* Post the lightweight semaphore lwsem by calling function _lwsem_post */
    _lwsem_post(&lwsem);
    /* Wait for semaphore SEM_NAME in DELAY_MS milliseconds by calling function _sem_wait */
    /* While MAIN_TASK suspends for waiting semaphore, WAIT_TASK runs */
    result = _sem_wait(sem_ptr, DELAY_MS);
    /* Check the result must be SEM_SEMAPHORE_DELETED to verify waiting for destroyed semaphore should fail */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == SEM_SEMAPHORE_DELETED, "TEST #7: Testing 7.12: Failed to waiting for semaphore when it's going to be destroyed");
    /* The semaphore was detected as destroyed so the calling task wasn't removed from waiting list. Then do it manually */
    remove_waiting_task(sem_ptr);

    /* Post the lightweight semaphore lwsem by calling function _lwsem_post */
    _lwsem_post(&lwsem);
    /* Wait for lightweight semaphore lwsem until it is available by calling function _lwsem_wait */
    _lwsem_wait(&lwsem);
    /* MAIN_TASK suspends, WAIT_TASK runs from here */

    /* Destroy the WAIT_TASK by calling function _task_destroy */
    result = _task_destroy(wait_tid);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "TEST #7: Testing 7.13: Destroy WAIT_TASK");

    /**********************************************************************************************
    **                                                                                           **
    **                               Deinit test case components                                 **
    **                                                                                           **
    **********************************************************************************************/

    /* Close the semaphore connection by calling function _sem_close */
    result = _sem_close(sem_ptr);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "TEST #7: Testing 7.14: Close the semaphore connection");

    /* Destroy the semaphore SEM_NAME by calling function _sem_destroy */
    result = _sem_destroy(SEM_NAME, TRUE);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "TEST #7: Testing 7.15: Destroy semaphore SEM_NAME");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8:
*   This test case is to verify function _sem_wait_ticks in the cases of:
*   - Waiting then holding semaphore properly.
*   - Waiting for semaphore in ISR
*   - Waiting for semaphore when connection is invalid
*   - Waiting for invalid semaphore
*   - Waiting for semaphore may cause deadlock
*   - Waiting for semaphore until timeout
*   - Waiting for semaphore which is going to be destroyed
* Requirements:
*   SEM081, SEM082, SEM083, SEM084, SEM085, SEM086, SEM087
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
    MQX_TICK_STRUCT start_ticks, end_ticks, expect_ticks;
    TD_STRUCT_PTR td_ptr;
    void *sem_ptr;
    _task_id wait_tid, prio1_tid, prio2_tid;
    _mqx_uint result, prio;
    uint16_t isr_value;

    /**********************************************************************************************
    **                                                                                           **
    **                                Init test case components                                  **
    **                                                                                           **
    **********************************************************************************************/

    /* Create strict semaphore SEM_NAME with initial count is 1 by calling function _sem_create */
    result = _sem_create(SEM_NAME, 1, SEM_STRICT | SEM_PRIORITY_INHERITANCE | SEM_PRIORITY_QUEUEING);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "TEST #8: Testing 8.01: Create a semaphore named SEM_NAME");

    /* Open a connection to semaphore SEM_NAME by calling function _sem_open. The handler pointer is stored in sem_ptr */
    result = _sem_open(SEM_NAME, &sem_ptr);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "TEST #8: Testing 8.02: Open a connection to semaphore SEM_NAME");

    /**********************************************************************************************
    **                                                                                           **
    **                  Failure: _sem_wait_ticks - Could not be called from ISR                  **
    **                                                                                           **
    **********************************************************************************************/

    /* Disable all interrupts and save the current ISR counting value to isr_value */
    _int_disable();
    isr_value = get_in_isr();
    /* Change the ISR counting value to make task become an ISR */
    set_in_isr(1);
    /* Try to wait for semaphore SEM_NAME by calling function _sem_wait_ticks in ISR */
    result = _sem_wait_ticks(sem_ptr, 0);
    /* Check the result must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "TEST #8: Testing 8.03: Failed to wait for semaphore in ISR");
    /* Reset the ISR counting value to original value which is stored in isr_value, then enable interrupts */
    set_in_isr(isr_value);
    _int_enable();

    /**********************************************************************************************
    **                                                                                           **
    **                Failure: _sem_wait_ticks - The semaphore handle is invalid                 **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the VALID field of semaphore connection sem_ptr to invalid value */
    set_sem_handle_valid_field(sem_ptr, SEM_VALID + INVALID_VALUE);
    /* Try to wait for semaphore SEM_NAME by calling function _sem_wait_ticks when semaphore connection is invalid */
    result = _sem_wait_ticks(sem_ptr, 0);
    /* Check the result must be SEM_INVALID_SEMAPHORE_HANDLE to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == SEM_INVALID_SEMAPHORE_HANDLE, "TEST #8: Testing 8.04: Failed to wait for semaphore when semaphore connection is invalid");
    /* Reset the VALID field of semaphore connection sem_ptr to original value */
    set_sem_handle_valid_field(sem_ptr, SEM_VALID);

    /**********************************************************************************************
    **                                                                                           **
    **                    Failure: _sem_wait_ticks - The semaphore is invalid                    **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the VALID field of semaphore SEM_NAME to invalid value */
    set_sem_valid_field(SEM_NAME, SEM_VALID + INVALID_VALUE);
    /* Try to wait for semaphore SEM_NAME by calling function _sem_wait_ticks when semaphore is invalid */
    result = _sem_wait_ticks(sem_ptr, 0);
    /* Check the result must be SEM_INVALID_SEMAPHORE to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == SEM_INVALID_SEMAPHORE, "TEST #8: Testing 8.05: Failed to wait for an invalid semaphore");
    /* Reset the VALID field of semaphore SEM_NAME to original value */
    set_sem_valid_field(SEM_NAME, SEM_VALID);

    /**********************************************************************************************
    **                                                                                           **
    **                  Failure: _sem_wait_ticks - Aborted to prevent deadlock                   **
    **                                                                                           **
    **********************************************************************************************/

    /* Wait and hold the semaphore SEM_NAME by calling function _sem_wait_ticks */
    result = _sem_wait_ticks(sem_ptr, 0);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "TEST #8: Testing 8.06: Wait and hold the semaphore");
    /* Try to wait for semaphore SEM_NAME by calling function _sem_wait_ticks while task has already hold the semaphore */
    result = _sem_wait_ticks(sem_ptr, 0);
    /* Check the result must be MQX_EDEADLK to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_EDEADLK, "TEST #8: Testing 8.07: Abort waiting to prevent deadlock");

    /**********************************************************************************************
    **                                                                                           **
    **                Success: _sem_wait_ticks - Wait for the semaphore in ticks                 **
    **                                 (PRIORITY INHERITANCE)                                    **
    **                                                                                           **
    **********************************************************************************************/

    /* Get the priority of MAIN_TASK by calling function _task_get_priority */
    td_ptr = _task_get_td(MQX_NULL_TASK_ID);
    prio = td_ptr->MY_QUEUE->PRIORITY;
    /* The priority value must be no-changed */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, prio == MAIN_PRIO, "TEST #8: Testing 8.08: The priority of MAIN_TASK hasn't been changed yet");

    /* Create PRIO1_TASK with higher priority by calling function _task_create */
    prio1_tid = _task_create(0, PRIO1_TASK, PRIO1_TASK);
    /* Check the returned task id must be non-zero to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, prio1_tid != 0, "TEST #8: Testing 8.09: Create PRIO1_TASK");

    /* Get the priority of MAIN_TASK by calling function _task_get_priority */
    td_ptr = _task_get_td(MQX_NULL_TASK_ID);
    prio = td_ptr->MY_QUEUE->PRIORITY;
    /* The priority value must be boosted to higher */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, prio == (MAIN_PRIO - 1), "TEST #8: Testing 8.10: The priority of MAIN_TASK has been boosted to higher");

    /* Create PRIO2_TASK with higher priority by calling function _task_create */
    prio2_tid = _task_create(0, PRIO2_TASK, PRIO2_TASK);
    /* Check the returned task id must be non-zero to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, prio2_tid != 0, "TEST #8: Testing 8.11: Create PRIO2_TASK");

    /* Get the priority of MAIN_TASK by calling function _task_get_priority */
    td_ptr = _task_get_td(MQX_NULL_TASK_ID);
    prio = td_ptr->MY_QUEUE->PRIORITY;
    /* The priority value must be boosted to highest */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, prio == (MAIN_PRIO - 2), "TEST #8: Testing 8.12: The priority of MAIN_TASK has been boosted to highest");

    /**********************************************************************************************
    **                                                                                           **
    **                Success: _sem_wait_ticks - Wait for the semaphore in ticks                 **
    **                                  (PRIORITY QUEUEING)                                      **
    **                                                                                           **
    **********************************************************************************************/

    /* Set the initial value of global_val as 1 */
    global_val = 1;

    /* Post the semaphore SEM_NAME by calling function _sem_post */
    result = _sem_post(sem_ptr);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "TEST #8: Testing 8.13: Post the held semaphore");

    /* Check the global_val must be 0 to verify the PRIO2_TASK ran before PRIO1_TASK because of priority queueing */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, global_val == 0, "TEST #8: Testing 8.14: Semaphore priority queueing");

    /* Destroy the PRIO1_TASK by calling function _task_destroy */
    result = _task_destroy(prio1_tid);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "TEST #8: Testing 8.15: Destroy PRIO1_TASK");

    /* Destroy the PRIO2_TASK by calling function _task_destroy */
    result = _task_destroy(prio2_tid);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "TEST #8: Testing 8.16: Destroy PRIO2_TASK");

    /**********************************************************************************************
    **                                                                                           **
    **                        Failure: _sem_wait_ticks - Timeout expired                         **
    **                                                                                           **
    **********************************************************************************************/

    /* Create WAIT_TASK with equal priority by calling function _task_create */
    wait_tid = _task_create(0, WAIT_TASK, 0);
    /* Check the returned task id must be non-zero to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, wait_tid != 0, "TEST #8: Testing 8.17: Create WAIT_TASK");

    /* Wait for lightweight semaphore lwsem until it is available by calling function _lwsem_wait */
    _lwsem_wait(&lwsem);
    /* MAIN_TASK suspends, WAIT_TASK runs from here */

    /* Get the tick time before delay, the ticks value is stored in start_ticks */
    _time_get_elapsed_ticks(&start_ticks);
    /* Wait for semaphore SEM_NAME in DELAY_TICKS ticks by calling function _sem_wait_ticks */
    result = _sem_wait_ticks(sem_ptr, DELAY_TICKS);
    /* Get the tick time after delay, the ticks value is stored in end_ticks */
    _time_get_elapsed_ticks(&end_ticks);
    /* Calculate the expected tick time and store in expect_ticks */
    PSP_ADD_TICKS_TO_TICK_STRUCT(&start_ticks, DELAY_TICKS, &expect_ticks);
    /* Check the result must be SEM_WAIT_TIMEOUT to verify task waited but didn't get the semaphore until timeout */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == SEM_WAIT_TIMEOUT, "TEST #8: Testing 8.18: Waiting for semaphore SEM_NAME when it has already been held");
    /* Check the tick value of expect_ticks and end_ticks must be equal to verify accurate waiting time */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, PSP_CMP_TICKS(&expect_ticks, &end_ticks) == 0, "TEST #8: Testing 8.19: Verify the timeout must be accurate");

    /**********************************************************************************************
    **                                                                                           **
    **             Failure: _sem_wait_ticks - The semaphore is going to be destroyed             **
    **                                                                                           **
    **********************************************************************************************/

    /* Post the lightweight semaphore lwsem by calling function _lwsem_post */
    _lwsem_post(&lwsem);
    /* Wait for semaphore SEM_NAME in DELAY_TICKS ticks by calling function _sem_wait_ticks */
    /* While MAIN_TASK suspends for waiting semaphore, WAIT_TASK runs */
    result = _sem_wait_ticks(sem_ptr, DELAY_TICKS);
    /* Check the result must be SEM_SEMAPHORE_DELETED to verify waiting for destroyed semaphore should fail */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == SEM_SEMAPHORE_DELETED, "TEST #8: Testing 8.20: Failed to waiting for semaphore when it's going to be destroyed");
    /* The semaphore was detected as destroyed so the calling task wasn't removed from waiting list. Then do it manually */
    remove_waiting_task(sem_ptr);

    /* Post the lightweight semaphore lwsem by calling function _lwsem_post */
    _lwsem_post(&lwsem);
    /* Wait for lightweight semaphore lwsem until it is available by calling function _lwsem_wait */
    _lwsem_wait(&lwsem);
    /* MAIN_TASK suspends, WAIT_TASK runs from here */

    /* Destroy the WAIT_TASK by calling function _task_destroy */
    result = _task_destroy(wait_tid);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "TEST #8: Testing 8.21: Destroy WAIT_TASK");

    /**********************************************************************************************
    **                                                                                           **
    **                               Deinit test case components                                 **
    **                                                                                           **
    **********************************************************************************************/

    /* Close the semaphore connection by calling function _sem_close */
    result = _sem_close(sem_ptr);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "TEST #8: Testing 8.22: Close the semaphore connection");

    /* Destroy the semaphore SEM_NAME by calling function _sem_destroy */
    result = _sem_destroy(SEM_NAME, TRUE);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "TEST #8: Testing 8.23: Destroy semaphore SEM_NAME");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : TEST #9:
*   This test case is to verify function _sem_wait_for in the cases of:
*   - Waiting then holding semaphore properly.
*   - Waiting for semaphore in ISR
*   - Waiting for semaphore when connection is invalid
*   - Waiting for invalid semaphore
*   - Waiting for semaphore may cause deadlock
*   - Waiting for semaphore until timeout
*   - Waiting for semaphore which is going to be destroyed
* Requirements:
*   SEM074, SEM075, SEM076, SEM077, SEM078, SEM079, SEM080
*
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
    MQX_TICK_STRUCT start_ticks, end_ticks, expect_ticks;
    TD_STRUCT_PTR td_ptr;
    void *sem_ptr;
    _task_id wait_tid, prio1_tid, prio2_tid;
    _mqx_uint result, prio;
    uint16_t isr_value;

    /**********************************************************************************************
    **                                                                                           **
    **                                Init test case components                                  **
    **                                                                                           **
    **********************************************************************************************/

    /* Create strict semaphore SEM_NAME with initial count is 1 by calling function _sem_create */
    result = _sem_create(SEM_NAME, 1, SEM_STRICT | SEM_PRIORITY_INHERITANCE | SEM_PRIORITY_QUEUEING);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "TEST #9: Testing 9.01: Create a semaphore named SEM_NAME");

    /* Open a connection to semaphore SEM_NAME by calling function _sem_open. The handler pointer is stored in sem_ptr */
    result = _sem_open(SEM_NAME, &sem_ptr);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "TEST #9: Testing 9.02: Open a connection to semaphore SEM_NAME");

    /**********************************************************************************************
    **                                                                                           **
    **                   Failure: _sem_wait_for - Could not be called from ISR                   **
    **                                                                                           **
    **********************************************************************************************/

    /* Disable all interrupts and save the current ISR counting value to isr_value */
    _int_disable();
    isr_value = get_in_isr();
    /* Change the ISR counting value to make task become an ISR */
    set_in_isr(1);
    /* Try to wait for semaphore SEM_NAME by calling function _sem_wait_for in ISR */
    result = _sem_wait_for(sem_ptr, NULL);
    /* Check the result must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "TEST #9: Testing 9.03: Failed to wait for semaphore in ISR");
    /* Reset the ISR counting value to original value which is stored in isr_value, then enable interrupts */
    set_in_isr(isr_value);
    _int_enable();

    /**********************************************************************************************
    **                                                                                           **
    **                 Failure: _sem_wait_for - The semaphore handle is invalid                  **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the VALID field of semaphore connection sem_ptr to invalid value */
    set_sem_handle_valid_field(sem_ptr, SEM_VALID + INVALID_VALUE);
    /* Try to wait for semaphore SEM_NAME by calling function _sem_wait_for when semaphore connection is invalid */
    result = _sem_wait_for(sem_ptr, NULL);
    /* Check the result must be SEM_INVALID_SEMAPHORE_HANDLE to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == SEM_INVALID_SEMAPHORE_HANDLE, "TEST #9: Testing 9.04: Failed to wait for semaphore when semaphore connection is invalid");
    /* Reset the VALID field of semaphore connection sem_ptr to original value */
    set_sem_handle_valid_field(sem_ptr, SEM_VALID);

    /**********************************************************************************************
    **                                                                                           **
    **                     Failure: _sem_wait_for - The semaphore is invalid                     **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the VALID field of semaphore SEM_NAME to invalid value */
    set_sem_valid_field(SEM_NAME, SEM_VALID + INVALID_VALUE);
    /* Try to wait for semaphore SEM_NAME by calling function _sem_wait_for when semaphore is invalid */
    result = _sem_wait_for(sem_ptr, NULL);
    /* Check the result must be SEM_INVALID_SEMAPHORE to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == SEM_INVALID_SEMAPHORE, "TEST #9: Testing 9.05: Failed to wait for an invalid semaphore");
    /* Reset the VALID field of semaphore SEM_NAME to original value */
    set_sem_valid_field(SEM_NAME, SEM_VALID);

    /**********************************************************************************************
    **                                                                                           **
    **                   Failure: _sem_wait_for - Aborted to prevent deadlock                    **
    **                                                                                           **
    **********************************************************************************************/

    /* Wait and hold the semaphore SEM_NAME by calling function _sem_wait_for */
    result = _sem_wait_for(sem_ptr, NULL);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "TEST #9: Testing 9.06: Wait and hold the semaphore");
    /* Try to wait for semaphore SEM_NAME by calling function _sem_wait_for while task has already hold the semaphore */
    result = _sem_wait_for(sem_ptr, NULL);
    /* Check the result must be MQX_EDEADLK to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_EDEADLK, "TEST #9: Testing 9.07: Abort waiting to prevent deadlock");

    /**********************************************************************************************
    **                                                                                           **
    **                 Success: _sem_wait_for - Wait for the semaphore in ticks                  **
    **                                 (PRIORITY INHERITANCE)                                    **
    **                                                                                           **
    **********************************************************************************************/

    /* Get the priority of MAIN_TASK by calling function _task_get_priority */
    td_ptr = _task_get_td(MQX_NULL_TASK_ID);
    prio = td_ptr->MY_QUEUE->PRIORITY;
    /* Check the priority value must be original */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, prio == MAIN_PRIO, "TEST #9: Testing 9.08: The priority of MAIN_TASK hasn't been changed yet");

    /* Create PRIO1_TASK with higher priority by calling function _task_create */
    prio1_tid = _task_create(0, PRIO1_TASK, PRIO1_TASK);
    /* Check the returned task id must be non-zero to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, prio1_tid != 0, "TEST #9: Testing 9.09: Create PRIO1_TASK");

    /* Get the priority of MAIN_TASK by calling function _task_get_priority */
    td_ptr = _task_get_td(MQX_NULL_TASK_ID);
    prio = td_ptr->MY_QUEUE->PRIORITY;
    /* Check the priority value must be boosted to higher */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, prio == (MAIN_PRIO - 1), "TEST #9: Testing 9.10: The priority of MAIN_TASK has been boosted to higher");

    /* Create PRIO2_TASK with higher priority by calling function _task_create */
    prio2_tid = _task_create(0, PRIO2_TASK, PRIO2_TASK);
    /* Check the returned task id must be non-zero to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, prio2_tid != 0, "TEST #9: Testing 9.11: Create PRIO2_TASK");

    /* Get the priority of MAIN_TASK by calling function _task_get_priority */
    td_ptr = _task_get_td(MQX_NULL_TASK_ID);
    prio = td_ptr->MY_QUEUE->PRIORITY;
    /* Check the priority value must be boosted to highest */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, prio == (MAIN_PRIO - 2), "TEST #9: Testing 9.12: The priority of MAIN_TASK has been boosted to highest");

    /**********************************************************************************************
    **                                                                                           **
    **                 Success: _sem_wait_for - Wait for the semaphore in ticks                  **
    **                                  (PRIORITY QUEUEING)                                      **
    **                                                                                           **
    **********************************************************************************************/

    /* Set the initial value of global_val as 1 */
    global_val = 1;

    /* Post the semaphore SEM_NAME by calling function _sem_post */
    result = _sem_post(sem_ptr);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "TEST #9: Testing 9.13: Post the held semaphore");

    /* Check the global_val must be 0 to verify the PRIO2_TASK ran before PRIO1_TASK because of priority queueing */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, global_val == 0, "TEST #9: Testing 9.14: Semaphore priority queueing");

    /* Destroy the PRIO1_TASK by calling function _task_destroy */
    result = _task_destroy(prio1_tid);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "TEST #9: Testing 9.15: Destroy PRIO1_TASK");

    /* Destroy the PRIO2_TASK by calling function _task_destroy */
    result = _task_destroy(prio2_tid);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "TEST #9: Testing 9.16: Destroy PRIO2_TASK");

    /**********************************************************************************************
    **                                                                                           **
    **                         Failure: _sem_wait_for - Timeout expired                          **
    **                                                                                           **
    **********************************************************************************************/

    /* Create WAIT_TASK with equal priority by calling function _task_create */
    wait_tid = _task_create(0, WAIT_TASK, 0);
    /* Check the returned task id must be non-zero to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, wait_tid != 0, "TEST #9: Testing 9.17: Create WAIT_TASK");

    /* Wait for lightweight semaphore lwsem until it is available by calling function _lwsem_wait */
    _lwsem_wait(&lwsem);
    /* MAIN_TASK suspends, WAIT_TASK runs from here */

    /* Initialize time structure with DELAY_TICKS ticks and store in variable expect_ticks*/
    _time_init_ticks(&expect_ticks, DELAY_TICKS);
    /* Get the tick time before delay, the ticks value is stored in start_ticks */
    _time_get_elapsed_ticks(&start_ticks);
    /* Wait for semaphore SEM_NAME in DELAY_TICKS ticks by calling function _sem_wait_for */
    result = _sem_wait_for(sem_ptr, &expect_ticks);
    /* Get the tick time after delay, the ticks value is stored in end_ticks */
    _time_get_elapsed_ticks(&end_ticks);
    /* Calculate the expected tick time and store in expect_ticks */
    PSP_ADD_TICKS(&start_ticks, &expect_ticks, &expect_ticks);
    /* Check the result must be SEM_WAIT_TIMEOUT to verify task waited for but didn't get the semaphore */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == SEM_WAIT_TIMEOUT, "TEST #9: Testing 9.18: Waiting for semaphore SEM_NAME when it has already been held");
    /* Check the tick value of expect_ticks and end_ticks must be equal to verify accurate waiting time */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, PSP_CMP_TICKS(&expect_ticks, &end_ticks) == 0, "TEST #9: Testing 9.19: Verify the timeout must be accurate");

    /**********************************************************************************************
    **                                                                                           **
    **              Failure: _sem_wait_for - The semaphore is going to be destroyed              **
    **                                                                                           **
    **********************************************************************************************/

    /* Initialize time structure with DELAY_TICKS ticks and store in variable expect_ticks*/
    _time_init_ticks(&expect_ticks, DELAY_TICKS);
    /* Post the lightweight semaphore lwsem by calling function _lwsem_post */
    _lwsem_post(&lwsem);
    /* Wait for semaphore SEM_NAME in DELAY_TICKS ticks by calling function _sem_wait_for */
    /* While MAIN_TASK suspends for waiting semaphore, WAIT_TASK runs */
    result = _sem_wait_for(sem_ptr, &expect_ticks);
    /* Check the result must be SEM_SEMAPHORE_DELETED to verify waiting for destroyed semaphore should fail */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == SEM_SEMAPHORE_DELETED, "TEST #9: Testing 9.20: Failed to waiting for semaphore when it's going to be destroyed");
    /* The semaphore was detected as destroyed so the calling task wasn't removed from waiting list. Then do it manually */
    remove_waiting_task(sem_ptr);

    /* Post the lightweight semaphore lwsem by calling function _lwsem_post */
    _lwsem_post(&lwsem);
    /* Wait for lightweight semaphore lwsem until it is available by calling function _lwsem_wait */
    _lwsem_wait(&lwsem);
    /* MAIN_TASK suspends, WAIT_TASK runs from here */

    /* Destroy the WAIT_TASK by calling function _task_destroy */
    result = _task_destroy(wait_tid);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "TEST #9: Testing 9.21: Destroy WAIT_TASK");

    /**********************************************************************************************
    **                                                                                           **
    **                               Deinit test case components                                 **
    **                                                                                           **
    **********************************************************************************************/

    /* Close the semaphore connection by calling function _sem_close */
    result = _sem_close(sem_ptr);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "TEST #9: Testing 9.22: Close the semaphore connection");

    /* Destroy the semaphore SEM_NAME by calling function _sem_destroy */
    result = _sem_destroy(SEM_NAME, TRUE);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "TEST #9: Testing 9.23: Destroy semaphore SEM_NAME");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_10_main_task
* Comments     : TEST #10:
*   This test case is to verify function _sem_wait_until in the cases of:
*   - Waiting then holding semaphore properly.
*   - Waiting for semaphore in ISR
*   - Waiting for semaphore when connection is invalid
*   - Waiting for invalid semaphore
*   - Waiting for semaphore may cause deadlock
*   - Waiting for semaphore until timeout
*   - Waiting for semaphore which is going to be destroyed
* Requirements:
*   SEM088, SEM089, SEM090, SEM091, SEM092, SEM093, SEM094
*
*END*---------------------------------------------------------------------*/
void tc_10_main_task(void)
{
    MQX_TICK_STRUCT start_ticks, end_ticks, expect_ticks;
    TD_STRUCT_PTR td_ptr;
    void *sem_ptr;
    _task_id wait_tid, prio1_tid, prio2_tid;
    _mqx_uint result, prio;
    uint16_t isr_value;

    /**********************************************************************************************
    **                                                                                           **
    **                                Init test case components                                  **
    **                                                                                           **
    **********************************************************************************************/

    /* Create strict semaphore SEM_NAME with initial count is 1 by calling function _sem_create */
    result = _sem_create(SEM_NAME, 1, SEM_STRICT | SEM_PRIORITY_INHERITANCE | SEM_PRIORITY_QUEUEING);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "TEST #10: Testing 10.01: Create a semaphore named SEM_NAME");

    /* Open a connection to semaphore SEM_NAME by calling function _sem_open. The handler pointer is stored in sem_ptr */
    result = _sem_open(SEM_NAME, &sem_ptr);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "TEST #10: Testing 10.02: Open a connection to semaphore SEM_NAME");

    /**********************************************************************************************
    **                                                                                           **
    **                  Failure: _sem_wait_until - Could not be called from ISR                  **
    **                                                                                           **
    **********************************************************************************************/

    /* Disable all interrupts and save the current ISR counting value to isr_value */
    _int_disable();
    isr_value = get_in_isr();
    /* Change the ISR counting value to make task become an ISR */
    set_in_isr(1);
    /* Try to wait for semaphore SEM_NAME by calling function _sem_wait_until in ISR */
    result = _sem_wait_until(sem_ptr, NULL);
    /* Check the result must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "TEST #10: Testing 10.03: Failed to wait for semaphore in ISR");
    /* Reset the ISR counting value to original value which is stored in isr_value, then enable interrupts */
    set_in_isr(isr_value);
    _int_enable();

    /**********************************************************************************************
    **                                                                                           **
    **                Failure: _sem_wait_until - The semaphore handle is invalid                 **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the VALID field of semaphore connection sem_ptr to invalid value */
    set_sem_handle_valid_field(sem_ptr, SEM_VALID + INVALID_VALUE);
    /* Try to wait for semaphore SEM_NAME by calling function _sem_wait_until when semaphore connection is invalid */
    result = _sem_wait_until(sem_ptr, NULL);
    /* Check the result must be SEM_INVALID_SEMAPHORE_HANDLE to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == SEM_INVALID_SEMAPHORE_HANDLE, "TEST #10: Testing 10.04: Failed to wait for semaphore when semaphore connection is invalid");
    /* Reset the VALID field of semaphore connection sem_ptr to original value */
    set_sem_handle_valid_field(sem_ptr, SEM_VALID);

    /**********************************************************************************************
    **                                                                                           **
    **                    Failure: _sem_wait_until - The semaphore is invalid                    **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the VALID field of semaphore SEM_NAME to invalid value */
    set_sem_valid_field(SEM_NAME, SEM_VALID + INVALID_VALUE);
    /* Try to wait for semaphore SEM_NAME by calling function _sem_wait_until when semaphore is invalid */
    result = _sem_wait_until(sem_ptr, NULL);
    /* Check the result must be SEM_INVALID_SEMAPHORE to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == SEM_INVALID_SEMAPHORE, "TEST #10: Testing 10.05: Failed to wait for an invalid semaphore");
    /* Reset the VALID field of semaphore SEM_NAME to original value */
    set_sem_valid_field(SEM_NAME, SEM_VALID);

    /**********************************************************************************************
    **                                                                                           **
    **                  Failure: _sem_wait_until - Aborted to prevent deadlock                   **
    **                                                                                           **
    **********************************************************************************************/

    /* Wait and hold the semaphore SEM_NAME by calling function _sem_wait_until */
    result = _sem_wait_until(sem_ptr, NULL);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "TEST #10: Testing 10.06: Wait and hold the semaphore");
    /* Try to wait for semaphore SEM_NAME by calling function _sem_wait_until while task has already hold the semaphore */
    result = _sem_wait_until(sem_ptr, NULL);
    /* Check the result must be MQX_EDEADLK to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_EDEADLK, "TEST #10: Testing 10.07: Abort waiting to prevent deadlock");

    /**********************************************************************************************
    **                                                                                           **
    **                Success: _sem_wait_until - Wait for the semaphore in ticks                 **
    **                                 (PRIORITY INHERITANCE)                                    **
    **                                                                                           **
    **********************************************************************************************/

    /* Get the priority of MAIN_TASK by calling function _task_get_priority */
    td_ptr = _task_get_td(MQX_NULL_TASK_ID);
    prio = td_ptr->MY_QUEUE->PRIORITY;
    /* Check the priority value must be original */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, prio == MAIN_PRIO, "TEST #10: Testing 10.08: The priority of MAIN_TASK hasn't been changed yet");

    /* Create PRIO1_TASK with higher priority by calling function _task_create */
    prio1_tid = _task_create(0, PRIO1_TASK, PRIO1_TASK);
    /* Check the returned task id must be non-zero to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, prio1_tid != 0, "TEST #10: Testing 10.09: Create PRIO1_TASK");

    /* Get the priority of MAIN_TASK by calling function _task_get_priority */
    td_ptr = _task_get_td(MQX_NULL_TASK_ID);
    prio = td_ptr->MY_QUEUE->PRIORITY;
    /* Check the priority value must be boosted to higher */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, prio == (MAIN_PRIO - 1), "TEST #10: Testing 10.10: The priority of MAIN_TASK has been boosted to higher");

    /* Create PRIO2_TASK with higher priority by calling function _task_create */
    prio2_tid = _task_create(0, PRIO2_TASK, PRIO2_TASK);
    /* Check the returned task id must be non-zero to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, prio2_tid != 0, "TEST #10: Testing 10.11: Create PRIO2_TASK");

    /* Get the priority of MAIN_TASK by calling function _task_get_priority */
    td_ptr = _task_get_td(MQX_NULL_TASK_ID);
    prio = td_ptr->MY_QUEUE->PRIORITY;
    /* Check the priority value must be boosted to highest */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, prio == (MAIN_PRIO - 2), "TEST #10: Testing 10.12: The priority of MAIN_TASK has been boosted to highest");

    /**********************************************************************************************
    **                                                                                           **
    **                Success: _sem_wait_until - Wait for the semaphore in ticks                 **
    **                                  (PRIORITY QUEUEING)                                      **
    **                                                                                           **
    **********************************************************************************************/

    /* Set the initial value of global_val as 1 */
    global_val = 1;

    /* Post the semaphore SEM_NAME by calling function _sem_post */
    result = _sem_post(sem_ptr);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "TEST #10: Testing 10.13: Post the held semaphore");

    /* Check the global_val must be 0 to verify the PRIO2_TASK ran before PRIO1_TASK because of priority queueing */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, global_val == 0, "TEST #10: Testing 10.14: Semaphore priority queueing");

    /* Destroy the PRIO1_TASK by calling function _task_destroy */
    result = _task_destroy(prio1_tid);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "TEST #10: Testing 10.15: Destroy PRIO1_TASK");

    /* Destroy the PRIO2_TASK by calling function _task_destroy */
    result = _task_destroy(prio2_tid);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "TEST #10: Testing 10.16: Destroy PRIO2_TASK");

    /**********************************************************************************************
    **                                                                                           **
    **                        Failure: _sem_wait_until - Timeout expired                         **
    **                                                                                           **
    **********************************************************************************************/

    /* Create WAIT_TASK with equal priority by calling function _task_create */
    wait_tid = _task_create(0, WAIT_TASK, 0);
    /* Check the returned task id must be non-zero to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, wait_tid != 0, "TEST #10: Testing 10.17: Create WAIT_TASK");

    /* Wait for lightweight semaphore lwsem until it is available by calling function _lwsem_wait */
    _lwsem_wait(&lwsem);
    /* MAIN_TASK suspends, WAIT_TASK runs from here */

    /* Get the tick time before delay, the ticks value is stored in start_ticks */
    _time_get_elapsed_ticks(&start_ticks);
    /* Calculate the moment (in tick time) to wait until and store in variable expect_ticks */
    PSP_ADD_TICKS_TO_TICK_STRUCT(&start_ticks, DELAY_TICKS, &expect_ticks);
    /* Wait for semaphore SEM_NAME in DELAY_TICKS ticks by calling function _sem_wait_until */
    result = _sem_wait_until(sem_ptr, &expect_ticks);
    /* Get the tick time after delay, the ticks value is stored in end_ticks */
    _time_get_elapsed_ticks(&end_ticks);
    /* Check the result must be SEM_WAIT_TIMEOUT to verify task waited for but didn't get the semaphore */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == SEM_WAIT_TIMEOUT, "TEST #10: Testing 10.18: Waiting for semaphore SEM_NAME when it has already been held");
    /* Check the tick value of expect_ticks and end_ticks must be equal to verify accurate waiting time */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, PSP_CMP_TICKS(&expect_ticks, &end_ticks) == 0, "TEST #10: Testing 10.19: Verify the timeout must be accurate");

    /**********************************************************************************************
    **                                                                                           **
    **             Failure: _sem_wait_until - The semaphore is going to be destroyed             **
    **                                                                                           **
    **********************************************************************************************/

    /* Get the tick time before delay, the ticks value is stored in start_ticks */
    _time_get_elapsed_ticks(&start_ticks);
    /* Calculate the moment (in tick time) to wait until and store in variable expect_ticks */
    PSP_ADD_TICKS_TO_TICK_STRUCT(&start_ticks, DELAY_TICKS, &expect_ticks);
    /* Post the lightweight semaphore lwsem by calling function _lwsem_post */
    _lwsem_post(&lwsem);
    /* Wait for semaphore SEM_NAME in DELAY_TICKS ticks by calling function _sem_wait_until */
    /* While MAIN_TASK suspends for waiting semaphore, WAIT_TASK runs */
    result = _sem_wait_until(sem_ptr, &expect_ticks);
    /* Check the result must be SEM_SEMAPHORE_DELETED to verify waiting for destroyed semaphore should fail */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == SEM_SEMAPHORE_DELETED, "TEST #10: Testing 10.20: Failed to waiting for semaphore when it's going to be destroyed");
    /* The semaphore was detected as destroyed so the calling task wasn't removed from waiting list. Then do it manually */
    remove_waiting_task(sem_ptr);

    /* Post the lightweight semaphore lwsem by calling function _lwsem_post */
    _lwsem_post(&lwsem);
    /* Wait for lightweight semaphore lwsem until it is available by calling function _lwsem_wait */
    _lwsem_wait(&lwsem);
    /* MAIN_TASK suspends, WAIT_TASK runs from here */

    /* Destroy the WAIT_TASK by calling function _task_destroy */
    result = _task_destroy(wait_tid);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "TEST #10: Testing 10.21: Destroy WAIT_TASK");

    /**********************************************************************************************
    **                                                                                           **
    **                               Deinit test case components                                 **
    **                                                                                           **
    **********************************************************************************************/

    /* Close the semaphore connection by calling function _sem_close */
    result = _sem_close(sem_ptr);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "TEST #10: Testing 10.22: Close the semaphore connection");

    /* Destroy the semaphore SEM_NAME by calling function _sem_destroy */
    result = _sem_destroy(SEM_NAME, TRUE);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "TEST #10: Testing 10.23: Destroy semaphore SEM_NAME");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_11_main_task
* Comments     : TEST #11:
*   This test case is to verify function _sem_post in the cases of:
*   - Posting a strict semaphore before waiting for it
*   - Posting a semaphore in ISR
*   - Posting a semaphore when connection is invalid
*   - Posting an invalid semaphore
*   - Posting a semaphore with corrupt data
* Requirements:
*   SEM058, SEM059, SEM060, SEM061, SEM062
*
*END*---------------------------------------------------------------------*/
void tc_11_main_task(void)
{
    void *sem_ptr;
    _mqx_uint result, bak_val;
    uint16_t isr_value;

    /**********************************************************************************************
    **                                                                                           **
    **                                Init test case components                                  **
    **                                                                                           **
    **********************************************************************************************/

    /* Create strict semaphore SEM_NAME with initial count is 1 by calling function _sem_create */
    result = _sem_create(SEM_NAME, 1, SEM_STRICT);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "TEST #11: Testing 11.01: Create a semaphore named SEM_NAME");

    /* Open a connection to semaphore SEM_NAME by calling function _sem_open. The handler pointer is stored in sem_ptr */
    result = _sem_open(SEM_NAME, &sem_ptr);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "TEST #11: Testing 11.02: Open a connection to semaphore SEM_NAME");

    /**********************************************************************************************
    **                                                                                           **
    **               Failure: _sem_post - The task is waiting for other semaphore                **
    **                                                                                           **
    **********************************************************************************************/

    /* Try to post semaphore SEM_NAME by calling function _sem_post before waiting for it */
    result = _sem_post(sem_ptr);
    /* Check the result must be SEM_CANNOT_POST to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == SEM_CANNOT_POST, "TEST #11: Testing 11.03: Failed to post semaphore SEM_NAME before waiting for it");

    /* Wait and hold the semaphore SEM_NAME by calling function _sem_wait */
    result = _sem_wait(sem_ptr, 0);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "TEST #11: Testing 11.04: Wait and hold the semaphore SEM_NAME");

    /**********************************************************************************************
    **                                                                                           **
    **                     Failure: _sem_post - Could not be called from ISR                     **
    **                                                                                           **
    **********************************************************************************************/

    /* Disable all interrupts and save the current ISR counting value to isr_value */
    _int_disable();
    isr_value = get_in_isr();
    /* Change the ISR counting value to make task become an ISR */
    set_in_isr(1);
    /* Try to post semaphore SEM_NAME by calling function _sem_post in ISR */
    result = _sem_post(sem_ptr);
    /* Check the result must be MQX_CANNOT_CALL_FUNCTION_FROM_ISR to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "TEST #11: Testing 11.05: Failed to post semaphore in ISR");
    /* Reset the ISR counting value to original value which is stored in isr_value, then enable interrupts */
    set_in_isr(isr_value);
    _int_enable();

    /**********************************************************************************************
    **                                                                                           **
    **                      Failure: _sem_post - Invalid semaphore handle                        **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the VALID field of semaphore connection sem_ptr to invalid value */
    set_sem_handle_valid_field(sem_ptr, SEM_VALID + INVALID_VALUE);
    /* Try to post semaphore SEM_NAME by calling function _sem_post when semaphore connection is invalid */
    result = _sem_post(sem_ptr);
    /* Check the result must be SEM_INVALID_SEMAPHORE_HANDLE to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == SEM_INVALID_SEMAPHORE_HANDLE, "TEST #11: Testing 11.06: Failed to post semaphore when semaphore connection is invalid");
    /* Reset the VALID field of semaphore connection sem_ptr to original value */
    set_sem_handle_valid_field(sem_ptr, SEM_VALID);

    /**********************************************************************************************
    **                                                                                           **
    **                       Failure: _sem_post - The semaphore is invalid                       **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the VALID field of semaphore SEM_NAME to invalid value */
    set_sem_valid_field(SEM_NAME, SEM_VALID + INVALID_VALUE);
    /* Try to post semaphore SEM_NAME by calling function _sem_post when semaphore is invalid */
    result = _sem_post(sem_ptr);
    /* Check the result must be SEM_INVALID_SEMAPHORE to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == SEM_INVALID_SEMAPHORE, "TEST #11: Testing 11.07: Failed to post an invalid semaphore");
    /* Reset the VALID field of semaphore SEM_NAME to original value */
    set_sem_valid_field(SEM_NAME, SEM_VALID);

    /**********************************************************************************************
    **                                                                                           **
    **                     Failure: _sem_post - Semaphore data is corrupted                      **
    **                                                                                           **
    **********************************************************************************************/

    /* Change the COUNT field of semaphore SEM_NAME to invalid value */
    bak_val = get_sem_count_field(SEM_NAME);
    set_sem_count_field(SEM_NAME, MAX_MQX_UINT);
    /* Try to post semaphore SEM_NAME by calling function _sem_post when semaphore data is corrupted */
    result = _sem_post(sem_ptr);
    /* Check the result must be SEM_INVALID_SEMAPHORE_COUNT to verify unsuccessful operation */
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == SEM_INVALID_SEMAPHORE_COUNT, "TEST #11: Testing 11.08: Failed to post semaphore with corrupt data");
    /* Reset the COUNT field of semaphore SEM_NAME to original value */
    set_sem_count_field(SEM_NAME, bak_val);

    /**********************************************************************************************
    **                                                                                           **
    **                               Deinit test case components                                 **
    **                                                                                           **
    **********************************************************************************************/

    /* Post the semaphore SEM_NAME by calling function _sem_post */
    result = _sem_post(sem_ptr);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "TEST #11: Testing 11.09: Post the held semaphore");

    /* Close the semaphore connection by calling function _sem_close */
    result = _sem_close(sem_ptr);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "TEST #11: Testing 11.10: Close the semaphore connection");

    /* Destroy the semaphore SEM_NAME by calling function _sem_destroy */
    result = _sem_destroy(SEM_NAME, TRUE);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "TEST #11: Testing 11.11: Destroy semaphore SEM_NAME");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_12_main_task
* Comments     : TEST #12:
*   This test case is to verify function _sem_test in the case of:
*   - Testing semaphore component when semaphore queue is corrupt
* Requirements:
*   SEM064
*
*END*---------------------------------------------------------------------*/
void tc_12_main_task(void)
{
    SEM_CONNECTION_STRUCT_PTR sem_connection_ptr;
    QUEUE_STRUCT_PTR q_ptr;
    void *sem_ptr, *error_ptr, *bak_ptr;
    _mqx_uint result;

    /**********************************************************************************************
    **                                                                                           **
    **                                Init test case components                                  **
    **                                                                                           **
    **********************************************************************************************/

    result = _sem_create(SEM_NAME, 1, 0);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "TEST #12: Testing 12.01: Create a semaphore named SEM_NAME");

    /* Open a connection to semaphore SEM_NAME by calling function _sem_open */
    result = _sem_open(SEM_NAME, &sem_ptr);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "TEST #12: Testing 12.02: Open a connection to semaphore SEM_NAME");

    sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)sem_ptr;

    /**********************************************************************************************
    **                                                                                           **
    **                     Failure: _sem_test - A semaphore queue is corrupt                     **
    **                         (Invalid element in WAITING_TASKS queue)                          **
    **                                                                                           **
    **********************************************************************************************/

    /* Get the waiting queue of semaphore SEM_NAME and store in q_ptr */
    q_ptr = &(sem_connection_ptr->SEM_PTR->WAITING_TASKS);
    /* Make the queue invalid by modifying the pointer q_ptr */
    bak_ptr = (void *)q_ptr->PREV;
    q_ptr->PREV = NULL;
    /* Test all the semaphore components by calling function _sem_test */
    result = _sem_test(&error_ptr);
    /* Check the result must be MQX_CORRUPT_QUEUE to verify there's corrupt queue detected */
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_CORRUPT_QUEUE, "TEST #12: Testing 12.03: There's corrupt queue detected");
    /* The returned error pointer must be pointer of waiting queue of semaphore SEM_NAME */
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, error_ptr == (void *)q_ptr, "TEST #12: Testing 12.04: Verify correct error pointer");
    /* Reset the modified pointer q_ptr to correct value */
    q_ptr->PREV = (QUEUE_ELEMENT_STRUCT_PTR)bak_ptr;

    /**********************************************************************************************
    **                                                                                           **
    **                     Failure: _sem_test - A semaphore queue is corrupt                     **
    **                          (Invalid element in OWNING_TASKS queue)                          **
    **                                                                                           **
    **********************************************************************************************/

    /* Get the owning queue of semaphore SEM_NAME and store in q_ptr */
    q_ptr = &(sem_connection_ptr->SEM_PTR->WAITING_TASKS);
    /* Make the queue invalid by modifying the pointer q_ptr */
    bak_ptr = (void *)q_ptr->PREV;
    q_ptr->PREV = NULL;
    /* Test all the semaphore components by calling function _sem_test */
    result = _sem_test(&error_ptr);
    /* Check the result must be MQX_CORRUPT_QUEUE to verify there's corrupt queue detected */
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_CORRUPT_QUEUE, "TEST #12: Testing 12.05: There's corrupt queue detected");
    /* The returned error pointer must be pointer of owning queue of semaphore SEM_NAME */
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, error_ptr == (void *)q_ptr, "TEST #12: Testing 12.06: Verify correct error pointer");
    /* Reset the modified pointer q_ptr to correct value */
    q_ptr->PREV = (QUEUE_ELEMENT_STRUCT_PTR)bak_ptr;

    /**********************************************************************************************
    **                                                                                           **
    **                               Deinit test case components                                 **
    **                                                                                           **
    **********************************************************************************************/

    /* Close the semaphore connection by calling function _sem_close */
    result = _sem_close(sem_ptr);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "TEST #12: Testing 12.07: Close the semaphore connection");

    /* Destroy the semaphore SEM_NAME by calling function _sem_destroy */
    result = _sem_destroy(SEM_NAME, TRUE);
    /* Check the result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "TEST #12: Testing 12.08: Destroy semaphore SEM_NAME");
}

/*------------------------------------------------------------------------
** Define Test Suite
*/
EU_TEST_SUITE_BEGIN(suite_sem2)
    EU_TEST_CASE_ADD(tc_1_main_task,  "TEST #1 - Testing function _sem_create_component"),
    EU_TEST_CASE_ADD(tc_2_main_task,  "TEST #2 - Testing function _sem_create_fast"),
    EU_TEST_CASE_ADD(tc_3_main_task,  "TEST #3 - Testing function _sem_create"),
    EU_TEST_CASE_ADD(tc_4_main_task,  "TEST #4 - Testing function _sem_destroy"),
    EU_TEST_CASE_ADD(tc_5_main_task,  "TEST #5 - Testing function _sem_open"),
    EU_TEST_CASE_ADD(tc_6_main_task,  "TEST #6 - Testing function _sem_close"),
    EU_TEST_CASE_ADD(tc_7_main_task,  "TEST #7 - Testing function _sem_wait"),
    EU_TEST_CASE_ADD(tc_8_main_task,  "TEST #8 - Testing function _sem_wait_ticks"),
    EU_TEST_CASE_ADD(tc_9_main_task,  "TEST #9 - Testing function _sem_wait_for"),
    EU_TEST_CASE_ADD(tc_10_main_task, "TEST #10 - Testing function _sem_wait_until"),
    EU_TEST_CASE_ADD(tc_11_main_task, "TEST #11 - Testing function _sem_post"),
    EU_TEST_CASE_ADD(tc_12_main_task, "TEST #12 - Testing function _sem_test"),
EU_TEST_SUITE_END(suite_sem2)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_sem2, " - MQX Semaphore Component Test Suite, 12 Tests")
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
    _mqx_uint result;

    /* Create the lightweight semaphore lwsem with 0 initial count */
    result = _lwsem_create(&lwsem, 0);
    if(result != MQX_OK)
    {
        printf("Unable to create test component");
    }

    /* Create test tasks */
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : owner_task
* Comments     :
*   This task opens a connection to the semaphore which was created in
*   MAIN_TASK, the connection will be used to test accessing resource of task.
*   After that, the task will be blocked.
*
*END*----------------------------------------------------------------------*/
void owner_task(uint32_t param)
{
    /* Open a connection to the semaphore SEM_NAME, the returned pointer is stored in global_sem_ptr */
    _sem_open(SEM_NAME, &global_sem_ptr);

    /* Wait for destroying */
    _task_block();
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : wait_task
* Comments     :
*   This task waits for the semaphore which was created in MAIN_TASK, then
*   make the semaphore to be invalid while MAIN_TASK is waiting for it.
*   Finally, this task posts the semaphore and blocks itself.
*
*END*----------------------------------------------------------------------*/
void wait_task(uint32_t param)
{
    void *sem_ptr;

    /* Open a connection to the semaphore SEM_NAME by calling function _sem_open */
    _sem_open(SEM_NAME, &sem_ptr);

    /* Wait and hold the semaphore SEM_NAME by calling function _sem_wait */
    _sem_wait(sem_ptr, 0);

    /* Post the lightweight semaphore lwsem by calling function _lwsem_post */
    _lwsem_post(&lwsem);
    /* Wait for lightweight semaphore lwsem until it is available by calling function _lwsem_wait */
    _lwsem_wait(&lwsem);
    /* WAIT_TASK suspends, MAIN_TASK runs from here */

    /* Change the VALID field of semaphore SEM_NAME to invalid value */
    set_sem_valid_field(SEM_NAME, SEM_VALID + INVALID_VALUE);
    /* Wait for lightweight semaphore lwsem until it is available by calling function _lwsem_wait */
    _lwsem_wait(&lwsem);
    /* WAIT_TASK suspends, MAIN_TASK runs after it finishes waiting for semaphore */

    /* Restore the VALID field of semaphore SEM_NAME to original value */
    set_sem_valid_field(SEM_NAME, SEM_VALID);
    /* Post the lightweight semaphore lwsem by calling function _lwsem_post */
    _lwsem_post(&lwsem);

    /* Post the semaphore SEM_NAME by calling function _sem_post */
    _sem_post(sem_ptr);

    /* Close the connection by calling function _sem_close */
    _sem_close(sem_ptr);

    /* Block itself, wait for destroying */
    _task_block();
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : prio_task
* Comments     :
*   This task waits for the semaphore which was created in MAIN_TASK, after
*   getting semaphore, it will changes the value of global_val.
*   Depend on task's priority, global_val will be decreased or increased.
*
*END*----------------------------------------------------------------------*/
void prio_task(uint32_t param)
{
    void *sem_ptr;

    /* Open a connection to the semaphore SEM_NAME by calling function _sem_open */
    _sem_open(SEM_NAME, &sem_ptr);

    /* Wait and hold the semaphore SEM_NAME by calling function _sem_wait */
    _sem_wait(sem_ptr, 0);

    if(param == PRIO1_TASK)
    {
        /* Increase the value of global_val */
        global_val *= 2;
    }
    else
    {
        /* Decrease the value of global_val */
        global_val /= 2;
    }

    /* Post the semaphore SEM_NAME by calling function _sem_post */
    _sem_post(sem_ptr);

    /* Close the connection by calling function _sem_close */
    _sem_close(sem_ptr);

    /* Block itself, wait for destroying */
    _task_block();
}
