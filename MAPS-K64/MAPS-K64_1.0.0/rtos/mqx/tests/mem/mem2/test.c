/**HEADER********************************************************************
*
* Copyright (c) 2008-2013 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $Date    : May-10-2013$
*
* Comments:
*
*   This file contains code for the MQX Memory verification program.
*
* Requirements:
*
*   MEM001, MEM005, MEM013, MEM033, MEM041, MEM046, MEM051, MEM053, MEM063,
*   MEM065, MEM085, MEM086.
*
*   LWMEM001, LWMEM019, LWMEM011, LWMEM013, LWMEM015, LWMEM017, LWMEM020,
*   LWMEM027, LWMEM028, LWMEM029
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include "util.h"
#include "test.h"
#include "mem_util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

/*------------------------------------------------------------------------
 * Test suite function prototype
 */
void tc_1_main_task(void); /* Testing the function _(lw)mem_create_pool() */
void tc_2_main_task(void); /* Testing the function _(lw)mem_alloc_from() */
void tc_3_main_task(void); /* Testing the function _(lw)mem_alloc_system_from() */
void tc_4_main_task(void); /* Testing the function _(lw)mem_free() */
void tc_5_main_task(void); /* Testing the function _(lw)mem_alloc_zero_from() */
void tc_6_main_task(void); /* Testing the function _(lw)mem_alloc_system_zero_from() */
void tc_7_main_task(void); /* Testing the functions _mem_get_highwater_pool() and _mem_extend_pool() */
void tc_8_main_task(void); /* Testing the functions _(lw)mem_test_pool() and _mem_get_error_pool() */

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
*   Test function: _(lw)mem_create_pool in normal case.
* Requirements :
*   MEM001, MEM046, MEM085
*   LWMEM001, LWMEM019, LWMEM028
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint result;

    /* Create external memory pool */
    pool_alloc_ptr = _mem_alloc(MEM_EXT_POOL_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, pool_alloc_ptr != NULL, "Test #1: Testing 1.1: Allocate memory operation");

    ext_pool_id = GENERAL_MEM_CREATE_POOL(pool_alloc_ptr, MEM_EXT_POOL_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, ext_pool_id != 0, "Test #1: Testing 1.2: Create the memory pool");

    /* Test the external memory pool */
    result = GENERAL_MEM_TEST_POOL(ext_pool_id);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.3: Test the memory pool operation");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   Test function: _(lw)mem_alloc_from in normal case.
* Requirements :
*   MEM005, MEM085
*   LWMEM011, LWMEM028
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _mqx_uint result;

    /* Allocate memory from external pool */
    to_ptr = (unsigned char *)_mem_alloc_from(ext_pool_id, TEST_ALLOC_SIZE );
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, to_ptr != NULL, "Test #2: Testing 2.1: Allocate memory from the memory pool");

    /* Check allocated block's owner */
    block_ptr = GENERAL_MEM_GET_BLOCK_PTR(to_ptr);
    result = BUILD_TASKID(kernel_data->INIT.PROCESSOR_NUMBER, GENERAL_MEM_TASK_NUMBER(block_ptr));
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == global_main_task_id, "Test #2: Testing 2.2: Check main task own resource");

    /* Test the external memory pool */
    result = GENERAL_MEM_TEST_POOL(ext_pool_id);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2: Testing 2.3: Test the memory pool operation");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   Test function: _(lw)mem_alloc_system_from in normal case.
* Requirements :
*   MEM033, MEM085
*   LWMEM013, LWMEM028
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    _mqx_uint result;

    /* Allocate memory from external pool */
    from_ptr = (unsigned char *)_mem_alloc_system_from(ext_pool_id, TEST_ALLOC_SIZE );
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, from_ptr != NULL, "Test #3: Testing 3.1: Allocate system memory block from memory pool");

    /* Check allocated block's owner */
    block_ptr = GENERAL_MEM_GET_BLOCK_PTR(from_ptr);
    result = BUILD_TASKID(kernel_data->INIT.PROCESSOR_NUMBER, GENERAL_MEM_TASK_NUMBER(block_ptr));
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == system_task_id, "Test #3: Testing 3.2: Check system task own memory block operation");

    /* Test the external memory pool */
    result = GENERAL_MEM_TEST_POOL(ext_pool_id);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "Test #3: Testing 3.3: Test the memory pool");
}
/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
    Test function: _(lw)_lwmem_free in normal case.
* Requirements :
*   MEM033, MEM053, MEM085
*   LWMEM013, LWMEM020, LWMEM027, LWMEM028
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    unsigned char      *r1_ptr;
    unsigned char      *r2_ptr;
    unsigned char      *r3_ptr;
    unsigned char      *r4_ptr;
    unsigned char      *r5_ptr;
    uint32_t     to_address;
    uint32_t     from_address;
    _mqx_uint   result;

    /* Free the allocated blocks of previous task */
    result = _mem_free( to_ptr );
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.1: mem free operation");
    if (result != MQX_OK)
    {
        _task_block();
    } /* Endif */
    result = _mem_free( from_ptr );
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.2: mem free operation");

    /* Test the external memory pool */
    result = GENERAL_MEM_TEST_POOL(ext_pool_id);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.3: mem test operation");

    /* Check co-allescing */
    from_ptr = (unsigned char *)_mem_alloc_system_from(ext_pool_id, TEST_ALLOC_SIZE );
    to_ptr = (unsigned char *)_mem_alloc_system_from(ext_pool_id, TEST_ALLOC_SIZE );
    r1_ptr = (unsigned char *)_mem_alloc_system_from(ext_pool_id, TEST_ALLOC_SIZE );
    r2_ptr = (unsigned char *)_mem_alloc_system_from(ext_pool_id, TEST_ALLOC_SIZE );
    r3_ptr = (unsigned char *)_mem_alloc_system_from(ext_pool_id, TEST_ALLOC_SIZE );
    r4_ptr = (unsigned char *)_mem_alloc_system_from(ext_pool_id, TEST_ALLOC_SIZE );
    r5_ptr = (unsigned char *)_mem_alloc_system_from(ext_pool_id, TEST_ALLOC_SIZE );
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (from_ptr && to_ptr && r1_ptr && r2_ptr && r3_ptr && r4_ptr), "Test #4: Testing 4.4: _(lw)mem_alloc_system_from operation");
    if (!from_ptr || !to_ptr || !r1_ptr || !r2_ptr || !r3_ptr || !r4_ptr)
    {
        _task_block();
    } /* Endif */

    from_address = (uint32_t)from_ptr;

    /* Free the allocated blocks */
    result = _mem_free(from_ptr);
    result |= _mem_free(r1_ptr);
    result |= _mem_free(r3_ptr);
    result |= _mem_free(r2_ptr);
    result |= _mem_free(r4_ptr);
    result |= _mem_free(to_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4: Testing 4.5: mem free operation");
    if (result != MQX_OK) {
        _task_block();
    } /* Endif */

    /* Verify coallescing operation */
    to_ptr = (unsigned char *)_mem_alloc_system_from(ext_pool_id, (TEST_ALLOC_SIZE * 6) + (4 * GENERAL_SIZE_OF_BLOCK_STRUCT));

    to_address = (uint32_t)to_ptr;

    EU_ASSERT_REF_TC_MSG(tc_4_main_task, to_address == from_address, "Test #4: Testing 4.6: _(lw)mem_free() coallescing operation");
    if (to_address != from_address) {
        _task_block();
    } /* Endif */

    /* Free the allocated blocks */
    result = _mem_free(r5_ptr);
    result = _mem_free(to_ptr);

#if MQX_USE_LWMEM_ALLOCATOR
    _lwmem_set_default_pool(ext_pool_id);
#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5:
*   Test function: _(lw)mem_alloc_zero_from in normal case.
* Requirements :
*   MEM013, MEM053, MEM085
*   LWMEM017, LWMEM020, LWMEM028
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    _mqx_uint   result;
    _mqx_uint   i;

    /* Allocate memory from external pool */
    to_ptr = (unsigned char *)_mem_alloc_zero_from(ext_pool_id, TEST_ALLOC_SIZE );
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, to_ptr != NULL, "Test #5: Testing 5.1: Allocate memory operation");

    /* Check to see if all allocated bytes are zero */
    for( i = 0; i < TEST_ALLOC_SIZE; i++ )
    {
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, to_ptr[i] == 0, "Test #5: Testing 5.2: Check non-zero value operation");
    } /* Endfor */

    /* Free the allocated block */
    result = _mem_free( to_ptr );
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.3: Free memory operation");

    /* Test the external memory pool */
    result = GENERAL_MEM_TEST_POOL(ext_pool_id);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5: Testing 5.4: Test memory pool operation");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6:
*   Test function: _(lw)mem_alloc_system_zero_from in normal case.
* Requirements :
*   MEM041, MEM053, MEM085
*   LWMEM015, LWMEM020, LWMEM028
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    _mqx_uint    result;
    _mqx_uint    i;

    /* Allocate memory from external pool */
    to_ptr = (unsigned char *)_mem_alloc_system_zero_from(ext_pool_id, TEST_ALLOC_SIZE );
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, to_ptr != NULL, "Test #6: Testing 6.1: _(lw)mem_alloc_system_zero_from operation ");

    /* Check to see if all allocated bytes are zero */
    for( i = 0; i < TEST_ALLOC_SIZE; i++ )
    {
        EU_ASSERT_REF_TC_MSG(tc_6_main_task, to_ptr[i] == 0, "Test #6: Testing 6.2: Check all allocated bytes are zero");
        if ( to_ptr[i] != 0 )
        {
            break;
        } /* Endif */
    } /* Endfor */

    EU_ASSERT_REF_TC_MSG(tc_6_main_task, i == TEST_ALLOC_SIZE, "Test #6: Testing 6.3: All bytes allocated should be zero");

    /* Free the allocated block */
    result = _mem_free( to_ptr );
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.4: Free memory operation");

    /* Test the external memory pool */
    result = GENERAL_MEM_TEST_POOL(ext_pool_id);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6: Testing 6.5: Test memory pool operation");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7:
*   Test function: _mem_get_highwater_pool and _mem_extend_pool in normal case.
* Requirements :
*   MEM005, MEM051, MEM053, MEM065, MEM085
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
#if MQX_USE_LWMEM_ALLOCATOR
   EU_ASSERT_REF_TC_MSG(tc_7_main_task, TRUE, "Test #7: Testing 7.1: ")
#else
    MEM_POINTER_LIST_STRUCT_PTR head_ptr;
    MEM_POINTER_LIST_STRUCT_PTR temp_ptr;
    unsigned char                      *r1_ptr;
    unsigned char                      *r2_ptr;
    _mqx_uint                   result;
    _mqx_uint                   k;

    /* Get highwater before allocating */
    r1_ptr = _mem_get_highwater_pool(ext_pool_id);

    /* Allocate memory from external pool until full */
    to_ptr = (unsigned char *)_mem_alloc_from(ext_pool_id, sizeof(MEM_POINTER_LIST_STRUCT));
    head_ptr = (MEM_POINTER_LIST_STRUCT_PTR)to_ptr;
    head_ptr->PREV = NULL;
    while( TRUE )
    {
        to_ptr = (unsigned char *)_mem_alloc_from(ext_pool_id, TEST_ALLOC_SIZE );
        if ( to_ptr == NULL )
        {
            break;
        } /* Endif */
        temp_ptr = head_ptr;
        head_ptr = (MEM_POINTER_LIST_STRUCT_PTR)to_ptr;
        head_ptr->PREV = temp_ptr;
    } /* Endwhile */

    /* Get highwater after allocating */
    r2_ptr = _mem_get_highwater_pool(ext_pool_id);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, r2_ptr > r1_ptr, "Test #7: Testing 7.1: Compare high water mask with previous high water mask");

    /* Extend the external pool size */
    result = _mem_extend_pool(ext_pool_id, global_memory_extension_buffer, MEM_EXT_SIZE );
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.2: _mem_extend_pool operation");

    /* Allocate more memory from pool */
    k = 0;
    while( TRUE )
    {
        to_ptr = (unsigned char *)_mem_alloc_from(ext_pool_id, TEST_ALLOC_SIZE );
        if ( to_ptr == NULL )
        {
             break;
        } /* Endif */
        k++;
        temp_ptr = head_ptr;
        head_ptr = (MEM_POINTER_LIST_STRUCT_PTR)to_ptr;
        head_ptr->PREV = temp_ptr;
    } /* Endwhile */
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, k != 0, "Test #7: Testing 7.3: Check allocation memory");

    /* Free all allocated memory blocks */
    while ( head_ptr )
    {
        temp_ptr = head_ptr->PREV;
        result = _mem_free( head_ptr );
        EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.4: Free memory operation");
        if (result != MQX_OK )
        {
            break;
        } /* Endif */
        head_ptr = temp_ptr;
    } /* Endwhile */

    /* Test the external memory pool */
    result = _mem_test_pool(ext_pool_id);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7: Testing 7.5: Test memory pool operation");
#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8: Test functions:
*   Test function: _(lw)mem_test_pool and _mem_get_error_pool in normal case.
* Requirements :
*   MEM005, MEM063, MEM085, MEM086
*   LWMEM011, LWMEM028, LWMEM029
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
#if !MQX_USE_LWMEM_ALLOCATOR
    unsigned char       *r1_ptr;
#endif
    _mqx_uint    result, i;

    /* Test memory pool operation #1 */
    to_ptr = (unsigned char *)_mem_alloc_from(ext_pool_id, TEST_ALLOC_SIZE );
    while( ((_mem_size)to_ptr >= (_mem_size)((unsigned char *)pool_alloc_ptr + MEM_EXT_POOL_SIZE)) ||
        ((_mem_size)to_ptr <= (_mem_size)pool_alloc_ptr) )
    {
        to_ptr = (unsigned char *)_mem_alloc_from(ext_pool_id, TEST_ALLOC_SIZE );
    } /* Endwhile */
    from_ptr = (unsigned char *)_mem_alloc_from(ext_pool_id, TEST_ALLOC_SIZE );
    result = GENERAL_MEM_TEST_POOL(ext_pool_id);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.1: Test memory pool operation");

    /* Test memory pool operation #2 */
    for ( i = 0; i < TEST_ALLOC_SIZE; i++ )
    {
        to_ptr[i] = (unsigned char)i;
    } /* Endfor */
    result = GENERAL_MEM_TEST_POOL(ext_pool_id);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8: Testing 8.2: Test memory pool operation");

    /* Test memory pool operation #3 */
    for ( i = TEST_ALLOC_SIZE; i < (TEST_ALLOC_SIZE * 2); i++ )
    {
        to_ptr[i] = (unsigned char)i;
    } /* Endfor */
    result = GENERAL_MEM_TEST_POOL(ext_pool_id);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result != MQX_OK, "Test #8: Testing 8.3: Test memory pool operation");

#if !MQX_USE_LWMEM_ALLOCATOR
    r1_ptr = (unsigned char *)_mem_get_error_pool(ext_pool_id);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, r1_ptr == from_ptr, "Test #8: Testing 8.4: _mem_extend_pool operation");
#endif
}

/******************** End Testcases ********************/

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_mem2)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Testing the function _(lw)mem_create_pool()"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Testing the function _(lw)mem_alloc_from()"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST #3 - Testing the function _(lw)mem_alloc_system_from()"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST #4 - Testing the function _(lw)mem_free()"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST #5 - Testing the function _(lw)mem_alloc_zero_from()"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST #6 - Testing the function _(lw)mem_alloc_system_zero_from()"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST #7 - Testing the functions _mem_get_highwater_pool() and _mem_extend_pool()"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST #8 - Testing the functions _(lw)mem_test_pool() and _mem_get_error_pool()")
EU_TEST_SUITE_END(suite_mem2)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_mem2, " - Test of additional memory management functions")
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
    TD_STRUCT_PTR td_ptr;

    /* Get current kernel data */
    kernel_data = (KERNEL_DATA_STRUCT_PTR)_mqx_get_kernel_data();
    /*Get main task ID */
    global_main_task_id = _task_get_id();
    /* Get system task ID */
    td_ptr = SYSTEM_TD_PTR(kernel_data);
    system_task_id = td_ptr->TASK_ID;

    /* Run test tasks */
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}
