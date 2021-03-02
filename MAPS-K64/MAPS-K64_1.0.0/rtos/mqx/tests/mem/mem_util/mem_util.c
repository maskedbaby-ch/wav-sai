/**HEADER*******************************************************************
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
* $Date    : Apr-26-2013$
*
* Comments:
*
*   This file contains the source functions for the Mem testing utility,
*   includes of get/set memory's components parameters.
*
*END************************************************************************/
#include <mqx_inc.h>
#include <bsp.h>
#include "mem_util.h"

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_pool_free_list_ptr
* Return Value  : pointer to next block
* Comments      : gets the pointer to next block
*
*END*----------------------------------------------------------------------*/
void *get_pool_free_list_ptr(void *pool_ptr)
{
#if MQX_USE_LWMEM_ALLOCATOR
    LWMEM_POOL_STRUCT_PTR mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR)pool_ptr;
    LWMEM_BLOCK_STRUCT_PTR test_block_ptr;

    /* Get the first memory block in the free memory list */
    test_block_ptr = (LWMEM_BLOCK_STRUCT_PTR)mem_pool_ptr->POOL_FREE_LIST_PTR;
    return test_block_ptr->U.NEXTBLOCK;
#else
    MEMPOOL_STRUCT_PTR mem_pool_ptr = (MEMPOOL_STRUCT_PTR)pool_ptr;
    STOREBLOCK_STRUCT_PTR test_block_ptr;

    /* Get the first memory block in the free memory list */
    test_block_ptr = mem_pool_ptr->POOL_FREE_LIST_PTR;
    /* Change the task number to another value */
    return (test_block_ptr->USER_AREA);
#endif
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_pool_free_list_ptr
* Return Value  : void
* Comments      : sets the pointer to next block
*
*END*----------------------------------------------------------------------*/
void set_pool_free_list_ptr(void *pool_ptr, void *temp_ptr)
{
#if MQX_USE_LWMEM_ALLOCATOR
    LWMEM_POOL_STRUCT_PTR   mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR)pool_ptr;
    LWMEM_BLOCK_STRUCT_PTR  block_ptr;

    _int_disable();
    /* Get the first memory block in the free memory list */
    block_ptr = (LWMEM_BLOCK_STRUCT_PTR)mem_pool_ptr->POOL_FREE_LIST_PTR;
    block_ptr->U.NEXTBLOCK = temp_ptr;
    _int_enable();
#else
    MEMPOOL_STRUCT_PTR mem_pool_ptr = (MEMPOOL_STRUCT_PTR)pool_ptr;
    STOREBLOCK_STRUCT_PTR test_block_ptr;

    _int_disable();
    /* Get the first memory block in the free memory list */
    test_block_ptr = mem_pool_ptr->POOL_FREE_LIST_PTR;
    /* Change the task number to another value */
    test_block_ptr->USER_AREA = temp_ptr;
    _int_enable();
#endif
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_pool_valid_field
* Returned Value   : _mqx_uint
* Comments         :
*   Set the VALID field of memory pool.
*   - [IN] pool_ptr: Indicate the memory pool that will be changed
    - [IN] value: Value to be set.
*
*END*--------------------------------------------------------------------*/
_mqx_uint get_pool_valid_field(void *pool_ptr)
{
#if MQX_USE_LWMEM_ALLOCATOR
    LWMEM_POOL_STRUCT_PTR   mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR)pool_ptr;
    return mem_pool_ptr->VALID;

#else
    MEMPOOL_STRUCT_PTR      mem_pool_ptr = (MEMPOOL_STRUCT_PTR)pool_ptr;
    STOREBLOCK_STRUCT_PTR   block_ptr = mem_pool_ptr->POOL_FREE_LIST_PTR;

    /* Return the current checksum of memory pool */
    return block_ptr->CHECKSUM;
#endif
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_pool_valid_field
* Returned Value   : None
* Comments         :
*   Set the VALID field of memory pool.
*   - [IN] pool_ptr: Indicate the memory pool that will be changed
    - [IN] value: Value to be set.
*
*END*--------------------------------------------------------------------*/
void set_pool_valid_field(void *pool_ptr, _mqx_uint value)
{
#if MQX_USE_LWMEM_ALLOCATOR
    LWMEM_POOL_STRUCT_PTR  mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR)pool_ptr;
    /* Save the valid field of memory pool */
    mem_pool_ptr->VALID = value;
#else
    MEMPOOL_STRUCT_PTR      mem_pool_ptr = (MEMPOOL_STRUCT_PTR)pool_ptr;
    STOREBLOCK_STRUCT_PTR   block_ptr = mem_pool_ptr->POOL_FREE_LIST_PTR;

    /* Return the current checksum of memory pool */
    block_ptr->CHECKSUM = value;
#endif
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_kernel_error_block
* Returned Value   : pointer
* Comments         :
*   Get the current error block of kernel memory pool
*
*END*--------------------------------------------------------------------*/
void *get_kernel_error_block(void)
{
#if MQX_USE_LWMEM_ALLOCATOR
    return NULL;
#else
    KERNEL_DATA_STRUCT_PTR kernel_data;

    /* Get kernel data struct */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the current kernel's error block */
    return (void *)kernel_data->KD_POOL.POOL_BLOCK_IN_ERROR;
#endif
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_kernel_error_block
* Returned Value   : None
* Comments         :
*   Set the current error block of kernel memory pool
*   - [IN] error_block_ptr: The value to be set.
*
*END*--------------------------------------------------------------------*/
void set_kernel_error_block(void *error_block_ptr)
{
#if MQX_USE_LWMEM_ALLOCATOR
#else
    KERNEL_DATA_STRUCT_PTR kernel_data;

    /* Get kernel data struct */
    _GET_KERNEL_DATA(kernel_data);
    /* Prevent another task from accessing memory */
    _int_disable();
    /* Set the current kernel's error block */
    kernel_data->KD_POOL.POOL_BLOCK_IN_ERROR = (STOREBLOCK_STRUCT_PTR)error_block_ptr;
    /* Enable all interrupts */
    _int_enable();
#endif
}

#if MQX_USE_LWMEM_ALLOCATOR

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_pool_storage_ptr
* Return Value  : pointer to kernel component
* Comments      : gets the pointer to kernel component
*
*END*----------------------------------------------------------------------*/
void *get_pool_storage_ptr(void *pool_ptr)
{
    LWMEM_POOL_STRUCT_PTR   mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR)pool_ptr;
    LWMEM_BLOCK_STRUCT_PTR  block_ptr;

    /* Get the first memory block in the free memory list */
    block_ptr = (LWMEM_BLOCK_STRUCT_PTR)mem_pool_ptr->POOL_ALLOC_START_PTR;
    return block_ptr->POOL;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_pool_storage_ptr
* Return Value  : pointer to kernel component
* Comments      : sets the pointer to kernel component
*
*END*----------------------------------------------------------------------*/
void set_pool_storage_ptr(void *pool_ptr, void *ptr)
{
    LWMEM_POOL_STRUCT_PTR   mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR) pool_ptr;
    LWMEM_BLOCK_STRUCT_PTR  block_ptr;

    _int_disable();
    /* Get the first memory block in the free memory list */
    block_ptr = (LWMEM_BLOCK_STRUCT_PTR)mem_pool_ptr->POOL_ALLOC_START_PTR;
    /* Set the task number of memory pool */
    block_ptr->POOL = ptr;
    _int_enable();
}

#else
/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_block_taskno
* Returned Value   : _task_number
* Comments         :
*   Get the task number value of memory block.
*   - [IN] mem_ptr: The memory component that will be polled
*   - [IN] is_pool: Indicate the component is memory pool or memory block
*
*END*--------------------------------------------------------------------*/
_task_number get_block_taskno(void *mem_ptr, bool is_pool)
{
    STOREBLOCK_STRUCT_PTR block_ptr;

    if(is_pool)
    {
        /* Get the first memory block in the pool's free list */
        block_ptr = ((MEMPOOL_STRUCT_PTR)mem_ptr)->POOL_FREE_LIST_PTR;
    }
    else
    {
        /* Get the structure of memory block */
        block_ptr = GET_MEMBLOCK_PTR(mem_ptr);
    }

    /* Return the current task number of memory block */
    return (block_ptr->TASK_NUMBER);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_block_taskno
* Returned Value   : None
* Comments         :
*   Set the task number value of memory block
*   - [IN] mem_ptr: The memory component that will be modified
*   - [IN] task_num: The value to be set.
*   - [IN] is_pool: Indicate the component is memory pool or memory block
*
*END*--------------------------------------------------------------------*/
void set_block_taskno(void *mem_ptr, _task_number task_num, bool is_pool)
{
    STOREBLOCK_STRUCT_PTR block_ptr;

    if(is_pool)
    {
        /* Get the first memory block in the pool's free list */
        block_ptr = ((MEMPOOL_STRUCT_PTR)mem_ptr)->POOL_FREE_LIST_PTR;
    }
    else
    {
        /* Get the structure of memory block */
        block_ptr = GET_MEMBLOCK_PTR(mem_ptr);
    }

    /* Prevent another task from accessing memory */
    _int_disable();
    /* Set the task number of memory block */
    block_ptr->TASK_NUMBER = task_num;
    /* Enable all interrupts */
    _int_enable();
}



/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_mem_component
* Returned Value   : None
* Comments         :
*   Set the VALID field of kernel memory component.
*   - [IN] value: The value to be set.
*
*END*--------------------------------------------------------------------*/
void set_mem_component(_mqx_uint value)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Disable all interrupts */
    _int_disable();
    /* Set the VALID field of kernel mem component */
    kernel_data->MEM_COMP.VALID = value;
    /* Enable interrupts */
    _int_enable();
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_block_checksum
* Returned Value   : _mqx_uint
* Comments         :
*   Get the checksum value of memory block
*   - [IN] mem_ptr: The memory component that will be polled
*   - [IN] is_pool: Indicate the component is memory pool or memory block
*
*END*--------------------------------------------------------------------*/
_mqx_uint get_block_checksum(void *mem_ptr)
{
    STOREBLOCK_STRUCT_PTR block_ptr;

    /* Get the structure of memory block */
    block_ptr = GET_MEMBLOCK_PTR(mem_ptr);
    /* Return the current checksum of memory pool */
    return (block_ptr->CHECKSUM);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_block_checksum
* Returned Value   : None
* Comments         :
*   Set the checksum value of memory block
*   - [IN] mem_ptr: The memory component that will be modified
*   - [IN] checksum: The value to be set.
*   - [IN] is_pool: Indicate the component is memory pool or memory block
*
*END*--------------------------------------------------------------------*/
void set_block_checksum(void *mem_ptr, _mqx_uint checksum)
{
    STOREBLOCK_STRUCT_PTR  block_ptr;

    /* Get the structure of memory block */
    block_ptr = GET_MEMBLOCK_PTR(mem_ptr);
    /* Prevent another task from accessing memory */
    _int_disable();
    /* Set the checksum of memory block */
    block_ptr->CHECKSUM = checksum;
    /* Enable all interrupts */
    _int_enable();
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_kernel_pool_queue
* Returned Value   : pointer
* Comments         :
*   Get the current kernel memory pool queue value.
*
*END*--------------------------------------------------------------------*/
void *get_kernel_pool_queue(void)
{
    KERNEL_DATA_STRUCT_PTR  kernel_data;
    QUEUE_STRUCT_PTR        queue_ptr;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the memory queue */
    queue_ptr = (QUEUE_STRUCT_PTR) &kernel_data->MEM_COMP.POOLS;
    /* Save the current queue pointer */
    return (queue_ptr->NEXT);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_kernel_pool_queue
* Returned Value   : None
* Comments         :
*   Set the current kernel memory pool queue value.
*   - [IN] pool_queue_ptr: The value to be set.
*
*END*--------------------------------------------------------------------*/
void set_kernel_pool_queue(void *pool_queue_ptr)
{
    KERNEL_DATA_STRUCT_PTR  kernel_data;
    QUEUE_STRUCT_PTR        queue_ptr;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the memory queue */
    queue_ptr = (QUEUE_STRUCT_PTR) &kernel_data->MEM_COMP.POOLS;
    /* Disable all interrupts */
    _int_disable();
    /* Set the current queue pointer */
    queue_ptr->NEXT = pool_queue_ptr;
    /* Enable interrupts */
    _int_enable();
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_block_physical
* Returned Value   : pointer
* Comments         :
*   Makes the pool's physical block to be invalid.
*   - [IN] pool_ptr: Indicates the memory pool that will be polled
*
*END*--------------------------------------------------------------------*/
void *get_block_physical(void *pool_ptr)
{
    MEMPOOL_STRUCT_PTR mem_pool_ptr = (MEMPOOL_STRUCT_PTR)pool_ptr;

    /* Save the physical block pointer */
    return (mem_pool_ptr->POOL_PTR);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_block_physical
* Returned Value   : None
* Comments         :
*   Resets the pool's physical block to original state
*   - [IN] pool_ptr: Indicates the memory pool that will be invalidated
*   - [IN] phys_block_ptr: The value to be set.
*
*END*--------------------------------------------------------------------*/
void set_block_physical(void *pool_ptr, void *phys_block_ptr)
{
    MEMPOOL_STRUCT_PTR mem_pool_ptr = (MEMPOOL_STRUCT_PTR)pool_ptr;

    /* Disable all interrupts */
    _int_disable();
    /* Set the physical block pointer */
    mem_pool_ptr->POOL_PTR = (MEMPOOL_STRUCT_PTR)phys_block_ptr;
    /* Enable interrupts */
    _int_enable();
}

#endif
