/**HEADER*********************************************************************
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
* $Version : 4.0.1.1$
* $Date    : Jun-1-2013$
*
* Comments   : This file contains the source functions for testing the accuracy
*              of the timer.
*
* Requirement:  TIME042,TIME043,TIME044,TIME064,TIME065,TIME001,TIME064,
*               TIME072,TIME073,TIME074,TIME075,TIME076,TIME077,TIME078,TIME079,
*               TIME080,TIME081,TIME082,TIME083,TIME087,TIME088,TIME089
*
*END***********************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define MAIN_TASK               10

#define DATE_LAST_YEAR          CLK_LAST_YEAR

void    main_task(uint32_t);

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    { MAIN_TASK,  main_task, 1500,  9,  "Main",  MQX_AUTO_START_TASK,  0},
    {         0,          0,    0,  0,       0,                    0,  0}
};

void tc_1_main_task(void);/* TEST #1: Testing function _time_to_date     */
void tc_2_main_task(void);/* TEST #2: Testing function _time_from_date   */
void tc_3_main_task(void);/* TEST #3: Testing function mktime            */
void tc_4_main_task(void);/* TEST #4: Testing function localtime_r       */
void tc_5_main_task(void);/* TEST #5: Testing function gmtime_r          */
void tc_6_main_task(void);/* TEST #6: Testing function timegm            */
void tc_7_main_task(void);/* TEST #7: Testing function _time_to_ticks    */
void tc_8_main_task(void);/* TEST #8: Testing function _ticks_to_time    */

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Testing function _time_to_date
*
* Requirement  : TIME042,TIME075,TIME076,TIME077
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    bool         result;
    TIME_STRUCT  time, time2;
    DATE_STRUCT  date;

    time.SECONDS = MAXIMUM_SECONDS_IN_TIME - 1;
    time.MILLISECONDS = MAXIMUM_MILLISECONDS;

    /* Calling function _time_to_date with one of parameters is NULL to verify the function must return FALSE*/
    result = _time_to_date(&time, NULL);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == FALSE, "Test #1 Testing 1.00: Test function _time_to_date operation");
    _task_set_error(MQX_OK);

    /* Calling function _time_to_date with one of parameters is NULL to verify the function must return FALSE*/
    result = _time_to_date(NULL, &date);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == FALSE, "Test #1 Testing 1.01: Test function _time_to_date operation");
    _task_set_error(MQX_OK);

    /* Calling the _time_to_date function in normal operation to verify the funtion should return TRUE */
    result = _time_to_date(&time, &date);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == TRUE, "Test #1 Testing 1.02: Test function _time_to_date operation");
    _task_set_error(MQX_OK);

    /* Calling the _time_from_date function to convert date get from previous step into time2 */
    result = _time_from_date(&date, &time2);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == TRUE, "Test #1 Testing 1.03: Test function _time_to_date operation");
    _task_set_error(MQX_OK);

    /* Verify the _time_to_data function called in previous step worked properly */
    result = (time.SECONDS == time2.SECONDS) && (time.MILLISECONDS == time2.MILLISECONDS);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == TRUE, "Test #1 Testing 1.04: Test function _time_to_date operation");

    time.MILLISECONDS = MAXIMUM_SECONDS_IN_TIME + 1;

    /* Calling _time_to_date function with input field is out of range to verify the function should return FALSE */
    result = _time_to_date(&time, &date);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == FALSE, "Test #1 Testing 1.05: Test function _time_to_date operation");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Testing function _time_from_date
*
* Requirement  : TIME042,TIME043,TIME044,TIME075
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    DATE_STRUCT         date;
    DATE_STRUCT         date2;
    TIME_STRUCT         time;
    bool                result;

    date.YEAR     = DATE_LAST_YEAR;
    date.MONTH    = MONTHS_IN_YEAR;
    date.DAY      = MAX_DAYS_IN_MONTH;
    date.HOUR     = HOURS_IN_DAY    - 1;
    date.MINUTE   = MINUTES_IN_HOUR - 1;
    date.SECOND   = SECS_IN_MINUTE  - 1;
    date.MILLISEC = MILLISECS_IN_SECOND - 1;

    /* Calling function _time_from_date with one of parameters to verify the function must return FALSE*/
    result = _time_from_date(NULL, &time);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == FALSE, "Test #2 Testing 2.00: _time_from_date conversion operation");
    _task_set_error(MQX_OK);

    /* Calling function _time_from_date with one of parameters to verify the function must return FALSE*/
    result = _time_from_date(&date, NULL);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == FALSE, "Test #2 Testing 2.01: _time_from_date conversion operation");
    _task_set_error(MQX_OK);

    /* Calling _time_from_date in normal operation to verify the function should return TRUE */
    result = _time_from_date(&date, &time);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == TRUE, "Test #2 Testing 2.02: _time_from_date conversion operation");
    _task_set_error(MQX_OK);

    /* Calling _time_to_date to convert the time get from previous step to date2 */
    result = _time_to_date(&time, &date2);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == TRUE, "Test #2 Testing 2.03: _time_to_date conversion operation");
    _task_set_error(MQX_OK);

    result = (date2.YEAR == date.YEAR) && (date2.MONTH == date.MONTH) && (date2.DAY == date.DAY) &&
                (date2.HOUR == date.HOUR) && (date2.MINUTE == date.MINUTE) && (date2.SECOND == date.SECOND);

    /* Verify the _time_from_date called in previous step worked properly */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result, "Test #2 Testing 2.04: date of _time_to_data conversion should be as expected");
    _task_set_error(MQX_OK);

    date.YEAR = DATE_LAST_YEAR + 1;

    /* Calling _time_from_date function with the input field is out of range to verify the function return FALSE */
    result = _time_from_date(&date, &time);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == FALSE, "Test #2 Testing 2.05: _time_from_date conversion operation");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Testing function mktime
*
* Requirement  :
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    struct tm tm_str;
    struct tm tm2_str;
    struct tm* tm_ptr;
    time_t timep;
    bool   result;

    tm_str.tm_year = DATE_LAST_YEAR    - 1900;
    tm_str.tm_mon  = MONTHS_IN_YEAR    - 1;
    tm_str.tm_mday = MAX_DAYS_IN_MONTH - 1;
    tm_str.tm_wday = DAYS_IN_WEEK      - 1;
    tm_str.tm_hour = HOURS_IN_DAY      - 1;
    tm_str.tm_min  = MINUTES_IN_HOUR   - 1;
    tm_str.tm_sec  = SECS_IN_MINUTE    - 1;

    /* Calling function mktime with one of parameters to verify the function must return FALSE*/
    timep = mktime(NULL);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, timep == 0, "Test #3 Testing 3.00: Test function mktime operation");
    _task_set_error(MQX_OK);

    /* Calling mktime in normal operation to verify the function should return TRUE */
    timep = mktime(&tm_str);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, timep != 0, "Test #3 Testing 3.01: Test function mktime operation");
    _task_set_error(MQX_OK);

    /* Calling localtime_r to convert the time get from previous step to tm2_srt */
    tm_ptr = localtime_r(&timep, &tm2_str);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, tm_ptr != NULL, "Test #3 Testing 3.02: Test function mktime operation");
    _task_set_error(MQX_OK);

    result = (tm_str.tm_year == tm2_str.tm_year) && (tm_str.tm_mon == tm2_str.tm_mon)
             && (tm_str.tm_mday == tm2_str.tm_mday) && (tm_str.tm_hour == tm2_str.tm_hour)
             && (tm_str.tm_min == tm2_str.tm_min) && (tm_str.tm_sec == tm2_str.tm_sec);

    /* Verify the mktime called in previous step worked properly */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result, "Test #3 Testing 3.03: Test function mktime operation");
    _task_set_error(MQX_OK);

    tm_str.tm_year = (DATE_LAST_YEAR + 1)    - 1900;

    result = mktime(&tm_str);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == FALSE, "Test #3 Testing 3.04: Test function mktime operation");
    _task_set_error(MQX_OK);

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Testing function localtime_r
*
* Requirement  :
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    time_t timep = MAXIMUM_SECONDS_IN_TIME;
    time_t timep2;
    struct tm  tm_str;
    struct tm* tm_ptr;
    bool   result;

    /* Calling function localtime_r with one of parameters to verify the function must return FALSE*/
    tm_ptr = localtime_r(NULL, &tm_str);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, tm_ptr == NULL, "Test #4 Testing 4.00: Test function localtime_r operation");
    _task_set_error(MQX_OK);

    /* Calling function localtime_r with one of parameters to verify the function must return FALSE*/
    tm_ptr = localtime_r(&timep, NULL);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, tm_ptr == NULL, "Test #4 Testing 4.01: Test function localtime_r operation");
    _task_set_error(MQX_OK);

    /* Calling localtime_r in normal operation to verify the function should return TRUE */
    tm_ptr = localtime_r(&timep, &tm_str);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, tm_ptr != NULL, "Test #4 Testing 4.02: Test function localtime_r operation");
    _task_set_error(MQX_OK);

    /* Calling mktime to convert the time get from previous step to tm2_srt */
    timep2 = mktime(tm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, timep2 != 0, "Test #4 Testing 4.03: Test function localtime_r operation");
    _task_set_error(MQX_OK);

    result = (timep == timep2);

    /* Verify the localtime_r called in previous step worked properly */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result, "Test #4 Testing 4.04: Test function localtime_r operation");
    _task_set_error(MQX_OK);

    timep = MAXIMUM_SECONDS_IN_TIME + 1;
    tm_ptr = localtime_r(&timep, &tm_str);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, tm_ptr == NULL, "Test #4 Testing 4.05: Test function localtime_r operation");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Testing function gmtime_r
*
* Requirement  :
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    time_t timep = MAXIMUM_SECONDS_IN_TIME;
    time_t timep2;
    struct tm tm_str;
    struct tm* tm_ptr;
    bool result;

    /* Calling function gmtime_r with one of parameters to verify the function must return FALSE*/
    tm_ptr = gmtime_r(NULL, &tm_str);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, tm_ptr == NULL, "Test #5 Testing 5.00: Test function gmtime_r operation");
    _task_set_error(MQX_OK);

    /* Calling function gmtime_r with one of parameters to verify the function must return FALSE*/
    tm_ptr = gmtime_r(&timep, NULL);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, tm_ptr == NULL, "Test #5 Testing 5.01: Test function gmtime_r operation");
    _task_set_error(MQX_OK);

    /* Calling gmtime_r in normal operation to verify the function should return TRUE */
    tm_ptr = gmtime_r(&timep, &tm_str);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, tm_ptr != NULL, "Test #5 Testing 5.02: Test function gmtime_r operation");
    _task_set_error(MQX_OK);

    /* Calling mktime to convert the time get from previous step to timep2 */
    timep2 = timegm(tm_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, timep2 != 0, "Test #5 Testing 5.03: Test function gmtime_r operation");
    _task_set_error(MQX_OK);

    result = (timep == timep2);

    /* Verify the gmtime_r called in previous step worked properly */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result, "Test #5 Testing 5.04: Test function gmtime_r operation");
    _task_set_error(MQX_OK);

    timep = MAXIMUM_SECONDS_IN_TIME + 1;
    tm_ptr = gmtime_r(&timep, &tm_str);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, tm_ptr == NULL, "Test #5 Testing 5.05: Test function gmtime_r operation");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Testing function: timegm
*
* Requirement  :
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    struct tm tm_str;
    struct tm tm2_str;
    struct tm* tm_ptr;
    time_t timep;
    bool result;

    tm_str.tm_year = DATE_LAST_YEAR    - 1900;
    tm_str.tm_mon  = MONTHS_IN_YEAR    - 1;
    tm_str.tm_mday = MAX_DAYS_IN_MONTH - 1;
    tm_str.tm_wday = DAYS_IN_WEEK      - 1;
    tm_str.tm_hour = HOURS_IN_DAY      - 1;
    tm_str.tm_min  = MINUTES_IN_HOUR   - 1;
    tm_str.tm_sec  = SECS_IN_MINUTE    - 1;

    /* Calling function timegm with one of parameters to verify the function must return FALSE*/
    timep = timegm(NULL);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, timep == 0, "Test #6 Testing 6.00: Test function timegm operation");
    _task_set_error(MQX_OK);

    /* Calling timegm in normal operation to verify the function should return TRUE */
    timep = timegm(&tm_str);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, timep != 0, "Test #6 Testing 6.01: Test function timegm operation");
    _task_set_error(MQX_OK);

    /* Calling gmtime_r to convert the time get from previous step to tm2_srt */
    tm_ptr = gmtime_r(&timep, &tm2_str);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, tm_ptr != NULL, "Test #6 Testing 6.02: Test function timegm operation");
    _task_set_error(MQX_OK);

    result = (tm_str.tm_year == tm2_str.tm_year) && (tm_str.tm_mon == tm2_str.tm_mon)
             && (tm_str.tm_mday == tm2_str.tm_mday) && (tm_str.tm_hour == tm2_str.tm_hour)
             && (tm_str.tm_min == tm2_str.tm_min) && (tm_str.tm_sec == tm2_str.tm_sec);

    /* Verify the gmtime called in previous step worked properly */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result, "Test #6 Testing 6.03: Test function timegm operation");
    _task_set_error(MQX_OK);

    tm_str.tm_year = (DATE_LAST_YEAR + 1) - 1900;
    result = timegm(&tm_str);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == FALSE, "Test #6 Testing 6.04: Test function timegm operation");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7: Testing function _time_to_ticks
*
* Requirement  : TIME078,TIME079,TIME080,TIME087
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    bool                result;
    TIME_STRUCT         time, time2;
    MQX_TICK_STRUCT     ticks;

    time.SECONDS = MAXIMUM_SECONDS_IN_TIME;
    time.MILLISECONDS = MAXIMUM_MILLISECONDS;

    /* Calling function _time_to_ticks with one of parameters is NULL to verify the function must return FALSE */
    result = _time_to_ticks(&time, NULL);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == FALSE, "Test #7 Testing 7.00: Test function _time_to_ticks operation");
    _task_set_error(MQX_OK);

    /* Calling function _time_to_ticks with one of parameters is NULL to verify the function must return FALSE */
    result = _time_to_ticks(NULL, &ticks );
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == FALSE, "Test #7 Testing 7.01: Test function _time_to_ticks operation");
    _task_set_error(MQX_OK);

    /* Calling the _time_to_ticks function in normal operation to verify the function should return TRUE */
    result = _time_to_ticks(&time, &ticks);
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == TRUE, "Test #7 Testing 7.02: Test function _time_to_ticks operation");
    _task_set_error(MQX_OK);

    /* Calling the _ticks_to_time function to convert ticks get from previous step to time2 */
    result = _ticks_to_time(&ticks, &time2);

    /* Verify the _time_to_ticks has converted time to tick correctly */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result && (time.SECONDS == time2.SECONDS), "Test #7 Testing 7.03: Test function _time_to_ticks operation");
    _task_set_error(MQX_OK);

    /* Calling the _time_to_ticks function with input field is out of range to verify the function should return FALSE */
    /* TODO: [ENGR00270397] The _time_to_ticks function does not check input range as RM/TIME080 describing */
    time.SECONDS = MAXIMUM_SECONDS_IN_TIME + 1;
    result = _time_to_ticks(&time, &ticks );
    EU_ASSERT_REF_TC_MSG( tc_7_main_task, result == FALSE, "Test #7 Testing 7.04: Test function _time_to_ticks operation");
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8: Testing function _ticks_to_time
*
* Requirement  : TIME087,TIME088,TIME089
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
    bool          result;
    TIME_STRUCT      times;
    MQX_TICK_STRUCT  ticks;

    /* Calling the _time_get_ticks to get the absolute time in ticks */
    _time_get_ticks(&ticks);

    /* Calling function _ticks_to_time with one of parameters is NULL to verify the function must return FALSE */
    result = _ticks_to_time(&ticks, NULL);
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, result == FALSE, "Test #8 Testing 8.00: Test function _ticks_to_time operation");
    _task_set_error(MQX_OK);

    /* Calling function _ticks_to_time with one of parameters is NULL to verify the function must return FALSE */
    result = _ticks_to_time(NULL, &times);
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, result == FALSE, "Test #8 Testing 8.01: Test function _ticks_to_time operation");
    _task_set_error(MQX_OK);

    /* Calling function _ticks_to_time in normal operation should return TRUE */
    result = _ticks_to_time(&ticks, &times);
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, result == TRUE, "Test #8 Testing 8.02: Test function _ticks_to_time operation");
    _task_set_error(MQX_OK);

    /* Set fields of ticks to maximum values */
    ticks.TICKS[0] = MAX_UINT_32;
    ticks.TICKS[1] = MAX_UINT_32;
    ticks.HW_TICKS = MAX_UINT_32;

    /* Calling _ticks_to_time with the very large value of ticks to verify the function should return TRUE */
    /* TODO: [ENGR00271568] The _ticks_to_time does not return correctly */
    result = _ticks_to_time(&ticks, &times);
    EU_ASSERT_REF_TC_MSG( tc_8_main_task, result == FALSE, "Test #8 Testing 8.03: Test function _ticks_to_time operation");
    _task_set_error(MQX_OK);
}

/*******  Add test cases */
 EU_TEST_SUITE_BEGIN(suite_convert)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1: Testing function _time_to_date"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2: Testing function _time_from_date "),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST #3: Testing function mktime"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST #4: Testing function localtime_r"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST #5: Testing function gmtime_r"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST #6: Testing function timegm"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST #7: Testing function _time_to_ticks"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST #8: Testing function _ticks_to_time"),
 EU_TEST_SUITE_END(suite_convert)

/*******  Add test suites */
 EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_convert, " Test of time module")
 EU_TEST_REGISTRY_END()

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   Just read and display the time.
*
*END*----------------------------------------------------------------------*/
 void main_task
    (
       uint32_t parameter
    )
 {

    _int_install_unexpected_isr();

    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
 } /* Endbody */

/* EOF */
