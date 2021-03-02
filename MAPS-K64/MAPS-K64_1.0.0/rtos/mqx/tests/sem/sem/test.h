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
* $Version : 3.5.1.0$
* $Date    : Nov-2-2009$
*
* Comments:
*
*   This include file is used for the testing software.
*
*END************************************************************************/

#define MAIN_TASK             10
#define TEST_DESTROY_SEM_TASK 15
#define PRIO_TEST_TASK1       16
#define PRIO_TEST_TASK2       17
#define WAIT_TIME_TASK        18


typedef struct destroy_settings_struct
{
   bool  force_destroy;
   char *name;
} DESTROY_SETTINGS_STRUCT, * DESTROY_SETTINGS_STRUCT_PTR;

extern void main_task(uint32_t);
extern void prio_test_task(uint32_t);
extern void test_destroy_sem_task(uint32_t);
extern void wait_time_task(uint32_t);

/* sem utility functions */
extern SEM_COMPONENT_STRUCT_PTR get_sem_component_ptr(void);
extern void      set_sem_component_ptr(SEM_COMPONENT_STRUCT_PTR);
extern _mqx_uint get_sem_component_valid_field(void);
extern void      set_sem_component_valid_field(_mqx_uint);


extern bool   get_sem_delayed_destroy_field(char *);
extern void      set_sem_delayed_destroy_field(char *, bool);
extern bool   get_sem_delayed_destroy_field_fast(_mqx_uint);
extern void      set_sem_delayed_destroy_field_fast(_mqx_uint, bool);
extern _mqx_uint get_sem_valid_field(char *);
extern void      set_sem_valid_field(char *, _mqx_uint);
extern _mqx_uint get_sem_valid_field_fast(_mqx_uint);
extern void      set_sem_valid_field_fast(_mqx_uint, _mqx_uint);


extern _mqx_uint get_sem_handle_valid_field(void *);
extern void      set_sem_handle_valid_field(void *, _mqx_uint);

/* EOF */
