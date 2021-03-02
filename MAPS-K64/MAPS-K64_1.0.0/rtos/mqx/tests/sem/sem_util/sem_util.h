#ifndef _sem_util_h_
#define _sem_util_h_
/**HEADER********************************************************************
*
* Copyright (c)  Freescale Semiconductor;
* All Rights Reserved
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING,  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING  OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: test.h$
* $Version : 4.0.2$
* $Date    : Aug-13-2013$
*
* Comments:
*
*   This file contains the prototypes of utility functions for Semaphore
*   testing module.
*
*END************************************************************************/

/* Get the semaphore component pointer */
void * get_sem_component_ptr(void);

/* Set the semaphore component pointer */
void set_sem_component_ptr(void *);

/* Get the VALID field of semaphore component */
_mqx_uint get_sem_component_valid_field(void);

/* Set the VALID field of semaphore component */
void set_sem_component_valid_field(_mqx_uint);

/* Get the name table handler of semaphore component */
void * get_sem_component_name_table(void);

/* Set the name table handler of semaphore component */
void set_sem_component_name_table(void *);

/* Get the VALID field of a semaphore which is identified by name */
_mqx_uint get_sem_valid_field(char *);

/* Set the VALID field of a semaphore which is identified by name */
void set_sem_valid_field(char *, _mqx_uint);

/* Get the DELAYED_DESTROY field of a semaphore which is identified by name */
bool get_sem_delayed_destroy_field(char *);

/* Set the DELAYED_DESTROY field of a semaphore which is identified by name */
void set_sem_delayed_destroy_field(char *, bool);

/* Get the COUNT field of a semaphore which is identified by name */
_mqx_uint get_sem_count_field(char *);

/* Set the COUNT field of a semaphore which is identified by name */
void set_sem_count_field(char *, _mqx_uint);

/* Get the VALID field of a semaphore which is identified by index */
_mqx_uint get_sem_valid_field_fast(_mqx_uint);

/* Set the VALID field of a semaphore which is identified by index */
void set_sem_valid_field_fast(_mqx_uint, _mqx_uint);

/* Get the DELAYED_DESTROY field of a semaphore which is identified by index */
bool get_sem_delayed_destroy_field_fast(_mqx_uint);

/* Set the DELAYED_DESTROY field of a semaphore which is identified by index */
void set_sem_delayed_destroy_field_fast(_mqx_uint, bool);

/* Get the owner (task) of semaphore connection */
void * get_sem_handle_owner(void *);

/* Set the owner (task) of semaphore connection */
void set_sem_handle_owner(void *, void *);

/* Get the VALID field of a semaphore connection */
_mqx_uint get_sem_handle_valid_field(void *);

/* Set the VALID field of a semaphore connection */
void set_sem_handle_valid_field(void *, _mqx_uint);

/* Remove the waiting task from the waiting list of semaphore */
void remove_waiting_task(void *);

#endif
