/*HEADER********************************************************************
 *
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
 * $FileName: hwtimer.c$
 * $Version : 4.0.1$
 * $Date : Jan-18-2013$
 *
 * Comments:
 *
 *   This file contains functions of the hwtimer component.
 *
 *END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "test_specific.h"

#include "EUnit.h"
#include "eunit_crit_secs.h"

#define MAIN_TASK 1

/* Our task should not be preempted.
  Therefore time_delay() function should be in the worst case delayed  by 2 ticks because of rounding*/
#define TIME_DELAY_TOLERANCE (2 * (1000/BSP_ALARM_FREQUENCY))

extern HWTIMER_DEVIF_STRUCT_PTR devif_array[];
extern int id_array[][ID_ARRAY_MAX];
extern CM_CLOCK_SOURCE clock_id_array[];

HWTIMER hwtimer;                               //hwtimer handle

void main_task(uint32_t);

//------------------------------------------------------------------------
// Test suite function prototipe
void tc_1_main_task();
void tc_2_main_task();
void tc_3_main_task();
void tc_4_main_task();
//------------------------------------------------------------------------

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK,  main_task, 2000,   8,   "Main",  MQX_AUTO_START_TASK, 0},
   { 0,          0,         0,      0,   0,       0,                   0}
};


/* Local functions */
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : callback_counter
* Returned Value   : void
* Comments         :
*   Increment value.
*
*END*----------------------------------------------------------------------*/
void callback_counter(int* counter)
{
    (*counter)++;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : callback_counter
* Returned Value   : void
* Comments         :
*   Increment value and block hwtrimer callback.
*
*END*----------------------------------------------------------------------*/
void callback_counter_block(int* counter)
{
    (*counter)++;
    hwtimer_callback_block(&hwtimer);
}

/******************** Begin Testcases ********************/
/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Init Deinit tests
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task()
{
    int i, id;
    int result;

    /* Repeat testing for each available low level timer */
    for (i = 0; DEVIF_ARRAY_TERMINATOR != devif_array[i]; i++)
    {
        /* Repeat for each ids(channels) of the timer */
        for (id = 0; ID_ARRAY_TERMINATOR != id_array[i][id]; id++)
        {
            /* Testing input parrameters in hwtimer_init_function*/
            result = hwtimer_init(NULL, devif_array[i], id_array[i][id], HWTIMER_DEFAULT_PRIORITY);
            EU_ASSERT_REF_TC_MSG( tc_1_main_task, (result != MQX_OK), "Test #1: Test 1.1 hwtimer init when hwtimer handle is NULL.");
            result = hwtimer_init(&hwtimer, NULL, id_array[i][id], HWTIMER_DEFAULT_PRIORITY);
            EU_ASSERT_REF_TC_MSG( tc_1_main_task, (result != MQX_OK), "Test #1: Test 1.2 hwtimer init when dev handle is NULL.");
            /* test probably non existing ID */
            result = hwtimer_init(&hwtimer, devif_array[i], UINT32_MAX, HWTIMER_DEFAULT_PRIORITY);
            EU_ASSERT_REF_TC_MSG( tc_1_main_task, (result != MQX_OK), "Test #1: Test 1.3 hwtimer init when id of dev is wrong");
            /* todo : deinit tests */
        }
    }
}


/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #1: Test for set and get period
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task()
{
    int i, id;
    int result, modulo;
    uint32_t clock_freq;

    /* Repeat testing for each available low level timer */
    for (i = 0; DEVIF_ARRAY_TERMINATOR != devif_array[i]; i++)
    {
        /* Repeat for each ids(channels) of the timer */
        for (id = 0; ID_ARRAY_TERMINATOR != id_array[i][id]; id++)
        {
            /* Initialization of hwtimer */
            result = hwtimer_init(&hwtimer, devif_array[i], id_array[i][id], HWTIMER_DEFAULT_PRIORITY);
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_OK, "Test #2: Test 2.1 Initialization failed, continue with next id.");
            if(result != MQX_OK){
                continue;
            }
            /* Set frequency 0 */
            result = hwtimer_set_freq(&hwtimer, clock_id_array[i], 0);     // Set frequency 0 should return error
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (MQX_OK != result), "Test #2: Test 2.2 Set frequency 0Hz. Should return error. ");

            /* Set period 0 */
            result = hwtimer_set_period(&hwtimer, clock_id_array[i], 0);   // Set period 0 should return error
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (MQX_OK != result), "Test #2: Test 2.3 Set period 0ms. Should return error.");

            /* Set frequency 10Hz */
            result = hwtimer_set_freq(&hwtimer, clock_id_array[i], 10);    // Set frequency 10 should return MQX_OK
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (MQX_OK == result), "Test #2: Test 2.4 Set frequency 10Hz.");
            result = hwtimer_get_freq(&hwtimer);                           // Get frequency should return 10Hz
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (10 == result)    , "Test #2: Test 2.5 Get frequency 10Hz.");
            result = hwtimer_get_period(&hwtimer);                         // Get period should return 1000000us
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (100000 == result), "Test #2: Test 2.6 Get period for 10Hz.");
            modulo = hwtimer_get_modulo(&hwtimer);                         // Modulo should be more than 0
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (0 != modulo)     , "Test #2: Test 2.7 Get modulo for 10Hz.");

            /* Set period 100000us */
            result = hwtimer_set_period(&hwtimer, clock_id_array[i], 100000);  // Set period 100000us should return MQX_OK
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (MQX_OK == result), "Test #2: Test 2.8 Set period 100000us.");
            result = hwtimer_get_freq(&hwtimer);                               // Get frequency should return 10Hz
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (10 == result)    , "Test #2: Test 2.9 Get frequency for 100000us.");
            result = hwtimer_get_period(&hwtimer);                             // Get period should return 100000us
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (100000 == result), "Test #2: Test 2.10 Get period for 100000us.");
            result = hwtimer_get_modulo(&hwtimer);                             // Modulo should be more than 0 and equal to modulo from set freq 1Hz
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (result == modulo), "Test #2: Test 2.11 Get modulo for 100000us.");

            /*Set frequency 33Hz*/
            result = hwtimer_set_freq(&hwtimer, clock_id_array[i], 33);    // Set frequency 33 should return MQX_OK
            /* Testing the accuracy of timer. Is discutable, but we believe that any timer should setup 33Hz with max +-1% deviation of period */
            result = hwtimer_get_period(&hwtimer);                         //result should be 30303 and our criteria is +-1%
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (30000 <= result) && (30606 >= result)    , "Test #2: Test 2.12 Get period for 33Hz.");
            result = hwtimer_get_freq(&hwtimer);                           // Get frequency should return 33Hz
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (33 == result)    , "Test #2: Test 2.13 Get frequency for 33Hz.");

            /* Set frequency equal to source clock */
            clock_freq = _bsp_get_clock(_bsp_get_clock_configuration(), clock_id_array[i]); // Find out source clock
            result = hwtimer_set_freq(&hwtimer, clock_id_array[i], clock_freq);            // Set frequency equal to source clock. Should return MQX_OK.
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (MQX_OK == result)    , "Test #2: Test 2.14 Set frequency equal to source clock.");
            result = hwtimer_get_freq(&hwtimer);                                           // Get frequency should return source_clock
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (clock_freq == result), "Test #2: Test 2.15 Get frequency equal to source clock.");
            modulo = hwtimer_get_modulo(&hwtimer);                                         // Modulo should be more than 0
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (1 == modulo)         , "Test #2: Test 2.16 Get modulo for frequency equal to source clock.");

            /* Set frequency bigger than source clock */
            result = hwtimer_set_freq(&hwtimer, clock_id_array[i], (clock_freq + 10));     // Set frequency bigger than source clock. Should return MQX_OK.
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (MQX_OK == result)    , "Test #2: Test 2.17 Set frequency bigger than source clock");
            result = hwtimer_get_freq(&hwtimer);                                           // Get frequency should return source_clock
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (clock_freq == result), "Test #2: Test 2.18 Get frequency bigger than source clock");
            result = hwtimer_get_modulo(&hwtimer);                                         // Modulo should be more than 0
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (result == modulo)    , "Test #2: Test 2.19 Get modulo for frequency bigger to source clock.");

            /* Deinitialization of hwtimer */
            hwtimer_deinit(&hwtimer);
        }
    }
}

/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Test for start and stop
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task()
{
    int i , id;
    int ticks1, ticks2, result;
    HWTIMER_TIME_STRUCT time;

    /* Repeat testing for each available low level timer */
    for (i = 0; DEVIF_ARRAY_TERMINATOR != devif_array[i]; i++)
    {
        /* Repeat for each ids(channels) of the timer */
        for (id = 0; ID_ARRAY_TERMINATOR != id_array[i][id]; id++)
        {
           /* Initialization of hwtimer */
            result = hwtimer_init(&hwtimer, devif_array[i], id_array[i][id], HWTIMER_DEFAULT_PRIORITY);
            EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_OK, "Test #3: Test 3.1 Initialization failed, continue with next id.");
            if (result != MQX_OK){
                continue; // Try next id.
            }

            /* Set period for timer 1mS */
            result = hwtimer_set_period(&hwtimer, clock_id_array[i], 1000);
            EU_ASSERT_REF_TC_MSG( tc_3_main_task, result == MQX_OK, "Test #3: Test 3.2 Set period 1mS failed");
            if (result != MQX_OK){
                continue; // Try next id.
            }
            /** Test: Hwtimer start and stop **/
            /* Start hwtimer, wait 200 mS, Stop timer */
            hwtimer_start(&hwtimer);
            _time_delay(200);
            hwtimer_stop(&hwtimer);

            /* Read ticks and time compare to 200ms */
            ticks1 = hwtimer_get_ticks(&hwtimer);
            EU_ASSERT_REF_TC_MSG( tc_3_main_task, (ticks1 >= 200) && (ticks1 <= 200 + TIME_DELAY_TOLERANCE), "Test #3: Test 3.4 hwtimer_get_ticks after delay 200ms");

            hwtimer_get_time(&hwtimer, &time);
            EU_ASSERT_REF_TC_MSG( tc_3_main_task, (time.TICKS == (uint64_t)ticks1), "Test #3: Test 3.5 hwtimer_get_time after delay 200ms");
            /* Delay 10ms to enshure that hwtimer is stoped*/
            _time_delay(10);

            ticks2 = hwtimer_get_ticks(&hwtimer);
            EU_ASSERT_REF_TC_MSG( tc_3_main_task, (ticks2 == ticks1), "Test #3: Test 3.6 hwtimer_get_ticks when hwtimer is stoped");

            /** Test: Restart of hwtimer and stop **/
            /* Start hwtimer, wait 10ms, Stop timer */
            hwtimer_start(&hwtimer);
            _time_delay(10);
            hwtimer_stop(&hwtimer);
            /* Read ticks and compare to 5ms */
            ticks2 = hwtimer_get_ticks(&hwtimer);
            EU_ASSERT_REF_TC_MSG( tc_3_main_task, (ticks2 >= (ticks1 + 10)) && (ticks2 <= (ticks1 + 10 + TIME_DELAY_TOLERANCE)), "Test #3: Test 3.7 hwtimer_get_ticks after restarting of  the hwtimer");

            hwtimer_get_time(&hwtimer, &time);
            EU_ASSERT_REF_TC_MSG( tc_3_main_task, (time.TICKS == ticks2), "Test #3: Test 3.8 hwtimer_get_time after restarting of the hwtimer");

            /** Test: Start hwtimer and then deinit **/
             /* Start hwtimer, wait 10 mS, deinit timer */
            hwtimer_start(&hwtimer);
            /* Deinitialization of hwtimer */
            hwtimer_deinit(&hwtimer);
            _time_delay(10);
            ticks2 = hwtimer_get_ticks(&hwtimer); //ticks should be 0 and hwtimer should be stoped.
            EU_ASSERT_REF_TC_MSG( tc_3_main_task, (ticks2 == 0), "Test #3: Test 3.9 hwtimer_get_time after deinitialization");
        }
    }
}

/*TASK*---------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #3: Test Callback functionality
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task()
{
    int i, id;
    int ticks, tmp_ticks, tmp_counter, result;
    static int counter;

    /* Repeat testing for each available low level timer */
    for (i = 0; DEVIF_ARRAY_TERMINATOR != devif_array[i]; i++)
    {
        /* Repeat for each ids(channels) of the timer */
        for (id = 0; ID_ARRAY_TERMINATOR != id_array[i][id]; id++)
        {
           /* Initialization of hwtimer */
            result = hwtimer_init(&hwtimer, devif_array[i], id_array[i][id], HWTIMER_DEFAULT_PRIORITY);
            EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_OK, "Test #4: Test 4.1 Initialization failed, continue with next id.");
            if (result != MQX_OK){
                continue; // Try next id.
            }

            /* Set period for timer 1mS */
            result = hwtimer_set_period(&hwtimer, clock_id_array[i], 1000);
            EU_ASSERT_REF_TC_MSG( tc_4_main_task, result == MQX_OK, "Test #4: Test 4.2 Set period 1mS failed");
            if (result != MQX_OK){
                continue; // Try next id.
            }

            /** Test: Registration of callback **/
            counter = 0;
            /* Register callback */
            hwtimer_callback_reg(&hwtimer, (HWTIMER_CALLBACK_FPTR)callback_counter, &counter);

            /* Start hwtimer for 10ms*/
            hwtimer_start(&hwtimer);
            _time_delay(10);
            hwtimer_stop(&hwtimer);

            /* Read ticks and compere them to counter */
            ticks = hwtimer_get_ticks(&hwtimer); // Count of ticks should be the same as counter incremented in callback
            EU_ASSERT_REF_TC_MSG( tc_4_main_task, (ticks == counter), "Test #4: Test 4.3 Test of counter incremented in callback");
            tmp_counter = counter;

            /** Test: Block callback**/
            /* Block callback */
            hwtimer_callback_block(&hwtimer);
            /* Start hwtimer for 10ms */
            /* This should set pending interrupt because callback is blocked */
            hwtimer_start(&hwtimer);
            _time_delay(10);
            /* Unblock hwtimer */
            hwtimer_callback_unblock(&hwtimer);
            hwtimer_stop(&hwtimer);

            /* If callback was pending  value of counter should be incremented by 1 */
            EU_ASSERT_REF_TC_MSG( tc_4_main_task, ((tmp_counter + 1) == counter), "Test #4: Test 4.4 Pending callback");

            /** Test: Callback unblocked, restart timer**/
            counter = 0x00;
            tmp_ticks = hwtimer_get_ticks(&hwtimer);

            /* Start hwtimer for 10ms */
            hwtimer_start(&hwtimer);
            _time_delay(10);
            hwtimer_stop(&hwtimer);

             /* Read ticks and time compare to counter */
            ticks = hwtimer_get_ticks(&hwtimer) - tmp_ticks;
            EU_ASSERT_REF_TC_MSG( tc_4_main_task, (ticks == counter), "Test #4: Test 4.5 Unblocking callback ");

            /** Test: Change callback. callback will increment counter and block callback **/
            counter = 0;
            hwtimer_callback_reg(&hwtimer, (HWTIMER_CALLBACK_FPTR)callback_counter_block, &counter);
            hwtimer_start(&hwtimer);
            _time_delay(10);
            hwtimer_stop(&hwtimer);
            EU_ASSERT_REF_TC_MSG( tc_4_main_task, (counter == 1), "Test #4: Test 4.6 Changing callback with callback block inside");
            /* Deinitialization of hwtimer */
            hwtimer_deinit(&hwtimer);
            //todo: add testcase for changing callback function when hwtimer is running
        }
    }
}

/******************** End Testcases ********************/

//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_timer)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1: "),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2: "),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3: "),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4: ")
 EU_TEST_SUITE_END(suite_timer)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_timer, " - Test of hwtimer Component")
 EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*-------------------------------------------------------------------
*
* Task Name : main_task
* Comments  :
*
*END*-------------------------------------------------------------------*/

void main_task( uint32_t param)
{
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
}
