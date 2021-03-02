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
* $FileName: mem.c$
* $Version : 3.8.7.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file contains the source functions for the demo of the
*   time slicing functions of the kernel.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <message.h>
#include <msg_prv.h>
#include "util.h"


void invalidate_msg_pool
   (
      void   *prv_pool
   )
{ /* Body */
    MSGPOOL_STRUCT_PTR msgpool_ptr;
    msgpool_ptr = (MSGPOOL_STRUCT_PTR)prv_pool;
    msgpool_ptr->VALID = 0;
} /* End Body */

void restore_msg_pool
   (
      void   *prv_pool
   )
{ /* Body */
    MSGPOOL_STRUCT_PTR msgpool_ptr;
    msgpool_ptr = (MSGPOOL_STRUCT_PTR)prv_pool;
    msgpool_ptr->VALID = MSG_VALID;
} /* End Body */

void invalidate_msg
   (
      void   *msg_ptr
   )
{ /* Body */
    INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr;
    imsg_ptr = GET_INTERNAL_MESSAGE_PTR(msg_ptr);
    imsg_ptr->VALID = 0;
} /* End Body */

void restore_msg
   (
      void   *msg_ptr
   )
{ /* Body */
    INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr;
    imsg_ptr = GET_INTERNAL_MESSAGE_PTR(msg_ptr);
    imsg_ptr->VALID = MSG_VALID;
} /* End Body */
/* EOF */
