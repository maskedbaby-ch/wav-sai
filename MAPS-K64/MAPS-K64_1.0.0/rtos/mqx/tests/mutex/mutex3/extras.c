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
* $FileName: extras.c$
* $Version : 3.7.5.0$
* $Date    : Mar-24-2011$
*
* Comments:
*
*   This file contains utility functions for the mutex test program
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <mutex.h>
#include <util.h>
#include "test.h"
//#include <test_module.h>

#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   extras


/*FUNCTION*-----------------------------------------------------------------
* 
* Task Name    : test_mutex_sequence
* Comments     :
*
*END*----------------------------------------------------------------------*/

void test_mutex_sequence
   (
      _mqx_uint test_no
   )
{
    _mqx_uint i,j,k;
    _mqx_uint result;

    j = 0;
    while( global_mutex_seq_array[j++] != 1 );

    j--;

    for( i = 0; i < NUMBER_OF_MUTEX_TASKS; i++ ) {
        k = (j + i) % NUMBER_OF_MUTEX_TASKS;
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (global_mutex_seq_array[k] == (i+1)), "Test #2 Testing: 2.4: _mutex_lock/unlock operation")
        if ( global_mutex_seq_array[k] != (i+1) ) {
            _time_delay_ticks(4);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */
    } /* Endfor */
}



/*FUNCTION*-----------------------------------------------------------------
* 
* Task Name    : test_mutex_task_state
* Comments     :
*
*END*----------------------------------------------------------------------*/

void test_mutex_task_state
   (
      _mqx_uint state,
      _mqx_uint test_no1,
      _mqx_uint test_no2
   )
{
    TD_STRUCT_PTR   td_ptr;
    _mqx_uint       i;
    _mqx_uint       result;

    for ( i = 0; i < NUMBER_OF_MUTEX_TASKS; i++ ) {
        td_ptr = _task_get_td(global_mutex_ids[i]);
        
        switch(test_no1) {
        case 2:
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, (td_ptr->STATE == state), "Test #2 Testing: 2.7: _mutex_lock operation")
            break;
        case 3:
            EU_ASSERT_REF_TC_MSG(tc_3_main_task, (td_ptr->STATE == state), "Test #3 Testing: 3.7: _mutex_lock operation")
            break;
        case 4:
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (td_ptr->STATE == state), "Test #4 Testing: 4.7: _mutex_lock operation")
            break;
        case 5:
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (td_ptr->STATE == state), "Test #5 Testing: 5.7: _mutex_lock operation")
            break;
        case 6:
            EU_ASSERT_REF_TC_MSG(tc_6_main_task, (td_ptr->STATE == state), "Test #6 Testing: 6.7: _mutex_lock operation")
            break;
        default:
            break;
        }
        
        if ( td_ptr->STATE != state ) {
            _time_delay_ticks(4);
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */
    } /* Endfor */
}

/*FUNCTION*-----------------------------------------------------------------
* 
* Task Name    : test_mutex_count
* Comments     :
*
*END*----------------------------------------------------------------------*/

void test_mutex_count
   (
      _mqx_uint test_no
   )
{
    _mqx_uint result;

    result = _mutex_get_wait_count( &mutex1 );

    switch(test_no) {
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == 0), "Test #2 Testing: 2.3: mutex wait count should be 0")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == 0), "Test #3 Testing: 3.2: mutex wait count should be 0")
        break;
    case 4:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == 0), "Test #4 Testing: 4.2: mutex wait count should be 0")
        break;
    case 5:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == 0), "Test #5 Testing: 5.3: mutex wait count should be 0")
        break;
    case 6:
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == 0), "Test #6 Testing: 6.3: mutex wait count should be 0")
        break;
    default:
        break;
    }
    
    if ( result != 0 ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
}

/*FUNCTION*-----------------------------------------------------------------
* 
* Task Name    : test_init_mutex
* Comments     :
*
*END*----------------------------------------------------------------------*/

void test_init_mutex
   (
      MUTEX_ATTR_STRUCT_PTR   mut_attr_ptr,
      _mqx_uint               wait_pol,
      _mqx_uint               sched_pol,
      _mqx_uint               test_no
   )
{
    _mqx_uint result;

    result = _mutatr_set_wait_protocol( mut_attr_ptr, wait_pol );
    
    switch(test_no) {
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.1: _mutatr_set_wait_protocol operation")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_EOK), "Test #3 Testing: 3.1: _mutatr_set_wait_protocol operation")
        break;
    case 4:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_EOK), "Test #4 Testing: 4.1: _mutatr_set_wait_protocol operation")
        break;
    case 5:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_EOK), "Test #5 Testing: 5.1: _mutatr_set_wait_protocol operation")
        break;
    case 6:
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_EOK), "Test #6 Testing: 6.1: _mutatr_set_wait_protocol operation")
        break;
    default:
        break;
    }
    
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    result = _mutatr_set_sched_protocol( mut_attr_ptr, sched_pol );
    
    switch(test_no) {
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.2: _mutatr_set_sched_protocol operation")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_EOK), "Test #3 Testing: 3.2: _mutatr_set_sched_protocol operation")
        break;
    case 4:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_EOK), "Test #4 Testing: 4.2: _mutatr_set_sched_protocol operation")
        break;
    case 5:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_EOK), "Test #5 Testing: 5.2: _mutatr_set_sched_protocol operation")
        break;
    case 6:
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_EOK), "Test #6 Testing: 6.2: _mutatr_set_sched_protocol operation")
        break;
    default:
        break;
    }
    
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    result = _mutatr_set_priority_ceiling( mut_attr_ptr, MAIN_PRIO );
    
    switch(test_no) {
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.3: _mutatr_set_priority_ceiling operation")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_EOK), "Test #3 Testing: 3.3: _mutatr_set_priority_ceiling operation")
        break;
    case 4:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_EOK), "Test #4 Testing: 4.3: _mutatr_set_priority_ceiling operation")
        break;
    case 5:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_EOK), "Test #5 Testing: 5.3: _mutatr_set_priority_ceiling operation")
        break;
    case 6:
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_EOK), "Test #6 Testing: 6.3: _mutatr_set_priority_ceiling operation")
        break;
    default:
        break;
    }
    
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    result = _mutatr_set_spin_limit( mut_attr_ptr, TEST_SPIN_LIMIT );
    
    switch(test_no) {
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.4: _mutatr_set_spin_limit operation")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_EOK), "Test #3 Testing: 3.4: _mutatr_set_spin_limit operation")
        break;
    case 4:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_EOK), "Test #4 Testing: 4.4: _mutatr_set_spin_limit operation")
        break;
    case 5:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_EOK), "Test #5 Testing: 5.4: _mutatr_set_spin_limit operation")
        break;
    case 6:
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_EOK), "Test #6 Testing: 6.4: _mutatr_set_spin_limit operation")
        break;
    default:
        break;
    }
    
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    result = _mutex_init(&mutex1, mut_attr_ptr);
    
    switch(test_no) {
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.4: _mutex_init operation")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_EOK), "Test #3 Testing: 3.4: _mutex_init operation")
        break;
    case 4:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_EOK), "Test #4 Testing: 4.4: _mutex_init operation")
        break;
    case 5:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_EOK), "Test #5 Testing: 5.4: _mutex_init operation")
        break;
    case 6:
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_EOK), "Test #6 Testing: 6.4: _mutex_init operation")
        break;
    default:
        break;
    }
    
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
}


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : test_get_task_current_priority
* Returned Value   : none
* Comments         :
*   This function returns the current_priority of the specified task
*
*END*----------------------------------------------------------------------*/

_mqx_uint test_get_task_current_priority
   (
      /* [IN] the task id to use */
      _task_id task_id,

      /* [OUT] the location where the task priority is to be placed */
      _mqx_uint_ptr priority_ptr
   )
{ /* Body */
    TD_STRUCT_PTR           td_ptr;

    td_ptr = _task_get_td(task_id);
    if (td_ptr == NULL) {
        return(MQX_INVALID_TASK_ID);
    } /* Endif */
    
    *priority_ptr = td_ptr->MY_QUEUE->PRIORITY;
    
    return MQX_OK;
} /* Endbody */


/*FUNCTION*-----------------------------------------------------------------
* 
* Task Name    : test_mutex_sched_seq
* Comments     :
*
*END*----------------------------------------------------------------------*/

void test_mutex_sched_seq
   (
      MUTEX_ATTR_STRUCT_PTR   mut_attr_ptr,
      _mqx_uint               wait_pol,
      _mqx_uint               sched_pol,
      _mqx_uint               test_no
   )
{
    _mqx_uint   result;
    _mqx_uint   i;


    test_init_mutex( mut_attr_ptr, wait_pol, sched_pol, test_no);

    result = _mutex_lock( &mutex1 );

    switch(test_no) {
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.5: lock mutex1 operation")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_EOK), "Test #3 Testing: 3.5: lock mutex1 operation")
        break;
    case 4:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_EOK), "Test #4 Testing: 4.5: lock mutex1 operation")
        break;
    case 5:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_EOK), "Test #5 Testing: 5.5: lock mutex1 operation")
        break;
    case 6:
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_EOK), "Test #6 Testing: 6.5: lock mutex1 operation")
        break;
    default:
        break;
    }
    
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    _mem_zero(global_mutex_seq_array, NUMBER_OF_MUTEX_TASKS );
    global_mutex_seq_index = 0;

    for ( i = 0; i < NUMBER_OF_MUTEX_TASKS; i++ ) {
        _task_ready( _task_get_td(global_mutex_ids[i]) );
    } /* Endfor */

    if ( sched_pol == MUTEX_PRIO_INHERIT ) {
        _task_set_priority( 0, DUMMY_PRIO, &i);
        test_get_task_current_priority( 0, &i );
        
        switch(test_no) {
        case 2:
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i == MUTEX_SPIN_PRIO), "Test #2 Testing: 2.6: mutex task's current priority should be MUTEX_SPIN_PRIO")
            break;
        case 3:
            EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i == MUTEX_SPIN_PRIO), "Test #3 Testing: 3.6: mutex task's current priority should be MUTEX_SPIN_PRIO")
            break;
        case 4:
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (i == MUTEX_SPIN_PRIO), "Test #4 Testing: 4.6: mutex task's current priority should be MUTEX_SPIN_PRIO")
            break;
        case 5:
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (i == MUTEX_SPIN_PRIO), "Test #5 Testing: 5.6: mutex task's current priority should be MUTEX_SPIN_PRIO")
            break;
        case 6:
            EU_ASSERT_REF_TC_MSG(tc_6_main_task, (i == MUTEX_SPIN_PRIO), "Test #6 Testing: 6.6: mutex task's current priority should be MUTEX_SPIN_PRIO")
            break;
        default:
            break;
        }
        
        if ( i != MUTEX_SPIN_PRIO ) {
         _time_delay_ticks(4);
         eunit_test_stop();
         _mqx_exit(1);
        } /* Endif */

        for ( i = 0; i < (TEST_SPIN_LIMIT+1); i++ ) {
         _sched_yield();
        } /* Endfor */
    } else {
        _time_delay_ticks(24);
    } /* Endif */

    if ( wait_pol == MUTEX_LIMITED_SPIN ) {
        test_mutex_task_state( BLOCKED, test_no, 7);
    } else {
        test_mutex_task_state( READY, test_no, 7);
    } /* Endif */

    result = _mutex_unlock( &mutex1 );
    
    switch(test_no) {
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_EOK), "Test #2 Testing: 2.8: unlock mutex1 operation")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_EOK), "Test #3 Testing: 3.8: unlock mutex1 operation")
        break;
    case 4:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_EOK), "Test #4 Testing: 4.8: unlock mutex1 operation")
        break;
    case 5:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_EOK), "Test #5 Testing: 5.8: unlock mutex1 operation")
        break;
    case 6:
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_EOK), "Test #6 Testing: 6.8: unlock mutex1 operation")
        break;
    default:
        break;
    }
    
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    _task_set_priority( 0, MAIN_PRIO, &i);

    if ( wait_pol != MUTEX_LIMITED_SPIN ) {
        _time_delay_ticks(35);
    } /* Endif */
}

/* EOF */
