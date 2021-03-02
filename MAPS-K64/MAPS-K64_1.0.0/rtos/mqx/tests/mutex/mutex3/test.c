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
*   This file contains functions for testing mutex locking/unlocking
*   features
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <mutex.h>
#include <util.h>
//#include "test_module.h"
#include "test.h"

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
void tc_1_main_task(void);// Test #1 - Testing Mutex Creation
void tc_2_main_task(void);// Test #2 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_NO_PRIO_INHERIT, MUTEX_SPIN_ONLY
void tc_3_main_task(void);// Test #3 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_PRIO_INHERIT, MUTEX_SPIN_ONLY
void tc_4_main_task(void);// Test #4 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_PRIO_PROTEXT, MUTEX_SPIN_ONLY
void tc_5_main_task(void);// Test #5 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_NO_PRIO_INHERIT, MUTEX_LIMITED_SPIN
void tc_6_main_task(void);// Test #6 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_PRIO_INHERIT, MUTEX_LIMITED_SPIN
void tc_2_mutex_spin_task(uint32_t index);
//------------------------------------------------------------------------
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK,        main_task,        2000,  MAIN_PRIO,       "Main",        MQX_AUTO_START_TASK},
   { MUTEX_SPIN_TASK,  mutex_spin_task,  1000,  MUTEX_SPIN_PRIO, "MutexSpin",   0},
   { DUMMY_TASK,       mutex_spin_task,  1000,  DUMMY_PRIO,      "Dummy",       0},
   { 0,                0,                0,     0,               0,             0}
};

MUTEX_STRUCT mutex1;
_task_id              global_mutex_ids[NUMBER_OF_MUTEX_TASKS];
unsigned char                 global_mutex_seq_array[NUMBER_OF_MUTEX_TASKS];
_mqx_uint             global_mutex_seq_index;

MUTEX_ATTR_STRUCT     mutex_attr;
_mqx_uint            test_index,i;

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST #1: Testing Mutex Creation
//
//END---------------------------------------------------------------------

void tc_1_main_task(void)
{
    _mqx_uint result;
    KERNEL_DATA_STRUCT_PTR     kernel_data;

    /* Intentional kernel data corruption, since mutexes are used by other components/peripherals
       and mutex kernel component already exists */
    kernel_data = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();
    kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES] = NULL;

    test_index = 1;
    result = _mutex_create_component();   
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.1: _mutex_create_component operation")
    if ( result != MQX_OK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST #2: Testing the following functions:
//               _mutex_lock(), _mutex_unlock()
//
//              with Mutex parameters:
//                  SCHED_POLICY: MUTEX_NO_PRIO_INHERIT
//                  WAIT_POLICY:  MUTEX_SPIN_ONLY
//END---------------------------------------------------------------------

void tc_2_main_task(void)
{
    _mqx_uint result;
    
    test_index++;
    result = _mutatr_init(&mutex_attr);   
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.1: _mutatr_init operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    for ( i = 0; i < NUMBER_OF_MUTEX_TASKS; i ++ ) {
        global_mutex_ids[i] = _task_create( 0, MUTEX_SPIN_TASK, i );
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (global_mutex_ids[i] != 0), "Test #2 Testing: 2.1: create mutex_spin_task operation")
        if ( global_mutex_ids[i] == 0 ) {
            _time_delay_ticks(4);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */
    } /* Endfor */

    _time_delay( 10 );

    test_mutex_sched_seq( &mutex_attr, MUTEX_SPIN_ONLY,
        MUTEX_NO_PRIO_INHERIT, test_index );

    test_mutex_count( 3 );

    test_mutex_sequence( 4 );

    result = _mutex_destroy( &mutex1 );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.4: destroy mutex1 operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
}

void tc_2_mutex_spin_task(uint32_t index)
{
    
    _mqx_uint result;
    _mqx_uint sched_pol;
    _mqx_uint mutex_prio;
    _mqx_uint my_prio;
    _mqx_uint wait_pol;
    

    while ( TRUE ) {
        _task_block();

        wait_pol = 0;
        result = _mutatr_get_wait_protocol( &mutex_attr, &wait_pol );
        EU_ASSERT_REF_TC_MSG( tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.1: _mutatr_get_wait_protocol operation")
        if ( result != MQX_EOK ) {
            _time_delay_ticks(4);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */

        result = _mutex_lock( &mutex1 );
        EU_ASSERT_REF_TC_MSG( tc_2_main_task, ((result == MQX_EOK) || (wait_pol == MUTEX_LIMITED_SPIN)), "Test #2 Testing: 2.2: lock mutex1 operation")
        if ( (result != MQX_EOK) && (wait_pol != MUTEX_LIMITED_SPIN) ) {
            work(20);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */

        if ( wait_pol == MUTEX_LIMITED_SPIN ) {
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (result == MQX_EBUSY), "Test #2 Testing: 2.3: _mutex_lock should have returned MQX_EBUSY when wait protocol is MUTEX_LIMITED_SPIN")
            if ( result != MQX_EBUSY ) {
                _time_delay_ticks(4);
                eunit_test_stop();
                _mqx_exit(1);
            } /* Endif */

            continue;
        } /* Endif */

        sched_pol = 0;
        result = _mutatr_get_sched_protocol( &mutex_attr, &sched_pol );
        EU_ASSERT_REF_TC_MSG( tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.5: _mutatr_get_sched_protocol operation")
        if ( result != MQX_EOK ) {
            _time_delay_ticks(4);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */

        if ( sched_pol == MUTEX_PRIO_PROTECT ) {
            result = _mutex_get_priority_ceiling( &mutex1, &mutex_prio );
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.6: _mutex_get_priority_ceiling operation")
            if ( result != MQX_EOK ) {
                _time_delay_ticks(4);
                eunit_test_stop();
                _mqx_exit(1);
            } /* Endif */

            result = test_get_task_current_priority( 0, &my_prio );
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.7: test_get_task_current_priority operation")
            if ( result != MQX_OK ) {
                _time_delay_ticks(4);
                eunit_test_stop();
                _mqx_exit(1);
            } /* Endif */

            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (my_prio == mutex_prio), "Test #2 Testing: 2.8: check mutex task priority operation")
            if ( my_prio != mutex_prio ) {
                _time_delay_ticks(4);
                eunit_test_stop();
                _mqx_exit(1);
            } /* Endif */

        } /* Endif */

        global_mutex_seq_index++;
        global_mutex_seq_array[index] = global_mutex_seq_index;

        result = _mutex_unlock( &mutex1 );
        EU_ASSERT_REF_TC_MSG( tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.9: unlock mutex1 operation")
        if ( result != MQX_EOK ) {
            work(20);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */

    } /* Endwhile */
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST #3: Testing the following functions:
//               _mutex_lock(), _mutex_unlock()
//
//              with Mutex parameters:
//                  SCHED_POLICY: MUTEX_PRIO_INHERIT
//                  WAIT_POLICY:  MUTEX_SPIN_ONLY
//END---------------------------------------------------------------------

void tc_3_main_task(void)
{
    _mqx_uint result;

    test_index++;
    test_mutex_sched_seq( &mutex_attr, MUTEX_SPIN_ONLY,
        MUTEX_PRIO_INHERIT, test_index );

    test_mutex_count( 2 );

    result = _mutex_destroy( &mutex1 );
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, (result == MQX_EOK), "Test #3 Testing: 3.3: destroy mutex1 operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST #4: Testing the following functions:
//               _mutex_lock(), _mutex_unlock()
//
//              with Mutex parameters:
//                  SCHED_POLICY: MUTEX_PRIO_PROTEXT
//                  WAIT_POLICY:  MUTEX_SPIN_ONLY
//END---------------------------------------------------------------------

void tc_4_main_task(void)
{
    _mqx_uint result;

    test_index++;

    test_mutex_sched_seq( &mutex_attr, MUTEX_SPIN_ONLY,
      MUTEX_PRIO_PROTECT, test_index );

    for ( i = 0; i < NUMBER_OF_MUTEX_TASKS; i++ ) {
        test_get_task_current_priority( global_mutex_ids[i], &result );
        EU_ASSERT_REF_TC_MSG( tc_4_main_task, (result == MUTEX_SPIN_PRIO), "Test #4 Testing: task's current priority should be MUTEX_SPIN_PRIO")
        if ( result != MUTEX_SPIN_PRIO ) {
            _time_delay_ticks(4);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */
    } /* Endfor */

    test_mutex_count( 2 );

    result = _mutex_destroy( &mutex1 );
    EU_ASSERT_REF_TC_MSG( tc_4_main_task, (result == MQX_EOK), "Test #4 Testing: 4.3: destroy mutex1 operation")
    if ( result != MQX_EOK ) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
    } /* Endif */
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_5_main_task
// Comments     : TEST #5: Testing the following functions:
//               _mutex_lock(), _mutex_unlock()
//
//              with Mutex parameters:
//                  SCHED_POLICY: MUTEX_NO_PRIO_INHERIT
//                  WAIT_POLICY:  MUTEX_LIMITED_SPIN
//END---------------------------------------------------------------------

void tc_5_main_task(void)
{
    _mqx_uint result;

    test_index++;
    test_mutex_sched_seq( &mutex_attr, MUTEX_LIMITED_SPIN,
        MUTEX_NO_PRIO_INHERIT, test_index );

    test_mutex_count( 3 );

    result = _mutex_destroy( &mutex1 );
    EU_ASSERT_REF_TC_MSG( tc_5_main_task, (result == MQX_EOK), "Test #5 Testing: 5.4: destroy mutex1 operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_6_main_task
// Comments     : TEST #6: Testing the following functions:
//               _mutex_lock(), _mutex_unlock()
//
//              with Mutex parameters:
//                  SCHED_POLICY: MUTEX_PRIO_INHERIT
//                  WAIT_POLICY:  MUTEX_LIMITED_SPIN
//END---------------------------------------------------------------------

void tc_6_main_task(void)
{
    _mqx_uint result;

    test_index++;
    test_mutex_sched_seq( &mutex_attr, MUTEX_LIMITED_SPIN,
        MUTEX_PRIO_INHERIT, test_index );

    test_mutex_count( 3 );

    result = _mutex_destroy( &mutex1 );
    EU_ASSERT_REF_TC_MSG( tc_6_main_task, (result == MQX_EOK), "Test #6 Testing: 6.4: destroy mutex1 operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
}

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : mutex_spin_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void mutex_spin_task
   (
      uint32_t index
   )
{
    tc_2_mutex_spin_task(index);
}

//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_3)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Testing Mutex Creation"),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_NO_PRIO_INHERIT, MUTEX_SPIN_ONLY"),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_PRIO_INHERIT, MUTEX_SPIN_ONLY"),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_PRIO_PROTEXT, MUTEX_SPIN_ONLY"),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_NO_PRIO_INHERIT, MUTEX_LIMITED_SPIN"),
    EU_TEST_CASE_ADD(tc_6_main_task, " Test #6 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_PRIO_INHERIT, MUTEX_LIMITED_SPIN")
EU_TEST_SUITE_END(suite_3)  
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_3, "- Testing mutex locking/unlocking features")
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

    _mem_zero(global_mutex_seq_array, NUMBER_OF_MUTEX_TASKS );
    global_mutex_seq_index = 0;

    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();

    _mqx_exit(0);
}

/* EOF */
