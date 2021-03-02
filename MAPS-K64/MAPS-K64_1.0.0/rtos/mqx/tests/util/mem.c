/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: mem.c$
* $Version : 3.8.7.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file contains the source functions for the demo of the
*   time slicing functions of the kernel.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <mqx_prv.h>
#if !MQX_USE_IO_OLD
#include <stdio.h>
#endif
#include "util.h"

void memory_alloc_all
   (
      MEMORY_ALLOC_INFO_STRUCT_PTR info_ptr
   )
{ /* Body */
#if MQX_USE_LWMEM_ALLOCATOR
   KERNEL_DATA_STRUCT_PTR       kernel_data;
   LWMEM_BLOCK_STRUCT_PTR       block_ptr;
   LWMEM_POOL_STRUCT_PTR        pool_ptr;
   MEMORY_ALLOC_INFO_STRUCT_PTR mem_ptr;
   _mem_size                    block_size;

   _GET_KERNEL_DATA(kernel_data);

   /* initialize the memory alloc_info_struct */
   info_ptr->block_ptr = NULL;
#if MQX_ENABLE_USER_MODE
   pool_ptr = kernel_data->KD_USER_POOL;
#else
   pool_ptr = kernel_data->KERNEL_LWMEM_POOL;
#endif
   block_ptr = pool_ptr->POOL_FREE_LIST_PTR;

   while (block_ptr != NULL) {
      block_size  = block_ptr->BLOCKSIZE;
      block_size = (block_size / (PSP_MEMORY_ALIGNMENT + 1)) * (PSP_MEMORY_ALIGNMENT + 1);
      block_size -= sizeof(LWMEM_BLOCK_STRUCT);
#if MQX_ENABLE_USER_MODE
      mem_ptr  = (void*)_mem_alloc(block_size);
#else
      mem_ptr  = (void*)_mem_alloc_system_zero(block_size);
#endif
      if (mem_ptr != NULL) {
         mem_ptr->block_ptr  = info_ptr->block_ptr;
         info_ptr->block_ptr = mem_ptr;
      } /* Endif */
      block_ptr = pool_ptr->POOL_FREE_LIST_PTR;
   } /* Endwhile */

#else
   KERNEL_DATA_STRUCT_PTR       kernel_data;
   STOREBLOCK_STRUCT_PTR        block_ptr;
   MEMORY_ALLOC_INFO_STRUCT_PTR mem_ptr;
   _mem_size                    block_size;

   _GET_KERNEL_DATA(kernel_data);

   /* initialize the memory alloc_info_struct */
   info_ptr->block_ptr = NULL;
   block_ptr = kernel_data->KD_POOL.POOL_FREE_LIST_PTR;
   while (block_ptr != NULL) {
      block_size  = block_ptr->BLOCKSIZE;
      block_size -= FIELD_OFFSET(STOREBLOCK_STRUCT,USER_AREA);
      mem_ptr  = (MEMORY_ALLOC_INFO_STRUCT_PTR)_mem_alloc_system_zero(block_size);
      if (mem_ptr != NULL) {
         mem_ptr->block_ptr  = info_ptr->block_ptr;
         info_ptr->block_ptr = mem_ptr;
      } /* Endif */
      block_ptr = kernel_data->KD_POOL.POOL_FREE_LIST_PTR;
   } /* Endwhile */
#endif

} /* Endbody */


void memory_free_all
   (
      MEMORY_ALLOC_INFO_STRUCT_PTR info_ptr
   )
{ /* Body */
   MEMORY_ALLOC_INFO_STRUCT_PTR mem_ptr;

   mem_ptr = (MEMORY_ALLOC_INFO_STRUCT_PTR)info_ptr->block_ptr;
   while (mem_ptr != NULL) {
      info_ptr->block_ptr = mem_ptr->block_ptr;
      _mem_free(mem_ptr);
      mem_ptr = (MEMORY_ALLOC_INFO_STRUCT_PTR)info_ptr->block_ptr;
   } /* Endwhile */

} /* Endbody */

void print_kernel_lwmem_pool
   (
      void
   )
{ /* Body */
#if MQX_USE_LWMEM
   KERNEL_DATA_STRUCT_PTR kernel_data;

   _GET_KERNEL_DATA(kernel_data);
   print_lwmem_pool((_lwmem_pool_id)kernel_data->KERNEL_LWMEM_POOL);
#endif
} /* Endbody */

void print_lwmem_pool
   (
      _lwmem_pool_id pool_id
   )
{ /* Body */
   LWMEM_POOL_STRUCT_PTR  mem_pool_ptr = (LWMEM_POOL_STRUCT_PTR)pool_id;
   LWMEM_BLOCK_STRUCT_PTR block_ptr;

   _int_disable();
   printf("***** Pool: 0x%lx  start: 0x%08lx  end: 0x%08lx  free: 0x%08lx\n", mem_pool_ptr,
      mem_pool_ptr->POOL_ALLOC_START_PTR, mem_pool_ptr->POOL_ALLOC_END_PTR,
      mem_pool_ptr->POOL_FREE_LIST_PTR);
   block_ptr = (LWMEM_BLOCK_STRUCT_PTR)mem_pool_ptr->POOL_ALLOC_START_PTR;
   while ((unsigned char *)block_ptr < (unsigned char *)mem_pool_ptr->POOL_ALLOC_END_PTR){
      if (block_ptr->POOL != pool_id) {
         printf("Bad block pool id 0x%x\n", block_ptr);
         break;
      } /* Endif */
      printf("  block: 0x%08lx   size: 0x%08lx  pool: 0x%08lx  nex: 0x%08lx\n",block_ptr,
         block_ptr->BLOCKSIZE, block_ptr->POOL, block_ptr->U.NEXTBLOCK);
      block_ptr = (LWMEM_BLOCK_STRUCT_PTR)((unsigned char *)block_ptr +
         block_ptr->BLOCKSIZE);
   } /* Endwhile */

   printf("\n    Free list:\n");
   block_ptr = (LWMEM_BLOCK_STRUCT_PTR)mem_pool_ptr->POOL_FREE_LIST_PTR;
   while (block_ptr) {
      if (((void *)block_ptr < mem_pool_ptr->POOL_ALLOC_START_PTR) ||
          ((void *)block_ptr > mem_pool_ptr->POOL_ALLOC_END_PTR) ||
          (block_ptr->POOL != mem_pool_ptr) ||
          (block_ptr->U.NEXTBLOCK &&
          (block_ptr->U.NEXTBLOCK <= (void *)((unsigned char *)block_ptr +
          block_ptr->BLOCKSIZE))))
      {
         printf("Bad block pool id 0x%x\n", block_ptr);
         break;
      } /* Endif */
      printf("  block: 0x%08lx   size: 0x%08lx  pool: 0x%08lx  nex: 0x%08lx\n",block_ptr,
         block_ptr->BLOCKSIZE, block_ptr->POOL, block_ptr->U.NEXTBLOCK);
      block_ptr = (LWMEM_BLOCK_STRUCT_PTR)block_ptr->U.NEXTBLOCK;
   } /* Endwhile */

   _int_enable();

} /* Endbody */


_mem_size get_free_mem
   (
      void
   )
{ /* Body */
   _mem_size                    size = 0;

#if MQX_USE_LWMEM_ALLOCATOR
   KERNEL_DATA_STRUCT_PTR       kernel_data;
   LWMEM_BLOCK_STRUCT_PTR       block_ptr;
   LWMEM_POOL_STRUCT_PTR        pool_ptr;

   _GET_KERNEL_DATA(kernel_data);

   /* initialize the memory alloc_info_struct */
   pool_ptr = kernel_data->KERNEL_LWMEM_POOL;
   block_ptr = pool_ptr->POOL_FREE_LIST_PTR;

   while (block_ptr != NULL) {
      size += block_ptr->BLOCKSIZE;
      block_ptr = block_ptr->U.NEXTBLOCK;
   } /* Endwhile */
#if MQX_ENABLE_USER_MODE
   pool_ptr = kernel_data->KD_USER_POOL;
   block_ptr = pool_ptr->POOL_FREE_LIST_PTR;

   while (block_ptr != NULL) {
      size += block_ptr->BLOCKSIZE;
      block_ptr = block_ptr->U.NEXTBLOCK;
   } /* Endwhile */
#endif


#else
   KERNEL_DATA_STRUCT_PTR kernel_data;
   STOREBLOCK_STRUCT_PTR  block_ptr;
   MEMPOOL_STRUCT_PTR     mem_pool_ptr;

   _GET_KERNEL_DATA(kernel_data);
   mem_pool_ptr = &kernel_data->KD_POOL;

   block_ptr = mem_pool_ptr->POOL_FREE_LIST_PTR;

   while ( block_ptr ) {
      size += block_ptr->BLOCKSIZE;
      block_ptr = (STOREBLOCK_STRUCT_PTR)NEXT_FREE(block_ptr);
   } /* Endwhile */
#endif

   return size;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : memory_get_owner
* Returned Value   : return the task id of the task that owns the memory block
* Comments         :
*   get the owner of the memory block
*   - [IN] addr: The pointer of memory block.
*
*END*--------------------------------------------------------------------*/
_task_id memory_get_owner(void *addr)
{
   _task_id                result = (_task_id)0;
   KERNEL_DATA_STRUCT_PTR  kernel_data;

#if MQX_USE_LWMEM_ALLOCATOR
   LWMEM_BLOCK_STRUCT_PTR  block_ptr;

   _GET_KERNEL_DATA(kernel_data);
   block_ptr = GET_LWMEMBLOCK_PTR(addr);
   result = BUILD_TASKID(kernel_data->INIT.PROCESSOR_NUMBER, block_ptr->U.S.TASK_NUMBER);
#else
   STOREBLOCK_STRUCT_PTR   block_ptr;

   _GET_KERNEL_DATA(kernel_data);
   block_ptr = GET_MEMBLOCK_PTR(addr);
   result = BUILD_TASKID(kernel_data->INIT.PROCESSOR_NUMBER, block_ptr->TASK_NUMBER);
#endif

   return result;
}
/* EOF */
