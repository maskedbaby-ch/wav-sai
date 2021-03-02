/**HEADER********************************************************************
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
* $FileName: test.c$
* $Version : 4.0.1.1$
* $Date    : May-04-2013$
*
* Comments:
*
*   This file contains the source for the (lw)mem stress testing.
* Requirements :
*   LWMEM011, LWMEM019, LWMEM020, LWMEM028.
*   MEM005, MEM046, MEM053, MEM078.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <lwevent.h>
#include <lwevent_prv.h>
#include <bsp.h>
#include "util.h"
#include "mem_util.h"
#include "test.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

void tc_1_main_task(void); /* Test #1 - Testing (lw)mem stress */

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
#if PSP_MQX_CPU_IS_KINETIS_L
{ MAIN_TASK, main_task,  1000,   MAIN_TASK, "Main_Task", MQX_AUTO_START_TASK},
#else
{ MAIN_TASK, main_task,  3000,   MAIN_TASK, "Main_Task", MQX_AUTO_START_TASK},
#endif
{ TEST_TASK, test_task,  1000,   TEST_TASK, "Test_Task1", 0                  },
{ 0,         0,          0,      0, 0,           0                  }
};

/*TASK*-------------------------------------------------------------------
*
* Task Name    : test_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void test_task
   (
      uint32_t task_num
   )
{
   _mqx_uint i = (_mqx_uint)task_num;
   _mqx_uint result;

   _time_delay_ticks(2);

   _task_set_priority(MQX_NULL_TASK_ID, TEST_TASK+i, &result);
   while (TRUE) {
      if (mem_ptrs[i]) {
         result = _mem_free(mem_ptrs[i]);
         free_count[i]++;
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: _(lw)lwmem_free operation");
         if (result != MQX_OK) {
            _int_disable();
            eunit_test_stop();
            _mqx_exit(1);
         } /* Endif */
         mem_ptrs[i] = 0;
      } else {
         mem_ptrs[i] = _mem_alloc_from(ext_pool_id, my_rand() % MAX_ALLOC);
         alloc_count[i]++;
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mem_ptrs[i] != NULL), "Test #1 Testing: _(lw)lwmem_alloc_from operation");
         if (mem_ptrs[i] == NULL) {
            _int_disable();
            eunit_test_stop();
            _mqx_exit(1);
         } /* Endif */
      } /* Endif */
      /* signal the main task that will perform memory check */
      _lwevent_set(&mem_test_event, 1 << i);
      _time_delay_ticks(my_rand() % 5);
   } /* Endwhile */
}

/******************** Begin Test cases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Testing (lw)mem stress
* Requirements :
*   LWMEM011, LWMEM019, LWMEM020, LWMEM028.
*   MEM005, MEM046, MEM053, MEM078.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint      i;
    _mqx_uint      result;
    _task_id       tid[NUM_TASKS];

    _mem_zero(mem_ptrs, sizeof(mem_ptrs));

    pool_alloc_ptr = _mem_alloc(POOL_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, pool_alloc_ptr != NULL, "Test #1: Testing 1.1: memory allocate operation");
    if (pool_alloc_ptr == NULL) {
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    ext_pool_id = GENERAL_MEM_CREATE_POOL(pool_alloc_ptr, POOL_SIZE);

    my_srand(0xAB0F);

    result = _lwevent_create(&mem_test_event, LWEVENT_AUTO_CLEAR);
    if (result != MQX_OK){
        eunit_test_stop();
        _mqx_exit(1);
    }

    for (i = 0; i < NUM_TASKS; i++ ) {
        tid[i] = _task_create(0, TEST_TASK, i);
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, tid[i] != MQX_NULL_TASK_ID, "Test #1: Testing 1.2: Create task operation");
        if (tid[i] == MQX_NULL_TASK_ID) {
            _int_disable();
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */
    } /* Endfor */

    for (i = 0; i < CYCLES_CNT; i++) {
        /* wait for all test tasks complete one round */
        _lwevent_wait_ticks(&mem_test_event, (1 << NUM_TASKS) - 1, TRUE, TEST_TIMEOUT_TICKS);

        GENERAL_MEM_TEST_ALL(result, pool_error, block_error);

        EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.3: mem test operation");
        if (result != MQX_OK ) {
            _int_disable();
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */
    } /* Endwhile */

    for (i = 0; i < NUM_TASKS; i++ ) {
        _task_destroy(tid[i]);
    } /* Endfor */
}

/******************** End Testcases ********************/

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_stress)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Testing lwmem stress")
EU_TEST_SUITE_END(suite_stress)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_stress, " - Stress test of light weight memory component")
EU_TEST_REGISTRY_END()
/*------------------------------------------------------------------------*/


/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   This is the main task to the MQX Kernel program, this task creates
*   all defined test tasks and lets them run continuously.
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t dummy
   )
{
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
} /* Endbody */

/* EOF */
