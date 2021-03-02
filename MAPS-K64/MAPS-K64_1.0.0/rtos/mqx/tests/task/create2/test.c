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
* $Date    : APR-08-2013$
*
* Comments   : This file contains the source for the task module testing.
* Requirement: 
* TASK006,TASK007,TASK008,TASK009,TASK019,TASK025,TASK010,TASK011,
* TASK012,TASK013,TASK059,TASK066
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <mqx_prv.h>
#include <timer.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define MAIN_TASK        1
#define TEST_TASK        2
#define TEST_TASK2       3
#define TEST_TEMP        10

#define MAIN_PRIO        7
#define TEST_PRIO        8

#define VALID_PARAMETER            0xCAFE
#define INVALID_PROCESSOR_NUMBER   (-1)

void    main_task(uint32_t);
void    test_task(uint32_t);
void    test_task2(uint32_t);

/*------------------------------------------------------------------------*/
/* Test suite function prototype*/
void tc_1_main_task(void);/* TEST #1 - Test _task_create in special cases */
void tc_2_main_task(void);/* TEST #2 - Test _task_create_block in special cases*/
/*------------------------------------------------------------------------*/
TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   main_task,   2500,  MAIN_PRIO,   "main", MQX_AUTO_START_TASK},
   {TEST_TASK,   test_task,   1500,  TEST_PRIO,   "test", 0},
   {TEST_TASK2,  test_task2,  1000,  TEST_PRIO,   "test2", 0},
   {0,           0,           0,     0,   0,      0,                 }
};

bool      is_task_running;
bool      alive  = FALSE;
_task_id     creator_id;

/*TASK*-----------------------------------------------------
*
* Task Name    : test_task
* Comments     : Gets the task ID of the task that created the TEST_TASK
*
*END*-----------------------------------------------------*/

void test_task(uint32_t initial_data)
{
    creator_id = _task_get_creator();  /*testing _task_get_creator function*/
    is_task_running = TRUE;

    _task_block();
}

/*TASK*-----------------------------------------------------
*
* Task Name    : test_task2
* Comments     :
*END*-----------------------------------------------------*/
void test_task2(uint32_t freq)
{
    alive = FALSE;
}
/******************** Begin Testcases ********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test function _task_create in special cases
* Requirement  : TASK006,TASK007,TASK008,TASK009,TASK019,TASK025
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint                   result;
    _task_id                    test_tid;
    MEMORY_ALLOC_INFO_STRUCT    memory_alloc_info;

    /* the all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);
    _task_set_error(MQX_OK);

    /*Create a task when the MQX cannot allocate memory for the task data structures*/
    test_tid = _task_create(0, TEST_TASK, 0);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, test_tid == MQX_NULL_TASK_ID , "Test #1: Testing 1.00: Create a task with MQX memory error should have returned MQX_OUT_OF_MEMORY ");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == MQX_OUT_OF_MEMORY , "Test #1: Testing 1.01: The task's error code must be MQX_OUT_OF_MEMORY ");
    _task_set_error(MQX_OK);

    memory_free_all(&memory_alloc_info);

    /*the task index of TEST_TEMP task is not in the template list*/
    test_tid = _task_create(0, TEST_TEMP, 0);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, test_tid == MQX_NULL_TASK_ID , "Test #1: Testing 1.02: Creates the TEST_TEMP task");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == MQX_NO_TASK_TEMPLATE , "Test #1: Testing 1.03: The task's error code must be MQX_NO_TASK_TEMPLATE ");
    _task_set_error(MQX_OK);

    /* Test _task_create function with processor number is not invalid*/
    test_tid = _task_create(INVALID_PROCESSOR_NUMBER, TEST_TASK, 0);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, test_tid == MQX_NULL_TASK_ID , "Test #1: Testing 1.04: Create a task with invalid processor number");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == MQX_INVALID_PROCESSOR_NUMBER , "Test #1: Testing 1.05: The task's error code must be MQX_INVALID_PROCESSOR_NUMBER ");
    _task_set_error(MQX_OK);

    /*create the TEST_TASK task*/
    test_tid = _task_create(0, TEST_TASK, VALID_PARAMETER);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, test_tid != MQX_NULL_TASK_ID , "Test #1: Testing 1.06: Create a task in normal operation will be successful ");

    is_task_running = FALSE;
    /* Suspend the main task and allow TEST_TASK run */
    _time_delay_ticks(10);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, (is_task_running)&&(_task_get_id() == creator_id) , "Test #1: Testing 1.07: Testing _task_get_creator function");

    result = _task_destroy(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, result == MQX_OK , "Test #1: Testing 1.08: Destroy TEST_TASK that is created by main task ");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Test function _task_create_block in special cases
* Requiment    : TASK010,TASK011,TASK012,TASK013,TASK019,TASK059,TASK066
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _mqx_uint                   result;
    _task_id                    test_tid;
    KERNEL_DATA_STRUCT_PTR      kernel_data;
    MEMORY_ALLOC_INFO_STRUCT    memory_alloc_info;

    _GET_KERNEL_DATA(kernel_data);
    memory_alloc_all(&memory_alloc_info);
    _task_set_error(MQX_OK);

    /* Test _task_create_blocked with MQX_OUT_OF_MEMORY */
    test_tid = _task_create_blocked(0, TEST_TASK, 0);

    EU_ASSERT_REF_TC_MSG( tc_2_main_task, test_tid == MQX_NULL_TASK_ID , "Test #2: Testing 2.00: Create a task with MQX memory error should have made error MQX_OUT_OF_MEMORY");
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, _task_errno == MQX_OUT_OF_MEMORY , "Test #2: Testing 2.01: The task's error code must be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);
    memory_free_all(&memory_alloc_info);

    /*Create TEST_TEMP task with task template is not in the task template list*/
    test_tid = _task_create_blocked(0, TEST_TEMP, 0);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, test_tid == MQX_NULL_TASK_ID , "Test #2: Testing 2.02: Create a task with task template is not in the task template list ");
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, _task_errno == MQX_NO_TASK_TEMPLATE , "Test #2: Testing 2.03: The task's error code must be MQX_NO_TASK_TEMPLATE ");
    _task_set_error(MQX_OK);

    test_tid = _task_create_blocked(INVALID_PROCESSOR_NUMBER, TEST_TASK, 0);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, test_tid == MQX_NULL_TASK_ID , "Test #2: Testing 2.04: Create a task with invalid processor number ");
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, _task_errno == MQX_INVALID_PROCESSOR_NUMBER , "Test #2: Testing 2.05: The task's error code must be MQX_INVALID_PROCESSOR_NUMBER ");
    _task_set_error(MQX_OK);

    test_tid = _task_create_blocked(0, TEST_TASK, 0);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, test_tid != MQX_NULL_TASK_ID , "Test #2: Testing 2.06: Test _task_create_blocked in normal operation");

    /* Gets processor number of TEST_TASK */
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, _task_get_processor(test_tid) == kernel_data->INIT.PROCESSOR_NUMBER, "Test #2: Testing 2.07: Tests function _task_get_processor function ");

    is_task_running = FALSE;
    /* suspend main task but TEST_TASK does not run because it was blocked */
    _time_delay_ticks(10);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, (!is_task_running), "Test #2: Testing 2.08: Tests TEST_TASK was created but it was blocked ");

    _task_ready(_task_get_td(test_tid));
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, _task_errno == MQX_OK , "Test #2: Testing 2.09: Tests function _task_ready ");

    is_task_running = FALSE;
    /* suspend main task and allow TEST_TAST run */
    _time_delay_ticks(10);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, (is_task_running), "Test #2: Testing 2.10: Testing TEST_TASK was created and was made ready ");

    result = _task_destroy(test_tid);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, result == MQX_OK , "Test #2: Testing 2.11: Destroys TEST_TASK that was create by main task ");

}

/******************** End Testcases ********************/

/*------------------------------------------------------------------------*/
/* Define Test Suite*/
    EU_TEST_SUITE_BEGIN(task_create2)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Test _task_create in special cases"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Test _task_create_block in special cases"),
    EU_TEST_SUITE_END(task_create2)
/* Add test suites*/
    EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(task_create2, " - Test task")
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
