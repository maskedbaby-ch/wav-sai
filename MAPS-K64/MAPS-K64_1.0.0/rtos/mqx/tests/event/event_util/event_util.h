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
* $Date    : Jun-1-2013$
*
* Comments:
*
*   This file contains the include information for Event testing utility
*   function prototypes
*
*END************************************************************************/
#ifndef _event_util_h_
#define _event_util_h_

extern _mqx_uint get_event_component_capacity(void);
extern void      set_event_component_valid_field(_mqx_uint);
extern void      set_event_handle_valid_field(void *,_mqx_uint);
extern void      set_event_valid_field(void *,_mqx_uint);
extern void      set_event_component_ptr(void *);
extern void      set_event_queue_ptr( void *, void *);
extern _mqx_uint get_event_component_valid_field(void);
extern _mqx_uint get_event_handle_valid_field(void *);
extern _mqx_uint get_event_valid_field(void *);
extern void     *get_event_component_ptr(void);
extern void     *get_event_queue_ptr( void *);

#endif
