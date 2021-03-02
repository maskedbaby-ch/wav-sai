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
* $FileName: per_new.c$
* $Version : 3.8.2.0$
* $Date    : Apr-13-2012$
*
* Comments:
*
*   This file contains the source functions for displaying any mqx error code
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <event.h>
#include <fio.h>
#include <io.h>
#include <message.h>
#include <ipc.h>
#include <ipc_pcb.h>
//#include <log.h>
#include <lwlog.h>
#include <lwmem.h>
#include <lwtimer.h>
#include <mutex.h>
#include <name.h>
#include <partition.h>
#include <pcb.h>
//#include <sem.h>
#include <timer.h>
#include <watchdog.h>
#include "util.h"


/*FUNCTION*-----------------------------------------------------
 * 
 * Function Name    : perror
 * Returned Value   : none
 * Comments         :
 *   this function displays a message followed by the string
 * representation of the mqx error code for the current task.
 *
 *END*--------------------------------------------------------*/

void perror
   (
      /* [IN] the string to print first */
      char  *string
   )
{ /* Body */
   
   fprintf(stderr, "%s: %s\n", string,
      _mqx_get_task_error_code_string(_task_errno));

} /* Endbody */


/*FUNCTION*-----------------------------------------------------
 * 
 * Function Name    : _mqx_get_task_error_code_string
 * Returned Value   : char *string
 * Comments         :
 *   this function returns a string address for a string
 * which describes the input error code
 *
 *END*--------------------------------------------------------*/

/* A macro to display the name for a KLOG function entry */
#define MQX_NAME(x) case x : return(#x)

char  *_mqx_get_task_error_code_string
   (
      /* [IN] the mqx error code */
      uint32_t error_code
   )
{ /* Body */
   
   switch (error_code) {

      MQX_NAME(MQX_OK);
      MQX_NAME(MQX_INVALID_POINTER);
      MQX_NAME(MQX_INVALID_SIZE);
      MQX_NAME(MQX_NOT_RESOURCE_OWNER);
      MQX_NAME(MQX_OUT_OF_MEMORY);
      MQX_NAME(MQX_CORRUPT_MEMORY_SYSTEM);
      MQX_NAME(MQX_CORRUPT_STORAGE_POOL);
      MQX_NAME(MQX_CORRUPT_STORAGE_POOL_FREE_LIST);
      MQX_NAME(MQX_CORRUPT_STORAGE_POOL_POINTERS);
      MQX_NAME(MQX_INVALID_CHECKSUM);
      MQX_NAME(MQX_OUT_OF_TASK_DESCRIPTORS);
      MQX_NAME(MQX_INVALID_MEMORY_BLOCK);
      MQX_NAME(MQX_INVALID_PARAMETER);
      MQX_NAME(MQX_CANNOT_CALL_FUNCTION_FROM_ISR);
      MQX_NAME(MQX_INVALID_TASK_PRIORITY);
      MQX_NAME(MQX_TASK_QUEUE_EMPTY);
      MQX_NAME(MQX_NO_TASK_TEMPLATE);
      MQX_NAME(MQX_INVALID_TASK_STATE);
      MQX_NAME(MQX_INVALID_TASK_ID);
      MQX_NAME(MQX_INVALID_PROCESSOR_NUMBER);
      MQX_NAME(MQX_INVALID_VECTORED_INTERRUPT);
      MQX_NAME(MQX_INVALID_TEMPLATE_INDEX);
      MQX_NAME(MQX_INVALID_CONFIGURATION);
      MQX_NAME(MQX_COMPONENT_EXISTS);
      MQX_NAME(MQX_COMPONENT_DOES_NOT_EXIST);
      MQX_NAME(MQX_INVALID_COMPONENT_HANDLE);
      MQX_NAME(MQX_INVALID_COMPONENT_BASE);
      MQX_NAME(MQX_INVALID_COMPONENT_NAME);
      MQX_NAME(MQX_INVALID_HANDLE);
      MQX_NAME(MQX_CORRUPT_QUEUE);
      MQX_NAME(MQX_INVALID_TASK_QUEUE);
      MQX_NAME(MQX_INVALID_LWSEM);
      MQX_NAME(MQX_CORRUPT_INTERRUPT_STACK);
      MQX_NAME(MQX_KERNEL_MEMORY_TOO_SMALL);
      MQX_NAME(MQX_COULD_NOT_CREATE_IPC_TASK);
      MQX_NAME(MQX_TOO_MANY_PRIORITY_LEVELS);
      MQX_NAME(MQX_TOO_MANY_INTERRUPTS);
      MQX_NAME(MQX_DUPLICATE_TASK_TEMPLATE_INDEX);
      MQX_NAME(MQX_TIMER_ISR_INSTALL_FAIL);
      MQX_NAME(MQX_SCHED_INVALID_POLICY);
      MQX_NAME(MQX_SCHED_INVALID_PARAMETER_PTR);
      MQX_NAME(MQX_SCHED_INVALID_PARAMETER);
      MQX_NAME(MQX_SCHED_INVALID_TASK_ID);
      MQX_NAME(MQX_INVALID_IO_CHANNEL);
      MQX_NAME(MQX_IO_OPERATION_NOT_AVAILABLE);
      MQX_NAME(MQX_INTER_PROCESSOR_INIT_FAILED);
      MQX_NAME(MQX_IPC_INVALID_MESSAGE);
      MQX_NAME(MQX_IPC_SERVICE_NOT_AVAILABLE);
      MQX_NAME(MQX_IPC_ROUTE_EXISTS);
      MQX_NAME(MQX_MEM_POOL_TOO_SMALL);
      MQX_NAME(MQX_MEM_POOL_INVALID);
      MQX_NAME(MQX_OUT_OF_MMU_PAGE_TABLES);
      MQX_NAME(MQX_MMU_CONTEXT_EXISTS);
      MQX_NAME(MQX_MMU_CONTEXT_DOES_NOT_EXIST);
      MQX_NAME(MQX_MMU_PARENT_TASK_CANNOT_BE_MMU);
      MQX_NAME(MQX_LWSEM_WAIT_TIMEOUT);
      MQX_NAME(MQX_LWMEM_POOL_INVALID);
      MQX_NAME(MQX_LWEVENT_INVALID);
      MQX_NAME(MQX_LWTIMER_INVALID);
/*      MQX_NAME(MQX_EOK);*/
      MQX_NAME(MQX_E2BIG);
      MQX_NAME(MQX_EACCES);
      MQX_NAME(MQX_EAGAIN);
      MQX_NAME(MQX_EBADF);
      MQX_NAME(MQX_EBADMSG);
      MQX_NAME(MQX_EBUSY);
      MQX_NAME(MQX_ECANCELED);
      MQX_NAME(MQX_ECHILD);
      MQX_NAME(MQX_EDEADLK);
      MQX_NAME(MQX_EDOM);
      MQX_NAME(MQX_EEXIST);
      MQX_NAME(MQX_EFAULT);
      MQX_NAME(MQX_EFBIG);
      MQX_NAME(MQX_EINPROGRESS);
      MQX_NAME(MQX_EINTR);
      MQX_NAME(MQX_EINVAL);
      MQX_NAME(MQX_EIO);
      MQX_NAME(MQX_EISDIR);
      MQX_NAME(MQX_EMFILE);
      MQX_NAME(MQX_EMLINK);
      MQX_NAME(MQX_EMSGSIZE);
      MQX_NAME(MQX_ENAMETOOLONG);
      MQX_NAME(MQX_ENFILE);
      MQX_NAME(MQX_ENODEV);
      MQX_NAME(MQX_ENOENT);
      MQX_NAME(MQX_ENOEXEC);
      MQX_NAME(MQX_ENOLCK);
      MQX_NAME(MQX_ENOMEM);
      MQX_NAME(MQX_ENOSPC);
      MQX_NAME(MQX_ENOSYS);
      MQX_NAME(MQX_ENOTDIR);
      MQX_NAME(MQX_ENOTEMPTY);
      MQX_NAME(MQX_ENOTSUP);
      MQX_NAME(MQX_ENOTTY);
      MQX_NAME(MQX_ENXIO);
      MQX_NAME(MQX_EPERM);
      MQX_NAME(MQX_EPIPE);
      MQX_NAME(MQX_ERANGE);
      MQX_NAME(MQX_EROFS);
      MQX_NAME(MQX_ESPIPE);
      MQX_NAME(MQX_ESRCH);
      MQX_NAME(MQX_ETIMEDOUT);
      MQX_NAME(MQX_EXDEV);

      default: 
         return(_mqx_get_task_error_code_string2(error_code));
         

   } /* Endswitch */
   
} /* Endbody */


char  *_mqx_get_task_error_code_string2
   (
      /* [IN] the mqx error code */
      uint32_t error_code
   )
{ /* Body */

   switch (error_code) {

      MQX_NAME(EVENT_MULTI_PROCESSOR_NOT_AVAILABLE);
      MQX_NAME(EVENT_DELETED);
      MQX_NAME(EVENT_NOT_DELETED);
      MQX_NAME(EVENT_INVALID_EVENT_HANDLE);
      MQX_NAME(EVENT_CANNOT_SET);
      MQX_NAME(EVENT_CANNOT_GET_EVENT);
      MQX_NAME(EVENT_INVALID_EVENT_COUNT);
      MQX_NAME(EVENT_WAIT_TIMEOUT);
      MQX_NAME(EVENT_EXISTS);
      MQX_NAME(EVENT_TABLE_FULL);
      MQX_NAME(EVENT_NOT_FOUND);
      MQX_NAME(EVENT_INVALID_EVENT);
      MQX_NAME(EVENT_CANNOT_WAIT_ON_REMOTE_EVENT);

      MQX_NAME(IPC_PCB_PACKET_POOL_CREATE_FAILED);
      MQX_NAME(IPC_PCB_INVALID_QUEUE);
      MQX_NAME(IPC_PCB_DEVICE_OPEN_FAILED);
      MQX_NAME(IPC_PCB_OUTPUT_PCB_POOL_CREATE_FAILED);
      MQX_NAME(IPC_PCB_INPUT_PCB_POOL_CREATE_FAILED);
      
      MQX_NAME(LOG_INVALID);
      MQX_NAME(LOG_EXISTS);
      MQX_NAME(LOG_DOES_NOT_EXIST);
      MQX_NAME(LOG_FULL);
      MQX_NAME(LOG_ENTRY_NOT_AVAILABLE);
      MQX_NAME(LOG_DISABLED);
      MQX_NAME(LOG_INVALID_READ_TYPE);
      MQX_NAME(LOG_INVALID_SIZE);

      MQX_NAME(MSGPOOL_OUT_OF_MESSAGES);
      MQX_NAME(MSGPOOL_OUT_OF_POOLS);
      MQX_NAME(MSGPOOL_INVALID_POOL_ID);
      MQX_NAME(MSGPOOL_ALL_MESSAGES_NOT_FREE);
      MQX_NAME(MSGPOOL_MESSAGE_SIZE_TOO_SMALL);
      MQX_NAME(MSGPOOL_POOL_NOT_CREATED);

      MQX_NAME(MSGQ_INVALID_QUEUE_ID);
      MQX_NAME(MSGQ_QUEUE_IN_USE);
      MQX_NAME(MSGQ_NOT_QUEUE_OWNER);
      MQX_NAME(MSGQ_QUEUE_IS_NOT_OPEN);
      MQX_NAME(MSGQ_MESSAGE_NOT_AVAILABLE);
      MQX_NAME(MSGQ_INVALID_MESSAGE_PRIORITY);
      MQX_NAME(MSGQ_INVALID_MESSAGE);
      MQX_NAME(MSGQ_QUEUE_FULL);
      MQX_NAME(MSGQ_RECEIVE_TIMEOUT);
      MQX_NAME(MSGQ_TOO_MANY_QUEUES);
      MQX_NAME(MSGQ_MESSAGE_IS_QUEUED);

      MQX_NAME(MSG_CANNOT_CREATE_COMPONENT);
      
      MQX_NAME(NAME_TABLE_FULL);
      MQX_NAME(NAME_EXISTS);
      MQX_NAME(NAME_NOT_FOUND);
      MQX_NAME(NAME_TOO_LONG);
      MQX_NAME(NAME_TOO_SHORT);
      
      MQX_NAME(PARTITION_INVALID);
      MQX_NAME(PARTITION_OUT_OF_BLOCKS);
      MQX_NAME(PARTITION_BLOCK_INVALID_ALIGNMENT);
      MQX_NAME(PARTITION_TOO_SMALL);
      MQX_NAME(PARTITION_INVALID_TASK_ID);
      MQX_NAME(PARTITION_BLOCK_INVALID_CHECKSUM);
      MQX_NAME(PARTITION_INVALID_TYPE);
      MQX_NAME(PARTITION_ALL_BLOCKS_NOT_FREE);

      MQX_NAME(SEM_MULTI_PROCESSOR_NOT_AVAILABLE);
      MQX_NAME(SEM_SEMAPHORE_DELETED);
      MQX_NAME(SEM_SEMAPHORE_NOT_DELETED);
      MQX_NAME(SEM_INVALID_SEMAPHORE_HANDLE);
      MQX_NAME(SEM_CANNOT_POST);
      MQX_NAME(SEM_CANNOT_GET_SEMAPHORE);
      MQX_NAME(SEM_INVALID_SEMAPHORE_COUNT);
      MQX_NAME(SEM_WAIT_TIMEOUT);
      MQX_NAME(SEM_SEMAPHORE_EXISTS);
      MQX_NAME(SEM_SEMAPHORE_TABLE_FULL);
      MQX_NAME(SEM_SEMAPHORE_NOT_FOUND);
      MQX_NAME(SEM_INVALID_POLICY);
      MQX_NAME(SEM_INVALID_SEMAPHORE);
      MQX_NAME(SEM_INCORRECT_INITIAL_COUNT);

      MQX_NAME(WATCHDOG_INVALID_ERROR_FUNCTION);
      MQX_NAME(WATCHDOG_INVALID_INTERRUPT_VECTOR);

      default: return "unknown error code";

   } /* Endswitch */

} /* Endbody */

/* EOF */
