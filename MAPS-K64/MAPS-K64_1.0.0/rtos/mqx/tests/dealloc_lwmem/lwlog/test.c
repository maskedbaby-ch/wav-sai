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
* $Version : 3.8.1.1$
* $Date    : Feb-22-2012$
*
* Comments:
*
*   This file contains the source for lightweight log deallocation testing.
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <name.h>
#include <name_prv.h>
#include <lwlog.h>
#include <lwlog_prv.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test
#define MAIN_TASK       10
#define TEST_TASK       11
#define CLEAN_TASK      12

#define LOGS_CREATED    5

#if PSP_MQX_CPU_IS_KINETIS_L
#define REC_MULTIPLIER  2
#else
#define REC_MULTIPLIER  10
#endif

//------------------------------------------------------------------------
// Test suite function prototype
void main_task(uint32_t parameter);
void test_task(uint32_t parameter);
void clean_task(uint32_t parameter);
_mqx_uint log_count(LWLOG_COMPONENT_STRUCT_PTR); 
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

//returns count of created lwlogs
_mqx_uint log_count(LWLOG_COMPONENT_STRUCT_PTR log_comp)
{
   _mqx_uint   i, result=0;
   
   for(i=0; i<16;i++){
      if(log_comp->LOGS[i] != NULL)
         result++;
   }
   
   return result;
}

//destroy lwlogs created in test_task
void clean_task(uint32_t index)
{
   _mqx_uint   result,i;

   for(i=0; i< LOGS_CREATED; i++){   
      result = _lwlog_destroy(i);
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.5 Destroying lwlogs")
   }

}

//create lwlogs
void test_task(uint32_t index)
{
   _mqx_uint   result,i;
   _mem_size   pre_free_mem, post_free_mem;
   
   for(i=0;i<LOGS_CREATED; i++){
      pre_free_mem = get_free_mem();
      result = _lwlog_create(i,(1+i)*REC_MULTIPLIER, LOG_OVERWRITE);
      post_free_mem = get_free_mem();
      system_owned += pre_free_mem - post_free_mem;
      EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.2 Creating logs")
   }
}
//----------------- Begin Testcases --------------------------------------

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test function:  _lwlog_create_component, _lwlog_create, _lwlog_destroy
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   _mem_size   free_mem_pre, free_mem_post;
   _mqx_uint   result, pre_counter, post_counter;
   LWLOG_COMPONENT_STRUCT_PTR  log_comp_struct;
   
   _GET_KERNEL_DATA(kernel_data);
   
   result = _lwlog_create_component();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.1 Creating LOG component") 

   log_comp_struct = (LWLOG_COMPONENT_STRUCT_PTR)kernel_data->KERNEL_COMPONENTS[KERNEL_LWLOG]; 
   pre_counter = log_count(log_comp_struct);
   free_mem_pre = get_free_mem();
   
   _task_create(0, TEST_TASK, 0);
   
   free_mem_post = get_free_mem();
   post_counter = log_count(log_comp_struct);
   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, free_mem_pre == (free_mem_post + system_owned) , "Test #1: Testing 1.3 All alocated resources must be freed ")    
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (pre_counter + LOGS_CREATED) == post_counter, "Test #1: Testing 1.4 All created lwlogs found in LWLOG_COMPONENT") 
      
   _task_create(0, CLEAN_TASK, 0);
   
   free_mem_post = get_free_mem();
   post_counter = log_count(log_comp_struct);
   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, free_mem_pre == free_mem_post, "Test #1: Testing 1.6 All alocated resources must be freed ")    
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, pre_counter == post_counter, "Test #1: Testing 1.7 All created lwlogs should be erased from LWLOG_COMPONENT") 

}

/******************** End Testcases ********************/

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_dealloc_lwlog)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST1 - Testing log deallocation after task exits"),
EU_TEST_SUITE_END(suite_dealloc_lwlog)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_dealloc_lwlog, " - Test of resource deallocation")
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
