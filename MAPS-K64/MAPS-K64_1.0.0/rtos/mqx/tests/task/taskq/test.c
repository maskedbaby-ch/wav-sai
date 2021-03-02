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
* $Version : 3.8.8.0$
* $Date    : Apr-5-2012$
*
* Comments:
*
*   This file contains code for the MQX Task Queue component verification
*   program.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <event.h>
// #include <log.h>
// #include <klog.h>
//#include <test_module.h>

#include "EUnit.h"
#include "eunit_crit_secs.h"

#include "test.h"
#include "util.h"
#define FILE_BASENAME test
/* #define KLOG_ON */

_mqx_uint    global_count[10];
_mqx_uint    check_order_of_exec,current_test;
void      *tqueues[5];
void        *current_tq;

extern void tc_1_main_task(void);
extern void tc_2_main_task(void);
extern void tc_3_main_task(void);
extern void tc_4_main_task(void);
extern void tc_5_main_task(void);
extern void tc_6_main_task(void);
extern void tc_7_main_task(void);
extern void tc_8_main_task(void);
extern void tc_9_main_task(void);

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK,    main_task,  3000,  9,  "main",  MQX_AUTO_START_TASK},
   { TEST_TASK8,   test_task,  1000,  8,  "test",  0},
   { TEST_TASK10,  test_task,  1000,  12, "test",  0},
   { TEST2_TASK,   test2_task, 1000,  10, "test2", 0},
   { 0,            0L,         0,     0,  0,       0}
};

_task_id    test_task_id[10];


_mqx_uint   expect_order[]={0,4,7,1,5,8,2,6,9,3};
_mqx_uint   expect_order2[]={8,0xffff,0xffff,9,0xffff,0xffff,6,0xffff,0xffff,7};
void       *event_ptr;
/******   Test cases body definition      *******/

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test function: _taskq_create
*                         Calls: _taskq_create, _taskq_test, _taskq_get_size, _taskq_destroy
*
*END*---------------------------------------------------------------------*/
 void tc_1_main_task(void)
 {
    _mqx_uint   result;
    _mqx_uint   policy[] = {MQX_TASK_QUEUE_FIFO,MQX_TASK_QUEUE_BY_PRIORITY,
                            MQX_TASK_QUEUE_FIFO,MQX_TASK_QUEUE_BY_PRIORITY, (0x100)};
    _mqx_uint   i;
    
   current_test = 1;
   /*
   ** initialize 5 task queues
   ** verify the integrity of the queue
   */
   for (i=0; i < 5; i++) {
      tqueues[i] = _taskq_create(policy[i]);
      if (i == 4) {
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, tqueues[i] == NULL, "Test #1: Testing 1.5: Create task queue should have failed")
         if (tqueues[i] == NULL) {
            result = _task_get_error();
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_INVALID_PARAMETER, "Test #1: Testing 1.5: Create task queue operation should have returned MQX_INVALID_PARAMETER")
            _task_set_error(MQX_OK);
         }/* Endif */
      } else {
        result = _task_get_error();
        if (i == 0)
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.1: Verify the integrity of the queue")
        else if (i == 1)
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.2: Verify the integrity of the queue")
        else if (i == 2)
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.3: Verify the integrity of the queue")
        else
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.4: Verify the integrity of the queue")
        
        if (result != MQX_OK) {
           _task_set_error(MQX_OK);
        }/* Endif */
      }/* Endif */
   }/* Endfor */
   result = _taskq_destroy(tqueues[3]);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.6: Destroy task queue")

   result = _taskq_destroy(tqueues[2]);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.7: Destroy task queue")

   TEST_queue_test(8);
 }
 
/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_2_3_main_task
* Comments     : create 10 tasks, each task will suspend itself
*                count number of tasks suspended, should be 10
*                resume all, _timeout
*
*                each task will suspend itself, when restarted set its counter 
*
*END*---------------------------------------------------------------------*/
 void tc_2_3_main_task(void)
 {
    _mqx_uint   result;
    _mqx_uint   size;
    _mqx_uint   i;
    
      for (i=0; i < 10; i++) {
         global_count[i] = 0xffff;
         test_task_id[i] = _task_create(0,TEST_TASK8,(uint32_t)i);
         if (current_test == 2) {
             EU_ASSERT_REF_TC_MSG(tc_2_main_task, test_task_id[i] != MQX_NULL_TASK_ID, "Test #2: Testing 2.1: Create task operation")
         } else {
             EU_ASSERT_REF_TC_MSG(tc_3_main_task, test_task_id[i] != MQX_NULL_TASK_ID, "Test #3: Testing 3.1: Create task operation")
         }
         if (test_task_id[i] == MQX_NULL_TASK_ID) {
             _task_set_error(MQX_OK);
         }/* Endif */
      } /*Endfor */
   
      /* check the counters are still set to 0xffff*/
      for (i=0; i < 10; i++) {
         if (current_test == 2) {
             EU_ASSERT_REF_TC_MSG(tc_2_main_task, global_count[i] == 0xffff, "Test #2: Testing 2.2: Check the counters are still set to 0xffff")
         } else {
             EU_ASSERT_REF_TC_MSG(tc_3_main_task, global_count[i] == 0xffff, "Test #3: Testing 3.2: Check the counters are still set to 0xffff")
         }
         if (global_count[i] != 0xffff) {
            break;
         }/* Endif */
      }/* Endfor */
      /* check no. of elements on the task queue */
      size = _taskq_get_value(current_tq);
      if (current_test == 2) {
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, size == 10, "Test #2: Testing 2.3: Check number of elements on the task queue")
      } else {
         EU_ASSERT_REF_TC_MSG(tc_3_main_task, size == 10, "Test #3: Testing 3.3: Check number of elements on the task queue")
      }
      if (size != 10) {
         _task_set_error(MQX_OK);
      }/* Endif */
      /* check size of a previously destroyed task queue */
      size = _taskq_get_value(tqueues[2]);
      if (current_test == 2) {
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, size == MAX_MQX_UINT, "Test #2: Testing 2.4: Check size of a previously destroyed task queue")
      } else {
         EU_ASSERT_REF_TC_MSG(tc_3_main_task, size == MAX_MQX_UINT, "Test #3: Testing 3.4: Check size of a previously destroyed task queue")
      }
      if (size == MAX_MQX_UINT) {
         _task_set_error(MQX_OK);
      }/* Endif */

      /* resume all tasks */
      check_order_of_exec = 0;
      result = _taskq_resume(current_tq,TRUE);
      if (current_test == 2) {
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.5: Resume all tasks")
      } else {
         EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.5: Resume all tasks")
      }

      /* check that all counters are set correctly */
      for (i=0; i < 10; i++) {
        if (current_test == 2) {
           EU_ASSERT_REF_TC_MSG(tc_2_main_task, global_count[i] == i, "Test #2: Testing 2.6: Check that all counters are set correctly")
        } else {
           EU_ASSERT_REF_TC_MSG(tc_3_main_task, global_count[i] == i, "Test #3: Testing 3.6: Check that all counters are set correctly")
        }

        global_count[i] = 0xffff;
      }/* Endfor */
      TEST_queue_test(7);
      check_order_of_exec = 0;
      /* check no. of elements on the task queue */
      size = _taskq_get_value(current_tq);
      if (current_test == 2) {
         EU_ASSERT_REF_TC_MSG(tc_2_main_task, size == 10, "Test #2: Testing 2.8: Check number of elements on the task queue")
      } else {
         EU_ASSERT_REF_TC_MSG(tc_3_main_task, size == 10, "Test #3: Testing 3.8: Check number of elements on the task queue")
      }
      if (size != 10) {
         _task_set_error(MQX_OK);
      }/* Endif */

      /*
      **  resume one task at a time 
      */
      for (i=0; i < 10; i++) {
         result = _taskq_resume(current_tq,FALSE);
         if (current_test == 2) {
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.9: Resume one task at a time")
         } else {
            EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.9: Resume one task at a time")
         }
      }/* Endfor */

      /*
      ** check order of execution of resumed tasks 
      */
      for (i=0; i < 10; i++) {
         if (current_test == 2) {
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, global_count[i] == i, "Test #2: Testing 2.11: Check order of execution of resumed tasks ")
         } else {
            EU_ASSERT_REF_TC_MSG(tc_3_main_task, global_count[i] == i, "Test #3: Testing 3.11: Check order of execution of resumed tasks ")
         }
      }/* Endfor */

      /*
      ** destroy test tasks
      */
      for (i=0; i < 10; i++) {
         if ( test_task_id[i] != MQX_NULL_TASK_ID ) {
             result = _task_destroy(test_task_id[i]);             
             if (current_test == 2) {
                EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.12: Destroy test tasks")
             } else {
                EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.12: Destroy test tasks")
             }
         }

      TEST_queue_test(14);
      current_tq = tqueues[1];  
      }
 }

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Testing resume/suspend on FIFO task queue with same priorities
*
*END*---------------------------------------------------------------------*/
 void tc_2_main_task(void)
 {
    current_tq = tqueues[0];
    current_test = 2;
    tc_2_3_main_task();
 }

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Testing resume/suspend on Priority task queue with same priorities
*
*END*---------------------------------------------------------------------*/
 void tc_3_main_task(void)
 {
    current_tq = tqueues[1];
    current_test = 3;  
    tc_2_3_main_task();
 }
 
/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_4_5_main_task
* Comments     : test creating different priority tasks, suspend on task queue
*                resume each one, verify order of execution
*                suspend tasks
*                resume ALL, verify order of execution
*
*END*---------------------------------------------------------------------*/
 void tc_4_5_main_task()
 {
    _mqx_uint   result;
    _mqx_uint   size;
    _mqx_uint   temp;
    _mqx_uint   i;
    _mqx_uint   priority[] = {3,4,5,3,4,5,3,4,5,3};
    
      for (i=0; i < 10; i++) {
         global_count[i] = 0xffff;
         test_task_id[i] = _task_create(0,TEST_TASK10,i);
         if (current_test == 4)
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, test_task_id[i] != MQX_NULL_TASK_ID, "Test #4: Testing 4.1: Create task operation")
         else
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, test_task_id[i] != MQX_NULL_TASK_ID, "Test #5: Testing 5.1: Create task operation")
         if (test_task_id[i] == MQX_NULL_TASK_ID) {
            _task_set_error(MQX_OK);
         }/* Endif */
         _task_set_priority(test_task_id[i], priority[i], &temp);
      } /*Endfor */
   
      /* check the counters are still set to 0xffff*/
      for (i=0; i < 10; i++) {
         if (current_test == 4)
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, global_count[i] == 0xffff, "Test #4: Testing 4.2: Check the counters are still set to 0xffff")
         else
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, global_count[i] == 0xffff, "Test #5: Testing 5.2: Check the counters are still set to 0xffff")
         if (global_count[i] != 0xffff) {
            break;
         }/* Endif */
      }/* Endfor */
      /* check no. of elements on the task queue */
      size = _taskq_get_value(current_tq);
      if (current_test == 4)
         EU_ASSERT_REF_TC_MSG(tc_4_main_task, size == 10, "Test #4: Testing 4.3: Check number of elements on the task queue")
      else
         EU_ASSERT_REF_TC_MSG(tc_5_main_task, size == 10, "Test #5: Testing 5.3: Check number of elements on the task queue")
      if (size != 10) {
         _task_set_error(MQX_OK);
      }/* Endif */
      /* check size of a previously destroyed task queue */
      size = _taskq_get_value(tqueues[2]);
      if (current_test == 4)
         EU_ASSERT_REF_TC_MSG(tc_4_main_task, size == MAX_MQX_UINT, "Test #4: Testing 4.4: Check size of a previously destroyed task queue")
      else
         EU_ASSERT_REF_TC_MSG(tc_5_main_task, size == MAX_MQX_UINT, "Test #5: Testing 5.4: Check size of a previously destroyed task queue")
      if (size == MAX_MQX_UINT) {
         _task_set_error(MQX_OK);
      }/* Endif */

      check_order_of_exec = 0;
      /* check no. of elements on the task queue */
      size = _taskq_get_value(current_tq);
      if (current_test == 4)
         EU_ASSERT_REF_TC_MSG(tc_4_main_task, size == 10, "Test #4: Testing 4.5: Check number of elements on the task queue")
      else
         EU_ASSERT_REF_TC_MSG(tc_5_main_task, size == 10, "Test #5: Testing 5.5: Check number of elements on the task queue")
      if (size != 10) {
         _task_set_error(MQX_OK);
      }/* Endif */

      /*
      **  resume one task at a time 
      */
      for (i=0; i < 10; i++) {
         result = _taskq_resume(current_tq,FALSE);
         if (current_test == 4)
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.6: Resume one task at a time")
         else
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.6: Resume one task at a time")
      }/* Endfor */
      /*
      ** check order of execution of resumed tasks
      ** order should not be affected by priorities in FIFO case
      ** order will be affected by priorities in Priority case 
      */
      for (i=0; i < 10; i++) {
         
         if (((current_test == 4) && (global_count[i] != i)) ||
            ((current_test == 5) && (global_count[i] != expect_order2[i])))
         {
             if (current_test == 4)
                EU_ASSERT_REF_TC_MSG(tc_4_main_task, FALSE, "Test #4: Testing 4.7: Check order of execution of resumed tasks")
             else
                EU_ASSERT_REF_TC_MSG(tc_5_main_task, FALSE, "Test #5: Testing 5.7: Check order of execution of resumed tasks")
         }/* Endif */
         global_count[i] = 0xffff;
      }/* Endfor */
      TEST_queue_test(8);
      if (current_test == 5) {
         /*
         ** resume two more tasks to get task0 and task3 back at the head of queue 
         */
         _taskq_resume(current_tq,FALSE);
         _taskq_resume(current_tq,FALSE);
         global_count[6] = 0xffff;
         global_count[9] = 0xffff;
      }/* Endif */

      /* resume all tasks */
      check_order_of_exec = 0;
      result = _taskq_resume(current_tq,TRUE);
      if (current_test == 4)
         EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.9: Resume all tasks")
      else
         EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.9: Resume all tasks")

      /* check that all counters are set correctly 
      ** order of execution is affected by priorities since dispatch 
      ** is done after all tasks are readied
      */
      for (i=0; i < 10; i++) {
         if (current_test == 4)
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, global_count[i] == expect_order[i], "Test #4: Testing 4.10: Check that all counters are set correctly ")
         else
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, global_count[i] == expect_order[i], "Test #5: Testing 5.10: Check that all counters are set correctly ")

         global_count[i] = 0xffff;
      }/* Endfor */

      /*
      ** destroy test tasks
      */
      for (i=0; i < 10; i++) {
         if ( test_task_id[i] != MQX_NULL_TASK_ID ) { 
             result = _task_destroy(test_task_id[i]);
             if (current_test == 4)
                EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.11: Destroy test tasks")
             else
                EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.11: Destroy test tasks")
         }
      }/* Endfor */
      TEST_queue_test(12);
      current_tq = tqueues[1];
 }
 
/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Testing resume/suspend on FIFO task queue with diff priorities
*
*END*---------------------------------------------------------------------*/
 void tc_4_main_task(void)
 {
    current_tq = tqueues[0];
    current_test = 4;
    tc_4_5_main_task();
 }

 /*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Testing resume/suspend on priority task queue with diff priorities
*
*END*---------------------------------------------------------------------*/
 void tc_5_main_task(void)
 {
    current_tq = tqueues[1];
    current_test = 5;
    tc_4_5_main_task();
    
 }

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_6_7_main_task
* Comments     : create 10 tasks, each task will suspend itself
*                count number of tasks suspended, should be 10
*                main task will suspend all tasks
*
*END*---------------------------------------------------------------------*/
 void tc_6_7_main_task(void)
 {
    _mqx_uint   result;
    _mqx_uint   size, event_bits;
    _mqx_uint   i;
    
      for (i=0; i < 10; i++) {
         global_count[i] = 0xffff;
         test_task_id[i] = _task_create(0,TEST2_TASK,(uint32_t)i);
         if (current_test == 6)
             EU_ASSERT_REF_TC_MSG(tc_6_main_task, test_task_id[i] != MQX_NULL_TASK_ID, "Test #6: Testing 6.1: Create task operation")
         else
             EU_ASSERT_REF_TC_MSG(tc_7_main_task, test_task_id[i] != MQX_NULL_TASK_ID, "Test #7: Testing 7.1: Create task operation")
         if (test_task_id[i] == MQX_NULL_TASK_ID) {
             _task_set_error(MQX_OK);
         } else {
            result = _taskq_suspend_task(test_task_id[i],current_tq);
            if (current_test == 6)
                EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.2: Suspend the ready task in task queue")
            else
                EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.2: Suspend the ready task in task queue")
         }/* Endif */
      } /* Endfor */
   
      /* check the counters are still set to 0xffff*/
      for (i=0; i < 10; i++) {
         if (current_test == 6)
             EU_ASSERT_REF_TC_MSG(tc_6_main_task, global_count[i] == 0xffff, "Test #6: Testing 6.3: Check the counters are still set to 0xffff")
         else
             EU_ASSERT_REF_TC_MSG(tc_7_main_task, global_count[i] == 0xffff, "Test #7: Testing 7.3: Check the counters are still set to 0xffff")
         if (global_count[i] != 0xffff) {
            break;
         }/* Endif */
      }/* Endfor */
      /* check no. of elements on the task queue */
      size = _taskq_get_value(current_tq);
      if (current_test == 6)
          EU_ASSERT_REF_TC_MSG(tc_6_main_task, size == 10, "Test #6: Testing 6.4: Check number of elements on the task queue")
      else
          EU_ASSERT_REF_TC_MSG(tc_7_main_task, size == 10, "Test #7: Testing 7.4: Check number of elements on the task queue")
      if (size != 10) {
          _task_set_error(MQX_OK);
      }/* Endif */

      /* resume all tasks */
      check_order_of_exec = 0;
      result = _taskq_resume(current_tq,TRUE);
      if (current_test == 6)
          EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.5: Resume all tasks")
      else
          EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.5: Resume all tasks")

      for  (size=0; size < 10; size++) {
         _event_wait_any(event_ptr,0x3ff,0);
         _event_get_value(event_ptr,&event_bits);
         if (current_test == 6)
             EU_ASSERT_REF_TC_MSG(tc_6_main_task, (event_bits >> size) == 1, "Test #6: Testing 6.6: _event_get_value() operation")
         else
             EU_ASSERT_REF_TC_MSG(tc_7_main_task, (event_bits >> size) == 1, "Test #7: Testing 7.6: _event_get_value() operation")
         
         if ( test_task_id[size] != MQX_NULL_TASK_ID ) {
             result = _taskq_suspend_task(test_task_id[size],current_tq);
             if (current_test == 6)
                 EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.6: Suspend the ready task in task queue")
             else
                 EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.6: Suspend the ready task in task queue")
         }

         _event_clear(event_ptr,event_bits);
      }/* Endfor */

      /* check that all counters are set correctly */
      for (i=0; i < 10; i++) {
         if (current_test == 6)
             EU_ASSERT_REF_TC_MSG(tc_6_main_task, global_count[i] == i, "Test #6: Testing 6.7: Check that all counters are set correctly")
         else
             EU_ASSERT_REF_TC_MSG(tc_7_main_task, global_count[i] == i, "Test #7: Testing 7.7: Check that all counters are set correctly")

         global_count[i] = 0xffff;
      }/* Endfor */
      TEST_queue_test(8);
      check_order_of_exec = 0;
      /* check no. of elements on the task queue */
      size = _taskq_get_value(current_tq);
      if (current_test == 6)
          EU_ASSERT_REF_TC_MSG(tc_6_main_task, size == 10, "Test #6: Testing 6.9: Check number of elements on the task queue")
      else
          EU_ASSERT_REF_TC_MSG(tc_7_main_task, size == 10, "Test #7: Testing 7.9: Check number of elements on the task queue")
      if (size != 10) {
         _task_set_error(MQX_OK);
      }/* Endif */
      /*
      **  resume one task at a time 
      */
      for (i=0; i < 10; i++) {
         if( test_task_id[i] != MQX_NULL_TASK_ID ) {
             result = _taskq_resume(current_tq,FALSE);
             if (current_test == 6)
                 EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.10: Resume one task at a time ")
             else
                 EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.10: Resume one task at a time ")
         }
      }/* Endfor */

      for  (size=0; size < 10; size++) {
         _event_wait_any(event_ptr,0x3ff,0);
         _event_get_value(event_ptr,&event_bits);
         if (current_test == 6)
             EU_ASSERT_REF_TC_MSG(tc_6_main_task, (event_bits >> size) == 1, "Test #6: Testing 6.12: Verify tasks resumed in order")
         else
             EU_ASSERT_REF_TC_MSG(tc_7_main_task, (event_bits >> size) == 1, "Test #7: Testing 7.12: Verify tasks resumed in order")
         
         if ( test_task_id[size] != MQX_NULL_TASK_ID ) {
             result = _taskq_suspend_task(test_task_id[size],current_tq);
             if (current_test == 6)
                 EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.14: Suspend the ready task in task queue")
             else
                 EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.14: Suspend the ready task in task queue")
         }
         _event_clear(event_ptr,event_bits);
      }/* Endfor */

      /*
      ** check order of execution of resumed tasks 
      */
      for (i=0; i < 10; i++) {
         if (current_test == 6)
             EU_ASSERT_REF_TC_MSG(tc_6_main_task, global_count[i] == i, "Test #6: Testing 6.15: Check order of execution of resumed tasks")
         else
             EU_ASSERT_REF_TC_MSG(tc_7_main_task, global_count[i] == i, "Test #7: Testing 7.15: Check order of execution of resumed tasks")

      }/* Endfor */

      /*
      ** destroy test tasks
      */
      for (i=0; i < 10; i++) {
         if ( test_task_id[i] != MQX_NULL_TASK_ID ) {
             result = _task_destroy(test_task_id[i]);
             if (current_test == 6)
                 EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.16: Destroy test tasks")
             else
                 EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.16: Destroy test tasks")
         }
      }/* Endfor */
      TEST_queue_test(17);
      current_tq = tqueues[1];
  

 }
/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Testing _taskq_suspend_task on FIFO task queue with same priorities
*
*END*---------------------------------------------------------------------*/
 void tc_6_main_task(void)
 {
   current_tq = tqueues[0];
   _event_create_fast(0);
   _event_open_fast(0,&event_ptr);
   current_test = 6;
   tc_6_7_main_task();  
 }
/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_7_main_task
* Comments     : TEST #7: Testing _taskq_suspend_task on Priority task queue with same priorities
*
*END*---------------------------------------------------------------------*/
 void tc_7_main_task(void)
 {
    current_tq = tqueues[1];
   _event_create_fast(0);
   _event_open_fast(0,&event_ptr);
   current_test = 7;
   tc_6_7_main_task();
 }
/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_8_9_main_task
* Comments     :  create 10 tasks, each task will suspend itself
*                 count number of tasks suspended, should be 10
*                 main task will suspend all tasks
*
*
*END*---------------------------------------------------------------------*/
 void tc_8_9_main_task(void)
 {
    _mqx_uint   result;
    _mqx_uint   size, event_bits;
    _mqx_uint   temp;
    _mqx_uint   i;
    _mqx_uint   priority2[] = {10,11,12,10,11,12,10,11,12,10};
    _mqx_uint   exec_order[] = {0,3,6,9,1,4,7,2,5,8};

      for (i=0; i < 10; i++) {
         global_count[i] = 0xffff;
         test_task_id[i] = _task_create(0,TEST2_TASK,(uint32_t)i);
         if (current_test == 8)
             EU_ASSERT_REF_TC_MSG(tc_8_main_task, test_task_id[i] != MQX_NULL_TASK_ID, "Test #8: Testing 8.1: Create task operation")
         else
             EU_ASSERT_REF_TC_MSG(tc_9_main_task, test_task_id[i] != MQX_NULL_TASK_ID, "Test #9: Testing 9.1: Create task operation")
         if (test_task_id[i] == MQX_NULL_TASK_ID) {
            _task_set_error(MQX_OK);
         } else {
            result = _taskq_suspend_task(test_task_id[i],current_tq);
            if (current_test == 8)
                EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.2: Suspend task in task queue")
            else
                EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9: Testing 9.2: Suspend task in task queue")

            _task_set_priority(test_task_id[i], priority2[i], &temp);
         }/* Endif */
      } /* Endfor */
   
      /* check the counters are still set to 0xffff*/
      for (i=0; i < 10; i++) {
         if (current_test == 8)
             EU_ASSERT_REF_TC_MSG(tc_8_main_task, global_count[i] == 0xffff, "Test #8: Testing 8.3: Check the counters are still set to 0xffff")
         else
             EU_ASSERT_REF_TC_MSG(tc_9_main_task, global_count[i] == 0xffff, "Test #9: Testing 9.3: Check the counters are still set to 0xffff")
         if (global_count[i] != 0xffff) {
            break;
         }/* Endif */
      }/* Endfor */
      /* check no. of elements on the task queue */
      size = _taskq_get_value(current_tq);
      if (current_test == 8)
          EU_ASSERT_REF_TC_MSG(tc_8_main_task, size == 10, "Test #8: Testing 8.4: Check number of elements on the task queue")
      else
          EU_ASSERT_REF_TC_MSG(tc_9_main_task, size == 10, "Test #9: Testing 9.4: Check number of elements on the task queue")
      if (size != 10) {
        _task_set_error(MQX_OK);
      }/* Endif */
      /* resume all tasks */
      check_order_of_exec = 0;
      result = _taskq_resume(current_tq,TRUE);
      if (current_test == 8)
          EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.5: Resume all tasks")
      else
          EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9: Testing 9.5: Resume all tasks")

      for  (size=0; size < 10; size++) {
         _event_wait_any(event_ptr,0x3ff,0);
         _event_get_value(event_ptr,&event_bits);
         if (current_test == 8)
             EU_ASSERT_REF_TC_MSG(tc_8_main_task, (event_bits >> exec_order[size]) == 1, "Test #8: Testing 8.6: _event_get_value() operation")
         else
             EU_ASSERT_REF_TC_MSG(tc_9_main_task, (event_bits >> exec_order[size]) == 1, "Test #9: Testing 9.6: _event_get_value() operation")
         
         if ( test_task_id[exec_order[size]] != MQX_NULL_TASK_ID ) {        
             result = _taskq_suspend_task(test_task_id[exec_order[size]],current_tq);
             if (current_test == 8)
                 EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.6: Suspend task in task queue")
             else
                 EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9: Testing 9.6: Suspend task in task queue")
         }

         _event_clear(event_ptr,event_bits);
      }/* Endfor */
      /* check that all counters are set correctly */
      for (i=0; i < 10; i++) {
         if (current_test == 8)
             EU_ASSERT_REF_TC_MSG(tc_8_main_task, global_count[i] == expect_order[i], "Test #8: Testing 8.7: Check that all counters are set correctly")
         else
             EU_ASSERT_REF_TC_MSG(tc_9_main_task, global_count[i] == expect_order[i], "Test #9: Testing 9.7: Check that all counters are set correctly")

         global_count[i] = 0xffff;
      }/* Endfor */
      TEST_queue_test(8);
      check_order_of_exec = 0;
      /* check no. of elements on the task queue */
      size = _taskq_get_value(current_tq);
      if (current_test == 8)
          EU_ASSERT_REF_TC_MSG(tc_8_main_task, size == 10, "Test #8: Testing 8.9: Check number of elements on the task queue")
      else
          EU_ASSERT_REF_TC_MSG(tc_9_main_task, size == 10, "Test #9: Testing 9.9: Check number of elements on the task queue")
      if (size != 10) {
         _task_set_error(MQX_OK);
      }/* Endif */
      /*
      **  resume one task at a time 
      */
      for (i=0; i < 10; i++) {
         result = _taskq_resume(current_tq,FALSE);
         if (current_test == 8)
             EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.10: Resume one task at a time")
         else
             EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9: Testing 9.10: Resume one task at a time")
      }/* Endfor */
      for  (size=0; size < 10; size++) {
         _event_wait_any(event_ptr,0x3ff,0);
         _event_get_value(event_ptr,&event_bits);
         if (current_test == 8)
             EU_ASSERT_REF_TC_MSG(tc_8_main_task, (event_bits >> exec_order[size]) == 1, "Test #8: Testing 8.12: _event_get_value() operation")
         else
             EU_ASSERT_REF_TC_MSG(tc_9_main_task, (event_bits >> exec_order[size]) == 1, "Test #9: Testing 9.12: _event_get_value() operation")

         if ( test_task_id[exec_order[size]] != MQX_NULL_TASK_ID ) {
             result = _taskq_suspend_task(test_task_id[exec_order[size]],current_tq);
             if (current_test == 8)
                 EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.14: Suspend task in task queue")
             else
                 EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9: Testing 9.14: Suspend task in task queue")
         }

         _event_clear(event_ptr,event_bits);
      }/* Endfor */
      /*
      ** check order of execution of resumed tasks 
      */
      for (i=0; i < 10; i++) {
         if (current_test == 8)
             EU_ASSERT_REF_TC_MSG(tc_8_main_task, global_count[i] == expect_order[i], "Test #8: Testing 8.15: Check order of execution of resumed tasks")
         else
             EU_ASSERT_REF_TC_MSG(tc_9_main_task, global_count[i] == expect_order[i], "Test #9: Testing 9.15: Check order of execution of resumed tasks")
      }/* Endfor */

      /*
      ** destroy test tasks
      */
      for (i=0; i < 10; i++) {
         if ( test_task_id[i] != MQX_NULL_TASK_ID ) {
             result = _task_destroy(test_task_id[i]);
             if (current_test == 8)
                 EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.16: Destroy test tasks")
             else
                 EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9: Testing 9.16: Destroy test tasks")
         }
      }/* Endfor */
      TEST_queue_test(17);
      current_tq = tqueues[1];
 }

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_8_main_task
* Comments     : TEST #8: Testing _taskq_suspend_task on FIFO task queue with diff priorities
*
*END*---------------------------------------------------------------------*/
 void tc_8_main_task(void)
 {
    current_tq = tqueues[0];
    current_test = 8;
    tc_8_9_main_task();
 }
 
/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_9_main_task
* Comments     : TEST #9: Testing _taskq_suspend_task on Priority task queue with diff priorities
*
*END*---------------------------------------------------------------------*/
 void tc_9_main_task(void)
 {
    current_tq = tqueues[1];
    current_test = 9;
    tc_8_9_main_task();
 }

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name: TEST_queue_test
* Return Value : none
* Comments     : 
*    test the integrity of the queues, report any errors
*
*END*----------------------------------------------------------------------*/

void TEST_queue_test
   (
      uint16_t case_no
   )
{/* Body */
    _mqx_uint result;
    void     *element_in_error, *queue_in_error;

    result = _taskq_test(&queue_in_error,&element_in_error);
    switch(current_test) {
    case 1:
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.8: _taskq_test operation")
        break;
    case 2:
        if (case_no == 7)
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.7: _taskq_test operation")
        else if (case_no == 14)
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.14: _taskq_test operation")
        break;
    case 3:
        if (case_no == 7)
            EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing: 3.7: _taskq_test operation")
        else if (case_no == 14)
            EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing: 3.14: _taskq_test operation")
        break;
    case 4:
        if (case_no == 8)
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: 4.8: _taskq_test operation")
        else if (case_no == 12)
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: 4.12: _taskq_test operation")
        break;
    case 5:
        if (case_no == 8)
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.8: _taskq_test operation")
        else if (case_no == 12)
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.12: _taskq_test operation")
        break;
    case 6:
        if (case_no == 8)
            EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing: 6.8: _taskq_test operation")
        else if (case_no == 17)
            EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing: 6.17: _taskq_test operation")
        break;
    case 7:
        if (case_no == 8)
            EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: 7.8: _taskq_test operation")
        else if (case_no == 17)
            EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: 7.17: _taskq_test operation")
        break;
    case 8:
        if (case_no == 8)
            EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: 8.8: _taskq_test operation")
        else if (case_no == 17)
            EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: 8.17: _taskq_test operation")
        break;
    case 9:
        if (case_no == 8)
            EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: 9.8: _taskq_test operation")
        else if (case_no == 17)
            EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: 9.17: _taskq_test operation")
        break;
    }
}


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : test_task
* Comments     : 
*    This is the test task with priority 8
*
*END*----------------------------------------------------------------------*/

void test_task
   (
      /* [IN] The task index */
      uint32_t param
   )
{ /* Body */
    _mqx_uint    result;

    while (TRUE) {
        /* suspend myself on task queue*/
        result = _taskq_suspend(current_tq);
        switch(current_test) {
        case 2:
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: _taskq_suspend from task operation")
            break;
        case 3:
            EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing: _taskq_suspend from task operation")
            break;
        case 4:
            EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: _taskq_suspend from task operation")
            break;
        case 5:
            EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: _taskq_suspend from task operation")
            break;
        }

        /* should be resumed by main_task, 
        ** set my counter to indicate order of execution 
        ** suspend myself again
        */
        global_count[(_mqx_uint)param] = check_order_of_exec;
        check_order_of_exec++;
    }/* Endwhile */

}/* Endbody */


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : test2_task
* Comments     : 
*    This is the test task with priority 10
*
*END*----------------------------------------------------------------------*/

void test2_task
   (
      /* [IN] The task index */
      uint32_t param
   )
{ /* Body */
    volatile _mqx_uint       *global_ptr;
             _mqx_uint        result;
             void            *event_ptr;

    global_ptr = &global_count[(_mqx_uint)param];
    _event_open_fast(0, &event_ptr);
    while (TRUE) {
       if (*global_ptr == 0xffff) {
          *global_ptr = check_order_of_exec;
          check_order_of_exec++;
          _event_set(event_ptr,(1 << (_mqx_uint)param));
       }/* Endif */
    } /* Endwhile */

}/* Endbody */

//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_taskq)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Testing _taskq_create primitive"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Testing resume/suspend on FIFO task queue with same priorities"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST #3 - Testing resume/suspend on Priority task queue with same priorities"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST #4 - Testing resume/suspend on FIFO task queue with diff priorities"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST #5 - Testing resume/suspend on priority task queue with diff priorities"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST #6 - Testing _taskq_suspend_task on FIFO task queue with same priorities"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST #7 - Testing _taskq_suspend_task on Priority task queue with same priorities"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST #8 - Testing _taskq_suspend_task on FIFO task queue with diff priorities"),
    EU_TEST_CASE_ADD(tc_9_main_task, " TEST #9 - Testing _taskq_suspend_task on Priority task queue with diff priorities")
 EU_TEST_SUITE_END(suite_taskq)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_taskq, " - Test taskq suite")
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
