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
* $Date    : Jul-23-2012$
*
* Comments:
*
*   This file contains the source for the schedule module testing.
*   NOTE: THIS TEST DOESN'T INCLUDE TESTING OF INVALID PRIMITIVE VARIABLES
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
// #include <log.h>
// #include <klog.h>
//#include <test_module.h>
#include "test.h"

#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME test
//------------------------------------------------------------------------
// Test suite function
extern void tc_1_main_task(void);//TEST # 1 - Testing:_sched_get_max_priority
extern void tc_2_main_task(void);//TEST # 2 - Testing:_sched_get_min_priority
extern void tc_3_main_task(void);//TEST # 3 - Testing:_sched_get_policy, _sched_set_policy
extern void tc_4_main_task(void);//TEST # 4 - Testing:_sched_get_rr_interval, _sched_get_rr_interval_ticks, _sched_set_rr_interval, _sched_set_rr_interval_ticks
extern void tc_5_main_task(void);//TEST # 5 - Testing:_sched_yield
//------------------------------------------------------------------------
extern void tc_3_test_task(void);//
extern void tc_4_test_task(void);//
extern void tc_5_test_task(void);//

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
{ MAIN_TASK,   main_task,  2500,   7,   "Main_Task",   MQX_AUTO_START_TASK},
{ TEST_TASK,   test_task,  1000,   7,   "Test_Task",   0},
{ 0,           0,          0,      0,   0,             0}
};

volatile bool  test_task_ran, main_task_ran;

_task_id    test_task_id;
_task_id    main_task_id;

TIME_STRUCT current_time;
uint32_t            test_task_start, test_task_slice, test_task_end;
volatile bool   test_task_loop, main_task_loop;
uint32_t            main_task_start, main_task_slice, main_task_end;
uint32_t            wait_slice_time;

#if MQX_USE_LWMEM_ALLOCATOR
_lwmem_pool_id  pool_err;
void           *block_err;
#define _mem_test() _lwmem_test(&pool_err, &block_err)
#endif
/******   Test cases body definition      *******/

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST #1: The following code tests the following primitive: 
//                               - _sched_get_max_priority                  
//END---------------------------------------------------------------------
 void tc_1_main_task(void)
 {
   _mqx_uint       result;
   _mqx_uint       max_priority;    

   max_priority = _sched_get_max_priority( MQX_SCHED_RR );
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,max_priority == 0,"Test #1 Testing: 1.0: Get max priority")

   result = _mem_test();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK),"Test #1 Testing: 1.1: _mem_test operation")
   if (result != MQX_OK) {
      //printf("\n\n...EXITING..._mem_test returned 0x%X", result);
      eunit_test_stop();
     _mqx_exit(0);
    }
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST #2: The following code tests the following primitive:
//                                - _sched_get_min_priority                  
//END---------------------------------------------------------------------
 void tc_2_main_task(void)
 {
   _mqx_uint       result;
   _mqx_uint       min_priority;

   min_priority = _sched_get_min_priority( MQX_SCHED_RR );
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (min_priority >= 1), "Test #2 Testing: 2.0:Get min priority")

   result = _mem_test();
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.1: _mem_test operation")
   if (result != MQX_OK) {
        //printf("\n\n...EXITING..._mem_test returned 0x%x", result);
         eunit_test_stop();
        _mqx_exit(0);
    }
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST #3: The following code tests the following primitives:
//                                - _sched_get_policy                         
//                                 - _sched_set_policy                         
//END---------------------------------------------------------------------
 void tc_3_main_task(void)
 {

   _mqx_uint       result;
   _mqx_uint       sched_policy, new_sched_policy, old_sched_policy;    

/* Get Processor's Scheduling Policy */
   result = _sched_get_policy(MQX_DEFAULT_TASK_ID, &sched_policy);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,!result,"Test #3 Testing: 3.0: Get policy") 

/* Verify that this is the correct policy */
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,sched_policy == _mqx_kernel_data->SCHED_POLICY,"Test #3 Testing: 3.1: Verify that this is the correct policy")
   
/* Get My Scheduling Policy using MQX_NULL_TASK_ID as input */
   result = _sched_get_policy(MQX_NULL_TASK_ID, &sched_policy);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK,"Test #3 Testing: 3.2: Get My Scheduling Policy using MQX_NULL_TASK_ID")

/* Get My Scheduling Policy using my actual ID as input */
   result = _sched_get_policy(main_task_id, &sched_policy);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK,"Test #3 Testing: 3.3: Get My Scheduling Policy using my actual ID")

/* Set Processor's Scheduling Policy */
   old_sched_policy = _sched_set_policy(MQX_DEFAULT_TASK_ID, MQX_SCHED_RR);
/* Get Processor's Scheduling Policy */
   result = _sched_get_policy(MQX_DEFAULT_TASK_ID, &new_sched_policy);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK,"Test #3 Testing: 3.4: Get processor's Scheduling Policy")

/* Verify that Scheduling Policy was set properly */
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((old_sched_policy == sched_policy) && (_mqx_kernel_data->SCHED_POLICY == MQX_SCHED_RR)),"Test #3 Testing: 3.5:Verify that Scheduling Policy was set properly")

/* Create a test task, it should have RR Scheduling Policy */
   
   test_task_id = _task_create(0, TEST_TASK, 3L);
   result = _sched_get_policy(test_task_id, &sched_policy);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK,"Test #3 Testing: 3.6:Create a test task, it should have RR Scheduling Policy")
/* Get Test_Task's Scheduling Policy */

/* Verify that Test_Task's Scheduling Policy is equal to processor's */
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,sched_policy == _mqx_kernel_data->SCHED_POLICY,"Test #3 Testing: 3.7:Verify that Test_Task's Scheduling Policy is equal to processor's")

/* Yield for Test_Task to test TIME-SLICING (RR SCHEDULING) */
/* Test_Task should dispatch in 50 msec (time-slice) */
   test_task_ran = FALSE;
   _sched_yield();
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, test_task_ran,"Test #3 Testing: 3.8:Yield for Test_Task to test TIME-SLICING")
   
   /* Test_Task gave up processor; check if involuntarily (time-slice) */
   main_task_ran = TRUE;
   _time_get(&current_time);
   test_task_slice = current_time.SECONDS*1000 + current_time.MILLISECONDS
                     - test_task_start;

/* Verify TIME-SLICE operation using a flag */
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,test_task_loop,"Test #3 Testing: 3.9:Verify TIME-SLICE operation using a flag")
   if (test_task_loop)  {
      /* Test_Task's RR scheduling Passed */
      test_task_loop = FALSE;
   }

/* Yield for Test_Task; it will change its policy to FIFO 
   and we'll verify that no time-slicing will occur */
   test_task_ran = FALSE;
   _sched_yield();
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,test_task_ran,"Test #3 Testing: 3.10:Yield for Test_Task")

/* I should only get the processor when Test_Task gives it up
   voluntarily; otherwise FIFO Scheduling has failed */
   main_task_ran = TRUE;

   /* Record dispatching time */
   test_task_slice = current_time.SECONDS*1000 + current_time.MILLISECONDS
                     - test_task_start;

/* Get Test_Task's new Scheduling Policy */
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((_sched_get_policy(test_task_id, &new_sched_policy) == MQX_OK) && (new_sched_policy == MQX_SCHED_FIFO)),"Test #3 Testing: 3.11:Get Test_Task's new Scheduling Policy")
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (test_task_loop == FALSE), "Test #3 Testing: 3.12:Get Test_Task's new Scheduling Policy")
   if (test_task_loop == FALSE)  {
      /* Test_Task's FIFO scheduling Passed */
      /* Test_Task has set this flag to FALSE indicating it
         gave up the processor voluntarily */
   }
   else {
      /* Test_Task's FIFO scheduling Failed */
      /* Test_Task has not set this flag to FALSE indicating it never
         gave up the processor voluntarily */
      //printf("\nMain_Task: Test_Task's FIFO scheduling FAILED; TEC %s",_mqx_get_task_error_code_string(_task_get_error()));
      /* Yield for Test_Task to report failure */
      _sched_yield();
      main_task_ran = TRUE;
   }

/* Set My Scheduling Policy to RR using my actual ID as input */
   old_sched_policy =
      _sched_set_policy(main_task_id, MQX_SCHED_RR);

/* I should give up the processor involuntarily after 50 msec */

   /* Keep track of my time-slicing */
   _time_get(&current_time);
   main_task_start = current_time.SECONDS*1000 + current_time.MILLISECONDS;

   /* Get My Scheduling Policy using my actual ID as input */
   result = _sched_get_policy(main_task_id, &new_sched_policy);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK,"Test #3 Testing: 3.13:Get My Scheduling Policy using my actual ID as input")

   /* Verify that Scheduling Policy was set properly */
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((old_sched_policy == MQX_SCHED_FIFO) && (new_sched_policy == MQX_SCHED_RR)),"Test #3 Testing: 3.14:Verify that Scheduling Policy was set properly")

   /* Get My Scheduling Policy using MQX_NULL_TASK_ID as input */
   result = _sched_get_policy(MQX_NULL_TASK_ID, &new_sched_policy);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK,"Test #3 Testing: 3.15:Get My Scheduling Policy using MQX_NULL_TASK_ID")

   /* Verify that Scheduling Policy was set properly */
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((old_sched_policy == MQX_SCHED_FIFO) && (new_sched_policy == MQX_SCHED_RR)),"Test #3 Testing: 3.16:Verify that Scheduling Policy was set properly")

/* Test the operation of RR Scheduling Policy */
/* I should run for 50 msec, then dispatch */
   _time_get(&current_time);
   main_task_loop = TRUE;
   while (main_task_loop)  {
      /* Loop! */
      _time_get(&current_time);
      
      if ((current_time.SECONDS*1000 + current_time.MILLISECONDS
            - main_task_start) > wait_slice_time){
         break;   /* time-slice should have occurred by now */
         }
   }
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,!main_task_loop,"Test #3 Testing: 3.17: dispatch should occur")
   if (main_task_loop)  {
      /* dispatch didn't occur, report Failure */
      //printf("\nMain_Task: RR Scheduling FAILED; TEC %s",
        //          _mqx_get_task_error_code_string(_task_get_error()));
      /* Inform Test_Task that time-slicing failed */
      main_task_loop = FALSE;
      _sched_yield();
      main_task_ran = TRUE;
   }

/* Test_Task just changed my policy to FIFO */
   main_task_ran = TRUE;

   /* Keep track of my time-slicing */
   _time_get(&current_time);
   main_task_start = current_time.SECONDS*1000 + current_time.MILLISECONDS;

/* Get My new Scheduling Policy using MQX_NULL_TASK_ID as input */
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((_sched_get_policy(MQX_NULL_TASK_ID, &new_sched_policy) == MQX_OK) && (new_sched_policy == MQX_SCHED_FIFO)),"Test #3 Testing: 3.18:Get My new Scheduling Policy using my actual ID")

/* Get My new Scheduling Policy using my actual ID as input */
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((_sched_get_policy(main_task_id, &new_sched_policy) == MQX_OK) && (new_sched_policy == MQX_SCHED_FIFO)),"Test #3 Testing: 3.19:Get My new Scheduling Policy using my actual ID")

/* I will not give up the processor involuntarily (no time-slice) */
   _time_get(&current_time);
   main_task_loop = TRUE;
   while (main_task_loop)  {
      /* Loop! */
      _time_get(&current_time);
      /* loop enough time to confirm no time-slicing will occur */
      main_task_slice = current_time.SECONDS*1000 +
         current_time.MILLISECONDS - main_task_start;
      if (main_task_slice > 50*3)
         break;   /* time-slice should have occurred by now */
   }
   
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (main_task_loop), "Test #3 Testing: 3.20: Check processor status")
   if (main_task_loop)  {
      /* Set this flag to FALSE indicating I'm giving up the
         processor voluntarily */
     main_task_loop = FALSE;
   }
   else {
      /* dispatch occurred, report Failure */
      /* Inform Test_Task that time-slicing failed */
      _sched_yield();
      main_task_ran = TRUE;
   }
        
   /* Yield for Test_Task */
   test_task_ran = FALSE;
   _sched_yield();
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,test_task_ran,"Test #3 Testing: 3.21:Yield for Test_Task")

   main_task_ran = TRUE;

/* Set Test_Task's Scheduling Policy to RR */
   old_sched_policy =
      _sched_set_policy(test_task_id, MQX_SCHED_RR);

   /* Get Test_Task's Scheduling Policy */
   result = _sched_get_policy(test_task_id, &new_sched_policy);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3 Testing: 3.22:Get Test_Task's Scheduling Policy")

   /* Verify that Scheduling Policy was set properly */
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((old_sched_policy == MQX_SCHED_FIFO) && (new_sched_policy == MQX_SCHED_RR)),"Test #3 Testing: 3.23:Verify that Scheduling Policy was set properly")

/* Yield for Test_Task to test TIME-SLICING (RR SCHEDULING) */
/* Test_Task should dispatch in 50 msec (time-slice) */
   test_task_ran = FALSE;
   _sched_yield();
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,test_task_ran,"Test #3 Testing: 3.24: Yield for Test_Task")

   main_task_ran = TRUE;

   /* Test_Task gave up processor involuntarily (time-slice) */
   _time_get(&current_time);
   test_task_slice = current_time.SECONDS*1000 + current_time.MILLISECONDS
                     - test_task_start;

/* Verify TIME-SLICE operation using a flag */
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (test_task_loop != FALSE), "Test #3 Testing: 3.25:Verify TIME-SLICE operation using a flag")
   if (test_task_loop)  {
      /* Test_Task's RR scheduling Passed */
      test_task_loop = FALSE;
   }
   else {
      /* Test_Task's RR scheduling Failed */
      /* Test_Task has set this flag to FALSE indicating it never
         gave up the processor */
   }

   /* Yield for Test_Task; */
   test_task_ran = FALSE;
   _sched_yield();
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,test_task_ran,"Test #3 Testing: 3.26:Yield for Test_Task")

   main_task_ran = TRUE;

   /* Destroy Test_Task, required by next test */
   result= _task_destroy(test_task_id);

   result = _mem_test();
   EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK),"Test #3 Testing: 3.27: _mem_test operation")
   if (result != MQX_OK) {
        //printf("\n\n...EXITING..._mem_test returned 0x%x", result);
         eunit_test_stop();
        _mqx_exit(0);
    }

 }

 
 void tc_3_test_task(void)
 {
    uint32_t          result;
    _mqx_uint        sched_policy, new_sched_policy, old_sched_policy;
    test_task_ran = TRUE;
    /* Keep track of Test_Task's time-slicing */
    test_task_start = current_time.SECONDS*1000 + current_time.MILLISECONDS;

    /* Get Processor's Scheduling Policy */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((_sched_get_policy(MQX_DEFAULT_TASK_ID, &sched_policy) == MQX_OK) && (sched_policy == MQX_SCHED_RR)),"Task #3 Test task: 3.1:Get Processor's Scheduling Policy")

    /* Get My Scheduling Policy using MQX_NULL_TASK_ID as input */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((_sched_get_policy(MQX_NULL_TASK_ID, &sched_policy) == MQX_OK) && (sched_policy == MQX_SCHED_RR)),"Task #3 Test task: 3.2:Get My Scheduling Policy using MQX_NULL_TASK_ID")

    /* Get My Scheduling Policy using my actual ID as input */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((_sched_get_policy(test_task_id, &sched_policy) == MQX_OK) && (sched_policy == MQX_SCHED_RR)),"Task #3 Test task: 3.3:Get My Scheduling Policy using my actual ID")

    /* Get Main_Task's Scheduling Policy */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((_sched_get_policy(main_task_id, &sched_policy) == MQX_OK) && (sched_policy == MQX_SCHED_FIFO)),"Task #3 Test task: 3.4:Get Main_Task's Scheduling Policy")

/* Test the operation of RR Scheduling Policy */
/* I should run for 50 msec, then dispatch */
    _time_get(&current_time);
    test_task_loop = TRUE;
    while (test_task_loop)  {
       /* Loop! */
       _time_get(&current_time);
       if ((current_time.SECONDS*1000 + current_time.MILLISECONDS
             - test_task_start) > wait_slice_time)
          break;   /* time-slice should have occurred by now */
    }
    test_task_ran = TRUE;
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, test_task_loop == FALSE, "Task #3 Test task: 3.5:Dispatch should occur")
    if (test_task_loop)  {
       /* dispatch didn't occur, report Failure */
       /* Inform Main_Task that time-slicing failed */
       test_task_loop = FALSE;
       _sched_yield();
       test_task_ran = TRUE;
    }
    else {
       /* time-slicing scheduling Passed */
    }

/* Set My Scheduling Policy using MQX_NULL_TASK_ID as input */
    old_sched_policy = 
       _sched_set_policy(MQX_NULL_TASK_ID, MQX_SCHED_FIFO);

    /* Keep track of my time-slicing */
    _time_get(&current_time);
    test_task_start = current_time.SECONDS*1000 + current_time.MILLISECONDS;

/* Get My Scheduling Policy using MQX_NULL_TASK_ID as input */
    result = _sched_get_policy(MQX_NULL_TASK_ID, &new_sched_policy);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK,"Task #3 Test task: 3.6:Get My Scheduling Policy using MQX_NULL_TASK_ID")

    /* Verify that Scheduling Policy was set properly */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((old_sched_policy == MQX_SCHED_RR) && (new_sched_policy == MQX_SCHED_FIFO)),"Task #3 Test task: 3.7:Verify that Scheduling Policy was set properly")

/* Get My Scheduling Policy using my actual ID as input */
    result = _sched_get_policy(test_task_id, &new_sched_policy);
    
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK,"Task #3 Test task: 3.8:Get My Scheduling Policy using my actual ID")

/* Verify that this is the correct Scheduling Policy */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, new_sched_policy == MQX_SCHED_FIFO,"Task #3 Test task: 3.9:Verify that this is the correct Scheduling Policy")


/* I will not give up the processor involuntarily (no time-slice) */
    _time_get(&current_time);
    test_task_loop = TRUE;
    while (test_task_loop)  {
       /* Loop! */
       _time_get(&current_time);
       /* loop enough time to confirm no time-slicing will occur */
       test_task_slice = current_time.SECONDS*1000 +
          current_time.MILLISECONDS - test_task_start;
       if (test_task_slice > 50*3)
          break;   /* time-slice should have occurred by now */
    }
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (test_task_loop != FALSE), "Task #3 Test task: 3.10: Check processor status")
    if (test_task_loop)  {
       /* Set this flag to FALSE indicating I'm giving up the
          processor voluntarily */
       test_task_loop = FALSE;
    }
    else {
       /* dispatch occurred, report Failure */
       /* Inform Main_Task that time-slicing failed */
       _sched_yield();
       test_task_ran = TRUE;
    }

/* Yield for Main_Task */
/* Main_Task will change its policy to RR */
    main_task_ran = FALSE;
    _sched_yield();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task,main_task_ran,"Task #3 Test task: 3.11:Yield for Main_Task")

    test_task_ran = TRUE;

/* Main_Task gave up processor involuntarily (time-slice) */
    _time_get(&current_time);
    main_task_slice = current_time.SECONDS*1000 +
       current_time.MILLISECONDS - main_task_start;

/* Verify TIME-SLICE operation using a flag */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (main_task_loop != FALSE), "Task #3 Test task: 3.12:Verify TIME-SLICE operation using a flag")
    if (main_task_loop)  {
       /* Main_Task's RR scheduling Passed */
       main_task_loop = FALSE;
    }
    else {
       /* Main_Task's RR scheduling Failed */
       /* Main_Task has set this flag to FALSE indicating it never
          gave up the processor */
    }

/* Change Main_Task's policy to FIFO and
   verify that no time-slicing will occur */
    old_sched_policy = 
       _sched_set_policy(main_task_id, MQX_SCHED_FIFO);

/* Get Main_Task's new Scheduling Policy */
    result = _sched_get_policy(main_task_id, &new_sched_policy);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK,"Task #3 Test task: 3.13:Get Main_Task's new Scheduling Policy")

    /* Verify that Scheduling Policy was set properly */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((old_sched_policy == MQX_SCHED_RR) && (new_sched_policy == MQX_SCHED_FIFO)),"Task #3 Test task: 3.14:Verify that Scheduling Policy was set properly")

    /* Yield for Main_Task */
    main_task_ran = FALSE;
    _sched_yield();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, main_task_ran,"Task #3 Test task: 3.15:Yield for Main_Task")

    test_task_ran = TRUE;
            
/* I should only get the processor when Main_Task gives it up
   voluntarily; otherwise FIFO Scheduling has failed */
    main_task_slice = current_time.SECONDS*1000 +
       current_time.MILLISECONDS - main_task_start;
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (main_task_loop == FALSE), "Task #3 Test task: 3.16: Get the processor")
    if (!main_task_loop)  {
       /* Main_Task's FIFO scheduling Passed */
       /* Main_Task has set this flag to FALSE indicating it
          gave up the processor voluntarily */
    }
    else {
       /* Main_Task's FIFO scheduling Failed */
       /* Main_Task has not set this flag to FALSE indicating it never
          gave up the processor voluntarily */
       /* Yield for Test_Task to report failure */
       _sched_yield();
       test_task_ran = TRUE;
    }

/* Yield for Main_Task; it will change my Policy to RR */
    main_task_ran = FALSE;
    _sched_yield();
    EU_ASSERT_REF_TC_MSG(tc_3_main_task,main_task_ran,"Task #3 Test task: 3.17:Yield for Main_Task, it will change my Policy to RR")

    test_task_ran = TRUE;

    /* Keep track of Test_Task's time-slicing */
    _time_get(&current_time);
    test_task_start = current_time.SECONDS*1000 + current_time.MILLISECONDS;

    /* Get My Scheduling Policy using MQX_NULL_TASK_ID as input */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((_sched_get_policy(MQX_NULL_TASK_ID, &sched_policy) == MQX_OK) && (sched_policy == MQX_SCHED_RR)),"Task #3 Test task: 3.18:Get My Scheduling Policy using MQX_NULL_TASK_ID")

    /* Get My Scheduling Policy using my actual ID as input */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ((_sched_get_policy(test_task_id, &sched_policy) == MQX_OK) && (sched_policy == MQX_SCHED_RR)),"Task #3 Test task: 3.19:Get My Scheduling Policy using my actual ID")

/* Test the operation of RR Scheduling Policy */
/* I should run for 50 msec, then dispatch */
    _time_get(&current_time);
    test_task_loop = TRUE;
    while (test_task_loop)  {
       /* Loop! */
       _time_get(&current_time);
       if ((current_time.SECONDS*1000 + current_time.MILLISECONDS
             - test_task_start) > wait_slice_time)
          break;   /* time-slice should have occurred by now */
    }
    test_task_ran = TRUE;
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, test_task_loop == FALSE,"Task #3 Test task: 3.20: Dispatch should occur")
    if (test_task_loop)  {
       /* dispatch didn't occur, report Failure */
       /* Inform Main_Task that time-slicing failed */
       test_task_loop = FALSE;
       _sched_yield();
       test_task_ran = TRUE;
    }
    else {
       /* time-slicing scheduling Passed */
    }
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST #4: The following code tests the following primitives:
//                                - _sched_get_rr_interval                    
//                                - _sched_get_rr_interval_ticks              
//                                - _sched_set_rr_interval                    
//                                - _sched_set_rr_interval_ticks              
//END---------------------------------------------------------------------
 void tc_4_main_task(void)
 {
    
   MQX_TICK_STRUCT rr_interval,  new_rr_interval, old_rr_interval;
   TIME_STRUCT     time;
   uint32_t         ms;
   _mqx_uint       result;
   _mqx_int        cmp_result;
   bool         overflow;    

/* Get Processor's RR interval */
   result = _sched_get_rr_interval_ticks(MQX_DEFAULT_TASK_ID, &rr_interval);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Test #4 Testing: 4.0:Get Processor's RR interval")

/* Verify that this is the correct RR interval */
   cmp_result = PSP_CMP_TICKS(&rr_interval, &_mqx_kernel_data->SCHED_TIME_SLICE);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, cmp_result == 0,"Test #4 Testing: 4.1:Verify that this is the correct RR interval")

   time.SECONDS = 0;
   result = _sched_get_rr_interval(MQX_DEFAULT_TASK_ID, &time.MILLISECONDS);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Test #4 Testing: 4.2:Get time slice in ms")

   _time_to_ticks(&time, &rr_interval);
   cmp_result = PSP_CMP_TICKS(&rr_interval, &_mqx_kernel_data->SCHED_TIME_SLICE);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, cmp_result == 0,"Test #4 Testing: 4.3:PSP_CMP_TICKS function should return 0 value")

/* Get My RR interval using MQX_NULL_TASK_ID as input */
   result = _sched_get_rr_interval_ticks(MQX_NULL_TASK_ID, &rr_interval);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Test #4 Testing: 4.4:Get My RR interval using MQX_NULL_TASK_ID")

/* Verify that this is the correct RR interval */
   cmp_result = PSP_CMP_TICKS(&rr_interval, &_mqx_kernel_data->ACTIVE_PTR->TIME_SLICE);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,cmp_result == 0,"Test #4 Testing: 4.5:Verify that this is the correct RR interval")

   result = _sched_get_rr_interval(MQX_NULL_TASK_ID, &time.MILLISECONDS);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Test #4 Testing: 4.6:_sched_get_rr_interval operation")

   _time_to_ticks(&time, &rr_interval);
   cmp_result = PSP_CMP_TICKS(&rr_interval, &_mqx_kernel_data->ACTIVE_PTR->TIME_SLICE);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, cmp_result == 0,"Test #4 Testing: 4.7:PSP_CMP_TICKS function should return 0 value")

/* Get My RR interval using my actual ID as input */
   result = _sched_get_rr_interval_ticks(main_task_id, &rr_interval);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Test #4 Testing: 4.8:Get My RR interval using my actual ID")

/* Verify that this is the correct RR interval */
   cmp_result = PSP_CMP_TICKS(&rr_interval, &_mqx_kernel_data->ACTIVE_PTR->TIME_SLICE);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,cmp_result == 0,"Test #4 Testing: 4.9:Verify that this is the correct RR interval")

/* Set Processor's RR interval */
   time.MILLISECONDS = TEST_RR_VALUE*10;
   _time_to_ticks(&time, &rr_interval);
   _sched_set_rr_interval_ticks(MQX_DEFAULT_TASK_ID, &rr_interval, &old_rr_interval);
/* Get Processor's RR interval */
   result = _sched_get_rr_interval_ticks(MQX_DEFAULT_TASK_ID, &new_rr_interval);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Test #4 Testing: 4.10:Get Processor's RR interval" )

/* Verify that RR interval was set properly */
   cmp_result = PSP_CMP_TICKS(&rr_interval, &new_rr_interval);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, cmp_result == 0,"Test #4 Testing: 4.11:Verify that RR interval was set properly")

   cmp_result = PSP_CMP_TICKS(&rr_interval, &_mqx_kernel_data->SCHED_TIME_SLICE);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, cmp_result == 0,"Test #4 Testing: 4.12:PSP_CMP_TICKS function should return 0 value")

   time.MILLISECONDS = TEST_RR_VALUE;
   ms = _sched_set_rr_interval(MQX_DEFAULT_TASK_ID, time.MILLISECONDS);
   result = _sched_get_rr_interval(MQX_DEFAULT_TASK_ID, &ms);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Test #4 Testing: 4.13: Get time slice in ms")

   EU_ASSERT_REF_TC_MSG(tc_4_main_task,ms == time.MILLISECONDS,"Test #4 Testing: 4.14:Compare time slice with time.MILLISECONDS")

   _time_to_ticks(&time, &rr_interval);
   cmp_result = PSP_CMP_TICKS(&rr_interval, &_mqx_kernel_data->SCHED_TIME_SLICE);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, cmp_result == 0,"Test #4 Testing: 4.15:PSP_CMP_TICKS function should return 0 value")

/* Create a test task, it should have same RR interval as processor's */
   test_task_id = _task_create(0, TEST_TASK, 4L);

/* Get Test_Task's RR interval */
   result = _sched_get_rr_interval_ticks(test_task_id, &rr_interval);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Test #4 Testing: 4.16:Get Test_Task's RR interval")

/* Verify that Test_Task's RR interval is equal to processor's */
   cmp_result = PSP_CMP_TICKS(&rr_interval, &_mqx_kernel_data->SCHED_TIME_SLICE);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, cmp_result == 0,"Test #4 Testing: 4.17:PSP_CMP_TICKS function should return 0 value")

   result = _sched_get_rr_interval(test_task_id, &time.MILLISECONDS);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Test #4 Testing: 4.18: Get time slice in ms")
   
   _time_to_ticks(&time, &rr_interval);
   cmp_result = PSP_CMP_TICKS(&rr_interval, &_mqx_kernel_data->SCHED_TIME_SLICE);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, cmp_result == 0,"Test #4 Testing: 4.19:PSP_CMP_TICKS function should return 0 value")

/* Yield for Test_Task to test new RR interval */
/* Test_Task should dispatch in TEST_RR_VALUE msec (time-slice) */
   test_task_ran = FALSE;
   _sched_yield();
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, test_task_ran,"Test #4 Testing: 4.20:Yield for Test_Task to test new RR interval")

   main_task_ran = TRUE;

   /* Test_Task gave up processor; check if involuntarily (time-slice) */
   _time_get(&current_time);
   test_task_slice = current_time.SECONDS*1000 + current_time.MILLISECONDS
                     - test_task_start;
   ms = _time_diff_milliseconds(&_mqx_kernel_data->SCHED_TIME_SLICE, 
      (MQX_TICK_STRUCT_PTR)(&_mqx_zero_tick_struct), &overflow);
   
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,( test_task_loop && !overflow && ( (test_task_slice <= (ms+BSP_ALARM_RESOLUTION) || (test_task_slice >= (ms-BSP_ALARM_RESOLUTION))))),"Test #4 Testing: 4.21:Test_Task gave up processor; check if involuntarily")
   if ( test_task_loop && !overflow &&
      ( (test_task_slice <= (ms+BSP_ALARM_RESOLUTION) ||
      (test_task_slice >= (ms-BSP_ALARM_RESOLUTION)))))
   {
      /* Test_Task's RR scheduling Passed */
      test_task_loop = FALSE;
   } else {
      /* Test_Task's RR scheduling Failed */
      /* Test_Task has set this flag to FALSE indicating it never
         gave up the processor */
   }

/* Yield for Test_Task; */
/* Test_Task should dispatch in 100 msec (time-slice) */
   test_task_ran = FALSE;
   _sched_yield();
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,test_task_ran,"Test #4 Testing: 4.22:Yield for Test_Task")

   main_task_ran = TRUE;

   /* Test_Task gave up processor; check if involuntarily (time-slice) */
   _time_get(&current_time);
   test_task_slice = current_time.SECONDS*1000 + current_time.MILLISECONDS
                     - test_task_start;

/* Verify TIME-SLICE operation using a flag */
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (test_task_loop && ((test_task_slice <= (100+BSP_ALARM_RESOLUTION)) || (test_task_slice >= (100-BSP_ALARM_RESOLUTION)))),"Test #4 Testing: 4.23:Verify TIME-SLICE operation using a flag")
   if (test_task_loop &&
      ((test_task_slice <= (100+BSP_ALARM_RESOLUTION)) ||
      (test_task_slice >= (100-BSP_ALARM_RESOLUTION))))  {
      /* Test_Task's RR scheduling Passed */
      test_task_loop = FALSE;
   }
   else {
      /* Test_Task's RR scheduling Failed */
      /* Test_Task has set this flag to FALSE indicating it never
         gave up the processor */
   }

   /* Yield for Test_Task; */
   test_task_ran = FALSE;
   _sched_yield();
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,test_task_ran,"Test #4 Testing: 4.24: Yield for Test_Task")

   main_task_ran = TRUE;

   result = _mem_test();
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK),"Test #4 Testing: 4.25: _mem_test operation")
   if (result != MQX_OK) {
        //printf("\n\n...EXITING..._mem_test returned 0x%x", result);
         eunit_test_stop();
        _mqx_exit(0);
    }

 }
 
 void tc_4_test_task(void)
 {
    uint32_t          ms;
    MQX_TICK_STRUCT  rr_interval, old_rr_interval, new_rr_interval;
    TIME_STRUCT      time;
    _mqx_int         result;
    bool          overflow;
    test_task_ran = TRUE;
            /* Keep track of Test_Task's time-slicing */
    test_task_start = current_time.SECONDS*1000 +
        current_time.MILLISECONDS;

    /* Get Processor's RR interval */
    result = _sched_get_rr_interval_ticks(MQX_DEFAULT_TASK_ID, &rr_interval);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Task #4 Test task: 4.1:Get Processor's RR interval")
    
    result = PSP_CMP_TICKS(&rr_interval, &_mqx_kernel_data->SCHED_TIME_SLICE);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Task #4 Test task: 4.2:PSP_CMP_TICKS function should return 0 value")

    /* Get My RR interval using MQX_NULL_TASK_ID as input */
    result = _sched_get_rr_interval_ticks(MQX_NULL_TASK_ID, &rr_interval);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Task #4 Test task: 4.3:Get My RR interval using MQX_NULL_TASK_ID")

    result = PSP_CMP_TICKS(&rr_interval, &_mqx_kernel_data->SCHED_TIME_SLICE);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == 0),"Task #4 Test task: 4.4:PSP_CMP_TICKS function should return 0 value")

    /* Get My RR interval using my actual ID as input */
    result = _sched_get_rr_interval_ticks(test_task_id, &rr_interval);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Task #4 Test task: 4.5:Get My RR interval using my actual ID")

    result = PSP_CMP_TICKS(&rr_interval, &_mqx_kernel_data->SCHED_TIME_SLICE);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == 0), "Task #4 Test task: 4.6:PSP_CMP_TICKS function should return 0 value")

    /* Get Main_Task's RR interval */
    result = _sched_get_rr_interval_ticks(main_task_id, &rr_interval);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Task #4 Test task: 4.7:Get Main_Task's RR interval")
 
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ((_time_diff_milliseconds(&rr_interval, (MQX_TICK_STRUCT_PTR)(&_mqx_zero_tick_struct), &overflow) == wait_slice_time) && (overflow == FALSE)),"Task #4 Test task: 4.8:_sched_get_rr_interval_ticks operation")

/* Test the operation of RR Scheduling Policy */
/* I should run for TEST_RR_VALUE msec, then dispatch */
    _ticks_to_time(&_mqx_kernel_data->SCHED_TIME_SLICE, &time);
    _time_get(&current_time);
    test_task_loop = TRUE;
    while (test_task_loop)  {
       /* Loop! */
       _time_get(&current_time);
       if ((current_time.SECONDS*1000 + current_time.MILLISECONDS
             - test_task_start) > time.MILLISECONDS)
          break;   /* time-slice should have occurred by now */
    }
    test_task_ran = TRUE;
    /* Keep track of Test_Task's time-slicing */
    _time_get(&current_time);
    test_task_start = current_time.SECONDS*1000 +
        current_time.MILLISECONDS;
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, test_task_loop == FALSE, "Task #4 Test task: 4.9:Keep track of Test_Task's time-slicing")
    if (test_task_loop)  {
       /* dispatch didn't occur, report Failure */
       /* Inform Main_Task that time-slicing failed */
       test_task_loop = FALSE;
       _sched_yield();
       test_task_ran = TRUE;
    }
    else {
       /* time-slicing scheduling Passed */
    }

/* Set My RR interval to 100 ms */
    rr_interval = _mqx_zero_tick_struct;
    _time_add_msec_to_ticks(&rr_interval, 100);
    _sched_set_rr_interval_ticks(test_task_id, &rr_interval, &old_rr_interval);

    /* Verify that RR interval was set properly */
    result = PSP_CMP_TICKS(&old_rr_interval, &_mqx_kernel_data->SCHED_TIME_SLICE);
    result |= PSP_CMP_TICKS(&rr_interval, &_mqx_kernel_data->ACTIVE_PTR->TIME_SLICE);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, !result,"Task #4 Test task: 4.10:Verify that RR interval was set properly")

    /* Get My RR interval using MQX_NULL_TASK_ID as input */
    result = _sched_get_rr_interval_ticks(MQX_NULL_TASK_ID, &new_rr_interval);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Task #4 Test task: 4.11:Get My RR interval using MQX_NULL_TASK_ID")

    result = PSP_CMP_TICKS(&new_rr_interval, &_mqx_kernel_data->ACTIVE_PTR->TIME_SLICE);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, !result,"Task #4 Test task: 4.12:PSP_CMP_TICKS function should return 0 value")

    /* Get My RR interval using my actual ID as input */
    result = _sched_get_rr_interval_ticks(test_task_id, &new_rr_interval);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Task #4 Test task: 4.13:Get My RR interval using my actual ID")

    result = PSP_CMP_TICKS(&new_rr_interval, &_mqx_kernel_data->ACTIVE_PTR->TIME_SLICE);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, !result,"Task #4 Test task: 4.14:PSP_CMP_TICKS function should return 0 value")

    /* Get Processor's RR interval */
    result = _sched_get_rr_interval_ticks(MQX_DEFAULT_TASK_ID, &new_rr_interval);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Task #4 Test task: 4.15:Get Processor's RR interval")

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ((_time_diff_milliseconds(&new_rr_interval, (MQX_TICK_STRUCT_PTR)(&_mqx_zero_tick_struct), &overflow) == TEST_RR_VALUE) && (overflow == FALSE)),"Task #4 Test task: 4.16:_sched_get_rr_interval_ticks operation")

    /* Get Main_Task's RR interval */
    result = _sched_get_rr_interval_ticks(main_task_id, &new_rr_interval);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK,"Task #4 Test task: 4.17:Get Main_Task's RR interval")

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, ((_time_diff_milliseconds(&new_rr_interval, (MQX_TICK_STRUCT_PTR)(&_mqx_zero_tick_struct), &overflow) == wait_slice_time) && (overflow == FALSE)),"Task #4 Test task: 4.18:_sched_get_rr_interval_ticks operation")

/* Test the operation of RR Scheduling Policy */
/* I should run for 100 msec, then dispatch */
    _time_get(&current_time);
    test_task_loop = TRUE;
    while (test_task_loop)  {
       /* Loop! */
       ms = _time_diff_milliseconds(&_mqx_kernel_data->ACTIVE_PTR->TIME_SLICE,
          (MQX_TICK_STRUCT_PTR)(&_mqx_zero_tick_struct), &overflow);
       _time_get(&current_time);
       if (((current_time.SECONDS*1000 + current_time.MILLISECONDS
             - test_task_start) > ms) || overflow)
       {
          break;   /* time-slice should have occurred by now */
       }
    }
    test_task_ran = TRUE;
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, test_task_loop == FALSE,"Task #4 Test task: 4.19:Test the operation of RR Scheduling Policy")
    if (test_task_loop)  {
       /* dispatch didn't occur, report Failure */
       /* Inform Main_Task that time-slicing failed */
       test_task_loop = FALSE;
       _sched_yield();
       test_task_ran = TRUE;
    }
    else {
       /* time-slicing scheduling Passed */
    }
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_5_main_task
// Comments     : TEST #5: The following code tests the following primitive:
//                         - _sched_yield 
//
//END---------------------------------------------------------------------
 void tc_5_main_task(void)
 {
   _mqx_uint       result;

   /* Yield for Test_Task */
   test_task_ran = FALSE;
   _sched_yield();
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, test_task_ran,"Test #5 Testing: 5.1:Yield for Test_Task ")

   /* Back from Dispatch */
   main_task_ran = TRUE;

   /* Yield again for Test_Task */
   test_task_ran = FALSE;
   _sched_yield();
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, test_task_ran,"Test #5 Testing: 5.2:Yield again for Test_Task")

   /* Back from Dispatch */
   main_task_ran = TRUE;

   result = _mem_test();
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.3: ")
   if (result != MQX_OK) {
        //printf("\n\n...EXITING..._mem_test returned 0x%x", result);
         eunit_test_stop();
        _mqx_exit(0);
    }
 }
 void tc_5_test_task(void)
 {
    test_task_ran = TRUE;
    /* Yield for Main_Task */
    main_task_ran = FALSE;
    _sched_yield();
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, main_task_ran,"Task #5 Test task: 5.1:Yield for Main_Task")

    test_task_ran = TRUE;
}
 
 /*END-----------------------------------*/
 
//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_sched)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1 - Testing:_sched_get_max_priority"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2 - Testing:_sched_get_min_priority"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3 - Testing:_sched_get_policy, _sched_set_policy"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4 - Testing:_sched_get_rr_interval, _sched_get_rr_interval_ticks, _sched_set_rr_interval, _sched_set_rr_interval_ticks"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST # 5 - Testing:_sched_yield")
 EU_TEST_SUITE_END(suite_sched)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_sched, " - Test sched suite")
 EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------
 
/*TASK*--------------------------------------------------------------
* 
* Task Name :  main_task
* Comments  :  This is the Main Task for testing "sched" primitives.
*              For certain primitives, processing is yielded to a
*              secondary task (Test_Task) in order to test these
*              primitives from a different task (e.g. daughter task,
*              or lower/higher priority task).
*END*--------------------------------------------------------------*/

void main_task
   (
      uint32_t dummy
   )
{
    _mqx_uint result;
     wait_slice_time = 10*1000/BSP_ALARM_FREQUENCY;//calculating slice time, it depend on ticks time = 1000/BSP_ALARM_FREQUENCY
    _time_set_hwtick_function(0,0);

    /* Get my task ID */
    main_task_id = _task_get_id();

   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();

    /* Destroy Test_Task */
    result= _task_destroy(test_task_id);

    _time_delay_ticks(4);
     eunit_test_stop();
    _mqx_exit(0);
}



/*TASK*--------------------------------------------------------------
* 
* Task Name :  test_task
* Comments  :  This is a secondary task, used to verify functionality
*              of some primitives from a different task.
* Input     :  This task accepts a test index as a creation parameter,
*              which corresponds to the test numbers in the Main Task.
*END*--------------------------------------------------------------*/

void test_task
   (
      uint32_t initial_data
   )
{
   
   uint32_t          test_index;
   _time_get(&current_time);
   
   test_index = initial_data;
   while (test_index) {

      switch (test_index) {
         case 3:  /* Test #3 */
            tc_3_test_task();
            test_index = 4;
            break;

         case 4:  /* Test #4 */
            tc_4_test_task();
            test_index = 5;
            break;

         case 5:  /* Test #5 */
            tc_5_test_task();
            test_index = 6;
            break;
      }

      main_task_ran = FALSE;
      _sched_yield();
      
      switch (test_index) {
         case 3:  /* Test #3 */
            EU_ASSERT_REF_TC(tc_3_main_task, main_task_ran)
            break;

         case 4:  /* Test #4 */
            EU_ASSERT_REF_TC(tc_4_main_task, main_task_ran)
            break;

         case 5:  /* Test #5 */
            EU_ASSERT_REF_TC(tc_5_main_task, main_task_ran)
            break;
      }

      test_task_ran = TRUE;
      
   }

}

/* EOF */
