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
* $Date    : Apr-26-2013$
*
* Comments:
*
*   This file contains code for the MQX Memory verification program,
*   includes all remaining test cases, which haven't been verified yet,
*   of functions modifying, testing and deallocating memory components.
*
* Requirements:
*
*   MEM001, MEM005, MEM046, MEM050, MEM052, MEM053, MEM058, MEM059, MEM060,
*   MEM061, MEM065, MEM075, MEM077, MEM087, MEM089, MEM101, MEM100,
*   TASK006, TASK019.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include "test.h"
#include "util.h"
#include "mem_util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

/*------------------------------------------------------------------------
 * Test suite function prototype
 */
void tc_1_main_task(void);  /* TEST # 1 - Testing function _mem_extend */
void tc_2_main_task(void);  /* TEST # 2 - Testing function _mem_extend_pool */
void tc_3_main_task(void);  /* TEST # 3 - Testing function _mem_get_highwater_pool */
void tc_4_main_task(void);  /* TEST # 4 - Testing function _mem_test */
void tc_5_main_task(void);  /* TEST # 5 - Testing function _mem_test_pool */
void tc_6_main_task(void);  /* TEST # 6 - Testing function _mem_free_part */

/*------------------------------------------------------------------------
 * MQX task template
 */
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
#if PSP_MQX_CPU_IS_KINETIS_L
    { MAIN_TASK,      main_task,      1500,  9, "main",    MQX_AUTO_START_TASK},
#else
    { MAIN_TASK,      main_task,      3000,  9, "main",    MQX_AUTO_START_TASK},
#endif
    { TEST_TASK,      test_task,      1000,  8, "test",                      0},
    {         0,              0,         0,  0,      0,                      0}
};

/*------------------------------------------------------------------------
 * Global variables
 */
/* Invalid memory size for extending pool */
unsigned char small_memory[INVALID_EXTENSION_SIZE];
/* Valid memory size for extending pool */
unsigned char valid_memory[VALID_EXTENSION_SIZE];
/* The memory block is used to test accessing resource of task */
void   *global_mem_ptr;
/* External pool ID */
_mem_pool_id pool_id;

/******************** Begin Test cases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*   The function _mem_extend is used to add the specified memory to
*   default pool.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM050, MEM100.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
#if MQX_USE_LWMEM_ALLOCATOR
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, TRUE, "Test #1 Testing: 1.01: ");
#else
    /* Kernel data struct pointer */
    KERNEL_DATA_STRUCT_PTR kernel_data;
    /* Return value of functions */
    _mqx_uint result;

    /* Get current kernel data struct */
    _GET_KERNEL_DATA(kernel_data);

    /* [_mem_extend] Try to add to the pool a small extension block */
    result = _mem_extend(small_memory, INVALID_EXTENSION_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_INVALID_SIZE, "Test #1: Testing 1.01: Failed to add to the memory a small extension block");
    _task_set_error(MQX_OK);

    /* [_mem_extend] Try to extend the pool when its handler is invalid */
    kernel_data->KD_POOL.VALID = MEMPOOL_VALID + INVALID_VALUE;
    result = _mem_extend(valid_memory, VALID_EXTENSION_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_INVALID_COMPONENT_HANDLE, "Test #1: Testing 1.02: Failed to extend the pool when its handler is invalid");
    kernel_data->KD_POOL.VALID = MEMPOOL_VALID;
    _task_set_error(MQX_OK);
#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   The function _mem_extend_pool is used to add the specified memory to
*   external pool.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM001, MEM046, MEM052, MEM101.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    /* Pointer of memory block of external pool */
    void   *pool_alloc_ptr;
    /* Return value of functions */
    _mqx_uint result;
    /* size of external pool */
    _mqx_uint pool_size = get_free_mem() / 2;

    /* Create external memory pool */
    pool_alloc_ptr = _mem_alloc(pool_size);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, pool_alloc_ptr != NULL, "Test #2: Testing 2.01: Allocate memory for memory pool");
    pool_id = _mem_create_pool(pool_alloc_ptr, pool_size);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, pool_id != NULL, "Test #2: Testing 2.02: Create external memory pool");
    _task_set_error(MQX_OK);

#if !MQX_USE_LWMEM_ALLOCATOR
    /* [_mem_extend_pool] Try to add to the pool a small extension block */
    result = _mem_extend_pool(pool_id, small_memory, INVALID_EXTENSION_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_INVALID_SIZE, "Test #2: Testing 2.03: Failed to add to the pool a small extension block");
    _task_set_error(MQX_OK);

    /* [_mem_extend_pool] Try to extend the pool when its handler is invalid */
    ((MEMPOOL_STRUCT_PTR)pool_id)->VALID = MEMPOOL_VALID + INVALID_VALUE;
    result = _mem_extend_pool(pool_id, valid_memory, VALID_EXTENSION_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_INVALID_COMPONENT_HANDLE, "Test #2: Testing 2.04: Failed to extend the pool when its handler is invalid");
    ((MEMPOOL_STRUCT_PTR)pool_id)->VALID = MEMPOOL_VALID;
    _task_set_error(MQX_OK);

#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   The function _mem_get_highwater_pool is used to get the highest memory
*   address that MQX has allocated in the pool.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM001, MEM005, MEM046, MEM053, MEM065.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
#if MQX_USE_LWMEM_ALLOCATOR
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, TRUE, "Test #3 Testing: 3.01: ");
#else
    /* Allocated block pointer */
    void   *mem_ptr;
    /* Pointer of memory block of external pool */
    void   *pool_alloc_ptr;
    /* Address of highest memory used before allocating */
    void   *old_ptr;
    /* Address of highest memory used after allocating */
    void   *new_ptr;
    /* Return value of functions */
    _mqx_uint result;

    /* [_mem_get_highwater_pool] Get the highest address before allocating memory */
    old_ptr = _mem_get_highwater_pool(pool_id);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, old_ptr != NULL, "Test #3: Testing 3.01: Get the highest address before allocating memory");
    new_ptr = _mem_get_highwater_pool(pool_id);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, new_ptr != NULL, "Test #3: Testing 3.02: Get the highest address before allocating memory again");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, old_ptr == new_ptr, "Test #3: Testing 3.03: The addresses should be the same");

    /* Allocate memory and the highest memory should be increased */
    mem_ptr = _mem_alloc_from(pool_id, ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, mem_ptr != NULL, "Test #3: Testing 3.04: Allocate memory on the pool");
    result = _mem_free(mem_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.05: Free the allocated memory");

    /* [_mem_get_highwater_pool] Get the highest address after allocating memory */
    new_ptr = _mem_get_highwater_pool(pool_id);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, new_ptr != NULL, "Test #3: Testing 3.06: Get the highest address after allocating memory");
    /* The highest address must be increased */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, new_ptr > old_ptr, "Test #3: Testing 3.07: The highest address must be increased");

#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*   The function _mem_test is are used to test all the memory blocks that
*   were allocated from the default memory pool.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM075, MEM077.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    /* Error block pointer */
    void   *error_block_ptr;
    /* Checksum value */
    _mqx_uint checksum;
    /* Return value of functions */
    _mqx_uint result;

    /* [_mem_test] Try to test default pool when memory free list is corrupt */
    error_block_ptr = get_kernel_error_block();
    set_pool_free_list_ptr(SYSTEM_POOL_ID, (void *)INVALID_VALUE);
    result = GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_CORRUPT_STORAGE_POOL_FREE_LIST, "Test #4: Testing 4.01: Failed to test default pool when memory free list is corrupt");
    set_pool_free_list_ptr(SYSTEM_POOL_ID, NULL);
    set_kernel_error_block(error_block_ptr);
    _task_set_error(MQX_OK);

    /* [_mem_test] Try to test default pool when memory block has invalid checksum */
    error_block_ptr = get_kernel_error_block();
    checksum = get_pool_valid_field(SYSTEM_POOL_ID);
    set_pool_valid_field(SYSTEM_POOL_ID, checksum + INVALID_VALUE);
    result = GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == GENERAL_MEM_INVALID_ERROR_CODE, "Test #4: Testing 4.02: Failed to test default pool when memory block has invalid checksum");
    set_pool_valid_field(SYSTEM_POOL_ID, checksum);
    set_kernel_error_block(error_block_ptr);
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5:
*   The function _mem_test_pool is used to test all the memory blocks that
*   were allocated from external pool.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM001, MEM046, MEM087, MEM089.
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    /* Pointer of memory block of external pool */
    void   *pool_alloc_ptr;
    /* Checksum value */
    _mqx_uint checksum;
    /* Return value of functions */
    _mqx_uint result;

    /* [_mem_test_pool] Try to test external pool when memory free list is corrupt */
    set_pool_free_list_ptr(pool_id, (void *)INVALID_VALUE);
    result = GENERAL_MEM_TEST_POOL(pool_id);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_CORRUPT_STORAGE_POOL_FREE_LIST, "Test #5: Testing 5.01: Failed to test external pool when memory free list is corrupt");
    set_pool_free_list_ptr(pool_id, NULL);
    _task_set_error(MQX_OK);

    /* [_mem_test_pool] Try to test external pool when memory block has invalid checksum */
    checksum = get_pool_valid_field(pool_id);
    set_pool_valid_field(pool_id, checksum + INVALID_VALUE);
    result = GENERAL_MEM_TEST_POOL(pool_id);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == GENERAL_MEM_INVALID_ERROR_CODE, "Test #5: Testing 5.02: Failed to test external pool when memory block has invalid checksum");
    set_pool_valid_field(pool_id, checksum);
    _task_set_error(MQX_OK);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6:
*   The function _mem_free_part is used to free a part of memory block.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM001, MEM053, MEM058, MEM059, MEM060, MEM061, TASK006, TASK019.
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
#if MQX_USE_LWMEM_ALLOCATOR
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, TRUE, "Test #6 Testing: 6.01: ");
#else
    /* Allocated block pointer */
    void   *mem_ptr;
    /* Created task's ID */
    _task_id tid;
    /* Checksum value */
    _mqx_uint checksum;
    /* Return value of functions */
    _mqx_uint result;
    /* Memory size */
    _mem_size origin_size, free_size;
    /* allocating size */
    _mqx_uint alloc_size = ALLOC_SIZE + 2 * MQX_MIN_MEMORY_STORAGE_SIZE;

    free_size = ALLOC_SIZE / 2;

    /* Allocate a memory block */
    mem_ptr = _mem_alloc(alloc_size);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, mem_ptr != NULL, "Test #6: Testing 6.01: Allocate a memory block");

    /* [_mem_free_part] Try to free an invalid memory block */
    result = _mem_free_part(NULL, free_size);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_INVALID_POINTER, "Test #6: Testing 6.02: Failed to free an invalid memory block");
    _task_set_error(MQX_OK);

    /* [_mem_free_part] Try to free a memory block with invalid checksum */
    checksum = get_block_checksum(mem_ptr);
    set_block_checksum(mem_ptr, checksum + INVALID_VALUE);
    result = _mem_free_part(mem_ptr, free_size);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_INVALID_CHECKSUM, "Test #6: Testing 6.03: Failed to free a memory block with invalid checksum");
    set_block_checksum(mem_ptr, checksum);
    _task_set_error(MQX_OK);

    /* [_mem_free_part] Try to free a memory block that task doesn't own */
    tid = _task_create(0, TEST_TASK, 0);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, tid != 0, "Test #6: Testing 6.04: Create test task");
    result = _mem_free_part(global_mem_ptr, free_size);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_NOT_RESOURCE_OWNER, "Test #6: Testing 6.05: Failed to free a memory block that task doesn't own");
    _task_set_error(MQX_OK);

    result = _task_destroy(tid);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.06: Destroy test task and all of its resources");

    /* [_mem_free_part] Try to free partial memory with invalid size */
    result = _mem_free_part(mem_ptr, _mem_get_size(mem_ptr) + 1);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_INVALID_SIZE, "Test #6: Testing 6.07: Failed to free partial memory with invalid size");
    _task_set_error(MQX_OK);

    /* Free the allocated memory */
    result = _mem_free(mem_ptr);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.08: Free the allocated memory");

    /* Allocate a bigger memory block and store the pointer in mem_ptr */
    mem_ptr = _mem_alloc(3 * MQX_MIN_MEMORY_STORAGE_SIZE);
    /* Check the returned pointer must be NOT NULL to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, mem_ptr != NULL, "Test #6: Testing 6.09: Allocate a bigger memory block");
    /* Get the size of allocated memory block and store in origin_size */
    origin_size = _mem_get_size(mem_ptr);
    /* Calculate a smaller size to free and store in free_size */
    free_size = origin_size - 2 * MQX_MIN_MEMORY_STORAGE_SIZE + 1;
    /* Free the free_size of memory block mem_ptr */
    result = _mem_free_part(mem_ptr, free_size);

    /* TODO: [ENGR00269142] The returned result is MQX_OK but the part of block was not freed. */
    /* Check the returned value must be MQX_INVALID_SIZE */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_INVALID_SIZE, "Test #6: Testing 6.10: Failed to free partial memory with invalid size");

    /* Verify the size of new block must be not changed */
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _mem_get_size(mem_ptr) == origin_size, "Test #6: Testing 6.11: Verify new size");

    /* Free the allocated memory */
    result = _mem_free(mem_ptr);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.12: Free the allocated memory");
#endif
}

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_mem4)
    EU_TEST_CASE_ADD(tc_1_main_task, "TEST # 1 - Testing function _mem_extend"),
    EU_TEST_CASE_ADD(tc_2_main_task, "TEST # 2 - Testing function _mem_extend_pool"),
    EU_TEST_CASE_ADD(tc_3_main_task, "TEST # 3 - Testing function _mem_get_highwater_pool"),
    EU_TEST_CASE_ADD(tc_4_main_task, "TEST # 4 - Testing function _mem_test"),
    EU_TEST_CASE_ADD(tc_5_main_task, "TEST # 5 - Testing function _mem_test_pool"),
    EU_TEST_CASE_ADD(tc_6_main_task, "TEST # 6 - Testing function _mem_free_part")
EU_TEST_SUITE_END(suite_mem4)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_mem4, " - MQX Memory Component Test Suite, 6 Tests")
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
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
    _mqx_exit(0);
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : test_task
* Comments     :
*   This task is to allocate a private memory segment and block itself
*   before destroying. The memory segment will be used to test accessing
*   resource of another task.
*
*END*----------------------------------------------------------------------*/
void test_task(uint32_t param)
{
    /* Allocate a memory block for transfering */
    global_mem_ptr = _mem_alloc(ALLOC_SIZE);
    /* Block itself */
    _task_block();
}
