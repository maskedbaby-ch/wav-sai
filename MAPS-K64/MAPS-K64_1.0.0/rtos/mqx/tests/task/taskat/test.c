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
* Comments:
*
*   This file contains the kernel test functions for creating a task
*   AT a specified address (using the specified memory for the task stack
*   and TD structure).
*
* Requirement: TASK014, TASK015, TASK016, TASK017, TASK019
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <mqx_prv.h>
#include <watchdog.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"


#undef  _NELEM
#define _NELEM(ary)     (sizeof(ary)/sizeof(ary[0]))

#define FILE_BASENAME test

#define PRINT_ON    0

/* #define KLOG_ON */

#define MAIN_TASK  10
#define TEST_TASK1 11
#define TEST_TASK2 12
#define TEST_TASK3 13

extern void main_task(uint32_t);
extern void test_task(uint32_t);
extern void test_task3(uint32_t);

void tc_1_main_task(void);/* TEST #1 - Check interrupt stack */
void tc_2_main_task(void);/* TEST #2 - Test function _task_create_at in special cases */

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   /*  Task number, Entry point, Stack, Pri, String, Auto? */
   { TEST_TASK2, test_task,  0,     7, "test",  0,  0}, // low priority wont run
   { MAIN_TASK,  main_task,  4000,  8, "Main",  MQX_AUTO_START_TASK,  0},
   { TEST_TASK1, test_task,  0,     9, "test",  0,  0}, // high priority preempt
   { TEST_TASK3, test_task3, 1500,  9, "test3", 0,  0},
   { 0,          0,          0,     0, 0,       0,                    0}
};

struct {
   // Should be ABI aligned ... cannot do so portably....
   double force_double_alignment;
   uint32_t precheck[4];
#if defined(PSP_PPC750)
   unsigned char int_stack[4*1024];
#elif 1
   /*
   ** Some BSPs define interrupt stack size in linker command file
   ** so the value of BSP_DEFAULT_INTERRUPT_STACK_SIZE is dynamic.
   */
   unsigned char int_stack[1024];
#else
   unsigned char int_stack[BSP_DEFAULT_INTERRUPT_STACK_SIZE];
#endif
   uint32_t postcheck[4];
} myistack = {
   0.0,
   {0x12345678,0x12345678,0x12345678,0x12345678},
   {0},
   {0x87654321,0x87654321,0x87654321,0x87654321}
};

#define TEST_STACK_SIZE 2000
static struct {
   // Should be ABI aligned ... cannot do so portably....
   double force_double_alignment;
   uint32_t precheck[4];
   unsigned char test_task_stack[TEST_STACK_SIZE];
   uint32_t postcheck[4];
} mymem = {
   0.0,
   {0x12345678,0x12345678,0x12345678,0x12345678},
   {0},
   {0x87654321,0x87654321,0x87654321,0x87654321}
};

MQX_INITIALIZATION_STRUCT  MQX_init_struct =
{
   /* PROCESSOR_NUMBER                */  BSP_DEFAULT_PROCESSOR_NUMBER,
   /* START_OF_KERNEL_MEMORY          */  BSP_DEFAULT_START_OF_KERNEL_MEMORY,
   /* END_OF_KERNEL_MEMORY            */  BSP_DEFAULT_END_OF_KERNEL_MEMORY,
   /* INTERRUPT_STACK_SIZE            */  sizeof(myistack.int_stack),//BSP_DEFAULT_INTERRUPT_STACK_SIZE,
   /* TASK_TEMPLATE_LIST              */  (void *)MQX_template_list,
   /* MQX_HARDWARE_INTERRUPT_LEVEL_MAX*/  BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX,
   /* MAX_MSGPOOLS                    */  BSP_DEFAULT_MAX_MSGPOOLS,
   /* MAX_MSGQS                       */  BSP_DEFAULT_MAX_MSGQS,
   /* IO_CHANNEL                      */  BSP_DEFAULT_IO_CHANNEL,
#if MQX_USE_IO_OLD
   /* IO_OPEN_MODE                    */  BSP_DEFAULT_IO_OPEN_MODE,
#else
   /* IO_OPEN_MODE                    */  0,
#endif
   /* INTERRUPT_STACK_PTR             */  &myistack.int_stack
};

volatile uint32_t test_task_val = 0;
bool          is_task_running;
unsigned char    test_task_stack[TEST_STACK_SIZE];

#define test_istack_trashing() test_istack_trashing_(__LINE__)
#define test_memory_trashing() test_memory_trashing_(__LINE__)


static void test_istack_trashing_
   (
      int line_number
   )
{/* Body */
    int i;
    for (i = 0; i < _NELEM(myistack.precheck); i++) {
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, (myistack.precheck[i] == 0x12345678), "Test #1 Testing: myistack precheck should have all member values of 0x12345678")
        if (myistack.precheck[i] != 0x12345678) {
#if PRINT_ON
            printf("Line %d, precheck[%d] trashed\n", line_number, i);
#endif
        } /* Endif */
    } /* Endfor */
    for (i = 0; i < _NELEM(myistack.postcheck); i++) {
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, (myistack.postcheck[i] == 0x87654321), "Test #1 Testing: myistack postcheck should have all member values of 0x87654321")
        if (myistack.postcheck[i] != 0x87654321) {
#if PRINT_ON
            printf("Line %d, postcheck[%d] trashed\n", line_number, i);
#endif
        } /* Endif */
    } /* Endfor */
} /* Endbody */

static void test_memory_trashing_
   (
      int line_number
   )
{/* Body */
    int i;
    for (i = 0; i < _NELEM(mymem.precheck); i++) {
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mymem.precheck[i] == 0x12345678), "Test #1 Testing: mymem precheck should have all member values of 0x12345678")
        if (mymem.precheck[i] != 0x12345678) {
#if PRINT_ON
            printf("Line %d, precheck[%d] trashed\n", line_number, i);
#endif
        } /* Endif */
    } /* Endfor */
    for (i = 0; i < _NELEM(mymem.postcheck); i++) {
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mymem.postcheck[i] == 0x87654321), "Test #1 Testing: mymem precheck should have all member values of 0x87654321")
        if (mymem.postcheck[i] != 0x87654321) {
#if PRINT_ON
            printf("Line %d, postcheck[%d] trashed\n", line_number, i);
#endif
        } /* Endif */
    } /* Endfor */
} /* Endbody */

/******   Test cases body definition      *******/

 /*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Check interrupt stack
* Requirement  : TASK014, TASK016, TASK019
*END*---------------------------------------------------------------------*/
 void tc_1_main_task(void)
 {

   _task_id tid;
   _mqx_uint result;

   test_istack_trashing();

   // printf("Test1: check task create at\n");
   tid = _task_create_at(0, TEST_TASK1, 0, mymem.test_task_stack, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, tid == MQX_NULL_TASK_ID, "Test #1: Testing 1.1: Check task create at should be fail with 0 stack size")

   test_istack_trashing();
   test_memory_trashing();

   tid = _task_create_at(0, TEST_TASK1, 0, mymem.test_task_stack, TEST_STACK_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, tid != MQX_NULL_TASK_ID, "Test #1: Testing 1.2: Check task create at")

   test_istack_trashing();
   test_memory_trashing();

   result = _task_destroy(tid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.3: Task destroy operation")

   tid = _task_create_at(0, TEST_TASK2, 0, mymem.test_task_stack, TEST_STACK_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, tid != MQX_NULL_TASK_ID, "Test #1: Testing 1.4: Task create at operation")

   test_istack_trashing();
   test_memory_trashing();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, test_task_val == 1, "Test #1: Testing 1.5: Verify task2 ran")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Test function _task_create_at in special cases
* Requirement  : TASK015, TASK017
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _task_id                    test_tid;
    _mqx_uint                   invalid_size;
    MEMORY_ALLOC_INFO_STRUCT    memory_alloc_info;

    invalid_size = (PSP_MINSTACKSIZE + sizeof(TD_STRUCT)) - 1 ;
    /* Allocate all free memory */
    memory_alloc_all(&memory_alloc_info);
    _task_set_error(MQX_OK);

    /* Create a task when memory is insufficient */
    test_tid = _task_create_at(0, TEST_TASK3, 0, NULL,TEST_STACK_SIZE  );
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, test_tid == MQX_NULL_TASK_ID , "Test #2: Testing 2.00: Create a task with MQX memory error should have made error MQX_OUT_OF_MEMORY ");
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, _task_errno == MQX_OUT_OF_MEMORY , "Test #2: Testing 2.01: The task's error code must be MQX_OUT_OF_MEMORY ");
    _task_set_error(MQX_OK);
    /* free memory temporary allocate */
    memory_free_all(&memory_alloc_info);

    /* Call function _task_create_at when stack_size parameter is not valid */
    test_tid = _task_create_at(0, TEST_TASK3, 0, test_task_stack,invalid_size );
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, test_tid == MQX_NULL_TASK_ID , "Test #2: Testing 2.02: Create a task with _task_create_at function when stack_size parameter invalid ");
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, _task_errno == MQX_INVALID_SIZE , "Test #2: Testing 2.03: The task's error code must be MQX_INVALID_SIZE ");
    _task_set_error(MQX_OK);

    /* Call function _task_create_at when processor number is not valid */
    test_tid = _task_create_at(-1, TEST_TASK3, 0, test_task_stack, TEST_STACK_SIZE );
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, test_tid == MQX_NULL_TASK_ID , "Test #2: Testing 2.04: Create a task with invalid processor number");
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, _task_errno == MQX_INVALID_PROCESSOR_NUMBER , "Test #2: Testing 2.05: The task's error code must be MQX_INVALID_PROCESSOR_NUMBER ");
    _task_set_error(MQX_OK);
}
 /*END-----------------------------------*/

/*TASK*-------------------------------------------------------------------
*
* Task Name    : test_task
* Comments     :
*   created task
*
*END*----------------------------------------------------------------------*/

void test_task
   (
      uint32_t param
   )
{/* Body */
   test_task_val++;
   test_memory_trashing();
}/* Endbody */

void test_task3
   (
      uint32_t param
   )
{
    is_task_running = TRUE;
}
//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_taskat)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Check interrupt stack and task create at"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Test function _task_create_at in special cases ")
 EU_TEST_SUITE_END(suite_taskat)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_taskat, " - Test taskat suite")
 EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   starts up the watchdog and tests it.
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t param
   )
{/* Body */
    _int_install_unexpected_isr();

   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* Endbody */

/* EOF */
