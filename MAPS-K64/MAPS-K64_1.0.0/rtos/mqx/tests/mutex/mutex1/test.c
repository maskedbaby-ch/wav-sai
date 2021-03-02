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
* $Version : 3.8.6.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file contains functions for testing mutex primitives as well
*   as simple mutex locking/unlocking features
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <mutex.h>
#include <util.h>
#include "test.h"
//#include "test_module.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"


//------------------------------------------------------------------------
// required user_config settings
//------------------------------------------------------------------------
#if !MQX_MUTEX_HAS_POLLING
    #error "Required MQX_MUTEX_HAS_POLLING"
#endif


#define FILE_BASENAME test
//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);  // Test #1 - Testing _mutex_create_component() function ...
void tc_2_main_task(void);  // Test #2 - Testing _mutex_init() function...
void tc_3_main_task(void);  // Test #3 - Testing _mutatr_init() function...
void tc_4_main_task(void);  // Test #4 - Testing _mutatr_set_sched_protocol(), _mutatr_get_sched_protocol() functions...
void tc_5_main_task(void);  // Test #5 - Testing _mutatr_set_wait_protocol(), _mutatr_get_wait_protocol() functions...
void tc_6_main_task(void);  // Test #6 - Testing _mutatr_set_priority_ceiling(), _mutatr_get_priority_ceiling() functions...
void tc_7_main_task(void);  // Test #7 - Testing _mutatr_set_spin_limit(), _mutatr_get_spin_limit() functions...
void tc_8_main_task(void);  // Test #8 - Testing _mutatr_destroy() function...
void tc_9_main_task(void);  // Test #9 - Testing  _mutex_lock(), _mutex_unlock(), _mutex_get_wait_count() functions...
void tc_10_main_task(void); // Test #10 - Testing _mutex_lock(), _mutex_unlock() functions...
void tc_11_main_task(void); // Test #11 - Testing  _mutex_lock(), _mutex_unlock() functions...
//------------------------------------------------------------------------                           
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK,         main_task,         2000,  MAIN_PRIO, "Main",        MQX_AUTO_START_TASK},
   { SIMPLE_MUTEX_TASK, simple_mutex_task, 1000,  MAIN_PRIO, "MutexSimple", 0},
   { 0,                 0,                 0,     0,         0,             0}
};

MUTEX_STRUCT          mutex1;
MUTEX_ATTR_STRUCT     mutex_attr;
bool               Test_Limited_Spin = FALSE;

_mqx_uint           test_index=0;

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST #1: Testing the following function:
//              _mutex_create_component()
//END---------------------------------------------------------------------

void tc_1_main_task(void)
{
    _mqx_uint         result, i;
    KERNEL_DATA_STRUCT_PTR     kernel_data;

    test_index++; //1

    /* Intentional kernel data corruption, since mutexes are used by other components/peripherals
       and mutex kernel component already exists */
    kernel_data = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();
    kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES] = NULL;

    for ( i = 0; i < 3; i++ ) {
        result = _mutex_create_component();
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.1: _mutex_create_component operation")
        if ( result != MQX_OK ) {
            _time_delay_ticks(4);
           eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */
    } /* Endfor */

}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST #2: Testing the following function:
//              _mutex_init()
//END---------------------------------------------------------------------

void tc_2_main_task(void)
{
    _mqx_uint         result;
    MUTEX_STRUCT_PTR  mutex_in_error_ptr;

    test_index++;

    result = _mutex_init(&mutex1, NULL);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.1: _mutex_init operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    result = _mutex_test( (void **)(&mutex_in_error_ptr) );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.2: _mutex_test operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    result = _mutex_destroy( &mutex1 );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.3: _mutex_destroy operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
       eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST #3: Testing the following function:
//              _mutatr_init()
//END---------------------------------------------------------------------

void tc_3_main_task(void)
{
    _mqx_uint         result, i;
    test_index++;

    result = _mutatr_init(&mutex_attr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_EOK), "Test #3 Testing: 3.1: _mutatr_init operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    i = 0;
    result = _mutatr_get_sched_protocol( &mutex_attr, &i );
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_EOK), "Test #3 Testing: 3.2: _mutatr_get_sched_protocol operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == MUTEX_NO_PRIO_INHERIT), "Test #3 Testing: 3.3: current scheduling protocol should be MUTEX_NO_PRIO_INHERIT")
    if ( i != MUTEX_NO_PRIO_INHERIT ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    i = 0;
    result = _mutatr_get_wait_protocol( &mutex_attr, &i );
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_EOK), "Test #3 Testing: 3.4: _mutatr_get_wait_protocol operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == MUTEX_QUEUEING), "Test #3 Testing: 3.5: current waiting protocol should be MUTEX_QUEUEING")
    if ( i != MUTEX_QUEUEING ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST #4: Testing the following functions:
//              _mutatr_set_sched_protocol(), _mutatr_get_sched_protocol()
//END---------------------------------------------------------------------

void tc_4_main_task(void)
{
    test_index++;


    test_mutatr_setget_sched_protocol( &mutex_attr, MUTEX_PRIO_INHERIT,    1 );
    test_mutatr_setget_sched_protocol( &mutex_attr, MUTEX_NO_PRIO_INHERIT, 2 );
    test_mutatr_setget_sched_protocol( &mutex_attr, MUTEX_PRIO_PROTECT,    3 );


}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_5_main_task
// Comments     : TEST #5: Testing the following functions:
//              _mutatr_set_wait_protocol(), _mutatr_get_wait_protocol()
//END---------------------------------------------------------------------

void tc_5_main_task(void)
{
    test_index++;


    test_mutatr_setget_wait_protocol( &mutex_attr, MUTEX_SPIN_ONLY,         1 );
    test_mutatr_setget_wait_protocol( &mutex_attr, MUTEX_QUEUEING,          2 );
    test_mutatr_setget_wait_protocol( &mutex_attr, MUTEX_PRIORITY_QUEUEING, 3 );
    test_mutatr_setget_wait_protocol( &mutex_attr, MUTEX_LIMITED_SPIN,      4 );
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_6_main_task
// Comments     : TEST #6: Testing the following functions:
//              _mutatr_set_priority_ceiling(), _mutatr_get_priority_ceiling()
//END---------------------------------------------------------------------

void tc_6_main_task(void)
{
    test_index++;

    test_mutatr_setget_priority_ceiling( &mutex_attr, 7, 1 );


}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_7_main_task
// Comments     : TEST #7: Testing the following functions:
//               _mutatr_set_spin_limit(), _mutatr_get_spin_limit()
//END---------------------------------------------------------------------

void tc_7_main_task(void)
{
    test_index++;

    test_mutatr_setget_spin_limit( &mutex_attr, TEST_SPIN_LIMIT, 1 );

}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_8_main_task
// Comments     : TEST #8: Testing the following function:
//              _mutatr_destroy()
//END---------------------------------------------------------------------

void tc_8_main_task(void)
{
   _mqx_uint         result;
   test_index++;


   result = _mutatr_destroy( &mutex_attr );
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_EOK), "Test #8 Testing: 8.1: _mutatr_destroy operation")
   if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
   } /* Endif */
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_9_main_task
// Comments     : TEST #9: Testing the following functions:
//              _mutex_lock(), _mutex_unlock(), _mutex_get_wait_count()
//END---------------------------------------------------------------------

void tc_9_main_task(void)
{
   _mqx_uint         result;
   _task_id          tid;
   
   test_index++;

   result = _mutex_init(&mutex1, NULL);
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_EOK), "Test #9 Testing: 9.1: _mutex_init operation")
   if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
   } /* Endif */

   result = _mutex_lock( &mutex1 );
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_EOK), "Test #9 Testing: 9.2: lock mutex1 operation")
   if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
   } /* Endif */

   result = _mutex_get_wait_count( &mutex1 );
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == 0), "Test #9 Testing: 9.3: _mutex_get_wait_count should have returned 0")
   if ( result != 0 ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
   } /* Endif */

   tid = _task_create( 0, SIMPLE_MUTEX_TASK, 0 );

   _sched_yield();

   result = _mutex_get_wait_count( &mutex1 );
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == 1), "Test #9 Testing: 9.4: _mutex_get_wait_count should have returned 1")
   if ( result != 1 ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
   } /* Endif */

   result = _mutex_unlock( &mutex1 );
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_EOK), "Test #9 Testing: 9.5: unlock mutex1 operation")
   if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
   } /* Endif */

   result = _mutex_get_wait_count( &mutex1 );
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == 0), "Test #9 Testing: 9.6: _mutex_get_wait_count should have returned 0")
   if ( result != 0 ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
   } /* Endif */
   
   result = _mutex_destroy( &mutex1 );
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_EOK), "Test #9 Testing: 9.7: destroy mutex1 operation")
   if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
   } /* Endif */
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_10_main_task
// Comments     : TEST #10: Testing the following functions:
//              _mutex_lock(), _mutex_unlock()
//              with MUTEX_SPIN_ONLY parameter
//END---------------------------------------------------------------------

void tc_10_main_task(void)
{
   _task_id          tid;
   _mqx_uint         result, i;   
   TD_STRUCT_PTR     td_ptr;
   
   test_index++;

   result = _mutatr_init(&mutex_attr);
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_EOK), "Test #10 Testing: 10.1: _mutatr_init operation")
   if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
   } /* Endif */

   test_init_mutex( &mutex_attr, MUTEX_SPIN_ONLY, MUTEX_NO_PRIO_INHERIT, test_index);

   result = _mutex_lock( &mutex1 );
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_EOK), "Test #10 Testing: 10.2: lock mutex1 operation")
   if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
   } /* Endif */


   tid = _task_create( 0, SIMPLE_MUTEX_TASK, 0 );

   for( i = 0; i < (TEST_SPIN_LIMIT+1); i++ ) {
        _sched_yield();
   } /* Endfor */

   td_ptr = _task_get_td( tid );
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, (td_ptr != NULL), "Test #10 Testing: 10.3: _task_get_td operation")
   if ( td_ptr == NULL ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
   } /* Endif */

   result = _mutex_unlock( &mutex1 );
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_EOK), "Test #10 Testing: 10.4: unlock mutex1 operation")
   if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
   } /* Endif */

   _sched_yield();

   result = _mutex_destroy( &mutex1 );
   EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_EOK), "Test #10 Testing: 10.5: destroy mutex1 operation")
   if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
   } /* Endif */
}

 
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_11_main_task
// Comments     : TEST #11: Testing the following functions:
//              _mutex_lock(), _mutex_unlock()
//              with MUTEX_LIMITED_SPIN parameter
//END---------------------------------------------------------------------

void tc_11_main_task(void)
{
    _mqx_uint       result, i;
    _task_id        tid;   
    TD_STRUCT_PTR   td_ptr;
    
    test_index++;

    result = _mutatr_init(&mutex_attr);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_EOK), "Test #11 Testing: 11.1: _mutatr_init operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    test_init_mutex( &mutex_attr, MUTEX_LIMITED_SPIN, MUTEX_NO_PRIO_INHERIT, test_index);

    result = _mutex_lock( &mutex1 );
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_EOK), "Test #11 Testing: 11.2: lock mutex1 operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    Test_Limited_Spin = TRUE;

    tid = _task_create( 0, SIMPLE_MUTEX_TASK, 0 );

    for( i = 0; i < (TEST_SPIN_LIMIT+1); i++ ) {
        _sched_yield();
    } /* Endfor */

    td_ptr = _task_get_td( tid );
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (td_ptr == NULL), "Test #11 Testing: 11.3: _task_get_td operation")
    if ( td_ptr != NULL ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    result = _mutex_unlock( &mutex1 );
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_EOK), "Test #11 Testing: 11.4: unlock mutex1 operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
}

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : simple_mutex_task
* Comments     :
*
*END*----------------------------------------------------------------------*/
void simple_mutex_task
   (
      uint32_t param
   )
{
    _mqx_uint result;

    result = _mutex_lock( &mutex1 );

    if ( Test_Limited_Spin ) {
        EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_EBUSY), "Test #11 Testing: 11.2: _mutex_lock with mutex 1 should have returned MQX_EBUSY")
        if ( result != MQX_EBUSY ) {
            work(20);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */
    } else {
        if (test_index == 9)
            EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_EOK), "Test #9 Testing: 9.3: lock mutex1 operation")
        else if (test_index == 10)
            EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_EOK), "Test #10 Testing: 10.3: lock mutex1 operation")
        if ( result != MQX_EOK ) {
            work(20);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */

        result = _mutex_unlock( &mutex1 );
        if (test_index == 9)
            EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_EOK), "Test #9 Testing: 9.4: unlock mutex1 operation")
        else if (test_index == 10)
            EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_EOK), "Test #10 Testing: 10.4: unlock mutex1 operation")
        if ( result != MQX_EOK ) {
            work(20);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */
    } /* Endif */
}

//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Testing _mutex_create_component() function ..."),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Testing _mutex_init() function..."),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Testing _mutatr_init() function..."),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - Testing _mutatr_set_sched_protocol(), _mutatr_get_sched_protocol() functions..."),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - Testing _mutatr_set_wait_protocol(), _mutatr_get_wait_protocol() functions..."),
    EU_TEST_CASE_ADD(tc_6_main_task, " Test #6 - Testing _mutatr_set_priority_ceiling(), _mutatr_get_priority_ceiling() functions..."),
    EU_TEST_CASE_ADD(tc_7_main_task, " Test #7 - Testing _mutatr_set_spin_limit(), _mutatr_get_spin_limit() functions..."),
    EU_TEST_CASE_ADD(tc_8_main_task, " Test #8 - Testing _mutatr_destroy() function..."),
    EU_TEST_CASE_ADD(tc_9_main_task, " Test #9 - Testing  _mutex_lock(), _mutex_unlock(), _mutex_get_wait_count() functions..."),
    EU_TEST_CASE_ADD(tc_10_main_task, " Test #10 - Testing _mutex_lock(), _mutex_unlock() functions..."),
    EU_TEST_CASE_ADD(tc_11_main_task, " Test #11 - Testing  _mutex_lock(), _mutex_unlock() functions...")
 EU_TEST_SUITE_END(suite_1) 
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, " - Testing mutex locking/unlocking features")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : Main_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t dummy
   )
{
    _int_install_unexpected_isr();

    test_index = 0;
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
    _mqx_exit(0);
}

/* EOF */
