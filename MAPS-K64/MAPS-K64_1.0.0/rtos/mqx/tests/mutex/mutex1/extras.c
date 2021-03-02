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
* $Version : 3.7.3.0$
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


#define FILE_BASENAME extras


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
    if (test_no == 10)
        EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.1: _mutatr_set_wait_protocol operation")
    else if (test_no == 11)
        EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.1: _mutatr_set_wait_protocol operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    result = _mutatr_set_sched_protocol( mut_attr_ptr, sched_pol );
    if (test_no == 10)
        EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.2: _mutatr_set_sched_protocol operation")
    else if (test_no == 11)
        EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.2: _mutatr_set_sched_protocol operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    result = _mutatr_set_priority_ceiling( mut_attr_ptr, MAIN_PRIO );
    if (test_no == 10)
        EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.3: _mutatr_set_priority_ceiling operation")
    else if (test_no == 11)
        EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.3: _mutatr_set_priority_ceiling operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    result = _mutatr_set_spin_limit( mut_attr_ptr, TEST_SPIN_LIMIT );
    if (test_no == 10)
        EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.4: _mutatr_set_spin_limit operation")
    else if (test_no == 11)
        EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.4: _mutatr_set_spin_limit operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    result = _mutex_init(&mutex1, mut_attr_ptr);
    if (test_no == 10)
        EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.4: _mutex_init operation")
    else if (test_no == 11)
        EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.4: _mutex_init operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
}


/*FUNCTION*-----------------------------------------------------------------
* 
* Task Name    : test_mutatr_setget_sched_protocol
* Comments     :
*
*END*----------------------------------------------------------------------*/

void test_mutatr_setget_sched_protocol
   (
      MUTEX_ATTR_STRUCT_PTR   mut_attr_ptr,
      _mqx_uint               protocol,
      _mqx_uint               test_no
   )
{
    _mqx_uint result,i;

    result = _mutatr_set_sched_protocol( mut_attr_ptr, protocol );
    switch(test_no) {
    case 1:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: 4.1.1: _mutatr_set_sched_protocol with the MUTEX_PRIO_INHERIT protocol operation")
        break;
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: 4.2.1: _mutatr_set_sched_protocol with the MUTEX_NO_PRIO_INHERIT protocol operation")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: 4.3.1: _mutatr_set_sched_protocol with the MUTEX_PRIO_PROTECT operation")
    default:
        break;
    }
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    switch(test_no) {
    case 1:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (mut_attr_ptr->SCHED_PROTOCOL == protocol), "Test #4 Testing: 4.1.2: _mutatr_set_sched_protocol with the MUTEX_PRIO_INHERIT protocol operation")
        break;
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (mut_attr_ptr->SCHED_PROTOCOL == protocol), "Test #4 Testing: 4.2.2: _mutatr_set_sched_protocol with the MUTEX_NO_PRIO_INHERIT protocol operation")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (mut_attr_ptr->SCHED_PROTOCOL == protocol), "Test #4 Testing: 4.3.2: _mutatr_set_sched_protocol with the MUTEX_PRIO_PROTECT operation")
    default:
        break;
    }
    if ( mut_attr_ptr->SCHED_PROTOCOL != protocol ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    i      = 0;
    result = _mutatr_get_sched_protocol( mut_attr_ptr, &i );
    switch(test_no) {
    case 1:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_EOK), "Test #4 Testing: 4.1.3: _mutatr_get_sched_protocol operation")
        break;
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_EOK), "Test #4 Testing: 4.2.3: _mutatr_get_sched_protocol operation")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_EOK), "Test #4 Testing: 4.3.3: _mutatr_get_sched_protocol operation")
    default:
        break;
    }
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    switch(test_no) {
    case 1:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (i == protocol), "Test #4 Testing: 4.1.4: _mutatr_get_sched_protocol should have returned MUTEX_PRIO_INHERIT protocol")
        break;
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (i == protocol), "Test #4 Testing: 4.2.4: _mutatr_get_sched_protocol should have returned MUTEX_NO_PRIO_INHERIT protocol")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (i == protocol), "Test #4 Testing: 4.3.4: _mutatr_get_sched_protocol should have returned MUTEX_PRIO_PROTECT protocol")
    default:
        break;
    }
    if ( i != protocol ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
}


/*FUNCTION*-----------------------------------------------------------------
* 
* Task Name    : test_mutatr_setget_wait_protocol
* Comments     :
*
*END*----------------------------------------------------------------------*/

void test_mutatr_setget_wait_protocol
   (
      MUTEX_ATTR_STRUCT_PTR   mut_attr_ptr,
      _mqx_uint               protocol,
      _mqx_uint               test_no
   )
{
    _mqx_uint result,i;

    result = _mutatr_set_wait_protocol( mut_attr_ptr, protocol );
    switch(test_no) {
    case 1:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_EOK), "Test #5 Testing: 5.1.1: _mutatr_set_wait_protocol with the MUTEX_SPIN_ONLY protocol operation")
        break;
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_EOK), "Test #5 Testing: 5.2.1: _mutatr_set_wait_protocol with the MUTEX_QUEUEING protocol operation")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_EOK), "Test #5 Testing: 5.3.1: _mutatr_set_wait_protocol with the MUTEX_PRIORITY_QUEUEING protocol operation")
        break;
    case 4:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_EOK), "Test #5 Testing: 5.4.1: _mutatr_set_wait_protocol with the MUTEX_LIMITED_SPIN protocol operation")
    default:
        break;
    }
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    switch(test_no) {
    case 1:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (mut_attr_ptr->WAIT_PROTOCOL == protocol), "Test #5 Testing: 5.1.2: _mutatr_set_wait_protocol with the MUTEX_SPIN_ONLY protocol operation")
        break;
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (mut_attr_ptr->WAIT_PROTOCOL == protocol), "Test #5 Testing: 5.2.2: _mutatr_set_wait_protocol with the MUTEX_QUEUEING protocol operation")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (mut_attr_ptr->WAIT_PROTOCOL == protocol), "Test #5 Testing: 5.3.2: _mutatr_set_wait_protocol with the MUTEX_PRIORITY_QUEUEING protocol operation")
        break;
    case 4:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (mut_attr_ptr->WAIT_PROTOCOL == protocol), "Test #5 Testing: 5.4.2: _mutatr_set_wait_protocol with the MUTEX_LIMITED_SPIN protocol operation")
    default:
        break;
    }
    if ( mut_attr_ptr->WAIT_PROTOCOL != protocol ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    i      = 0;
    result = _mutatr_get_wait_protocol( mut_attr_ptr, &i );
    switch(test_no) {
    case 1:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_EOK), "Test #5 Testing: 5.1.3: _mutatr_get_wait_protocol operation")
        break;
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_EOK), "Test #5 Testing: 5.2.3: _mutatr_get_wait_protocol operation")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_EOK), "Test #5 Testing: 5.3.3: _mutatr_get_wait_protocol operation")
        break;
    case 4:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_EOK), "Test #5 Testing: 5.4.3: _mutatr_get_wait_protocol operation")
    default:
        break;
    }
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    switch(test_no) {
    case 1:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (i == protocol), "Test #5 Testing: 5.1.3: _mutatr_get_wait_protocol should have returned MUTEX_SPIN_ONLY protocol")
        break;
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (i == protocol), "Test #5 Testing: 5.2.3: _mutatr_get_wait_protocol should have returned MUTEX_QUEUEING protocol")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (i == protocol), "Test #5 Testing: 5.3.3: _mutatr_get_wait_protocol should have returned MUTEX_PRIORITY_QUEUEING protocol")
        break;
    case 4:
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (i == protocol), "Test #5 Testing: 5.4.3: _mutatr_get_wait_protocol should have returned MUTEX_LIMITED_SPIN protocol")
    default:
        break;
    }
    if ( i != protocol ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
}

/*FUNCTION*-----------------------------------------------------------------
* 
* Task Name    : test_mutatr_setget_priority_ceiling
* Comments     :
*
*END*----------------------------------------------------------------------*/

void test_mutatr_setget_priority_ceiling
   (
      MUTEX_ATTR_STRUCT_PTR   mut_attr_ptr,
      _mqx_uint               priority,
      _mqx_uint               test_no
   )
{
    _mqx_uint result,i;

    result = _mutatr_set_priority_ceiling( mut_attr_ptr, priority );
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_EOK), "Test #6 Testing: 6.1.1: _mutatr_set_priority_ceiling operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (mut_attr_ptr->PRIORITY_CEILING == priority), "Test #6 Testing: 6.1.2: _mutatr_set_priority_ceiling operation")
    if ( mut_attr_ptr->PRIORITY_CEILING != priority ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    i      = 0;
    result = _mutatr_get_priority_ceiling( mut_attr_ptr, &i );
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_EOK), "Test #6 Testing: 6.1.3: _mutatr_get_priority_ceiling operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (i == priority), "Test #6 Testing: 6.1.4: _mutatr_get_priority_ceiling should have returned i == priority")
    if ( i != priority ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
}

/*FUNCTION*-----------------------------------------------------------------
* 
* Task Name    : test_mutatr_setget_spin_limit
* Comments     :
*
*END*----------------------------------------------------------------------*/

void test_mutatr_setget_spin_limit
   (
      MUTEX_ATTR_STRUCT_PTR   mut_attr_ptr,
      _mqx_uint               spin_limit,
      _mqx_uint               test_no
   )
{
    _mqx_uint result,i;

    result = _mutatr_set_spin_limit( mut_attr_ptr, spin_limit );
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_EOK), "Test #7 Testing: 7.1.1: _mutatr_set_spin_limit operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (mut_attr_ptr->COUNT == spin_limit), "Test #7 Testing: 7.1.2: _mutatr_set_spin_limit operation")
    if ( mut_attr_ptr->COUNT != spin_limit ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    i      = 0;
    result = _mutatr_get_spin_limit( mut_attr_ptr, &i );
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_EOK), "Test #7 Testing: 7.1.3: _mutatr_get_spin_limit operation")
    if ( result != MQX_EOK ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (i == spin_limit), "Test #7 Testing: 7.1.4: _mutatr_get_spin_limit should have returned i == spin_limit")
    if ( i != spin_limit ) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
}

/* EOF */
