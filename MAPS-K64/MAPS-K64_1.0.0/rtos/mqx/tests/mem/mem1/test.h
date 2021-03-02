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
* $FileName: test.h$
* $Version : 4.0.1.1$
* $Date    : May-10-2013$
*
* Comments:
*
*   This file contains definitions, extern and structs for
*   test suite suite_1 of Memory testing module.
*
*END************************************************************************/
/* General defines */
#define MAIN_TASK                (10)
#define TEST_TASK                (11)
#define TEST_ALLOC_SIZE          (200)
#define TEST_ENDIAN_ARRAY_SIZE   (100)
#define TEST_ENDIAN_MAX_OBJ_SIZE (9)
#define MEM_EXT_SIZE             (1024)
#define FALLOUT_SIZE             (16)
#define ALLOC_SIZE               (TEST_SIZE + 4)
#define TOTAL_SIZE               (ALLOC_SIZE + FALLOUT_SIZE)
/* _mem_copy and _mem_zero test defines */
#if defined(BSP_SDSIM860) || defined(BSP_SDSIM360)
#define TEST_SIZE   (256)
#else
#define TEST_SIZE   (2048)
#endif


/* Main task prototype */
extern void     main_task(uint32_t);
/* Test task prototype */
extern void     test_task(uint32_t);
/* Compare same memory with different endian type */
extern bool  test_endian(unsigned char *, unsigned char *, uint32_t);

/* Structure and typedefs for the test program */
typedef struct mem_pointer_list_struct
{
   unsigned char DATA[100];
   struct mem_pointer_list_struct      *PREV;
} MEM_POINTER_LIST_STRUCT, * MEM_POINTER_LIST_STRUCT_PTR;

/* Global variables */
KERNEL_DATA_STRUCT_PTR kernel_data;
unsigned char                 *to_ptr;
unsigned char                 *from_ptr;
void                *mem_alloc_pool_ptr;
void                *global_mem_ptr;
_task_id               global_test_task_id;
_task_id               global_main_task_id;
_task_id               system_task_id;
unsigned char                  From[ALLOC_SIZE];
unsigned char                  To[TOTAL_SIZE];

#if MQX_USE_LWMEM_ALLOCATOR
LWMEM_BLOCK_STRUCT_PTR block_ptr;
_lwmem_pool_id         ext_pool_id;

LWMEM_POOL_STRUCT      ext_mem_pool;
_lwmem_pool_id         pool_error;
void                  *block_error;

#else
STOREBLOCK_STRUCT_PTR  block_ptr;
_mem_pool_id           ext_pool_id;
unsigned char                  global_memory_extension_buffer[ MEM_EXT_SIZE ];
unsigned char                  global_tns_test_loc;
unsigned char                  global_endian_swap_def[TEST_ENDIAN_MAX_OBJ_SIZE+1];
unsigned char                  global_endian_test_array[TEST_ENDIAN_ARRAY_SIZE];
unsigned char                  global_endian_test_array_copy[TEST_ENDIAN_ARRAY_SIZE];
#endif
