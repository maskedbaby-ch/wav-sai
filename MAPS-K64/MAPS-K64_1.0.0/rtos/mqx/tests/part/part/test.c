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
* $Version : 3.8.4.1$
* $Date    : Mar-30-2012$
* $Porting Date : Aug-20-2009 
* Comments:
*
*   This file contains the source for the task module testing.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <partition.h>
#include <partition_prv.h>
//#include <log.h>
//#include <klog.h>
//#include <test_module.h>
#include "test.h"

#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test

//------------------------------------------------------------------------
// Test suite function prototipe
extern void tc_1_main_task(void);//TEST # 1 - Testing the following functions: _partition_create()
extern void tc_2_main_task(void);//TEST # 2 - Testing the following functions: _partition_get_block_size()
extern void tc_3_main_task(void);//TEST # 3 - Testing the following functions: _partition_get_total_blocks()
extern void tc_4_main_task(void);//TEST # 4 - Testing the following functions: _partition_calculate_size()
extern void tc_5_main_task(void);//TEST # 5 - Testing the following functions: _partition_get_total_size()
extern void tc_6_main_task(void);//TEST # 6 - Testing the following functions: _partition_calculate_blocks()
extern void tc_7_main_task(void);//TEST # 7 - Testing the following functions: _partition_alloc(), _partition_get_free_blocks(), _partition_get_max_used_blocks()
extern void tc_8_main_task(void);//TEST # 8 - Testing the following functions: _partition_alloc_zero()
extern void tc_9_main_task(void);//TEST # 9 - Testing the following functions: _partition_alloc_system()
extern void tc_10_main_task(void);//TEST # 10 - Testing the following functions: _partition_alloc_system_zero()
extern void tc_11_main_task(void);//TEST # 11 - Testing the following functions: _partition_transfer()
extern void tc_12_main_task(void);//TEST # 12 - Testing the following functions: _partition_extend()
extern void tc_13_main_task(void);//TEST # 13 - Testing the following functions: _partition_destroy()
extern void tc_14_main_task(void);//TEST # 14 - Testing the following functions: _partition_test()
//------------------------------------------------------------------------

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
{ MAIN_TASK,   main_task,  2000,   7, "Main_Task",  MQX_AUTO_START_TASK},
{ TEST_TASK,   test_task,  500,   7, "Test_Task",  0},
{ TEST2_TASK,  test2_task, 500,   6, "Test2_Task", 0},
{ 0,           0,          0,      0,  0,           0}
};

unsigned char      global_memory_buffer_for_part[MEM_PART_SIZE];
unsigned char      global_memory_extension_buffer[MEM_EXT_SIZE];
unsigned char      global_memory_extension_buffer2[MEM_EXT_SIZE];
unsigned char  *global_mem_ptr_storage[100];
unsigned char  *global_ext_loc_array[100];
_mqx_uint  global_blocks_allocated;
unsigned char      global_invalid_partition[10];
////////////********************/////////////////////////
TD_STRUCT_PTR                       td_ptr;
KERNEL_DATA_STRUCT_PTR              kernel_data;
INTERNAL_PARTITION_BLOCK_STRUCT_PTR block_ptr;
_task_id                            system_task_id;
_task_id                            test_task_id;
_mqx_uint                           temp_index;
_partition_id                       part_id_1;
_partition_id                       part_id_2;
_partition_id                       part_id_3;
_mqx_uint                           i, j;
unsigned char                           *mem_ptr;
_partition_id                       part_id_in_error;
void                             *pool_ptr;
void                             *block_in_err_ptr;
bool                             ptr_in_extended_part;
PARTPOOL_STRUCT_PTR                 partpool_ptr;

#if MQX_USE_LWMEM_ALLOCATOR
_lwmem_pool_id  pool_err;
void           *block_err;
#define _mem_test() _lwmem_test(&pool_err, &block_err)
#endif
//////////////*******************///////////////////////

//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_1_main_task
// Comments     : TEST # 1 - Testing the following functions: _partition_create() 
//
//END---------------------------------------------------------------------
 void tc_1_main_task(void)
 {
   _mqx_uint result;

   part_id_3 = _partition_create(BLOCK_SIZE, NUMBER_OF_BLOCKS, 0, 0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, part_id_3 != PARTITION_NULL_ID,"Test #1 Testing: 1.1: Create partition in system memory")
   if (part_id_3 == PARTITION_NULL_ID) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   part_id_2 = _partition_create_at(global_memory_buffer_for_part,
      sizeof(global_memory_buffer_for_part), BLOCK_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, part_id_2 != PARTITION_NULL_ID,"Test #1 Testing: 1.2: Create partition in global memory")
   if (part_id_2 == PARTITION_NULL_ID) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _mem_test();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,result == MQX_OK,"Test #1 Testing: 1.3: Test memory")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   i = _partition_calculate_size(NUMBER_OF_BLOCKS, BLOCK_SIZE);

   part_id_1 = _partition_create(BLOCK_SIZE, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS * 4);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,part_id_1 != PARTITION_NULL_ID,"Test #1 Testing: 1.4: Create partition the part size")
   if (part_id_1 == PARTITION_NULL_ID) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _partition_get_total_blocks(part_id_1);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,result == NUMBER_OF_BLOCKS,"Test #1 Testing: 1.5: Get the total number of partition blocks in the partition")
   if (result != NUMBER_OF_BLOCKS) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_1_main_task,result == MQX_OK,"Test #1 Testing: 1.6: Get the task error code")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_2_main_task
// Comments     : TEST # 2 - Testing the following functions: _partition_get_block_size() 
//
//END---------------------------------------------------------------------
 void tc_2_main_task(void)
 {
   _mqx_uint result = 0;

   result = _partition_get_block_size(part_id_1);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result >= BLOCK_SIZE),"Test #2 Testing: 2.1: Get the size of the partition blocks in the partition")
   if (result < BLOCK_SIZE) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   i = test_calculate_max_block_size(BLOCK_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result <= i),"Test #2 Testing: 2.2: Calculate max block size")
   if (result > i) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _partition_get_block_size((_partition_id)global_invalid_partition);
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == 0, "Test #2 Testing: 2.3: Get the size of the partition blocks in the partition")
   if ( result != 0 ) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_2_main_task,result == PARTITION_INVALID,"Test #2 Testing: 2.4: Get task error code")
   if (result != PARTITION_INVALID) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   _task_set_error(MQX_OK);
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_3_main_task
// Comments     : TEST # 3 - Testing the following functions: _partition_get_total_blocks() 
//
//END---------------------------------------------------------------------
 void tc_3_main_task(void)
 {
   _mqx_uint result;

   result = _partition_get_total_blocks(part_id_1);
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,result == NUMBER_OF_BLOCKS,"Test #3 Testing: 3.1: Get the total number of partition blocks in the partition")
   if (result != NUMBER_OF_BLOCKS) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_3_main_task,result == MQX_OK,"Test #3 Testing: 3.2: Get task error code")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_4_main_task
// Comments     : TEST # 4 - Testing the following functions: _partition_calculate_size() 
//
//END---------------------------------------------------------------------
 void tc_4_main_task(void)
 {
   _mqx_uint result = 0;

   result = _partition_calculate_size(NUMBER_OF_BLOCKS, BLOCK_SIZE);
   i = test_calculate_max_part_size(0, BLOCK_SIZE, NUMBER_OF_BLOCKS);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result <= i),"Test #4 Testing: 4.1: Calculate the number of single-addressable units in a partition")
   if (result > i) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   i = test_calculate_min_part_size(0, BLOCK_SIZE, NUMBER_OF_BLOCKS);
   EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result >= i),"Test #4 Testing: 4.2: Calculate min partition size")
   if (result < i) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_4_main_task,result == MQX_OK,"Test #4 Testing: 4.3: Get task error code")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_5_main_task
// Comments     : TEST # 5 - Testing the following functions: _partition_get_total_size() 
//
//END---------------------------------------------------------------------
 void tc_5_main_task(void)
 {
   _mqx_uint result = 0;

   result = _partition_get_total_size(part_id_1);

   global_ext_loc_array[0] = 0;
   i = test_calculate_actual_part_size((unsigned char *)part_id_1, 0, BLOCK_SIZE, NUMBER_OF_BLOCKS);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task,i == result,"Test #5 Testing: 5.1: Get total size of partition")
   if (i != result) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_5_main_task,result == MQX_OK,"Test #5 Testing: 5.2: Get task error code")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _partition_get_total_size((_partition_id)global_invalid_partition);
   EU_ASSERT_REF_TC_MSG(tc_5_main_task,result == 0,"Test #5 Testing: 5.2: Get total size of partition")
   if (result != 0) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_5_main_task,result == PARTITION_INVALID,"Test #5 Testing: 5.3: Get task error code")
   if (result != PARTITION_INVALID) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   _task_set_error(MQX_OK);
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_6_main_task
// Comments     : TEST # 6 - Testing the following functions: _partition_calculate_blocks() 
//
//END---------------------------------------------------------------------
 void tc_6_main_task(void)
 {
   _mqx_uint result;

   result = _partition_calculate_blocks(i, BLOCK_SIZE);
   EU_ASSERT_REF_TC_MSG(tc_6_main_task,result == NUMBER_OF_BLOCKS, "Test #6 Testing: 6.1: Calculate the number of partition blocks in a static partition")
   if ( result != NUMBER_OF_BLOCKS ) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_6_main_task,result == MQX_OK,"Test #6 Testing: 6.2: Get task error code")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   //printf("\nResult = PASSED.");
 }
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_7_main_task
// Comments     : TEST # 7 - Testing the following functions: _partition_alloc(), _partition_get_free_blocks(), _partition_get_max_used_blocks() 
//
//END---------------------------------------------------------------------
 void tc_7_main_task(void)
 {
   _mqx_uint result = 0;

   temp_index = 7;

   result = _partition_get_free_blocks(part_id_1);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == NUMBER_OF_BLOCKS,"Test #7 Testing: 7.1: Get the number of free partition blocks in the partition")
   if (result != NUMBER_OF_BLOCKS) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _partition_get_max_used_blocks(part_id_1);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == 0,"Test #7 Testing: 7.2: Get the number of allocated partition blocks in the partition")
   if (result != 0) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */


   mem_ptr = (unsigned char *)_partition_alloc(part_id_1);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,mem_ptr != NULL,"Test #7 Testing: 7.3: Allocate a private partition block from the partition")
   if (!mem_ptr) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */


   block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)(mem_ptr -
      sizeof(INTERNAL_PARTITION_BLOCK_STRUCT));
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,block_ptr->TASK_ID == _task_get_id(), "Test #7 Testing: 7.4: Check ID task")
   if (block_ptr->TASK_ID != _task_get_id()) {
      _time_delay_ticks(4);                             
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */


   result = _partition_get_free_blocks(part_id_1);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == (NUMBER_OF_BLOCKS - 1),"Test #7 Testing: 7.5: Get the number of free partition blocks in the partition")
   if (result != (NUMBER_OF_BLOCKS - 1)) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _partition_get_max_used_blocks(part_id_1);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == 1,"Test #7 Testing: 7.5: Get max block used in the partition")
   if (result != 1) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _partition_free(mem_ptr);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == MQX_OK,"Test #7 Testing: 7.6:Free partition block")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
  
   result = test_alloc_all_blocks(part_id_1, TRUE);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == NUMBER_OF_BLOCKS * 4,"Test #7 Testing: 7.7: Test function test_alloc_all_blocks")
   if (result != NUMBER_OF_BLOCKS * 4) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
   
   result = _mem_test();
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == MQX_OK,"Test #7 Testing: 7.8: Test memory")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   i = test_free_all_blocks(&result);
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == MQX_OK,"Test #7 Testing: 7.9: Test function test_free_all_blocks")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _mem_test();
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == MQX_OK,"Test #7 Testing: 7.10: Test memory")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_7_main_task,result == MQX_OK,"Test #7 Testing: 7.11: Get task error code")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_8_main_task
// Comments     : TEST # 8 - Testing the following functions: _partition_alloc_zero() 
//
//END---------------------------------------------------------------------
 void tc_8_main_task(void)
 {
   _mqx_uint result = 0;

   mem_ptr = (unsigned char *)_partition_alloc_zero(part_id_1);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,mem_ptr != NULL,"Test #8 Testing: 8.1: Allocate a zero-filled private partition block")
   if (!mem_ptr) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   for (i = 0;  i < BLOCK_SIZE; i++) {
      EU_ASSERT_REF_TC_MSG(tc_8_main_task,mem_ptr[i] == 0,"Test #8 Testing: 8.2: Test memory" )
      if (mem_ptr[i] != 0) {
         _time_delay_ticks(4);
         eunit_test_stop();
         _mqx_exit(1);
      } /* Endif */
   } /* Endfor */

   result = _partition_free(mem_ptr);
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == MQX_OK,"Test #8 Testing: 8.3: Free partition")
   if (result != MQX_OK) {
      //printf("\nmain_task: _partition_free() FAILED 3.");
      //printf("\n           Error code returned: 0x%X", result);
      //result = _task_get_error();
      //printf("\n           Task error code: 0x%X", result);
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */


   result = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_8_main_task,result == MQX_OK,"Test #8 Testing: 8.3: Get task error code")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_9_main_task
// Comments     : TEST # 9 - Testing the following functions: _partition_alloc_system() 
//
//END---------------------------------------------------------------------
 void tc_9_main_task(void)
 {
   _mqx_uint result = 0;


   mem_ptr = (unsigned char *)_partition_alloc_system(part_id_1);

   block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)(mem_ptr -
      sizeof(INTERNAL_PARTITION_BLOCK_STRUCT));
   EU_ASSERT_REF_TC_MSG(tc_9_main_task,block_ptr->TASK_ID == system_task_id,"Test #9 Testing: 9.1: Allocate a system partition block")
   if (block_ptr->TASK_ID != system_task_id) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _partition_free(mem_ptr);
   EU_ASSERT_REF_TC_MSG(tc_9_main_task,result == MQX_OK,"Test #9 Testing: 9.2: Free partition")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
   
   result = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_9_main_task,result == MQX_OK,"Test #9 Testing: 9.3: Get task error code")
   if (result != MQX_OK ) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_10_main_task
// Comments     : TEST # 10 - Testing the following functions: _partition_alloc_system_zero() 
//
//END---------------------------------------------------------------------
 void tc_10_main_task(void)
 {
   _mqx_uint result = 0;

   mem_ptr = (unsigned char *)_partition_alloc_system_zero(part_id_1);
   EU_ASSERT_REF_TC_MSG(tc_10_main_task,mem_ptr != NULL,"Test #10 Testing: 10.1:Allocate a zero-filled system partition block")
   if (!mem_ptr ) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   for ( i = 0;  i < BLOCK_SIZE;  i++) {
      EU_ASSERT_REF_TC_MSG(tc_10_main_task,mem_ptr[i] == 0,"Test #10 Testing: 10.2: Test memory")
      if (mem_ptr[i] != 0) {
         _time_delay_ticks(4);
         eunit_test_stop();
         _mqx_exit(1);
      } /* Endif */
   } /* Endfor */

   result = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_10_main_task,result == MQX_OK,"Test #10 Testing: 10.3: Get task error code")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_11_main_task
// Comments     : TEST # 11 - Testing the following functions: _partition_transfer()e 
//
//END---------------------------------------------------------------------
 void tc_11_main_task(void)
 {
   _mqx_uint result = 0;
   temp_index = 11;

   test_task_id = _task_create(0, TEST_TASK, (uint32_t)mem_ptr );

   result = _partition_transfer( mem_ptr, test_task_id );
   EU_ASSERT_REF_TC_MSG(tc_11_main_task,result == MQX_OK,"Test #11 Testing: 11.1: Transfer the ownership of the partition block")
   if ( result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)(mem_ptr -
      sizeof(INTERNAL_PARTITION_BLOCK_STRUCT));
   EU_ASSERT_REF_TC_MSG(tc_11_main_task,block_ptr->TASK_ID == test_task_id,"Test #11 Testing: 11.2: Test ID task")
   if (block_ptr->TASK_ID != test_task_id ) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _partition_free(mem_ptr);
   EU_ASSERT_REF_TC_MSG(tc_11_main_task,result == MQX_NOT_RESOURCE_OWNER,"Test #11 Testing: 11.6: Free partition")
   if (result != MQX_NOT_RESOURCE_OWNER) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   _sched_yield();

   result = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_11_main_task,result == MQX_OK,"Test #11 Testing: 11.3: Get task error code")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_12_main_task
// Comments     : TEST # 12 - Testing the following functions: _partition_extend() 
//
//END---------------------------------------------------------------------
 void tc_12_main_task(void)
 {
   _mqx_uint result = 0;
   temp_index = 12;

   j = test_calculate_number_of_blocks( global_memory_extension_buffer, 
      sizeof(global_memory_extension_buffer), BLOCK_SIZE, TRUE);

   result = _partition_extend(part_id_2, global_memory_extension_buffer,
      sizeof(global_memory_extension_buffer) );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.1: Add partition blocks to the static partition")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   j += test_calculate_number_of_blocks( global_memory_extension_buffer, 
      sizeof(global_memory_extension_buffer), BLOCK_SIZE, FALSE);

   result = _partition_get_total_size(part_id_2);

   i = test_calculate_actual_part_size(part_id_2, 1, BLOCK_SIZE, j);
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,i == result,"Test #12 Testing: 12.2: Test calculate actual part size")
   if (i != result ) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _partition_extend(part_id_2, global_memory_extension_buffer2,
      sizeof(global_memory_extension_buffer2) );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.3:Add partition blocks to the static partition")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   j += test_calculate_number_of_blocks( global_memory_extension_buffer2, 
      sizeof(global_memory_extension_buffer2), BLOCK_SIZE, FALSE);

   result = _mem_test();
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.4: Test memory")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _partition_get_total_blocks(part_id_2);
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == j,"Test #12 Testing: 12.5: Get total partition block")
   if ( result != j) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _partition_get_total_size(part_id_2);

   i = test_calculate_actual_part_size(part_id_2, 2, BLOCK_SIZE, j);
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,i == result,"Test #12 Testing: 12.6: Test calculate actual part size")
   if (i != result) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = test_alloc_all_blocks(part_id_2, TRUE);
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == j,"Test #12 Testing: 12.7: Test alloc for all blocks")
   if (result != j) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _mem_test();
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.8: Test memory")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   i = test_free_all_blocks( &result );
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.9: Test free for all blocks")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   i = _partition_get_free_blocks(part_id_2);
   test_task_id = _task_create(0, TEST2_TASK, (uint32_t)part_id_2);
   j = _partition_get_free_blocks(part_id_2);
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,j == (i-1),"Test #12 Testing: 12.10: Get the number of free partition blocks in the partition")
   if (j != (i-1)) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.11: Get task error code")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   _task_destroy(test_task_id);
   j = _partition_get_free_blocks(part_id_2);
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,j == i, "Test #12 Testing: 12.12: Test when destroy task")
   if (j != i) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */


   result = _mem_test();
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK, "Test #12 Testing: 12.13: Test memory")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_12_main_task,result == MQX_OK,"Test #12 Testing: 12.14: Get task error code")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_13_main_task
// Comments     : TEST # 13 - Testing the following functions: _partition_destroy() 
//
//END---------------------------------------------------------------------
 void tc_13_main_task(void)
 {
   _mqx_uint result = 0;
   _partition_id new_part_id;

   new_part_id = _partition_create(BLOCK_SIZE, NUMBER_OF_BLOCKS, 0, 0);
   EU_ASSERT_REF_TC_MSG(tc_13_main_task,new_part_id != PARTITION_NULL_ID,"Test #13 Testing: 13.1:Create a partition")
   if (new_part_id == PARTITION_NULL_ID) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   mem_ptr = _partition_alloc(new_part_id);
   EU_ASSERT_REF_TC_MSG(tc_13_main_task,mem_ptr != NULL,"Test #13 Testing: 13.2: Allocate a private partition ")
   if (!mem_ptr ) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _partition_destroy(new_part_id);
   EU_ASSERT_REF_TC_MSG(tc_13_main_task,result == PARTITION_ALL_BLOCKS_NOT_FREE,"Test #13 Testing: 13.3: Destroy partition")
   if (result != PARTITION_ALL_BLOCKS_NOT_FREE) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _partition_free(mem_ptr);
   EU_ASSERT_REF_TC_MSG(tc_13_main_task,result == MQX_OK,"Test #13 Testing: 13.4: Free partition")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   result = _partition_destroy(new_part_id);
   EU_ASSERT_REF_TC_MSG(tc_13_main_task,result == MQX_OK,"Test #13 Testing: 13.5: Destroy partition")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
}
//TASK--------------------------------------------------------------------
// 
// Task Name    : tc_14_main_task
// Comments     : TEST # 14 - Testing the following functions: _partition_test() 
//
//END---------------------------------------------------------------------
 void tc_14_main_task(void)
 {
   _mqx_uint result;

   mem_ptr = _partition_alloc(part_id_2);
   EU_ASSERT_REF_TC_MSG(tc_14_main_task, mem_ptr != NULL,"Test #14 Testing: 14.1: Alllocate a private partition" )
   if (!mem_ptr ) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   ptr_in_extended_part = 
      ((_mqx_uint)mem_ptr >= (_mqx_uint)global_memory_extension_buffer) &&
      ((_mqx_uint)mem_ptr <= (_mqx_uint)&global_memory_extension_buffer[MEM_EXT_SIZE]);

   result = _partition_test( &part_id_in_error, &pool_ptr, &block_in_err_ptr);
   EU_ASSERT_REF_TC_MSG(tc_14_main_task,result == MQX_OK,"Test #14 Testing: 14.2: Test partition")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   test_fill_block(mem_ptr, BLOCK_SIZE);

   result = _partition_test( &part_id_in_error, &pool_ptr, &block_in_err_ptr);
   EU_ASSERT_REF_TC_MSG(tc_14_main_task,result == MQX_OK,"Test #14 Testing: 14.3: Test partition")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   partpool_ptr = (PARTPOOL_STRUCT_PTR)part_id_2;

   i = partpool_ptr->POOL.VALID;
   partpool_ptr->POOL.VALID = ~partpool_ptr->POOL.VALID;

   result = _partition_test(&part_id_in_error, &pool_ptr, &block_in_err_ptr);

   j = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_14_main_task,result == PARTITION_INVALID,"Test #14 Testing: 14.4: Get task error code")
   if (result != PARTITION_INVALID) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
   EU_ASSERT_REF_TC_MSG(tc_14_main_task,j == MQX_OK,"Test #14 Testing: 14.5: Test partition")
   if (j != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   partpool_ptr->POOL.VALID = i;
   _task_set_error(MQX_OK);

   block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)(mem_ptr -
      sizeof(INTERNAL_PARTITION_BLOCK_STRUCT));

   block_ptr->CHECKSUM = ~block_ptr->CHECKSUM;

   result = _partition_test(&part_id_in_error, &pool_ptr, &block_in_err_ptr);

   j = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_14_main_task,result == PARTITION_BLOCK_INVALID_CHECKSUM, "Test #14 Testing: 14.6: Test partition")
   if (result != PARTITION_BLOCK_INVALID_CHECKSUM) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
   EU_ASSERT_REF_TC_MSG(tc_14_main_task,j == MQX_OK,"Test #14 Testing: 14.7: Test partition")
   if (j != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   _task_set_error(MQX_OK);
   EU_ASSERT_REF_TC_MSG(tc_14_main_task,part_id_in_error == part_id_2,"Test #14 Testing: 14.8: Get task error code")
   if (part_id_in_error != part_id_2) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
   EU_ASSERT_REF_TC_MSG(tc_14_main_task, (ptr_in_extended_part || ((_mqx_uint)pool_ptr == (_mqx_uint)&partpool_ptr->POOL)),"Test #14 Testing: 14.9: Test partition")
   if (!ptr_in_extended_part && ((_mqx_uint)pool_ptr !=
      (_mqx_uint)&partpool_ptr->POOL))
   {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */

   partpool_ptr = (PARTPOOL_STRUCT_PTR)global_memory_extension_buffer;
   EU_ASSERT_REF_TC_MSG(tc_14_main_task, (ptr_in_extended_part == FALSE || ((_mqx_uint)pool_ptr == (_mqx_uint)&partpool_ptr->POOL)),"Test #14 Testing: 14.10: Test partition")
   if (ptr_in_extended_part && ((_mqx_uint)pool_ptr !=
      (_mqx_uint)&partpool_ptr->POOL))
   {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
   EU_ASSERT_REF_TC_MSG(tc_14_main_task, ((_mqx_uint)block_in_err_ptr == (_mqx_uint)mem_ptr),"Test #14 Testing: 14.11: Test partition")
   if ((_mqx_uint)block_in_err_ptr != (_mqx_uint)mem_ptr) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
   
   result = _task_get_error();
   EU_ASSERT_REF_TC_MSG(tc_14_main_task,result == MQX_OK,"Test #14 Testing: 14.12: Get task error code")
   if (result != MQX_OK) {
      _time_delay_ticks(4);
      eunit_test_stop();
      _mqx_exit(1);
   } /* Endif */
}
//------------------------------------------------------------------------
// Define Test Suite 
 
 EU_TEST_SUITE_BEGIN(suite_part)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1 - Testing the following functions: _partition_create()"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2 - Testing the following functions: _partition_get_block_size()"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3 - Testing the following functions: _partition_get_total_blocks()"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4 - Testing the following functions: _partition_calculate_size()"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST # 5 - Testing the following functions: _partition_get_total_size()"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST # 6 - Testing the following functions: _partition_calculate_blocks()"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST # 7 - Testing the following functions: _partition_alloc(), _partition_get_free_blocks(), _partition_get_max_used_blocks()"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST # 8 - Testing the following functions: _partition_alloc_zero()"),
    EU_TEST_CASE_ADD(tc_9_main_task, " TEST # 9 - Testing the following functions: _partition_alloc_system()"),
    EU_TEST_CASE_ADD(tc_10_main_task, " TEST # 10 - Testing the following functions: _partition_alloc_system_zero()"),
    EU_TEST_CASE_ADD(tc_11_main_task, " TEST # 11 - Testing the following functions: _partition_transfer()"),
    EU_TEST_CASE_ADD(tc_12_main_task, " TEST # 12 - Testing the following functions: _partition_extend()"),
    EU_TEST_CASE_ADD(tc_13_main_task, " TEST # 13 - Testing the following functions: _partition_destroy()"),
    EU_TEST_CASE_ADD(tc_14_main_task, " TEST # 14 - Testing the following functions: _partition_test()")
 EU_TEST_SUITE_END(suite_part)

// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_part, " - Test part suite")
 EU_TEST_REGISTRY_END()

//-------------------------------------------------------------------
/*TASK*--------------------------------------------------------------
* 
* Task Name : test2_task
* Comments  :
*
*END*--------------------------------------------------------------*/

void test2_task
   (
      uint32_t param
   )
{
    _partition_id    part_id = (_partition_id)param;
    void            *ptr;

    ptr = _partition_alloc(part_id);
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, ptr != NULL, "Test #12 Testing: _partition_alloc operation")
    _task_block();
}

/*TASK*--------------------------------------------------------------
* 
* Task Name : test_task
* Comments  :
*
*END*--------------------------------------------------------------*/

void test_task
   (
      uint32_t param
   )
{
    unsigned char  *mem_ptr;
    _mqx_uint  result;

    mem_ptr = (unsigned char *)param;

    result = _partition_free(mem_ptr);
    switch (temp_index){
    case 11: 
        EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK,"Test #11 Testing: _partition_free operation")
        break;
    case 12: 
        EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK,"Test #12 Testing: _partition_free operation")
        break;
    }

    if (result != MQX_OK) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    _task_block();
} 

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
   _int_install_unexpected_isr();

   kernel_data = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();
   td_ptr = SYSTEM_TD_PTR(kernel_data);
   system_task_id = td_ptr->TASK_ID;

   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
   
   //_time_delay_ticks(4);
   _time_delay(1000);  // waiting for all loggings to be printed out

   _mqx_exit(0);
}


/*FUNCTION*---------------------------------------------------------
* 
* Functiob Name : test_fill_block
* Comments  :
*
*END*--------------------------------------------------------------*/

void test_fill_block
   (
      unsigned char    *block_ptr,
      _mqx_uint      size
   )
{
   _mqx_uint i;

   for (i = 0; i < size; i++) {
      block_ptr[i] = (unsigned char)i;
   } /* Endfor */
}

/*FUNCTION*---------------------------------------------------------
* 
* Functiob Name : test_alloc_all_blocks
* Comments  :
*
*END*--------------------------------------------------------------*/

_mqx_uint test_alloc_all_blocks
   (
      _partition_id   part_id,
      bool         fill_blocks
   )
{
    unsigned char  *mem_ptr;
    _mqx_uint  result;

    global_blocks_allocated = 0;
    mem_ptr = _partition_alloc(part_id);

    while (mem_ptr) {
        if (fill_blocks) {
            test_fill_block(mem_ptr, BLOCK_SIZE);
        } /* Endif */
        global_mem_ptr_storage[global_blocks_allocated] = mem_ptr;
        global_blocks_allocated++;
        mem_ptr = _partition_alloc(part_id);
    } /* Endwhile */

    result = _task_get_error();
    if (temp_index == 7)
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == PARTITION_OUT_OF_BLOCKS), "Test #7 Testing: 7.11: _partition_alloc operation")
    else
        EU_ASSERT_REF_TC_MSG(tc_12_main_task, (result == PARTITION_OUT_OF_BLOCKS), "Test #12 Testing: 12.11: _partition_alloc operation")
    if (result != PARTITION_OUT_OF_BLOCKS) {
        _time_delay_ticks(4);
        eunit_test_stop();
        _mqx_exit(1);
    } /* Endif */

    _task_set_error(MQX_OK);

    return(global_blocks_allocated);
}

/*FUNCTION*---------------------------------------------------------
* 
* Functiob Name : test_free_all_blocks
* Comments  :
*
*END*--------------------------------------------------------------*/

_mqx_uint test_free_all_blocks
   (
      _mqx_uint_ptr error_ptr
   )
{
    _mqx_uint  result, i, j;

    result = _partition_free(global_mem_ptr_storage[0]);

    j = 0;
    for (i = 1; (i < global_blocks_allocated) && (result == MQX_OK); i++) {
      j++;
      result = _partition_free( global_mem_ptr_storage[i]);
    } /* Endwhile */

    *error_ptr = result;

    return(j);
}


/*FUNCTION*---------------------------------------------------------
* 
* Functiob Name : test_calculate_min_part_size
* Comments  :
*
*END*--------------------------------------------------------------*/

_mqx_uint test_calculate_min_part_size
   (
      _mqx_uint  number_of_extensions,
      _mqx_uint  block_size,
      _mqx_uint  number_of_blocks
   )
{
   _mqx_uint  size;
   

   size = sizeof(PARTPOOL_STRUCT) +
      number_of_extensions * sizeof(PARTPOOL_BLOCK_STRUCT) +
      (block_size + sizeof(INTERNAL_PARTITION_BLOCK_STRUCT)) *
      number_of_blocks;

   return(size);

}


/*FUNCTION*---------------------------------------------------------
* 
* Functiob Name : test_calculate_max_part_size
* Comments  :
*
*END*--------------------------------------------------------------*/

_mqx_uint test_calculate_max_part_size
   (
      _mqx_uint  number_of_extensions,
      _mqx_uint  block_size,
      _mqx_uint  number_of_blocks
   )
{
   _mqx_uint  size, tmp;
   

/*
** Calculate what the worst case would be for processors
** that support memory alignement
*/
#if PSP_MEMORY_ALIGNMENT
   size = sizeof(PARTPOOL_STRUCT) + PSP_MEMORY_ALIGNMENT;

   tmp = sizeof(PARTPOOL_BLOCK_STRUCT) + PSP_MEMORY_ALIGNMENT;

   size += number_of_extensions * tmp;

   tmp = block_size + sizeof(INTERNAL_PARTITION_BLOCK_STRUCT);
   _MEMORY_ALIGN_VAL_LARGER(tmp);

   size += tmp * number_of_blocks;
#else
   size = test_calculate_min_part_size(number_of_extensions, block_size,
      number_of_blocks);
#endif

   return(size);

}


/*FUNCTION*---------------------------------------------------------
* 
* Functiob Name : test_calculate_actual_part_size
* Comments  :
*
*END*--------------------------------------------------------------*/

_mqx_uint test_calculate_actual_part_size
   (
      unsigned char   *part_start,
      _mqx_uint   number_of_extensions,
      _mqx_uint   block_size,
      _mqx_uint   number_of_blocks
   )
{
   PARTPOOL_BLOCK_STRUCT_PTR pool_ptr;
   unsigned char                 *tmp_ptr;
   _mqx_uint                 size, tmp, i;
   

/*
** Calculate what the worst case would be for processors
** that support memory alignement
*/
#if PSP_MEMORY_ALIGNMENT
   tmp_ptr = (unsigned char *)part_start + sizeof(PARTPOOL_STRUCT);
   tmp_ptr = (unsigned char *)_ALIGN_ADDR_TO_HIGHER_MEM( tmp_ptr );

   size =  (_mqx_uint)(tmp_ptr - part_start);
   pool_ptr = ((PARTPOOL_STRUCT_PTR)(part_start))->POOL.NEXT_POOL_PTR;

   /* Calculate size of extension headers */
   tmp = 0;
   for ( i = 0; i < number_of_extensions; i++) {
      tmp_ptr  = (unsigned char *)pool_ptr + sizeof(PARTPOOL_BLOCK_STRUCT);
      tmp_ptr  = (unsigned char *)_ALIGN_ADDR_TO_HIGHER_MEM( tmp_ptr );
      tmp     += (_mqx_uint)(tmp_ptr - (unsigned char *)pool_ptr);
      pool_ptr = pool_ptr->NEXT_POOL_PTR;
   } /* Endfor */

   size += tmp;

   tmp = block_size + sizeof(INTERNAL_PARTITION_BLOCK_STRUCT);
   _MEMORY_ALIGN_VAL_LARGER(tmp);

   size += tmp * number_of_blocks;
#else
   size = test_calculate_min_part_size(number_of_extensions, block_size,
      number_of_blocks);
#endif

   return(size);

}

/*FUNCTION*---------------------------------------------------------
* 
* Functiob Name : test_calculate_max_block_size
* Comments  :
*
*END*--------------------------------------------------------------*/

_mqx_uint test_calculate_max_block_size
   (
      _mqx_uint  block_size
   )
{
   _mqx_uint  size;

#if PSP_MEMORY_ALIGNMENT
   size = block_size + sizeof(INTERNAL_PARTITION_BLOCK_STRUCT);
   _MEMORY_ALIGN_VAL_LARGER(size);
   size -= sizeof(INTERNAL_PARTITION_BLOCK_STRUCT);
   return(size);
#else
   return(block_size);
#endif
}

/*FUNCTION*---------------------------------------------------------
* 
* Functiob Name : test_calculate_number_of_blocks
* Comments  :
*
*END*--------------------------------------------------------------*/

_mqx_uint test_calculate_number_of_blocks
   (
      unsigned char  *start_ptr,
      _mqx_uint  ext_size,
      _mqx_uint  block_size,
      bool    calculate_initial_number_of_blocks
   )
{
   _mqx_uint  number_of_blocks;
   _mqx_uint  actual_block_size;
   unsigned char  *end_ptr;


   actual_block_size = block_size + sizeof(INTERNAL_PARTITION_BLOCK_STRUCT);
#if PSP_MEMORY_ALIGNMENT
   _MEMORY_ALIGN_VAL_LARGER(actual_block_size);

   end_ptr = start_ptr + ext_size;

   if ( calculate_initial_number_of_blocks ) {
      start_ptr += sizeof(PARTPOOL_STRUCT);
   } else {
      start_ptr += sizeof(PARTPOOL_BLOCK_STRUCT);
   } /* Endif */

   start_ptr = (unsigned char *)_ALIGN_ADDR_TO_HIGHER_MEM( start_ptr );

   number_of_blocks = (_mqx_uint)(end_ptr - start_ptr) / actual_block_size;
#else
   if ( calculate_initial_number_of_blocks ) {
      number_of_blocks = (ext_size - sizeof(PARTPOOL_STRUCT)) / actual_block_size;
   } else {
      number_of_blocks = (ext_size - sizeof(PARTPOOL_BLOCK_STRUCT)) / actual_block_size;
   } /* Endif */
#endif

   return(number_of_blocks);
}

/* EOF */
