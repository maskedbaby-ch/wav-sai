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
* Requirement: TIME010,TIME014,TIME034,TIME050,TIME062,TIME012,TIME016,
               TIME005,TIME008,TIME040,TIME041,TIME007,TIME038,TIME039,
               TIME006,TIME036,TIME037,TIME035,TIME004,TIME032,TIME033,
               TIME003,TIME030,TIME031,TIME002,TIME028,TIME029,TIME001,
               TIME026,TIME027,TIME024,TIME025,
*
*END***********************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define MAIN_TASK        10
#define TEST_TASK        11

#define DELAY_TICKS      10
#define TIME_TEST        1

#define PSEC_TO_MSEC(x)  ((x)/1000000000)
#define NSEC_TO_MSEC(x)  ((x)/1000000)
#define USEC_TO_MSEC(x)  ((x)/1000)

#define ABS_DIFF(a, b)   ((a) > (b) ? (a) - (b) : (b) - (a))


void    main_task(uint32_t);
void    test_task(uint32_t);

bool             test_task_run;
MQX_TICK_STRUCT     zero_tick_struct;

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    { MAIN_TASK,  main_task, 1500,  9,  "Main",  MQX_AUTO_START_TASK,  0},
    { TEST_TASK,  test_task, 1000,  10, "Test",                    0,  0},
    {         0,          0,    0,  0,       0,                    0,  0}
};

void tc_1_main_task(void);/* TEST #1: Test the function _time_delay_for     */
void tc_2_main_task(void);/* TEST #2: Test the function _time_delay_until   */
void tc_3_main_task(void);/* TEST #3: Test functions _time_add_psec_to_ticks, time_diff_picoseconds */
void tc_4_main_task(void);/* TEST #4: Test functions _time_add_nsec_to_ticks, time_diff_nanoseconds */
void tc_5_main_task(void);/* TEST #5: Test functions _time_add_usec_to_ticks, time_diff_microseconds */
void tc_6_main_task(void);/* TEST #6: Test functions _time_add_msec_to_ticks, time_diff_milliseconds */
void tc_7_main_task(void);/* TEST #7: Test functions _time_add_sec_to_ticks, time_diff_seconds */
void tc_8_main_task(void);/* TEST #8: Test functions _time_add_min_to_ticks, time_diff_minutes */
void tc_9_main_task(void);/* TEST #9: Test functions _time_add_hour_to_ticks, time_diff_hours */
void tc_10_main_task(void);/* TEST #10: Testing functions _time_add_day_to_ticks, _time_diff_days */
void tc_11_main_task(void);/* TEST #11: Testing functions _time_diff_ticks */

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test the function _time_delay_for
*
* Requirement  : TIME010,TIME014,TIME034,TIME050,TIME062
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    bool             over_flow;
    uint32_t            delay_ms, diff_ms;
    _task_id            tid;
    MQX_TICK_STRUCT     ticks, start_ticks, end_ticks;

    /* Calling function _time_init_ticks to initialize a tick time structure with the number of ticks */
    _time_init_ticks(&ticks, DELAY_TICKS);

    /* Create TEST_TASK */
    tid = _task_create(0, TEST_TASK, 0);

    /* Calling function _time_delay_for with input parameter is NULL to verify the function will set task error code to MQX_INVALID_PARAMETER */
    _time_delay_for(NULL);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MQX_INVALID_PARAMETER, "Test #1 Testing 1.00: Tests function _time_delay_for with parameter is not correct");
    _task_set_error(MQX_OK);

    test_task_run = FALSE;

    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    /* Calling function _time_delay_for to suspend the active task. Checking the global variable
     * task_test_run must be TRUE to verify the main task is suspended and TEST_TASK run */
    _time_delay_for(&ticks);
    _time_get_elapsed_ticks(&end_ticks);

    EU_ASSERT_REF_TC_MSG(tc_1_main_task, test_task_run == TRUE, "Test #1 Testing 1.01: Tests operation of function _time_delay_for");
    _task_set_error(MQX_OK);

    diff_ms = _time_diff_milliseconds(&end_ticks, &start_ticks, &over_flow);
    delay_ms = _time_diff_milliseconds(&ticks, &zero_tick_struct, &over_flow);

    /* Verify the delay time must be accurate, difference is within 1 tick */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ABS_DIFF(diff_ms, delay_ms) < _time_get_resolution(), "Test #1 Testing 1.02: Tests operation of function _time_delay_until");

    _task_destroy(tid);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Test the function _time_delay_until
*
* Requirement  : TIME012,TIME016,TIME050,TIME062
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _task_id            tid;
    MQX_TICK_STRUCT     ticks, end_ticks;

    /* Create TEST_TASK */
    tid = _task_create(0, TEST_TASK, 0);

    /* Calling function _time_delay_until when the tick_time_delay_ptr parameter is not correct to
     * verify task error code must be MQX_INVALID_PARAMETER */
    _time_delay_until(NULL);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_INVALID_PARAMETER, "Test #2 Testing 2.00: Tests function _time_delay_until with parameter is not correct");
    _task_set_error(MQX_OK);

    test_task_run = FALSE;

    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&ticks);

    /* Add DELAY_TICKS to ticks struct */
    _time_add_msec_to_ticks(&ticks, DELAY_TICKS * _time_get_resolution());

    /* Calling function _time_delay_until to suspend the active task. Checking the global variable
     * task_test_run must be TRUE to verify the main task is suspended and TEST_TASK run */
    _time_delay_until(&ticks);
    _time_get_elapsed_ticks(&end_ticks);

    EU_ASSERT_REF_TC_MSG(tc_2_main_task, test_task_run == TRUE, "Test #2 Testing 2.01: Tests operation of function _time_delay_until");
    _task_set_error(MQX_OK);

    /* Verify the delay time must be accurate */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, PSP_CMP_TICKS(&end_ticks, &ticks) == 0, "Test #2 Testing 2.02: Tests operation of function _time_delay_until");

    _task_destroy(tid);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Test functions _time_add_psec_to_ticks, time_diff_picoseconds
*
* Requirement  : TIME005,TIME008,TIME010,TIME012,TIME040,TIME041,TIME050,TIME062
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    bool             over_flow;
    uint32_t            psecs, diff_psecs, i;
    MQX_TICK_STRUCT     start_ticks, end1_ticks, end2_ticks;
    MQX_TICK_STRUCT     delay_ticks, ticks;

    /* Convert maximum uint32_t picoseconds to ticks */
    PSP_PICOSECONDS_TO_TICKS(MAX_UINT_32, &ticks);

    /* Add one tick to the ticks */
    _time_add_msec_to_ticks(&ticks, _time_get_resolution());

    /* Verify the over_flow should be TRUE */
    _time_diff_picoseconds(&ticks, &zero_tick_struct, &over_flow);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, over_flow, "Test #3 Testing 3.00: The function _time_diff_picoseconds should set over_flow to TRUE");

    /* Initialize delay_ticks by 1 tick */
    _time_init_ticks(&delay_ticks, 1);
    /* Convert to 1 tick to picoseconds */
    psecs = PSP_TICKS_TO_PICOSECONDS(&delay_ticks, &over_flow);

    /* If 1 tick to picoseconds conversion is overflow */
    if (over_flow)
    {
        ticks = delay_ticks;
        /* Add maximum uint32_t value by _time_add_psec_to_ticks will no change the ticks in this case */
        _time_add_psec_to_ticks(&ticks, MAX_UINT_32);
        /* Verify the ticks is not changed */
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, PSP_CMP_TICKS(&ticks, &delay_ticks) == 0, "Test #3 Testing 3.01: Tests operation of function _time_add_psec_to_ticks");
        _task_set_error(MQX_OK);

        /* Add one tick to the ticks */
        _time_add_msec_to_ticks(&ticks, _time_get_resolution());

        /* Verify the function _time_diff_picoseconds will over_flow and return value is non-zero */
        diff_psecs = _time_diff_picoseconds(&ticks, &delay_ticks, &over_flow);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, over_flow && (diff_psecs > 0), "Test #3 Testing 3.02: Tests operation of function _time_diff_picoseconds");
        _task_set_error(MQX_OK);
    }
    else
    {
        i = DELAY_TICKS;
        /* Try to get more than one tick in delay_ticks */
        do
        {
            _time_init_ticks(&delay_ticks, i);
            psecs = PSP_TICKS_TO_PICOSECONDS(&delay_ticks, &over_flow);
            i--;
        } while ((i > 0) && over_flow);

        _time_delay_ticks(1);
        _time_get_elapsed_ticks(&start_ticks);
        end1_ticks = start_ticks;
        /* make end1_ticks to be destination time with psecs added */
        _time_add_psec_to_ticks(&end1_ticks, psecs);
        _time_delay_until(&end1_ticks);
        _time_get_elapsed_ticks(&end1_ticks);
        _time_delay_for(&delay_ticks);
        _time_get_elapsed_ticks(&end2_ticks);

        /* Verify the _time_add_psec_to_ticks added psecs to end1_ticks correctly */
        diff_psecs = _time_diff_picoseconds(&end1_ticks, &start_ticks, &over_flow);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, PSEC_TO_MSEC(ABS_DIFF(psecs, diff_psecs)) < 1, "Test #3 Testing 3.01: Tests operation of function _time_add_psec_to_ticks");
        _task_set_error(MQX_OK);

        /* Verify the function _time_diff_picoseconds is not overflow and return value is non-zero */
        diff_psecs = _time_diff_picoseconds(&end2_ticks, &end1_ticks, &over_flow);
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (!over_flow) && (diff_psecs > 0), "Test #3 Testing 3.02: Tests operation of function _time_diff_picoseconds");
        _task_set_error(MQX_OK);

        /* Verify the different should be less than 1 milliseconds */
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, PSEC_TO_MSEC(ABS_DIFF(psecs, diff_psecs)) < 1, "Test #3 Testing 3.03: Tests operation of function _time_diff_picoseconds");
        _task_set_error(MQX_OK);
    }
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Test functions _time_add_nsec_to_ticks, time_diff_nanoseconds
*
* Requirement  : TIME005,TIME007,TIME010,TIME012,TIME038,TIME039,TIME050,TIME062
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    bool             over_flow;
    uint32_t            nsecs, diff_nsecs, i;
    MQX_TICK_STRUCT     start_ticks, end1_ticks, end2_ticks;
    MQX_TICK_STRUCT     delay_ticks, ticks;

    /* Convert maximum uint32_t nanoseconds to ticks */
    PSP_NANOSECONDS_TO_TICKS(MAX_UINT_32, &ticks);

    /* Add one tick to the ticks */
    _time_add_msec_to_ticks(&ticks, _time_get_resolution());

    /* Verify the over_flow should be TRUE */
    _time_diff_nanoseconds(&ticks, &zero_tick_struct, &over_flow);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, over_flow, "Test #4 Testing 4.00: The function _time_diff_nanoseconds should set over_flow to TRUE");

    /* Initialize delay_ticks by 1 tick */
    _time_init_ticks(&delay_ticks, 1);
    /* Convert to 1 tick to nanoseconds */
    nsecs = PSP_TICKS_TO_NANOSECONDS(&delay_ticks, &over_flow);

    /* If 1 tick to nanoseconds conversion is overflow */
    if (over_flow)
    {
        ticks = delay_ticks;
        /* Add maximum uint32_t value by _time_add_nsec_to_ticks will no change the ticks in this case */
        _time_add_nsec_to_ticks(&ticks, MAX_UINT_32);
        /* Verify the end1_tick is not changed */
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, PSP_CMP_TICKS(&ticks, &delay_ticks) == 0, "Test #4 Testing 4.01: Tests operation of function _time_add_nsec_to_ticks");
        _task_set_error(MQX_OK);

        /* Add one tick to the ticks */
        _time_add_msec_to_ticks(&ticks, _time_get_resolution());

        /* Verify the function _time_diff_nanoseconds will over_flow and return non-zero value */
        diff_nsecs = _time_diff_nanoseconds(&ticks, &delay_ticks, &over_flow);
         EU_ASSERT_REF_TC_MSG(tc_4_main_task, over_flow && (diff_nsecs > 0), "Test #4 Testing 4.02: Tests operation of function _time_diff_nanoseconds");
         _task_set_error(MQX_OK);
    }
    else
    {
        i = DELAY_TICKS;
        /* Try to get more than one tick in delay_ticks */
        do
        {
            _time_init_ticks(&delay_ticks, i);
            nsecs = PSP_TICKS_TO_NANOSECONDS(&delay_ticks, &over_flow);
            i--;
        } while ((i > 0) && over_flow);

        _time_delay_ticks(1);
        _time_get_elapsed_ticks(&start_ticks);
        end1_ticks = start_ticks;
        /* make end1_ticks to be destination time with nsecs added */
        _time_add_nsec_to_ticks(&end1_ticks, nsecs);
        _time_delay_until(&end1_ticks);
        _time_get_elapsed_ticks(&end1_ticks);
        _time_delay_for(&delay_ticks);
        _time_get_elapsed_ticks(&end2_ticks);

        /* Verify the _time_add_nsec_to_ticks added nsecs to end1_ticks correctly */
        diff_nsecs = _time_diff_nanoseconds(&end1_ticks, &start_ticks, &over_flow);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, NSEC_TO_MSEC(ABS_DIFF(nsecs, diff_nsecs)) < 1, "Test #4 Testing 4.01: Tests operation of function _time_add_nsec_to_ticks");
        _task_set_error(MQX_OK);

        /* Verify the function _time_diff_nanoseconds is not overflow and return non-zero value */
        diff_nsecs = _time_diff_nanoseconds(&end2_ticks, &end1_ticks, &over_flow);
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (!over_flow) && (diff_nsecs > 0), "Test #4 Testing 4.02: Tests operation of function _time_diff_nanoseconds");
        _task_set_error(MQX_OK);

        /* Verify the different should be less than 1 milliseconds */
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, NSEC_TO_MSEC(ABS_DIFF(nsecs, diff_nsecs)) < 1, "Test #4 Testing 4.03: Tests operation of function _time_diff_nanoseconds");
        _task_set_error(MQX_OK);
    }

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Test functions _time_add_usec_to_ticks, time_diff_microseconds
*
* Requirement  : TIME005,TIME006,TIME010,TIME012,TIME036,TIME037,TIME050,TIME062
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    bool             over_flow;
    uint32_t            usecs, diff_usecs;
    MQX_TICK_STRUCT     start_ticks, end1_ticks, end2_ticks;
    MQX_TICK_STRUCT     delay_ticks, ticks;

    _time_init_ticks(&delay_ticks, DELAY_TICKS);
    usecs = PSP_TICKS_TO_MICROSECONDS(&delay_ticks, &over_flow);

    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    end1_ticks = start_ticks;
    /* make end1_ticks to be destination time with usecs added */
    _time_add_usec_to_ticks(&end1_ticks, usecs);
    _time_delay_until(&end1_ticks);
    _time_get_elapsed_ticks(&end1_ticks);
    _time_delay_for(&delay_ticks);
    _time_get_elapsed_ticks(&end2_ticks);

    /* Verify the _time_add_usec_to_ticks added usecs to end1_ticks correctly */
    diff_usecs = _time_diff_microseconds(&end1_ticks, &start_ticks, &over_flow);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, USEC_TO_MSEC(ABS_DIFF(usecs, diff_usecs)) < 1 , "Test #5 Testing 5.00: Tests operation of function _time_add_usec_to_ticks");
    _task_set_error(MQX_OK);

    /* Verify the function _time_diff_microseconds is not overflow and return non-zero value */
    diff_usecs = _time_diff_microseconds(&end2_ticks, &end1_ticks, &over_flow);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, diff_usecs > 0, "Test #5 Testing 5.01: Tests operation of function _time_diff_microseconds");
    _task_set_error(MQX_OK);

    /* Verify the different should be less than 1 milliseconds */
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, USEC_TO_MSEC(ABS_DIFF(usecs, diff_usecs)) < 1, "Test #5 Testing 5.02: Tests operation of function _time_diff_microseconds");
    _task_set_error(MQX_OK);

    /* Convert maximum microseconds to ticks */
    PSP_MICROSECONDS_TO_TICKS(MAX_UINT_32, &ticks);

    /* Add one tick to the ticks */
    _time_add_msec_to_ticks(&ticks, _time_get_resolution());

    /* Verify The function _time_diff_microseconds should set over_flow to TRUE */
    _time_diff_microseconds(&ticks, &zero_tick_struct, &over_flow);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, over_flow, "Test #5 Testing 5.03: The function _time_diff_microseconds should set over_flow to TRUE");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Test functions _time_add_msec_to_ticks, time_diff_milliseconds
*
* Requirement  : TIME005,TIME010,TIME012,TIME034,TIME035,TIME050,TIME062
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    bool             over_flow;
    uint32_t            msecs, diff_msecs;
    MQX_TICK_STRUCT     start_ticks, end1_ticks, end2_ticks;
    MQX_TICK_STRUCT     delay_ticks, ticks;

    /* Calling function _time_init_ticks to initialize a tick time structure with the number of ticks */
    _time_init_ticks(&delay_ticks, DELAY_TICKS);
    msecs = PSP_TICKS_TO_MILLISECONDS(&delay_ticks, &over_flow);

    _time_delay_ticks(1);
    _time_get_elapsed_ticks(&start_ticks);
    end1_ticks = start_ticks;
    /* make end1_ticks to be destination time with usecs added */
    _time_add_msec_to_ticks(&end1_ticks, msecs);
    _time_delay_until(&end1_ticks);
    _time_get_elapsed_ticks(&end1_ticks);
    _time_delay_for(&delay_ticks);
    _time_get_elapsed_ticks(&end2_ticks);

    /* Verify the _time_add_msec_to_ticks added msecs to end1_ticks correctly */
    diff_msecs = _time_diff_milliseconds(&end1_ticks, &start_ticks, &over_flow);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, ABS_DIFF(msecs, diff_msecs) < 1, "Test #6 Testing 6.00: Tests operation of function _time_add_msec_to_ticks");
    _task_set_error(MQX_OK);

    /* Verify the function _time_diff_milliseconds returns non-zero value */
    diff_msecs = _time_diff_milliseconds(&end2_ticks, &end1_ticks, &over_flow);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, diff_msecs > 0, "Test #6 Testing 6.01: Tests operation of function _time_diff_milliseconds");
    _task_set_error(MQX_OK);

    /* Verify the _time_add_msec_to_ticks and _time_diff_milliseconds work properly */
    /* the different should be less than 1 milliseconds */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, ABS_DIFF(msecs, diff_msecs) < 1, "Test #6 Testing 6.02: Tests operation of function _time_diff_milliseconds");
    _task_set_error(MQX_OK);

    /* Convert maximum uint32_t milliseconds to ticks */
    PSP_MILLISECONDS_TO_TICKS(MAX_UINT_32, &ticks);

    /* Add one tick to the ticks */
    _time_add_msec_to_ticks(&ticks, _time_get_resolution());

    /* Verify The function _time_diff_milliseconds should set over_flow to TRUE */
    _time_diff_milliseconds(&ticks, &zero_tick_struct, &over_flow);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, over_flow, "Test #6 Testing 6.03: The function _time_diff_milliseconds should set over_flow to TRUE");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7: Test functions _time_add_sec_to_ticks, time_diff_seconds
*
* Requirement  : TIME004,TIME005,TIME032,TIME033,TIME062
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    bool             over_flow;
    uint32_t            secs, diff_secs;
    MQX_TICK_STRUCT     ticks_1, ticks_2;

    /* Initialize time */
    secs = TIME_TEST;

    /* Initialize ticks_1 and ticks_2 to zero */
    _time_init_ticks(&ticks_1, 0);
    _time_init_ticks(&ticks_2, 0);

    /* Add secs to ticks_1 by _time_add_sec_to_ticks */
    _time_add_sec_to_ticks(&ticks_1, secs);

    /* Add secs * MILLISECS_IN_SECOND to ticks_2 by _time_add_msec_to_ticks */
    _time_add_msec_to_ticks(&ticks_2, secs * MILLISECS_IN_SECOND);

    /* verify ticks_1 and ticks_2 are equal */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, PSP_CMP_TICKS(&ticks_2, &ticks_1) == 0, "Test #7 Testing 7.00: Tests operation of function _time_add_sec_to_ticks");

    /* Add secs * MILLISECS_IN_SECOND to ticks_2 by _time_add_msec_to_ticks */
    _time_add_msec_to_ticks(&ticks_2, secs * MILLISECS_IN_SECOND);

    /* Calculate the difference in seconds between ticks_2 and ticks_1 by the _time_diff_seconds */
    diff_secs = _time_diff_seconds(&ticks_2, &ticks_1, &over_flow);

    /* Verify the function _time_diff_seconds return correct value */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, diff_secs == secs, "Test #7 Testing 7.01: Tests operation of function _time_diff_seconds");
    _task_set_error(MQX_OK);

    /* Convert maximum uint32_t seconds to ticks */
    PSP_SECONDS_TO_TICKS(MAX_UINT_32, &ticks_2);

    /* Add one more second to the ticks */
    _time_add_sec_to_ticks(&ticks_2, 1);

    /* Verify The function _time_diff_seconds should set over_flow to TRUE */
    _time_diff_seconds(&ticks_2, &zero_tick_struct, &over_flow);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, over_flow, "Test #7 Testing 7.02: The function _time_diff_seconds should set over_flow to TRUE");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8: Test functions _time_add_min_to_ticks, time_diff_minutes
*
* Requirement  : TIME003,TIME004,TIME030,TIME031,TIME062
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
    bool             over_flow;
    uint32_t            mins, diff_mins;
    MQX_TICK_STRUCT     ticks_1, ticks_2;

    mins = TIME_TEST;

    /* Initialize ticks_1 and ticks_2 to zero */
    _time_init_ticks(&ticks_1, 0);
    _time_init_ticks(&ticks_2, 0);

    /* Add number of mins to ticks_1 */
    _time_add_min_to_ticks(&ticks_1, mins);

    /* Add mins * SECS_IN_MINUTE of seconds to ticks_2 */
    _time_add_sec_to_ticks(&ticks_2, mins * SECS_IN_MINUTE);

    /* verify ticks_1 and ticks_2 are equal */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, PSP_CMP_TICKS(&ticks_1, &ticks_2) == 0, "Test #8 Testing 8.00: Tests operation of function _time_add_min_to_ticks");

    /* Add mins * SECS_IN_MINUTE of seconds to ticks_2 */
    _time_add_sec_to_ticks(&ticks_2, mins * SECS_IN_MINUTE);

    /* Calculate the difference in minutes between ticks_2 and ticks_1 by the _time_diff_minutes */
    diff_mins = _time_diff_minutes(&ticks_2, &ticks_1, &over_flow);

    /* Verify the function _time_diff_minutes return correct value */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, mins == diff_mins, "Test #8 Testing 8.01: Tests operation of function _time_diff_minutes");
    _task_set_error(MQX_OK);

    /* Convert maximum uint32_t minutes to ticks_2 */
    PSP_MINUTES_TO_TICKS(MAX_UINT_32, &ticks_2);

    /* Add one more minute to the ticks_2 */
    _time_add_min_to_ticks(&ticks_2, 1);

    /* Verify The function _time_diff_minutes should set over_flow to TRUE */
    _time_diff_minutes(&ticks_2, &zero_tick_struct, &over_flow);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, over_flow, "Test #8 Testing 8.02: The function _time_diff_minutes should set over_flow to TRUE");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : TEST #9: Test functions _time_add_hour_to_ticks, time_diff_hours
*
* Requirement  : TIME002,TIME003,TIME028,TIME029,TIME062
*
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
    bool             over_flow;
    uint32_t            hours, diff_hours;
    MQX_TICK_STRUCT     ticks_1, ticks_2;

    hours = TIME_TEST;

    /* Initialize ticks_1 and ticks_2 to zero */
    _time_init_ticks(&ticks_1, 0);
    _time_init_ticks(&ticks_2, 0);

    /* Add hours to ticks_1 */
    _time_add_hour_to_ticks(&ticks_1, hours);

    /* Add hours * MINUTES_IN_HOUR of minutes to ticks_2 */
    _time_add_min_to_ticks(&ticks_2, hours * MINUTES_IN_HOUR);

    /* verify ticks_1 and ticks_2 are equal */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, PSP_CMP_TICKS(&ticks_1, &ticks_2) == 0, "Test #9 Testing 9.00: Tests operation of function _time_add_hour_to_ticks");

    /* Add hours * MINUTES_IN_HOUR of minutes to ticks_2 */
    _time_add_min_to_ticks(&ticks_2, hours * MINUTES_IN_HOUR);

    /* Calculate the difference in hours between ticks_2 and ticks_1 by the _time_diff_hours */
    diff_hours = _time_diff_hours(&ticks_2, &ticks_1, &over_flow);

    /* Verify the function _time_diff_hours returns correct value */
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, hours == diff_hours, "Test #9 Testing 9.01: Tests operation of function _time_diff_hours");
    _task_set_error(MQX_OK);

    /* Convert maximum uint32_t hours to ticks_2 */
    PSP_HOURS_TO_TICKS(MAX_UINT_32, &ticks_2);

    /* Add one more hour to the ticks_2 */
    _time_add_hour_to_ticks(&ticks_2, 1);

    /* Verify The function _time_diff_hours should set over_flow to TRUE */
    _time_diff_hours(&ticks_2, &zero_tick_struct, &over_flow);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, over_flow, "Test #9 Testing 9.02: The function _time_diff_minutes should set over_flow to TRUE");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_10_main_task
* Comments     : TEST #10: Testing functions _time_add_day_to_ticks, _time_diff_days
*
* Requirement  : TIME001,TIME002,TIME026,TIME027,TIME062
*
*END*---------------------------------------------------------------------*/
void tc_10_main_task(void)
{
    bool             over_flow;
    uint32_t            days, diff_days;
    MQX_TICK_STRUCT     ticks_1, ticks_2;

    days = TIME_TEST;

    /* Initialize ticks_1 and ticks_2 to zero */
    _time_init_ticks(&ticks_1, 0);
    _time_init_ticks(&ticks_2, 0);

    /* Add number of days to ticks_1 */
    _time_add_day_to_ticks(&ticks_1, days);

    /* Add days * HOURS_IN_DAY of hours to ticks_2 */
    _time_add_hour_to_ticks(&ticks_2, days * HOURS_IN_DAY);

    /* verify ticks_1 and ticks_2 are equal */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, PSP_CMP_TICKS(&ticks_1, &ticks_2) == 0, "Test #10 Testing 10.00: Tests operation of function _time_add_day_to_ticks");

    /* Add days * HOURS_IN_DAY of hours to ticks_2 */
    _time_add_hour_to_ticks(&ticks_2, days * HOURS_IN_DAY);

    /* Calculate the difference in days between ticks_2 and ticks_1 by the _time_diff_days */
    diff_days = _time_diff_days(&ticks_2, &ticks_1, &over_flow);

    /* Verify the function _time_diff_days return correct value */
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, days == diff_days, "Test #10 Testing 10.01: Tests operation of function _time_diff_days");
    _task_set_error(MQX_OK);

    /* Convert maximum uint32_t days to ticks_2 */
    PSP_DAYS_TO_TICKS(MAX_UINT_32, &ticks_2);

    /* Add one more day to the ticks_2 */
    _time_add_day_to_ticks(&ticks_2, 1);

    /* Verify The function _time_diff_minutes should set over_flow to TRUE */
    _time_diff_days(&ticks_2, &zero_tick_struct, &over_flow);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, over_flow, "Test #10 Testing 10.02: The function _time_diff_minutes should set over_flow to TRUE");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_11_main_task
* Comments     : TEST #11: Testing functions _time_diff_ticks
*
* Requirement  : TIME024,TIME025,TIME062
*
*END*---------------------------------------------------------------------*/
void tc_11_main_task(void)
{
    _mqx_uint           result;
    MQX_TICK_STRUCT     ticks_1, ticks_2, diff_ticks;

    /* Initialize ticks_1 with DELAY_TICKS in TICKS field  */
    _time_init_ticks(&ticks_1, DELAY_TICKS);

    /* Initialize ticks_2 with 2 * DELAY_TICKS in TICKS field  */
    _time_init_ticks(&ticks_2, 2 * DELAY_TICKS);

    result = _time_diff_ticks(&ticks_2, &ticks_1, &diff_ticks);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "Test #11 Testing 11.00: Tests operation of function _time_diff_ticks");
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, PSP_GET_ELEMENT_FROM_TICK_STRUCT(&diff_ticks, 0) == DELAY_TICKS, "Test #11 Testing 11.01: Tests operation of function _time_diff_ticks");
    _task_set_error(MQX_OK);

    /* Calling _time_diff_ticks function with one of parameters is NULL to verify the function return MQX_INVALID_PARAMETER  */
    result = _time_diff_ticks(&ticks_2, NULL, &diff_ticks);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_INVALID_PARAMETER, "Test #11 Testing 11.02: Tests operation of function _time_diff_ticks");
    _task_set_error(MQX_OK);

    /* Calling _time_diff_ticks function with one of parameters is NULL to verify the function return MQX_INVALID_PARAMETER  */
    result = _time_diff_ticks(NULL, &ticks_1, &diff_ticks);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_INVALID_PARAMETER, "Test #11 Testing 11.03: Tests operation of function _time_diff_ticks");
    _task_set_error(MQX_OK);

    /* Calling _time_diff_ticks function with one of parameters is NULL to verify the function return MQX_INVALID_PARAMETER  */
    result = _time_diff_ticks(&ticks_2, &ticks_1, NULL);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_INVALID_PARAMETER, "Test #11 Testing 11.04: Tests operation of function _time_diff_ticks");
    _task_set_error(MQX_OK);
}

 /******  Define Test Suite      *******/

 EU_TEST_SUITE_BEGIN(suite_time1)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1: Test the function _time_delay_for "),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2: Test the function _time_delay_until"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST #3: Test functions _time_add_psec_to_ticks, time_diff_picoseconds"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST #4: Test functions _time_add_nsec_to_ticks, time_diff_nanoseconds"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST #5: Test functions _time_add_usec_to_ticks, time_diff_microseconds"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST #6: Test functions _time_add_msec_to_ticks, time_diff_milliseconds"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST #7: Test functions _time_add_sec_to_ticks, time_diff_seconds"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST #8: Test functions _time_add_min_to_ticks, time_diff_minutes"),
    EU_TEST_CASE_ADD(tc_9_main_task, " TEST #9: Test functions _time_add_hour_to_ticks, time_diff_hours"),
    EU_TEST_CASE_ADD(tc_10_main_task, " TEST #10: Testing functions _time_add_day_to_ticks, _time_diff_days"),
    EU_TEST_CASE_ADD(tc_11_main_task, " TEST #11: Testing functions _time_diff_ticks"),
 EU_TEST_SUITE_END(suite_time1)

/*******  Add test suites */
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_time1, " Test of time module")
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
     _time_init_ticks(&zero_tick_struct, 0);

    _int_install_unexpected_isr();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
 } /* Endbody */

 /*TASK*-------------------------------------------------------------------
 *
 * Task Name    : main_task
 * Comments     : Just read and display the time.
 *
 *END*----------------------------------------------------------------------*/
 void test_task
    (
       uint32_t param
    )
{

    test_task_run = TRUE;

    _task_block();
}
/* EOF */

