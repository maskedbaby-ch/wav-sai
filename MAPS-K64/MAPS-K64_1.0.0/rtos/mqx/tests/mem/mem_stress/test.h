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
* $Version : 4.0.1.1$
* $Date    : May-04-2013$
*
* Comments:
*
*   This include file is used to define the demonstration constants
*
*END************************************************************************/

/* General defines */

#define TEST_TASK  10

#if PSP_MQX_CPU_IS_KINETIS_L
#define NUM_TASKS  4
#define POOL_SIZE  ((NUM_TASKS+1) * 512)
#define MAX_ALLOC  100
#define CYCLES_CNT 25
#else
#define NUM_TASKS  8
#define POOL_SIZE  ((NUM_TASKS+1) * 1024)
#define MAX_ALLOC  512
#define CYCLES_CNT 100
#endif

#define MAIN_TASK  (TEST_TASK - 1)

#define TEST_TIMEOUT_TICKS  ((NUM_TASKS+1) * 5)

extern void    test_task(uint32_t);
extern void    main_task(uint32_t);

LWEVENT_STRUCT              mem_test_event;
void                     *mem_ptrs[NUM_TASKS] = {0};
_mqx_uint                   alloc_count[NUM_TASKS] = {0};
_mqx_uint                   free_count[NUM_TASKS] = {0};

void                     *pool_alloc_ptr;
void                       *block_error;

#if MQX_USE_LWMEM_ALLOCATOR
_lwmem_pool_id              ext_pool_id;
_lwmem_pool_id              pool_error;
LWMEM_POOL_STRUCT           ext_mem_pool;

#else
_mem_pool_id                ext_pool_id;
_mem_pool_id                pool_error;

#endif

