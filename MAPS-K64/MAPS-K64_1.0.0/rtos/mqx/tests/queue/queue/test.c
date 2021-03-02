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
* $Version : 3.7.3.0$
* $Date    : Mar-24-2011$
*
* Comments:
*
*   This file contains code for the MQX Queue component verification
*   program.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
// #include <log.h>
// #include <klog.h>
//#include <test_module.h>
#include "test.h"

#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME test
#define PRINT_ON    0

#define _PRINTING_ALLOWED_

//------------------------------------------------------------------------
// Test suite function prototipe
extern void tc_1_main_task(void);//TEST # 1 - Testing _queue_init primitive
extern void tc_2_main_task(void);//TEST # 2 - Testing dequeue and enqueue primitives on a queue with max size
extern void tc_3_main_task(void);//TEST # 3 - Testing _queue_insert primitive
extern void tc_4_main_task(void);//TEST # 4 - Testing _queue_insert and _queue_next with NULL
//------------------------------------------------------------------------
TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK,  main_task, 3000,  9,  "main",  MQX_AUTO_START_TASK},
   { 0,          0,         0,     0,  0,       0 }
};

QUEUE_STRUCT            q_no_max, q_max;
QUEUE_ELEMENT_STRUCT    elements[20], * e_ptr, * e2_ptr; 
QUEUE_STRUCT_PTR        qmax_ptr, qnomax_ptr;  




#ifndef _PRINTING_ALLOWED_
#undef printf
#define printf my_printf

_mqx_uint my_printf( char *, ... ) {
{
   return 0;
}
#endif


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name: result_code
* Return Value : none
* Comments     : 
*    report a result error in a test
*
*END*----------------------------------------------------------------------*/
void result_code(char * error1,_mqx_uint result)
{
#if PRINT_ON
   printf("\n   %s failed, error code 0x%X, %s", error1,
              result, _mqx_get_task_error_code_string(result));
#endif
}

/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name: TEST_queue_test
* Return Value : none
* Comments     : 
*    test the integrity of the queues, report any errors
*
*END*----------------------------------------------------------------------*/
void TEST_queue_test(QUEUE_STRUCT_PTR qnomax_ptr, 
                        QUEUE_STRUCT_PTR qmax_ptr, 
                        uint16_t test_no,
                        uint16_t case_no)
{
    _mqx_uint result;
    char      error[50];
    void     *element_in_error;

    result = _queue_test(qnomax_ptr,&element_in_error);
    switch (test_no) {
    case 1:
        if (case_no == 1)
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.1: _queue_test operation")
        else
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.9: _queue_test operation")
        break;
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.14: _queue_test operation")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing: 3.15: _queue_test operation")
        break;
    default:
        break;
    }
    if (result != MQX_OK) {
        sprintf(error,"%d.%d _queue_test ",test_no,case_no);
        result_code(error,result);
#if PRINT_ON
        printf("\n     unlimited size queue, element in error is 0x%P",element_in_error);
#endif
    }
    result = _queue_test(qmax_ptr,&element_in_error);
    switch (test_no) {
    case 1:
        if (case_no == 1)
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.2: _queue_test operation")
        else
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.10: _queue_test operation")
        break;
    case 2:
        EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.15: _queue_test operation")
        break;
    case 3:
        EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing: 3.16: _queue_test operation")
        break;
    default:
        break;
    }
    if (result != MQX_OK) {
        sprintf(error,"%d.%d _queue_test ",test_no,case_no+1);
        result_code(error,result);
#if PRINT_ON
        printf("\n     limited size queue, element in error is 0x%P",element_in_error);
#endif
    }
}

/******   Test cases body definition      *******/

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST # 1 - Testing _queue_init primitive 
//
//END---------------------------------------------------------------------
 void tc_1_main_task(void)
 {
       _mqx_uint               size;
       bool                 bool_result;
       qmax_ptr = &q_max;
       qnomax_ptr = &q_no_max;

       /*
       ** initialize a queue
       ** verify the integrity of the queue
       */
       _queue_init(qnomax_ptr,0);
       _queue_init(qmax_ptr,10);
       TEST_queue_test(qnomax_ptr, qmax_ptr, 1,1);

       size = _queue_get_size(qnomax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_1_main_task, size == 0,"Test #1 Testing: 1.3: _queue_get_size should return 0 with unlimited size queue")

       size = _queue_get_size(qmax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_1_main_task, size == 0,"Test #1 Testing: 1.4: _queue_get_size should return 0 with limited size queue")

       bool_result = _queue_is_empty(qnomax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_1_main_task, bool_result != FALSE,"Test #1 Testing: 1.5: _queue_is_empty with unlimited size queue operation")

       bool_result = _queue_is_empty(qmax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_1_main_task, bool_result != FALSE,"Test #1 Testing: 1.6:_queue_is_empty with limited size queue operation")

       e_ptr = _queue_head(qnomax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_1_main_task, e_ptr == NULL,"Test #1 Testing: 1.7: _queue_head with unlimited size queue should return NULL")

       e_ptr = _queue_head(qmax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_1_main_task, e_ptr == NULL,"Test #1 Testing: 1.8: _queue_head with limited size queue should return NULL")

       TEST_queue_test(qnomax_ptr, qmax_ptr, 1,9);  
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST # 2 - Testing dequeue and enqueue primitives on a queue with max size 
//
//END---------------------------------------------------------------------
  void tc_2_main_task(void)
 {
       _mqx_uint               i,size;
       bool                 bool_result;

       /*
       **  enqueue the max no. of entries
       **  calls: _queue_enqueue, _queue_dequeue, _queue_insert, 
       **        _queue_get_size, _queue_test, _queue_is_empty
       **        _queue_head _queue_next
       */
       /*
       ** fill up the queue with a maximum size 
       */

       for (i=0; i < 11; i++) {
          bool_result = _queue_enqueue(qmax_ptr,&elements[i]);
          if (bool_result == FALSE) {
            break;
          }
       }
       EU_ASSERT_REF_TC_MSG(tc_2_main_task,i == 10,"Test #2 Testing: 2.1: Number of elements on a queue should equal 10")

       /*
       ** check the size of the queue, should be 10
       ** queue is not empty 
       */
       size = _queue_get_size(qmax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_2_main_task, size == 10,"Test #2 Testing: 2.2: Queue should have been full")

       bool_result = _queue_is_empty(qmax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == FALSE,"Test #2 Testing: 2.3: _queue_is_empty should reports that queue is empty")

       /*
       ** get the head of the queue
       ** try to insert an element after head
       ** should fail because queue is full
       */
       e_ptr = _queue_head(qmax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_2_main_task, e_ptr == &elements[0],"Test #2 Testing: 2.4: _queue_head get element at head of queue")

       bool_result = _queue_insert(qmax_ptr,e_ptr,&elements[10]);
       EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result == FALSE,"Test #2 Testing: 2.5: _queue_insert should have failed")

       /*
       ** use _queue_head and _queue_next to verify the elements on the queue
       */
       e_ptr = _queue_head(qmax_ptr);
       for (i=0; i < 10; i++) {         
          if (e_ptr != &elements[i]) { 
            break;
          } else {
            e_ptr = _queue_next(qmax_ptr,e_ptr);
          }
       }
       EU_ASSERT_REF_TC_MSG(tc_2_main_task, (i >= 10),"Test #2 Testing: 2.6: Use _queue_head and _queue_next to verify the elements on the queue")
       EU_ASSERT_REF_TC_MSG(tc_2_main_task, e_ptr == NULL,"Test #2 Testing: 2.7: _queue_next should have returned NULL")

       /*
       ** dequeue 11 items from queue, 11th should fail 
       */
       i=1;
       e_ptr = _queue_dequeue(qmax_ptr);
       if (e_ptr != &elements[0]) {
          e_ptr = NULL;
       }
       while (e_ptr) {
          e_ptr = _queue_dequeue(qmax_ptr);
          if (e_ptr != &elements[i]) {
            break;
          }
          i++;
       }
       EU_ASSERT_REF_TC_MSG(tc_2_main_task,i==10,"Test #2 Testing: 2.8: should have dequeued 10 elements")
       EU_ASSERT_REF_TC_MSG(tc_2_main_task, e_ptr == NULL,"Test #2 Testing: 2.9: last dequeue should have returned NULL")

       /*
       ** enqueue another entry to verify that it can be done
       ** do a queue test
       */ 

       bool_result = _queue_is_empty(qmax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result != FALSE,"Test #2 Testing: 2.10: Queue should have indicated empty")

       bool_result = _queue_enqueue(qmax_ptr,&elements[0]);
       EU_ASSERT_REF_TC_MSG(tc_2_main_task, bool_result != FALSE,"Test #2 Testing: 2.11: element should be enqueued")

       e_ptr = _queue_dequeue(qmax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_2_main_task, e_ptr == &elements[0],"Test #2 Testing: 2.12: _queue_dequeue operation")

       TEST_queue_test(qnomax_ptr, qmax_ptr, 2,14); 
}
 //TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST # 3 - Testing _queue_insert primitive 
//
//END---------------------------------------------------------------------
  void tc_3_main_task(void)
 {     
       _mqx_uint               i,size;
       bool                 bool_result;

       /*
       **  enqueue entries
       **  calls: _queue_enqueue, _queue_dequeue, _queue_unlink
       **        _queue_get_size, _queue_test, _queue_is_empty
       **        _queue_head, _queue_next
       */

       /*
       ** enqueue elements on the queue
       */

       for (i=0; i < 11; i++) {
          bool_result = _queue_enqueue(qnomax_ptr,&elements[i]);
          if (bool_result == FALSE) {
            break;
          }
       }
       EU_ASSERT_REF_TC_MSG(tc_3_main_task,i == 11,"Test #3 Testing: 3.1: Enqueue elements on the queue")

       /*
       ** check the size of the queue, should be 11
       ** queue is not empty 
       */
       size = _queue_get_size(qnomax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_3_main_task,size == 11,"Test #3 Testing: 3.2: Queue should be 11 items")

       bool_result = _queue_is_empty(qnomax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == FALSE,"Test #3 Testing: 3.3: _queue_is_empty should reports that queue is empty")

       /*
       ** get the head of the queue
       ** try to insert an element after head
       ** should succeed because queue has no limit
       */
       e_ptr = _queue_head(qnomax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_3_main_task, e_ptr == &elements[0],"Test #3 Testing: 3.4: _queue_head get the element at head of queue")

       bool_result = _queue_insert(qnomax_ptr,e_ptr,&elements[11]);
       EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == TRUE,"Test #3 Testing: 3.5: _queue_insert should have succeeded")

       size = _queue_get_size(qnomax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_3_main_task,size == 12,"Test #3 Testing: 3.6: Queue should be 12 items")

       _queue_unlink(qnomax_ptr,&elements[11]);
       size = _queue_get_size(qnomax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_3_main_task, size == 11,"Test #3 Testing: 3.7: Queue should 11 items")

       /*
       ** use _queue_head and _queue_next to verify the elements on the queue
       */
       e_ptr = _queue_head(qnomax_ptr);
       for (i=0; i < 11; i++) {
          if (e_ptr != &elements[i]) { 
            break;
          } else {
            e_ptr = _queue_next(qnomax_ptr,e_ptr);
          }
       }
       EU_ASSERT_REF_TC_MSG(tc_3_main_task, (i >= 11),"Test #3 Testing: 3.8: use _queue_head and _queue_next to verify the elements on the queue")
       EU_ASSERT_REF_TC_MSG(tc_3_main_task, e_ptr == NULL,"Test #3 Testing: 3.9: _queue_next should have returned NULL")

       /*
       ** dequeue 11 items from queue, 11th should fail 
       */
       e_ptr = _queue_head(qnomax_ptr);
       e_ptr = _queue_next(qnomax_ptr,e_ptr);
       while (e_ptr) {
          e2_ptr = _queue_next(qnomax_ptr,e_ptr);
          _queue_unlink(qnomax_ptr,e_ptr);
          e_ptr = e2_ptr;
       }
       size = _queue_get_size(qnomax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_3_main_task, size == 1,"Test #3 Testing: 3.10: should have 1 element left on queue")

       e_ptr = _queue_head(qnomax_ptr);
       _queue_unlink(qnomax_ptr,e_ptr);
       EU_ASSERT_REF_TC_MSG(tc_3_main_task, e_ptr == &elements[0],"Test #3 Testing: 3.11: last dequeue should have returned head item")

       /*
       ** queue should now be empty
       */ 

       bool_result = _queue_is_empty(qnomax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_3_main_task, bool_result == TRUE,"Test #3 Testing: 3.12: queue should have indicated empty")

       /*
       ** enqueue/dequeue an entry on queue to ensure that it is still valid 
       ** after the unlink of all elements was done
       */
       _queue_enqueue(qnomax_ptr,&elements[0]);
       e_ptr = _queue_dequeue(qnomax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_3_main_task, e_ptr == &elements[0],"Test #3 Testing: 3.14: enqueue/dequeue an entry on queue")

       TEST_queue_test(qnomax_ptr, qmax_ptr, 3,15);
}
 //TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST # 4 - Testing _queue_insert and _queue_next with NULL 
//
//END---------------------------------------------------------------------
  void tc_4_main_task(void)
 {
       _mqx_uint               i;
       bool                 bool_result;

       /*
       ** enqueue elements on the queue
       */
       
       for (i=0; i < 11; i++) {
          bool_result = _queue_enqueue(qnomax_ptr,&elements[i]);
          if (bool_result == FALSE) {
            break;
          }
       }

       bool_result = _queue_insert(qnomax_ptr,NULL,&elements[11]);
       EU_ASSERT_REF_TC_MSG(tc_4_main_task, bool_result != FALSE,"Test #4 Testing: 4.1: _queue_insert should return TRUE value")

       e_ptr = _queue_dequeue(qnomax_ptr);
       EU_ASSERT_REF_TC_MSG(tc_4_main_task, e_ptr == &elements[11],"Test #4 Testing: 4.2:_queue_insert should place item at head of queue")

       e_ptr = _queue_next(qnomax_ptr,NULL);
       EU_ASSERT_REF_TC_MSG(tc_4_main_task, e_ptr == NULL,"Test #4 Testing: 4.3: _queu_next operation")
}


//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_queue)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1 - Testing _queue_init primitive"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2 - Testing dequeue and enqueue primitives on a queue with max size"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3 - Testing _queue_insert primitive"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4 - Testing _queue_insert and _queue_next with NULL")
 EU_TEST_SUITE_END(suite_queue)

// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_queue, " - Test queue")
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
      uint32_t param
      /* [IN] The MQX task parameter */
   )
{ /* Body */
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();

    _time_delay_ticks(10);  /* waiting for all loggings to be printed out */
    _mqx_exit(0);
} /* endbody */



/* EOF */
