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
*   test suite suite_4 of Memory testing module.
*
*END************************************************************************/

#define MAIN_TASK               (10)
#define TEST_TASK               (11)
#define ALLOC_SIZE              (50)
#define POOL_SIZE               (1000)
#define VALID_SIZE              (ALLOC_SIZE / 2)
#define INVALID_SIZE            (ALLOC_SIZE + 1)
#define INVALID_EXTENSION_SIZE  (3 * GENERAL_MEM_MIN_STORAGE_SIZE - 1)
#define VALID_EXTENSION_SIZE    (4 * GENERAL_MEM_MIN_STORAGE_SIZE)
#define INVALID_VALUE           (1)

#if MQX_USE_LWMEM_ALLOCATOR
LWMEM_POOL_STRUCT  ext_mem_pool;
void              *block_error;
_lwmem_pool_id     pool_error;
#endif

/* Main task prototype */
extern void main_task(uint32_t);
/* Test task prototype */
extern void test_task(uint32_t);
