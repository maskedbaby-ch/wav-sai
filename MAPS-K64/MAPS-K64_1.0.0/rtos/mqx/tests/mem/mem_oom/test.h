/**HEADER********************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
* All Rights Reserved
*
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
* $Version : 4.0.1$
* $Date    : Apr-2-2013$
*
* Comments:
*Requirement ID: MEM097, MEM004, MEM008, MEM012, MEM016, MEM020, MEM024, MEM028, MEM032, MEM036, MEM040, MEM44
* LWMEM002,LWMEM004,LWMEM006,LWMEM008,LWMEM010,LWMEM012,LWMEM014,LWMEM016,LWMEM018,LWMEM038,LWMEM041
*   This include file is used to define the demonstration constants
*
*END************************************************************************/

/* General defines */
#define MAIN_TASK  10
#define MAIN_TASK_STACK 3000
#define TEST_ALLOC_SIZE 2000
#define MEM_EXT_POOL_SIZE    1024
#define TEST_ALLOC_ALIGN_SIZE 4


/* Structure and typedefs for the test program */
typedef struct mem_pointer_list_struct
{
   unsigned char                                DATA[100];
   struct mem_pointer_list_struct      *PREV;
} MEM_POINTER_LIST_STRUCT, * MEM_POINTER_LIST_STRUCT_PTR;


#if MQX_USE_LWMEM_ALLOCATOR
    _lwmem_pool_id          pool_error;
    void                   *block_error;
    _lwmem_pool_id          ext_pool_id;
#else
    _mem_pool_id            ext_pool_id;
    _mem_pool_id            pool_error;
#endif

#if MQX_USE_LWMEM_ALLOCATOR
    #define GENERAL_MEM_TEST    _lwmem_test(&pool_error, &block_error)
#else
    #define GENERAL_MEM_TEST    _mem_test_all(&pool_error)
#endif

extern void    main_task(uint32_t);


