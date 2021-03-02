/**HEADER********************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
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
* $Date    : Apr-08-2013$
*
* Comments   : This file contains the source for the task module testing.
* Requirement: TASK015,TASK017
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
#define TEST_TEMP        3

#define MAIN_PRIO        7
#define TEST_PRIO        8

#define VALID_PARAMETER  0xCAFE
#define TEST_STACK_SIZE  2000

void    main_task(uint32_t);
void    test_task(uint32_t);
/*------------------------------------------------------------------------*/
/* Test suite function prototype*/
void tc_1_main_task(void);/* TEST #1 - Test function _task_create_at in special cases */
/*------------------------------------------------------------------------*/
TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   main_task,   1500,  MAIN_PRIO,   "main", MQX_AUTO_START_TASK},
   {TEST_TASK,   test_task,   1500,  TEST_PRIO,   "test", 0},
   {0,           0,           0,     0,            0,     0,                 }
};

bool     is_task_running;
unsigned char       test_task_stack[TEST_STACK_SIZE];

/*FUNCTION*-----------------------------------------------------
*
* Function Name : test_task
* Comments      : Sets global variable is_task_running to TRUE
*
*END*-----------------------------------------------------*/
void test_task(uint32_t initial_data)
{
    is_task_running = TRUE;
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test function _task_create_at in special cases
* Requirement  : TASK015,TASK017
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _task_id                    test_tid;
    _mqx_uint                   invalid_size;
    MEMORY_ALLOC_INFO_STRUCT    memory_alloc_info;

    invalid_size = (PSP_MINSTACKSIZE + sizeof(TD_STRUCT)) - 1 ;
    /* Allocate all free memory */
    memory_alloc_all(&memory_alloc_info);
    _task_set_error(MQX_OK);

    /* Create a task when memory is insufficient */
    test_tid = _task_create_at(0, TEST_TASK, 0, NULL,TEST_STACK_SIZE  );
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, test_tid == MQX_NULL_TASK_ID , "Test #1: Testing 1.00: Create a task with MQX memory error should have made error MQX_OUT_OF_MEMORY ");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == MQX_OUT_OF_MEMORY , "Test #1: Testing 1.01: The task's error code must be MQX_OUT_OF_MEMORY ");
    _task_set_error(MQX_OK);
    /* free memory temporary allocate */
    memory_free_all(&memory_alloc_info);

    /* Call function _task_create_at when stack_size parameter is not valid */
    test_tid = _task_create_at(0, TEST_TASK, 0, test_task_stack,invalid_size );
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, test_tid == MQX_NULL_TASK_ID , "Test #1: Testing 1.02: Create a task with _task_create_at function when stack_size parameter invalid ");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == MQX_INVALID_SIZE , "Test #1: Testing 1.03: The task's error code must be MQX_INVALID_SIZE ");
    _task_set_error(MQX_OK);

    /* Call function _task_create_at when processor number is not valid */
    test_tid = _task_create_at(-1, TEST_TASK, 0, test_task_stack, TEST_STACK_SIZE );
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, test_tid == MQX_NULL_TASK_ID , "Test #1: Testing 1.04: Create a task with invalid processor number");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, _task_errno == MQX_INVALID_PROCESSOR_NUMBER , "Test #1: Testing 1.05: The task's error code must be MQX_INVALID_PROCESSOR_NUMBER ");
    _task_set_error(MQX_OK);
}

/******************** End Testcases ********************/

/*------------------------------------------------------------------------*/
/* Define Test Suite*/
    EU_TEST_SUITE_BEGIN(task_task)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Test function _task_create_at in special cases "),
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
