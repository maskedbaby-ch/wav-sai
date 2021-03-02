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
* $Version : 4.0.1.1$
* $Date    : Apr-26-2013$
*
* Comments:
*
*   This file contains definitions, extern and structs for
*   test suite suite_3 of Memory testing module.
*
*END************************************************************************/

#define MAIN_TASK           (10)
#define TEST_TASK           (11)
#define POOL_TYPE           (TRUE)
#define BLOCK_TYPE          (FALSE)
#define ALLOC_SIZE          (50)
#define POOL_SIZE           (1000)
#define POOL_INVALID_SIZE   (MQX_MIN_MEMORY_POOL_SIZE - 1)
#define INVALID_VALUE       (1)
#define INVALID_TASK_ID     (-1)

/* Main task prototype */
extern void main_task(uint32_t);
/* Test task prototype */
extern void test_task(uint32_t);

/* Global variables */
void       *pool_alloc_ptr;
void         *block_error;

#if MQX_USE_LWMEM_ALLOCATOR
LWMEM_POOL_STRUCT  ext_mem_pool;
_lwmem_pool_id     pool_error;
_lwmem_pool_id     ext_pool_id;
#else
_mem_pool_id       pool_error;
_mem_pool_id       ext_pool_id;
_task_number       task_num;
#endif /* MQX_USE_LWMEM_ALLOCATOR */


