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
* $Version : 3.8.5.1$
* $Date    : Jul-23-2012$
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
//#include <test_module.h>
#include "test.h"

#include "EUnit.h"
#include "eunit_crit_secs.h"


//------------------------------------------------------------------------
// required user_config settings
//------------------------------------------------------------------------
#if !MQX_MUTEX_HAS_POLLING
    #error "Required MQX_MUTEX_HAS_POLLING"
#endif


#define DEBUGM(x) x
#define FILE_BASENAME test
//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);// Test #1 - Testing Mutex Creation
void tc_2_main_task(void);// Test #2 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_NO_PRIO_INHERIT, MUTEX_QUEUEING
void tc_3_main_task(void);// Test #3 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_PRIO_INHERIT, MUTEX_QUEUEING
void tc_4_main_task(void);// Test #4 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_PRIO_PROTECT, MUTEX_QUEUEING
void tc_5_main_task(void);// Test #5 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_NO_PRIO_INHERIT, MUTEX_PRIORITY_QUEUEING
void tc_6_main_task(void);// Test #6 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_PRIO_INHERIT, MUTEX_PRIORITY_QUEUEING
void tc_7_main_task(void);// Test #7 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_PRIO_PROTECT, MUTEX_PRIORITY_QUEUEING
//------------------------------------------------------------------------
void tc_2_mutex_task(uint32_t index);

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
#if PSP_MQX_CPU_IS_KINETIS_L
   { MAIN_TASK,  main_task,  1000,  MAIN_PRIO,   "Main",   MQX_AUTO_START_TASK},
   { MUTEX_TASK, mutex_task, 500,  MUTEX_PRIO,  "Mutex",  0},
#else
   { MAIN_TASK,  main_task,  3000,  MAIN_PRIO,   "Main",   MQX_AUTO_START_TASK},
   { MUTEX_TASK, mutex_task, 1500,  MUTEX_PRIO,  "Mutex",  0},
#endif
   { 0,          0,          0,     0,           0,        0}
};

MUTEX_STRUCT          mutex1;
_task_id              global_mutex_ids[NUMBER_OF_MUTEX_TASKS];
volatile unsigned char        global_mutex_seq_array[NUMBER_OF_MUTEX_TASKS];
volatile _mqx_uint    global_mutex_seq_index;
MUTEX_ATTR_STRUCT     mutex_attr;

_mqx_uint            test_index;


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

    test_index = 1;
    
    /* Intentional kernel data corruption, since mutexes are used by other components/peripherals
       and mutex kernel component already exists */
    kernel_data = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();
    kernel_data->KERNEL_COMPONENTS[KERNEL_MUTEXES] = NULL;

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
//              _mutex_lock(), _mutex_unlock()
//              with Mutex parameters:
//                  SCHED_POLICY: MUTEX_NO_PRIO_INHERIT
//                  WAIT_POLICY:  MUTEX_QUEUEING
//END---------------------------------------------------------------------

void tc_2_main_task(void)
{
    _mqx_uint result, i;
    unsigned char c;
    
    test_index++;

    result = _mutatr_init(&mutex_attr);   
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.1: _mutatr_init operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    for ( i = 0; i < NUMBER_OF_MUTEX_TASKS; i ++ ) {
        global_mutex_ids[i] = _task_create( 0, MUTEX_TASK, (uint32_t)i );
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (global_mutex_ids[i] != 0), "Test #2 Testing: 2.2: create mutex_task operation")
        if ( global_mutex_ids[i] == 0 ) {
            _time_delay_ticks(4);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */
    } /* Endfor */

    _time_delay_ticks(10);

    test_mutex_sched_seq( &mutex_attr, MUTEX_QUEUEING,
      MUTEX_NO_PRIO_INHERIT, test_index );

    c = 'A';
    for ( i = 0; i < NUMBER_OF_MUTEX_TASKS; i ++ ) {      
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (global_mutex_seq_array[i] == c), "Test #2 Testing: 2.3: _mutex_lock/unlock operation")
        if ( global_mutex_seq_array[i] != c ) {
            _time_delay_ticks(4);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */
        c++;
    } /* Endfor */

    test_check_mutex_task_priorities();

    test_mutex_count( 4 );

    result = _mutex_destroy( &mutex1 ); 
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.5: destroy mutex1 operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
}

void tc_2_mutex_task(uint32_t index)
{
    _mqx_uint result;
    _mqx_uint sched_pol;
    _mqx_uint mutex_prio;
    _mqx_uint my_prio;

    _task_set_priority( 0, MUTEX_PRIO - index - 1, &result);

    while ( TRUE ) {
        _task_block();

        result = _mutex_lock( &mutex1 ); 
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.1: lock mutex1 operation")
        if ( result != MQX_EOK ) {
            work(20);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */

        global_mutex_seq_array[global_mutex_seq_index] = 'A' + index;
        global_mutex_seq_index++;

        sched_pol = 0;
        result = _mutatr_get_sched_protocol( &mutex_attr, &sched_pol ); 
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.2: _mutatr_get_sched_protocol operation")
        if ( result != MQX_EOK ) {
            _time_delay_ticks(4);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */

        if ( sched_pol == MUTEX_PRIO_PROTECT ) {
            result = _mutex_get_priority_ceiling( &mutex1, &mutex_prio ); 
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.3: _mutex_get_priority_ceiling operation")
            if ( result != MQX_EOK ) {
                _time_delay_ticks(4);
                eunit_test_stop();
                _mqx_exit(1);
            } /* Endif */

            result = test_get_task_current_priority( 0, &my_prio ); 
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.4: test_get_task_current_priority operation")
            if ( result != MQX_OK ) {
                _time_delay_ticks(4);
                eunit_test_stop();
                _mqx_exit(1);
            } /* Endif */ 

            EU_ASSERT_REF_TC_MSG(tc_2_main_task, (my_prio == mutex_prio), "Test #2 Testing: 2.5: task priority operation")
            if ( my_prio != mutex_prio ) {
                _time_delay_ticks(4);
                eunit_test_stop();
                _mqx_exit(1);
            } /* Endif */

        } /* Endif */


        result = _mutex_unlock( &mutex1 );
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.6: unlock mutex1 operation")
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
//              _mutex_lock(), _mutex_unlock()
//
//              with Mutex parameters:
//                  SCHED_POLICY: MUTEX_PRIO_INHERIT
//                  WAIT_POLICY:  MUTEX_QUEUEING
//END---------------------------------------------------------------------

void tc_3_main_task(void)
{
    _mqx_uint result;

    test_index++;

    test_mutex_sched_seq( &mutex_attr, MUTEX_QUEUEING,
      MUTEX_PRIO_INHERIT, test_index );

    test_check_mutex_task_priorities();

    test_mutex_count( 2 );

    result = _mutex_destroy( &mutex1 );
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_EOK), "Test #3 Testing: 3.3: destroy mutex1 operation")
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
//              _mutex_lock(), _mutex_unlock()
//
//              with Mutex parameters:
//                  SCHED_POLICY: MUTEX_PRIO_PROTECT
//                  WAIT_POLICY:  MUTEX_QUEUEING
//END---------------------------------------------------------------------

void tc_4_main_task(void)
{
    _mqx_uint result, i;
    unsigned char c;

    test_index++;

    test_mutex_sched_seq( &mutex_attr, MUTEX_QUEUEING,
      MUTEX_PRIO_PROTECT, test_index );

    c = 'A';
    for ( i = 0; i < NUMBER_OF_MUTEX_TASKS; i ++ ) {
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (global_mutex_seq_array[i] == c), "Test #4 Testing: 4.1: _mutex_lock/unlock operation")
        if ( global_mutex_seq_array[i] != c ) {
            _time_delay_ticks(4);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */
        c++;
    } /* Endfor */

    test_check_mutex_task_priorities();

    test_mutex_count( 2 );

    result = _mutex_destroy( &mutex1 );
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_EOK), "Test #4 Testing: 4.3: destroy mutex1 operation")
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
//              _mutex_lock(), _mutex_unlock()
//
//              with Mutex parameters:
//                  SCHED_POLICY: MUTEX_NO_PRIO_INHERIT
//                  WAIT_POLICY:  MUTEX_PRIORITY_QUEUEING
//END---------------------------------------------------------------------

void tc_5_main_task(void)
{
    _mqx_uint result, i;
    unsigned char c;

    test_index++;


    test_mutex_sched_seq( &mutex_attr, MUTEX_PRIORITY_QUEUEING,
    MUTEX_NO_PRIO_INHERIT, test_index );

    c = 'A' + NUMBER_OF_MUTEX_TASKS - 1;
    for ( i = 0; i < NUMBER_OF_MUTEX_TASKS; i ++ ) {
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (global_mutex_seq_array[i] == c), "Test #5 Testing: 5.1: _mutex_lock/unlock operation")
        if ( global_mutex_seq_array[i] != c ) {
            _time_delay_ticks(4);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */
        c--;
    } /* Endfor */

    test_check_mutex_task_priorities();

    test_mutex_count( 2 );

    result = _mutex_destroy( &mutex1 );
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_EOK), "Test #5 Testing: 5.3: destroy mutex1 operation")
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
//              _mutex_lock(), _mutex_unlock()
//
//              with Mutex parameters:
//                  SCHED_POLICY: MUTEX_PRIO_INHERIT
//                  WAIT_POLICY:  MUTEX_PRIORITY_QUEUEING
//END---------------------------------------------------------------------

void tc_6_main_task(void)
{
    _mqx_uint result, i;
    unsigned char c;

    test_index++;

    test_mutex_sched_seq( &mutex_attr, MUTEX_PRIORITY_QUEUEING,
      MUTEX_PRIO_INHERIT, test_index );

    c = 'A' + NUMBER_OF_MUTEX_TASKS - 1;
    for ( i = 0; i < NUMBER_OF_MUTEX_TASKS; i ++ ) {
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (global_mutex_seq_array[i] == c), "Test #6 Testing: 6.1: _mutex_lock/unlock operation")
        if ( global_mutex_seq_array[i] != c ) {
            _time_delay_ticks(4);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */
        c--;
    } /* Endfor */

    test_check_mutex_task_priorities();

    test_mutex_count( 2 );

    result = _mutex_destroy( &mutex1 );
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_EOK), "Test #6 Testing: 6.3: destroy mutex1 operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_7_main_task
// Comments     : TEST #7: Testing the following functions:
//              _mutex_lock(), _mutex_unlock()
//
//              with Mutex parameters:
//                  SCHED_POLICY: MUTEX_PRIO_PROTECT
//                  WAIT_POLICY:  MUTEX_PRIORITY_QUEUEING
//END---------------------------------------------------------------------

void tc_7_main_task(void)
{
    _mqx_uint result, i;
    unsigned char c;

    test_index++;

    test_mutex_sched_seq( &mutex_attr, MUTEX_PRIORITY_QUEUEING,
      MUTEX_PRIO_PROTECT, test_index );

    c = 'A' + NUMBER_OF_MUTEX_TASKS - 1;
    for ( i = 0; i < NUMBER_OF_MUTEX_TASKS; i ++ ) {
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (global_mutex_seq_array[i] == c), "Test #7 Testing: 7.1: _mutex_lock/unlock operation")
        if ( global_mutex_seq_array[i] != c ) {
            _time_delay_ticks(4);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */
        c--;
    } /* Endfor */

    test_check_mutex_task_priorities();

    test_mutex_count( 2 );

    result = _mutex_destroy( &mutex1 );
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_EOK), "Test #7 Testing: 7.3: destroy mutex1 operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    for ( i = 0; i < NUMBER_OF_MUTEX_TASKS; i++ ) {
        result = _task_destroy( global_mutex_ids[i] );
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_EOK), "Test #7 Testing: 7.4: destroy mutex tasks operation")
        if ( result != MQX_OK ) {
            _time_delay_ticks(4);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */
    } /* Endfor */
}

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : mutex_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void mutex_task
   (
      uint32_t index
   )
{
    tc_2_mutex_task(index);
}

//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_2)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Testing Mutex Creation"),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_NO_PRIO_INHERIT, MUTEX_QUEUEING"),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_PRIO_INHERIT, MUTEX_QUEUEING"),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_PRIO_PROTECT, MUTEX_QUEUEING"),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_NO_PRIO_INHERIT, MUTEX_PRIORITY_QUEUEING"),
    EU_TEST_CASE_ADD(tc_6_main_task, " Test #6 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_PRIO_INHERIT, MUTEX_PRIORITY_QUEUEING"),
    EU_TEST_CASE_ADD(tc_7_main_task, " Test #7 - Testing the following functions: _mutex_lock(), _mutex_unlock() with mutex paramters: MUTEX_PRIO_PROTECT, MUTEX_PRIORITY_QUEUEING")
EU_TEST_SUITE_END(suite_2)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_2, " - Testing mutex locking/unlocking features")
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

    _mem_zero((void *)global_mutex_seq_array, sizeof(global_mutex_seq_array));
    global_mutex_seq_index = 0;
    test_index = 0;
    //test_initialize();
    //EU_RUN_ALL_TESTS(test_result_array);
    //_time_delay_ticks(10);  /* waiting for all loggings to be printed out */
    //test_stop();
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
    _mqx_exit(0);
}

/* EOF */
