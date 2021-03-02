/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: queue.c$
* $Version : 3.8.2.0$
* $Date    : Sep-8-2011$
*
* Comments:
*
*   This file contains the source functions for displaying and manipulating
*   queues.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include "util.h"

void print_queue
   (
      QUEUE_STRUCT_PTR q
   )
{ /* Body */
   QUEUE_ELEMENT_STRUCT_PTR e = q->NEXT;
   uint32_t                  n = q->SIZE;

   printf("Q at 0x%x,  max: %d,  size: %d\n",
      (uint32_t)q, (uint32_t)q->MAX, (uint32_t)q->SIZE);
   while ( n-- ) {
      printf("   Element: 0x%x\n", (uint32_t)e);
      e = e->NEXT;
   } /* Endwhile */

} /* Endbody */

/* EOF */
