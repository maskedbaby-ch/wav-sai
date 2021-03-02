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
* $Version : 3.8.4.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file tests the light weight message queue functions .
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <util.h>
#include <lwmsgq.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test

#define SUPER_TASK   9
#define MAIN_TASK    10
#define PRIV_TASK    13

#define MAIN_PRIO    10

#define NUM_MESSAGES 32
#define MSG_SIZE     4
#define TEST_VALUE   1

void super_task(uint32_t parameter);
void priv_task(uint32_t parameter);
void main_task(uint32_t parameter);

void tc_1_main_task(void);// Test #1 - destruction of uninitialized message queue....
void tc_2_main_task(void);// Test #2 - create lwsem....
                         
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { SUPER_TASK,  super_task, 1000, MAIN_PRIO+1,   "super", MQX_AUTO_START_TASK},
   { MAIN_TASK,   main_task,  1000, MAIN_PRIO,     "user",  MQX_USER_TASK},
   { PRIV_TASK,   priv_task,  1000, MAIN_PRIO,     "priv",  0},
   { 0,           0,          0,    0,             0,       0}
};

USER_NO_ACCESS uint32_t queue_no[sizeof(LWMSGQ_STRUCT)/sizeof(uint32_t) + NUM_MESSAGES * MSG_SIZE];
USER_RO_ACCESS uint32_t queue_priv[sizeof(LWMSGQ_STRUCT)/sizeof(uint32_t) + NUM_MESSAGES * MSG_SIZE];
USER_RO_ACCESS uint32_t queue_usr[sizeof(LWMSGQ_STRUCT)/sizeof(uint32_t) + NUM_MESSAGES * MSG_SIZE];
USER_RO_ACCESS uint32_t queue_usr2[sizeof(LWMSGQ_STRUCT)/sizeof(uint32_t) + NUM_MESSAGES * MSG_SIZE];
USER_RW_ACCESS uint32_t queue_rw[sizeof(LWMSGQ_STRUCT)/sizeof(uint32_t) + NUM_MESSAGES * MSG_SIZE];

uint32_t volatile  error = 0;

/******************** Begin Testcases **********************/

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Attempts to use "no acces" or privilege lwsems, should return MQX_LWSEM_INVALID
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint        result;    
    uint32_t          msg[MSG_SIZE];

#if MQX_ENABLE_USER_STDAPI == 1

    result = _lwmsgq_init((void *)queue_no, NUM_MESSAGES, MSG_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.2: Creation of 'no access' message queue from user task using privilege API")

    result = _lwmsgq_init((void *)queue_rw, NUM_MESSAGES, MSG_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.4: Posting of privilege message queue from user task using privilege API")

    result = _lwmsgq_send((void *)queue_priv, msg, 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.6: Polling on privilege message queue from user task using privilege API")

    result = _lwmsgq_receive((void *)queue_priv, msg, 0, 0, 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.8: Waiting (ticks) on privilege message queue from user task using privilege API")

#endif    

    result = _usr_lwmsgq_init((void *)queue_no, NUM_MESSAGES, MSG_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.1: Creation of 'no access' message queue ")

    result = _usr_lwmsgq_init((void *)queue_rw, NUM_MESSAGES, MSG_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.3: Posting of privilege message queue from user task")
    
    result = _usr_lwmsgq_send((void *)queue_priv, msg, 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.5: Polling on privilege message queue from user task")
    
    result = _usr_lwmsgq_receive((void *)queue_priv, msg, 0, 0, 0);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result != MQX_OK, "Test #1: Testing 1.7: Waiting (ticks) on privilege message queue from user task")

}
/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Create the lwsem, return code should be MQX_OK, 0.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
   _mqx_uint     result;
   _mqx_uint     msg[MSG_SIZE];

   result = _usr_lwmsgq_init((void *)queue_usr2, NUM_MESSAGES, MSG_SIZE);   
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.1: Create the lwsem return code should be MQX_OK")   
    
   msg[0] = TEST_VALUE;
   result = _usr_lwmsgq_send((void *)queue_usr, msg, 0);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.2: Setting the lwsem -> run privilege task return code should be MQX_OK")   

   result = _usr_lwmsgq_receive((void *)queue_usr2, msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.3: Waiting from priv task to set user message queue using user API; return code should be MQX_OK")   
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (msg[0] == TEST_VALUE + 1), "Test #2: Testing 2.4: Priv task aswered properly; return code should be MQX_OK")   

   result = _usr_lwmsgq_send((void *)queue_usr, msg, 0);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.5: Setting the lwsem -> run privilege task; return code should be MQX_OK")   

   result = _usr_lwmsgq_receive((void *)queue_usr2, msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.6: Waiting from priv task to set user message queue using priv API; return code should be MQX_OK")       
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (msg[0] == TEST_VALUE + 2) , "Test #2: Testing 2.7: Priv task aswered properly; return code should be MQX_OK")   
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_usermode)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - attempts to reach 'no access' or privilege lwmsgq...."),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - synchronizig with privilege task...."),
EU_TEST_SUITE_END(suite_usermode)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_usermode, " - Test of Lightweight message queue #2")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : main_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t parameter
   )
{ /* Body */

   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* Endbody */

void priv_task
   (
      uint32_t parameter
   )
{
   _mqx_uint   result;
   _mqx_uint   msg[MSG_SIZE]; 
   result = /*_usr*/_lwmsgq_receive((void *)queue_usr, msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0);
   if(result != MQX_OK){
      _task_destroy(0);
   }
   msg[0]++;
   result = /*_usr*/_lwmsgq_send((void *)queue_usr2, msg, 0);

   result = _lwmsgq_receive((void *)queue_usr, msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0);
   if(result != MQX_OK){
      _task_destroy(0);
   }
   msg[0]++;
   result = _lwmsgq_send((void *)queue_usr2, msg, LWMSGQ_SEND_BLOCK_ON_SEND);
}

void super_task
   (
      uint32_t parameter
   )
{
   _task_id    tid;
   _mqx_uint   result;
   
   _int_install_unexpected_isr();
   
   _mem_zero(&queue_no, sizeof(LWMSGQ_STRUCT)/sizeof(uint32_t) + NUM_MESSAGES * MSG_SIZE);
   _mem_zero(&queue_rw, sizeof(LWMSGQ_STRUCT)/sizeof(uint32_t) + NUM_MESSAGES * MSG_SIZE);
   _mem_zero(&queue_priv, sizeof(LWMSGQ_STRUCT)/sizeof(uint32_t) + NUM_MESSAGES * MSG_SIZE);
   _mem_zero(&queue_usr, sizeof(LWMSGQ_STRUCT)/sizeof(uint32_t) + NUM_MESSAGES * MSG_SIZE);
   _mem_zero(&queue_usr2, sizeof(LWMSGQ_STRUCT)/sizeof(uint32_t) + NUM_MESSAGES * MSG_SIZE);

   result = _lwmsgq_init(&queue_priv, NUM_MESSAGES, MSG_SIZE);
   if(result != MQX_OK){
      printf("ERROR: Cannot create privilege lwmsgq -> app terminated!");
      return;
   }
   result = _usr_lwmsgq_init(&queue_usr, NUM_MESSAGES, MSG_SIZE);
   if(result != MQX_OK){
      printf("ERROR: Cannot create user lwmsgq -> app terminated!");
      return;
   }

   tid = _task_create(0, PRIV_TASK, 0);
   if(tid == MQX_NULL_TASK_ID){
     printf("Unable to create test application");
     return;
   }
   
   tid = _task_create(0, MAIN_TASK, 0);
   if(tid == MQX_NULL_TASK_ID){
     printf("Unable to create test application");
     return;
   }
}

/* EOF */
