/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved                       
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
************************************************************************** 
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
* $Version : 3.8.10.1$
*
* Comments:
*
*   This file contains code for the MQX time verification
*   program.
*
*END************************************************************************/

/*
** Time primitives
**   _time_notify_kernel, _time_set_timer_vector and _time_set_resolution
**    are internal functions and are not included in the testing
*/

#include <mqx.h>
#include <bsp.h>
#include "util.h"
//#include <test_module.h>

#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME test

#define MAIN_TASK  1
#define TEST_TASK  2
#define DELAY_TASK 3
#define NUM_SAMPLES  200

/* #define KLOG_ON */
#define DEBUG

void main_task(uint32_t);
void test_task(uint32_t);
void delay_task(uint32_t);

void tc_1_main_task(void);// TEST #1 - Testing _time_set, _time_get, _time_get_elapsed, _time_diff
void tc_2_main_task(void);// TEST #2 - Testing _time_from_date, _time_to_date
void tc_3_main_task(void);// TEST #3 - Testing _time_get_resolution, _time_get_microseconds, _time_delay
void tc_4_main_task(void);// TEST #4 - Testing _time_delay, _time_dequeue, time_dequeue_td

bool time_date_test(uint16_t year, uint16_t month, uint16_t day);

uint32_t count;

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
#if PSP_MQX_CPU_IS_KINETIS_L
   { MAIN_TASK,   main_task,  1000,   8,   "Main",  MQX_AUTO_START_TASK, 0},
   { TEST_TASK,   test_task,  1000,   8,   "test_task",  0 ,             0},
   { DELAY_TASK,  delay_task, 1000,   8,   "delay_task", 0,              0},
#else
   { MAIN_TASK,   main_task,  8000,   8,   "Main",  MQX_AUTO_START_TASK, 0},
   { TEST_TASK,   test_task,  2000,   8,   "test_task",  0 ,             0},
   { DELAY_TASK,  delay_task, 2000,   8,   "delay_task", 0,              0},
#endif   
   { 0,           0,          0,      0,   0,            0,              0}

};

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST #1: Testing the following functions:
//              _time_set, _time_get, _time_get_elapsed, _time_diff
//END---------------------------------------------------------------------

void tc_1_main_task()
{
    TIME_STRUCT       time_start;
    TIME_STRUCT       time_end;
    TIME_STRUCT       diff_time;
    
    
    _time_delay_ticks(1); 
    _int_disable();
    
    /* Obtain kernel time */
    _time_get_elapsed(&time_start);
    
    /* Move absolute MQX time forward */
    time_start.SECONDS++;
    _time_set(&time_start);
    
    /* Count diff between actual MQX and kernel time */ 
    _time_get_elapsed(&time_end);
    _time_get(&time_start);
    _time_diff(&time_end, &time_start, &diff_time);
    
    /* Diff should be 1s and less than 1 tick [in ms] */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, time_check_struct(diff_time,1,0,1), "Test #1 Testing: 1.1: diff time between 1st get and 2nd get should be approximatly 1 second");
    
    _int_enable();
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST #2: Testing the following functions:
//              _time_from_date, _time_to_date
//END---------------------------------------------------------------------
void tc_2_main_task()
{
    _mqx_uint result;

    result = time_date_test(2000, 2, 29);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result, "Test #2 Testing: test with time date 2000, 2, 29 should be ok");

    result = time_date_test(1998, 4, 30);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result, "Test #2 Testing: test with time date 1998, 4, 30 should be ok");
    
}

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST #3: Testing the following functions:
//              _time_get_resolution, _time_get_microseconds, _time_delay
//END---------------------------------------------------------------------

void tc_3_main_task()
{
    bool           warn;
    uint16_t           data[NUM_SAMPLES];
    _mqx_uint         resolution, i, j, avg = 0, num = 0, low, high, inc, err, calc_res;
    volatile uint32_t  k = 0;
    calc_res = (1000/BSP_ALARM_FREQUENCY);
    resolution = _time_get_resolution();
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, (resolution == calc_res), "Test #3 Testing: 3.1: _time_get_resolution operation");
    _int_disable();
    _time_delay_ticks(2);

    for (i = 0; i < NUM_SAMPLES; ++i) {
        data[i] = _time_get_microseconds();
        for (j = 0; j < 200; j++) {
            k = k * 99;
        } /* Endfor */
    } /* Endfor */
    _int_enable();

    warn = TRUE;
    for (i = 1; i < NUM_SAMPLES; ++i) {
        if ((data[i] >= (resolution * 1000)) && warn) {
            warn = FALSE;
            break;
        } /* Endif */
        if (data[i] > data [i - 1]) {
            num++;
            avg += (data[i] - data[i - 1]);
        } else {
            break;
        } /* Endif */
    } /* Endfor */
    
    EU_ASSERT_REF_TC_MSG( tc_3_main_task, (num != 0), "Test #3 Testing: valid samples should be enough");
    if (num == 0) {
    } else {
        avg /= num;
        /* Differences should be withing +/-5% of the average */
        err = ((avg + 10) / 20) + 1;
        err = (err > 2*resolution)? err : 2*resolution;
        low  = avg - err;
        high = avg + err;
        /* Re-calculate low and high for targets with fast tickers */
        if ( (avg - low) < 2 ) {
            low = avg - 2;
        } /* Endif */
        if ( (high - avg) < 2 ) {
            high = avg + 2;
        } /* Endif */

        if ((_mqx_int)low < 0) low = 0;
        /* Don't include the first sample */
        for (i = 2; i < num; i++) {
            if (data[i] >= resolution * 2000) {
                break;
            } /* Endif */
            if (data[i] > data[i - 1]) {
                inc = data[i] - data[i - 1];
            } else {
                continue; /* Timer interrupt should happen here */
                /* inc = (resolution * 1000) - data[i - 1] + data[i]; */
            } /* Endif */
            if ((inc < low) || (inc > high)) {
                /* Allow a bit of jitter when passing thru alarm resolution */
                EU_ASSERT_REF_TC_MSG( tc_3_main_task, ((data[i] > (resolution * 1000)) && (data[i-1] < (resolution * 1000))), "Test #3 Testing: 3.2: _time_get_microseconds operation");
                if ((data[i] > (resolution * 1000)) && 
                    (data[i-1] < (resolution * 1000)))
                {
                    continue;
                } else {
                    break;
                } /* Endif */
            } /* Endif */
        } /* Endfor */
        
        EU_ASSERT_REF_TC_MSG( tc_3_main_task, (avg != 0), "Test #3 Testing: 3.3: _time_get_microseconds should have avg is non-zero");
     } /* Endif */
} /* Endbody */

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST #4: Testing the following functions:
//              _time_delay, _time_dequeue, time_dequeue_td
//END---------------------------------------------------------------------

void tc_4_main_task()
{
    _task_create(0, TEST_TASK, 0);
    _task_block();
}

/*FUNCTION*---------------------------------------------------------------
* 
* Function Name : time_date_test
* Comments      : Test time_from_date and time_to_date
*
*END*-------------------------------------------------------------------*/
bool time_date_test
   (
      uint16_t year,
      uint16_t month,
      uint16_t day
   )
{
/* Body */
    DATE_STRUCT date;
    TIME_STRUCT time;
    bool     passed = TRUE;
    bool     result;

    date.YEAR      = year;
    date.MONTH     = month;
    date.DAY       = day;
    date.HOUR      = 0;
    date.MINUTE    = 0;
    date.SECOND    = 0;
    date.MILLISEC  = 0;

    result = _time_from_date (&date, &time);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result, "Test #2 Testing: 2.1: _time_from_date conversion operation");
    if (!result) {
        passed = FALSE;
    }/* Endif */
    _time_set (&time);
    _time_get (&time);
    result = _time_to_date (&time, &date);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result, "Test #2 Testing: 2.2: _time_to_date conversion operation");
    if (!result) {
        passed = FALSE;
    }/* Endif */
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, (date.YEAR == year && date.MONTH == month && date.DAY == day), "Test #2 Testing: 2.3: date of _time_to_data conversion should be as expected");
    if (date.YEAR != year || date.MONTH != month || date.DAY != day) {
        passed = FALSE;
    } /* Endif */
    if (passed) {
        return TRUE;
    } else {
        return FALSE;
    }/* Endif */

} /* Endbody */

//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_get_set)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Testing _time_set, _time_get, _time_get_elapsed, _time_diff"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Testing _time_from_date, _time_to_date"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST #3 - Testing _time_get_resolution, _time_get_microseconds, _time_delay."),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST #4 - Testing _time_delay, _time_dequeue, time_dequeue_td")
 EU_TEST_SUITE_END(suite_get_set)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_get_set, "- Test of kernel time functions.")
 EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------
 
/*TASK*-------------------------------------------------------------------
* 
* Task Name : main_task
* Comments  : Test _time_set, _time_get, _time_get_elapsed, _time_diff
*
*END*-------------------------------------------------------------------*/
           
void main_task
   (
      uint32_t param
   )
{/* Body */
    
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
}
 
/*TASK*-------------------------------------------------------------------
* 
* Task Name : test_task
* Comments  :
*
*END*-------------------------------------------------------------------*/
void test_task
   (
      uint32_t param
   )
{/* Body */
    _task_id taskb_id, taskc_id;
    void    *taskc_td;

    taskb_id = _task_create (0, DELAY_TASK, 0);
    taskc_id = _task_create (0, DELAY_TASK, 0);
    count = 0;
    _sched_yield();
    _time_dequeue (taskb_id);
    taskc_td = _task_get_td (taskc_id);
    _time_dequeue_td (taskc_td);
    _time_delay (100);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (count == 0), "Test #4 Testing: 4.1: _time_dequeue operation");
/*b23362*/
    eunit_test_stop();
} /* Endbody */

/*TASK*-------------------------------------------------------------------
* 
* Task Name : delay_task
* Comments  :
*
*END*-------------------------------------------------------------------*/
void delay_task
   (
      uint32_t param
   )
{/* Body */

    _time_delay(50);
    count++;

}/* Endbody */
