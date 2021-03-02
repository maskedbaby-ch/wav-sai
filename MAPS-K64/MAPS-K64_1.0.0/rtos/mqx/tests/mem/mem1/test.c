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
*   MEM001, MEM009, MEM037, MEM045, MEM049, MEM053, MEM057, MEM062, MEM064
*   MEM066, MEM072, MEM073, MEM074, MEM090, MEM095
*
*   LWMEM001, LWMEM003, LWMEM005, LWMEM007, LWMEM020, LWMEM025, LWMEM028, LWMEM029
*   LWMEM032
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
void tc_1_main_task(void);  /* TEST #1 - Testing the function _(lw)mem_alloc() */
void tc_2_main_task(void);  /* TEST #2 - Testing the function _(lw)mem_alloc_system() */
void tc_3_main_task(void);  /* TEST #3 - Testing the function _(lw)mem_free() */
void tc_4_main_task(void);  /* TEST #4 - Testing the function _(lw)mem_alloc_zero() */
void tc_5_main_task(void);  /* TEST #5 - Testing the function _(lw)mem_alloc_system_zero() */
void tc_6_main_task(void);  /* TEST #6 - Testing the function _(lw)mem_copy() */
void tc_7_main_task(void);  /* TEST #7 - Testing the functions _mem_free_part() and _(lw)mem_get_size() */
void tc_8_main_task(void);  /* TEST #8 - Testing the function _mem_swap_endian() */
void tc_9_main_task(void);  /* TEST #9 - Testing the function _(lw)mem_transfer() */
void tc_10_main_task(void); /* TEST #10 - Testing the function _mem_zero() */
void tc_11_main_task(void); /* TEST #11 - Testing the functions _mem_get_highwater() and _mem_extend() */
void tc_12_main_task(void); /* TEST #12 - Testing the functions _mem_get_error() and _mem_test() */

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
    { TEST_TASK,      test_task,      3000,  9, "test",                      0},
    {         0,              0,         0,  0,      0,                      0}
};

/******************** Begin Test cases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*   Test function: _(lw)mem_alloc in normal case.
* Requirements :
*   MEM001, MEM073
*   LWMEM001, LWMEM028
*
*END*---------------------------------------------------------------------*/

void tc_1_main_task(void)
{
    _mqx_uint result;

    /* Test the default memory pool */
    result = GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.1: _(lw)mem_test operation");

    /* Allocate memory from default pool */
    to_ptr = (unsigned char *)_mem_alloc(TEST_ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (to_ptr != NULL), "Test #1 Testing: 1.2: _(lw)mem_alloc operation");

    /* Check allocated block's owner */
    block_ptr = GENERAL_MEM_GET_BLOCK_PTR(to_ptr);
    result = BUILD_TASKID(kernel_data->INIT.PROCESSOR_NUMBER, GENERAL_MEM_TASK_NUMBER(block_ptr));
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == global_main_task_id, "Test #1 Testing: 1.3: _(lw)mem_alloc operation");

    /* Test the default memory pool */
    result = GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, (result == MQX_OK), "Test #1 Testing: 1.4: _(lw)mem_test operation");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   Test function: _(lw)mem_alloc_system in normal case.
* Requirements :
*   MEM029, MEM073
*   LWMEM003, LWMEM028
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _mqx_uint result;

    /* Allocate memory from default pool */
    from_ptr = (unsigned char *)_mem_alloc_system(TEST_ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (from_ptr != NULL), "Test #2 Testing: 2.1: _(lw)mem_alloc_system operation");

    /* Check allocated block's owner */
    block_ptr = GENERAL_MEM_GET_BLOCK_PTR(from_ptr);
    result = BUILD_TASKID(kernel_data->INIT.PROCESSOR_NUMBER, GENERAL_MEM_TASK_NUMBER(block_ptr));
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == system_task_id), "Test #2 Testing: 2.2: _(lw)mem_alloc_system operation");

    /* Test the default memory pool */
    result = GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, (result == MQX_OK), "Test #2 Testing: 2.3: _(lw)mem_test operation");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   Test function: _(lw)mem_free in normal case.
* Requirements :
*   MEM053, MEM073
*   LWMEM020, LWMEM028
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    _mqx_uint result;

    /* Free the allocated memory blocks of previous task */
    result = _mem_free(to_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing: 3.1: _(lw)mem_free operation");
    result = _mem_free(from_ptr);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing: 3.2: _(lw)mem_free operation");

    /* Test the default memory pool */
    result = GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, (result == MQX_OK), "Test #3 Testing: 3.3: _(lw)mem_test operation");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : TEST #4:
*   Test function: _(lw)mem_alloc_zero in normal case.
* Requirements :
*   MEM009, MEM073
*   LWMEM007, LWMEM028
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    _mqx_uint result;
    _mqx_uint i;

    /* Allocate memory from default pool */
    to_ptr = (unsigned char *)_mem_alloc_zero(TEST_ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (to_ptr != NULL), "Test #4 Testing: 4.1: _(lw)mem_alloc_zero operation");

    /* Check to see if all allocated bytes are zero */
    for(i = 0; i < TEST_ALLOC_SIZE; i++)
    {
        EU_ASSERT_REF_TC_MSG(tc_4_main_task, (to_ptr[i] == 0), "Test #4 Testing: 4.2: _(lw)mem_alloc_zero operation");
    }

    /* Free the allocated memory block */
    result = _mem_free(to_ptr);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: 4.3: _(lw)mem_free operation");

    /* Test the default memory pool */
    result = GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, (result == MQX_OK), "Test #4 Testing: 4.4: _(lw)mem_test operation");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : TEST #5:
*   Test function: _(lw)mem_alloc_system_zero in normal case.
* Requirements :
*   MEM037, MEM073
*   LWMEM005, LWMEM028
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    _mqx_uint result;
    _mqx_uint i;

    /* Allocate memory from default pool */
    to_ptr = (unsigned char *)_mem_alloc_system_zero(TEST_ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (to_ptr != NULL), "Test #5 Testing: 5.1: _(lw)mem_alloc_system_zero operation");

    /* Check to see if all allocated bytes are zero */
    for(i = 0; i < TEST_ALLOC_SIZE; i++)
    {
        EU_ASSERT_REF_TC_MSG(tc_5_main_task, (to_ptr[i] == 0), "Test #5 Testing: 5.2: _(lw)mem_alloc_system_zero operation");
    }

    /* Free the allocated memory block */
    result = _mem_free(to_ptr);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.3: _(lw)mem_free operation");

    /* Test the default memory pool */
    result = GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, (result == MQX_OK), "Test #5 Testing: 5.4: _(lw)mem_test operation");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : TEST #6:
*   Test function: _(lw)mem_copy in normal case.
* Requirements :
*   MEM045, MEM073
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    _mqx_uint result;
    _mqx_uint i;
    _mqx_uint n = ALLOC_SIZE + 1;

    from_ptr = &From[0];
    to_ptr   = &To[0];

    /* Set ALLOC area */
    for (i = 0; i < ALLOC_SIZE; ++i)
    {
        *from_ptr++ = i;
        *to_ptr++   = 0;
    }

    /* Set FALLOUT area */
    to_ptr = &To[ALLOC_SIZE];
    for (i = ALLOC_SIZE; i < TOTAL_SIZE; ++i)
    {
        *to_ptr++ = 0xE5;
    }

    /* Test the copy command */
    _mem_copy(&From[0], &To[0], ALLOC_SIZE);

    /* Verify ALLOC_SIZE of data after copying */
    i = From[0];
    to_ptr = &To[0];
    while (--n)
    {
        if (*to_ptr != (unsigned char)i)
        {
            break;
        }
        to_ptr++;
        i++;
    }
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, n == 0, "Test #6 Testing: 6.1: _mem_copy operation");

    /* Verify FALLOUT_SIZE of data after copying */
    to_ptr = &To[ALLOC_SIZE];
    for (i = 0; i < FALLOUT_SIZE; i++)
    {
        if (*to_ptr++ != 0xE5)
        {
            break;
        }
    }
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, i == FALLOUT_SIZE, "Test #6 Testing: 6.2: _mem_copy operation");

    /* Test the default pool */
    result = GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, (result == MQX_OK), "Test #6 Testing: 6.3: _mem_test operation");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : TEST #7:
*   Test function: _mem_free_part and _(lw)mem_get_size in normal case.
* Requirements :
*   MEM001, MEM057, MEM066, MEM073
*   LWMEM001, LWMEM025, LWMEM028
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    _mqx_uint result;
    _mqx_uint i, j, alloc_size;

    alloc_size = TEST_ALLOC_SIZE + 2 * GENERAL_MEM_MIN_STORAGE_SIZE;

    /* Allocate a memory block */
    to_ptr = (unsigned char*)_mem_alloc(alloc_size);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (to_ptr != NULL), "Test #7 Testing: 7.1: _(lw)mem_alloc operation");

    /* Get mem size before partial freeing */
    i = GENERAL_MEM_GET_SIZE(to_ptr);
    j = alloc_size + 2 * GENERAL_MEM_MIN_STORAGE_SIZE;
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, ((i >= alloc_size) && (i <= j)), "Test #7 Testing: 7.2: _(lw)mem_get_size operation");

#if !MQX_USE_LWMEM_ALLOCATOR
    /* Free memory partially */
    result = _mem_free_part(to_ptr, alloc_size / 2);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: 7.3: _mem_free_part operation");

    /* Get mem size after partial freeing */
    i = GENERAL_MEM_GET_SIZE(to_ptr);
    j = alloc_size / 2 + 2 * GENERAL_MEM_MIN_STORAGE_SIZE;
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, ((i >= (alloc_size/2)) && (i <= j)), "Test #7 Testing: 7.4: _mem_free_part operation");
#endif

    /* Free the allocated memory block */
    result = _mem_free(to_ptr);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: 7.5: _(lw)mem_free operation");

    /* Test the default memory pool */
    result = GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, (result == MQX_OK), "Test #7 Testing: 7.6: _(lw)mem_test operation");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : TEST #8:
*   Test function: _mem_swap_endian in normal case.
* Requirements :
*   MEM072, MEM073
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
#if MQX_USE_LWMEM_ALLOCATOR
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, TRUE, "Test #8 Testing: 8.01: ");
#else
    _mqx_uint result;
    _mqx_uint k, c, i, j;

    /* Initialize data */
    c = 'A';
    for (i = 0; i < TEST_ENDIAN_ARRAY_SIZE; i++)
    {
        if (c > 'Z')
        {
            c = 'A';
        } /* Endif */
        global_endian_test_array[i] = c;
        global_endian_test_array_copy[i] = c;
        c++;
    } /* Endfor */

    /* Initialize defintion */
    for (i = 0; i < TEST_ENDIAN_MAX_OBJ_SIZE; i++)
    {
        global_endian_swap_def[i] = i+1;
    } /* Endfor */
    global_endian_swap_def[TEST_ENDIAN_MAX_OBJ_SIZE] = 0;

    /* Start testing various offsets */
    result = TRUE;
    for (i = 0; (i <= TEST_ENDIAN_MAX_OBJ_SIZE && result); i++)
    {
        _mem_swap_endian(global_endian_swap_def, &global_endian_test_array[i]);
        k = i;
        for (j = 0; (j < TEST_ENDIAN_MAX_OBJ_SIZE) && result; j++)
        {
            result = test_endian(&global_endian_test_array[k],
                &global_endian_test_array_copy[k],
                global_endian_swap_def[j]);
            k = k + global_endian_swap_def[j];
        } /* Endfor */

        /* unswap */
        _mem_copy(global_endian_test_array_copy, global_endian_test_array,
            TEST_ENDIAN_ARRAY_SIZE);
    } /* Endfor */
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result != 0), "Test #8 Testing: 8.3: _mem_swap_endian operation");

    /* Test the default memory pool */
    result = _mem_test();
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, (result == MQX_OK), "Test #8 Testing: 8.4: _mem_test operation");
#endif
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : TEST #9:
*   Test function: _(lw)mem_transfer in normal case.
* Requirements :
*   MEM001, MEM053, MEM073, MEM090
*   LWMEM001, LWMEM020, LWMEM028, LWMEM032
*
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
    _mqx_uint result;

    /* This task will allocate memory and then transfer it to us */
    global_test_task_id = _task_create(0, TEST_TASK, 0);

    /* Let the test task run */
    _sched_yield();

    /* Check the received memory block */
    block_ptr = GENERAL_MEM_GET_BLOCK_PTR(global_mem_ptr);
    result = (BUILD_TASKID(kernel_data->INIT.PROCESSOR_NUMBER, GENERAL_MEM_TASK_NUMBER(block_ptr)) == global_main_task_id);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == TRUE), "Test #9 Testing: 9.3: create test_task operation");

    /* Free the received memory block */
    result = _mem_free(global_mem_ptr);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: 9.4: free resource operation");

    /* Test the default memory pool */
    result = GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: 9.5: _mem_test operation");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_10_main_task
* Comments     : TEST #10:
*   Test function: _mem_zero in normal case.
* Requirements :
*   MEM073, MEM095
*
*END*---------------------------------------------------------------------*/
void tc_10_main_task(void)
{
    _mqx_uint result;
    _mqx_uint i;
    _mqx_uint n = ALLOC_SIZE + 1;

    to_ptr = &To[0];

    /* Set ALLOC area */
    for (i = 0; i < ALLOC_SIZE; ++i)
    {
        *to_ptr++ = i;
    }

    /* Set FALLOUT area */
    to_ptr = &To[ALLOC_SIZE];
    for (i = ALLOC_SIZE; i < TOTAL_SIZE; ++i)
    {
        *to_ptr++ = 0xE5;
    }

    /* Test the mem zero command */
    _mem_zero(&To[0], ALLOC_SIZE);

    /* Verify ALLOC_SIZE of data after clearing */
    to_ptr = &To[0];
    while (--n)
    {
        if (*to_ptr != (unsigned char)0)
        {
            break;
        }
        to_ptr++;
    }
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, n == 0, "Test #10 Testing: 10.1: _mem_zero operation");

    /* Verify FALLOUT_SIZE of data after clearing */
    to_ptr = &To[ALLOC_SIZE];
    for (i = 0; i < FALLOUT_SIZE; i++)
    {
        if (*to_ptr++ != 0xE5)
        {
            break;
        }
    }
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, i == FALLOUT_SIZE, "Test #10 Testing: 10.2: _mem_zero operation");

    /* Test the default pool */
    result = GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, (result == MQX_OK), "Test #10 Testing: 10.3: _mem_test operation");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_11_main_task
* Comments     : TEST #11:
*   Test function: _mem_get_highwater and _mem_extend in normal case.
* Requirements :
*   MEM001, MEM049, MEM053, MEM064, MEM073
*
*END*---------------------------------------------------------------------*/
void tc_11_main_task(void)
{
    MEM_POINTER_LIST_STRUCT_PTR head_ptr;
    MEM_POINTER_LIST_STRUCT_PTR temp_ptr;
    unsigned char                      *r1_ptr;
    unsigned char                      *r2_ptr;
    _mqx_uint                   result;
    uint32_t                     k;

    /* Get highwater before allocating */
    r1_ptr = _mem_get_highwater();

    /* Allocate memory from default pool until full */
    to_ptr = (unsigned char *)_mem_alloc(sizeof(MEM_POINTER_LIST_STRUCT));
    head_ptr = (MEM_POINTER_LIST_STRUCT_PTR)to_ptr;
    head_ptr->PREV = NULL;
    while(TRUE)
    {
        to_ptr = (unsigned char *)_mem_alloc(TEST_ALLOC_SIZE);
        if (to_ptr == NULL)
        {
            break;
        } /* Endif */
        temp_ptr = head_ptr;
        head_ptr = (MEM_POINTER_LIST_STRUCT_PTR)to_ptr;
        head_ptr->PREV = temp_ptr;
    } /* Endwhile */

    /* Get highwater after allocating */
    r2_ptr = _mem_get_highwater();
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (r2_ptr > r1_ptr), "Test #11 Testing: 11.1: _mem_get_highwater operation");

#if !MQX_USE_LWMEM_ALLOCATOR
    /* Extend the default pool size */
    result = _mem_extend(global_memory_extension_buffer, MEM_EXT_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.2: _mem_extend operation");

    /* Allocate more memory on pool */
    k = 0;
    while(TRUE)
    {
        to_ptr = (unsigned char *)_mem_alloc(TEST_ALLOC_SIZE);
        if (to_ptr == NULL)
        {
            break;
        } /* Endif */
        k++;
        temp_ptr = head_ptr;
        head_ptr = (MEM_POINTER_LIST_STRUCT_PTR)to_ptr;
        head_ptr->PREV = temp_ptr;
    } /* Endwhile */
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (k != 0), "Test #11 Testing: 11.3: _mem_extend operation");

#endif

    k = 0;
    /* Free all allocated memory blocks */
    while (head_ptr)
    {
        temp_ptr = head_ptr->PREV;
        result = _mem_free(head_ptr);
        head_ptr = temp_ptr;
        if (result != MQX_OK) k++;
    } /* Endwhile */

    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (k == 0), "Test #11 Testing: 11.4: _mem_free operation");

    /* Test the external memory pool */
    result = GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, (result == MQX_OK), "Test #11 Testing: 11.5: _mem_test operation");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_12_main_task
* Comments     : TEST #12:
*   Test function: _(lw)mem_test and _mem_get_error in normal case.
* Requirements :
*   MEM001, MEM062, MEM073, MEM074
*   LWMEM001, LWMEM028, LWMEM029
*
*END*---------------------------------------------------------------------*/
void tc_12_main_task(void)
{
#if !MQX_USE_LWMEM_ALLOCATOR
    unsigned char      *r1_ptr;
#endif
    _mqx_uint   alloc_overhead;
    _mqx_uint   result;
    _mqx_uint   i;

    /* Test memory pool operation #1 */
    to_ptr = (unsigned char *)_mem_alloc(TEST_ALLOC_SIZE);
    while(((_mem_size)to_ptr >= (_mem_size)kernel_data->INIT.END_OF_KERNEL_MEMORY) ||
        ((_mem_size)to_ptr <= (_mem_size)kernel_data->INIT.START_OF_KERNEL_MEMORY))
    {
        to_ptr = (unsigned char *)_mem_alloc(TEST_ALLOC_SIZE);
    } /* Endwhile */
#if MQX_USE_LWMEM_ALLOCATOR
    alloc_overhead = (_mem_size) sizeof(LWMEM_BLOCK_STRUCT);
#else
    alloc_overhead = (_mem_size)(FIELD_OFFSET(STOREBLOCK_STRUCT,USER_AREA));
#endif
    _MEMORY_ALIGN_VAL_LARGER(alloc_overhead);
    result = GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, (result == MQX_OK), "Test #12 Testing: 12.1: _mem_test operation");

    /* Test memory pool operation #2 */
    for (i = 0; i < TEST_ALLOC_SIZE; i++) {
      to_ptr[i] = (unsigned char)i;
    } /* Endfor */
    result = GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, (result == MQX_OK), "Test #12 Testing: 12.2: _mem_test operation");

    /* Test memory pool operation #3 */
    for (i = 0; i < alloc_overhead; i++) {
        to_ptr[-i] = (unsigned char)i;
    } /* Endfor */
    result = GENERAL_MEM_TEST;
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, (result != MQX_OK), "Test #12 Testing: 12.3: _mem_test should be failed");

#if !MQX_USE_LWMEM_ALLOCATOR
    r1_ptr = (unsigned char *)_mem_get_error();
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, (r1_ptr == to_ptr), "Test #12 Testing: 12.4: _mem_get_error operation");
#endif
}

/******************** End Testcases ********************/

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_mem1)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Testing the function _(lw)mem_alloc()"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST #2 - Testing the function _(lw)mem_alloc_system()"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST #3 - Testing the function _(lw)mem_free()"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST #4 - Testing the function _(lw)mem_alloc_zero()"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST #5 - Testing the function _(lw)mem_alloc_system_zero()"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST #6 - Testing the function _mem_copy()"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST #7 - Testing the functions _mem_free_part() and _mem_get_size()"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST #8 - Testing the function _mem_swap_endian()"),
    EU_TEST_CASE_ADD(tc_9_main_task, " TEST #9 - Testing the function _(lw)mem_transfer()"),
    EU_TEST_CASE_ADD(tc_10_main_task, " TEST #10 - Testing the function _mem_zero()"),
    EU_TEST_CASE_ADD(tc_11_main_task, " TEST #11 - Testing the functions _mem_get_highwater() and _mem_extend()"),
    EU_TEST_CASE_ADD(tc_12_main_task, " TEST #12 - Testing the functions _mem_get_error() and _mem_test()")
EU_TEST_SUITE_END(suite_mem1)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_mem1, " - Test of MQX memory functions")
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

/*TASK*-------------------------------------------------------------------
*
* Task Name    : test_task
* Comments     :
*   This task is to allocate a private memory segment and block itself
*   before destroying. The memory segment will be used to test transferring
*   memory.
*
*END*----------------------------------------------------------------------*/
void test_task( uint32_t dummy )
{
    _mqx_uint result;

    /* Allocate memory */
    global_mem_ptr = _mem_alloc(TEST_ALLOC_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (global_mem_ptr != NULL), "Test #9 Testing: 9.1: _mem_alloc operation");

    /* Transfer to main task */
    result = _mem_transfer(global_mem_ptr, global_test_task_id, global_main_task_id);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, (result == MQX_OK), "Test #9 Testing: 9.2: _mem_transfer operation");

    /* Block itself */
    _task_block();
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : test_endian
* Returned Value   : bool
* Comments         :
*   Compare same memory with different endian type
*   - [IN] swapped_ptr: Little endian array
    - [IN] copy_ptr: Big endian array
    - [IN] size: Array size
*
*END*--------------------------------------------------------------------*/
bool test_endian(unsigned char *swapped_ptr, unsigned char *copy_ptr, uint32_t size)
{
    uint32_t i;

    for (i = 0; i < size; i++)
    {
        if (swapped_ptr[i] != copy_ptr[size-1-i])
        {
           return(FALSE);
        }
    }

   return(TRUE);
}
