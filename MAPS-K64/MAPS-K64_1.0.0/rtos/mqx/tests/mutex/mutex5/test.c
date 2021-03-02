/**HEADER********************************************************************
*
* Copyright (c) 2008-2013 Freescale Semiconductor;
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
* $Version : 4.0.0.1$
* $Date    : Apr-16-2011$
*
* Comments   : This file contains the kernel test functions for the mutex
*              component.
*
* Requirement: MUTEX022,MUTEX023,MUTEX024,MUTEX025,MUTEX026,MUTEX028,MUTEX030,
*              MUTEX032,MUTEX033,MUTEX034,MUTEX035,MUTEX036,MUTEX037,MUTEX038,
*              MUTEX039,MUTEX040,MUTEX041,MUTEX042,MUTEX043,MUTEX044,MUTEX045,
*              MUTEX046,MUTEX047,MUTEX048,MUTEX049,MUTEX050
*
*END***********************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <mqx_prv.h>
#include <mutex.h>
#include <mutex_prv.h>

#include "EUnit.h"
#include "eunit_crit_secs.h"
#include "util.h"

#define MAIN_TASK       (10)
#define TEST_TASK       (11)
#define MAIN_PRIO       (7)
#define TEST_PRIO       (8)

#define MUTEX_INVALID   (MUTEX_VALID + 1)
#define MUTEX_COUNT     (1)
#define MUTEX_PROTOCOL  (2)

void       main_task(uint32_t);
void       test_task(uint32_t);
void       set_mutex_queue_ptr(void *);
void       set_mutex_component_ptr(void *);
void       set_mutex_component_valid_field(_mqx_uint );
void       set_mutex_valid_field(MUTEX_STRUCT_PTR ,_mqx_uint);
void       set_mutex_attribute_valid_field(MUTEX_ATTR_STRUCT_PTR ,_mqx_uint);
void      *get_mutex_queue_ptr();
void      *get_mutex_component_ptr( );
_mqx_uint  get_mutex_component_valid_field( );
_mqx_uint  get_mutex_valid_field(MUTEX_STRUCT_PTR);
_mqx_uint  get_mutex_attribute_valid_field(MUTEX_ATTR_STRUCT_PTR);
/*------------------------------------------------------------------------
* required user_config settings
*-----------------------------------------------------------------------*/
#if !MQX_MUTEX_HAS_POLLING
    #error "Required MQX_MUTEX_HAS_POLLING"
#endif

/*----------------------------------------------------------------------*/
/* Test suite function prototype */
void tc_1_main_task(void);/* Test #1 - Testing function _mutex_create_component in special cases         */
void tc_2_main_task(void);/* Test #2 - Testing function _mutex_init in special cases                     */
void tc_3_main_task(void);/* Test #3 - Testing function _mutex_destroy in special cases                  */
void tc_4_main_task(void);/* Test #4 - Testing function _mutex_get/set_priority_ceiling in special cases */
void tc_5_main_task(void);/* Test #5 - Testing function _mutex_get_wait_count in special cases           */
void tc_6_main_task(void);/* Test #6 - Testing function _mutex_lock in special cases                     */
void tc_7_main_task(void);/* Test #7 - Testing function _mutex_try_lock in special cases                 */
void tc_8_main_task(void);/* Test #8 - Testing function _mutex_test in special cases                     */
void tc_9_main_task(void);/* Test #9 - Testing function _mutex_unlock in special cases                   */
//------------------------------------------------------------------------
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK,   main_task,   2000,  MAIN_PRIO,   "Main",   MQX_AUTO_START_TASK},
   { TEST_TASK,   test_task,   1000,  TEST_PRIO,   "test",           0},
   {     0    ,       0    ,      0,      0    ,        0,           0}
};

uint32_t         test_number = 0;
MUTEX_STRUCT    mutex;

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Testing function _mutex_create_component in special cases
*
* Requirement  : MUTEX022,MUTEX023
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint                  result;
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    MEMORY_ALLOC_INFO_STRUCT   memory_alloc_info;

    test_number++;

    /* Get the current kernel data */
    kernel_data = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();

    /* Intentional kernel data corruption, since mutexes are used by other components/peripherals
       and mutex kernel component already exists */
    kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES] = NULL;

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);
    _task_set_error(MQX_OK);

    /* Create the mutex component when the memory is full */
    result = _mutex_create_component();
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == MQX_OUT_OF_MEMORY , "Test #1: Testing 1.00: Creates the mutex component with MQX memory error should have returned MQX_OUT_OF_MEMORY ");
    _task_set_error(MQX_OK);

    /* Free temporary allocated memory */
    memory_free_all(&memory_alloc_info);

    /* Create the mutex component */
    result = _mutex_create_component();
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == MQX_EOK , "Test #1: Testing 1.01: Creates the mutex component when all parameter is valid ");

    /* check the mutex component was created */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES] != NULL ,"Test #1: Testing 1.02: The mutex component was created ");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Testing function _mutex_init in special cases
*
* Requirement  : MUTEX024,MUTEX033,MUTEX034,MUTEX035
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _mqx_uint            temp;
    _mqx_uint            result;
    MUTEX_ATTR_STRUCT    mutex_attr;

    test_number++;

    /* Call _mutex_init with NULL parameters should be failed */
    result = _mutex_init(NULL, NULL);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_EINVAL , "Test #2: Testing 2.00: Initializes mutex with mutex_ptr is NULL ");
    _task_set_error(MQX_OK);

    /* Initializes the mutex attributes */
    result = _mutatr_init(&mutex_attr);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_EOK , "Test #2: Testing 2.01: Initializes the mutex attribute ");

    /* Backup VALID field of mutex attribute structure */
    temp = get_mutex_attribute_valid_field(&mutex_attr);

    /* Makes the mutex attribute to invalid */
    set_mutex_attribute_valid_field(&mutex_attr, MUTEX_INVALID);

    /* Initialize the mutex when the mutex attribute is not valid */
    result = _mutex_init(&mutex, &mutex_attr);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_EINVAL , "Test #2: Testing 2.02: Initializes mutex with attr_ptr is not valid ");
    _task_set_error(MQX_OK);

    /* Restore mutex attribute to original value */
    set_mutex_attribute_valid_field(&mutex_attr, temp);

    /* Backup VALID field of mutex component structure */
    temp = get_mutex_component_valid_field();

    /* Makes the mutex component to invalid */
    set_mutex_component_valid_field(MUTEX_INVALID);

    /* Initializes the mutex when the mutex component is not valid */
    result = _mutex_init(&mutex, &mutex_attr);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_INVALID_COMPONENT_BASE , "Test #2: Testing 2.03: Initializes mutex with mutex component is not valid ");
    _task_set_error(MQX_OK);

    /* Restore VALID field of mutex component structure to original value */
    set_mutex_component_valid_field(temp);

    /* Initializes the mutex */
    result = _mutex_init(&mutex, &mutex_attr);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_EOK , "Test #2: Testing 2.04: Initializes the mutex ");

    /* Destroy mutex */
    result = _mutex_destroy(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_EOK , "Test #2: Testing 2.05: Destroy mutex that was initialized ");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Testing function _mutex_destroy in special cases
*
* Requirement  : MUTEX024,MUTEX025,MUTEX026,MUTEX033,MUTEX036,MUTEX049
*                TASK006,TASK0019
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    _task_id     test_tid;
    _mqx_uint    temp;
    _mqx_uint    result;
    void        *temp_ptr;

    test_number++;

    /* Initializes the mutex */
    result = _mutex_init(&mutex, NULL);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_EOK , "Test #3: Testing 3.00: Initializes the mutex ");

    /* The mutex components was previously created
     * so we need to zero it before doing this test */

    /* Save pointer to mutex component */
    temp_ptr = get_mutex_component_ptr();

    set_mutex_component_ptr(NULL);

    /* Destroy mutex when mutex component does not exist */
    result = _mutex_destroy(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST , "Test #3: Testing 3.01: Tests function _mutex_destroy with mutex component was not previously creasted ");
    _task_set_error(MQX_OK);

    /* restore kernel mutex component to original value */
    set_mutex_component_ptr(temp_ptr);

    /* Backup VALID field of mutex component structure */
    temp = get_mutex_component_valid_field();

    /* Makes the mutex component to invalid */
    set_mutex_component_valid_field(MUTEX_INVALID);

    /* Destroy mutex when the mutex component is not valid*/
    result = _mutex_destroy(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_INVALID_COMPONENT_BASE , "Test #3: Testing 3.02: Tests function _mutex_destroy with mutex component is not valid ");
    _task_set_error(MQX_OK);

    /* Restore VALID field of mutex component to original value */
    set_mutex_component_valid_field(temp);

    mutex.PROTOCOLS = MUTEX_PROTOCOL;
    mutex.COUNT     = MUTEX_COUNT;
    /* Creates TEST_TASK */
    test_tid = _task_create(0, TEST_TASK, test_number);

    /* Suspend main task and allow TEST_TASK run */
    _time_delay_ticks(10);

    /* Destroys mutex when it was locked by other task */
    result = _mutex_destroy(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_EBUSY , "Test #3: Testing 3.04: Tests function _mutex_destroy with the mutex was locked by other task ");
    _task_set_error(MQX_OK);

    /* Suspend main task and allow TEST_TASK run */
    _time_delay_ticks(10);

    /* Destroy TEST_TASK */
    result = _task_destroy(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_OK , "Test #3: Testing 3.06: Destroys TEST_TASK that was create by main task ");

    /* Destroy mutex */
    result = _mutex_destroy(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_EOK , "Test #3: Testing 3.07: Destroys mutex that was initialized ");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Testing function _mutex_get_priority_ceiling,
*                _mutex_set_priority_ceiling in special cases.
*
* Requirement  : MUTEX024,MUTEX028,MUTEX030,MUTEX033
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    _mqx_uint               temp;
    _mqx_uint               result;
    _mqx_uint               odl_ceil,ceil;
    KERNEL_DATA_STRUCT_PTR  kernel_data;

    test_number++;

    kernel_data = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();

    /* Gets priority of the mutex when pointer to the mutex is NULL */
    result = _mutex_get_priority_ceiling(NULL, &ceil);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_EINVAL , "Test #4: Testing 4.00: Tests function _mutex_get_priority_ceiling with mutex_ptr is NULL ");
    _task_set_error(MQX_OK);

    /* Initializes the mutex */
    result = _mutex_init(&mutex, NULL);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_EOK , "Test #4: Testing 4.01: Initializes the mutex");

    ceil = TEST_TASK;
    /* Sets priority of the mutex that is NULL */
    result = _mutex_set_priority_ceiling(NULL, ceil, &odl_ceil);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_EINVAL , "Test #4: Testing 4.02: Tests function _mutex_set_priority_ceiling with mutex_ptr is NUL ");
    _task_set_error(MQX_OK);

    ceil = kernel_data->LOWEST_TASK_PRIORITY + 1;
    /* Sets priority of the mutex invalid priority */
    result = _mutex_set_priority_ceiling(&mutex, ceil, &odl_ceil);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_EINVAL , "Test #4: Testing 4.03: Tests function _mutex_set_priority_ceiling with new priority is not valid ");
    _task_set_error(MQX_OK);

    /* Backup VALID field of mutex structure */
    temp = get_mutex_valid_field(&mutex);

    /* Makes the mutex to invalid */
    set_mutex_valid_field(&mutex, MUTEX_INVALID);

    /* Gets priority of the mutex when the mutex is not valid */
    result = _mutex_get_priority_ceiling(&mutex, &ceil);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_EINVAL , "Test #4: Testing 4.04: Tests function _mutex_get_priority_ceiling with the mutex_ptr is not valid ");
    _task_set_error(MQX_OK);

    /* Sets priority of the mutex when the mutex is not valid */
    result = _mutex_set_priority_ceiling(&mutex, ceil, &odl_ceil);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_EINVAL , "Test #4: Testing 4.05: Tests function _mutex_set_priority_ceiling with the mutex is not valid ");
    _task_set_error(MQX_OK);

    /* restore mutex */
    set_mutex_valid_field(&mutex,temp);

    /* Destroy mutex */
    result = _mutex_destroy(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_EOK , "Test #4: Testing 4.06: Destroys mutex that was initialized ");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Testing function _mutex_get_wait_count in special cases.
*
* Requirement  : MUTEX024,MUTEX032,MUTEX033
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    _mqx_uint  temp;
    _mqx_uint  result;

    test_number++;

    /* Gets the number of tasks that are waiting for the mutex when pointer to the mutex is NULL */
    result = _mutex_get_wait_count(NULL);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == MAX_MQX_UINT , "Test #5: Testing 5.00: Tests function _mutex_get_wait_count with the mutex_ptr is not valid ");
    _task_set_error(MQX_OK);

    /* Initializes the mutex */
    result = _mutex_init(&mutex,NULL);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == MQX_EOK , "Test #5: Testing 5.01: Initializes the mutex");

    /* Backup VALID field of mutex structure */
    temp = get_mutex_valid_field(&mutex);

    /* Makes the mutex to invalid */
    set_mutex_valid_field(&mutex,MUTEX_INVALID);

    /* Gets the number of tasks that are waiting for the mutex when the mutex is not valid */
    result = _mutex_get_wait_count(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == MAX_MQX_UINT , "Test #5: Testing 5.02: Tests function _mutex_get_wait_count with the mutex_ptr is not valid ");
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, _task_errno == MQX_EINVAL , "Test #5: Testing 5.03: Checks the _task_errno must be return MQX_EINVAL ");
    _task_set_error(MQX_OK);

    /* restore mutex */
    set_mutex_valid_field(&mutex, temp);

    /* Destroy mutex */
    result = _mutex_destroy(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == MQX_EOK , "Test #5: Testing 5.04: Destroys mutex that was initialized ");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Testing function _mutex_lock in special cases.
*
* Requirement  : MUTEX024,MUTEX033,MUTEX036,MUTEX037,MUTEX038,MUTEX039,
*                MUTEX040,MUTEX049
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    _task_id                test_tid;
    _mqx_uint               temp;
    _mqx_uint               result;
    KERNEL_DATA_STRUCT_PTR  kernel_data;

    test_number++;

    kernel_data = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();

    /* Locks mutex when pointer to the mutex is NULL */
    result = _mutex_lock(NULL);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_EINVAL , "Test #6: Testing 6.00: Tests function _mutex_lock with mutex_ptr is NULL ");
    _task_set_error(MQX_OK);

    /* Initializes the mutex */
    result = _mutex_init(&mutex, NULL);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_EOK , "Test #6: Testing 6.01: Initializes the mutex");

    _int_disable();
    temp = kernel_data->IN_ISR;
    kernel_data->IN_ISR = 1;
    /* Calling this function in ISR */
    result = _mutex_lock(&mutex);
    kernel_data->IN_ISR = temp;
    _int_enable();

    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR , "Test #6: Testing 6.02: Cannot call function _mutex_test in the ISR ");
    _task_set_error(MQX_OK);

    /* Backup VALID field of mutex structure */
    temp = get_mutex_valid_field(&mutex);

    /* Makes the mutex to invalid */
    set_mutex_valid_field(&mutex, MUTEX_INVALID);

    /* Lock mutex when the mutex is not valid */
    result = _mutex_lock(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_EINVAL , "Test #6: Testing 6.03: Tests function _mutex_lock with the mutex_ptr is not valid ");
    _task_set_error(MQX_OK);

    /* Restore mutex */
    set_mutex_valid_field(&mutex, temp);

    mutex.PROTOCOLS = MUTEX_PROTOCOL;
    mutex.COUNT     = MUTEX_COUNT;
    /* Creates TEST_TASK */
    test_tid = _task_create(0, TEST_TASK, test_number);

    /* Suspends main task and allow TEST_TASK run */
    _time_delay_ticks(10);

    /* Locks mutex when it was locked by other task */
    result = _mutex_lock(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_EBUSY , "Test #6: Testing 6.05: Tests function _mutex_lock with the mutex was locked by other task ");
    _task_set_error(MQX_OK);

    _time_delay_ticks(10);

    /* Locks mutex */
    result = _mutex_lock(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_EOK , "Test #6: Testing 6.07: The mutex is locked by main task ");

    /* Locks mutex when the task already has the mutex locked */
    result = _mutex_lock(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_EDEADLK , "Test #6: Testing 6.08: Tests function _mutex_lock with the task already has the mutex locked ");
    _task_set_error(MQX_OK);

    /* Destroy mutex */
    result = _mutex_destroy(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_EOK , "Test #6: Testing 6.09: Destroys mutex that was initialized ");

    /* Destroy TEST_TASK */
    result = _task_destroy(test_tid);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7: Testing function _mutex_try_lock in special cases.
*
* Requirement  : MUTEX024,MUTEX033,MUTEX045,MUTEX046,MUTEX047,MUTEX048,
*                MUTEX040,MUTEX049
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    _task_id    test_tid;
    _mqx_uint   temp;
    _mqx_uint   result;

    test_number++;

    /* Try lock a NULL mutex */
    result = _mutex_try_lock(NULL);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_EINVAL , "Test #7: Testing 7.00: Tests function _mutex_try_lock with mutex_ptr is NULL ");
    _task_set_error(MQX_OK);

    /* Initializes the mutex */
    result = _mutex_init(&mutex, NULL);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_EOK , "Test #7: Testing 7.01: Initializes the mutex");

    /* Backup VALID field of mutex structure */
    temp = get_mutex_valid_field(&mutex);

    /* Makes the mutex to invalid */
    set_mutex_valid_field(&mutex, MUTEX_INVALID);

    /* Tries to lock mutex when the mutex is not valid */
    result = _mutex_try_lock(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_EINVAL , "Test #7: Testing 7.02: Tests function _mutex_try_lock with the mutex_ptr is not valid ");
    _task_set_error(MQX_OK);

    /* Restore mutex */
    set_mutex_valid_field(&mutex, temp);

    mutex.PROTOCOLS = MUTEX_PROTOCOL;
    mutex.COUNT     = MUTEX_COUNT;
    /* Creates TEST_TASK */
    test_tid = _task_create(0, TEST_TASK, test_number);

    /* Suspend main task and allow TEST_TASK run */
    _time_delay_ticks(10);

    /* Tries to lock mutex was locked by TEST_TASK */
    result = _mutex_try_lock(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_EBUSY , "Test #7: Testing 7.04: Tests function _mutex_try_lock with the mutex was locked by other task ");
    _task_set_error(MQX_OK);

    /* Suspend main task and allow TEST_TASK run */
    _time_delay_ticks(10);

    /* Locks the mutex*/
    result = _mutex_try_lock(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_EOK , "Test #7: Testing 7.06: Tests function _mutex_lock ");

    result = _mutex_try_lock(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_EDEADLK , "Test #7: Testing 7.07: Tests function _mutex_test with mutex component is not valid ");
    _task_set_error(MQX_OK);

    /* Destroy mutex */
    result = _mutex_destroy(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_EOK , "Test #7: Testing 7.08: Destroys mutex that was created ");

    /* Destroy TEST_TASK */
    result = _task_destroy(test_tid);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8: Testing function _mutex_test in special cases.
*
* Requirement  : MUTEX024,MUTEX033,MUTEX041,MUTEX042,MUTEX043,MUTEX044
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
    void             *temp_ptr;
    _mqx_uint         temp;
    _mqx_uint         result;
    MUTEX_STRUCT_PTR  mutex_in_error_ptr;

    test_number++;

    /* The mutex components was previously created
     * so we need to zero it before doing this test */

    /* Initializes the mutex */
    result = _mutex_init(&mutex, NULL);
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, result == MQX_EOK , "Test #8: Testing 8.00: Initializes the mutex");

    /* Backup VALID field of mutex component structure */
    temp = get_mutex_component_valid_field();

    /* Makes the mutex component to invalid */
    set_mutex_component_valid_field(MUTEX_INVALID);

    /* Test mutex when the component data is not valid */
    result = _mutex_test( (void **)(&mutex_in_error_ptr) );
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, result == MQX_INVALID_COMPONENT_BASE , "Test #8: Testing 8.01: Tests function _mutex_test with mutex component data is not valid ");
    _task_set_error(MQX_OK);

    /* Restore VALID field of mutex component structure to original value */
    set_mutex_component_valid_field(temp);

    /* Backup pointer to the mutex queue */
    temp_ptr = get_mutex_queue_ptr();

    /* Makes the mutex queue to invalid */
    set_mutex_queue_ptr(NULL);

    /* Test mutex when the mutex queue is not valid */
    result = _mutex_test( (void **)(&mutex_in_error_ptr) );
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, result == MQX_CORRUPT_QUEUE , "Test #8: Testing 8.02: Tests function _mutex_test with the mutex queue is not valid ");
    _task_set_error(MQX_OK);

    /* Restores the mutex queue */
    set_mutex_queue_ptr(temp_ptr);

    /* Backup VALID field of mutex structure */
    temp = get_mutex_valid_field(&mutex);

    /* Makes the mutex to invalid */
    set_mutex_valid_field(&mutex, MUTEX_INVALID);

    /* Test mutex when either mutex or mutex queue is not valid */
    result = _mutex_test( (void **)(&mutex_in_error_ptr) );
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, result == MQX_EINVAL , "Test #8: Testing 8.03: Tests function _mutex_test with mutex is not valid ");
    _task_set_error(MQX_OK);

    /* Restore the VALID field of mutex to original value */
    set_mutex_valid_field(&mutex, temp);

    result = _mutex_test( (void **)(&mutex_in_error_ptr) );
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, result == MQX_OK , "Test #8: Testing 8.04: Tests function _mutex_test in normal operation ");
    _task_set_error(MQX_OK);

    /* Destroy mutex */
    result = _mutex_destroy(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, result == MQX_EOK , "Test #8: Testing 8.05: Destroys mutex that was initialized ");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : TEST #9: Testing function _mutex_unlock in special cases.
*
* Requirement  : MUTEX024,MUTEX033,MUTEX049,MUTEX050
*
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
    _task_id    test_tid;
    _mqx_uint   temp;
    _mqx_uint   result;

    test_number++;

    /* Unlocks mutex when the pointer to mutex is NULL */
    result = _mutex_unlock(NULL);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_EINVAL , "Test #9: Testing 9.00: Tests function _mutex_unlock with mutex_ptr is NULL");
    _task_set_error(MQX_OK);

    /* Initializes the mutex */
    result = _mutex_init(&mutex,NULL);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_EOK , "Test #9: Testing 9.01: Initializes the mutex");

    /* Backup VALID field of mutex structure */
    temp = get_mutex_valid_field(&mutex);

    /* Makes the mutex to invalid */
    set_mutex_valid_field(&mutex, MUTEX_INVALID);

    /* Unlocks mutex when it is not valid */
    result = _mutex_unlock(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_EINVAL , "Test #9: Testing 9.02: Tests function _mutex_unlock with the mutex is not valid ");
    _task_set_error(MQX_OK);

    /* Restore the VALID field of mutex to original value */
    set_mutex_valid_field(&mutex, temp);

    /* Unlocks mutex when it was not locked */
    result = _mutex_unlock(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_EINVAL , "Test #9: Testing 9.03: Tests function _mutex_unlock with it was not locked ");
    _task_set_error(MQX_OK);

    mutex.PROTOCOLS = MUTEX_PROTOCOL;
    mutex.COUNT     = MUTEX_COUNT;
    /* Creates TEST_TASK */
    test_tid = _task_create(0, TEST_TASK, test_number);

    /* Suspend main task and allow TEST_TASK run */
    _time_delay_ticks(10);

    /* Unlocks mutex when it was locked by other task */
    result = _mutex_unlock(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_EINVAL , "Test #9: Testing 9.05: Tests function _mutex_unlock with the mutex was locked by other task ");
    _task_set_error(MQX_OK);

    /* Suspend main task and allow TEST_TASK run */
    _time_delay_ticks(10);

    /* Destroy mutex */
    result = _mutex_destroy(&mutex);
    EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_EOK , "Test #9: Testing 9.07: Destroys mutex in normal operation ");

    /* Destroy TEST_TASK */
    result = _task_destroy(test_tid);
}
/******  Define Test Suite      *******/

 EU_TEST_SUITE_BEGIN(suite_mutex5)
     EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Testing function _mutex_create_component in special cases"),
     EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Testing function _mutex_init in special cases"),
     EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Testing function _mutex_destroy in special cases"),
     EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - Testing function _mutex_get/set_priority_ceiling in special cases"),
     EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - Testing function _mutex_get_wait_count in special cases"),
     EU_TEST_CASE_ADD(tc_6_main_task, " Test #6 - Testing function _mutex_lock in special cases"),
     EU_TEST_CASE_ADD(tc_7_main_task, " Test #7 - Testing function _mutex_try_lock in special cases"),
     EU_TEST_CASE_ADD(tc_8_main_task, " Test #8 - Testing function _mutex_test in special cases"),
     EU_TEST_CASE_ADD(tc_9_main_task, " Test #9 - Testing function _mutex_unlock in special cases"),
 EU_TEST_SUITE_END(suite_mutex5)

/*******  Add test suites        *******/
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_mutex5, "Tests functions for mutex")
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
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* Endbody */

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : test_task
* Return Value  : void
*
*END*----------------------------------------------------------------------*/
void test_task
   (
      uint32_t test_no
   )
{
    _mqx_uint result;

    /* lock mutex that is initialized by main task */
    result = _mutex_lock(&mutex);

    if (test_no == 3) {
        EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_EOK , "Test #3: Testing 3.03: lock mutex in normal operation ");
    }
    else if (test_no == 6) {
        EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_EOK , "Test #6: Testing 6.04: lock mutex in normal operation ");
    }
    else if (test_no == 7) {
        EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_EOK , "Test #7: Testing 7.03: lock mutex in normal operation ");
    }
    else if (test_no == 9) {
        EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_EOK , "Test #9: Testing 9.04: lock mutex in normal operation ");
    }

    /* suspend TEST_TASK and allow the main task run */
   _time_delay_ticks(10);

   /* Unlock mutex */
   result = _mutex_unlock(&mutex);

    if (test_no == 3){
        EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_EOK , "Test #3: Testing 3.05: unlock mutex in normal operation ");
    }
    else if (test_no == 6){
        EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_EOK , "Test #6: Testing 6.06: unlock mutex in normal operation ");
    }
    else if (test_no == 7) {
        EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == MQX_EOK , "Test #7: Testing 7.05: unlock mutex in normal operation ");
    }
    else if (test_no == 9) {
        EU_ASSERT_REF_TC_MSG( tc_9_main_task, result == MQX_EOK , "Test #9: Testing 9.06: unlock mutex in normal operation ");
    }

   _task_block();
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_mutex_attribute_valid_field
* Return Value  : Value of VALID field
* Comments      : get the VALID field value of mutex attribute structure
*
*END*----------------------------------------------------------------------*/
_mqx_uint get_mutex_attribute_valid_field
   (
      MUTEX_ATTR_STRUCT_PTR mutatr_ptr
   )
{
   return (_mqx_uint)mutatr_ptr->VALID;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_mutex_attribute_valid_field
* Return Value  : void
* Comments      : set the VALID field value of mutex attribute structure
*
*END*----------------------------------------------------------------------*/
void set_mutex_attribute_valid_field
   (
      MUTEX_ATTR_STRUCT_PTR mutatr_ptr,
      _mqx_uint             mutatr_valid
   )
{
   mutatr_ptr->VALID = mutatr_valid;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_mutex_valid_field
* Return Value  : Value of VALID field
* Comments      : get the VALID field value of mutex structure
*
*END*----------------------------------------------------------------------*/
_mqx_uint get_mutex_valid_field
   (
      MUTEX_STRUCT_PTR mutex_ptr
   )
{
    return (_mqx_uint)mutex_ptr->VALID;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_mutex_valid_field
* Return Value  : void
* Comments      : set the VALID field value of mutex structure
*
*END*----------------------------------------------------------------------*/
void set_mutex_valid_field
   (
      MUTEX_STRUCT_PTR mutex_ptr,
      _mqx_uint        mutex_valid
   )
{
   mutex_ptr->VALID = mutex_valid;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_mutex_component_valid_field
* Return Value  : Value of VALID field
* Comments      : Gets the VALID field value of mutex component structure
*
*END*----------------------------------------------------------------------*/
_mqx_uint get_mutex_component_valid_field
   (
      void
   )
{
   KERNEL_DATA_STRUCT_PTR        kernel_data;
   MUTEX_COMPONENT_STRUCT_PTR    mutex_component_ptr;

   _GET_KERNEL_DATA(kernel_data);
   mutex_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES];
   return mutex_component_ptr->VALID ;

}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_mutex_component_valid_field
* Return Value  : void
* Comments      : set the VALID field value of mutex component structure
*
*END*----------------------------------------------------------------------*/
void set_mutex_component_valid_field
   (
      _mqx_uint wat_valid
   )
{
   KERNEL_DATA_STRUCT_PTR        kernel_data;
   MUTEX_COMPONENT_STRUCT_PTR    mutex_component_ptr;

   _GET_KERNEL_DATA(kernel_data);

   /* Disable all interrupt */
   _int_disable();
   mutex_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES];
   mutex_component_ptr->VALID = wat_valid;

   /* Enable all interrupt */
   _int_enable();
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_mutex_component_ptr
* Return Value  : pointer to kernel component
* Comments      : gets the pointer to kernel component
*
*END*----------------------------------------------------------------------*/
void *get_mutex_component_ptr
   (
      void
   )
{
   KERNEL_DATA_STRUCT_PTR        kernel_data;

   _GET_KERNEL_DATA(kernel_data);

   return  (void *)kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES];

}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_mutex_component_ptr
* Return Value  : void
* Comments      : set the kernel component pointer.
*
*END*----------------------------------------------------------------------*/
void set_mutex_component_ptr
   (
      void   *mut_ptr
   )
{
   KERNEL_DATA_STRUCT_PTR     kernel_data;

   _GET_KERNEL_DATA(kernel_data);

   /* Disable all interrupt */
   _int_disable();

   kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES] = (MUTEX_COMPONENT_STRUCT_PTR)mut_ptr;

   /* Enable all interrupt */
   _int_enable();
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_queue_ptr
* Return Value  : void
* Comments      : gets the pointer to the mutex queue
*
*END*----------------------------------------------------------------------*/
void *get_mutex_queue_ptr
   (
      void
   )
{
   KERNEL_DATA_STRUCT_PTR     kernel_data;
   QUEUE_ELEMENT_STRUCT_PTR   element_ptr;
   MUTEX_COMPONENT_STRUCT_PTR mutex_component_ptr;

   _GET_KERNEL_DATA(kernel_data);

   mutex_component_ptr = (MUTEX_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES];
   QUEUE_STRUCT_PTR q_ptr = (QUEUE_STRUCT_PTR)&mutex_component_ptr->MUTEXES;

   element_ptr = q_ptr->NEXT;
   return (void *)element_ptr->PREV;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_queue_ptr
* Return Value  : void
* Comments      : set the kernel component pointer.
*
*END*----------------------------------------------------------------------*/
void set_mutex_queue_ptr
   (
      void   *temp_ptr
   )
{
   KERNEL_DATA_STRUCT_PTR     kernel_data;
   QUEUE_ELEMENT_STRUCT_PTR   element_ptr;
   MUTEX_COMPONENT_STRUCT_PTR mutex_component_ptr;

   _GET_KERNEL_DATA(kernel_data);

   mutex_component_ptr = (MUTEX_COMPONENT_STRUCT_PTR) kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES];
   QUEUE_STRUCT_PTR q_ptr = (QUEUE_STRUCT_PTR)&mutex_component_ptr->MUTEXES;

   _int_disable();
   element_ptr = q_ptr->NEXT;
   element_ptr->PREV = temp_ptr;
   _int_enable();
}

/* EOF */
