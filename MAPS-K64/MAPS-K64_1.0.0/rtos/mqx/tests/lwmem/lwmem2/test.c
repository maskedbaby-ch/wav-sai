/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $Version : 3.8.5.1$
* $Date    : Mar-30-2012$
*
* Comments:
*
*   This file contains the source for the lwmem stress testing.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <lwmem.h>
#include <lwmem_prv.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"



#define FILE_BASENAME   test
#define PRINT_ON        0

/* General defines */
#define TEST_TASK  10

#if PSP_MQX_CPU_IS_KINETIS_L
#define NUM_TASKS  4
#define POOL_SIZE  ((NUM_TASKS+1) * 512)
#define MAX_ALLOC  100
#define CYCLES_CNT 1000
#else
#define NUM_TASKS  8
#define POOL_SIZE  ((NUM_TASKS+1) * 1024)
#define MAX_ALLOC  512
#define CYCLES_CNT 4000
#endif

#define MAIN_TASK  (TEST_TASK + NUM_TASKS + 2)



extern void    test_task(uint32_t);
extern void    main_task(uint32_t);

void tc_1_main_task(void);// Test #1 - Testing lwmem stress

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
#if PSP_MQX_CPU_IS_KINETIS_L
{ MAIN_TASK, main_task,  1000,   MAIN_TASK, "Main_Task", MQX_AUTO_START_TASK},
#else
{ MAIN_TASK, main_task,  3000,   MAIN_TASK, "Main_Task", MQX_AUTO_START_TASK},
#endif
{ TEST_TASK, test_task,  500,   6, "Test_Task1", 0                  },
{ 0,         0,          0,      0, 0,           0                  }
};

void             *mem_ptrs[NUM_TASKS] = {0};
_mqx_uint         alloc_count[NUM_TASKS] = {0};
_mqx_uint         free_count[NUM_TASKS] = {0};
LWMEM_POOL_STRUCT pool;
_lwmem_pool_id pool_id;
_lwmem_pool_id test_pool_id;

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

   _task_set_priority(0, TEST_TASK+i, &result);
   while (TRUE) {
      if (mem_ptrs[i]) {
         result = _lwmem_free(mem_ptrs[i]);
         free_count[i]++;
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: _lwmem_free operation")
         if (result != MQX_OK) {
            _int_disable();
            while (TRUE) {
            } /* Endwhile */
         } /* Endif */
         mem_ptrs[i] = 0;
      } else {
         mem_ptrs[i] = _lwmem_alloc_from(pool_id,my_rand() % MAX_ALLOC);
         alloc_count[i]++;
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mem_ptrs[i] != NULL), "Test #1 Testing: _lwmem_alloc_from operation")
         if (mem_ptrs[i] == NULL) {
            _int_disable();
            while (TRUE) {
            } /* Endwhile */
         } /* Endif */
      } /* Endif */
      _time_delay_ticks(my_rand() % 5);
   } /* Endwhile */

}


//----------------- Begin Testcases --------------------------------------

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Testing lwmem stress
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    void          *mem_alloc_pool_ptr;
    _mqx_uint      i;
    _mqx_uint      result;
    void          *block_ptr;
    _task_id       tid;

    _mem_zero(mem_ptrs, sizeof(mem_ptrs));

    mem_alloc_pool_ptr = _mem_alloc(POOL_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, mem_alloc_pool_ptr != NULL, "Test #1: Testing 1.1: memory allocate operation")
    if (mem_alloc_pool_ptr == NULL) {
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */
    pool_id = _lwmem_create_pool(&pool, mem_alloc_pool_ptr, POOL_SIZE);
    /*   _lwmem_set_default_pool(pool_id);*/

    my_srand(0xAB0F);

    for (i = 0; i < NUM_TASKS; i++ ) {
        tid = _task_create(0, TEST_TASK, i);
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, tid != MQX_NULL_TASK_ID, "Test #1: Testing 1.2: Create task operation")
        if (tid == MQX_NULL_TASK_ID) {
            _int_disable();
            eunit_test_stop();
            _mqx_exit(1);
        } /* Endif */
    } /* Endfor */

    for (i = 0; i < CYCLES_CNT; i++) {
        result = _lwmem_test(&test_pool_id, &block_ptr);
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.3: lwmem test operation")
        if (result != MQX_OK ) {
            _int_disable();
            while (TRUE) {
            } /* Endwhile */
        } /* Endif */
    } /* Endwhile */
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_2)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Testing lwmem stress")
EU_TEST_SUITE_END(suite_2)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_2, " - Stress test of light weight memory component")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------


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
} /* Endbody */

/* EOF */
