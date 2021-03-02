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
* $Version : 3.8.2.0$
* $Date    : Apr-13-2012$
*
* Comments:
*
*   This file contains code for _task_restart function 
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <event.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include "test.h"
#include "util.h"
#if MQX_USE_LWMEM
#include "lwmem.h"
#include "lwmem_prv.h"
#endif
#if MQX_USE_MEM
#include "mem_prv.h"
#endif
#include <string.h>
#define FILE_BASENAME test

void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
void tc_4_main_task(void);

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK,   main_task,  3000,  9,  "main",  MQX_AUTO_START_TASK | MQX_USER_TASK},
   { TEST_TASK,   test_task,  TEST_STACK_SIZE,  8,  "test",  MQX_USER_TASK},
   { 0,            0L,         0,     0,  0,       0}
};

TD_STRUCT   td_global;
_mqx_uint   glob_result, tc_task;


/******   Test cases body definition      *******/

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test function: _task_restart
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   _mqx_uint   result, i;
   _task_id    test_tid;
   
   tc_task = 1;
   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, TRUE, "Test #1 Test starts")

   result = _task_restart(_mqx_get_system_task_id(), 0, FALSE);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result != MQX_OK), "Test #1 Testing 1.1: Attempt to restart SYSTEM TASK should fail")
      
   test_tid = _task_create(0, TEST_TASK, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (test_tid != MQX_NULL_TASK_ID), "Test #1 Testing 1.2: Creating TEST TASK")
   result = _task_destroy(test_tid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing 1.3: Destroying TEST TASK")
   result = _task_restart(test_tid, (uint32_t *)&td_global, FALSE);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result != MQX_OK), "Test #1 Testing: 1.4: Restarting destroyed task should fail")

   test_tid = _task_create(0, TEST_TASK, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (test_tid != MQX_NULL_TASK_ID), "Test #1 Testing 1.5: Creating TEST TASK")
   for(i = 0; i < TEST_COUNT; i++){
      result = _task_restart(test_tid, (uint32_t *)&td_global, FALSE);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing 1.11: Restarting TEST TASK")
      if(glob_result != MQX_OK){ 
         printf("Failiure on %d. attempt\n\n", (i+1));
    	  break;
      }
   }
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (glob_result == MQX_OK), "Test #1 Testing 1.18: All test competed succesfully")
   result = _task_destroy(test_tid);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing 1.19: Test task destroyed")
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_2_main_task
* Comments     : TEST #1: Test function: restarting dynamicaly created task
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
   _mqx_uint   result, i;
   _task_id    test_tid;
   
   tc_task = 2;
   
   test_tid = _task_create(0, 0, (uint32_t)&MQX_template_list[1]);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (test_tid != MQX_NULL_TASK_ID), "Test #2 Testing 2.1: Creating TEST TASK dynamicaly")
   for(i = 0; i < TEST_COUNT; i++){
      result = _task_restart(test_tid, (uint32_t *)&td_global, FALSE);
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing 2.6: Restarting TEST TASK")
      if(glob_result != MQX_OK){ 
         printf("Failiure on %d. attempt\n\n", (i+1));
         break;
      }
   }
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (glob_result == MQX_OK), "Test #2 Testing 2.13: All test competed succesfully")
   result = _task_destroy(test_tid);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing 2.14: Test task destroyed")
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_3_main_task
* Comments     : TEST #1: Test function: restarting dynamicaly created task
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
   _mqx_uint   result, i;
   _task_id    test_tid;
   uint8_t*    test_task_stack;
   
   tc_task = 3;
   
   test_task_stack = _mem_alloc_zero((_mem_size)TEST_STACK_SIZE_TD);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, test_task_stack != NULL, "Test #3 Testing 3.0: Allocating stack for test tast")

   test_tid = _task_create_at(0, TEST_TASK, 0, test_task_stack, TEST_STACK_SIZE_TD);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (test_tid != MQX_NULL_TASK_ID), "Test #3 Testing 3.1: Creating TEST TASK dynamicaly")
   for(i = 0; i < TEST_COUNT; i++){
      result = _task_restart(test_tid, (uint32_t *)&td_global, FALSE);
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing 3.6: Restarting TEST TASK")
      if(glob_result != MQX_OK){ 
         printf("Failiure on %d. attempt\n\n", (i+1));
         break;
      }
   }
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (glob_result == MQX_OK), "Test #3 Testing 3.13: All test competed succesfully")
   result = _task_destroy(test_tid);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing 3.14: Test task destroyed")
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #1: Test ends
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, TRUE, "Test #4 Testing ends succesfully")
}

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : test_task
*
*END*----------------------------------------------------------------------*/
void test_task
   (
      uint32_t param
   )
{
   TD_STRUCT   td, *td_ptr;
   _mqx_uint   result;
   
   glob_result = 1;
   td_ptr = _task_get_td(MQX_NULL_TASK_ID);
   if(param == 0){
      if(tc_task == 1)
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (td_ptr != NULL), "Test #1 Testing 1.6: Getting TASK DESCRIPTOR from created task")
      else if(tc_task == 2)
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (td_ptr != NULL), "Test #2 Testing 2.1: Getting TASK DESCRIPTOR from dynamicaly created task")
      else
         EU_ASSERT_REF_TC_MSG(tc_3_main_task, (td_ptr != NULL), "Test #3 Testing 3.1: Getting TASK DESCRIPTOR from dynamicaly created task")
   } else {
      if(tc_task == 1)
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (td_ptr != NULL), "Test #1 Testing 1.12: Getting TASK DESCRIPTOR from restarted task")
      else if(tc_task == 2)
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (td_ptr != NULL), "Test #2 Testing 2.7: Getting TASK DESCRIPTOR from restarted task")
      else
         EU_ASSERT_REF_TC_MSG(tc_3_main_task, (td_ptr != NULL), "Test #3 Testing 3.7: Getting TASK DESCRIPTOR from restarted task")
   }
   if(td_ptr == NULL)
      _task_block();
   _mem_copy(td_ptr, &td, sizeof(TD_STRUCT));
   
   result = is_addr_free(_task_get_td(MQX_NULL_TASK_ID));
   if(param == 0){
      if(tc_task == 1)
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == FALSE), "Test #1 Testing 1.7: TASK DESCRIPTOR for created task is allocated")
      else if(tc_task == 2)
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == FALSE), "Test #2 Testing 2.2: TASK DESCRIPTOR for created task is allocated")
      else 
         EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == FALSE), "Test #3 Testing 3.2: TASK DESCRIPTOR for created task is allocated")
   } else {
      if(tc_task == 1)
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == FALSE), "Test #1 Testing 1.13: TASK DESCRIPTOR for restarted task is allocated")
      else if(tc_task == 2)
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == FALSE), "Test #2 Testing 2.8: TASK DESCRIPTOR for restarted task is allocated")
      else 
         EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == FALSE), "Test #3 Testing 3.8: TASK DESCRIPTOR for restarted task is allocated")
      
   }
   if(result == TRUE)
      _task_block();
   
   result = is_addr_free(td.STACK_BASE);
   if(param == 0){
      if(tc_task == 1)
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == FALSE), "Test #1 Testing 1.8: STACK BASE for created task is allocated")
      else if(tc_task == 2)
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == FALSE), "Test #2 Testing 2.3: STACK BASE for created task is allocated")
      else
         EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == FALSE), "Test #3 Testing 3.3: STACK BASE for created task is allocated")
   } else {
      if(tc_task == 1)
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == FALSE), "Test #1 Testing 1.14: STACK BASE for restarted task is allocated")
      else if(tc_task == 2)
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == FALSE), "Test #2 Testing 2.9: STACK BASE for restarted task is allocated")
      else
         EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == FALSE), "Test #3 Testing 3.9: STACK BASE for restarted task is allocated")
   }
   if(result == TRUE)
      _task_block();
   
   result = is_addr_free(td.STACK_LIMIT);
   if(param == 0){
      if(tc_task == 1)
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == FALSE), "Test #1 Testing 1.9: STACK LIMIT address for created task is allocated")
      else if(tc_task == 2)
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == FALSE), "Test #2 Testing 2.4: STACK LIMIT address for created task is allocated")
      else
         EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == FALSE), "Test #3 Testing 3.4: STACK LIMIT address for created task is allocated")
   } else {
      if(tc_task == 1)
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == FALSE), "Test #1 Testing 1.15: STACK LIMIT address for restarted task is allocated")
      else if(tc_task == 2)
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == FALSE), "Test #2 Testing 2.10: STACK LIMIT address for restarted task is allocated")
      else
         EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == FALSE), "Test #3 Testing 3.10: STACK LIMIT address for restarted task is allocated")
   }
   if(result == TRUE)
      _task_block();

   result = td.STACK_BASE > td.STACK_LIMIT;
   if(result){
      result =  td.STACK_BASE > td.STACK_PTR;
      result &= td.STACK_LIMIT < td.STACK_PTR;
   } else {
      result =  td.STACK_BASE < td.STACK_PTR;
      result &= td.STACK_LIMIT > td.STACK_PTR;
   }
   if(param == 0){
      if(tc_task == 1)
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == TRUE), "Test #1 Testing 1.10: STACK PTR of created task is between STACK_BASE and STACK_LIMIT")
      else if(tc_task == 2)
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == TRUE), "Test #2 Testing 2.5: STACK PTR of created task is between STACK_BASE and STACK_LIMIT")
      else
         EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == TRUE), "Test #3 Testing 3.5: STACK PTR of created task is between STACK_BASE and STACK_LIMIT")
   } else {
      if(tc_task == 1)
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == TRUE), "Test #1 Testing 1.16: STACK PTR of restarted task is between STACK_BASE and STACK_LIMIT")
      else if(tc_task == 2)
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == TRUE), "Test #2 Testing 2.11: STACK PTR of restarted task is between STACK_BASE and STACK_LIMIT")
      else
         EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == TRUE), "Test #3 Testing 3.11: STACK PTR of restarted task is between STACK_BASE and STACK_LIMIT")
   }
   if(result == FALSE)
      _task_block();

/*exclude STACK_PTR, INFO, TD_LIST_INFO, TIME_SLICE, CURRENT_TIME_SLICE, WATCHDOG_TIME from comparision */
   td.STACK_PTR = 0;
   td.INFO = 0;
   td.MEMORY_RESOURCE_LIST = 0;
   _mem_zero(&td.TD_LIST_INFO, sizeof(QUEUE_ELEMENT_STRUCT));
#if MQX_HAS_TIME_SLICE
   _mem_zero(&td.TIME_SLICE, sizeof(MQX_TICK_STRUCT));
   _mem_zero(&td.CURRENT_TIME_SLICE, sizeof(MQX_TICK_STRUCT));
#endif
#if MQX_USE_SW_WATCHDOGS
   _mem_zero(&td.WATCHDOG_TIME, sizeof(MQX_TICK_STRUCT));
#endif

   if(param == 0){
      _mem_copy(&td, &td_global, sizeof(TD_STRUCT));
   } else {
      result = memcmp(&td, &td_global, sizeof(TD_STRUCT));
      if(tc_task == 1)
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == 0), "Test #1 Testing 1.17: TASK DESCRIPTOR for created and restartd task are equal")
      else if(tc_task == 2)
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == 0), "Test #2 Testing 2.12: TASK DESCRIPTOR for created and restartd task are equal")
      else 
         EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == 0), "Test #3 Testing 3.12: TASK DESCRIPTOR for created and restartd task are equal")
      if(result != 0){
         print_td_compare(&td, &td_global);
         _task_block();
      }
   }
   
   glob_result = MQX_OK;
   _task_block();

}

/* function returns TRUE if "addr" is found in free space from kernel memory pool
*  including USER memory pool, if aplicable */
bool is_addr_free(void* addr)
{
   KERNEL_DATA_STRUCT_PTR       kernel_data;
   _mqx_uint   result;
   
   _GET_KERNEL_DATA(kernel_data);

#if MQX_USE_LWMEM_ALLOCATOR
   result = is_addr_free_in((_mem_pool_id)kernel_data->KERNEL_LWMEM_POOL, addr);
#if MQX_ENABLE_USER_MODE
   if(result == FALSE)
      result = is_addr_free_in((_mem_pool_id)kernel_data->KD_USER_POOL, addr);
#endif
   
#else //MQX_USE_MEM
   result = is_addr_free_in((_mem_pool_id)&kernel_data->KD_POOL, addr);
#endif
   
   return result;
}

/* function returns TRUE if "addr" is found in free space in certain memory pool */ 
bool is_addr_free_in(_mem_pool_id pool_id, void* addr)
{
#if MQX_USE_LWMEM_ALLOCATOR
   LWMEM_POOL_STRUCT_PTR   pool_ptr = (LWMEM_POOL_STRUCT_PTR)pool_id;
   LWMEM_BLOCK_STRUCT_PTR  block_ptr;
   
   block_ptr = pool_ptr->POOL_FREE_LIST_PTR;
   while(block_ptr){
      if( (block_ptr <= addr) && (addr < block_ptr + block_ptr->BLOCKSIZE) )
         return TRUE;
      block_ptr = block_ptr->U.NEXTBLOCK;
   }      
#else
   MEMPOOL_STRUCT_PTR     pool_ptr = (MEMPOOL_STRUCT_PTR)pool_id;
   STOREBLOCK_STRUCT_PTR  block_ptr;

   block_ptr = pool_ptr->POOL_FREE_LIST_PTR;
   while(block_ptr){
      if( (block_ptr <= addr) && (addr <= NEXT_PHYS(block_ptr)) )
         return TRUE;
      block_ptr = (STOREBLOCK_STRUCT_PTR)NEXT_FREE(block_ptr);
   }
#endif
   return FALSE;
}

void print_td(TD_STRUCT_PTR td_ptr)
{
   printf("TD_NEXT     : 0x%X\n", td_ptr->TD_NEXT);
   printf("TD_PREV     : 0x%X\n", td_ptr->TD_PREV);
   printf("STATE       : 0x%X\n", td_ptr->STATE);
   printf("TASK_ID     : 0x%X\n", td_ptr->TASK_ID);
   printf("STACK_BASE  : 0x%X\n", td_ptr->STACK_BASE);
   printf("STACK_PTR   : 0x%X\n", td_ptr->STACK_PTR);
   printf("STACK_LIMIT : 0x%X\n", td_ptr->STACK_LIMIT);
   printf("MY_QUEUE    : 0x%X\n", td_ptr->MY_QUEUE);
   printf("HOME_QUEUE  : 0x%X\n", td_ptr->HOME_QUEUE);
   printf("TASK_SR     : 0x%X\n", td_ptr->TASK_SR);
   printf("FLAGS       : 0x%X\n", td_ptr->FLAGS);
   printf("MMU         : 0x%X\n", td_ptr->MMU_VIRTUAL_CONTEXT_PTR);
   printf("DSP_PTR     : 0x%X\n", td_ptr->DSP_CONTEXT_PTR);
   printf("FLOAT_PTR   : 0x%X\n", td_ptr->FLOAT_CONTEXT_PTR);
   printf("INFO        : 0x%X\n", td_ptr->INFO);
   printf("BOOSTED     : 0x%X\n", td_ptr->BOOSTED);
   printf("DISABLED_LVL: 0x%X\n", td_ptr->DISABLED_LEVEL);
   printf("RES_LIST    : 0x%X\n", td_ptr->MEMORY_RESOURCE_LIST);
   printf("TASK_ERROR  : 0x%X\n", td_ptr->TASK_ERROR_CODE);
   printf("PARENT      : 0x%X\n", td_ptr->PARENT);
   printf("TEMPLATE_IDX: 0x%X\n", td_ptr->TEMPLATE_INDEX);
   printf("TEMPLATE_PTR: 0x%X\n", td_ptr->TASK_TEMPLATE_PTR);
   printf("TD_LIST_INFO: 0x%X\n", td_ptr->TD_LIST_INFO);
   printf("STD_IN      : 0x%X\n", td_ptr->STDIN_STREAM);
   printf("STD_OUT     : 0x%X\n", td_ptr->STDOUT_STREAM);
   printf("STD_ERR     : 0x%X\n", td_ptr->STDERR_STREAM);
   printf("ENVIRONMENT : 0x%X\n", td_ptr->ENVIRONMENT_PTR);
   printf("EXIT_HANDLER: 0x%X\n", td_ptr->EXIT_HANDLER_PTR);
   printf("EXCP_HANDLER: 0x%X\n", td_ptr->EXCEPTION_HANDLER_PTR);
   printf("STACK_ALLOC : 0x%X\n", td_ptr->STACK_ALLOC_BLOCK);
#if MQX_USE_MESSAGES
   printf("MSG_HEAD    : 0x%X\n", td_ptr->MSG_QUEUE_HEAD);
   printf("MSG_TAIL    : 0x%X\n", td_ptr->MSG_QUEUE_TAIL);
   printf("MSG_AVAIL   : 0x%X\n", td_ptr->MESSAGES_AVAILABLE);
   printf("MESSAGE     : 0x%X\n", td_ptr->MESSAGE);
#endif
#if MQX_USE_LWEVENTS
   printf("LWEVENT_BITS: 0x%X\n", td_ptr->LWEVENT_BITS);
#endif
#if MQX_HAS_TIME_SLICE
   printf("TIME_SLICE  : 0x%X\n", td_ptr->TIME_SLICE);
   printf("CUR_TIME_SLI: 0x%X\n", td_ptr->CURRENT_TIME_SLICE);
#endif
#if MQX_USE_SW_WATCHDOGS
   printf("WATCHDOG    : 0x%X\n", td_ptr->WATCHDOG_TIME);
#endif
#if MQX_PROFILING_ENABLE
   printf("PROFILER    : 0x%X\n", td_ptr->PROFILER_CONTEXT_PTR);
#endif
#if MQX_RUN_TIME_ERR_CHECK_ENABLE
   printf("RUNTIME CHK : 0x%X\n", td_ptr->RUNTIME_ERROR_CHECK_PTR);
#endif
#if MQX_THREAD_LOCAL_STORAGE_ENABLE
   printf("CRT_TLS     : 0x%X\n", td_ptr->CRT_TLS);
#endif

}

void print_td_compare(TD_STRUCT_PTR a, TD_STRUCT_PTR b)
{
   printf("TD_NEXT     : 0x%08X 0x%08X", a->TD_NEXT, b->TD_NEXT);
   if(a->TD_NEXT == b->TD_NEXT) printf(" OK\n"); else printf(" ERROR\n");
   printf("TD_PREV     : 0x%08X 0x%08X", a->TD_PREV, b->TD_PREV);
   if(a->TD_PREV == b->TD_PREV) printf(" OK\n"); else printf(" ERROR\n");
   printf("STATE       : 0x%08X 0x%08X", a->STATE, b->STATE);
   if(a->STATE == b->STATE) printf(" OK\n"); else printf(" ERROR\n");
   printf("TASK_ID     : 0x%08X 0x%08X", a->TASK_ID, b->TASK_ID);
   if(a->TASK_ID == b->TASK_ID) printf(" OK\n"); else printf(" ERROR\n");
   printf("STACK_BASE  : 0x%08X 0x%08X", a->STACK_BASE, b->STACK_BASE);
   if(a->STACK_BASE == b->STACK_BASE) printf(" OK\n"); else printf(" ERROR\n");
   printf("STACK_PTR   : 0x%08X 0x%08X", a->STACK_PTR, b->STACK_PTR);
   if(a->STACK_PTR == b->STACK_PTR) printf(" OK\n"); else printf(" ERROR\n");
   printf("STACK_LIMIT : 0x%08X 0x%08X", a->STACK_LIMIT, b->STACK_LIMIT);
   if(a->STACK_LIMIT, b->STACK_LIMIT) printf(" OK\n"); else printf(" ERROR\n");
   printf("MY_QUEUE    : 0x%08X 0x%08X", a->MY_QUEUE, b->MY_QUEUE);
   if(a->MY_QUEUE == b->MY_QUEUE) printf(" OK\n"); else printf(" ERROR\n");
   printf("HOME_QUEUE  : 0x%08X 0x%08X", a->HOME_QUEUE, b->HOME_QUEUE);
   if(a->HOME_QUEUE == b->HOME_QUEUE) printf(" OK\n"); else printf(" ERROR\n");
   printf("TASK_SR     : 0x%08X 0x%08X", a->TASK_SR, b->TASK_SR);
   if(a->TASK_SR == b->TASK_SR) printf(" OK\n"); else printf(" ERROR\n");
   printf("FLAGS       : 0x%08X 0x%08X", a->FLAGS, b->FLAGS);
   if(a->FLAGS == b->FLAGS) printf(" OK\n"); else printf(" ERROR\n");
   printf("MMU         : 0x%08X 0x%08X", a->MMU_VIRTUAL_CONTEXT_PTR, b->MMU_VIRTUAL_CONTEXT_PTR);
   if(a->MMU_VIRTUAL_CONTEXT_PTR == b->MMU_VIRTUAL_CONTEXT_PTR) printf(" OK\n"); else printf(" ERROR\n");
   printf("DSP_PTR     : 0x%08X 0x%08X", a->DSP_CONTEXT_PTR, b->DSP_CONTEXT_PTR);
   if(a->DSP_CONTEXT_PTR == b->DSP_CONTEXT_PTR) printf(" OK\n"); else printf(" ERROR\n");
   printf("FLOAT_PTR   : 0x%08X 0x%08X", a->FLOAT_CONTEXT_PTR, b->FLOAT_CONTEXT_PTR);
   if(a->FLOAT_CONTEXT_PTR == b->FLOAT_CONTEXT_PTR) printf(" OK\n"); else printf(" ERROR\n");
   printf("INFO        : 0x%08X 0x%08X", &a->INFO, &b->INFO);
   if(&a->INFO == &b->INFO) printf(" OK\n"); else printf(" ERROR\n");
   printf("BOOSTED     : 0x%08X 0x%08X", a->BOOSTED, b->BOOSTED);
   if(a->BOOSTED == b->BOOSTED) printf(" OK\n"); else printf(" ERROR\n");
   printf("DISABLED_LVL: 0x%08X 0x%08X", a->DISABLED_LEVEL, b->DISABLED_LEVEL);
   if(a->DISABLED_LEVEL == b->DISABLED_LEVEL) printf(" OK\n"); else printf(" ERROR\n");
   printf("RES_LIST    : 0x%08X 0x%08X", a->MEMORY_RESOURCE_LIST, b->MEMORY_RESOURCE_LIST);
   if(a->MEMORY_RESOURCE_LIST == b->MEMORY_RESOURCE_LIST) printf(" OK\n"); else printf(" ERROR\n");
   printf("TASK_ERROR  : 0x%08X 0x%08X", a->TASK_ERROR_CODE, b->TASK_ERROR_CODE);
   if(a->TASK_ERROR_CODE == b->TASK_ERROR_CODE) printf(" OK\n"); else printf(" ERROR\n");
   printf("PARENT      : 0x%08X 0x%08X", a->PARENT, b->PARENT);
   if(a->PARENT == b->PARENT) printf(" OK\n"); else printf(" ERROR\n");
   printf("TEMPLATE_IDX: 0x%08X 0x%08X", a->TEMPLATE_INDEX, b->TEMPLATE_INDEX);
   if(a->TEMPLATE_INDEX == b->TEMPLATE_INDEX) printf(" OK\n"); else printf(" ERROR\n");
   printf("TEMPLATE_PTR: 0x%08X 0x%08X", a->TASK_TEMPLATE_PTR, b->TASK_TEMPLATE_PTR);
   if(a->TASK_TEMPLATE_PTR == b->TASK_TEMPLATE_PTR) printf(" OK\n"); else printf(" ERROR\n");
   printf("TD_LIST_INFO: 0x%08X 0x%08X", &a->TD_LIST_INFO, &b->TD_LIST_INFO);
   if(&a->TD_LIST_INFO == &b->TD_LIST_INFO) printf(" OK\n"); else printf(" ERROR\n");
   printf("STD_IN      : 0x%08X 0x%08X", a->STDIN_STREAM, b->STDIN_STREAM);
   if(a->STDIN_STREAM == b->STDIN_STREAM) printf(" OK\n"); else printf(" ERROR\n");
   printf("STD_OUT     : 0x%08X 0x%08X", a->STDOUT_STREAM, b->STDOUT_STREAM);
   if(a->STDOUT_STREAM == b->STDOUT_STREAM) printf(" OK\n"); else printf(" ERROR\n");
   printf("STD_ERR     : 0x%08X 0x%08X", a->STDERR_STREAM, b->STDERR_STREAM);
   if(a->STDERR_STREAM == b->STDERR_STREAM) printf(" OK\n"); else printf(" ERROR\n");
   printf("ENVIRONMENT : 0x%08X 0x%08X", a->ENVIRONMENT_PTR, b->ENVIRONMENT_PTR);
   if(a->ENVIRONMENT_PTR == b->ENVIRONMENT_PTR) printf(" OK\n"); else printf(" ERROR\n");
   printf("EXIT_HANDLER: 0x%08X 0x%08X", a->EXIT_HANDLER_PTR, b->EXIT_HANDLER_PTR);
   if(a->EXIT_HANDLER_PTR == b->EXIT_HANDLER_PTR) printf(" OK\n"); else printf(" ERROR\n");
   printf("EXCP_HANDLER: 0x%08X 0x%08X", a->EXCEPTION_HANDLER_PTR, b->EXCEPTION_HANDLER_PTR);
   if(a->EXCEPTION_HANDLER_PTR == b->EXCEPTION_HANDLER_PTR) printf(" OK\n"); else printf(" ERROR\n");
   printf("STACK_ALLOC : 0x%08X 0x%08X", a->STACK_ALLOC_BLOCK, b->STACK_ALLOC_BLOCK);
   if(a->STACK_ALLOC_BLOCK == b->STACK_ALLOC_BLOCK) printf(" OK\n"); else printf(" ERROR\n");
#if MQX_USE_MESSAGES
   printf("MSG_HEAD    : 0x%08X 0x%08X", a->MSG_QUEUE_HEAD, b->MSG_QUEUE_HEAD);
   if(a->MSG_QUEUE_HEAD == b->MSG_QUEUE_HEAD) printf(" OK\n"); else printf(" ERROR\n");
   printf("MSG_TAIL    : 0x%08X 0x%08X", a->MSG_QUEUE_TAIL, b->MSG_QUEUE_TAIL);
   if(a->MSG_QUEUE_TAIL == b->MSG_QUEUE_TAIL) printf(" OK\n"); else printf(" ERROR\n");
   printf("MSG_AVAIL   : 0x%08X 0x%08X", a->MESSAGES_AVAILABLE, b->MESSAGES_AVAILABLE);
   if(a->MESSAGES_AVAILABLE == b->MESSAGES_AVAILABLE) printf(" OK\n"); else printf(" ERROR\n");
   printf("MESSAGE     : 0x%08X 0x%08X", a->MESSAGE, b->MESSAGE);
   if(a->MESSAGE == b->MESSAGE) printf(" OK\n"); else printf(" ERROR\n");
#endif
#if MQX_USE_LWEVENTS
   printf("LWEVENT_BITS: 0x%08X 0x%08X", a->LWEVENT_BITS, b->LWEVENT_BITS);
   if(a->LWEVENT_BITS == b->LWEVENT_BITS) printf(" OK\n"); else printf(" ERROR\n");
#endif

#if MQX_HAS_TIME_SLICE
   printf("TIME_SLICE  : 0x%08X 0x%08X", &a->TIME_SLICE, &b->TIME_SLICE);
   if(&a->TIME_SLICE == &b->TIME_SLICE) printf(" OK\n"); else printf(" ERROR\n");
   printf("CUR_TIME_SLI: 0x%08X 0x%08X", &a->CURRENT_TIME_SLICE, &b->CURRENT_TIME_SLICE);
   if(&a->CURRENT_TIME_SLICE == &b->CURRENT_TIME_SLICE) printf(" OK\n"); else printf(" ERROR\n");
#endif
#if MQX_USE_SW_WATCHDOGS
   printf("WATCHDOG    : 0x%08X 0x%08X", &a->WATCHDOG_TIME, &b->WATCHDOG_TIME);
   if(&a->WATCHDOG_TIME == &b->WATCHDOG_TIME) printf(" OK\n"); else printf(" ERROR\n");
#endif
#if MQX_PROFILING_ENABLE
   printf("PROFILER    : 0x%08X 0x%08X", a->PROFILER_CONTEXT_PTR, b->PROFILER_CONTEXT_PTR);
   if(a->PROFILER_CONTEXT_PTR == b->PROFILER_CONTEXT_PTR) printf(" OK\n"); else printf(" ERROR\n");
#endif
#if MQX_RUN_TIME_ERR_CHECK_ENABLE
   printf("RUNTIME CHK : 0x%08X 0x%08X", a->RUNTIME_ERROR_CHECK_PTR, b->RUNTIME_ERROR_CHECK_PTR);
   if(a->RUNTIME_ERROR_CHECK_PTR == b->RUNTIME_ERROR_CHECK_PTR) printf(" OK\n"); else printf(" ERROR\n");
#endif
#if MQX_THREAD_LOCAL_STORAGE_ENABLE
   printf("CRT_TLS     : 0x%08X 0x%08X", a->CRT_TLS, b->CRT_TLS);
   if(a->CRT_TLS == b->CRT_TLS) printf(" OK\n"); else printf(" ERROR\n");
#endif

}
//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_task)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Testing _task_restart"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Testing _task_restart with dynamicaly created task"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST #3 - Testing _task_restart with task_create_at"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST #4 - Testing ends succesfully")
 EU_TEST_SUITE_END(suite_task)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_task, " - Test task suite")
 EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : main_task
* Comments     : 
*    This is the main task of the test program
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      /* [IN] The MQX task parameter */
      uint32_t param
   )
{ /* Body */
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* Endbody */

/* EOF */
