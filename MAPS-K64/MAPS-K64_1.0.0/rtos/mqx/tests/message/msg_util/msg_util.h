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
* $Version : 4.0.1.0$
* $Date    : Apr-12-2013$
*
* Comments:
*
*   This file contains the include information for Message testing utility
*   function prototypes
*
*END************************************************************************/
#ifndef _msg_util_h_
#define _msg_util_h_

#include <mqx_inc.h>
#include <bsp.h>
#include <message.h>
#include <msg_prv.h>

/* Get the system message component pointer */
void   *get_msg_component(void);

/* Set the system message component pointer */
void set_msg_component(void *msg_comp_ptr);

/* Set the value of VALID field of message pool */
void set_pool_valid(MSGPOOL_STRUCT_PTR msg_pool_ptr, uint16_t value);

/* Set the value of VALID field of a message */
void set_msg_valid(MESSAGE_HEADER_STRUCT_PTR msg_ptr, uint16_t value);

#endif
