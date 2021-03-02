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
*   This file contains the source for semaphore deallocation testing.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <sem.h>
#include <name.h>
#include <sem_prv.h>
#include <name_prv.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test
#define MAIN_TASK       10
#define TEST_TASK       11
#define CLEAN_TASK      12
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
_mem_size   system_owned;

//destroy semaphore created in test_task
void clean_task(uint32_t index)
{
   _mqx_uint   result;
   
   result = _sem_destroy("sem.one", TRUE);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.8 Destroying sem1")
      
   result = _sem_destroy("sem.two", TRUE);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.9 Destroying sem2")
    
   result = _sem_destroy_fast(7, TRUE);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.10 Destroying sem3")
      
   result = _sem_destroy_fast(13, TRUE);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.11 Destroying sem4")
}

//create and open semaphores
void test_task(uint32_t index)
{
   _mqx_uint   result;
   SEM_CONNECTION_STRUCT_PTR     sem_pointer;
   _mem_size   pre_free_mem, post_free_mem;
   
   system_owned = 0;
   
   pre_free_mem = get_free_mem();
   result = _sem_create("sem.one", 3, SEM_PRIORITY_QUEUEING);
   post_free_mem = get_free_mem();
   system_owned += pre_free_mem - post_free_mem;
   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.2 Creating sem1")
   result = _sem_open("sem.one", (void*)&sem_pointer);
   
   pre_free_mem = get_free_mem();
   result = _sem_create("sem.two", 1, 0);   
   post_free_mem = get_free_mem();
   system_owned += pre_free_mem - post_free_mem;
   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.3 Creating sem2")
   result = _sem_open("sem.two", (void*)&sem_pointer);
   
   pre_free_mem = get_free_mem();
   result = _sem_create_fast(7, 1, SEM_STRICT);
   post_free_mem = get_free_mem();
   system_owned += pre_free_mem - post_free_mem;
   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.4 Creating sem3")
   result = _sem_open_fast(7, (void*)&sem_pointer);
   
   pre_free_mem = get_free_mem();
   result = _sem_create_fast(13, 8, SEM_PRIORITY_QUEUEING);
   post_free_mem = get_free_mem();
   system_owned += pre_free_mem - post_free_mem;
   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.5 Creating sem4")
   result = _sem_open_fast(13, (void*)&sem_pointer);

}
//----------------- Begin Testcases --------------------------------------

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test function: _sem_create_component, _sem_create, _sem_open, _sem_destroy
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   _mem_size   free_mem_pre, free_mem_post;
   _mqx_uint   result, pre_count, post_count;
   NAME_COMPONENT_STRUCT_PTR  name_comp_struct;
   
   _GET_KERNEL_DATA(kernel_data);
   
   result = _sem_create_component(16,8,0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.1 Creating SEMAPHORE component") 

   name_comp_struct = ((SEM_COMPONENT_STRUCT_PTR)kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES])->NAME_TABLE_HANDLE;
   pre_count = name_comp_struct->NUMBER;
   free_mem_pre = get_free_mem();

   _task_create(0, TEST_TASK, 0);
   
   post_count = name_comp_struct->NUMBER;
   free_mem_post = get_free_mem();
   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, free_mem_pre == (free_mem_post + system_owned), "Test #1: Testing 1.6 All alocated resources must be freed ")   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (pre_count + 4) == post_count , "Test #1: Testing 1.7 All created semaphores found in SEM_COMPONENT") 

   _task_create(0, CLEAN_TASK, 0);
   post_count = name_comp_struct->NUMBER;
   free_mem_post = get_free_mem();
   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, free_mem_pre == free_mem_post, "Test #1: Testing 1.12 All alocated resources must be freed ")    
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, pre_count == post_count, "Test #1: Testing 1.13 All semaphores should be deleted") 

}

/******************** End Testcases ********************/

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_dealloc_sem)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST1 - Testing semaphore deallocation after task exits"),
EU_TEST_SUITE_END(suite_dealloc_sem)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_dealloc_sem, " - Test of resource deallocation")
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
