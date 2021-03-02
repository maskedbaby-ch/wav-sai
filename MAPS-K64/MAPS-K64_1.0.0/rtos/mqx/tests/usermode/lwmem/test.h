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
* $Version : 4.0.2.0$
* $Date    : Jun-14-2013$
*
* Comments:
*
*   This file contains definitions, prototypes and global variables for
*   test suite suite_usr_lwmem of Usermode testing module.
*
*END************************************************************************/

#define SUPER_TASK      (10)
#define MAIN_TASK       (11)
#define SUB_TASK        (12)
#define PRIV_TASK       (13)
#define EVENT_MASK_MAIN (1)
#define EVENT_MASK_PRIV (4)
#define ALLOC_SIZE      (100)
#define POOL_SIZE       (1024)
#define INVALID_VALUE   (1)

/* External memory pool */
USER_RW_ACCESS LWMEM_POOL_STRUCT mem_pool;
/* Memory space of external pool */
USER_RW_ACCESS unsigned char mem_space[POOL_SIZE];
/* This block is used to transfer between tasks */
USER_RW_ACCESS void   *global_mem_ptr;
/* Global lightweight event */
USER_RO_ACCESS LWEVENT_STRUCT isr_lwevent;
/* Functions' returned value */
_mqx_uint result, ret_val, enable_tc3_null_test;

/* Task prototypes */
void super_task(uint32_t);
void main_task(uint32_t);
void sub_task(uint32_t);
void priv_task(uint32_t);
