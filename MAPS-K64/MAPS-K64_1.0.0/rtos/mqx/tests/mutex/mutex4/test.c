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
* $Version : 4.0.1.1$
* $Date    : Apr-14-2013$
*
* Comments   : This file contains the kernel test functions for the mutex
*              component.
* Requirement: MUTEX002,MUTEX004,MUTEX006,MUTEX008,MUTEX010,MUTEX012,
*              MUTEX014,MUTEX016,MUTEX018,MUTEX019,MUTEX020
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
#define MAIN_PRIO       (7)

#define INVALID_NUMBER  (0xFFFF)
#define MUTEX_INVALID   (MUTEX_VALID + 1)

MUTEX_ATTR_STRUCT       mutex_attr;

void       main_task(uint32_t);
void       set_mutex_attribute_valid_field(MUTEX_ATTR_STRUCT_PTR ,_mqx_uint);
_mqx_uint  get_mutex_attribute_valid_field(MUTEX_ATTR_STRUCT_PTR);

/*------------------------------------------------------------------------
* required user_config settings
*------------------------------------------------------------------------*/
#if !MQX_MUTEX_HAS_POLLING
    #error "Required MQX_MUTEX_HAS_POLLING"
#endif

/*------------------------------------------------------------------------*/
/* Test suite function prototype*/
void tc_1_main_task(void);/* Test #1: Testing function _mutatr_init in special cases                      */
void tc_2_main_task(void);/* Test #2: Testing function _mutatr_destroy in special cases                   */
void tc_3_main_task(void);/* Test #3: Testing functions _mutatr_get/set_priority_ceiling in special cases */
void tc_4_main_task(void);/* Test #4: Testing functions _mutatr_get/set_sched_protocol in special cases   */
void tc_5_main_task(void);/* Test #5: Testing functions _mutatr_get/set_spin_limit in special cases       */
void tc_6_main_task(void);/* Test #6: Testing functions _mutatr_get/set_wait_protocol in special cases    */
/*------------------------------------------------------------------------*/
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK,  main_task,  2000,  MAIN_PRIO,  "Main",  MQX_AUTO_START_TASK},
   {     0    ,      0    ,     0,      0    ,       0,           0         }
};

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Testing function _mutatr_init in special cases
*
* Requirement  : MUTEX019,MUTEX020
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint   result;
    KERNEL_DATA_STRUCT_PTR     kernel_data;

    /* Intentional kernel data corruption, since mutexes are used by other components/peripherals
       and mutex kernel component already exists */
    kernel_data = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();
    kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES] = NULL;

    /* Initializes the mutex attribute with NULL input pointer should return MQX_EINVAL */
    result = _mutatr_init(NULL);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == MQX_EINVAL , "Test #1: Testing 1.00: Initializes mutex attribute with attr_ptr is NULL ");
    _task_set_error(MQX_OK);

    /* Initializes the mutex attribute */
    result = _mutatr_init(&mutex_attr);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == MQX_EOK , "Test #1: Testing 1.01: Initializes the mutex attribute ");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Testing function _mutatr_destroy in special cases
*
* Requirement  : MUTEX002
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _mqx_uint   result;
    _mqx_uint   temp;

    /* Destroy the NULL mutex attributes should return MQX_EINVAL */
    result = _mutatr_destroy(NULL);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_EINVAL , "Test #2: Testing 2.00: Tests function _mutatr_destroy with attr_ptr is NULL ");
    _task_set_error(MQX_OK);

    /* Backup VALID field of mutex attribute structure */
    temp = get_mutex_attribute_valid_field(&mutex_attr);

    /* Makes the mutex attribute to invalid */
    set_mutex_attribute_valid_field(&mutex_attr, MUTEX_INVALID);

    /* Destroy the invalid mutex attributes should return MQX_EINVAL */
    result = _mutatr_destroy(&mutex_attr);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_EINVAL , "Test #2: Testing 2.01: Tests function _mutatr_destroy with mutex attribute is not valid ");
    _task_set_error(MQX_OK);

    /* Restore the mutex attribute */
    set_mutex_attribute_valid_field(&mutex_attr, temp);

    /* Destroy the mutex atrribute */
    result = _mutatr_destroy(&mutex_attr);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_OK , "Test #2: Testing 2.02: Tests function _mutatr_destroy in normal operation ");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Testing function _mutatr_get_priority_ceiling,
*                _mutatr_set_priority_ceiling in special cases.
*
* Requirement  : MUTEX004,MUTEX006
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    KERNEL_DATA_STRUCT_PTR   kernel_data;
    _mqx_uint                ceil;
    _mqx_uint                temp;
    _mqx_uint                result;

    /* Get the current kernel data*/
    _GET_KERNEL_DATA(kernel_data);

    /* Initializes the mutex attribute */
    result = _mutatr_init(&mutex_attr);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_EOK , "Test #3: Testing 3.00: Initializes the mutex attribute ");

    /* Gets the priority ceiling of the NULL mutex attributes should return MQX_EINVAL */
    result = _mutatr_get_priority_ceiling(NULL, &ceil);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_EINVAL , "Test #3: Testing 3.01: Tests function _mutatr_get_priority_ceiling with attr_ptr is NULL ");
    _task_set_error(MQX_OK);

    /* Gets the priority ceiling of a mutex attributes with NULL parameter should return MQX_EINVAL */
    result = _mutatr_get_priority_ceiling(&mutex_attr, NULL);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_EINVAL , "Test #3: Testing 3.02: Tests function _mutatr_get_priority_ceiling with priority_ptr is NULL ");
    _task_set_error(MQX_OK);

    /* Backup VALID field of mutex attribute structure */
    temp = get_mutex_attribute_valid_field(&mutex_attr);

    /* Makes the mutex_attr to invalid */
    set_mutex_attribute_valid_field(&mutex_attr, MUTEX_INVALID);

    /* Gets the priority ceiling of invalid mutex attributes should return MQX_EINVAL */
    result = _mutatr_get_priority_ceiling(&mutex_attr, &ceil);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_EINVAL , "Test #3: Testing 3.03: Tests function _mutatr_get_priority_ceiling with mutex attribute is not valid ");
    _task_set_error(MQX_OK);

    /* Sets the priority ceiling of the invalid mutex attributes should return MQX_EINVAL */
    result = _mutatr_set_priority_ceiling(&mutex_attr, ceil);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_EINVAL , "Test #3: Testing 3.04: Tests function _mutatr_set_priority_ceiling with mutex attribute is not valid ");
    _task_set_error(MQX_OK);

    /* restore mutex_attr */
    set_mutex_attribute_valid_field(&mutex_attr, temp);

    /* Sets the priority value of the NULL mutex attributes should be failed */
    result = _mutatr_set_priority_ceiling(NULL, ceil);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_EINVAL , "Test #3: Testing 3.05: Tests function _mutatr_set_priority_ceiling with attr_ptr is NULL ");
    _task_set_error(MQX_OK);

    ceil = kernel_data->LOWEST_TASK_PRIORITY + 1;

    /* Sets the priority value of the mutex attributes structure with invalid priority should return MQX_EINVAL */
    result = _mutatr_set_priority_ceiling(&mutex_attr, ceil);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_EINVAL , "Test #3: Testing 3.06: Tests function _mutatr_set_priority_ceiling with the new priority is not valid ");
    _task_set_error(MQX_OK);

    /* Destroy the mutex atrribute */
    result = _mutatr_destroy(&mutex_attr);
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_OK , "Test #3: Testing 3.07: Destroy the mutex attribute ");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Testing function _mutatr_get_sched_protocol,
*                _mutatr_set_sched_protocol in special cases.
*
* Requirement  : MUTEX008,MUTEX010
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    _mqx_uint   temp;
    _mqx_uint   result;
    _mqx_uint   prot;

    /* Initializes the mutex attribute */
    result = _mutatr_init(&mutex_attr);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_EOK , "Test #4: Testing 4.00: Initializes the mutex attribute ");

    /* Gets the scheduling protocol of the NULL mutex attributes should be failed */
    result = _mutatr_get_sched_protocol(NULL, &prot);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_EINVAL , "Test #4: Testing 4.01: Tests function _mutatr_get_sched_protocol with attr_ptr is NULL ");
    _task_set_error(MQX_OK);

    /* Gets the scheduling protocol with input parameter is NULL should be failed */
    result = _mutatr_get_sched_protocol(&mutex_attr, NULL);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_EINVAL , "Test #4: Testing 4.02: Tests function _mutatr_get_sched_protocol with protocol_ptr is NULL ");
    _task_set_error(MQX_OK);

    /* Backup VALID field of mutex attribute structure */
    temp = get_mutex_attribute_valid_field(&mutex_attr);

    /* Makes the mutex_attr to invalid */
    set_mutex_attribute_valid_field(&mutex_attr, MUTEX_INVALID);

    /* Gets the scheduling protocol of the invalid mutex attributes should be failed */
    result = _mutatr_get_sched_protocol(&mutex_attr, &prot);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_EINVAL , "Test #4: Testing 4.03: Tests function _mutatr_get_sched_protocol with mutex attribute is not valid ");
    _task_set_error(MQX_OK);

    /* Sets the scheduling protocol of the invalid mutex attributes */
    result = _mutatr_set_sched_protocol(&mutex_attr, prot);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_EINVAL , "Test #4: Testing 4.04: Tests function _mutatr_set_sched_protocol with mutex attribute is not valid ");
    _task_set_error(MQX_OK);

    /* restore mutex_attr */
    set_mutex_attribute_valid_field(&mutex_attr, temp);

    /* Sets the scheduling protocol of the NULL mutex attributes should be failed */
    result = _mutatr_set_sched_protocol(NULL, prot);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_EINVAL , "Test #4: Testing 4.05: Tests function _mutatr_set_sched_protocol with attr_ptr is NULL ");
    _task_set_error(MQX_OK);

    /* Sets the scheduling protocol of the mutex attributes with invalid protocol should be failed */
    result = _mutatr_set_sched_protocol(&mutex_attr, INVALID_NUMBER);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_EINVAL , "Test #4: Testing 4.06: Tests function _mutatr_set_sched_protocol with protocol value is not valid ");
    _task_set_error(MQX_OK);

    /* Destroy the mutex atrribute */
    result = _mutatr_destroy(&mutex_attr);
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_OK , "Test #4: Testing 4.07: Destroy the mutex attribute ");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Testing function _mutatr_get_spin_limit,
*                _mutatr_set_spin_limit in special cases.
* Requirement  : MUTEX012,MUTEX014
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    _mqx_uint   temp;
    _mqx_uint   result;
    _mqx_uint   spin_number;

     /* Initializes the mutex attribute */
    result = _mutatr_init(&mutex_attr);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == MQX_EOK , "Test #5: Testing 5.00: Initializes the mutex attribute ");

    /* Gets the spin limit of the NULL mutex attributes should be failed */
    result = _mutatr_get_spin_limit(NULL, &spin_number);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == MQX_EINVAL , "Test #5: Testing 5.01: Tests function _mutatr_get_spin_limit with attr_ptr is NULL ");
    _task_set_error(MQX_OK);

    /* Gets the spin limit of the mutex attributes with NULL parameter should be failed */
    result = _mutatr_get_spin_limit(&mutex_attr, NULL);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == MQX_EINVAL , "Test #5: Testing 5.02: Tests function _mutatr_get_spin_limit with spin_count_ptr is NULL");
    _task_set_error(MQX_OK);

    /* Backup VALID field of mutex attribute structure */
    temp = get_mutex_attribute_valid_field(&mutex_attr);

    /* Makes the mutex attribute to invalid */
    set_mutex_attribute_valid_field(&mutex_attr, MUTEX_INVALID);

    /* Gets the spin limit of the invalid mutex attributes should be failed */
    result = _mutatr_get_spin_limit(&mutex_attr, &spin_number);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == MQX_EINVAL , "Test #5: Testing 5.03: Tests function _mutatr_get_spin_limit with mutex attribute is NULL ");
    _task_set_error(MQX_OK);

    /* Sets the spin limit of the invalid mutex attributes should be failed */
    result = _mutatr_set_spin_limit(&mutex_attr, spin_number);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == MQX_EINVAL , "Test #5: Testing 5.04: Tests function _mutatr_set_spin_limit with mutex attribute is not valid ");
    _task_set_error(MQX_OK);

    /* Restore mutex_attr */
    set_mutex_attribute_valid_field(&mutex_attr, temp);

    /* Sets the spin limit of the NULL mutex attributes should be failed */
    result = _mutatr_set_spin_limit(NULL, spin_number);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == MQX_EINVAL , "Test #5: Testing 5.05: Tests function _mutatr_set_spin_limit with attr_ptr is NULL ");
    _task_set_error(MQX_OK);

    /* Destroy the mutex atrribute */
    result = _mutatr_destroy(&mutex_attr);
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, result == MQX_OK , "Test #5: Testing 5.06: Destroy the mutex attribute ");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Testing function _mutatr_get_wait_protocol,
*                _mutatr_get_wait_protocol in special cases.
* Requirement  : MUTEX016,MUTEX018
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    _mqx_uint   temp;
    _mqx_uint   result;
    _mqx_uint   wait_pro;

    /* Initializes the mutex attribute */
    result = _mutatr_init(&mutex_attr);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_EOK , "Test #1: Testing 6.00: Initializes the mutex attribute ");

    /* gets the waiting policy of the NULL mutex attributes should be failed */
    result = _mutatr_get_wait_protocol(NULL, &wait_pro);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_EINVAL , "Test #6: Testing 6.01: Tests function _mutatr_get_wait_protocol with attr_ptr is NULL ");
    _task_set_error(MQX_OK);

    /* gets the waiting policy of the mutex attributes with NULL parameter should be failed */
    result = _mutatr_get_wait_protocol(&mutex_attr, NULL);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_EINVAL , "Test #6: Testing 6.02: Tests function _mutatr_get_wait_protocol with waiting_protocol_ptr is NULL ");
    _task_set_error(MQX_OK);

    /* Backup VALID field of mutex attribute structure */
    temp = get_mutex_attribute_valid_field(&mutex_attr);

    /* Makes the mutex attribute to invalid */
    set_mutex_attribute_valid_field(&mutex_attr, MUTEX_INVALID);

    /* gets the waiting policy of the invalid mutex attributes should be failed */
    result = _mutatr_get_wait_protocol(&mutex_attr, &wait_pro);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_EINVAL , "Test #6: Testing 6.03: Tests function _mutatr_get_wait_protocol with mutex attribute is not valid ");
    _task_set_error(MQX_OK);

    /* sets the waiting policy of the invalid mutex attributes should be failed */
    result = _mutatr_set_wait_protocol(&mutex_attr, wait_pro);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_EINVAL , "Test #6: Testing 6.04: Tests function _mutatr_set_wait_protocol with mutex attribute is not valid ");
    _task_set_error(MQX_OK);

    /* Restore the mutex_attr mutex attribute */
    set_mutex_attribute_valid_field(&mutex_attr, temp);

    /* sets the waiting policy of the NULL mutex attributes should be failed */
    result = _mutatr_set_wait_protocol(NULL, wait_pro);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_EINVAL , "Test #6: Testing 6.05: Tests function _mutatr_set_wait_protocol with attr_ptr is NULL ");
    _task_set_error(MQX_OK);

    /* gets the waiting policy of the mutex attributes with invalid protocol should be failed */
    result = _mutatr_set_wait_protocol(&mutex_attr, INVALID_NUMBER);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_EINVAL , "Test #6: Testing 6.06: Tests function _mutatr_set_wait_protocol with new waiting protocol is not valid ");
    _task_set_error(MQX_OK);

    /* Destroy the mutex atrribute */
    result = _mutatr_destroy(&mutex_attr);
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, result == MQX_OK , "Test #6: Testing 6.07: Destroy the mutex attribute ");
}

 /******  Define Test Suite      *******/
 EU_TEST_SUITE_BEGIN(suite_mutex4)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Testing function _mutatr_init in special cases "),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Testing function _mutatr_destroy in special cases "),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Testing functions _mutatr_get/set_priority_ceiling in special cases "),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - Testing functions _mutatr_get/set_sched_protocol in special cases "),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - Testing functions _mutatr_get/set_spin_limit in special cases "),
    EU_TEST_CASE_ADD(tc_6_main_task, " Test #6 - Testing functions _mutatr_get/set_wait_protocol in special cases "),
 EU_TEST_SUITE_END(suite_mutex4)

/*******  Add test suites */
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_mutex4, "Tests functions for mutex")
 EU_TEST_REGISTRY_END()

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   starts up the mutex and tests it.
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

/* EOF */
