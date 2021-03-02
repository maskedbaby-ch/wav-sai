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
#ifndef __TEST_H__
#define __TEST_H__
#define MAIN_TASK                (10)
#define TEST_TASK                (11)
#define TEST_ALLOC_SIZE          (200)
#define TEST_ALLOC_ALIGN         (2)
#define TEST_INVALID_CHECKSUM   (-100)

typedef struct MEM_LIST_S
{
  void              *pdata;
  struct MEM_LIST_S *prev;
} MEM_LIST, * MEM_LIST_PTR;

/* Main task prototype */
extern void     main_task(uint32_t);
_mqx_uint       test_memory(MEM_LIST_PTR, _mqx_uint);

void                      *block_error;

#if MQX_USE_LWMEM_ALLOCATOR
_lwmem_pool_id              ext_pool_id;
_lwmem_pool_id              pool_error;
LWMEM_POOL_STRUCT           ext_mem_pool;

#else
_mem_pool_id                ext_pool_id;
_mem_pool_id                pool_error;
#endif


#endif
