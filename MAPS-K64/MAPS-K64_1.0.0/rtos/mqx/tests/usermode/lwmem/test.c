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
* $Version : 4.0.2.0$
* $Date    : Jun-14-2013$
*
* Comments:
*
*   This file contains code for the MQX Usermode verification program,
*   includes all remaining test cases, which haven't been verified yet,
*   of Lightweight Memory functions in usermode.
*
* Requirements:
*
*   USERMODE024, USERMODE025, USERMODE026, USERMODE027, USERMODE028,
*   USERMODE029, USERMODE031, USERMODE032, USERMODE033.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <lwmem.h>
#include <lwmem_prv.h>
#include "test.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

/*------------------------------------------------------------------------
 * Test suite function prototype
 */
void tc_1_main_task(void);  /* TEST # 1 - Testing function _usr_lwmem_alloc */
void tc_2_main_task(void);  /* TEST # 2 - Testing function _usr_lwmem_alloc_from */
void tc_3_main_task(void);  /* TEST # 3 - Testing function _usr_lwmem_free */

/*------------------------------------------------------------------------
 * MQX task template
 */
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    { SUPER_TASK, super_task, 1500, 10, "super", MQX_AUTO_START_TASK },
    {  MAIN_TASK,  main_task, 3000,  9,  "main",       MQX_USER_TASK },
    {   SUB_TASK,   sub_task, 1500,  8,   "sub",       MQX_USER_TASK },
    {  PRIV_TASK,  priv_task, 1500,  9,  "priv",                   0 },
    {          0,          0,    0,  0,       0,                   0 }
};

/******************** Begin Test cases **********************/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*   This test case is to verify function _usr_lwmem_alloc in the cases of:
*   - Allocating memory block properly.
*   - Allocating memory block from a full lightweight memory pool.
* Requirements:
*   USERMODE024, USERMODE025, USERMODE029.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    MEMORY_ALLOC_INFO_STRUCT memory_alloc_info;
    void *mem_ptr;

    /* Allocate all the remaining blocks of user memory pool, save the block pointers to memory_alloc_info */
    memory_alloc_all(&memory_alloc_info);
    /* Reset the task's error code to MQX_OK */
    _usr_task_set_error(MQX_OK);
    /* Try to allocate block from the full memory pool by calling function _usr_lwmem_alloc */
    mem_ptr = _usr_lwmem_alloc(ALLOC_SIZE);
    /* Check the returned pointer must be NULL to verify allocating block from full memory pool should fail */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, mem_ptr == NULL, "TEST #1: Testing 1.01: Failed to allocate memory when memory is insufficient");
    /* Check the task's error code must be MQX_OUT_OF_MEMORY to verify correct error code was set */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, _task_errno == MQX_OUT_OF_MEMORY, "TEST #1: Testing 1.02: The task's error code must be MQX_OUT_OF_MEMORY");
    /* Free all the memory blocks whose pointers was stored in memory_alloc_info */
    memory_free_all(&memory_alloc_info);
    /* Reset the task's error code to MQX_OK */
    _usr_task_set_error(MQX_OK);

    /* Allocate memory from user memory pool properly by calling function _usr_lwmem_alloc */
    mem_ptr = _usr_lwmem_alloc(ALLOC_SIZE);
    /* Check the returned pointer must be NOT NULL to verify successfully allocating */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, mem_ptr != NULL, "TEST #1: Testing 1.03: Allocate memory properly");

    /* Free the allocated block by calling function _usr_lwmem_free */
    result = _usr_lwmem_free(mem_ptr);
    /* Check the returned result must be MQX_OK to verify successfully deallocating */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "TEST #1: Testing 1.04: Free the allocated block");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : TEST #2:
*   This test case is to verify function _usr_lwmem_alloc_from in the cases of:
*   - Allocating memory block properly.
*   - Allocating memory block from a full lightweight memory pool.
* Requirements:
*   USERMODE026, USERMODE027, USERMODE028, USERMODE029.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    void *mem_ptr;
    _lwmem_pool_id pool_id;

    /* Create external memory pool by calling function _usr_lwmem_create_pool */
    pool_id = _usr_lwmem_create_pool(&mem_pool, mem_space, POOL_SIZE);
    /* Check the returned pool ID must be NOT NULL to verify successfully creating */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, pool_id != NULL, "TEST #2: Testing 2.01: Create external memory pool");

    /* Try to allocate block with very big size from external memory pool by calling function _usr_lwmem_alloc_from */
    mem_ptr = _usr_lwmem_alloc_from(pool_id, POOL_SIZE + ALLOC_SIZE);
    /* Check the returned pointer must be NULL to verify allocating block should fail when memory is insufficient */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, mem_ptr == NULL, "TEST #2: Testing 2.02: Failed to allocate memory when memory is insufficient");
    /* Check the task's error code must be MQX_OUT_OF_MEMORY to verify correct error code was set */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_OUT_OF_MEMORY, "TEST #2: Testing 2.03: The task's error code must be MQX_OUT_OF_MEMORY");
    /* Reset the task's error code to MQX_OK */
    _usr_task_set_error(MQX_OK);

    /* Allocate memory from external memory pool properly by calling function _usr_lwmem_alloc_from */
    mem_ptr = _usr_lwmem_alloc_from(pool_id, ALLOC_SIZE);
    /* Check the returned pointer must be NOT NULL to verify successfully allocating */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, mem_ptr != NULL, "TEST #2: Testing 2.04: Allocate memory properly");

    /* Free the allocated block by calling function _usr_lwmem_free */
    result = _usr_lwmem_free(mem_ptr);
    /* Check the returned result must be MQX_OK to verify successfully deallocating */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "TEST #2: Testing 2.05: Free the allocated block");
}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : TEST #3:
*   This test case is to verify function _usr_lwmem_free in the cases of:
*   - Deallocating an invalid memory block.
*   - Deallocating memory block that task doesn't own.
* Requirements:
*   USERMODE029, USERMODE031, USERMODE032, USERMODE033.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    void *mem_ptr;
    _task_id tid;

    /* Allocate memory from user memory pool properly by calling function _usr_lwmem_alloc */
    mem_ptr = _usr_lwmem_alloc(ALLOC_SIZE);
    /* Check the returned pointer must be NOT NULL to verify successfully allocating */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, mem_ptr != NULL, "TEST #3: Testing 3.01: Allocate memory properly");

    if(enable_tc3_null_test){
      /* Assign NULL value to pointer global_mem_ptr */
      global_mem_ptr = NULL;
      /* Try to free an invalid memory block (global_mem_ptr) by calling function _usr_lwmem_free */
      result = _usr_lwmem_free(global_mem_ptr);
      /* Check the returned result must be MQX_INVALID_POINTER to verify deallocating an invalid memory block should fail */
      EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_INVALID_POINTER, "TEST #3: Testing 3.02: Failed to free an invalid memory block");
    }
    /* Create the SUB_TASK by calling function _task_create. With higher priority, SUB_TASK runs immediately */
    tid = _task_create(0, SUB_TASK, 0);
    /* Check the returned task ID must be non-zero to verify successfully creating */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, tid != 0, "TEST #3: Testing 3.03: Create SUB task");
    /* Check the global_mem_ptr must be NOT NULL to verify successfuly allocating in SUB_TASK */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, global_mem_ptr != NULL, "TEST #3: Testing 3.04: Allocate private block");
    /* Try to free the memory block global_mem_ptr, which belongs to another task, by calling function _usr_lwmem_free */
    result = _usr_lwmem_free(global_mem_ptr);
    /* Check the returned result must be MQX_NOT_RESOURCE_OWNER to verify deallocating block of another task should fail */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_NOT_RESOURCE_OWNER, "TEST #3: Testing 3.05: Failed to free the memory block that task doesn't own");
    /* Destroy the SUB_TASK and all of its resources by calling function _task_destroy */
    result = _task_destroy(tid);
    /* Check the returned result must be MQX_OK to verify successful operation */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "TEST #3: Testing 3.06: Destroy the SUB task and deallocate all of its resources");

    /* Set bit EVENT_MASK_PRIV of isr_lwevent by calling function _usr_lwevent_set. The PRIV_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_PRIV);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret_val == MQX_OK, "TEST #3: Testing 3.07: Set event bit EVENT_MASK_PRIV");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in PRIV_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret_val == MQX_OK, "TEST #3: Testing 3.08: Wait for event bit EVENT_MASK_MAIN");
    /* Try to free block from the invalid lightweight memory pool by calling function _usr_lwmem_free */
    result = _usr_lwmem_free(mem_ptr);
    /* Check the returned result must be MQX_LWMEM_POOL_INVALID to verify deallocating block from invalid pool should fail */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_LWMEM_POOL_INVALID, "TEST #3: Testing 3.09: Failed to free block from the invalid lightweight memory pool");
    /* Set bit EVENT_MASK_PRIV of isr_lwevent by calling function _usr_lwevent_set. The PRIV_TASK runs after this */
    ret_val = _usr_lwevent_set(&isr_lwevent, EVENT_MASK_PRIV);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret_val == MQX_OK, "TEST #3: Testing 3.10: Set event bit EVENT_MASK_PRIV");
    /* Wait for bit EVENT_MASK_MAIN of isr_lwevent to be set in PRIV_TASK by calling function _usr_lwevent_wait_ticks */
    ret_val = _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_MAIN, TRUE, 0);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, ret_val == MQX_OK, "TEST #3: Testing 3.11: Wait for event bit EVENT_MASK_MAIN");

    /* Free the allocated block by calling function _usr_lwmem_free */
    result = _usr_lwmem_free(mem_ptr);
    /* Check the returned result must be MQX_OK to verify successfully deallocating */
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, result == MQX_OK, "TEST #3: Testing 3.12: Free the allocated block");
}

/*------------------------------------------------------------------------
 * Define Test Suite
 */
EU_TEST_SUITE_BEGIN(suite_usr_lwmem)
    EU_TEST_CASE_ADD(tc_1_main_task, "TEST # 1 - Testing function _usr_lwmem_alloc"),
    EU_TEST_CASE_ADD(tc_2_main_task, "TEST # 2 - Testing function _usr_lwmem_alloc_from"),
    EU_TEST_CASE_ADD(tc_3_main_task, "TEST # 3 - Testing function _usr_lwmem_free"),
EU_TEST_SUITE_END(suite_usr_lwmem)
/* Add test suites */
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_usr_lwmem, "MQX Usermode Component Test Suite, 3 Tests")
EU_TEST_REGISTRY_END()
/*------------------------------------------------------------------------*/

/*TASK*-------------------------------------------------------------------
*
* Task Name    : main_task
* Comments     :
*   This is the main task with user access rights, this task creates
*   all defined test tasks and lets them run continuously.
*
*END*----------------------------------------------------------------------*/
void main_task(uint32_t param)
{
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    eunit_test_stop();
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : super_task
* Comments     :
*   This is the privilege task to the MQX Kernel program, this task creates
*   main task with user access rights.
*
*END*----------------------------------------------------------------------*/
void super_task(uint32_t param)
{
    _task_id tid;

    /* Install all unexpected interrupt */
    _int_install_unexpected_isr();

    /* Create PRIV task. With higher priority, this task run immediately */
    tid = _task_create(0, PRIV_TASK, 0);
    if(tid == MQX_NULL_TASK_ID)
    {
        printf("Unable to create test application");
        return;
    }

    /* Create testing MPU record used in tc_3 */
    result = _psp_mpu_add_region((unsigned char*)0, (unsigned char*)32, \
            MPU_WORD_M1SM(MPU_SM_RWX) | MPU_WORD_M1UM(MPU_UM_R | MPU_UM_W) | \
            MPU_WORD_M0SM(MPU_SM_RWX) | MPU_WORD_M0UM(MPU_UM_R | MPU_UM_W));
    if(result == MQX_OK)
        enable_tc3_null_test = 1;
    else
        enable_tc3_null_test = 0;

    /* Create the MAIN_TASK with user access rights and make it ready */
    tid = _task_create(0, MAIN_TASK, 0);
    /* Check if the MAIN_TASK was created */
    if(tid == MQX_NULL_TASK_ID)
    {
        printf("Unable to create test application");
        return;
    }
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : sub_task
* Comments     :
*   This task allocates a memory block which is used to test deallocating
*   memory of another task.
*
*END*----------------------------------------------------------------------*/
void sub_task(uint32_t param)
{
    /* Allocate a private memory block by calling function _usr_lwmem_alloc */
    global_mem_ptr = _usr_lwmem_alloc(ALLOC_SIZE);

    /* This task blocks itself by calling function _task_block */
    _task_block();
}

/*TASK*-------------------------------------------------------------------
*
* Task Name    : priv_task
* Comments     :
*   This task is to change a lightweight message queue between valid and
*   invalid state.
*
*END*----------------------------------------------------------------------*/
void priv_task(uint32_t param)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    LWMEM_POOL_STRUCT_PTR mem_pool_ptr;

    /* Create the isr_lwevent with auto clear bits by calling function _usr_lwevent_create */
    result = _usr_lwevent_create(&isr_lwevent, LWEVENT_AUTO_CLEAR);
    if(result != MQX_OK)
    {
        printf("Unable to create test component");
    }
    /* Wait for isr_lwevent to be set in MAIN_TASK by calling function _usr_lwevent_wait_ticks */
    _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_PRIV, TRUE, 0);

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the user memory pool pointer from kernel data */
    mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR)kernel_data->KD_USER_POOL;

    /* Make the user memory pool to be invalid by changing its VALID field */
    _int_disable();
    mem_pool_ptr->VALID = LWMEM_POOL_VALID + INVALID_VALUE;
    _int_enable();
    /* Set the isr_lwevent by calling function _usr_lwevent_set. The MAIN_TASK runs after this */
    _usr_lwevent_set(&isr_lwevent, EVENT_MASK_MAIN);
    /* Wait for isr_lwevent to be set in MAIN_TASK by calling function _usr_lwevent_wait_ticks */
    _usr_lwevent_wait_ticks(&isr_lwevent, EVENT_MASK_PRIV, TRUE, 0);

    /* Make the user memory pool to be valid by restoring its VALID field */
    _int_disable();
    mem_pool_ptr->VALID = LWMEM_POOL_VALID;
    _int_enable();
    /* Set the isr_lwevent by calling function _usr_lwevent_set. The MAIN_TASK runs after this */
    _usr_lwevent_set(&isr_lwevent, EVENT_MASK_MAIN);
}
