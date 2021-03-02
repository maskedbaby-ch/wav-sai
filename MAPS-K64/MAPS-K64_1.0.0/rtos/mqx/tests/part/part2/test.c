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
* $Version : 4.0.1.0$
* $Date    : May-02-2013$
*
* Comments    :
*               This file contains the source for the task module testing.
* Requirement :
*               PARTITION001,PARTITION002,PARTITION003,PARTITION005,PARTITION008,
*               PARTITION009,PARTITION010,PARTITION012,PARTITION013,PARTITION014,
*               PARTITION016,PARTITION017,PARTITION018,PARTITION019,PARTITION020,
*               PARTITION024,PARTITION023,PARTITION025,PARTITION026,PARTITION027,
*               PARTITION028,PARTITION029,PARTITION030,PARTITION031,PARTITION032,
*               PARTITION033,PARTITION034,PARTITION035,PARTITION036,PARTITION037,
*               PARTITION038,PARTITION039,PARTITION041,PARTITION042,PARTITION046,
*               PARTITION048,PARTITION050,PARTITION056,PARTITION057,PARTITION058.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mem_prv.h>
#include <mqx_prv.h>
#include <partition.h>
#include <partition_prv.h>
#include "test.h"
#include "part_util.h"
#include "util.h"

#include "EUnit.h"
#include "eunit_crit_secs.h"

/*------------------------------------------------------------------------*/
/* Test suite function prototype.*/
void tc_1_main_task(void);/* TEST # 1 - Testing function _partition_create_component                   */
void tc_2_main_task(void);/* TEST # 2 - Testing function _partition_create in special cases            */
void tc_3_main_task(void);/* TEST # 3 - Testing function _partition_create_at in special cases         */
void tc_4_main_task(void);/* TEST # 4 - Testing function _partition_destroy in special cases           */
void tc_5_main_task(void);/* TEST # 5 - Testing function _partition_alloc in special cases             */
void tc_6_main_task(void);/* TEST # 6 - Testing function _partition_alloc_zero in special cases        */
void tc_7_main_task(void);/* TEST # 7 - Testing function _partition_alloc_system in special cases      */
void tc_8_main_task(void);/* TEST # 8 - Testing function _partition_alloc_system_zero in special cases */
void tc_9_main_task(void);/* TEST # 9 - Testing function _partition_extend in special cases            */
void tc_10_main_task(void);/* TEST # 10 - Testing function _partition_free in special cases            */
void tc_11_main_task(void);/* TEST # 11 - Testing function _partition_get_total_blocks,
              _partition_get_max_used_blocks, _partition_get_total_blocks when partition is not valid. */
void tc_12_main_task(void);/* TEST # 12 - Testing function _partition_transfer in special cases        */

/*------------------------------------------------------------------------*/

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
{ MAIN_TASK,   main_task,  2000,   9,  "Main_Task",  MQX_AUTO_START_TASK},
{ TEST_TASK,   test_task,  1000,   10,  "Test_Task",         0},
{         0,           0,     0,   0,            0,         0}
};

/*------------------------------------------------------------------------*/

unsigned char   *global_memory_buffer_for_part;
unsigned char   *global_memory_extension_buffer;

/*------------------------------------------------------------------------*/

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : Testing function _partition_create_component
* Requirements :
*                PARTITION028,PARTITION029,PARTITION030.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    _mqx_uint                 temp;
    _mqx_uint                 result;
    KERNEL_DATA_STRUCT_PTR    kernel_data;
    MEMORY_ALLOC_INFO_STRUCT  memory_alloc_info;

    /* Get the current kernel data */
    _GET_KERNEL_DATA(kernel_data);

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);

    /* Tests function _partition_create_component when memory is insufficient */
    result = _partition_create_component();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OUT_OF_MEMORY, "Test #1 Testing 1.00: Create partition component when memory is insufficient");
    _task_set_error(MQX_OK);

    /* Frees all of memory that was temporary allocated */
    memory_free_all(&memory_alloc_info);

    _int_disable();
    /* Save the current isr counting value */
    temp = kernel_data->IN_ISR;
    /* Set the current task as an isr */
    kernel_data->IN_ISR = 1;
    /* [_partition_create_component] Create partition component in ISR */
    result = _partition_create_component();
    /* Restore the isr counting value */
    kernel_data->IN_ISR = temp;
    _int_enable();
    /* Verify the result of creating */
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_CANNOT_CALL_FUNCTION_FROM_ISR, "Test #1 Testing 1.01: Calling _partition_create_component in ISR should be failed");
    _task_set_error(MQX_OK);

    /* Create partition component */
    result = _partition_create_component();
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1 Testing 1.02: Create partition component");
    EU_ASSERT_REF_TC_MSG(tc_1_main_task, kernel_data->KERNEL_COMPONENTS[KERNEL_PARTITIONS] != NULL, "Test #1 Testing 1.03: Checking partition component was created");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_2_main_task
* Comments     : Testing function _partition_create in special cases
* Requirements :
*                PARTITION023,PARTITION024,PARTITION025,PARTITION031.
*
*END*---------------------------------------------------------------------*/
void tc_2_main_task(void)
{
    _mqx_uint                 temp;
    _mqx_uint                 result;
    _partition_id             part_id;
    KERNEL_DATA_STRUCT_PTR    kernel_data;
    MEMORY_ALLOC_INFO_STRUCT  memory_alloc_info;

    /* Get the current kernel data */
    _GET_KERNEL_DATA(kernel_data);

    /* Disable all interrupt */
    _int_disable();
    /* Save the current isr counting value */
    temp = kernel_data->IN_ISR;
    /* Set the current task as an isr */
    kernel_data->IN_ISR = 1;
    /* [_partition_create] Create a partition in ISR */
    part_id = _partition_create(BLOCK_SIZE, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS * 4);
    /* Restore the isr counting value */
    kernel_data->IN_ISR = temp;
    /* Enable all interrupt */
    _int_enable();
    /* Verify the result of creating */
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, part_id == PARTITION_NULL_ID, "Test #2 Testing 2.00: Calling _partition_create in ISR should be failed");
    _task_set_error(MQX_OK);

    /* Creates partition with blocks size is not valid */
    part_id = _partition_create(0, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS * 4);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, part_id == PARTITION_NULL_ID, "Test #2 Testing 2.01: Tests function _patition_create when parameter is not valid");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_INVALID_PARAMETER, "Test #2 Testing 2.02: The task's error code should be MQX_INVALID_PARAMETER");
    _task_set_error(MQX_OK);

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);

    /* Create partition when memory is insufficient */
    part_id = _partition_create(BLOCK_SIZE, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS * 4);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, part_id == PARTITION_NULL_ID, "Test #2 Testing 2.03: Tests function _patition_create when memory is insufficient");
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #2 Testing 2.04: The task's error code should be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

    /* Frees all of memory that was temporary allocated */
    memory_free_all(&memory_alloc_info);

    /* Create partition */
    part_id = _partition_create(BLOCK_SIZE, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS * 4);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, part_id != PARTITION_NULL_ID, "Test #2 Testing 2.05: Create partition the part size");

    /* Destroy partition */
    result = _partition_destroy(part_id);
    EU_ASSERT_REF_TC_MSG(tc_2_main_task, result == MQX_OK, "Test #2 Testing 2.06: Destroy partition that was createed");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_3_main_task
* Comments     : Testing function _partition_create_at in special cases
* Requirements :
*                PARTITION026,PARTITION027,PARTITION031.
*
*END*---------------------------------------------------------------------*/
void tc_3_main_task(void)
{
    _partition_id  part_id;

    global_memory_buffer_for_part = _mem_alloc_system(MEM_PART_SIZE);
    /* Create partition when block size is not valid */
    part_id = _partition_create_at(global_memory_buffer_for_part, MEM_PART_SIZE, 0);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, part_id == PARTITION_NULL_ID,"Test #3 Testing 3.00: Tests function _partition_create_at with blocks size is not valid");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_INVALID_PARAMETER, "Test #3 Testing 3.01: The task's error code should be MQX_INVALID_PARAMETER");
    _task_set_error(MQX_OK);

    /* Create partition when partition size is too small */
    part_id = _partition_create_at(global_memory_buffer_for_part, sizeof(PARTPOOL_STRUCT) - 1, BLOCK_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, part_id == PARTITION_NULL_ID,"Test #3 Testing 3.02: Tests function _partition_create_at with partition size is too small");
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, _task_errno == MQX_INVALID_PARAMETER, "Test #3 Testing 3.03: The task's error code should be MQX_INVALID_PARAMETER");
    _task_set_error(MQX_OK);

    /* Create partition when block_size is not valid */
    part_id = _partition_create_at(global_memory_buffer_for_part, MEM_PART_SIZE, MEM_PART_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_3_main_task, part_id == PARTITION_NULL_ID,"Test #3 Testing 3.04: Tests function _partition_create_at with blocks size is not valid");
    _task_set_error(MQX_OK);

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_4_main_task
* Comments     : Testing function _partition_destroy in special cases
* Requirements :
*                PARTITION023,PARTITION031,PARTITION032,PARTITION033,
*                PARTITION034,PARTITION035.
*
*END*---------------------------------------------------------------------*/
void tc_4_main_task(void)
{
    void          *temp_ptr;
    _mqx_uint      temp;
    _mqx_uint      result;
    _partition_id  part_id;

    /* Create partition */
    part_id = _partition_create(BLOCK_SIZE, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS * 4);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, part_id != PARTITION_NULL_ID, "Test #4 Testing 4.00: Create partition the part size");

    /* Backups pointer to partition component */
    temp_ptr = get_partition_component_ptr();

    /* Makes pointer to partition component to NULL */
    set_partition_component_ptr(NULL);

    /* Tests function _partiton_destroy when partition component was not previously created*/
    result = _partition_destroy(part_id);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_COMPONENT_DOES_NOT_EXIST, "Test #4 Testing 4.01: Tests _partiton_destroy when partition component was not previously created");
    _task_set_error(MQX_OK);

    /* Restore pointer to partition component to original value */
    set_partition_component_ptr(temp_ptr);

    /* Backups the VALID field of partition */
    temp = get_partition_valid_field(part_id);

    /* Makes the VALID field of partition to be an invalid value */
    set_partition_valid_field(part_id, PARTITION_VALID + 1);

    /* Destroy partition that is not valid */
    result = _partition_destroy(part_id);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_INVALID_PARAMETER, "Test #4 Testing 4.02: Tests _partiton_destroy when partition is not valid");
    _task_set_error(MQX_OK);

    /* Restores the VALID field of partition to be original value */
    set_partition_valid_field(part_id, temp);

    /* Sets partition type to PARTITION_STATIC */
    set_partition_type_field(part_id, PARTITION_STATIC);

    /* Destroy partition when partition type is not valid */
    result = _partition_destroy(part_id);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == PARTITION_INVALID_TYPE, "Test #4 Testing 4.03: Tests _partiton_destroy when partition type is not valid");
    _task_set_error(MQX_OK);

    /* Sets partition type to PARTITION_DYNAMIC */
    set_partition_type_field(part_id, PARTITION_DYNAMIC);

    /* Backups the available field of partition */
    temp = get_partition_available_field(part_id);

    /* Makes the available field of partition to be an invalid value */
    set_partition_available_field(part_id, 0);

    /* Destroy partition when all block of partition is not free */
    result = _partition_destroy(part_id);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == PARTITION_ALL_BLOCKS_NOT_FREE, "Test #4 Testing 4.04: Tests _partiton_destroy when all block of partition is not free");
    _task_set_error(MQX_OK);

    /* Restore the available field of partition */
    set_partition_available_field(part_id, temp);

    /* Destroy partition */
    result = _partition_destroy(part_id);
    EU_ASSERT_REF_TC_MSG(tc_4_main_task, result == MQX_OK, "Test #4 Testing 4.05: Destroy partition that was created");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_5_main_task
* Comments     : Testing function _partition_alloc in special cases
* Requirements :
*                PARTITION002,PARTITION003,PARTITION005,PARTITION023,PARTITION031.
*
*END*---------------------------------------------------------------------*/
void tc_5_main_task(void)
{
    void                     *mem_ptr;
    _mqx_uint                 temp;
    _mqx_uint                 result;
    _partition_id             part_id;
    MEMORY_ALLOC_INFO_STRUCT  memory_alloc_info;

    /* Allocate private partition block when partition id is NULL*/
    mem_ptr = _partition_alloc(NULL);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, mem_ptr == NULL, "Test #5 Testing 5.00: Tests function _partition_alloc with partition id is NULL");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == PARTITION_INVALID, "Test #5 Testing 5.01: The task's error code should be PARTITION_INVALID");
    _task_set_error(MQX_OK);

    /* Create partition */
    part_id = _partition_create(BLOCK_SIZE, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS * 4);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, part_id != PARTITION_NULL_ID, "Test #5 Testing 5.02: Create partition the part size");

    /* Backups the VALID field of partition */
    temp = get_partition_valid_field(part_id);

    /* Makes the VALID field of partition to be an invalid value */
    set_partition_valid_field(part_id, PARTITION_VALID + 1);

    /* Allocate private partition block when partition is not valid */
    mem_ptr = _partition_alloc(part_id);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, mem_ptr == NULL, "Test #5 Testing 5.03: Tests function _partition_alloc with partition is not valid ");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == PARTITION_INVALID, "Test #5 Testing 5.04: The task's error code should be PARTITION_INVALID");
    _task_set_error(MQX_OK);

    /* Restores the VALID field of partition to be original value */
    set_partition_valid_field(part_id, temp);

    /* Backups the available field of partition */
    temp = get_partition_available_field(part_id);

    /* Makes the available field of partition to be an invalid value */
    set_partition_available_field(part_id, 0);

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);

    mem_ptr = _partition_alloc(part_id);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, mem_ptr == NULL, "Test #5 Testing 5.05: Tests function _partition_alloc with partition is not valid ");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #5 Testing 5.06: The task's error code should be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

    /* Frees all of memory that was temporary allocated */
    memory_free_all(&memory_alloc_info);

    /* Makes the available field of partition to be an invalid value */
    set_partition_available_field(part_id, temp);

    /* Backups the CHECKSUM field of partition */
    temp = get_partition_checksum_field(part_id);

    /* Makes the CHECKSUM field of partition to be an invalid value */
    set_partition_checksum_field(part_id, temp + 1);

    /* Allocate partition block when partition is not valid */
    mem_ptr = _partition_alloc(part_id);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, mem_ptr == NULL, "Test #5 Testing 5.07: Tests function _partition_alloc with partition is not valid ");
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, _task_errno == PARTITION_BLOCK_INVALID_CHECKSUM, "Test #5 Testing 5.08: The task's error code should be PARTITION_BLOCK_INVALID_CHECKSUM");
    _task_set_error(MQX_OK);

    /* Makes the CHECKSUM field of partition to be an invalid value */
    set_partition_checksum_field(part_id, temp);

    /* Destroy partition */
    result = _partition_destroy(part_id);
    EU_ASSERT_REF_TC_MSG(tc_5_main_task, result == MQX_OK, "Test #5 Testing 5.09: Destroy partition that was created");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_6_main_task
* Comments     : Testing function _partition_alloc_zero in special cases
* Requirements :
*                PARTITION007,PARTITION008,PARTITION009,PARTITION010
*                PARTITION023,PARTITION031.
*
*END*---------------------------------------------------------------------*/
void tc_6_main_task(void)
{
    void                     *mem_ptr;
    _mqx_uint                 temp;
    _mqx_uint                 result;
    _partition_id             part_id;
    MEMORY_ALLOC_INFO_STRUCT  memory_alloc_info;

    /* Allocate private partition block when partition id is NULL*/
    mem_ptr = _partition_alloc_zero(NULL);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, mem_ptr == NULL, "Test #6 Testing 6.00: Tests function _partition_alloc_zero with partition id is NULL");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == PARTITION_INVALID, "Test #6 Testing 6.01: The task's error code should be PARTITION_INVALID");
    _task_set_error(MQX_OK);

    part_id = _partition_create(BLOCK_SIZE, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS * 4);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, part_id != PARTITION_NULL_ID, "Test #6 Testing 6.02: Create partition the part size");

    /* Backups the VALID field of partition */
    temp = get_partition_valid_field(part_id);

    /* Makes the VALID field of partition to be an invalid value */
    set_partition_valid_field(part_id, PARTITION_VALID + 1);

    /* Allocate private partition block when partition is not valid */
    mem_ptr = _partition_alloc_zero(part_id);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, mem_ptr == NULL, "Test #6 Testing 6.03: Tests function _partition_alloc_zero with partition is not valid ");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == PARTITION_INVALID, "Test #6 Testing 6.04: The task's error code should be PARTITION_INVALID");
    _task_set_error(MQX_OK);

    /* Restores the VALID field of partition to be original value */
    set_partition_valid_field(part_id, temp);

    /* Backups the CHECKSUM field of partition */
    temp = get_partition_checksum_field(part_id);

    /* Makes the CHECKSUM field of partition to be an invalid value */
    set_partition_checksum_field(part_id, temp + 1);

    /* Allocate partition block when incorrect checksum in the partition block header*/
    mem_ptr = _partition_alloc_zero(part_id);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, mem_ptr == NULL, "Test #6 Testing 6.05: Tests function _partition_alloc_zero with partition is not valid ");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == PARTITION_BLOCK_INVALID_CHECKSUM, "Test #6 Testing 6.06: The task's error code should be PARTITION_BLOCK_INVALID_CHECKSUM");
    _task_set_error(MQX_OK);

    /* Makes the CHECKSUM field of partition to be an invalid value */
    set_partition_checksum_field(part_id, temp);

    /* Backups the available field of partition */
    temp = get_partition_available_field(part_id);

    /* Makes the available field of partition to be an invalid value */
    set_partition_available_field(part_id, 0);

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);

    /* Allocate partition block when memory is insufficient */
    mem_ptr = _partition_alloc_zero(part_id);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, mem_ptr == NULL, "Test #6 Testing 6.07: Tests function _partition_alloc_zero with memory is insufficient");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #6 Testing 6.08: The task's error code should be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

    /* Frees all of memory that was temporary allocated */
    memory_free_all(&memory_alloc_info);

    /* Sets partition type to PARTITION_STATIC */
    set_partition_type_field(part_id, PARTITION_STATIC);

    /* Allocate partition block when all block of partition was allocated */
    mem_ptr = _partition_alloc_zero(part_id);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, mem_ptr == NULL, "Test #6 Testing 6.09: Tests function _partition_alloc_zero with all blocks of partition was allocated ");
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, _task_errno == PARTITION_OUT_OF_BLOCKS, "Test #6 Testing 6.10: The task's error code should be PARTITION_OUT_OF_BLOCKS");
    _task_set_error(MQX_OK);

    /* Sets partition type to PARTITION_DYNAMIC */
    set_partition_type_field(part_id, PARTITION_DYNAMIC);

    /* Makes the available field of partition to be an invalid value */
    set_partition_available_field(part_id, temp);

    /* Destroy partition */
    result = _partition_destroy(part_id);
    EU_ASSERT_REF_TC_MSG(tc_6_main_task, result == MQX_OK, "Test #6 Testing 6.11: Destroy partition that was created");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_7_main_task
* Comments     : Testing function _partition_alloc_system in special cases
* Requirements :
*                PARTITION012,PARTITION013,PARTITION014,PARTITION016
*                PARTITION023,PARTITION031.
*
*END*---------------------------------------------------------------------*/
void tc_7_main_task(void)
{
    void                     *mem_ptr;
    _mqx_uint                 temp;
    _mqx_uint                 result;
    _partition_id             part_id;
    MEMORY_ALLOC_INFO_STRUCT  memory_alloc_info;

    /* Allocate private partition block when partition id is NULL*/
    mem_ptr = _partition_alloc_system(NULL);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, mem_ptr == NULL, "Test #7 Testing 7.00: Tests function _partition_alloc_system with partition id is NULL");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, _task_errno == PARTITION_INVALID, "Test #7 Testing 7.01: The task's error code should be PARTITION_INVALID");
    _task_set_error(MQX_OK);

    /* Create partition */
    part_id = _partition_create(BLOCK_SIZE, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS * 4);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, part_id != PARTITION_NULL_ID, "Test #7 Testing 7.02: Create partition the part size");

    /* Backups the VALID field of partition */
    temp = get_partition_valid_field(part_id);

    /* Makes the VALID field of partition to be an invalid value */
    set_partition_valid_field(part_id, PARTITION_VALID + 1);

    /* Allocate private partition block when partition is not valid */
    mem_ptr = _partition_alloc_system(part_id);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, mem_ptr == NULL, "Test #7 Testing 7.03: Tests function _partition_alloc_system with partition is not valid ");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, _task_errno == PARTITION_INVALID, "Test #7 Testing 7.04: The task's error code should be PARTITION_INVALID");
    _task_set_error(MQX_OK);

    /* Restores the VALID field of partition to be original value */
    set_partition_valid_field(part_id, temp);

    /* Backups the CHECKSUM field of partition */
    temp = get_partition_checksum_field(part_id);

    /* Makes the CHECKSUM field of partition to be an invalid value */
    set_partition_checksum_field(part_id, temp + 1);

    /* Allocate partition block when incorrect checksum in the partition block header */
    mem_ptr = _partition_alloc_system(part_id);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, mem_ptr == NULL,"Test #7 Testing 7.05: Tests function _partition_alloc_system with partition is not valid ");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, _task_errno == PARTITION_BLOCK_INVALID_CHECKSUM, "Test #7 Testing 7.06: The task's error code should be PARTITION_BLOCK_INVALID_CHECKSUM");
    _task_set_error(MQX_OK);

    /* Makes the CHECKSUM field of partition to be an invalid value */
    set_partition_checksum_field(part_id, temp);

    /* Backups the available field of partition */
    temp = get_partition_available_field(part_id);

    /* Makes the available field of partition to 0 */
    set_partition_available_field(part_id, 0);

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);

    /* Allocate partition block when memory is insufficient */
    mem_ptr = _partition_alloc_system(part_id);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, mem_ptr == NULL, "Test #7 Testing 7.07: Tests function _partition_alloc_system with memory is insufficient");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #7 Testing 7.08: The task's error code should be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

    /* Frees all of memory that was temporary allocated */
    memory_free_all(&memory_alloc_info);

    /* Sets partition type to PARTITION_STATIC */
    set_partition_type_field(part_id, PARTITION_STATIC);

    /* Allocate partition block when all block of partition was allocated */
    mem_ptr = _partition_alloc_system(part_id);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, mem_ptr == NULL, "Test #7 Testing 7.09: Tests function _partition_alloc_system with all block of partition was allocated");
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, _task_errno == PARTITION_OUT_OF_BLOCKS, "Test #7 Testing 7.10: The task's error code should be PARTITION_OUT_OF_BLOCKS");
    _task_set_error(MQX_OK);

    /* Sets partition type to PARTITION_DYNAMIC */
    set_partition_type_field(part_id, PARTITION_DYNAMIC);

    /* Restore the available field of partition original value */
    set_partition_available_field(part_id, temp);

    /* Destroy partition */
    result = _partition_destroy(part_id);
    EU_ASSERT_REF_TC_MSG(tc_7_main_task, result == MQX_OK, "Test #7 Testing 7.11: Destroy partition that was created");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_8_main_task
* Comments     : Testing function _partition_alloc_system_zero in special cases
* Requirements :
*                PARTITION017,PARTITION018,PARTITION019,PARTITION020
*                PARTITION023,PARTITION031.
*
*END*---------------------------------------------------------------------*/
void tc_8_main_task(void)
{
    void                     *mem_ptr;
    _mqx_uint                 temp;
    _mqx_uint                 result;
    _partition_id             part_id;
    MEMORY_ALLOC_INFO_STRUCT  memory_alloc_info;

    /* Allocate private partition block when partition id is NULL*/
    mem_ptr = _partition_alloc_system_zero(NULL);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, mem_ptr == NULL, "Test #8 Testing 8.00: Tests function _partition_alloc_system_zero with partition id is NULL");
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _task_errno == PARTITION_INVALID, "Test #8 Testing 8.01: The task's error code should be PARTITION_INVALID");
    _task_set_error(MQX_OK);

    /* Create partition */
    part_id = _partition_create(BLOCK_SIZE, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS * 4);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, part_id != PARTITION_NULL_ID, "Test #8 Testing 8.02: Create partition the part size");

    /* Backups the VALID field of partition */
    temp = get_partition_valid_field(part_id);

    /* Makes the VALID field of partition to be an invalid value */
    set_partition_valid_field(part_id, PARTITION_VALID + 1);

    /* Allocate private partition block when partition is not valid */
    mem_ptr = _partition_alloc_system_zero(part_id);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, mem_ptr == NULL, "Test #8 Testing 8.03: Tests function _partition_alloc_system_zero with partition is not valid ");
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _task_errno == PARTITION_INVALID, "Test #8 Testing 8.04: The task's error code should be PARTITION_INVALID");
    _task_set_error(MQX_OK);

    /* Restores the VALID field of partition to be original value */
    set_partition_valid_field(part_id, temp);

    /* Backups the CHECKSUM field of partition */
    temp = get_partition_checksum_field(part_id);

    /* Makes the CHECKSUM field of partition to be an invalid value */
    set_partition_checksum_field(part_id, temp + 1);

    /* Allocate partition block when incorrect checksum in the partition block header */
    mem_ptr = _partition_alloc_system_zero(part_id);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, mem_ptr == NULL, "Test #8 Testing 8.05: Tests function _partition_alloc_system_zero with partition is not valid ");
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _task_errno == PARTITION_BLOCK_INVALID_CHECKSUM, "Test #8 Testing 8.06: The task's error code should be PARTITION_BLOCK_INVALID_CHECKSUM");
    _task_set_error(MQX_OK);

    /* Restore the CHECKSUM field of partition to original value */
    set_partition_checksum_field(part_id, temp);

    /* Backups the available field of partition */
    temp = get_partition_available_field(part_id);

    /* Makes the available field of partition to be 0 */
    set_partition_available_field(part_id, 0);

    /* The all free memory is allocated */
    memory_alloc_all(&memory_alloc_info);

    /* Allocate partition block when memory is insufficient */
    mem_ptr = _partition_alloc_system_zero(part_id);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, mem_ptr == NULL, "Test #8 Testing 8.07: Tests function _partition_alloc_system_zero with memory is insufficient ");
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _task_errno == MQX_OUT_OF_MEMORY, "Test #8 Testing 8.08: The task's error code should be MQX_OUT_OF_MEMORY");
    _task_set_error(MQX_OK);

    /* Frees all of memory that was temporary allocated */
    memory_free_all(&memory_alloc_info);

    /* Sets pratition type to PARTITION_STATIC */
    set_partition_type_field(part_id, PARTITION_STATIC);

    /* Allocate partition block when all block of partition was allocated */
    mem_ptr = _partition_alloc_system_zero(part_id);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, mem_ptr == NULL, "Test #8 Testing 8.09: Tests function _partition_alloc_system_zero with all block of partition was allocated ");
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, _task_errno == PARTITION_OUT_OF_BLOCKS, "Test #8 Testing 8.10: The task's error code should be PARTITION_OUT_OF_BLOCKS");
    _task_set_error(MQX_OK);

    /* Sets pratition type to PARTITION_DYNAMIC */
    set_partition_type_field(part_id, PARTITION_DYNAMIC);

    /* Restore the available field of partition to be original value */
    set_partition_available_field(part_id, temp);

    /* Destroy partition */
    result = _partition_destroy(part_id);
    EU_ASSERT_REF_TC_MSG(tc_8_main_task, result == MQX_OK, "Test #8 Testing 8.11: Destroy partition that was created");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_9_main_task
* Comments     : Testing function _partition_extend in special cases
* Requirements :
*                PARTITION023,PARTITION031,PARTITION036,PARTITION037,PARTITION038.
*
*END*---------------------------------------------------------------------*/
void tc_9_main_task(void)
{
    _mqx_uint      temp;
    _mqx_uint      result;
    _partition_id  part_id;

    global_memory_extension_buffer = _mem_alloc_system(MEM_EXT_SIZE);
    
    /* Create partition */
    part_id = _partition_create(BLOCK_SIZE, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS * 4);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, part_id != PARTITION_NULL_ID,"Test #9 Testing 9.00: Create partition in global memory");

    /* Backups the VALID field of partition */
    temp = get_partition_valid_field(part_id);

    /* Makes the VALID field of partition to be an invalid value */
    set_partition_valid_field(part_id, PARTITION_VALID + 1);

    /* Adds partition blocks to the static partition when partition is not valid */
    result = _partition_extend(part_id, global_memory_extension_buffer, MEM_EXT_SIZE );
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == PARTITION_INVALID, "Test #9 Testing 9.01: Tests function _partition_extend when partition is not valid");

    /* Restores the VALID field of partition to be original value */
    set_partition_valid_field(part_id, temp);

    /* Adds partition blocks to the static partition when partition type is not PARTITION_STATIC*/
    result = _partition_extend(part_id, global_memory_extension_buffer, MEM_EXT_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_INVALID_PARAMETER, "Test #9 Testing 9.02: Tests function _partition_extend when partition type is PARTITION_DYNAMIC");

    /* Sets partition type to PARTITION_STATIC */
    set_partition_type_field(part_id, PARTITION_STATIC);

    /* Adds partition blocks to the static partition when patition_size is not valid */
    result = _partition_extend(part_id, global_memory_extension_buffer, (_mem_size) 0);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_INVALID_PARAMETER, "Test #9 Testing 9.03: Tests function _partition_extend when parameter is not valid");

    /* Adds partition blocks to the static partition when all parameter is valid */
    result = _partition_extend(part_id, global_memory_extension_buffer, MEM_EXT_SIZE);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing 9.04: Tests function _partition_extend normal operation");

    /* Sets partition type to PARTITION_DYNAMIC*/
    set_partition_type_field(part_id, PARTITION_DYNAMIC);

    /* Destroy partition */
    result = _partition_destroy(part_id);
    EU_ASSERT_REF_TC_MSG(tc_9_main_task, result == MQX_OK, "Test #9 Testing 9.05: Destroy partition that was created");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_10_main_task
* Comments     : Testing function _partition_free in special cases
* Requirements :
*                PARTITION001,PARTITION023,PARTITION031,PARTITION039
*                PARTITION041,PARTITION042.
*
*END*---------------------------------------------------------------------*/
void tc_10_main_task(void)
{
    void          *mem_ptr;
    _mqx_uint      temp;
    _mqx_uint      result;
    _partition_id  part_id;

    /* Create partition */
    part_id = _partition_create(BLOCK_SIZE, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS * 4);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, part_id != PARTITION_NULL_ID, "Test #10 Testing 10.00: Create partition in global memory");

    /* Allocate private partition block from partition */
    mem_ptr = _partition_alloc(part_id);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, mem_ptr != NULL, "Test #10 Testing 10.01: Allocate private partition block from partition");

    /* Backups CHECKSUM field of block */
    temp = get_block_checksum_field(mem_ptr);

    /* Makes CHECKSUM field of block to invalid */
    set_block_checksum_field(mem_ptr, temp + 1);

    /* Frees the partition block when checksum in the partition block header is not correct*/
     result = _partition_free(mem_ptr);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task,result == PARTITION_BLOCK_INVALID_CHECKSUM, "Test #10 Testing 10.02: Tests function _partition_free when checksum in the partition block header is not correct");
    _task_set_error(MQX_OK);

    /* Restore CHECKSUM field of block to original value */
    set_block_checksum_field(mem_ptr, temp);

    /* Backups the VALID field of partition */
    temp = get_partition_valid_field(part_id);

    /* Makes the VALID field of partition to be an invalid value */
    set_partition_valid_field(part_id, PARTITION_VALID + 1);

    /* Frees the partition block when partition is not valid */
    result = _partition_free(mem_ptr);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == PARTITION_INVALID, "Test #10 Testing 10.03: Tests function _partition_free when partition is not valid");
    _task_set_error(MQX_OK);

    /* Restores the VALID field of partition to be original value */
    set_partition_valid_field(part_id, temp);

    /* Frees the partition block */
    result = _partition_free(mem_ptr);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task,result == MQX_OK, "Test #10 Testing 10.04: Tests function _partition_free operation");

    /* Destroy partition */
    result = _partition_destroy(part_id);
    EU_ASSERT_REF_TC_MSG(tc_10_main_task, result == MQX_OK, "Test #10 Testing 10.05: Destroy partition that was created");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_11_main_task
* Comments     : Testing function _partition_get_total_blocks, _partition_get_max_used_blocks
*                _partition_get_total_blocks when partition is not valid.
*
* Requirements :
*                PARTITION023,PARTITION031,PARTITION046
*                PARTITION048,PARTITION050.
*
*END*---------------------------------------------------------------------*/
void tc_11_main_task(void)
{
    _mqx_uint      temp;
    _mqx_uint      result;
    _partition_id  part_id;

    /* Create partition */
    part_id = _partition_create(BLOCK_SIZE, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS * 4);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, part_id != PARTITION_NULL_ID, "Test #11 Testing 11.00: Create partition in global memory");

    /* Backups the VALID field of partition */
    temp = get_partition_valid_field(part_id);

    /* Makes the VALID field of partition to be an invalid value */
    set_partition_valid_field(part_id, PARTITION_VALID + 1);

    /* Gets the number of free partition blocks in the partition when partition is not valid */
    result = _partition_get_free_blocks(part_id);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MAX_MQX_UINT, "Test #11 Testing 11.01: Tests function _partition_get_free_blocks when partition is nor valid");
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, _task_errno == PARTITION_INVALID, "Test #11 Testing 11.02: The task's error code should be PARTITION_INVALID");
    _task_set_error(MQX_OK);

    /* Gets the number of allocated partition blocks in the partition when partition is not valid */
    result = _partition_get_max_used_blocks(part_id);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task,result == 0, "Test #11 Testing 11.03: Tests function _partition_get_max_used_blocks when partition is nor valid");
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, _task_errno == PARTITION_INVALID, "Test #11 Testing 11.04: The task's error code should be PARTITION_INVALID");
    _task_set_error(MQX_OK);

    /* Gets the total number of partition blocks in the partition when partition is not valid */
    result = _partition_get_total_blocks(part_id);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task,result == 0, "Test #11 Testing 11.05: Tests function _partition_get_total_blocks when partition is not valid");
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, _task_errno == PARTITION_INVALID, "Test #11 Testing 11.06: The task's error code should be PARTITION_INVALID");
    _task_set_error(MQX_OK);

    /* Restores the VALID field of partition to be original value */
    set_partition_valid_field(part_id, temp);

    /* Destroy partition */
    result = _partition_destroy(part_id);
    EU_ASSERT_REF_TC_MSG(tc_11_main_task, result == MQX_OK, "Test #11 Testing 11.07: Destroy partition that was created");

}

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_12_main_task
* Comments     : Testing function _partition_transfer in special cases
* Requirements :
*                PARTITION001,PARTITION023,PARTITION031,PARTITION039
*                PARTITION056,PARTITION057,PARTITION058.
*
*END*---------------------------------------------------------------------*/
void tc_12_main_task(void)
{
    void          *mem_ptr;
    _task_id       test_tid;
    _mqx_uint      temp;
    _mqx_uint      result;
    _partition_id  part_id;

    /* Create partition */
    part_id = _partition_create(BLOCK_SIZE, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS, NUMBER_OF_BLOCKS * 4);
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, part_id != PARTITION_NULL_ID, "Test #12 Testing 12.00: Create partition in global memory");

    /* Allocate private partition */
    mem_ptr = _partition_alloc(part_id);
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, mem_ptr != NULL, "Test #12 Testing 12.01: Allocate private partition block from partition");

    /* Create TEST_TASK */
    test_tid = _task_create(0, TEST_TASK, (uint32_t)mem_ptr );

    /* Backups value of checksum field */
    temp = get_block_checksum_field(mem_ptr);
    /* Makes checksum field to value invalid */
    set_block_checksum_field(mem_ptr, temp + 1);

    result = _partition_transfer(mem_ptr,test_tid);
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == PARTITION_BLOCK_INVALID_CHECKSUM, "Test #12 Testing 12.02: Tests function _partition_transfer when partition block is not valid");
    _task_set_error(MQX_OK);

    /* Restore checksum field to original value */
    set_block_checksum_field(mem_ptr, temp);

    /* Transfer the ownership of the partition block when the task ID is not valid */
    result = _partition_transfer( mem_ptr, (-1) );
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == PARTITION_INVALID_TASK_ID, "Test #12 Testing 12.03: Tests function _partition_transfer when task_id is not valid");

    /* Transfer the ownership of the partition block */
    result = _partition_transfer( mem_ptr, test_tid );
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "Test #12 Testing 12.04: Transfer the ownership of the partition block");

    /* Suspend main task and allow TEST_TASK run */
    _time_delay_ticks(10);

    result = _task_destroy(test_tid);
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "Test #12 Testing 12.06: Destroy partition that was created");

    /* Destroy partition */
    result = _partition_destroy(part_id);

}
/*------------------------------------------------------------------------
* Define Test Suite   */

 EU_TEST_SUITE_BEGIN(suite_part2)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1 - Testing function _partition_create_component()"),
    EU_TEST_CASE_ADD(tc_2_main_task, " TEST # 2 - Testing function _partition_create in special cases"),
    EU_TEST_CASE_ADD(tc_3_main_task, " TEST # 3 - Testing function _partition_create_at in special cases"),
    EU_TEST_CASE_ADD(tc_4_main_task, " TEST # 4 - Testing function _partition_destroy in special cases"),
    EU_TEST_CASE_ADD(tc_5_main_task, " TEST # 5 - Testing function _partition_alloc in special cases"),
    EU_TEST_CASE_ADD(tc_6_main_task, " TEST # 6 - Testing function _partition_alloc_zero in special cases"),
    EU_TEST_CASE_ADD(tc_7_main_task, " TEST # 7 - Testing function _partition_alloc_system in special cases"),
    EU_TEST_CASE_ADD(tc_8_main_task, " TEST # 8 - Testing function _partition_alloc_system_zero in special cases"),
    EU_TEST_CASE_ADD(tc_9_main_task, " TEST # 9 - Testing function _partition_extend in special cases"),
    EU_TEST_CASE_ADD(tc_10_main_task, " TEST # 10 - Testing function _partition_free in special cases"),
    EU_TEST_CASE_ADD(tc_11_main_task, " TEST # 11 - Testing function _partition_get_total_blocks, _partition_get_max_used_blocks, _partition_get_total_blocks in special cases"),
    EU_TEST_CASE_ADD(tc_12_main_task, " TEST # 12 - Testing function _partition_transfer in special cases"),
 EU_TEST_SUITE_END(suite_part2)

/* Add test suites  */
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_part2, " - Test part2 suite")
 EU_TEST_REGISTRY_END()

/*-------------------------------------------------------------------*/

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
    EU_ASSERT_REF_TC_MSG(tc_12_main_task, result == MQX_OK, "Test #12 Testing 12.05: _partition_free operation");

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

   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();

   _mqx_exit(0);
}

/* EOF */
