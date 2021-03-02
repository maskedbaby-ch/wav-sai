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
* $Version : 4.0.2.0$
* $Date    : July-15-2013$
*
* Comments:
*
*   This file tests the light weight event functions.
* Requirements:
*   LWEVENT005, LWEVENT011
*END************************************************************************/

#include <mqx_inc.h>
#include <lwevent.h>
#include <lwevent_prv.h>
#include <bsp.h>
#include <util.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test


#define MAIN_TASK       10
#define LWEVENT_TASK    11
#define WAIT_TASK       12

#define MAIN_PRIO       9

#define NUMBER_OF_TASKS 5

extern void       main_task(uint32_t parameter);
extern void       lwevent_task(uint32_t index);
extern void       wait_task(uint32_t index);
extern char   *_mqx_get_task_error_code_string(uint32_t);

void tc_1_main_task(void);// Test #1 - destruction of event....
void tc_2_main_task(void);// Test #2 - create lwevent....
void tc_3_main_task(void);// Test #3 - wait on lwevent with IN_ISR == 1...
void tc_4_main_task(void);// Test #4 - wait on invalid lwevent
void tc_5_main_task(void);// Test #5 - tasks wait on lwevent.....
void tc_6_main_task(void);// Test #6 - set to an invalid event.....
void tc_7_main_task(void);// Test #7 - lwevent testing error codes...
void tc_8_main_task(void);// Test #8 - test of lwevent normal operation....
void tc_9_main_task(void);// Test #9 - testing task priorities....

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK,    main_task,  1500,   MAIN_PRIO,     "main",     MQX_AUTO_START_TASK},
   { LWEVENT_TASK, lwevent_task, 1500,   MAIN_PRIO - 1, "lwevent",    0},
   { WAIT_TASK,    wait_task,  1500,   MAIN_PRIO - 1, "wait",     0},
   { 0,           0,          0,      0,             0,          0}
};

LWEVENT_STRUCT lwevent;
volatile _mqx_uint order_of_exec, counter, count[NUMBER_OF_TASKS];
bool tc_9_is_testing = 0;

/*TASK*-------------------------------------------------------------------
*
* Task Name    : lwevent_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void lwevent_task
   (
      uint32_t index
   )
{ /* Body */
    _mqx_uint  result, unblock_value;


    result = _lwevent_wait_ticks(&lwevent,1<<index,TRUE,0);
    if (tc_9_is_testing != 0)
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: _lwevent_wait_ticks operation")
    else
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: _lwevent_wait_ticks operation")

    unblock_value = _lwevent_get_signalled();
    if (tc_9_is_testing != 0)
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (unblock_value == (1<<index) ), "Test #9 Testing: _lwevent_get_signalled operation")
    else
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, (unblock_value == (1<<index) ), "Test #8 Testing: _lwevent_get_signalled operation")

    result = (lwevent.VALUE & unblock_value);
    if(index%2){//every even bit should by autocleared
       if(result == 0) result = MQX_OK;
       else result = 1; //ERROR;
    } else { //manual clear needed
       if(result != 0) result = MQX_OK;
       else result = 1; //ERROR;
    }
    if (tc_9_is_testing != 0)
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: _lwevent_auto_clear operation")
    else
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: _lwevent_auto_clear operation")


    count[index] = order_of_exec++;

    result = _lwevent_set(&lwevent, 1<<(index+1));
    if (tc_9_is_testing != 0)
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: _lwevent_set operation")
    else
        EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: _lwevent_set operation")
} /* Endbody */


/*TASK*-------------------------------------------------------------------
*
* Task Name    : wait_task
* Comments     :
*
*END*----------------------------------------------------------------------*/
void wait_task
   (
      uint32_t index
   )
{ /* Body */
    _mqx_uint result;

    while (1) {
        result = _lwevent_wait_ticks(&lwevent,1,TRUE,0);
        if (result == MQX_LWEVENT_INVALID) {
            break;
        } else {
            if (index == 0)
                counter++;
        } /* Endif */
    } /* Endwhile */
    counter++;
} /* Endbody */

/******************** Begin Testcases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Destroy uninitialized semaphore, returns MQX_LWEVENT_INVALID, 0x72
* Requirements :
*    LWEVENT005
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint               result;
    _mqx_uint in_isr_value;
    _mqx_uint valid_value;
    _mem_zero(&lwevent, sizeof(lwevent));
    result = _lwevent_destroy(&lwevent);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_LWEVENT_INVALID, "Test #1: Testing 1.1: Destruction of uninitialized event should have returned MQX_LWEVENT_INVALID");
    /* Test _lwevent_destroy() which will fail if calling from ISR */
    _int_disable();
    in_isr_value = get_in_isr();
    set_in_isr(1);
    result=_lwevent_destroy(&lwevent);
    set_in_isr(in_isr_value);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #1: Testing 1.2: _lwevent_destroy() fails and returns MQX_CANNOT_CALL_FUNCTION_FROM_ISR");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Create the lwevent, return code should be MQX_OK, 0.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _mqx_uint               result;

    result = _lwevent_create(&lwevent,0);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.1: Create the lwevent return code should be MQX_OK")

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Wait on lwevent with IN_ISR == 1 ,
*                         expected error, MQX CANNOT CALL FUNCTION FROM ISR, 0x0C
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    _mqx_uint               in_isr_value, result;


    _int_disable();
    in_isr_value = get_in_isr();
    set_in_isr(1);
    result = _lwevent_wait_ticks(&lwevent, 1, TRUE,0);
    set_in_isr(in_isr_value);
    _int_enable();

    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #3: Testing 3.1: lwevent_wait operation should have returned MQX_CANNOT_CALL_FUNCTION_FROM_ISR")

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Wait on invalid lwevent error: MQX_LWEVENT_INVALID = 0x72
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    _mqx_uint               valid_value, result;


    valid_value = lwevent.VALID;
    lwevent.VALID = 0;
    result = _lwevent_wait_ticks(&lwevent, 1, TRUE,0);
    lwevent.VALID = valid_value;
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_LWEVENT_INVALID, "Test #4: Testing 4.1: Wait on in valid lwevent should have returned MQX_LWEVENT_INVALID")

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Create 4 tasks waiting on the semapohre
*                         then destroy event, tasks should detect destruction.
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    _mqx_uint               result;
    _task_id                tid;


    counter = 0;
    tid = _task_create(0, WAIT_TASK, 0);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, tid != MQX_NULL_TASK_ID, "Test #5: Testing 5.1.1: Create instance of wait_task")

    tid = _task_create(0, WAIT_TASK, 1);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, tid != MQX_NULL_TASK_ID, "Test #5: Testing 5.1.2: Create instance of wait_task")

    tid = _task_create(0, WAIT_TASK, 2);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, tid != MQX_NULL_TASK_ID, "Test #5: Testing 5.1.3: Create instance of wait_task")

    tid = _task_create(0, WAIT_TASK, 3);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, tid != MQX_NULL_TASK_ID, "Test #5: Testing 5.1.4: Create instance of wait_task")
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, lwevent.VALUE == 0, "Test #5: Testing 5.2: Check lwevent.VALUE should be 0")
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, lwevent.WAITING_TASKS.SIZE == 4, "Test #5: Testing 5.3: Check lwevent.WAITING_TASKS.SIZE should be 4")

    counter = 0;
    /* Destroy the event */
    result = _lwevent_destroy(&lwevent);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.4: Destroy the lwevent operation")
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, counter == 4, "Test #5: Testing 5.5: Check all task were readied after sem destroyed")

    result = _lwevent_create(&lwevent,0);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.6: Create the lwevent operation")

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Set to an invalid event, error: MQX_LWEVENT_INVALID = 0x72
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    _mqx_uint               valid_value, result;


    valid_value = lwevent.VALID;
    lwevent.VALID = 0;
    result = _lwevent_set(&lwevent,1);
    lwevent.VALID = valid_value;
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_LWEVENT_INVALID, "Test #6: Testing 6.1: Set an invalid event should have returned MQX_LWEVENT_INVALID")

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7: lwevent test, error: MQX_CANNOT_CALL_FUNCTION_FROM_ISR = 0x0C
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    void                   *lwevent_error, *td_error;
    _mqx_uint               in_isr_value, valid_value, result, size;
    KERNEL_DATA_STRUCT_PTR  kernel_data = _mqx_get_kernel_data();

    lwevent_error = NULL;
    td_error = NULL;
    _int_disable();
    in_isr_value = get_in_isr();
    set_in_isr(1);
    result = _lwevent_test(&lwevent_error, &td_error);
    set_in_isr(in_isr_value);
    _int_enable();
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #7: Testing 7.1: Lwevent test error code should have returned MQX_CANNOT_CALL_FUNCTION_FROM_ISR")

    /*
    ** lwevent test, error: MQX_LWEVENT_INVALID
    */
    lwevent_error = NULL;
    td_error = NULL;
    valid_value = lwevent.VALID;
    lwevent.VALID = 0;
    result = _lwevent_test(&lwevent_error, &td_error);
    lwevent.VALID = valid_value;
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_LWEVENT_INVALID, "Test #7: Testing 7.2: Lwevent test should have returned MQX_LWEVENT_INVALID")
    if (result == MQX_LWEVENT_INVALID) {
       EU_ASSERT_REF_TC_MSG(tc_7_main_task, lwevent_error == &lwevent, "Test #7: Testing 7.3: Test lwevent_error")
    } /* Endif */

    /*
    ** lwevent test, error: MQX_CORRUPT_QUEUE:
    */
    lwevent_error = NULL;
    td_error = NULL;
    size = kernel_data->LWEVENTS.SIZE;
    kernel_data->LWEVENTS.SIZE = 99;
    result = _lwevent_test(&lwevent_error, &td_error);
    kernel_data->LWEVENTS.SIZE = size;
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_CORRUPT_QUEUE, "Test #7: Testing 7.4: lwevent_test error code should be MQX_CORRUPT_QUEUE")

    /*
    ** lwevent test, error: MQX_CORRUPT_QUEUE
    */
    lwevent_error = NULL;
    td_error = NULL;
    size = lwevent.WAITING_TASKS.SIZE;
    lwevent.WAITING_TASKS.SIZE = 99;
    result = _lwevent_test(&lwevent_error, &td_error);
    lwevent.WAITING_TASKS.SIZE = size;
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_CORRUPT_QUEUE, "Test #7: Testing 7.5: lwevent_test error code should MQX_CORRUPT_QUEUE")
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, lwevent_error == &lwevent, "Test #7: Testing 7.6: Test lwevent_error")

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8: Normal operation
* Requirements :
*   LWEVENT011
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
    _mqx_uint               i, result;
    _task_id                id[NUMBER_OF_TASKS];
    /* Test _lwevent_set_auto_clear() which shall return the error MQX_LWEVENT_INVALID if lightweight event group is not valid */
    _mqx_uint valid_value;
    valid_value=lwevent.VALID;
    lwevent.VALID=valid_value+1;
    result=_lwevent_set_auto_clear(&lwevent,0xAAAAAAAA);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_LWEVENT_INVALID, "Test #8: Testing 8.1: _lwevent_set_auto_clear() fails and returns MQX_LWEVENT_INVALID");
    lwevent.VALID=valid_value;

    result = _lwevent_set_auto_clear(&lwevent, 0xAAAAAAAA); //every even bit is set to AutoClear
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.2: lwevent_set_auto_clear operation")

    result = _lwevent_clear(&lwevent, -1);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.3: lwevent_clear operation")

    order_of_exec = 0;
    for (i = 0; i < NUMBER_OF_TASKS; i++) {
      id[i] = _task_create(0, LWEVENT_TASK, i);
      count[i] = 0;
    } /* Endfor */
    result = _lwevent_set(&lwevent,1);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.4: lwevent_set operation")

    for (i = 0; i < NUMBER_OF_TASKS; i++) {
      EU_ASSERT_REF_TC_MSG(tc_8_main_task, count[i] == i, "Test #8: Testing 8.5: Count operation")
    } /* Endfor */
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : TEST #9: different priorities
*
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
    _mqx_uint               i, result, old_prio;
    _task_id                id[NUMBER_OF_TASKS];

    tc_9_is_testing = 1;
    result = _lwevent_clear(&lwevent, -1);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9: Testing 9.1: Lwevent clear operation")

    order_of_exec = 0;
    for (i = 0; i < NUMBER_OF_TASKS; i++) {
        id[i] = _task_create(0, LWEVENT_TASK, i);
        _task_set_priority(id[i], MAIN_PRIO - i - 1, &old_prio);
        count[i] = 0;
    } /* Endfor */
    result = _lwevent_set(&lwevent,1);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9: Testing 9.2: lwevent set operation")

    for (i = 0; i < NUMBER_OF_TASKS; i++) {
        EU_ASSERT_REF_TC_MSG(tc_9_main_task, count[i] == i, "Test #9: Testing 9.3: Count operation")
    } /* Endfor */
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - destruction of uninitialized event...."),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - create lwevent...."),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - wait on lwevent with IN_ISR == 1..."),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - wait on invalid lwevent"),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - tasks wait on lwevent....."),
    EU_TEST_CASE_ADD(tc_6_main_task, " Test #6 - set to an invalid event....."),
    EU_TEST_CASE_ADD(tc_7_main_task, " Test #7 - lwevent testing error codes..."),
    EU_TEST_CASE_ADD(tc_8_main_task, " Test #8 - test of lwevent normal operation...."),
    EU_TEST_CASE_ADD(tc_9_main_task, " Test #9 - testing task priorities....")
EU_TEST_SUITE_END(suite_1)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, " - Test of Lightweight events")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------


/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t parameter
   )
{ /* Body */
   _int_install_unexpected_isr();
  // test_initialize();
  // EU_RUN_ALL_TESTS(test_result_array);
  // test_stop();
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* Endbody */
/* EOF */


