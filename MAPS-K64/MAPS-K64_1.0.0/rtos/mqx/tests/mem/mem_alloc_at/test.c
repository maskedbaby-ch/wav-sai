/**HEADER********************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
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
* $Version : 4.0.1.1$
* $Date    : May-04-2013$
*
* Comments:
*
*   This file contains code for the MQX Memory verification program,
*   includes all remaining test cases, which haven't been verified yet,
*   of functions allocating memory at specified address.
*
* Requirements :
*   MEM001, MEM025, MEM026, MEM027, MEM028, MEM053.
*   LWMEM001, LWMEM009, LWMEM010, LWMEM020.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include "util.h"
#include "mem_util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define MAIN_TASK       (10)
#define INVALID_VALUE   (1)
#define TEST_ALLOC_SIZE (200)

void    main_task(uint32_t);

/*------------------------------------------------------------------------
 * Test suite function prototype
 */
void tc_1_main_task(void); /* Test #1 - Testing the function _(lw)mem_alloc_at */
void tc_2_main_task(void); /* TEST #2 - Testing the function _(lw)mem_alloc_at in special cases */

/*------------------------------------------------------------------------
 * MQX task template
 */
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
#if PSP_MQX_CPU_IS_KINETIS_L
    { MAIN_TASK,      main_task,      1500,  7, "main",    MQX_AUTO_START_TASK},
#else
    { MAIN_TASK,      main_task,      3000,  7, "main",    MQX_AUTO_START_TASK},
#endif
    {         0,              0,         0,  0,      0,                      0}
};

/******************** Begin Test cases **********************/
void                     *ptr_0;
void                     *ptr_1;
void                     *ptr_2;
void                       *ptr_alloc;

/* Main task prototype */
extern void main_task(uint32_t);

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST 1: Test function: _(lw)mem_alloc_at
* Requirements :
*   MEM001, MEM025, MEM028, MEM053.
*   LWMEM001, LWMEM009, LWMEM010, LWMEM020.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint   result, size_min;
    uint32      local_address = 0;

    /* allocate 3 memory blocks */
    ptr_0 = _mem_alloc( TEST_ALLOC_SIZE );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ptr_0 != NULL, "Test 1: Testing 1.0: _mem_alloc operation");
    if(!ptr_0){
        return;
    }
    ptr_1 = _mem_alloc( TEST_ALLOC_SIZE );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ptr_1 != NULL, "Test 1: Testing 1.1: _mem_alloc operation");
    if(!ptr_1){
        return;
    }
    ptr_2 = _mem_alloc( TEST_ALLOC_SIZE );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ptr_2 != NULL, "Test 1: Testing 1.2: _mem_alloc operation");
    if(!ptr_2){
        return;
    }
    /* deallocate ptr_1 */
    result = _mem_free(ptr_1);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == 0, "Test 1: Testing 1.3: _mem_free operation");
    if(result != 0){
        return;
    }
   /*allocation mem block smaller that the deallocated memory block inside the deallocated memory block*/
#if MQX_USE_LWMEM_ALLOCATOR
   size_min= LWMEM_MIN_MEMORY_STORAGE_SIZE;
#else
   size_min= MQX_MIN_MEMORY_STORAGE_SIZE;
#endif

    /* calcullate start address for allocation */
    local_address = (uint32)ptr_1 + (uint32)(size_min + 4);
    _MEMORY_ALIGN_VAL_LARGER(local_address);

    /* allocate new block "ptr_alloc" inside "ptr_1" memory block */
    ptr_alloc = _mem_alloc_at(TEST_ALLOC_SIZE - size_min - 20, (void *)local_address);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ptr_alloc != NULL, "Test 1: Testing 1.4: _mem_alloc_at operation allocate");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (uint32)ptr_alloc == local_address, "Test 1: Testing 1.5: _mem_alloc_at operation, allocate address");
    if(ptr_alloc !=NULL){
        result = _mem_free(ptr_alloc);
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == 0, "Test 1: Testing 1.6: _mem_free operation");
    }
    /*allocation mem block bigger that the deallocated memory block inside the deallocated memory block*/
    /* allocate new block "ptr_alloc" inside "ptr_1" memory block */
    ptr_alloc = _mem_alloc_at(3 * TEST_ALLOC_SIZE, (void *)local_address);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ptr_alloc == NULL, "Test 1: Testing 1.7: _mem_alloc_at operation allocate");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #1: Testing 1.8: The task's error code must be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);
    if(ptr_alloc !=NULL){
        return;
    }
    /*allocation mem block inside allocadet memory block*/
    /* allocate new block "ptr_alloc" inside "ptr_1" memory block */
    local_address = (uint32)ptr_1 - (uint32)(size_min + 1);
    ptr_alloc = _mem_alloc_at(TEST_ALLOC_SIZE, (void *)local_address );
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ptr_alloc == NULL, "Test 1: Testing 1.9: _mem_alloc_at operation allocate");
    _task_set_error(MQX_OK);
    if(ptr_alloc !=NULL){
        return;
    }
    /*allocation mem block smaller that the deallocated memory block from the start+(size_min -1) of the deallocated memory block*/
    /* calcullate start address for allocation */
    local_address = (uint32)ptr_1 + (uint32)(size_min - 1);
    /* allocate new block "ptr_alloc" inside "ptr_1" memory block */
    ptr_alloc = _mem_alloc_at(TEST_ALLOC_SIZE - size_min - 20, (void *)local_address);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ptr_alloc == NULL, "Test 1: Testing 1.10: _mem_alloc_at operation allocate");
    _task_set_error(MQX_OK);
    if(ptr_alloc !=NULL){
        return;
   }
}
/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   The function _mem_alloc_at is used to allocate private memory at
*   specified address of default pool.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM001, MEM026, MEM027, MEM053.
*   LWMEM001, LWMEM010, LWMEM020.
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    /* Allocated block pointer */
    void   *mem_ptr;
    /* Address to allocate memory */
    void   *alloc_ptr;
#if !MQX_USE_LWMEM_ALLOCATOR
    /* Task's number value */
    _task_number task_num;
#endif
    /* Checksum value */
    _mqx_uint checksum;
    /* Return value of functions */
    _mqx_uint result;

    /* Allocate memory and get the memory block's address */
    mem_ptr = _mem_alloc(TEST_ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, mem_ptr != NULL, "Test #2: Testing 2.01: Allocate mem to get the address");
    alloc_ptr = mem_ptr;
    result = _mem_free(mem_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.02: Free the allocated block");

    /* [_mem_alloc_at] Try to allocate memory when memory block has invalid checksum */
    checksum = get_pool_valid_field(SYSTEM_POOL_ID);
    set_pool_valid_field(SYSTEM_POOL_ID, checksum + INVALID_VALUE);
    mem_ptr = _mem_alloc_at(TEST_ALLOC_SIZE, alloc_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, mem_ptr == NULL, "Test #2: Testing 2.03: Failed to allocate memory when memory block has invalid checksum");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == GENERAL_MEM_INVALID_ERROR_CODE, "Test #2: Testing 2.04: The task's error code must be MQX_INVALID_CHECKSUM");
    set_pool_valid_field(SYSTEM_POOL_ID, checksum);
    _task_set_error(MQX_OK);

#if !MQX_USE_LWMEM_ALLOCATOR
    /* [_mem_alloc_at] Try to allocate memory when memory free list is corrupt */
    task_num = get_block_taskno(SYSTEM_POOL_ID, POOL_TYPE);
    set_block_taskno(SYSTEM_POOL_ID, task_num + INVALID_VALUE, POOL_TYPE);
    mem_ptr = _mem_alloc_at(TEST_ALLOC_SIZE, alloc_ptr);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, mem_ptr == NULL, "Test #2: Testing 2.05: Failed to allocate memory when memory free list is corrupt");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_CORRUPT_STORAGE_POOL_FREE_LIST, "Test #2: Testing 2.06: The task's error code must be MQX_CORRUPT_STORAGE_POOL_FREE_LIST");
    set_block_taskno(SYSTEM_POOL_ID, task_num, POOL_TYPE);
    _task_set_error(MQX_OK);
#endif
}

/******************** End Test cases **********************/

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_at)
    EU_TEST_CASE_ADD(tc_1_main_task, "TEST # 1 - Testing function _(lw)mem_alloc_at"),
    EU_TEST_CASE_ADD(tc_2_main_task, "TEST # 2 - Testing function _(lw)mem_alloc_at in special cases "),
EU_TEST_SUITE_END(suite_at)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_at, " - MQX Memory Component Test Suite, 1 Test")
EU_TEST_REGISTRY_END()
/*------------------------------------------------------------------------*/

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   This is the main task to the MQX Kernel program, this task creates
*   all defined test tasks and lets them run continuously.
*
*END*----------------------------------------------------------------------*/
void main_task(uint32_t param)
{
    _int_install_unexpected_isr();

    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
    _mqx_exit(0);
}
