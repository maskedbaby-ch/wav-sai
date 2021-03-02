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
* $FileName: test.h$
* $Version : 3.7.2.0$
* $Date    : Mar-17-2011$
*
* Comments:
*
*   This include file is used for the testing software.
*
*END************************************************************************/



#define MAIN_TASK       10
#define WAIT_ALL_TASK1  11
#define WAIT_ALL_TASK2  12
#define WAIT_ALL_TASK3  13
#define WAIT_ANY_TASK   14

extern void main_task(uint32_t);
extern void wait_all_task(uint32_t);
extern void wait_any_task(uint32_t);

/* event utility functions */
extern EVENT_COMPONENT_STRUCT_PTR get_event_component_ptr(void);
extern void      set_event_component_ptr(EVENT_COMPONENT_STRUCT_PTR);
extern _mqx_uint get_event_component_valid_field(void);
extern void      set_event_component_valid_field(_mqx_uint);


extern _mqx_uint get_event_valid_field(char *);
extern void      set_event_valid_field(char *, _mqx_uint);
extern _mqx_uint get_event_valid_field_fast(_mqx_uint);
extern void      set_event_valid_field_fast(_mqx_uint, _mqx_uint);


extern _mqx_uint get_event_handle_valid_field(void *);
extern void      set_event_handle_valid_field(void *, _mqx_uint);

/* EOF */
