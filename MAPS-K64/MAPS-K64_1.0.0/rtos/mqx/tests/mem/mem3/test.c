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
*   of functions creating memory pool and allocating memory.
*
* Requirements:
*
*   MEM001, MEM002, MEM003, MEM005, MEM006, MEM007, MEM010, MEM011, MEM012,
*   MEM014, MEM015, MEM016, MEM030, MEM031, MEM032, MEM034, MEM035, MEM036,
*   MEM038, MEM039, MEM040, MEM042, MEM043, MEM044, MEM046, MEM047, MEM048,
*   MEM053, MEM054, MEM055, MEM056, MEM065, MEM078, MEM079, MEM080, MEM082,
*   MEM083, MEM091, MEM092, MEM093, MEM094.
*
*   LWMEM001, LWMEM003, LWMEM004, LWMEM006, LWMEM008, LWMEM012, LWMEM014,
*   LWMEM015, LWMEM016, LWMEM017, LWMEM018, LWMEM020, LWMEM022, LWMEM023,
*   LWMEM024, LWMEM025, LWMEM026, LWMEM028, LWMEM029, LWMEM030, LWMEM031,
*   LWMEM032, LWMEM033, LWMEM034, LWMEM035.
*
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
void tc_1_main_task(void);  /* TEST # 1 - Testing function _(lw)mem_create_pool */
void tc_2_main_task(void);  /* TEST # 2 - Testing function _(lw)mem_alloc */
void tc_3_main_task(void);  /* TEST # 3 - Testing function _(lw)mem_alloc_zero */
void tc_4_main_task(void);  /* TEST # 4 - Testing function _(lw)mem_alloc_system */
void tc_5_main_task(void);  /* TEST # 5 - Testing function _(lw)mem_alloc_system_zero */
void tc_6_main_task(void);  /* TEST # 6 - Testing function _(lw)mem_alloc_from */
void tc_7_main_task(void);  /* TEST # 7 - Testing function _(lw)mem_alloc_zero_from */
void tc_8_main_task(void);  /* TEST # 8 - Testing function _(lw)mem_alloc_system_from */
void tc_9_main_task(void);  /* TEST # 9 - Testing function _(lw)mem_alloc_system_zero_from */
void tc_10_main_task(void);  /* TEST # 10 - Testing function _(lw)mem_free */
void tc_11_main_task(void);  /* TEST # 11 - Testing function _(lw)mem_transfer */
void tc_12_main_task(void);  /* TEST # 12 - Testing function _(lw)mem_get_size */
void tc_13_main_task(void);  /* TEST # 13 - Testing function _mem_test_all (_lwmem_test) */

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

/******************** Begin Test cases **********************/

/* The memory block is used to test accessing resource of task */
void   *global_mem_ptr;

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*   The function _mem_create_pool is used to create the memory pool from
*   memory that is outside the default memory pool.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM001, MEM046, MEM047, MEM048.
*   LWMEM019
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{

    /* Allocate memory block for external pool */
    pool_alloc_ptr = _mem_alloc(POOL_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, pool_alloc_ptr != NULL, "Test #1: Testing 1.00: Allocate memory for external pool");

#if !MQX_USE_LWMEM_ALLOCATOR
    /* [_mem_create_pool] Try to create pool with too small size */
    ext_pool_id = GENERAL_MEM_CREATE_POOL(pool_alloc_ptr, POOL_INVALID_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ext_pool_id == NULL, "Test #1: Testing 1.01: Failed to create pool with too small size");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MQX_MEM_POOL_TOO_SMALL, "Test #1: Testing 1.03: The task's error code must be MQX_MEM_POOL_TOO_SMALL");
    _task_set_error(MQX_OK);

    /* [_mem_create_pool] Try to create pool when memory is corrupt */
    set_mem_component(MEMPOOL_VALID + INVALID_VALUE);
    ext_pool_id = GENERAL_MEM_CREATE_POOL(pool_alloc_ptr, POOL_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ext_pool_id == NULL, "Test #1: Testing 1.02: Failed to create pool when memory is corrupt");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MQX_CORRUPT_MEMORY_SYSTEM, "Test #1: Testing 1.05: The task's error code must be MQX_CORRUPT_MEMORY_SYSTEM");
    set_mem_component(MEMPOOL_VALID);
    _task_set_error(MQX_OK);

    ext_pool_id = GENERAL_MEM_CREATE_POOL(pool_alloc_ptr, POOL_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ext_pool_id != NULL, "Test #1: Testing 1.03: Create pool sucessful");
#else
    ext_pool_id = GENERAL_MEM_CREATE_POOL(pool_alloc_ptr, POOL_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ext_pool_id != NULL, "Test #1: Testing 1.01: Create pool sucessful");
#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   The function _mem_alloc is used to allocate private memory from
*   default pool.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM002, MEM003.
8   LWMEM002
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    MEMORY_ALLOC_INFO_STRUCT memory_alloc_info;
    /* Allocated block pointer */
    void   *mem_ptr;
    /* Checksum value */
    _mqx_uint checksum;

    /* [_mem_alloc] Try to allocate memory when memory block has invalid */
    checksum = get_pool_valid_field(SYSTEM_POOL_ID);
    set_pool_valid_field(SYSTEM_POOL_ID, checksum + INVALID_VALUE);
    mem_ptr = _mem_alloc(ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, mem_ptr == NULL, "Test #2: Testing 2.00: Failed to allocate memory when memory block has invalid");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == GENERAL_MEM_INVALID_ERROR_CODE, "Test #2: Testing 2.01: The task's error code must not be MQX_OK");
    set_pool_valid_field(SYSTEM_POOL_ID, checksum);
    _task_set_error(MQX_OK);

    /* [_lwmem_alloc] Try to allocate memory when memory is insufficient */
    memory_alloc_all(&memory_alloc_info);
    mem_ptr = _mem_alloc(ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, mem_ptr == NULL, "Test #2: Testing 2.02: Failed to allocate memory when memory is insufficient ");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #2: Testing 2.03: The task's error code must be MQX_OUT_OF_MEMORY ");
    memory_free_all(&memory_alloc_info);
    _task_set_error(MQX_OK);

#if !MQX_USE_LWMEM_ALLOCATOR
    /* [_mem_alloc] Try to allocate memory when memory free list is corrupt */
    task_num = get_block_taskno(SYSTEM_POOL_ID, POOL_TYPE);
    set_block_taskno(SYSTEM_POOL_ID, task_num + INVALID_VALUE, POOL_TYPE);
    mem_ptr = _mem_alloc(ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, mem_ptr == NULL, "Test #2: Testing 2.04: Failed to allocate memory when memory free list is corrupt");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_CORRUPT_STORAGE_POOL_FREE_LIST, "Test #2: Testing 2.05: The task's error code must be MQX_CORRUPT_STORAGE_POOL_FREE_LIST");
    set_block_taskno(SYSTEM_POOL_ID, task_num, POOL_TYPE);
    _task_set_error(MQX_OK);
#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   The function _mem_alloc_zero is used to allocate private memory with
*   zero-filled from default pool.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM010, MEM011, MEM012.
*   LWMEM008
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    /* The linked list of allocated memory blocks */
    MEMORY_ALLOC_INFO_STRUCT memory_alloc_info;
    /* Allocated block pointer */
    void   *mem_ptr;
    /* Checksum value */
    _mqx_uint checksum;

    /* [_mem_alloc_zero] Try to allocate memory when memory block has invalid */
    checksum = get_pool_valid_field(SYSTEM_POOL_ID);
    set_pool_valid_field(SYSTEM_POOL_ID, checksum + INVALID_VALUE);
    mem_ptr = _mem_alloc_zero(ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, mem_ptr == NULL, "Test #3: Testing 3.00: Failed to allocate memory when memory block has invalid");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == GENERAL_MEM_INVALID_ERROR_CODE, "Test #3: Testing 3.01: The task's error code must not be MQX_OK");
    set_pool_valid_field(SYSTEM_POOL_ID, checksum);
    _task_set_error(MQX_OK);

    /* [_mem_alloc_zero] Try to allocate memory when free memory is insufficient */
    memory_alloc_all(&memory_alloc_info);
    _task_set_error(MQX_OK);
    mem_ptr = _mem_alloc_zero(ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, mem_ptr == NULL, "Test #3: Testing 3.02: Failed to allocate memory when free memory is insufficient");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #3: Testing 3.03: The task's error code must be MQX_OUT_OF_MEMORY");
    memory_free_all(&memory_alloc_info);
    _task_set_error(MQX_OK);

#if !MQX_USE_LWMEM_ALLOCATOR
    /* [_mem_alloc_zero] Try to allocate memory when memory free list is corrupt */
    task_num = get_block_taskno(SYSTEM_POOL_ID, POOL_TYPE);
    set_block_taskno(SYSTEM_POOL_ID, task_num + INVALID_VALUE, POOL_TYPE);
    mem_ptr = _mem_alloc_zero(ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, mem_ptr == NULL, "Test #3: Testing 3.04: Failed to allocate memory when memory free list is corrupt");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_CORRUPT_STORAGE_POOL_FREE_LIST, "Test #3: Testing 3.05: The task's error code must be MQX_CORRUPT_STORAGE_POOL_FREE_LIST");
    set_block_taskno(SYSTEM_POOL_ID, task_num, POOL_TYPE);
    _task_set_error(MQX_OK);
#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*   The function _mem_alloc_system is used to allocate system memory from
*   default pool.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM030, MEM031, MEM032
*   LWMEM003, LWMEM004, LWMEM020
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    /* The linked list of allocated memory blocks */
    MEMORY_ALLOC_INFO_STRUCT memory_alloc_info;
    /* Allocated block pointer */
    void   *mem_ptr;
    /* Checksum value */
    _mqx_uint checksum;

    /* [_mem_alloc_system] Try to allocate memory when memory block has invalid */
    checksum = get_pool_valid_field(SYSTEM_POOL_ID);
    set_pool_valid_field(SYSTEM_POOL_ID, checksum + INVALID_VALUE);
    mem_ptr = _mem_alloc_system(ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, mem_ptr == NULL, "Test #4: Testing 4.00: Failed to allocate memory when memory block has invalid");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == GENERAL_MEM_INVALID_ERROR_CODE, "Test #4: Testing 4.01: The task's error code must not be MQX_OK");
    set_pool_valid_field(SYSTEM_POOL_ID, checksum);
    _task_set_error(MQX_OK);

    /* [_mem_alloc_system] Try to allocate memory when free memory is insufficient */
    memory_alloc_all(&memory_alloc_info);
    _task_set_error(MQX_OK);
    mem_ptr = _mem_alloc_system(ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, mem_ptr == NULL, "Test #4: Testing 4.02: Failed to allocate memory when free memory is insufficient");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #4: Testing 4.03: The task's error code must be MQX_OUT_OF_MEMORY");
    memory_free_all(&memory_alloc_info);
    _task_set_error(MQX_OK);

#if MQX_USE_LWMEM_ALLOCATOR
    /* Allocate memory block */
    mem_ptr = _mem_alloc_system(ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, mem_ptr != NULL, "Test #4: Testing 4.04: Allocate memory block");

    /* Free the memory block */
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _mem_free(mem_ptr)== MQX_OK, "Test #4: Testing 4.05: Free the memory block");
#else
    /* [_mem_alloc_system] Try to allocate memory when memory free list is corrupt */
    task_num = get_block_taskno(SYSTEM_POOL_ID, POOL_TYPE);
    set_block_taskno(SYSTEM_POOL_ID, task_num + INVALID_VALUE, POOL_TYPE);
    mem_ptr = _mem_alloc_system(ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, mem_ptr == NULL, "Test #4: Testing 4.04: Failed to allocate memory when memory free list is corrupt");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MQX_CORRUPT_STORAGE_POOL_FREE_LIST, "Test #4: Testing 4.05: The task's error code must be MQX_CORRUPT_STORAGE_POOL_FREE_LIST");
    set_block_taskno(SYSTEM_POOL_ID, task_num, POOL_TYPE);
    _task_set_error(MQX_OK);
#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5:
*   The function _mem_alloc_system_zero is used to allocate system memory
*   with zero-filled from default pool.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM038, MEM039, MEM040
*   LWMEM006
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    /* The linked list of allocated memory blocks */
    MEMORY_ALLOC_INFO_STRUCT memory_alloc_info;
    /* Allocated block pointer */
    void   *mem_ptr;
    /* Checksum value */
    _mqx_uint checksum;

    /* [_mem_alloc_system_zero] Try to allocate memory when memory block has invalid */
    checksum = get_pool_valid_field(SYSTEM_POOL_ID);
    set_pool_valid_field(SYSTEM_POOL_ID, checksum + INVALID_VALUE);
    mem_ptr = _mem_alloc_system_zero(ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, mem_ptr == NULL, "Test #5: Testing 5.00: Failed to allocate memory when memory block has invalid");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == GENERAL_MEM_INVALID_ERROR_CODE, "Test #5: Testing 5.01: The task's error code must be MQX_INVALID_CHECKSUM");
    set_pool_valid_field(SYSTEM_POOL_ID, checksum);
    _task_set_error(MQX_OK);

    /* [_mem_alloc_system_zero] Try to allocate memory when free memory is insufficient */
    memory_alloc_all(&memory_alloc_info);
    _task_set_error(MQX_OK);
    mem_ptr = _mem_alloc_system_zero(ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, mem_ptr == NULL, "Test #5: Testing 5.02: Failed to allocate memory when free memory is insufficient");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #5: Testing 5.03: The task's error code must be MQX_OUT_OF_MEMORY");
    memory_free_all(&memory_alloc_info);
    _task_set_error(MQX_OK);

#if !MQX_USE_LWMEM_ALLOCATOR
    /* [_mem_alloc_system_zero] Try to allocate memory when memory free list is corrupt */
    task_num = get_block_taskno(SYSTEM_POOL_ID, POOL_TYPE);
    set_block_taskno(SYSTEM_POOL_ID, task_num + INVALID_VALUE, POOL_TYPE);
    mem_ptr = _mem_alloc_system_zero(ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, mem_ptr == NULL, "Test #5: Testing 5.04: Failed to allocate memory when memory free list is corrupt");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == MQX_CORRUPT_STORAGE_POOL_FREE_LIST, "Test #5: Testing 5.05: The task's error code must be MQX_CORRUPT_STORAGE_POOL_FREE_LIST");
    set_block_taskno(SYSTEM_POOL_ID, task_num, POOL_TYPE);
    _task_set_error(MQX_OK);
#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6:
*   The function _mem_alloc_from is used to allocate private memory from
*   specified pool.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM001, MEM046, MEM006, MEM007
*   LWMEM012
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    /* Allocated block pointer */
    void   *mem_ptr;
    /* Checksum value */
    _mqx_uint checksum;

    /* [_mem_alloc_from] Try to allocate memory when memory block has invalid */
    checksum = get_pool_valid_field(ext_pool_id);
    set_pool_valid_field(ext_pool_id, checksum + INVALID_VALUE);
    mem_ptr = _mem_alloc_from(ext_pool_id, ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, mem_ptr == NULL, "Test #6: Testing 6.00: Failed to allocate memory when memory block has invalid");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == GENERAL_MEM_INVALID_ERROR_CODE, "Test #6: Testing 6.01: The task's error code must not be MQX_OK");
    set_pool_valid_field(ext_pool_id, checksum);
    _task_set_error(MQX_OK);

    mem_ptr = _mem_alloc_from(ext_pool_id, POOL_SIZE + ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, mem_ptr == NULL, "Test #6: Testing 6.02: Failed to allocate memory when memory is insufficient ");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #6: Testing 6.03: The task's error code must be MQX_OUT_OF_MEMORY ");
    _task_set_error(MQX_OK);

#if !MQX_USE_LWMEM_ALLOCATOR
    /* [_mem_alloc_from] Try to allocate memory when memory free list is corrupt */
    task_num = get_block_taskno(ext_pool_id, POOL_TYPE);
    set_block_taskno(ext_pool_id, task_num + INVALID_VALUE, POOL_TYPE);
    mem_ptr = _mem_alloc_from(ext_pool_id, ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, mem_ptr == NULL, "Test #6: Testing 6.04: Failed to allocate memory when memory free list is corrupt");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == MQX_CORRUPT_STORAGE_POOL_FREE_LIST, "Test #6: Testing 6.05: The task's error code must be MQX_CORRUPT_STORAGE_POOL_FREE_LIST");
    set_block_taskno(ext_pool_id, task_num, POOL_TYPE);
    _task_set_error(MQX_OK);
#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7:
*   The function _mem_alloc_zero_from is used to allocate private memory
*   with zero-filled from specified pool.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM001, MEM046, MEM014, MEM015, MEM016
*   LWMEM017, LWMEM018, LWMEM020
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    /* Allocated block pointer */
    void   *mem_ptr;
    /* Checksum value */
    _mqx_uint checksum;

    /* [_mem_alloc_zero_from] Try to allocate memory when memory block has invalid */
    checksum = get_pool_valid_field(ext_pool_id);
    set_pool_valid_field(ext_pool_id, checksum + INVALID_VALUE);
    mem_ptr = _mem_alloc_zero_from(ext_pool_id, ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, mem_ptr == NULL, "Test #7: Testing 7.00: Failed to allocate memory when memory block has invalid");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, _task_errno == GENERAL_MEM_INVALID_ERROR_CODE, "Test #7: Testing 7.01: The task's error code must not be MQX_OK");
    set_pool_valid_field(ext_pool_id, checksum);
    _task_set_error(MQX_OK);

    /* [_mem_alloc_zero_from] Try to allocate memory when free memory is insufficient */
    _task_set_error(MQX_OK);
    mem_ptr = _mem_alloc_zero_from(ext_pool_id, POOL_SIZE + ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, mem_ptr == NULL, "Test #7: Testing 7.02: Failed to allocate memory when free memory is insufficient");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #7: Testing 7.03: The task's error code must be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

#if MQX_USE_LWMEM_ALLOCATOR
    /* Allocate memory block */
    mem_ptr = _mem_alloc_zero_from(ext_pool_id, ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, mem_ptr != NULL, "Test #7: Testing 7.04: Allocate memory block");

    /* Free the memory block */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, _mem_free(mem_ptr) == MQX_OK, "Test #7: Testing 7.05: Free the memory block ");
#else
    /* [_mem_alloc_zero_from] Try to allocate memory when memory free list is corrupt */
    task_num = get_block_taskno(ext_pool_id, POOL_TYPE);
    set_block_taskno(ext_pool_id, task_num + INVALID_VALUE, POOL_TYPE);
    mem_ptr = _mem_alloc_zero_from(ext_pool_id, ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, mem_ptr == NULL, "Test #7: Testing 7.04: Failed to allocate memory when memory free list is corrupt");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, _task_errno == MQX_CORRUPT_STORAGE_POOL_FREE_LIST, "Test #7: Testing 7.05: The task's error code must be MQX_CORRUPT_STORAGE_POOL_FREE_LIST");
    set_block_taskno(ext_pool_id, task_num, POOL_TYPE);
    _task_set_error(MQX_OK);
#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8:
*   The function _mem_alloc_system_from is used to allocate system memory
*   from specified pool.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM001, MEM046, MEM034, MEM035, MEM036
*   LWMEM014
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
    /* Allocated block pointer */
    void   *mem_ptr;
    /* Checksum value */
    _mqx_uint checksum;

    /* [_mem_alloc_system_from] Try to allocate memory when memory block has invalid */
    checksum = get_pool_valid_field(ext_pool_id);
    set_pool_valid_field(ext_pool_id, checksum + INVALID_VALUE);
    mem_ptr = _mem_alloc_system_from(ext_pool_id, ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, mem_ptr == NULL, "Test #8: Testing 8.00: Failed to allocate memory when memory block has invalid");
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _task_errno == GENERAL_MEM_INVALID_ERROR_CODE, "Test #8: Testing 8.01: The task's error code must not be MQX_OK");
    set_pool_valid_field(ext_pool_id, checksum);
    _task_set_error(MQX_OK);

    /* [_mem_alloc_system_from] Try to allocate memory when free memory is insufficient */
    _task_set_error(MQX_OK);
    mem_ptr = _mem_alloc_system_from(ext_pool_id, POOL_SIZE + ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, mem_ptr == NULL, "Test #8: Testing 8.02: Failed to allocate memory when free memory is insufficient");
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #8: Testing 8.03: The task's error code must be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

#if !MQX_USE_LWMEM_ALLOCATOR
    /* [_mem_alloc_system_from] Try to allocate memory when memory free list is corrupt */
    task_num = get_block_taskno(ext_pool_id, POOL_TYPE);
    set_block_taskno(ext_pool_id, task_num + INVALID_VALUE, POOL_TYPE);
    mem_ptr = _mem_alloc_system_from(ext_pool_id, ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, mem_ptr == NULL, "Test #8: Testing 8.04: Failed to allocate memory when memory free list is corrupt");
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _task_errno == MQX_CORRUPT_STORAGE_POOL_FREE_LIST, "Test #8: Testing 8.05: The task's error code must be MQX_CORRUPT_STORAGE_POOL_FREE_LIST");
    set_block_taskno(ext_pool_id, task_num, POOL_TYPE);
    _task_set_error(MQX_OK);
#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : TEST #9:
*   The function _mem_alloc_system_zero_from is used to allocate system memory
*   with zero-filled from specified pool.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM001, MEM046, MEM042, MEM043, MEM044
*
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
    /* Allocated block pointer */
    void   *mem_ptr;
    /* Checksum value */
    _mqx_uint checksum;

    /* [_mem_alloc_system_zero_from] Try to allocate memory when memory block has invalid */
    checksum = get_pool_valid_field(ext_pool_id);
    set_pool_valid_field(ext_pool_id, checksum + INVALID_VALUE);
    mem_ptr = _mem_alloc_system_zero_from(ext_pool_id, ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, mem_ptr == NULL, "Test #9: Testing 9.00: Failed to allocate memory when memory block has invalid");
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, _task_errno == GENERAL_MEM_INVALID_ERROR_CODE, "Test #9: Testing 9.01: The task's error code must not be MQX_OK");
    set_pool_valid_field(ext_pool_id, checksum );
    _task_set_error(MQX_OK);

    /* [_mem_alloc_system_zero_from] Try to allocate memory when free memory is insufficient */
    _task_set_error(MQX_OK);
    mem_ptr = _mem_alloc_system_zero_from(ext_pool_id, POOL_SIZE + ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, mem_ptr == NULL, "Test #9: Testing 9.02: Failed to allocate memory when free memory is insufficient");
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #9: Testing 9.03: The task's error code must be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

#if MQX_USE_LWMEM_ALLOCATOR
    /* Allocated memory from external pool */
    mem_ptr = _mem_alloc_system_zero_from(ext_pool_id, ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, mem_ptr != NULL, "Test #8: Testing 8.04: Allocated memory from external pool ");

    /* Free the memory block */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _mem_free(mem_ptr) == MQX_OK, "Test #8: Testing 8.05: Free the memory block ");
#else
    /* [_mem_alloc_system_zero_from] Try to allocate memory when memory free list is corrupt */
    task_num = get_block_taskno(ext_pool_id, POOL_TYPE);
    set_block_taskno(ext_pool_id, task_num + INVALID_VALUE, POOL_TYPE);
    mem_ptr = _mem_alloc_system_zero_from(ext_pool_id, ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, mem_ptr == NULL, "Test #9: Testing 9.04: Failed to allocate memory when memory free list is corrupt");
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, _task_errno == MQX_CORRUPT_STORAGE_POOL_FREE_LIST, "Test #9: Testing 9.05: The task's error code must be MQX_CORRUPT_STORAGE_POOL_FREE_LIST");
    set_block_taskno(ext_pool_id, task_num, POOL_TYPE);
    _task_set_error(MQX_OK);
#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_10_main_task
* Comments     : TEST #10:
*   The function _mem_free is used to free an allocated memory block.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM001, MEM053, MEM054, MEM055, MEM056
*   LWMEM032, LWMEM033, LWMEM034, LWMEM035
*   TASK006, TASK019
*
*END*---------------------------------------------------------------------*/
void tc_10_main_task(void)
{
    /* Allocated block pointer */
    void   *mem_ptr;
    /* Created task's ID */
    _task_id tid;
    /* Checksum value */
    _mqx_uint checksum;
    /* Return value of functions */
    _mqx_uint result;

    /* Allocate a memory block */
    mem_ptr = _mem_alloc(ALLOC_SIZE);

    /* [_mem_free] Try to free an invalid memory block */
    result = _mem_free(NULL);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_INVALID_POINTER, "Test #10: Testing 10.00: Failed to free an invalid memory block");

#if MQX_USE_LWMEM_ALLOCATOR

    /* [_mem_free] Try to free a memory block with invalid pool */
    checksum = get_pool_valid_field(SYSTEM_POOL_ID);
    set_pool_valid_field(SYSTEM_POOL_ID, checksum + INVALID_VALUE);
    result = _mem_free(mem_ptr);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_LWMEM_POOL_INVALID, "Test #10: Testing 10.01: Failed to free a memory block with invalid pool ");
    set_pool_valid_field(SYSTEM_POOL_ID, checksum);

#else
    /* [_mem_free] Try to free a memory block with invalid checksum */
    checksum = get_block_checksum(mem_ptr);
    set_block_checksum(mem_ptr, checksum + INVALID_VALUE);
    result = _mem_free(mem_ptr);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_INVALID_CHECKSUM, "Test #10: Testing 10.01: Failed to free a memory block with invalid checksum");
    set_block_checksum(mem_ptr, checksum);
#endif

    /* [_mem_free] Try to free a memory block that task doesn't own */
    tid = _task_create(0, TEST_TASK, 0);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, tid != 0, "Test #10: Testing 10.02: Create test task");
    result = _mem_free(global_mem_ptr);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_NOT_RESOURCE_OWNER, "Test #10: Testing 10.03: Failed to free a memory block that task doesn't own");
    result = _task_destroy(tid);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10: Testing 10.04: Destroy test task and all of its resources");

    /* Free the allocated memory */
    result = _mem_free(mem_ptr);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10: Testing 10.05: Free the allocated memory");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_11_main_task
* Comments     : TEST #11:
*   The function _mem_transfer is used to transfer the ownership of the
*   memory block from one task to another.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM001, MEM053, MEM091, MEM092, MEM093, MEM094, TASK006, TASK019.
*   LWMEM032, LWMEM033, LWMEM034, LWMEM035
*
*END*---------------------------------------------------------------------*/
void tc_11_main_task(void)
{
    /* Allocated block pointer */
    void   *mem_ptr;
    /* Task id of the created task */
    _task_id tid, main_tid;
#if !MQX_USE_LWMEM_ALLOCATOR
    /* Checksum value of block */
    _mqx_uint checksum;
#endif
    /* Return value of functions */
    _mqx_uint result;

    main_tid = _task_get_id();

    /* [_mem_transfer] Try to transfer a memory block which task doesn't own */
    tid = _task_create(0, TEST_TASK, 0);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, tid != MQX_NULL_TASK_ID, "Test #11: Testing 11.00: Create test task");
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, tid == memory_get_owner(global_mem_ptr), "Test #11: Testing 11.01: Create test task");

    /* Allocate a memory block */
    mem_ptr = _mem_alloc(ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, mem_ptr != NULL, "Test #11: Testing 11.02: Allocate memory for memory pool");
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, main_tid == memory_get_owner(mem_ptr), "Test #11: Testing 11.03: Create test task");

    /* [_mem_transfer] Try to transfer an invalid memory block */
    result = _mem_transfer(NULL, main_tid, tid);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_INVALID_POINTER, "Test #11: Testing 11.04.0: Failed to transfer an invalid memory block");

#if !MQX_USE_LWMEM_ALLOCATOR
    /* [_mem_transfer] Try to transfer a memory block, which has incorrect checksum */
    checksum = get_block_checksum(mem_ptr);
    set_block_checksum(mem_ptr, checksum + INVALID_VALUE);
    result = _mem_transfer(mem_ptr, main_tid, tid);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_INVALID_CHECKSUM, "Test #11: Testing 11.04.1: Failed to transfer a memory block, which has incorrect checksum");
    set_block_checksum(mem_ptr, checksum);
#endif

    /* [_mem_transfer] Try to transfer a memory block when task id is invalid */
    result = _mem_transfer(mem_ptr, main_tid, INVALID_TASK_ID);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_INVALID_TASK_ID, "Test #11: Testing 11.05: Failed to transfer a memory block when task id is invalid");
    result = _mem_transfer(mem_ptr, INVALID_TASK_ID, tid);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_INVALID_TASK_ID, "Test #11: Testing 11.06: Failed to transfer a memory block when task id is invalid");

    result = _mem_transfer(global_mem_ptr, main_tid, tid);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_NOT_RESOURCE_OWNER, "Test #11: Testing 11.07: Failed to transfer a memory block which task doesn't own");

    /* [_lwmem_transfer] Try to transfer the ownership of the memory block from main task to TEST_TASK */
    result = _mem_transfer(mem_ptr, main_tid, tid);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "Test #11: Testing 11.08: Transfer the ownership of the memory block from TEST_TASK to main task ");
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, tid == memory_get_owner(mem_ptr), "Test #11: Testing 11.09: Create test task");

    /* Free the allocated private memory */
    result = _mem_free(mem_ptr);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result = MQX_NOT_RESOURCE_OWNER, "Test #11: Testing 11.10: Free the allocated private memory that does not own ");

    /* [_lwmem_transfer] Try to transfer the ownership of the memory block from TEST_TASK to main task */
    result = _mem_transfer(global_mem_ptr, tid, main_tid);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "Test #11: Testing 11.11: Transfer the ownership of the memory block from TEST_TASK to main task ");
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, main_tid == memory_get_owner(global_mem_ptr), "Test #11: Testing 11.11: Create test task");

    /* Free the allocated private memory */
    result = _mem_free(global_mem_ptr);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "Test #11: Testing 11.13: Free the allocated private memory");

    result = _task_destroy(tid);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "Test #11: Testing 11.14: Destroy the test task");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_12_main_task
* Comments     : TEST #12:
*   The function _mem_get_size is used to get the size of a memory block.
*   This test case is to verify all the uncovered cases of this function.
* Requirements :
*   MEM001, MEM005, MEM046, MEM053, MEM065.
*   LWMEM003, LWMEM025, LWMEM026
*
*END*---------------------------------------------------------------------*/
void tc_12_main_task(void)
{
#if !MQX_USE_LWMEM_ALLOCATOR
    /* Allocated block pointer */
    void   *mem_ptr;
    /* Current error memory block */
    void   *error_block_ptr;
    /* Checksum value */
    _mqx_uint checksum;
    /* Return value of functions */
    _mqx_uint result;
#endif
    /* Size of memory block */
    _mem_size block_size;

    /* [_mem_get_size] Try to get size of an invalid memory block */
    block_size = GENERAL_MEM_GET_SIZE(NULL);
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, block_size == 0, "Test #12: Testing 12.00: Failed to get size of an invalid memory block");
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, _task_errno == MQX_INVALID_POINTER, "Test #12: Testing 12.01: The task's error code must be MQX_INVALID_POINTER");
    _task_set_error(MQX_OK);

#if !MQX_USE_LWMEM_ALLOCATOR
    /* Allocate private memory from default pool */
    mem_ptr = _mem_alloc(ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, mem_ptr != NULL, "Test #12: Testing 12.02: Allocate private memory from default pool");

    /* [_mem_get_size] Try to get size of a memory block, which has incorrect checksum */
    checksum = get_block_checksum(mem_ptr);
    set_block_checksum(mem_ptr, checksum + INVALID_VALUE);
    block_size = GENERAL_MEM_GET_SIZE(mem_ptr);
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, block_size == 0, "Test #12: Testing 12.03: Failed to get size of a memory block, which has incorrect checksum");
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, _task_errno == MQX_INVALID_CHECKSUM, "Test #12: Testing 12.04: The task's error code must be MQX_INVALID_CHECKSUM");
    set_block_checksum(mem_ptr, checksum);
    _task_set_error(MQX_OK);

    /* [_mem_get_size] Try to get size of a corrupt memory block */
    error_block_ptr = get_kernel_error_block();
    task_num = get_block_taskno(mem_ptr, BLOCK_TYPE);
    set_block_taskno(mem_ptr, 0, BLOCK_TYPE);
    block_size = GENERAL_MEM_GET_SIZE(mem_ptr);
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, block_size == 0, "Test #12: Testing 12.05: Failed to get size of a corrupt memory block");
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, _task_errno == MQX_CORRUPT_STORAGE_POOL, "Test #12: Testing 12.06: The task's error code must be MQX_CORRUPT_STORAGE_POOL");
    set_block_taskno(mem_ptr, task_num, BLOCK_TYPE);
    set_kernel_error_block(error_block_ptr);
    _task_set_error(MQX_OK);

    /* Free the allocated private memory */
    result = _mem_free(mem_ptr);
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "Test #12: Testing 12.07: Free the allocated private memory");
#endif

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_13_main_task
* Comments     : TEST #13:
*   The function _mem_test_all is used to tests the memory in all memory pools
*   This test case is to verify this function shall work properly.
* Requirements :
*   MEM078, MEM079, MEM080, MEM082, MEM083
*   LWMEM028, LWMEM029, LWMEM030, LWMEM031
*
*END*---------------------------------------------------------------------*/
void tc_13_main_task(void)
{
#if !MQX_USE_LWMEM_ALLOCATOR
    /* Pool queue pointer */
    QUEUE_STRUCT_PTR pool_queue_ptr;
#endif
    /* Pool's free list pointer */
    void   *temp_ptr;
    /* Error block pointer */
    void   *error_block_ptr;
    /* Checksum value */
    _mqx_uint checksum;
    /* Return value of functions */
    _mqx_uint result;

    /* TODO: [ENGR00253445] Everything is OK but returned error pool is still available */
    /* [_mem_test_all] Test all memory pool with no error */
    GENERAL_MEM_TEST_ALL(result, pool_error, block_error);
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, result == MQX_OK, "Test #13: Testing 13.00: Test all memory pool with no error");
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, pool_error == NULL, "Test #13: Testing 13.01.01: The returned error pool must be NULL");
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, block_error == NULL, "Test #13: Testing 13.01.02: The returned error block must be NULL");

#if !MQX_USE_LWMEM_ALLOCATOR
    /* [_mem_test_all] Try to test all memory pool when pool queue is invalid */
    error_block_ptr = get_kernel_error_block();
    pool_queue_ptr = get_kernel_pool_queue();
    set_kernel_pool_queue(pool_queue_ptr->PREV);
    GENERAL_MEM_TEST_ALL(result, pool_error, block_error);
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, result == MQX_CORRUPT_QUEUE, "Test #13: Testing 13.02: Failed to test all memory pool when pool queue is invalid");
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, pool_error == pool_queue_ptr->PREV, "Test #13: Testing 13.03: The returned error pool must be not NULL");
    set_kernel_pool_queue(pool_queue_ptr);
    set_kernel_error_block(error_block_ptr);
    _task_set_error(MQX_OK);

    /* [_mem_test_all] Try to test all memory pool when a pool's physical block is invalid*/
    error_block_ptr = get_kernel_error_block();
    temp_ptr = get_block_physical(SYSTEM_POOL_ID);
    set_block_physical(SYSTEM_POOL_ID, (void *)INVALID_VALUE);
    GENERAL_MEM_TEST_ALL(result, pool_error, block_error);
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, result == MQX_CORRUPT_STORAGE_POOL, "Test #13: Testing 13.04: Failed to test all memory pool when a pool's physical block is invalid");
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, pool_error == SYSTEM_POOL_ID, "Test #13: Testing 13.05.01: The returned error pool must be not NULL");
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, block_error != NULL, "Test #13: Testing 13.05.02: The returned error block must be not NULL");
    set_block_physical(SYSTEM_POOL_ID, temp_ptr);
    set_kernel_error_block(error_block_ptr);
    _task_set_error(MQX_OK);
#else
    temp_ptr = get_pool_storage_ptr(SYSTEM_POOL_ID);
    /* Makes the block of the allocate pool to invalid */
    set_pool_storage_ptr(SYSTEM_POOL_ID, NULL);
    /*[_lwmem_test] Test all memory when the a memory pool pointer is not correct */
    GENERAL_MEM_TEST_ALL(result, pool_error, block_error);
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, (result == MQX_CORRUPT_STORAGE_POOL), "Test #13: Testing 13.02: Tests function _lwmem_test when the a memory pool void *is not correct ") ;
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, pool_error == SYSTEM_POOL_ID, "Test #13: Testing 13.03.01: The returned error pool must be not NULL");
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, block_error != NULL, "Test #13: Testing 13.03.02: The returned error block must be not NULL");
    /* Restores the block of allocate pool */
    set_pool_storage_ptr(SYSTEM_POOL_ID, temp_ptr);
#endif

    /* [_mem_test_all] Try to test all memory pool when memory free list is corrupt */
    error_block_ptr = get_kernel_error_block();
    temp_ptr = get_pool_free_list_ptr(SYSTEM_POOL_ID);
    set_pool_free_list_ptr(SYSTEM_POOL_ID, (void *)INVALID_VALUE);
    GENERAL_MEM_TEST_ALL(result, pool_error, block_error);
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, result == MQX_CORRUPT_STORAGE_POOL_FREE_LIST, "Test #13: Testing 13.06: Failed to test all memory pool when memory free list is corrupt");
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, pool_error == SYSTEM_POOL_ID, "Test #13: Testing 13.07.01: The returned error pool must be not NULL");
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, block_error != NULL, "Test #13: Testing 13.07.02: The returned error block must be not NULL");
    set_pool_free_list_ptr(SYSTEM_POOL_ID, temp_ptr);
    set_kernel_error_block(error_block_ptr);
    _task_set_error(MQX_OK);

    /* [_mem_test_all] Try to test all memory pool when memory pool is not valid */
    checksum = get_pool_valid_field(SYSTEM_POOL_ID);
    set_pool_valid_field(SYSTEM_POOL_ID, checksum + INVALID_VALUE);
    GENERAL_MEM_TEST_ALL(result, pool_error, block_error);
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, result == GENERAL_MEM_INVALID_ERROR_CODE, "Test #13: Testing 13.08: Failed to test all memory pool when memory block has invalid checksum");
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, pool_error == SYSTEM_POOL_ID, "Test #13: Testing 13.09: The returned error pool must be not NULL");
    set_pool_valid_field(SYSTEM_POOL_ID, checksum);
    _task_set_error(MQX_OK);

    /* TODO: [ENGR00253445] Everything is OK but returned error pool is still available */
    /* [_mem_test_all] Test all memory pool with no error */
    GENERAL_MEM_TEST_ALL(result, pool_error, block_error);
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, result == MQX_OK, "Test #13: Testing 13.10: Test all memory pool with no error");
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, pool_error == NULL, "Test #13: Testing 13.11.01: The returned error pool must be NULL");
    EU_ASSERT_REF_TC_MSG(tc_13_main_task, block_error == NULL, "Test #13: Testing 13.11.02: The returned error block must be NULL");
}

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_mem3)
    EU_TEST_CASE_ADD(tc_1_main_task, "TEST # 1 - Testing function _(lw)mem_create_pool"),
    EU_TEST_CASE_ADD(tc_2_main_task, "TEST # 2 - Testing function _(lw)mem_alloc"),
    EU_TEST_CASE_ADD(tc_3_main_task, "TEST # 3 - Testing function _(lw)mem_alloc_zero"),
    EU_TEST_CASE_ADD(tc_4_main_task, "TEST # 4 - Testing function _(lw)mem_alloc_system"),
    EU_TEST_CASE_ADD(tc_5_main_task, "TEST # 5 - Testing function _(lw)mem_alloc_system_zero"),
    EU_TEST_CASE_ADD(tc_6_main_task, "TEST # 6 - Testing function _(lw)mem_alloc_from"),
    EU_TEST_CASE_ADD(tc_7_main_task, "TEST # 7 - Testing function _(lw)mem_alloc_zero_from"),
    EU_TEST_CASE_ADD(tc_8_main_task, "TEST # 8 - Testing function _(lw)mem_alloc_system_from"),
    EU_TEST_CASE_ADD(tc_9_main_task, "TEST # 9 - Testing function _(lw)mem_alloc_system_zero_from"),
    EU_TEST_CASE_ADD(tc_10_main_task," TEST # 10 - Testing function _(lw)mem_free"),
    EU_TEST_CASE_ADD(tc_11_main_task," TEST # 11 - Testing function _(lw)mem_transfer"),
    EU_TEST_CASE_ADD(tc_12_main_task," TEST # 12 - Testing function _(lw)mem_get_size"),
    EU_TEST_CASE_ADD(tc_13_main_task," TEST # 13 - Testing function _mem_test_all (_lwmem_test)")
EU_TEST_SUITE_END(suite_mem3)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_mem3, " - MQX Memory Component Test Suite, 13 Tests")
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
