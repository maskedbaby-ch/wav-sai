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
* $Version : 4.0.2$
* $Date    : May-29-2013$
*
* Comments:
*
*   This file contains code for the MQX Memory verification program.
*
* Requirements: MEM200, MEM201, MEM202,MEM203, MEM204, MEM205,
*               MEM206, MEM207, MEM208, MEM209
*               LWMEM200, LWMEM201, LWMEM202, LWMEM205, LWMEM206,
*               LWMEM208
*
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <mqx.h>
#include "util.h"
#include "test.h"
#include <mem_prv.h>
#if MQX_USE_LWMEM_ALLOCATOR
#include <lwmem.h>
#endif

#include "EUnit.h"
#include "eunit_crit_secs.h"
#include "mem_util.h"

/*------------------------------------------------------------------------
 * Test suite function prototype
 */
void tc_1_main_task(void);  /* TEST #1 - Testing the function _mem_alloc_system_align() */
void tc_2_main_task(void);  /* TEST #2 - Testing the function _mem_alloc_system_align_from() */

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
* Test function: _mem_alloc_system_align.
* Requirements : MEM200, MEM201, MEM202, MEM203, MEM204
*                LWMEM200, LWMEM201, LWMEM202
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint               result;
    _mqx_uint               keep;
    _mem_pool_id            pool_error;
    unsigned char          *mptr;
#if !MQX_USE_LWMEM_ALLOCATOR
    unsigned char          *mptr_keep;
    STOREBLOCK_STRUCT_PTR block_ptr;
    MEMPOOL_STRUCT_PTR mem_pool_ptr;
#endif
    MEMORY_ALLOC_INFO_STRUCT info_ptr;

    /* Test the default memory pool */
    GENERAL_MEM_TEST_ALL(result, pool_error, block_error);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.1: _(lw)mem_test operation");

    /* Allocate memory from default pool */
    mptr = (unsigned char*)GENERAL_MEM_ALLOC_SYSTEM_ALIGN(TEST_ALLOC_SIZE, TEST_ALLOC_ALIGN);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mptr != NULL && MQX_OK == _task_get_error()), "Test #1 Testing: 1.2: _(lw)mem_alloc_system_align");

    /*free the allocaed memory*/
    _mem_free(mptr);

    /* use up all memory */
    memory_alloc_all(&info_ptr);
    _task_set_error(MQX_OK);
    /* try to allocate when there is no more memory */
    mptr = (unsigned char*)GENERAL_MEM_ALLOC_SYSTEM_ALIGN(TEST_ALLOC_SIZE, TEST_ALLOC_ALIGN);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mptr == NULL && MQX_OUT_OF_MEMORY == _task_get_error()), "Test #1 Testing: 1.3: _(lw)mem_alloc_system_align MQX_OUT_OF_MEMORY");

    _task_set_error(MQX_OK);
    /*  free all memory allocated before and test the memory again */
    memory_free_all(&info_ptr);

    /* disable the interrupt*/
    _int_disable();
    /* preserve the memory pool checksum */
    keep = get_pool_valid_field(SYSTEM_POOL_ID);
    /* modify the checksum */
    set_pool_valid_field(SYSTEM_POOL_ID,(_mqx_uint)TEST_INVALID_CHECKSUM);
    /* try to call _mem_alloc_system_align with bad checksum*/
    mptr = (unsigned char*)GENERAL_MEM_ALLOC_SYSTEM_ALIGN(TEST_ALLOC_SIZE, TEST_ALLOC_ALIGN);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mptr == NULL && GENERAL_MEM_INVALID_ERROR_CODE == _task_get_error()), "Test #1 Testing: 1.4: _(lw)mem_alloc_system_align GENERAL_MEM_INVALID_ERROR_CODE");

    /*  resume checksum */
    set_kernel_error_block(NULL);
    set_pool_valid_field(SYSTEM_POOL_ID,keep);
    /* enable the interrupt*/
    _int_enable();
    _task_set_error(MQX_OK);

#if !MQX_USE_LWMEM_ALLOCATOR
    /* disable the interrupt*/
    _int_disable();
    /* perserve the  free list ptr*/
    mem_pool_ptr = SYSTEM_POOL_ID;
    keep = (_mqx_uint)mem_pool_ptr->POOL_FREE_LIST_PTR;
    /* allocate a test range to get bad alignment pool*/
    mptr_keep = (unsigned char*)GENERAL_MEM_ALLOC_SYSTEM_ALIGN(2 * sizeof(STOREBLOCK_STRUCT), PSP_MEMORY_ALIGNMENT + 1);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mem_pool_ptr != NULL), "Test #1 Testing: 1.5: _(lw)mem_alloc_system_align normal");

    mem_pool_ptr->POOL_FREE_LIST_PTR  = (STOREBLOCK_STRUCT_PTR)(mptr_keep + PSP_MEMORY_ALIGNMENT);
    /* try to call _mem_alloc_system_align with bad aligment pool*/
    mptr = (unsigned char*)GENERAL_MEM_ALLOC_SYSTEM_ALIGN(TEST_ALLOC_SIZE, TEST_ALLOC_ALIGN);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mptr == NULL && MQX_CORRUPT_STORAGE_POOL_FREE_LIST == _task_get_error()), "Test #1 Testing: 1.6: _(lw)mem_alloc_system_align MQX_CORRUPT_STORAGE_POOL_FREE_LIST-1");

    /* resume pool list */
    mem_pool_ptr->POOL_FREE_LIST_PTR = (void *)keep;
    set_kernel_error_block(NULL);
    _task_set_error(MQX_OK);
    /*free the allocaed test memory*/
    _mem_free(mptr_keep);

    /* try to call _mem_alloc_system_align after set correct condition*/
    mptr = (unsigned char*)GENERAL_MEM_ALLOC_SYSTEM_ALIGN(TEST_ALLOC_SIZE, TEST_ALLOC_ALIGN);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mptr != NULL) && (MQX_OK == _task_get_error()), "Test #1 Testing: 1.7: _(lw)mem_alloc_system_align normal");

    /*free the allocaed memory*/
    _mem_free(mptr);

    /* fake the pool being used*/
    mem_pool_ptr = SYSTEM_POOL_ID;
    block_ptr = (STOREBLOCK_STRUCT_PTR)mem_pool_ptr->POOL_FREE_LIST_PTR;
    block_ptr->TASK_NUMBER = 1;
    /* try to call _mem_alloc_system_align after set correct condition*/
    mptr = (unsigned char*)GENERAL_MEM_ALLOC_SYSTEM_ALIGN(TEST_ALLOC_SIZE, TEST_ALLOC_ALIGN);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mptr == NULL && MQX_CORRUPT_STORAGE_POOL_FREE_LIST == _task_get_error()), "Test #1 Testing: 1.8: _(lw)mem_alloc_system_align MQX_CORRUPT_STORAGE_POOL_FREE_LIST-2");

    block_ptr->TASK_NUMBER = 0;
    /* enable the interrupt*/
    _int_enable();
    _task_set_error(MQX_OK);


    mptr = (unsigned char*)GENERAL_MEM_ALLOC_SYSTEM_ALIGN(TEST_ALLOC_SIZE, 3);
    result = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (mptr == NULL && MQX_INVALID_PARAMETER == result), "Test #1 Testing: 1.9: _(lw)mem_alloc_system_align MQX_INVALID_PARAMETER");
    _task_set_error(MQX_OK);
#endif
    /* Test the default memory pool */
    GENERAL_MEM_TEST_ALL(result, pool_error, block_error);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.10: _mem_test operation");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   Test function: _mem_alloc_system_align_from in normal case.
* Requirements : MEM205, MEM206, MEM207, MEM208, MEM209
*                LWMEM205, LWMEM206, LWMEM208
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _mqx_uint                   result;
    _mqx_uint                   keep;
    _mqx_int                    pool_size;
    _mem_pool_id                pool_error;
    _mem_pool_id                ext_pool_id;
    unsigned char               *mptr;
    void                       *pool_alloc_ptr;
#if !MQX_USE_LWMEM_ALLOCATOR
    unsigned char              *mptr_keep;
    STOREBLOCK_STRUCT_PTR       block_ptr;
    MEMPOOL_STRUCT_PTR          mem_pool_ptr;
    MEM_LIST_PTR                ptr, tptr;
#endif
    pool_size = get_free_mem() / 2;

    /* Create external memory pool */
    pool_alloc_ptr = _mem_alloc (pool_size);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, pool_alloc_ptr != NULL, "Test #2: Testing 2.1: Allocate memory for memory pool");

    result = _task_get_error();
    ext_pool_id = _mem_create_pool(pool_alloc_ptr, pool_size);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, ext_pool_id != NULL, "Test #2: Testing 2.2: Create external memory pool");

    /* Allocate memory from  pool */
    mptr = (unsigned char*)GENERAL_MEM_ALLOC_SYSTEM_ALIGN_FROM(ext_pool_id, TEST_ALLOC_SIZE, TEST_ALLOC_ALIGN);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (mptr != NULL) && (MQX_OK == _task_get_error()), "Test #2 Testing: 2.3: _(lw)mem_alloc_system_align");
    /*free the allocaed memory*/
    _mem_free(mptr);


    /* disable the interrupt*/
    _int_disable();
    /* preserve the memory pool checksum */
    keep = get_pool_valid_field(ext_pool_id);
    /* modify the checksum */
    set_pool_valid_field(ext_pool_id,(_mqx_uint)TEST_INVALID_CHECKSUM);
    /* try to call _mem_alloc_system_align with bad checksum*/
    mptr = (unsigned char*)GENERAL_MEM_ALLOC_SYSTEM_ALIGN_FROM(ext_pool_id, TEST_ALLOC_SIZE, TEST_ALLOC_ALIGN);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (mptr == NULL && GENERAL_MEM_INVALID_ERROR_CODE == _task_get_error()), "Test #2 Testing: 2.4: _(lw)mem_alloc_system_align_from GENERAL_MEM_INVALID_ERROR_CODE");

    /*  resume checksum */
    set_kernel_error_block(NULL);
    set_pool_valid_field(ext_pool_id,keep);
    /* enable the interrupt*/
    _int_enable();
    _task_set_error(MQX_OK);

#if !MQX_USE_LWMEM_ALLOCATOR
    /* disable the interrupt*/
    _int_disable();
    /* perserve the  free list ptr*/
    mem_pool_ptr = (MEMPOOL_STRUCT_PTR)ext_pool_id;
    keep = (_mqx_uint)mem_pool_ptr->POOL_FREE_LIST_PTR;
    /* allocate a test range to get bad alignment pool*/
    mptr_keep = (unsigned char*)GENERAL_MEM_ALLOC_SYSTEM_ALIGN(2 * sizeof(STOREBLOCK_STRUCT), PSP_MEMORY_ALIGNMENT + 1);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (mem_pool_ptr != NULL), "Test #2 Testing: 2.5: _(lw)mem_alloc_system_align normal");

    mem_pool_ptr->POOL_FREE_LIST_PTR  = (STOREBLOCK_STRUCT_PTR)(mptr_keep + PSP_MEMORY_ALIGNMENT);
    /* try to call _mem_alloc_system_align with bad aligment pool*/
    mptr = (unsigned char*)GENERAL_MEM_ALLOC_SYSTEM_ALIGN_FROM(ext_pool_id, TEST_ALLOC_SIZE, TEST_ALLOC_ALIGN);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (mptr == NULL && MQX_CORRUPT_STORAGE_POOL_FREE_LIST == _task_get_error()), "Test #2 Testing: 2.6: _(lw)mem_alloc_system_align_from MQX_CORRUPT_STORAGE_POOL_FREE_LIST-1");

    /* resume pool list */
    mem_pool_ptr->POOL_FREE_LIST_PTR = (void *)keep;
    mem_pool_ptr->POOL_BLOCK_IN_ERROR = NULL;
    _task_set_error(MQX_OK);
    /*free the allocaed test memory*/
    _mem_free(mptr_keep);

    /* try to call _mem_alloc_system_align after set correct condition*/
    mptr = (unsigned char*)GENERAL_MEM_ALLOC_SYSTEM_ALIGN_FROM(ext_pool_id, TEST_ALLOC_SIZE, TEST_ALLOC_ALIGN);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (mptr != NULL) && (MQX_OK == _task_get_error()), "Test #2 Testing: 2.7: _(lw)mem_alloc_system_align normal");

    /*free the allocaed memory*/
    _mem_free(mptr);

    /* fake the pool being used*/
    mem_pool_ptr = (MEMPOOL_STRUCT_PTR)ext_pool_id;
    block_ptr = (STOREBLOCK_STRUCT_PTR)mem_pool_ptr->POOL_FREE_LIST_PTR;
    block_ptr->TASK_NUMBER = 1;
    /* try to call _mem_alloc_system_align after set correct condition*/
    mptr = (unsigned char*)GENERAL_MEM_ALLOC_SYSTEM_ALIGN_FROM(ext_pool_id, TEST_ALLOC_SIZE, TEST_ALLOC_ALIGN);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (mptr == NULL && MQX_CORRUPT_STORAGE_POOL_FREE_LIST == _task_get_error()), "Test #2 Testing: 2.8: _(lw)mem_alloc_system_align_from MQX_CORRUPT_STORAGE_POOL_FREE_LIST-2");

    block_ptr->TASK_NUMBER = 0;
    /* enable the interrupt*/
    _int_enable();
    _task_set_error(MQX_OK);


    mptr = (unsigned char*)GENERAL_MEM_ALLOC_SYSTEM_ALIGN_FROM(ext_pool_id, TEST_ALLOC_SIZE, 3);
    result = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (mptr == NULL && MQX_INVALID_PARAMETER == result), "Test #2 Testing: 2.9: _(lw)mem_alloc_system_align_from MQX_INVALID_PARAMETER");
     _task_set_error(MQX_OK);

    /* allocate memory till no enough memory available */
    tptr = NULL;
    do {
      mptr = GENERAL_MEM_ALLOC_SYSTEM_ALIGN_FROM(ext_pool_id, get_free_mem() / 4, TEST_ALLOC_ALIGN);
      if (NULL != mptr)
      {
        ptr = (MEM_LIST_PTR)mptr;
        ptr->pdata = mptr + sizeof(MEM_LIST);
        ptr->prev = tptr;
        tptr = ptr;
      }
    }while(mptr != NULL);
    result = _task_get_error();
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (mptr == NULL && MQX_OUT_OF_MEMORY == result), "Test #2 Testing: 2.10: MQX_OUT_OF_MEMORY");

    result = test_memory(ptr, (get_free_mem() / 4 - sizeof(MEM_LIST)) / sizeof(_mqx_uint));
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == 0), "Test #2 Testing: 2.11: _mem_test operation ok");
    _task_set_error(MQX_OK);

    /* free test memory */
    while(NULL != ptr)
    {
        tptr = ptr->prev;
        _mem_free(ptr);
        ptr = tptr;
    }
#endif

    /* Test the default memory pool */
    GENERAL_MEM_TEST_ALL(result, pool_error, block_error);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.12: _mem_test operation");
}


/******************** End Testcases ********************/

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_mem_align_system)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Testing the function _mem_alloc_system_align()"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Testing the function _mem_alloc_system_align_from()"),
EU_TEST_SUITE_END(suite_mem_align_system)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_mem_align_system, " - Test of MQX memory functions")
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

void main_task( uint32_t dummy )
{
    /* Run test tasks */
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}

_mqx_uint test_memory(MEM_LIST_PTR ptr, _mqx_uint size)
{
  _mqx_uint i = 0;
  MEM_LIST_PTR tmp = ptr;
  while (NULL != tmp)
  {
    _mqx_uint *pdata = (_mqx_uint*)tmp->pdata;
    for (i = 0; i < size ; i++, pdata++)
      *pdata = 0xAAAAAAAA;
    tmp = tmp->prev;
  }
  tmp = ptr;
   while (NULL != tmp)
  {
    _mqx_uint *pdata = (_mqx_uint*)tmp->pdata;
    for (i = 0; i < size ; i++, pdata++)
    {
      if ( *pdata != 0xAAAAAAAA)
        return 1;
    }
    tmp = tmp->prev;
  }
  return 0;
}

