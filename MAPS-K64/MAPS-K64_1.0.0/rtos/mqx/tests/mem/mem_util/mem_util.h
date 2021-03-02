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
* $FileName: test.h$
* $Version : 4.0.1.0$
* $Date    : May-04-2013$
*
* Comments:
*
*   This file contains the include information for Mem testing utility
*   function prototypes
*
*END************************************************************************/
#ifndef _mem_util_h_
#define _mem_util_h_

#define POOL_TYPE       (TRUE)
#define BLOCK_TYPE      (FALSE)

/* get the pointer to the default pool in kernel */
#if MQX_USE_LWMEM_ALLOCATOR
#define SYSTEM_POOL_ID                          _lwmem_get_system_pool_id()
#else
#define SYSTEM_POOL_ID                          _mem_get_system_pool_id()
#endif

/* general error code for memory pool invalid */
#if MQX_USE_LWMEM_ALLOCATOR
#define GENERAL_MEM_INVALID_ERROR_CODE          MQX_LWMEM_POOL_INVALID
#else
#define GENERAL_MEM_INVALID_ERROR_CODE          MQX_INVALID_CHECKSUM
#endif

/* The minimum memory storage */
#if MQX_USE_LWMEM_ALLOCATOR
#define GENERAL_MEM_MIN_STORAGE_SIZE            LWMEM_MIN_MEMORY_STORAGE_SIZE
#else
#define GENERAL_MEM_MIN_STORAGE_SIZE            MQX_MIN_MEMORY_STORAGE_SIZE
#endif

/* The size of a memory block structure */
#if MQX_USE_LWMEM_ALLOCATOR
#define GENERAL_SIZE_OF_BLOCK_STRUCT            sizeof(LWMEM_BLOCK_STRUCT)
#else
#define GENERAL_SIZE_OF_BLOCK_STRUCT            sizeof(STOREBLOCK_STRUCT)
#endif

/* Get the task number that owns the memory block */
#if MQX_USE_LWMEM_ALLOCATOR
#define GENERAL_MEM_TASK_NUMBER(block)          (((LWMEM_BLOCK_STRUCT_PTR)(block))->U.S.TASK_NUMBER)
#else
#define GENERAL_MEM_TASK_NUMBER(block)          (((STOREBLOCK_STRUCT_PTR)(block))->TASK_NUMBER)
#endif

/* Get the block size of the memory block */
#if MQX_USE_LWMEM_ALLOCATOR
#define GENERAL_MEM_GET_BLOCK_SIZE(block)       (((LWMEM_BLOCK_STRUCT_PTR)(block))->BLOCKSIZE)
#else
#define GENERAL_MEM_GET_BLOCK_SIZE(block)       (((STOREBLOCK_STRUCT_PTR)(block))->BLOCKSIZE)
#endif

/* Get the block header from block user area */
#if MQX_USE_LWMEM_ALLOCATOR
#define GENERAL_MEM_GET_BLOCK_PTR(addr)         GET_LWMEMBLOCK_PTR(addr)
#else
#define GENERAL_MEM_GET_BLOCK_PTR(addr)         GET_MEMBLOCK_PTR(addr)
#endif

/* Get the block header from block user area */
#if MQX_USE_LWMEM_ALLOCATOR
#define GENERAL_MEM_FREE_LIST_PTR(pool)         (((LWMEM_POOL_STRUCT_PTR)(pool))->POOL_FREE_LIST_PTR)
#else
#define GENERAL_MEM_FREE_LIST_PTR(pool)         (((MEMPOOL_STRUCT_PTR)(pool))->POOL_FREE_LIST_PTR)
#endif

/* Get the size of memory block */
#if MQX_USE_LWMEM_ALLOCATOR
#define GENERAL_MEM_GET_SIZE(ptr)               _lwmem_get_size(ptr)
#else
#define GENERAL_MEM_GET_SIZE(ptr)               _mem_get_size(ptr)
#endif

/* Create the memory pool */
#if MQX_USE_LWMEM_ALLOCATOR
#define GENERAL_MEM_CREATE_POOL(start, size)    _lwmem_create_pool(&ext_mem_pool, start, size)
#else
#define GENERAL_MEM_CREATE_POOL(start, size)    _mem_create_pool(start, size)
#endif


/* test system memory pool */
#if MQX_USE_LWMEM_ALLOCATOR
#define GENERAL_MEM_TEST                        _lwmem_test(&pool_error, &block_error)
#else
#define GENERAL_MEM_TEST                        _mem_test()
#endif

/* test memory for specific pool */
#if MQX_USE_LWMEM_ALLOCATOR
#define GENERAL_MEM_TEST_POOL(p)                _lwmem_test(&pool_error, &block_error)
#else
#define GENERAL_MEM_TEST_POOL(p)                _mem_test_pool(p)
#endif

/* (lw)mem_alloc_system_align(_from) */
#if MQX_USE_LWMEM_ALLOCATOR
#define GENERAL_MEM_ALLOC_SYSTEM_ALIGN(p,s)   _lwmem_alloc_system_align(p,s)
#define GENERAL_MEM_ALLOC_SYSTEM_ALIGN_FROM(p,s,a) _lwmem_alloc_system_align_from(p,s,a)
#else
#define GENERAL_MEM_ALLOC_SYSTEM_ALIGN(p,s)   _mem_alloc_system_align(p,s)
#define GENERAL_MEM_ALLOC_SYSTEM_ALIGN_FROM(p,s,a) _mem_alloc_system_align_from(p,s,a)
#endif

/* test all memory */
#if MQX_USE_LWMEM_ALLOCATOR
#define GENERAL_MEM_TEST_ALL(r, p, b)           { r = _lwmem_test(&p, &b); }
#else
#define GENERAL_MEM_TEST_ALL(r, p, b) do { \
 r = _mem_test_all(&p); \
 (r == MQX_OK) ? (b = NULL) : (b = _mem_get_error_pool(p)); \
 }while(0)
#endif

/* variable */
#if MQX_USE_LWMEM_ALLOCATOR
extern LWMEM_POOL_STRUCT  ext_mem_pool;
extern void            *block_error;
extern _lwmem_pool_id     pool_error;
#endif

/* function prototype */

/* Get the pointer to next block */
extern void   *get_pool_free_list_ptr(void *);
/* Set the pointer to next block */
extern void set_pool_free_list_ptr(void *, void *);
/* Get pool VALID field */
extern _mqx_uint get_pool_valid_field(void *);
/* Set pool VALID field */
extern void set_pool_valid_field(void *, _mqx_uint);
/* Get the current error block of kernel memory pool */
extern void   *get_kernel_error_block(void);
/* Set the current error block of kernel memory pool */
extern void set_kernel_error_block(void *);

#if MQX_USE_LWMEM_ALLOCATOR
/* Get the pointer to kernel component */
extern void   *get_pool_storage_ptr(void *);
/* Set the pointer to kernel component */
extern void set_pool_storage_ptr(void *, void *);
#else
/* Get the task number value of memory block */
extern _task_number get_block_taskno(void *, bool);
/* Set the task number value of memory block */
extern void set_block_taskno(void *, _task_number, bool);
/* Get the checksum value of memory block */
extern _mqx_uint get_block_checksum(void *);
/* Set the checksum value of memory block */
extern void set_block_checksum(void *, _mqx_uint);
/* Get the current kernel memory pool queue value */
extern void   *get_kernel_pool_queue(void);
/* Set the current kernel memory pool queue value */
extern void set_kernel_pool_queue(void *);
/* Get the pool's physical block value */
extern void   *get_block_physical(void *);
/* Set the pool's physical block value */
extern void set_block_physical(void *, void *);
/* Set the VALID field of kernel memory component */
extern void set_mem_component(_mqx_uint value);

#endif /* MQX_USE_LWMEM_ALLOCATOR */

#endif /* _mem_util_h_ */
