/**HEADER********************************************************************
*
* Copyright (c) 2008-2013 Freescale Semiconductor;
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
* $FileName: util.h$
* $Version : 3.8.7.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file contains the include information for kernel test utility
*   functions.
*
*END************************************************************************/
#ifndef _util_h_
#define _util_h_

#include "mem_prv.h"
#include "mqx_prv.h"
#include "lwmem.h"
#include "lwmem_prv.h"
typedef struct memory_alloc_info_struct
{
   void   *block_ptr;
} MEMORY_ALLOC_INFO_STRUCT, * MEMORY_ALLOC_INFO_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif


/*
BEART add custom define - LINKER_USED and its value represent current (running)
target (intram, ...). These values are hard coded in tapp_settings.mak makefile,
so for easy of use in tests there are pre-defined macros.
*/
#define LINKER_USED_NONE        (0)
#define LINKER_USED_INTRAM      (1)
#define LINKER_USED_INTFLASH    (2)
#define LINKER_USED_EXTRAM      (3)
#define LINKER_USED_EXTFLASH    (4)
#define LINKER_USED_DDR         (5)


/* Error reporting utilities */
void ktest_start(char *);
void ktest_end(void);
void ktest_fatal(char *, ...);
void ktest_error(char *, ...);
void ktest_warn(char *, ...);

/* Kernel data utilities */
extern uint16_t get_in_isr(void);
extern void    set_in_isr(uint16_t);
extern void    invalidate_msg_component(void** msg_compnt);
extern void    restore_msg_component(void* msg_compnt);

/* Message utilities */
extern void    invalidate_msg_pool(void *prv_pool);
extern void    restore_msg_pool(void *prv_pool);
extern void    invalidate_msg(void *msg_ptr);
extern void    restore_msg(void *msg_ptr);

/* Memory management utilities */
extern void     memory_free_all(MEMORY_ALLOC_INFO_STRUCT_PTR info);
extern void     memory_alloc_all(MEMORY_ALLOC_INFO_STRUCT_PTR info);
extern _task_id memory_get_owner(void *);

/* General memory display */
extern void      dump_memory(unsigned char  *address, uint32_t size);
extern void      print_kernel_lwmem_pool(void);
extern void      print_lwmem_pool(_lwmem_pool_id);
extern _mem_size get_free_mem(void);

/* Error code display */
//extern void    perror(char  *string);
extern char      *_mqx_get_task_error_code_string(uint32_t);
extern char      *_mqx_get_task_error_code_string2(uint32_t);

/* Queue display functions */
extern void    print_queue(QUEUE_STRUCT_PTR);

/* Work functions */
extern void work(uint32_t);
extern void work_ticks(_mqx_uint);

/* Random number generator */
extern int16_t my_rand(void);
extern void   my_srand(uint16_t);

/* Time checking functions */
extern uint32_t time_check_struct(TIME_STRUCT,uint32_t,uint32_t,uint32_t);
extern uint32_t time_check(uint32_t, uint32_t, uint32_t);

/* call stack checking funcitons */
extern int32_t init_stack(void);
extern uint32_t destry_stack(void);
extern int32_t push_stack(char const *fn);
extern uint32_t pop_stack(void);
extern uint32_t print_stack(void);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */

