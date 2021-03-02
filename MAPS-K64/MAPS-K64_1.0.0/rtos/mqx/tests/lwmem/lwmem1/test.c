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
* $Date    : Mar-30-2012$
*
* Comments:
*
*   This file contains the source for the lwmem testing.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <lwmem.h>
#include <lwmem_prv.h>
// #include <klog.h>
// #include <lwlog.h>
#include "util.h"
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test

/* General defines */
#define MAIN_TASK  10
#define TEST_TASK  11

#define TEST_ALLOC_SIZE (200)

#define TEST_SIZE    2048

#define ALLOC_SIZE   TEST_SIZE+4
#define FALLOUT_SIZE 16
#define TOTAL_SIZE ALLOC_SIZE+FALLOUT_SIZE

extern void    test_task(uint32_t);
extern void    main_task(uint32_t);

void tc_1_main_task(void);// Test #1 - Testing the function _lwmem_test()
void tc_2_main_task(void);// Test #2 - Testing the function _lwmem_alloc()
void tc_3_main_task(void);// Test #3 - Testing the function _lwmem_alloc_system_from()
void tc_4_main_task(void);// Test #4 - Testing the function _lwmem_free()
void tc_5_main_task(void);// Test #5 - Testing the function _lwmem_alloc_zero()
void tc_6_main_task(void);// Test #6 - Testing the function _lwmem_alloc_system_zero()
void tc_7_main_task(void);// Test #7 - Testing the function _lwmem_get_size()
void tc_8_main_task(void);// Test #8 - Testing the function _lwmem_transfer()
void tc_9_main_task(void);// Test #9 - Testing the function _lwmem_test()
                         

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
{ MAIN_TASK, main_task,  2000,   7, "Main_Task", MQX_AUTO_START_TASK},
{ TEST_TASK, test_task,  1000,   7, "Test_Task", 0                  },
{ 0,         0,          0,      0, 0,           0                  }
};

_task_id global_test_task_id;
_task_id global_main_task_id;
void  *global_lwmem_ptr;

LWMEM_POOL_STRUCT pool;
void             *mem_alloc_pool_ptr;

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : test_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void test_task
   (
      uint32_t dummy
   )
{
    _mqx_uint result;

    global_lwmem_ptr = _lwmem_alloc( TEST_ALLOC_SIZE );
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (global_lwmem_ptr != NULL), "Test #8 Testing: 8.1: _lwmem_alloc operation")

    result = _lwmem_transfer( global_lwmem_ptr, global_test_task_id,
      global_main_task_id);

    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: 8.2: _lwmem_transfer operation")

    _task_block();
}


//----------------- Begin Testcases --------------------------------------
KERNEL_DATA_STRUCT_PTR      kernel_data;
LWMEM_BLOCK_STRUCT_PTR      block_ptr;
_task_id                    system_task_id;
_lwmem_pool_id              pool_id;
_lwmem_pool_id              test_pool_id;
unsigned char                      *to_ptr;
unsigned char                      *from_ptr;
void                       *error_block_ptr;

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test function: _mem_alloc
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint       result;
    
    result = _lwmem_test(&test_pool_id, &error_block_ptr);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.1: lwmem test operation")
   if (result != MQX_OK ) {
      _task_block();
   } /* Endif */

   mem_alloc_pool_ptr = _mem_alloc(20 * TEST_ALLOC_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, mem_alloc_pool_ptr != NULL, "Test #1: Testing 1.2: Allocate memory operation")
   if (mem_alloc_pool_ptr == NULL) {
      _task_block();
   } /* Endif */
   pool_id = _lwmem_create_pool(&pool, mem_alloc_pool_ptr, 20 * TEST_ALLOC_SIZE);
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_2_main_task
* Comments     : TEST #2: Test function: _lwmem_alloc
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
   _mqx_uint        result;
   
   to_ptr = (unsigned char *)_lwmem_alloc_from(pool_id, TEST_ALLOC_SIZE );

   EU_ASSERT_REF_TC_MSG(tc_2_main_task, to_ptr != NULL, "Test #2: Testing 2.1: Lwmem_alloc_from operation")
   if (!to_ptr) {
      _task_block();
   } /* Endif */

   block_ptr = (LWMEM_BLOCK_STRUCT_PTR) (to_ptr - sizeof(LWMEM_BLOCK_STRUCT));
   result = BUILD_TASKID(kernel_data->INIT.PROCESSOR_NUMBER, block_ptr->U.S.TASK_NUMBER);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == global_main_task_id, "Test #2: Testing 2.2: Check resource of main task")
   if ( result != global_main_task_id ) {
      _task_block();
   } /* Endif */

   result = _lwmem_test(&test_pool_id, &error_block_ptr);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.3: lwmem test operation")
   if (result != MQX_OK ) {
      _task_block();
   } /* Endif */
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_3_main_task
* Comments     : TEST #3: Test function: _lwmem_alloc_system
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    _mqx_uint       result;

    from_ptr = (unsigned char *)_lwmem_alloc_system_from(pool_id, TEST_ALLOC_SIZE );

    EU_ASSERT_REF_TC_MSG(tc_3_main_task, from_ptr != NULL, "Test #3: Testing 3.1: _lwmem_alloc_system_from() failed operation")
    if (!from_ptr) {
        _task_block();
    } /* Endif */

    block_ptr = (LWMEM_BLOCK_STRUCT_PTR) ((char *)from_ptr -
      sizeof(LWMEM_BLOCK_STRUCT));
    result = BUILD_TASKID(kernel_data->INIT.PROCESSOR_NUMBER, block_ptr->U.S.TASK_NUMBER);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == system_task_id, "Test #3: Testing 3.2: System task own memory block ")
    if ( result != system_task_id ) {       
        _task_block();
    } /* Endif */

    result = _lwmem_test(&test_pool_id, &error_block_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.3:lwmem test operation")
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_4_main_task
* Comments     : TEST #4: Test function: _lwmem_free
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
   _mqx_uint        result;
   unsigned char           *r1_ptr;
   unsigned char           *r2_ptr;
   unsigned char           *r3_ptr;
   unsigned char           *r4_ptr;
   unsigned char           *r5_ptr;

   result = _lwmem_free( to_ptr );

   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.1: lwmem free operation")
   if (result != MQX_OK ) {
      _task_block();
   } /* Endif */

   result = _lwmem_free( from_ptr );
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.2: lwmem free operation")
   if ( result != MQX_OK ) {
      _task_block();
   } /* Endif */

   result = _lwmem_test(&test_pool_id, &error_block_ptr);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.3: lwmem test operation")
   if (result != MQX_OK ) {
      _task_block();      
   } /* Endif */

   /* Check co-allescing */
   from_ptr = (unsigned char *)_lwmem_alloc_system_from(pool_id, TEST_ALLOC_SIZE );
   to_ptr = (unsigned char *)_lwmem_alloc_system_from(pool_id, TEST_ALLOC_SIZE );
   r1_ptr = (unsigned char *)_lwmem_alloc_system_from(pool_id, TEST_ALLOC_SIZE );
   r2_ptr = (unsigned char *)_lwmem_alloc_system_from(pool_id, TEST_ALLOC_SIZE );
   r3_ptr = (unsigned char *)_lwmem_alloc_system_from(pool_id, TEST_ALLOC_SIZE );
   r4_ptr = (unsigned char *)_lwmem_alloc_system_from(pool_id, TEST_ALLOC_SIZE );
   r5_ptr = (unsigned char *)_lwmem_alloc_system_from(pool_id, TEST_ALLOC_SIZE );
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (from_ptr && to_ptr && r1_ptr && r2_ptr && r3_ptr || r4_ptr), "Test #4: Testing 4.4: _lwmem_alloc_system_from operation")
   if (!from_ptr || !to_ptr || !r1_ptr || !r2_ptr || !r3_ptr || !r4_ptr) {      
      _task_block();
   } /* Endif */
   result = _lwmem_free(from_ptr);
   result |= _lwmem_free(r1_ptr);
   result |= _lwmem_free(r3_ptr);
   result |= _lwmem_free(r2_ptr);
   result |= _lwmem_free(r4_ptr);
   result |= _lwmem_free(to_ptr);
   
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.5: lwmem free operation")
   if (result != MQX_OK) {
      _task_block();
   } /* Endif */
   to_ptr = (unsigned char *)_lwmem_alloc_system_from(pool_id,
      (TEST_ALLOC_SIZE * 6)  + (5 * sizeof(LWMEM_BLOCK_STRUCT)));
      
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, to_ptr == from_ptr, "Test #4: Testing 4.6: _lwmem_free() coallescing operation")
   if (to_ptr != from_ptr) {
      _task_block();
   } /* Endif */
   result = _lwmem_free(r5_ptr);
   result = _lwmem_free(to_ptr);

   _lwmem_set_default_pool(pool_id);
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_5_main_task
* Comments     : TEST #5: Test function: _lwmem_alloc_zero
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
   _mqx_uint        result;
   _mqx_uint        i;

   to_ptr = (unsigned char *)_lwmem_alloc_zero( TEST_ALLOC_SIZE );

   EU_ASSERT_REF_TC_MSG(tc_5_main_task, to_ptr != NULL, "Test #5: Testing 5.1: lwmem alloc zero operation")
   if (!to_ptr) {
      _task_block();
   } /* Endif */

   /* Check to see if all allocated bytes are zero */
   for( i = 0; i < TEST_ALLOC_SIZE; i++ ) {
      EU_ASSERT_REF_TC_MSG(tc_5_main_task, (to_ptr[i] == 0), "Test #5: Testing 5.2: Check to see if all allocated bytes are zero")
      if ( to_ptr[i] != 0 ) {
         _task_block();
      } /* Endif */
   } /* Endfor */

   result = _lwmem_free( to_ptr );
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.3: lwmem free operation")
   if (result != MQX_OK ) {
      _task_block();
   } /* Endif */

   result = _lwmem_test(&test_pool_id, &error_block_ptr);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.4: lwmem test operation")
   if (result != MQX_OK ) {
      _task_block();
   } /* Endif */
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_6_main_task
* Comments     : TEST #6: Test function: _lwmem_alloc_system_zero
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
   _mqx_uint        result;
   _mqx_uint        i;

   to_ptr = (unsigned char *)_lwmem_alloc_system_zero( TEST_ALLOC_SIZE );

   EU_ASSERT_REF_TC_MSG(tc_6_main_task, to_ptr != NULL, "Test #6: Testing 6.1: lwmem_alloc_system_zero() operation")
   if (!to_ptr) {
      _task_block();
   } /* Endif */

   /* Check to see if all allocated bytes are zero */
   for( i = 0; i < TEST_ALLOC_SIZE; i++ ) {
      EU_ASSERT_REF_TC_MSG(tc_6_main_task, (to_ptr[i] == 0), "Test #6: Testing 6.2: Check to see if all allocated bytes are zero")
      if ( to_ptr[i] != 0 ) {
         _task_block();
      } /* Endif */
   } /* Endfor */

   result = _lwmem_free( to_ptr );
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.3: lwmem free operation")
   if (result != MQX_OK ) {
      _task_block();
   } /* Endif */

   result = _lwmem_test(&test_pool_id, &error_block_ptr);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.4: lwmem test operation")
   if (result != MQX_OK ) {
      _task_block();
   } /* Endif */
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_7_main_task
* Comments     : TEST #7: Test function: _lwmem_get_size
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
   _mqx_uint        i, j;

   to_ptr = (unsigned char *)_lwmem_alloc( TEST_ALLOC_SIZE );
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, to_ptr != NULL, "Test #7: Testing 7.1: lwmem_get_size operation")
   if ( to_ptr == NULL ) {
      _task_block();
   } /* Endif */

   i = _lwmem_get_size( to_ptr );
   j = TEST_ALLOC_SIZE + 2 * LWMEM_MIN_MEMORY_STORAGE_SIZE;

   EU_ASSERT_REF_TC_MSG(tc_7_main_task, ((i >= TEST_ALLOC_SIZE) && (i <= j)), "Test #7: Testing 7.2: Check valid size range")
   if ( (i < TEST_ALLOC_SIZE) || (i > j) ) {
      _task_block();
   } /* Endif */
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_8_main_task
* Comments     : TEST #8: Test function: _lwmem_transfer
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
   _mqx_uint        result;
   
   /* This task will allocate memory and then transfer it to us */
   global_test_task_id = _task_create(0, TEST_TASK, 0 );

   /* Let the test task run */
   _sched_yield();

   block_ptr = (LWMEM_BLOCK_STRUCT_PTR) ((char *)global_lwmem_ptr -
      sizeof(LWMEM_BLOCK_STRUCT));

   result = BUILD_TASKID(kernel_data->INIT.PROCESSOR_NUMBER, block_ptr->U.S.TASK_NUMBER) == global_main_task_id;
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == TRUE), "Test #8: Testing 8.1: Check main task own resource")
   if (result != TRUE) {
      _task_block();
   } /* Endif */

   result = _lwmem_free( global_lwmem_ptr );

   EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.2: lwmem free operation")
   if ( result != MQX_OK ) {
      _task_block();
   } /* Endif */
  
   result = _lwmem_test(&test_pool_id, &error_block_ptr);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.3: lwmem test operation")
   if (result != MQX_OK ) {
      _task_block();
   } /* Endif */
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_9_main_task
* Comments     : TEST #9: Test function: _lwmem_test
*
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
   _mqx_uint        result;
   _mqx_uint        i;

   to_ptr = (unsigned char *)_lwmem_alloc( TEST_ALLOC_SIZE );
   from_ptr = (unsigned char *)_lwmem_alloc( TEST_ALLOC_SIZE );

   result = _lwmem_test(&test_pool_id, &error_block_ptr);
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9: Testing 9.1: lwmem test all should be MQX_OK")
   if ( result != MQX_OK ) {
      _task_block();
   } /* Endif */

   for ( i = 0; i < TEST_ALLOC_SIZE; i++ ) {
      to_ptr[i] = (unsigned char)i;
   } /* Endfor */

   result = _lwmem_test(&test_pool_id, &error_block_ptr);
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9: Testing 9.2: all errors of lwmem test should be MQX_OK")
   if ( result != MQX_OK ) {
      _task_block();
   } /* Endif */


   for ( i = TEST_ALLOC_SIZE; i < (TEST_ALLOC_SIZE * 2); i++ ) {
      to_ptr[i] = (unsigned char)i;
   } /* Endfor */

   result = _lwmem_test(&test_pool_id, &error_block_ptr);
   EU_ASSERT_REF_TC_MSG(tc_9_main_task, result != MQX_OK, "Test #9: Testing 9.3: lwmem test operation")
   if ( result == MQX_OK ) {
      _task_block();
   } /* Endif */
}

//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test #1 - Testing the function _lwmem_test()"),
    EU_TEST_CASE_ADD(tc_2_main_task, " Test #2 - Testing the function _lwmem_alloc()"),
    EU_TEST_CASE_ADD(tc_3_main_task, " Test #3 - Testing the function _lwmem_alloc_system_from()"),
    EU_TEST_CASE_ADD(tc_4_main_task, " Test #4 - Testing the function _lwmem_free()"),
    EU_TEST_CASE_ADD(tc_5_main_task, " Test #5 - Testing the function _lwmem_alloc_zero()"),
    EU_TEST_CASE_ADD(tc_6_main_task, " Test #6 - Testing the function _lwmem_alloc_system_zero()"),
    EU_TEST_CASE_ADD(tc_7_main_task, " Test #7 - Testing the function _lwmem_get_size()"),
    EU_TEST_CASE_ADD(tc_8_main_task, " Test #8 - Testing the function _lwmem_transfer()"),
    EU_TEST_CASE_ADD(tc_9_main_task, " Test #9 - Testing the function _lwmem_test()")
EU_TEST_SUITE_END(suite_1)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, " - Test of Light Weight Memory Component")
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
    TD_STRUCT_PTR   td_ptr;

    _int_install_unexpected_isr();

    kernel_data = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();

    global_main_task_id = _task_get_id();

    td_ptr = SYSTEM_TD_PTR(kernel_data);
    system_task_id = td_ptr->TASK_ID;

    //test_initialize();
    //EU_RUN_ALL_TESTS(test_result_array);
    //test_stop();
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}

/* EOF */
