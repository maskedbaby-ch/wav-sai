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
* $FileName: stack.c$
* $Version : 4.0.2$
* $Date    : June-24-2013$
*
* Comments:
*
*   This file contains utility function for function call stack
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "util.h"

/* how many string can store for 1 step */
#define STKLEN 255
/*stack depth*/
#define DEPTH  50
/*the way stack memory is allocated*/
#define _MALLOC_STACK _mem_alloc

/*local routine*/
static void *__stack_malloc_internal(void);
static void __stack_free_internal(void*);

typedef struct Stack {
    char fn[STKLEN];
    struct Stack * pnext;
} TStack, * pTStack;

typedef struct STackH {
    pTStack pnext;
    uint32_t count;
} STH, * pSTH;

static STH stkh;
static void *_ptr_top;
volatile static void *_ptr_free;


/*FUNCTION*-----------------------------------------------------
 *
 * @Function Name    : __malloc_internal
 * @Returned Value   : pointer
 * @Comments         :
 *   this function prints out the error message
 *
 *END*--------------------------------------------------------*/
void *__stack_malloc_internal(void)
{
   void *pp = (void*)_ptr_free;
   if ((uint32_t)_ptr_free + sizeof(TStack) >= (uint32_t)_ptr_top + DEPTH * sizeof(TStack))
     _ptr_free = _ptr_top;
   else
     _ptr_free = (void*)((uint32_t)_ptr_free + sizeof(TStack));
   return pp;
}

/*FUNCTION*-----------------------------------------------------
 *
 * @Function Name    : __free_internal
 * @Returned Value   : pointer
 * @Comments         :
 *   this function prints out the error message
 *
 *END*--------------------------------------------------------*/
void __stack_free_internal(void *a)
{
  /* no needs to do anyting*/
  return;
}


/*FUNCTION*-----------------------------------------------------
 *
 * @Function Name    : init_stack
 * @Returned Value   : 0 success, otherwise -1
 * @Comments         :
 *   this function prints out the error message
 *
 *END*--------------------------------------------------------*/
int32_t init_stack(void)
{
   destry_stack();
   stkh.count = 0;
   stkh.pnext = NULL;
   _ptr_free = (void*)_MALLOC_STACK(DEPTH*sizeof(TStack));
   _ptr_top = (void*)_ptr_free;
   if (_ptr_free == NULL )
     return -1;
   return 0;
}

/*FUNCTION*-----------------------------------------------------
 *
 * @Function Name    : destry_stack
 * @Returned Value   : none
 * @Comments         :
 *   this function prints out the error message
 *
 *END*--------------------------------------------------------*/
uint32_t destry_stack(void)
{
   while (stkh.count && stkh.pnext )
   {
      pTStack pp = stkh.pnext;
      stkh.pnext = pp->pnext;
      __stack_free_internal(pp);
      stkh.count--;
   }
   _mem_free(_ptr_top);
   _ptr_top = NULL;
   return 0;
}

/*FUNCTION*-----------------------------------------------------
 *
 * @Function Name    : push_stack
 * @Returned Value   : none
 * @Comments         :
 *   this function prints out the error message
 *
 *END*--------------------------------------------------------*/
int32_t push_stack(char const *fn)
{
  pTStack pp = stkh.pnext;
  stkh.pnext = (pTStack)__stack_malloc_internal();
  if (stkh.pnext == NULL)
    return -1;
  snprintf(stkh.pnext->fn, STKLEN, "%s\n", fn);
  stkh.pnext->pnext = pp;
  stkh.count = stkh.count == DEPTH ? DEPTH : (stkh.count + 1);
  return stkh.count;
}


/*FUNCTION*-----------------------------------------------------
 *
 * @Function Name    : pop_stack
 * @Returned Value   : none
 * @Comments         :
 *   this function prints out the error message
 *
 *END*--------------------------------------------------------*/
uint32_t pop_stack(void)
{
  if (stkh.count == 0)
    return 0;
  pTStack pp = stkh.pnext;
  stkh.pnext = pp->pnext;
  __stack_free_internal(pp);
  stkh.count--;
  return stkh.count;
}

/*FUNCTION*-----------------------------------------------------
 *
 * @Function Name    : print_statck
 * @Returned Value   : none
 * @Comments         :
 *   this function prints out the error message
 *
 *END*--------------------------------------------------------*/
uint32_t print_stack(void)
{
  pTStack pp;
  uint32_t count = stkh.count;
  pp = stkh.pnext;
  while (pp && count > 0)
  {
    printf("function name %s\n",pp->fn);
    pp = pp->pnext;
    count--;
  }
  return 0;
}
