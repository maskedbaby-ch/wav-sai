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
* $Date    : Jun-03-2013$
*
* Comments:
*
*   This file contains code for the MQX Memory verification program, includes
*   all test cases of functions allocating memory with alignment option.
*
* Requirements:
*
*   MEM001, MEM017, MEM018m MEM019, MEM020, MEM021, MEM022, MEM023, MEM024,
*   MEM046, MEM053, MEM106, MEM107.
*
*   LWMEM001, LWMEM019, LWMEM020, LWMEM036, LWMEM037, LWMEM038, LWMEM039,
*   LWMEM040, LWMEM041.
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
void tc_1_main_task(void);  /* TEST # 0 - Testing function _(lw)mem_alloc_align in normal case */
void tc_2_main_task(void);  /* TEST # 1 - Testing function _(lw)mem_alloc_align_from in normal case */
void tc_3_main_task(void);  /* TEST # 2 - Testing function _(lw)mem_alloc_align in special cases */
void tc_4_main_task(void);  /* TEST # 3 - Testing function _(lw)mem_alloc_align_from in special cases */

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
    {         0,              0,         0,  0,      0,                      0}
};

/******************** Begin Test cases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*   This test case is to verify the success case of function
*   _(lw)mem_alloc_align
* Requirements :
*   MEM001, MEM017, MEM053.
*   LWMEM001, LWMEM020, LWMEM036, MEM106.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    void   *p0;
    uint32_t req_align;

#if !MQX_USE_LWMEM_ALLOCATOR
    uint32_t p0_address = 0;
    uint32_t p0_size = 0;
#endif

    valid_alignment = 1;
    _MEMORY_ALIGN_VAL_LARGER(valid_alignment);

    /* allocate memory with the requested align is memory alignment value must be successful */
    p0 = _mem_alloc_align(TEST_SIZE_1, valid_alignment);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, p0 != NULL, "Test #1: Testing 1.01: allocate aligned memory with memory alignment value" );
    if (p0 == NULL)
    {
        return;
    }

#if MQX_USE_LWMEM_ALLOCATOR
    /* TODO: [ENGR00264968] The test causes the following tests cannot run */
    /* While the CR is not fixed, disable this test to run all cases */
#if 1
    _mem_free(p0);
    for (req_align = valid_alignment + 1; req_align <= (2 * valid_alignment); req_align++)
    {
        p0 = _mem_alloc_align(TEST_SIZE_1, req_align);
        if (p0 != NULL)
        {
            break;
        }
    }
    /* At least success with req_align = 2 * valid_alignment */
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, p0 != NULL, "Test #1: Testing 1.02: allocate aligned memory " );
#endif

#else

    p0_address = (uint32_t)p0;
    p0_size = GENERAL_MEM_GET_BLOCK_SIZE(p0);
    _mem_free(p0);

    /* Check req_align is less than valid_alignment should be behaved as valid_alignment */
    for (req_align = 1; req_align < valid_alignment; req_align++)
    {
        void   *p1;
        uint32_t p1_address = 0;
        uint32_t p1_size = 0;

        p1 = _mem_alloc_align(TEST_SIZE_1, req_align);

        /* Verify the req_align is not power of 2 should failed */
        if (req_align & (req_align -1))
        {
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, p1 == NULL, "TEST #1: Testing 1.02: req_align is not power of 2 should failed");
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MQX_INVALID_PARAMETER, "TEST #1: Testing 1.03: The task error code must be MQX_INVALID_PARAMETER");
            _task_set_error(MQX_OK);

            if (p1 != NULL)
            {
                return;
            }
        }
        else
        {
            EU_ASSERT_REF_TC_MSG( tc_1_main_task, p1 != NULL, "Test #1: Testing 1.04: allocate aligned memory with memory alignment value" );
            p1_address = (uint32_t)p1;
            p1_size = GENERAL_MEM_GET_BLOCK_SIZE(p1);
            _mem_free(p1);

            /* Check it is allocated in the same block and size with p0 */
            EU_ASSERT_REF_TC_MSG( tc_1_main_task, (p0_address == p1_address) && (p0_size == p1_size), "Test #1: Testing 1.05: allocate aligned memory " );
        }
    }

    for (req_align = valid_alignment + 1; req_align <= (2 * valid_alignment); req_align++)
    {
        p0 = _mem_alloc_align(TEST_SIZE_1, req_align);

        /* Verify the req_align is not power of 2 should failed */
        if (req_align & (req_align -1))
        {
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, p0 == NULL, "TEST #1: Testing 1.06: req_align is not power of 2 should failed");
            EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MQX_INVALID_PARAMETER, "TEST #1: Testing 1.07: The task error code must be MQX_INVALID_PARAMETER");
            _task_set_error(MQX_OK);

            if (p0 != NULL)
            {
                return;
            }
        }
        else
        {
            if (p0 != NULL)
            {
                break;
            }
            /* Verify if failed in this case, the error code should be MQX_INVALID_CONFIGURATION*/
            else
            {
                EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MQX_INVALID_CONFIGURATION, "TEST #1: Testing 1.08: The task error code must be MQX_INVALID_CONFIGURATION");
                _task_set_error(MQX_OK);
            }
        }
    }

    /* At least success with req_align = 2 * valid_alignment */
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, p0 != NULL, "Test #1: Testing 1.09: allocate aligned memory " );
    if (p0 == NULL)
    {
        return;
    }

#endif /* MQX_USE_LWMEM_ALLOCATOR */

    _mem_free(p0);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   This test case is to verify the success case of function
*   _(lw)mem_alloc_align_from
* Requirements :
*   MEM001, MEM021, MEM053.
*   LWMEM001, LWMEM020, LWMEM039, MEM107.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    void   *p0;
    uint32_t req_align;

#if !MQX_USE_LWMEM_ALLOCATOR
    uint32_t p0_address = 0;
    uint32_t p0_size = 0;
#endif

    POOL_SIZE = get_free_mem() / 2;

    /* Create external memory pool */
    pool_alloc_ptr = _mem_alloc (POOL_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, pool_alloc_ptr != NULL, "TEST #2: Testing 2.01: Allocate memory for memory pool");

    ext_pool_id = _mem_create_pool(pool_alloc_ptr, POOL_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ext_pool_id != NULL, "TEST #2: Testing 2.02: Create external memory pool");

    /* allocate memory with the requested align is memory alignment value must be successful */
    p0 = _mem_alloc_align_from(ext_pool_id, TEST_SIZE_1, valid_alignment);
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, p0 != NULL, "Test #2: Testing 2.03: allocate aligned memory with memory alignment value" );
    if (p0 == NULL)
    {
        return;
    }

#if MQX_USE_LWMEM_ALLOCATOR
    /* TODO: [ENGR00264968] The test causes the following tests cannot run */
    /* While the CR is not fixed, disable this test to run all cases */
#if 1
    _mem_free(p0);
    for (req_align = valid_alignment + 1; req_align <= (2 * valid_alignment); req_align++)
    {
        p0 = _mem_alloc_align_from(ext_pool_id, TEST_SIZE_1, req_align);
        if (p0 != NULL)
        {
            break;
        }
    }
    /* At least success with req_align = 2 * valid_alignment */
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, p0 != NULL, "Test #2: Testing 2.02: allocate aligned memory " );
#endif

#else

    p0_address = (uint32_t)p0;
    p0_size = GENERAL_MEM_GET_BLOCK_SIZE(p0);
    _mem_free(p0);

    /* Check req_align is less than valid_alignment should be behaved as valid_alignment */
    for (req_align = 1; req_align < valid_alignment; req_align++)
    {
        void   *p1;
        uint32_t p1_address = 0;
        uint32_t p1_size = 0;

        p1 = _mem_alloc_align_from(ext_pool_id, TEST_SIZE_1, req_align);

        /* Verify the req_align is not power of 2 should failed */
        if (req_align & (req_align -1))
        {
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, p1 == NULL, "TEST #2: Testing 2.04: req_align is not power of 2 should failed");
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_INVALID_PARAMETER, "TEST #1: Testing 2.05: The task error code must be MQX_INVALID_PARAMETER");
            _task_set_error(MQX_OK);

            if (p1 != NULL)
            {
                return;
            }
        }
        else
        {
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, p1 != NULL, "Test #2: Testing 2.06: allocate aligned memory with memory alignment value" );
            p1_address = (uint32_t)p1;
            p1_size = GENERAL_MEM_GET_BLOCK_SIZE(p1);
            _mem_free(p1);

            /* Check it is allocated in the same block and size with p0 */
            EU_ASSERT_REF_TC_MSG( tc_2_main_task, (p0_address == p1_address) && (p0_size == p1_size), "Test #2: Testing 2.07: allocate aligned memory " );

            if (!((p0_address == p1_address) && (p0_size == p1_size))){
                printf("req_align = %d\n", req_align);
            }
        }
    }

    for (req_align = valid_alignment + 1; req_align <= (2 * valid_alignment); req_align++)
    {
        p0 = _mem_alloc_align_from(ext_pool_id, TEST_SIZE_1, req_align);

        /* Verify the req_align is not power of 2 should failed */
        if (req_align & (req_align -1))
        {
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, p0 == NULL, "TEST #2: Testing 2.08: req_align is not power of 2 should failed");
            EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_INVALID_PARAMETER, "TEST #2: Testing 2.09: The task error code must be MQX_INVALID_PARAMETER");
            _task_set_error(MQX_OK);

            if (p0 != NULL)
            {
                return;
            }
        }
        else
        {
            if (p0 != NULL)
            {
                break;
            }
            /* Verify if failed in this case, the error code should be MQX_INVALID_CONFIGURATION*/
            else
            {
                EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_INVALID_CONFIGURATION, "TEST #2: Testing 2.10: The task error code must be MQX_INVALID_CONFIGURATION");
                _task_set_error(MQX_OK);
            }
        }
    }

    /* At least success with req_align = 2 * valid_alignment */
    EU_ASSERT_REF_TC_MSG( tc_2_main_task, p0 != NULL, "Test #2: Testing 2.11: allocate aligned memory " );
    if (p0 == NULL)
    {
        return;
    }

#endif /* MQX_USE_LWMEM_ALLOCATOR */

    _mem_free(p0);
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   The function _(lw)mem_alloc_align are used to allocate private memory with
*   aligned-address from default memory pool.
*   This test case is to verify all uncovered cases of this function.
* Requirements :
*   MEM018, MEM019, MEM020.
*   LWMEM037, LWMEM038.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    /* Allocated memory list */
    MEMORY_ALLOC_INFO_STRUCT memory_alloc_info;
    /* Allocated block pointer */
    void   *mem_ptr;
#if !MQX_USE_LWMEM_ALLOCATOR
    /* Task's number value */
    _task_number task_num;
    /* Checksum value */
    _mqx_uint checksum;
#endif

    /* [_mem_alloc_align] Try to allocate memory when pool's memory is insufficient */
    memory_alloc_all(&memory_alloc_info);
    _task_set_error(MQX_OK);
    mem_ptr = _mem_alloc_align(ALLOC_SIZE, valid_alignment);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, mem_ptr == NULL, "TEST #3: Testing 3.01: Failed to allocate memory when when pool's memory is insufficient");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_OUT_OF_MEMORY, "TEST #3: Testing 3.02: The task's error code must be MQX_OUT_OF_MEMORY");
    memory_free_all(&memory_alloc_info);
    _task_set_error(MQX_OK);

#if MQX_USE_LWMEM_ALLOCATOR
    /* [_mem_alloc_align] Try to allocate memory when memory pool is invalid */
    ((LWMEM_POOL_STRUCT_PTR)SYSTEM_POOL_ID)->VALID = LWMEM_POOL_VALID + INVALID_VALUE;
    mem_ptr = _mem_alloc_align(ALLOC_SIZE, valid_alignment);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, mem_ptr == NULL, "TEST #3: Testing 3.03: Failed to allocate memory when memory pool is invalid");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_LWMEM_POOL_INVALID, "TEST #3: Testing 3.04: The task's error code must be MQX_LWMEM_POOL_INVALID");
    set_pool_valid_field(SYSTEM_POOL_ID, LWMEM_POOL_VALID);
    ((LWMEM_POOL_STRUCT_PTR)SYSTEM_POOL_ID)->VALID = LWMEM_POOL_VALID;
    _task_set_error(MQX_OK);
#else
    /* [_mem_alloc_align] Try to allocate memory when memory free list is corrupt */
    task_num = get_block_taskno(SYSTEM_POOL_ID, POOL_TYPE);
    set_block_taskno(SYSTEM_POOL_ID, task_num + INVALID_VALUE, POOL_TYPE);
    mem_ptr = _mem_alloc_align(ALLOC_SIZE, valid_alignment);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, mem_ptr == NULL, "TEST #3: Testing 3.05: Failed to allocate memory when memory free list is corrupt");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_CORRUPT_STORAGE_POOL_FREE_LIST, "TEST #3: Testing 3.06: The task's error code must be MQX_CORRUPT_STORAGE_POOL_FREE_LIST");
    set_block_taskno(SYSTEM_POOL_ID, task_num, POOL_TYPE);
    _task_set_error(MQX_OK);

    /* [_mem_alloc_align] Try to allocate memory when memory block has invalid checksum */
    checksum = get_pool_valid_field(SYSTEM_POOL_ID);
    set_pool_valid_field(SYSTEM_POOL_ID, checksum + INVALID_VALUE);
    mem_ptr = _mem_alloc_align(ALLOC_SIZE, valid_alignment);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, mem_ptr == NULL, "TEST #3: Testing 3.07: Failed to allocate memory when memory block has invalid checksum");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_INVALID_CHECKSUM, "TEST #3: Testing 3.08: The task's error code must be MQX_INVALID_CHECKSUM");
    set_pool_valid_field(SYSTEM_POOL_ID, checksum);
    _task_set_error(MQX_OK);

#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*   The function _(lw)mem_alloc_align_from are used to allocate private memory
*   with aligned-address from external memory pool.
*   This test case is to verify all uncovered cases of this function.
* Requirements :
*   MEM001, MEM021, MEM022, MEM023, MEM024, MEM046, MEM053.
*   LWMEM001, LWMEM019, LWMEM020, LWMEM039, LWMEM040, LWMEM041.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    /* Allocated block pointer */
    void   *mem_ptr;
#if !MQX_USE_LWMEM_ALLOCATOR
    /* Task's number value */
    _task_number task_num;
    /* Checksum value */
    _mqx_uint checksum;
#endif

    /* [_mem_alloc_align_from] Allocate memory with MQX default alignment from external pool */
    mem_ptr = _mem_alloc_align_from(ext_pool_id, ALLOC_SIZE, valid_alignment);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, mem_ptr != NULL, "TEST #4: Testing 4.01: Allocate mem with default alignment");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, TEST_MEM_ALIGN(mem_ptr, valid_alignment), "TEST #4: Testing 4.02: Check address alignment");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _mem_free(mem_ptr) == MQX_OK, "TEST #4: Testing 4.03: Free mem");

    /* [_mem_alloc_align_from] Allocate memory with 32 bytes alignment from external pool */
    mem_ptr = _mem_alloc_align_from(ext_pool_id, ALLOC_SIZE, 2*valid_alignment);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, mem_ptr != NULL, "TEST #4: Testing 4.04: Allocate mem with 32 bytes alignment");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, TEST_MEM_ALIGN(mem_ptr, 2*valid_alignment), "TEST #4: Testing 4.05: Check address alignment");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _mem_free(mem_ptr) == MQX_OK, "TEST #4: Testing 4.06: Free mem");

    /* [_mem_alloc_align_from] Allocate memory with 64 bytes alignment from external pool */
    mem_ptr = _mem_alloc_align_from(ext_pool_id, ALLOC_SIZE, 4*valid_alignment);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, mem_ptr != NULL, "TEST #4: Testing 4.07: Allocate mem with 64 bytes alignment");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, TEST_MEM_ALIGN(mem_ptr, 4*valid_alignment), "TEST #4: Testing 4.08: Check address alignment");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _mem_free(mem_ptr) == MQX_OK, "TEST #4: Testing 4.09: Free mem");

    /* [_mem_alloc_align_from] Try to allocate memory when pool's memory is insufficient */
    mem_ptr = _mem_alloc_align_from(ext_pool_id, POOL_SIZE + ALLOC_SIZE, valid_alignment);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, mem_ptr == NULL, "TEST #4: Testing 4.10: Failed to allocate memory when when pool's memory is insufficient");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MQX_OUT_OF_MEMORY, "TEST #4: Testing 4.11: The task's error code must be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

#if MQX_USE_LWMEM_ALLOCATOR
    /* [_mem_alloc_align_from] Try to allocate memory when memory pool is invalid */
    ((LWMEM_POOL_STRUCT_PTR)ext_pool_id)->VALID = LWMEM_POOL_VALID + INVALID_VALUE;
    mem_ptr = _mem_alloc_align_from(ext_pool_id, ALLOC_SIZE, valid_alignment);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, mem_ptr == NULL, "TEST #4: Testing 4.12: Failed to allocate memory when when memory pool is invalid");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MQX_LWMEM_POOL_INVALID, "TEST #4: Testing 4.13: The task's error code must be MQX_LWMEM_POOL_INVALID");
    ((LWMEM_POOL_STRUCT_PTR)ext_pool_id)->VALID = LWMEM_POOL_VALID;
    _task_set_error(MQX_OK);
#else
    /* [_mem_alloc_align_from] Try to allocate memory when memory free list is corrupt */
    task_num = get_block_taskno(ext_pool_id, POOL_TYPE);
    set_block_taskno(ext_pool_id, task_num + INVALID_VALUE, POOL_TYPE);
    mem_ptr = _mem_alloc_align_from(ext_pool_id, ALLOC_SIZE, valid_alignment);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, mem_ptr == NULL, "TEST #4: Testing 4.12: Failed to allocate memory when memory free list is corrupt");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MQX_CORRUPT_STORAGE_POOL_FREE_LIST, "TEST #4: Testing 4.13: The task's error code must be MQX_CORRUPT_STORAGE_POOL_FREE_LIST");
    set_block_taskno(ext_pool_id, task_num, POOL_TYPE);
    _task_set_error(MQX_OK);

    /* [_mem_alloc_align_from] Try to allocate memory when memory block has invalid checksum */
    checksum = get_pool_valid_field(ext_pool_id);
    set_pool_valid_field(ext_pool_id, checksum + INVALID_VALUE);
    mem_ptr = _mem_alloc_align_from(ext_pool_id, ALLOC_SIZE, valid_alignment);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, mem_ptr == NULL, "TEST #4: Testing 4.14: Failed to allocate memory when memory block has invalid checksum");
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, _task_errno == MQX_INVALID_CHECKSUM, "TEST #4: Testing 4.15: The task's error code must be MQX_INVALID_CHECKSUM");
    set_pool_valid_field(ext_pool_id, checksum);
    _task_set_error(MQX_OK);
#endif

    _mem_free(pool_alloc_ptr);
}

/******************** End Test cases **********************/

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_align)
    EU_TEST_CASE_ADD(tc_1_main_task, "TEST # 1 - Testing function _(lw)mem_alloc_align in normal case"),
    EU_TEST_CASE_ADD(tc_2_main_task, "TEST # 2 - Testing function _(lw)mem_alloc_align_from in normal case"),
    EU_TEST_CASE_ADD(tc_3_main_task, "TEST # 3 - Testing function _(lw)mem_alloc_align in special cases"),
    EU_TEST_CASE_ADD(tc_4_main_task, "TEST # 4 - Testing function _(lw)mem_alloc_align_from in special cases"),
EU_TEST_SUITE_END(suite_align)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_align, " - MQX Memory Component Test Suite, 4 Tests")
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
