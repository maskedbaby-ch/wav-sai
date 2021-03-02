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
* $FileName: kdata.c$
* $Version : 3.8.2.0$
* $Date    : Sep-8-2011$
*
* Comments:
*
*   This file contains the source functions for the demo of the
*   time slicing functions of the kernel.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include "util.h"

uint16_t get_in_isr
   (
      void
   )
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
   
   _GET_KERNEL_DATA(kernel_data);                                         
   return kernel_data->IN_ISR;

} /* Endbody */


void set_in_isr
   (
      uint16_t value
   )
{ /* Body */
   KERNEL_DATA_STRUCT_PTR kernel_data;
   
   _GET_KERNEL_DATA(kernel_data);                                         
   kernel_data->IN_ISR = value;

} /* Endbody */

void invalidate_msg_component
   (
      void** msg_compnt
   )
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
   
    _INT_DISABLE();
    _GET_KERNEL_DATA(kernel_data);
    *msg_compnt = kernel_data->KERNEL_COMPONENTS[KERNEL_MESSAGES];
    kernel_data->KERNEL_COMPONENTS[KERNEL_MESSAGES] = NULL;
    _INT_ENABLE();
} /* Endbody */

void restore_msg_component
   (
      void* msg_compnt
   )
{ /* Body */
    KERNEL_DATA_STRUCT_PTR kernel_data;
   
    _INT_DISABLE();
    _GET_KERNEL_DATA(kernel_data);
    kernel_data->KERNEL_COMPONENTS[KERNEL_MESSAGES] = msg_compnt;
    _INT_ENABLE();
} /* Endbody */

/* EOF */
