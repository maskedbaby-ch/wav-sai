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
* $Version : 4.0.1.1$
* $Date    : Mar-27-2013$
*
* Comments    : This file contains the source for the task module testing.
* Requirement : TASK002,TASK006
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <mqx_prv.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define MAIN_TASK        1
#define TEST_TASK        2

#define MAIN_PRIO        7
#define TEST_PRIO        8
#define VALID_PARAMETER  0xCAFE

void    main_task(uint32_t);
void    test_task(uint32_t);
/*------------------------------------------------------------------------*/
/* Test suite function prototype*/
void tc_1_main_task(void);/* TEST #1 - Testing a task aborts itself */
/*------------------------------------------------------------------------*/
TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   main_task,   1500,  MAIN_PRIO,   "main", MQX_AUTO_START_TASK},
   {TEST_TASK,   test_task,   1500,  TEST_PRIO,   "test", 0},
   {0,           0,           0,     0,            0,     0,}
};

bool     is_task_running;
bool     is_task_aborted;

/*FUNCTION*-----------------------------------------------------
*
* Function Name : test_task
* Comments      : Sets global variable is_task_running to verify
*                 TEST_TASK is run
*END*-----------------------------------------------------*/
void test_task(uint32_t initial_data)
{
    is_task_running = TRUE;    
    _task_abort(MQX_NULL_TASK_ID);    /* Test _task_abort function with argument is MQX_NULL_TASK_ID*/
    is_task_aborted = FALSE;          /* If come here, the above task_abort was failed   */
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:  Testing a task aborts itself
* Requirement  : TASK002,TASK006
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _task_id      test_tid;
    _mqx_uint     result;

    test_tid = _task_create(0, TEST_TASK, VALID_PARAMETER);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, test_tid != MQX_NULL_TASK_ID , "Test #1: Testing 1.00: Create a task in normal operation will be successful ");

    is_task_running = FALSE;
    is_task_aborted = TRUE;
    /* suspend main task and allow TEST_TASK run */
    _time_delay_ticks(10);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (is_task_running)&&(is_task_aborted) , "Test #1: Testing 1.01: Aborting task that is calling function");
    /* [_task_abort] Try to abort a task when it was aborted */
    result = _task_abort(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result != MQX_OK , "Test #1: Testing 1.02: Tests function _task_abort with the task was aborted ");

}
/******************** End Testcases ********************/

/*------------------------------------------------------------------------*/
/* Define Test Suite*/
    EU_TEST_SUITE_BEGIN(task_task)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Testing a task aborts itself "),
    EU_TEST_SUITE_END(task_task)
/* Add test suites*/
    EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(task_task, " - Test task")
    EU_TEST_REGISTRY_END()
/*------------------------------------------------------------------------*/

/*TASK*--------------------------------------------------------------
*
* Task Name : main_task
* Comments  :
*
*END*--------------------------------------------------------------*/
void main_task
   (
      uint32_t dummy
   )
{
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
}/* Endbody */
/* EOF */
