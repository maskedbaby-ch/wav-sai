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
*   This file contains the source for message queue deallocation testing.
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <name.h>
#include <name_prv.h>
#include <message.h>
#include <msg_prv.h>
#include "test.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test

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
_pool_id    pool_id[2];

//destroy message pools created in test_task
void clean_task(uint32_t index)
{
   _mqx_uint   result;

   result = _msgpool_destroy(pool_id[0]);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.7 Destroying msgpool 1")
      
   result = _msgpool_destroy(pool_id[1]);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.8 Destroying msgpool 2")
}

//create message pools, open pool, allocate memory for a message, send message
void test_task(uint32_t index)
{
   _mqx_uint   result;
   _mem_size   pre_free_mem, post_free_mem;
   _queue_id   queue_id;
   MESSAGE_PTR msg;
   
   system_owned = 0;

   pre_free_mem = get_free_mem();
   pool_id[0] = _msgpool_create(MSG_SIZE, NUM_MSG, 0, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, pool_id[0] != NULL, "Test #1: Testing 1.2 Creating MESSAGE pool1") 
   pool_id[1] = _msgpool_create(MSG_SIZE, NUM_MSG, 0, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, pool_id[1] != NULL, "Test #1: Testing 1.3 Creating MESSAGE pool2") 
   post_free_mem = get_free_mem();
   system_owned += pre_free_mem - post_free_mem;
   
   queue_id = _msgq_open(8,0);   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, queue_id != MSGQ_NULL_QUEUE_ID, "Test #1: Testing 1.4 Opening message queue") 
      
   msg = (MESSAGE_PTR)_msg_alloc(pool_id[0]);
   msg->HEADER.SIZE = MSG_SIZE;
   msg->HEADER.TARGET_QID = queue_id;
   msg->HEADER.SOURCE_QID = queue_id;
   result = _msgq_send(msg);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == TRUE, "Test #1: Testing 1.5 Sending a message") 
}
//----------------- Begin Testcases --------------------------------------

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test function:  _msg_create_component, _msgpool_create, _msgpool_destroy
                                          _msg_open, _msg_alloc, _msg_send
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   _mem_size   free_mem_pre, free_mem_post;
   _mqx_uint   result;
   
   _GET_KERNEL_DATA(kernel_data);
   
   result = _msg_create_component();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.1 Creating MESSAGE component") 

   free_mem_pre = get_free_mem();  
   _task_create(0, TEST_TASK, 0);
   free_mem_post = get_free_mem();
   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, free_mem_pre == (free_mem_post + system_owned), "Test #1: Testing 1.6 All alocated resources must be freed ")    

   _task_create(0, CLEAN_TASK, 0);
   free_mem_post = get_free_mem();
   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, free_mem_pre == free_mem_post, "Test #1: Testing 1.9 All alocated resources must be freed ")    

}

/******************** End Testcases ********************/

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_dealloc_message)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST1 - Testing event deallocation after task exits"),
EU_TEST_SUITE_END(suite_dealloc_message)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_dealloc_message, " - Test of resource deallocation")
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
