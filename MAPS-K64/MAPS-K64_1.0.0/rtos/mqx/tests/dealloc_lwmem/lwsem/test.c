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
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file contains the source for lightweight semaphore deallocation testing.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test
#define MAIN_TASK       10
#define TEST_TASK       11
#define CLEAN_TASK      12

#define LWSEM_CREATED   4
//------------------------------------------------------------------------
// Test suite function prototype
void main_task(uint32_t parameter);
void test_task(uint32_t parameter);
void clean_task(uint32_t parameter);
void tc_1_main_task(void);//Testing deallocation after task exits
//------------------------------------------------------------------------                      
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
{ MAIN_TASK, main_task,  2000,   8, "Main_Task", MQX_AUTO_START_TASK},
{ TEST_TASK, test_task,  1000,   7, "Test_Task", 0},
{ CLEAN_TASK, clean_task,  1000,   7, "CleanUp_Task", 0},
{ 0,         0,          0,      0, 0,           0}
};

KERNEL_DATA_STRUCT_PTR  kernel_data;
LWSEM_STRUCT   lwsem1, lwsem2;
_mem_size   system_owned;
LWSEM_STRUCT_PTR lwsem_ptr[LWSEM_CREATED];

//dealloc memory and destroy lwsems created in test_task
void clean_task(uint32_t index)
{
   _mqx_uint   result,i;

   for(i=0; i< LWSEM_CREATED; i++){   
      result = _lwsem_destroy(lwsem_ptr[i]);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.6 Destroying lwsems")
      _mem_free(lwsem_ptr[i]);
   }
   result = _lwsem_destroy(&lwsem1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.7 Destroying static lwsem")
      
   result = _lwsem_destroy(&lwsem2);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.8 Destroying static lwsem")
}

//allocate memory and create lwsems
void test_task(uint32_t index)
{
   _mqx_uint   result,i;
   _mem_size   pre_free_mem, post_free_mem;
   
   system_owned = 0;
   
   for(i=0; i<LWSEM_CREATED; i++){
      pre_free_mem = get_free_mem();
      lwsem_ptr[i] = (LWSEM_STRUCT_PTR)_mem_alloc_system(sizeof(LWSEM_STRUCT));
      post_free_mem = get_free_mem();
      system_owned += pre_free_mem - post_free_mem;
      
      result = _lwsem_create(lwsem_ptr[i], i);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.1 Creating sem")
   }
   result = _lwsem_create(&lwsem1, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.2 Creating sem")
   result = _lwsem_create(&lwsem2, 1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.3 Creating sem")
}
//----------------- Begin Testcases --------------------------------------

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test function:  _lwsem_create, _lwsem_destroy
                                          _mem_alloc_system, _mem_free
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   _mem_size   free_mem_pre, free_mem_post;
   _mqx_uint   pre_count, post_count;
   
   _GET_KERNEL_DATA(kernel_data);
     
   free_mem_pre = get_free_mem();
   pre_count = kernel_data->LWSEM.SIZE;
   
   _task_create(0, TEST_TASK, 0);
   
   free_mem_post = get_free_mem();
   post_count = kernel_data->LWSEM.SIZE;

   EU_ASSERT_REF_TC_MSG(tc_1_main_task, free_mem_pre == (free_mem_post + system_owned), "Test #1: Testing 1.4 All alocated resources must be freed ") 
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (pre_count + LWSEM_CREATED + 2) == post_count, "Test #1: Testing 1.5 All created lwsems must be in KERNEL_DATA") 

   _task_create(0, CLEAN_TASK, 0);
   
   free_mem_post = get_free_mem();
   post_count = kernel_data->LWSEM.SIZE;

   EU_ASSERT_REF_TC_MSG(tc_1_main_task, free_mem_pre == free_mem_post, "Test #1: Testing 1.9 All alocated resources must be freed ") 
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, pre_count == post_count, "Test #1: Testing 1.10 All created lwsems must be deleted from KERNEL_DATA") 

}

/******************** End Testcases ********************/

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_dealloc_lwsem)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST1 - Testing lw semaphore deallocation after task exits"),
EU_TEST_SUITE_END(suite_dealloc_lwsem)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_dealloc_lwsem, " - Test of resource deallocation")
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
}/* Endbody */
/* EOF */
