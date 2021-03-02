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
* $Version : 3.8.6.1$
* $Date    : Jul-23-2012$
*
* Comments:
*
*   This file contains code for the MQX message verification
*   program.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <message.h>
#include "test.h"
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"


//------------------------------------------------------------------------
// required user_config settings
//------------------------------------------------------------------------
#if !MQX_CHECK_ERRORS
    #error "Required MQX_CHECK_ERRORS"
#endif
#if !MQX_CHECK_VALIDITY
    #error "Required MQX_CHECK_VALIDITY"
#endif
#if !MQXCFG_ENABLE_MSG_TIMEOUT_ERROR
    #error "Required MQXCFG_ENABLE_MSG_TIMEOUT_ERROR"
#endif
#if !MQX_USE_MESSAGES
    #error "Required MQX_USE_MESSAGES"
#endif



#define FILE_BASENAME   test

unsigned char definition[] = {4,2,1,2,4,0};
_mqx_uint   global_count;
bool     error_found;

//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);//TEST # 1 - Testing _msg_create_component primitive
void tc_2_main_task(void);//TEST # 2 - Testing _msgpool_create and _msgpool_create_system primitives
void tc_3_main_task(void);//TEST # 3 - Testing system message queue primitives
void tc_4_main_task(void);//TEST # 4 - Testing private message queue primitives
void tc_5_main_task(void);//TEST # 5 - Testing private message queue primitives with a maximum size
void tc_6_main_task(void);//TEST # 6 - Testing send priority and urgent messages
void tc_7_main_task(void);//TEST # 7 - Testing send queue messages
void tc_8_main_task(void);//TEST # 8 - Testing send broadcast messages
//------------------------------------------------------------------------
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
#if PSP_MQX_CPU_IS_KINETIS_L
   { MAIN_TASK,      main_task,      1500,  9, "main",    MQX_AUTO_START_TASK},
   { TEST4_TASK,     test4_task,     1500,  9, "test4",   0},
#else
   { MAIN_TASK,      main_task,      3000,  9, "main",    MQX_AUTO_START_TASK},
   { TEST4_TASK,     test4_task,     3000,  9, "test4",   0},
#endif
   { 0,              0,              0,     0, 0,         0}
};

MQX_INITIALIZATION_STRUCT  MQX_init_struct =
{
   /* PROCESSOR_NUMBER                */  BSP_DEFAULT_PROCESSOR_NUMBER,
   /* START_OF_KERNEL_MEMORY          */  BSP_DEFAULT_START_OF_KERNEL_MEMORY,
   /* END_OF_KERNEL_MEMORY            */  BSP_DEFAULT_END_OF_KERNEL_MEMORY,
   /* INTERRUPT_STACK_SIZE            */  BSP_DEFAULT_INTERRUPT_STACK_SIZE,
   /* TASK_TEMPLATE_LIST              */  (void *)MQX_template_list,
   /* MQX_HARDWARE_INTERRUPT_LEVEL_MAX*/  BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX,
   /* MAX_MSGPOOLS                    */  10,
#if (MQX_USE_32BIT_MESSAGE_QIDS == 1)
   /* MAX_MSGQS                       */  300,
#else
   /* MAX_MSGQS                       */  15,
#endif
   /* IO_CHANNEL                      */  BSP_DEFAULT_IO_CHANNEL,
#if MQX_USE_IO_OLD
   /* IO_OPEN_MODE                    */  BSP_DEFAULT_IO_OPEN_MODE
#else
   /* IO_OPEN_MODE                    */  0
#endif
};


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name: queue_not_function
* Return Value : none
* Comments     : 
*    This notification function is called in test3
*
*END*----------------------------------------------------------------------*/

void queue_not_function(void *qno)
{
   MESSAGE_HEADER_STRUCT_PTR   msg_ptr;
   _mqx_uint                   result;

   /*
   ** function called in task3 when a msg is placed on system queue
   ** msg is received from system queue
   ** increment global counter of number of msgs processed
   ** free the received message
   */
   msg_ptr = _msgq_poll(_msgq_get_id((_processor_number)0,(_queue_number)qno));
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr != NULL, "Test #3: Testing 3.30: Poll message queue for a message")
   if (msg_ptr == NULL) {
      _task_set_error(MQX_OK);
   } else {   
      global_count++;
      _msg_free(msg_ptr);
      result = _task_get_error();
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.31: Free message operation")
      if (result != MQX_OK) {
         _task_set_error(MQX_OK);
      }
   }
}


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name: test4_not
* Return Value : none
* Comments     : 
*    This notification function is called in test4
*
*END*----------------------------------------------------------------------*/

void test4_not(void *task_td)
{
   /*
   ** this function is called when msgs are placed on a private queue
   ** in test4
   ** when a msg is placed on private queue, ready the owner of the queue
   */
   _task_ready(task_td);
}


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : test4_task
* Comments     : 
*    Task required for test4
*
*END*----------------------------------------------------------------------*/

void test4_task(uint32_t parameter)
{
   TEST4_MSG_STRUCT_PTR   test4_ptr;
   _queue_id              test4_qid;
   bool                result;

   test4_qid = _msgq_open(TEST_SYSTEM_QUEUE+1,0);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, test4_qid != 0, "Test #4: Testing 4.7: Open the message queue")
   if (test4_qid == 0) {
      _task_set_error(MQX_OK);
      return;
   }
   test4_ptr = _msg_alloc_system(sizeof(TEST4_MSG_STRUCT));
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, test4_ptr != NULL, "Test #4: Testing 4.8: _msg_alloc_system() operation")
   if (test4_ptr == NULL) {
      _task_set_error(MQX_OK);
      return;
   }   
   test4_ptr->HEADER.TARGET_QID = (_queue_id)parameter;
   test4_ptr->HEADER.SOURCE_QID = test4_qid;
   test4_ptr->DATA1 = 10;
   test4_ptr->DATA2 = 20;
   test4_ptr->DATA3 = 'd';
   test4_ptr->DATA4 = 30;
   test4_ptr->DATA5 = 40;
   result = _msgq_send(test4_ptr);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result != FALSE , "Test #4: Testing 4.9: Send the message to the message queue")
   if (result == FALSE) {
      _task_set_error(MQX_OK);
      return;
   }   

   test4_ptr = _msgq_receive_ticks(test4_qid, 20);

   /* if didn't receive msg then msg swap on header failed */
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, test4_ptr != NULL, "Test #4: Testing 4.10: If didn't receive msg then msg swap on header")
   if (test4_ptr == NULL) {
      _task_set_error(MQX_OK);
      return;
   }
   
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, ((test4_ptr->DATA1 == 10) && (test4_ptr->DATA2 == 20) &&(test4_ptr->DATA3 == 'd') && (test4_ptr->DATA4 == 30) && (test4_ptr->DATA5 == 40)), "Test #4: Testing 4.11: _msg_swap_endian_data operation")
   if ((test4_ptr->DATA1 == 10) && (test4_ptr->DATA2 == 20) &&
      (test4_ptr->DATA3 == 'd') && (test4_ptr->DATA4 == 30) &&
      (test4_ptr->DATA5 == 40))
   {
      /* swap succeeded */
      test4_ptr->HEADER.TARGET_QID = (_queue_id)parameter;
      result = _msgq_send(test4_ptr);
      EU_ASSERT_REF_TC_MSG(tc_4_main_task, result != FALSE , "Test #4: Testing 4.12: Send the message to the message queue")
      if (result == FALSE) {
         _task_set_error(MQX_OK);
      }
   } else {
      /* error occurred */
      error_found = TRUE;
   }
   
   result = _msgq_close(test4_qid);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result != FALSE , "Test #4: Testing 4.13: Close the message queue")
   if (result == FALSE) {
      _task_set_error(MQX_OK);
   }
}


/******************** Begin Testcases **********************/

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Testing _msg_create_component primitive
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   _mqx_uint    result;
   bool      bool_result;
   void        *queue_error;
   void        *msg_error;
   void        *pool_error;
   _pool_id     pool[11];
   _queue_id    queue[11];
   
   error_found = FALSE;

   /*
   ** create the message component
   ** verify the integrity of msg pools and msg queues
   */
   result = _msg_create_component();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.1: Create the message component")
   if (result != MQX_OK) {
      eunit_test_stop();
      _mqx_exit(1L);  
   } 
   result = _msgpool_test(&pool_error, &msg_error);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.2: Test message pool operation")
   result = _msgq_test(&queue_error, &msg_error);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.3: Test all messages open in all message queues")
   /* 
   ** create a msg pool, create a msg queue
   ** verify the integrity of msg pools and msg queues
   */
   pool[0] = _msgpool_create(sizeof(MESSAGE_HEADER_STRUCT),14,0,0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, pool[0] != MSGPOOL_NULL_POOL_ID, "Test #1: Testing 1.4: Create message pool")
   if (pool[0] == MSGPOOL_NULL_POOL_ID) {
      _task_set_error(MQX_OK);
   }
   queue[0] = _msgq_open(0,0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, queue[0] != 0, "Test #1: Testing 1.5: open the message queue operation")
   if (queue[0] == 0) {
      _task_set_error(MQX_OK);
   }
   result = _msgpool_test(&pool_error,&msg_error);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.6: Test the message pool")
   result = _msgq_test(&queue_error, &msg_error);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.7: Test message queue operation")
   /*
   ** close msg pool, close msg queue
   ** verify the integrity of the msg pools and msg queues
   */
   result = _msgpool_destroy(pool[0]);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.8: Destroy the message pool")
   bool_result = _msgq_close(queue[0]);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, bool_result != FALSE, "Test #1: Testing 1.9:Close the message queue ")
   if (bool_result == FALSE) {
      _task_set_error(MQX_OK);
   }
   result = _msgpool_test(&pool_error,&msg_error);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.10: Test the message pool")
   result = _msgq_test(&queue_error, &msg_error);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.11: Test the message queue")
   /*
   ** create the msg component again
   */
   result = _msg_create_component();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.12: Create the message component again")
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Testing _msgpool_create and _msgpool_create_system primitives
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
   _mqx_uint    result;
   bool      bool_result;
   _mqx_uint    new_count;
   _mqx_uint    count;
   _mqx_uint    i;
   _mqx_uint    hehe;
   void        *msg_error;
   void        *pool_error;
   _pool_id     pool[11];
   void        *test_msg[11];
   uint16_t      pool_sizes[] = {sizeof(MESSAGE_HEADER_STRUCT),50,75,100,150};
   uint16_t      grow_num[] = {2,4,0,0,0};
   uint16_t      grow_limit[] = {4,0,0,0};
   
   /*
   **  creating the maximum no. of message pools
   **  calls: _msgpool_create,_msgpool_create_system,_msg_alloc, _msg_alloc_system,
   **         _msg_free, _msg_available, msgpool_destroy, _msgpool_test    
   */
   error_found = FALSE;
   /*
   ** create 5 private pools, 5 system queues
   ** try to create a 6th private pool, should result in an error because
   ** max msg pools is 10.
   */
   for (i=0; i < 6; i++) {
      pool[i] = _msgpool_create(pool_sizes[i],2,grow_num[i],grow_limit[i]);
      if (pool[i] == MSGPOOL_NULL_POOL_ID) {
         result = _task_get_error();
         _task_set_error(MQX_OK);
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, i == 5, "Test #2: Testing 2.1: Create message pool")
         if (i != 5) { 
            error_found = TRUE;

         } else
            break;
      } else{ 
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, i != 5 , "Test #2: Testing 2.2: Create 6th private message pool should have failed")
         if (i == 5) {
         error_found = TRUE;
         break;
        }
      }
      bool_result = _msgpool_create_system(pool_sizes[i],2,grow_num[i],grow_limit[i]);
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result != FALSE, "Test #2: Testing 2.3: Create system message pool")
      if (bool_result == FALSE) {
         _task_set_error(MQX_OK);
      }
   }   
   /*
   ** force first private pool to grow, by allocating all messages
   ** deallocate messages
   ** destroy the pool
   */
   for (i = 0; i < grow_limit[0]+1; i++) {
      test_msg[i] = _msg_alloc(pool[0]);
      if (test_msg[i] == NULL) {
         result = _task_get_error();
         _task_set_error(MQX_OK);
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, i == grow_limit[0], "Test #2: Testing 2.4: _msg_alloc operation")
      } else {
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, i != grow_limit[0] , "Test #2: Testing 2.5: _msg_alloc() operation")
        if (i == grow_limit[0]) {
         error_found = TRUE;
        } 
      }/* endif */
   } /* endfor */ 
   for (i=0; i < grow_limit[0]-1; i++) {
      _msg_free(test_msg[i]);
      result = _task_get_error();
      _task_set_error(MQX_OK);
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.6: Free all messages")
   }

   result = _msgpool_destroy(pool[0]);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result != MQX_OK, "Test #2: Testing 2.7: Destroy the message pool should have failed")
   _msg_free(test_msg[grow_limit[0]-1]);
   result = _task_get_error();
   _task_set_error(MQX_OK);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.8: Free message queue operation")
   /*
   ** use _msg_available to verify that all msgs in pool have been freed
   */
   count = _msg_available(pool[0]);
   result = _task_get_error();
   _task_set_error(MQX_OK);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, count == grow_limit[0], "Test #2: Testing 2.9: verify all messages in pool have been freed")
   if (count != grow_limit[0]) {
      /* not all messages free */
      error_found = TRUE;     
   } else {
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK , "Test #2: Testing 2.10: _msg_available operation")
      if (result != MQX_OK) {
         error_found = TRUE;
         }
    }
   /*
   ** destroy the message pool
   */
   result = _msgpool_destroy(pool[0]);
   hehe = _task_get_error();
   
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.11: Destroy the message pool")

   /*
   ** force system pool to grow
   ** check deallocation of messages, and message available
   */


   for (i = 0; i < grow_limit[0]+2; i++) {
      test_msg[i] = _msg_alloc_system(pool_sizes[0]);
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, test_msg[i] != NULL, "Test #2: Testing 2.12: _msg_alloc_system operation")
       if (test_msg[i] == NULL) {
         _task_set_error(MQX_OK);
       } /* endif */
   } /* endfor */ 
   /*
   ** get the current number of available msgs in system pools
   */
   count = _msg_available((_pool_id)0);
   result = _task_get_error();
   _task_set_error(MQX_OK);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.13: Get the current number of available messaages in system pools")
   /*
   ** free the msgs allocated from system pool
   */
   for (i=0; i < grow_limit[0]+2; i++) {
      _msg_free(test_msg[i]);
      result = _task_get_error();
      _task_set_error(MQX_OK);
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.14: Free the messages allocated from system pool")
   }
   /*
   ** verify that all msgs in system pools are free
   */
   new_count = _msg_available((_pool_id)0);
   result = _task_get_error();
   _task_set_error(MQX_OK);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (new_count == count + grow_limit[0]+2), "Test #2: Testing 2.15: Verify that all messages in system pools are free")
   if (new_count != count + grow_limit[0]+2) {
      /* not all messages free */
   } else {
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2: Testing 2.16: _msg_available operation failed") 
   }
   /*
   ** create a new private pool, destroy pool
   */
   pool[0] = _msgpool_create(sizeof(MESSAGE_HEADER_STRUCT),2,0,0);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, pool[0] != MSGPOOL_NULL_POOL_ID, "Test #2: Testing 2.17: Create a new private pool")
   if (pool[0] == MSGPOOL_NULL_POOL_ID) {
      _task_set_error(MQX_OK);

   } else {
      result = _msgpool_destroy(pool[0]);
      EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.18: Destroy the message pool")
   }

   /*
   ** verify integrity of msg pool
   */
   result = _msgpool_test(&pool_error,&msg_error);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.19: Test message pool")
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Testing system message queue primitives
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
   _mqx_uint    result;
   _mqx_uint    count;
   _mqx_uint    i;
   void        *queue_error;
   void        *msg_error;
   _queue_id    system_qid;
   MESSAGE_HEADER_STRUCT_PTR    msg_ptr;
   MESSAGE_HEADER_STRUCT_PTR    msg2_ptr;
   void (_CODE_PTR_ not_function)(void *);
   void        *not_data;
   /*
   **  calls: _msgq_get_id,_msgq_open_system, _msg_alloc_system,
   **         _msg_free, _msgq_poll, msgq_get_count, _msgq_test,
   **         _msgq_get_notification_function, _msgq_peek, _msgq_send,
   **         _msgq_set_notification_function, _msgq_close    
   */
   global_count = 0;
   error_found = FALSE;
   /*
   ** open a system queue, verify the setting of the notification 
   ** function and notification data 
   */
   system_qid = _msgq_open_system((_queue_number)TEST_SYSTEM_QUEUE,0,
                    queue_not_function,(void *)TEST_SYSTEM_QUEUE);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, system_qid != 0, "Test #3: Testing 3.1: Open a system message queue")
   if (system_qid == 0) {
      _task_set_error(MQX_OK);
   }
   result = _msgq_get_notification_function(system_qid,&not_function,&not_data);
   /* check result */
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.2: _msgq_get_notification_function operation")   
   /* check value of notification function */
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, not_function == queue_not_function, "Test #3: Testing 3.3: Check value of notification function")
   /* check value of notification data */
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, not_data == (void *)TEST_SYSTEM_QUEUE, "Test #3: Testing 3.4: Check value of notification data")
   if (not_data != (void *)TEST_SYSTEM_QUEUE) {
      error_found = TRUE;
   } 
   /*
   ** allocate three messages, send msgs to system queue
   ** notification queue increments global_count for each read msg
   */
   for (i=0; i < 3; i++) {
      msg_ptr = _msg_alloc_system(sizeof(MESSAGE_HEADER_STRUCT));
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr != NULL, "Test #3: Testing 3.5: Allocate message operation")
      if (msg_ptr == NULL) {
         _task_set_error(MQX_OK);
         break;
      }/*end if */
      msg_ptr->TARGET_QID = _msgq_get_id((_processor_number)0,
                                          (_queue_id)TEST_SYSTEM_QUEUE);
      msg_ptr->SOURCE_QID = msg_ptr->TARGET_QID;
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr->SOURCE_QID == system_qid, "Test #3: Testing 3.6: Verify _msgq_get_id operation")
      result = _msgq_send(msg_ptr);
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, result != FALSE, "Test #3: Testing 3.7: Send message to system queue")
      if (result == FALSE) {
         _task_set_error(MQX_OK);
         break;
      }/*end if */
    } /* endfor */
    /*
    ** check that messages were processed by notification function 
    */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, global_count == 3, "Test #3: Testing 3.8: Check that messages were processed by notification function")
    /*
    ** remove the notification function from the system queue
    */
    not_function = _msgq_set_notification_function(system_qid,NULL,NULL);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, not_function != NULL, "Test #3: Testing 3.9: _msgq_set_notification_function operation")
    if (not_function == NULL) {
       _task_set_error(MQX_OK);
    } else{
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, not_function == queue_not_function, "Test #3: Testing 3.10: Check _msgq_set_notification_function returned correct function address")
        if (not_function != queue_not_function) {
            error_found = TRUE;
        }
     }
    /*
    ** get 3 msgs
    ** send them to the system queue
    */
    for (i=0; i < 3; i++) {
      msg_ptr = _msg_alloc_system(sizeof(MESSAGE_HEADER_STRUCT));
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr != NULL, "Test #3: Testing 3.11: Allocate message operation")
      if (msg_ptr == NULL) {
         _task_set_error(MQX_OK);
         break;
      }/*end if */
      msg_ptr->TARGET_QID = _msgq_get_id((_processor_number)0,
                                          (_queue_id)TEST_SYSTEM_QUEUE);
      msg_ptr->SOURCE_QID = msg_ptr->TARGET_QID;
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr->SOURCE_QID == system_qid, "Test #3: Testing 3.12: _msgq_get_id operation")
      result = _msgq_send(msg_ptr);
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, result != FALSE , "Test #3: Testing 3.14: Send message to system queue")
      if (result == FALSE) {
         _task_set_error(MQX_OK);
         break;
      }/*end if */
    } /* endfor */
    /*
    ** verify that 3 msgs have been received on system queue
    */
    count = _msgq_get_count(system_qid);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, count == 3, "Test #3: Testing 3.15: Verify that 3 msgs have been received on system queue")
    if (count != 3) {
       _task_set_error(MQX_OK);
    } 
    /*
    ** use peek to verify msg waiting
    ** use poll to dequeue msg from system queue
    ** free received msg
    */
    for (i=0; i < 3; i++) {
       msg_ptr = _msgq_peek(system_qid);
       EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg_ptr != NULL, "Test #3: Testing 3.16: Use peek to verify message waiting")
       if (msg_ptr == NULL) {
          _task_set_error(MQX_OK);
          break;
       }
       msg2_ptr = _msgq_poll(system_qid);
       EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg2_ptr != NULL, "Test #3: Testing 3.17: Use poll to dequeue message from system queue")
       if (msg2_ptr == NULL) {
          _task_set_error(MQX_OK);
          break;
       }
       
       EU_ASSERT_REF_TC_MSG(tc_3_main_task, msg2_ptr == msg_ptr, "Test #3: Testing 3.18: Compare returned message of _msgq_poll with msgq_peek")
       _msg_free(msg2_ptr);
       result = _task_get_error();
       EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.19: Free message operation")
       if (result != MQX_OK) {
          _task_set_error(MQX_OK);
       }
   }
   /*
   ** poll system queue again
   ** expect to receive an error code indicating no msgs available
   ** close queue
   ** verify integrity of msg queues using test primitive
   */
   msg_ptr = _msgq_poll(system_qid);
   result = _task_get_error();
   _task_set_error(MQX_OK);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MSGQ_MESSAGE_NOT_AVAILABLE, "Test #3: Testing 3.20: _msgq_poll should have returned MSGQ_MESSAGE_NOT_AVAILABLE")
   result = _msgq_close(system_qid);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result != FALSE , "Test #3: Testing 3.21: Close message queue operation")
   if (result == FALSE) {
      _task_set_error(MQX_OK);
   }
   result = _msgq_test(&queue_error, &msg_error);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.22: Test message queue operation")
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Testing private message queue primitives
*
*END*---------------------------------------------------------------------*/

void tc_4_main_task(void)
{
   _mqx_uint    result;
   void        *queue_error;
   void        *msg_error;
   _queue_id    private_qid;
   TEST4_MSG_STRUCT_PTR         test4_msg_ptr;
   void (_CODE_PTR_ not_function)(void *);
   /*
   **  calls: _msgq_get_id,_msgq_open, _msg_alloc,
   **         _msg_free, _msgq_receive,  _msgq_test,
   **         _msgq_get_notification_function,  _msgq_send,
   **         _msgq_set_notification_function, _msgq_close, 
   **         _msg_swap_endian_data, _msg_swap_endian_header    
   */
   error_found = FALSE;
   /*
   ** open a private queue, with unlimited max entries
   ** set a notification function for this queue
   */
   private_qid = _msgq_open((_queue_number)TEST_SYSTEM_QUEUE,0);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, private_qid != 0, "Test #4: Testing 4.1: Open the private message queue")
   if (private_qid == 0) {
      _task_set_error(MQX_OK);
   }
   not_function = _msgq_set_notification_function(private_qid,test4_not,
                                  (void *)_task_get_td(_task_get_id()));
   result = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.2: Set a notification function for this queue")
   if (result != MQX_OK) {
      _task_set_error(MQX_OK);
   }
   /*
   ** create TEST4_TASK to send msgs to this queue
   ** this task is blocked until it is readied by the notification function 
   ** receive msg from queue
   */
   _task_create(0,TEST4_TASK,(uint32_t)private_qid);
   _task_block();
   test4_msg_ptr = _msgq_receive(private_qid,0);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, test4_msg_ptr != NULL, "Test #4: Testing 4.3: Receive message from queue")
   if (test4_msg_ptr == NULL) {
      _task_set_error(MQX_OK);
   } else {
      /* swap header and data of received message
      ** swapping twice makes it the same as it was received
      */
      _msg_swap_endian_header((MESSAGE_HEADER_STRUCT_PTR)test4_msg_ptr);
      _msg_swap_endian_data((unsigned char *)&definition,(MESSAGE_HEADER_STRUCT_PTR)test4_msg_ptr);
      _msg_swap_endian_header((MESSAGE_HEADER_STRUCT_PTR)test4_msg_ptr);
      _msg_swap_endian_data((unsigned char *)&definition,(MESSAGE_HEADER_STRUCT_PTR)test4_msg_ptr);
      test4_msg_ptr->HEADER.TARGET_QID = test4_msg_ptr->HEADER.SOURCE_QID;
      test4_msg_ptr->HEADER.SOURCE_QID = private_qid;
      result = _msgq_send(test4_msg_ptr);
      EU_ASSERT_REF_TC_MSG(tc_4_main_task, result != FALSE, "Test #4: Testing 4.4: Send message to the message queue")
      if (result == FALSE) {
         _task_set_error(MQX_OK);
      } else
         _task_block();
   }
   result = _msgq_close(private_qid);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result != FALSE , "Test #4: Testing 4.5: Close the message queue")
   if (result == FALSE) {
      _task_set_error(MQX_OK);
   }
   result = _msgq_test(&queue_error, &msg_error);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.6: Test the message queue")
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Testing private message queue primitives
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
   _mqx_uint    result;
   _mqx_uint    new_count;
   _mqx_uint    count;
   _mqx_uint    i;
   _queue_id    private_qid;
   MESSAGE_HEADER_STRUCT_PTR    msg_ptr;

   /*
   **  calls: _msgq_open, _msg_alloc,
   **         _msg_free, _msgq_receive,  _msgq_test,
   **         _msgq_send, _msgq_close, 
   */
   error_found = FALSE;
   /*
   ** open private queue with max entries of 10
   */
   private_qid = _msgq_open((_queue_number)TEST_SYSTEM_QUEUE,10);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, private_qid != 0, "Test #5: Testing 5.1: Open the message queue")
   if (private_qid == 0) {
      _task_set_error(MQX_OK);
   }
   /* fill queue with messages */
   for (i=0; i< 10; i++) {
      msg_ptr = _msg_alloc_system(sizeof(MESSAGE_HEADER_STRUCT));
      EU_ASSERT_REF_TC_MSG(tc_5_main_task, msg_ptr != NULL, "Test #5: Testing 5.2: Allocate a message from a system message pool")
      if (msg_ptr == NULL) {
         _task_set_error(MQX_OK);
         break;
      } 
      msg_ptr->TARGET_QID = private_qid;
      result = _msgq_send(msg_ptr);
      EU_ASSERT_REF_TC_MSG(tc_5_main_task, result != FALSE , "Test #5: Testing 5.3: Send message to the message queue")
      if (result == FALSE) {
         _task_set_error(MQX_OK);
      }
   }
   /*
   ** verify no. entries on queue
   ** try to send another msg, verify that queue full error occurs
   */
   count = _msgq_get_count(private_qid);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, count == 10, "Test #5: Testing 5.4: Get the number of messages in message queue")
   if (count != 10) {
      _task_set_error(MQX_OK);
   }
   msg_ptr = _msg_alloc_system(sizeof(MESSAGE_HEADER_STRUCT));
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, msg_ptr != NULL, "Test #5: Testing 5.5:  Allocate a message from a system message pool")
   if (msg_ptr == NULL) {
      _task_set_error(MQX_OK);
   } else {
      msg_ptr->TARGET_QID = private_qid;
      if (_msgq_send(msg_ptr) == FALSE) {
         result = _task_get_error();
         _task_set_error(MQX_OK);
         EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MSGQ_QUEUE_FULL, "Test #5: Testing 5.6: _task_get_error should have return MSGQ_QUEUE_FULL")
      }
   }
   /*
   ** receive a message   from queue, new count should indicate one less entry on queue
   */
   msg_ptr = _msgq_receive_ticks(private_qid,2);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, msg_ptr != NULL, "Test #5: Testing 5.7: Receive a message from queue")
   if (msg_ptr == NULL) {
      _task_set_error(MQX_OK);
   } else {
      new_count = _msgq_get_count(private_qid);
      EU_ASSERT_REF_TC_MSG(tc_5_main_task, new_count == 9, "Test #5: Testing 5.8: Get the number of messages in message queue")
      if (new_count != 9) {
         _task_set_error(MQX_OK);
      }
   }
   /*
   ** verify that another msg can be sent to queue and that count 
   ** indicates that the queue is full again 
   */
   result = _msgq_send(msg_ptr);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, result != FALSE , "Test #5: Testing 5.9: Verify that another msg can be sent to queue and that count indicates that the queue is full again")
   if (result == FALSE) {
      _task_set_error(MQX_OK);
   }
   count = _msgq_get_count(private_qid);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, count == 10, "Test #5: Testing 5.10: Get the number of messages in message queue")
   if (count != 10) {
      _task_set_error(MQX_OK);
   }
   /*
   ** use peek to verify that a msg is available
   ** use msgq_receive to receive all entries on queue
   ** verify that 10 entries were read
   */
   count = 0;
   msg_ptr = _msgq_peek(private_qid);
   
   while (msg_ptr) {
      msg_ptr = _msgq_receive_ticks(private_qid,2);
      EU_ASSERT_REF_TC_MSG(tc_5_main_task, msg_ptr != NULL , "Test #5: Testing 5.11: Check msg_ptr")
      if (msg_ptr == NULL) {
         break;
      } else {
         _msg_free(msg_ptr);
         result = _task_get_error();
         _task_set_error(MQX_OK);
         EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.12: Free the message")
         count++;
         msg_ptr = _msgq_peek(private_qid);
      }
   }
   
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, count == 10, "Test #5: Testing 5.13: Verify that 10 entries were read, count should be 10")
   if (count != 10) {
      _task_set_error(MQX_OK);
   } else {
      _task_set_error(MQX_OK);
   }
   /* close queue */
   result = _msgq_close(private_qid);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, result != FALSE , "Test #5: Testing 5.14: Close the message queue")
   if (result == FALSE) {
      _task_set_error(MQX_OK);
   }
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Testing send priority and urgent messages
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
   _mqx_uint    result;
   bool      correct_msg;
   _mqx_uint    i;
   _queue_id    private_qid;
   TEST4_MSG_STRUCT_PTR         test4_msg_ptr;
   /*
   **  calls: _msgq_open, _msg_alloc,
   **         _msg_free, _msgq_receive,  _msgq_send_priority,
   **         _msgq_send, _msgq_close, _msgq_send_urgent
   */
   error_found = FALSE;
   private_qid = _msgq_open((_queue_number)TEST_SYSTEM_QUEUE,10);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, private_qid != 0, "Test #6: Testing 6.1: Open the message queue")
   if (private_qid == 0) {
      _task_set_error(MQX_OK);
   }
   /* send 2 normal messages to queue  */
   for (i=0; i< 2; i++) {
      test4_msg_ptr = _msg_alloc_system(sizeof(TEST4_MSG_STRUCT));
      EU_ASSERT_REF_TC_MSG(tc_6_main_task, test4_msg_ptr != NULL, "Test #6: Testing 6.2: Allocate a message from a system message pool")
      if (test4_msg_ptr == NULL) {
         _task_set_error(MQX_OK);
         break;
      } 
      test4_msg_ptr->HEADER.TARGET_QID = private_qid;
      test4_msg_ptr->DATA1 = 0;
      result = _msgq_send(test4_msg_ptr);
      EU_ASSERT_REF_TC_MSG(tc_6_main_task, result != FALSE, "Test #6: Testing 6.3: Send message to the message queue")
      if (result == FALSE) {
         _task_set_error(MQX_OK);
      }
   }
   /* send msg with priority 5 */
   test4_msg_ptr = _msg_alloc_system(sizeof(TEST4_MSG_STRUCT));
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, test4_msg_ptr != NULL, "Test #6: Testing 6.4: Allocate a message from a system message pool")
   if (test4_msg_ptr == NULL) {
      _task_set_error(MQX_OK);
   } 
   test4_msg_ptr->HEADER.TARGET_QID = private_qid;
   test4_msg_ptr->DATA1 = 5;
   result = _msgq_send_priority(test4_msg_ptr,5);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, result != FALSE , "Test #6: Testing 6.5: _msgq_send_priority operation")
   if (result == FALSE) {
      _task_set_error(MQX_OK);
   }
   /* send msg with priority 3 */
   test4_msg_ptr = _msg_alloc_system(sizeof(TEST4_MSG_STRUCT));
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, test4_msg_ptr != NULL, "Test #6: Testing 6.6: Allocate a message from a system message pool")
   if (test4_msg_ptr == NULL) {
      _task_set_error(MQX_OK);
   } 
   test4_msg_ptr->HEADER.TARGET_QID = private_qid;
   test4_msg_ptr->DATA1 = 3;
   result = _msgq_send_priority(test4_msg_ptr,3);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, result != FALSE , "Test #6: Testing 6.7: Send message with priority 3")
   if (result == FALSE) {
      _task_set_error(MQX_OK);
   }
   /* send urgent message      */
   test4_msg_ptr = _msg_alloc_system(sizeof(TEST4_MSG_STRUCT));
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, test4_msg_ptr != NULL, "Test #6: Testing 6.8: Allocate a message from a system message pool")
   if (test4_msg_ptr == NULL) {
      _task_set_error(MQX_OK);
   } 
   test4_msg_ptr->HEADER.TARGET_QID = private_qid;
   test4_msg_ptr->DATA1 = 10;
   result = _msgq_send_urgent(test4_msg_ptr);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, result != FALSE , "Test #6: Testing 6.9: Send urgent message")
   if (result == FALSE) {
      _task_set_error(MQX_OK);
   }
   /*
   ** verify that urgent msg is at head of queue
   ** followed by priority 5 msg and priority 3 msg
   ** followed by two normal msgs
   */
   for (i=0; i < 5; i++) {
      correct_msg = FALSE;
      test4_msg_ptr = _msgq_receive(private_qid,10);
      EU_ASSERT_REF_TC_MSG(tc_6_main_task, test4_msg_ptr != NULL, "Test #6: Testing 6.10: _msgq_receive() operation")
      if (test4_msg_ptr == NULL) {
         _task_set_error(MQX_OK);
      }else {
         switch  (i) {
         case 0: if (test4_msg_ptr->DATA1 == 10) correct_msg = TRUE;
            break;
         case 1: if (test4_msg_ptr->DATA1 == 5) correct_msg = TRUE;
            break;
         case 2: if (test4_msg_ptr->DATA1 == 3) correct_msg = TRUE;
            break;
         case 3:
         case 4: if (test4_msg_ptr->DATA1 == 0) correct_msg = TRUE;
            break;
         } 
         
         EU_ASSERT_REF_TC_MSG(tc_6_main_task, correct_msg != FALSE, "Test #6: Testing 6.11: Verify send priority or send urgent")
         if (correct_msg == FALSE) {
            break;
         }
         _msg_free(test4_msg_ptr);
         result = _task_get_error();
         _task_set_error(MQX_OK);
         EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.12: Free message operation")

      }
   }
   result = _msgq_close(private_qid);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, result != FALSE , "Test #6: Testing 6.13: Close the message queue")
   if (result == FALSE) {
      _task_set_error(MQX_OK);
   }
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_7_main_task
* Comments     : TEST #7: Testing send queue messages
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
   _mqx_uint    result;
   _mqx_uint    count;
   _queue_id    private_qid;
   TEST4_MSG_STRUCT_PTR         test4_msg_ptr;
   /*
   **  calls: _msgq_open, _msg_alloc,
   **         _msg_free, _msgq_receive,  _msgq_send_queue,
   **         _msgq_close
   */
   error_found = FALSE;
   /*
   ** open a private
   ** send a message to the queue using _msgq_send_queue
   ** verify that msg is received and that data within msg is correct
   */
   private_qid = _msgq_open((_queue_number)TEST_SYSTEM_QUEUE,10);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, private_qid != 0, "Test #7: Testing 7.1: Open the message queue")
   if (private_qid == 0) {
      _task_set_error(MQX_OK);
   }
   test4_msg_ptr = _msg_alloc_system(sizeof(TEST4_MSG_STRUCT));
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, test4_msg_ptr != NULL, "Test #7: Testing 7.2: Allocate a message from a system message pool")
   if (test4_msg_ptr == NULL) {
      _task_set_error(MQX_OK);
   } else {
      test4_msg_ptr->HEADER.TARGET_QID = 0;
      test4_msg_ptr->DATA1 = 0xd0d0d0d0;
      result = _msgq_send_queue(test4_msg_ptr,private_qid);
      EU_ASSERT_REF_TC_MSG(tc_7_main_task, result != FALSE , "Test #7: Testing 7.3: Send the message directly to the private message queue")
      if (result == FALSE) {
         _task_set_error(MQX_OK);
      }
      count = _msgq_get_count(private_qid);    
      EU_ASSERT_REF_TC_MSG(tc_7_main_task, count == 1, "Test #7: Testing 7.4: Get the number of messages in message queue count should be 1")
      if (count != 1) {
         _task_set_error(MQX_OK);
      }               

      test4_msg_ptr = _msgq_receive_ticks(private_qid,2);
      EU_ASSERT_REF_TC_MSG(tc_7_main_task, test4_msg_ptr != NULL, "Test #7: Testing 7.5: _msgq_receive_ticks() operation")
      if (test4_msg_ptr == NULL) {
         _task_set_error(MQX_OK);
      } else {
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, test4_msg_ptr->DATA1 == 0xd0d0d0d0, "Test #7: Testing 7.6: Verify send queue")
        _msg_free(test4_msg_ptr);
        result = _task_get_error();
        _task_set_error(MQX_OK);
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.7: Free message operation")
     }
     result = _msgq_close(private_qid);
     EU_ASSERT_REF_TC_MSG(tc_7_main_task, result != FALSE , "Test #7: Testing 7.8: Close the message queue")
     if (result == FALSE) {
        _task_set_error(MQX_OK);
     }
  }
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_8_main_task
* Comments     : TEST #8: Testing send broadcast messages
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
   _mqx_uint    result;
   _mqx_uint    count;
   _mqx_uint    i;
   _queue_id    queue[11];
   TEST4_MSG_STRUCT_PTR         test4_msg_ptr;
   /*
   **  calls: _msgq_open, _msg_alloc,
   **         _msg_free, _msgq_receive,  _msgq_send_broadcast,
   **         _msgq_close
   */
   error_found = FALSE;
   /* open four private queues to receive broadcast msg */
   for (i=0; i < 4; i++) {
      queue[i] = _msgq_open((_queue_number)TEST_SYSTEM_QUEUE+(_queue_number)i,10);
      EU_ASSERT_REF_TC_MSG(tc_8_main_task, queue[i] != 0, "Test #8: Testing 8.1: Open four private queues to receive broadcast message")
      if (queue[i] == 0) {
         _task_set_error(MQX_OK);
      }
   }
   /* 
   ** open a system queue
   */
   queue[i] =
      _msgq_open_system((_queue_number)TEST_SYSTEM_QUEUE+(_queue_number)i,10,
      NULL,NULL);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, queue[i] != 0, "Test #8: Testing 8.2: Open a system queue")
   if (queue[i] == 0) {
      _task_set_error(MQX_OK);
   }
   /* close queue[0]
   */
   _msgq_close(queue[0]);
   queue[i+1] = 0;
   test4_msg_ptr = _msg_alloc_system(sizeof(TEST4_MSG_STRUCT));
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, test4_msg_ptr != NULL, "Test #8: Testing 8.3: _msg_alloc_system() operation")
   if (test4_msg_ptr == NULL) {
      _task_set_error(MQX_OK);
   } else {
      test4_msg_ptr->HEADER.TARGET_QID = 0;
      test4_msg_ptr->DATA1 = 0xd0d0d0d0;
      /*
      ** send msg to all five created queues
      ** one will fail because the queue was closed
      */
      count = _msgq_send_broadcast(test4_msg_ptr,(_queue_id *)&queue,MSGPOOL_NULL_POOL_ID);
      result = _task_get_error();
      _task_set_error(MQX_OK);
      EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.4: Send message to all five created queues")
      
      EU_ASSERT_REF_TC_MSG(tc_8_main_task, count == 4, "Test #8: Testing 8.5: Check _msgq_send_broadcast one should be failed (count == 4)")
      /*
      ** read from 3 remaining private queues, verify that broadcast msg was received
      */
      for (i=1; i < 4; i++) {
         test4_msg_ptr = _msgq_receive_ticks(queue[i],2);
         EU_ASSERT_REF_TC_MSG(tc_8_main_task, test4_msg_ptr != NULL, "Test #8: Testing 8.6: Read from 3 remaining private queues")
         if (test4_msg_ptr == NULL) {
            _task_set_error(MQX_OK);
         } else {
            EU_ASSERT_REF_TC_MSG(tc_8_main_task, test4_msg_ptr->DATA1 == 0xd0d0d0d0, "Test #8: Testing 8.7: Send boradcast operation")
            if (test4_msg_ptr->DATA1 != 0xd0d0d0d0) {
               error_found = TRUE;
            }
            _msg_free(test4_msg_ptr);
            result = _task_get_error();
            _task_set_error(MQX_OK);
            EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.8: Free message operation")
         }
      }
      /*
      ** read from system queue
      */
      test4_msg_ptr = _msgq_poll(queue[i]);
      EU_ASSERT_REF_TC_MSG(tc_8_main_task, test4_msg_ptr != NULL, "Test #8: Testing 8.9: Poll the message queue for a message")
      if (test4_msg_ptr == NULL) {
         _task_set_error(MQX_OK);
      } else {
         EU_ASSERT_REF_TC_MSG(tc_8_main_task, test4_msg_ptr->DATA1 == 0xd0d0d0d0, "Test #8: Testing 8.10: Send broadcast operation")
         _msg_free(test4_msg_ptr);
         result = _task_get_error();
         _task_set_error(MQX_OK);
         EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.11: Free message operation")
      }
   }
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1 - Testing _msg_create_component primitive"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2 - Testing _msgpool_create and _msgpool_create_system primitives"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3 - Testing system message queue primitives"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4 - Testing private message queue primitives"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST # 5 - Testing private message queue primitives with a maximum size"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST # 6 - Testing send priority and urgent messages"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST # 7 - Testing send queue messages"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST # 8 - Testing send broadcast messages")
EU_TEST_SUITE_END(suite_1)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, " - MQX Message Component Test Suite, 8 Tests")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : main_task
* Comments     : 
*    This is the main task to the MQX Kernel program
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t param
      /* [IN] The MQX task parameter */
   )
{ /* Body */

   /*
   ** The following code tests the  message_create_component function  
   ** calls:  _msg_create_component, _msgpool_test, _msgq_test,
   **         _msgpool_create, _msgq_open, _msgpool_destroy, _msgq_close
   */
   
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
   
   _mqx_exit(0);
} /* endbody */



/* EOF */
