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
* $FileName: test.c$
* $Version : 4.0.2$
* $Date    : Aug-13-2013$
*
* Comments:
*
*   This file contains the utility functions for Semaphore testing module,
*   includes of getting/setting components' parameters.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <bsp.h>
#include <name.h>
#include <name_prv.h>
#include <sem.h>
#include <sem_prv.h>
#include "sem_util.h"

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_sem_component_ptr
* Returned Value   : void *
* Comments         :
*   Get the semaphore component pointer.
*
*END*--------------------------------------------------------------------*/
void * get_sem_component_ptr(void)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the semaphore component pointer */
    return kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_sem_component_ptr
* Returned Value   : void
* Comments         :
*   Set the semaphore component pointer
*   - [IN] sem_ptr: The pointer to be set as semaphore component pointer.
*
*END*--------------------------------------------------------------------*/
void set_sem_component_ptr(void *sem_ptr)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Disable all interrupts */
    _int_disable();
    /* Set the semaphore component pointer */
    kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES] = sem_ptr;
    /* Enable all interrupts */
    _int_enable();
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_sem_component_valid_field
* Returned Value   : _mqx_uint
* Comments         :
*   Get the VALID field of semaphore component.
*
*END*--------------------------------------------------------------------*/
_mqx_uint get_sem_component_valid_field(void)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    SEM_COMPONENT_STRUCT_PTR sem_component_ptr;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the semaphore component pointer */
    sem_component_ptr = (SEM_COMPONENT_STRUCT_PTR)kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    /* Get the VALID field of semaphore component */
    return sem_component_ptr->VALID;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_sem_component_valid_field
* Returned Value   : void
* Comments         :
*   Set the VALID field of semaphore component
*   - [IN] sem_valid: The value to be set.
*
*END*--------------------------------------------------------------------*/
void set_sem_component_valid_field(_mqx_uint sem_valid)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    SEM_COMPONENT_STRUCT_PTR sem_component_ptr;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the semaphore component pointer */
    sem_component_ptr = (SEM_COMPONENT_STRUCT_PTR)kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    /* Disable all interrupts */
    _int_disable();
    /* Set the VALID field of semaphore component */
    sem_component_ptr->VALID = sem_valid;
    /* Enable all interrupts */
    _int_enable();
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_sem_component_name_table
* Returned Value   : void *
* Comments         :
*   Get the name table handler of semaphore component.
*
*END*--------------------------------------------------------------------*/
void * get_sem_component_name_table(void)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    SEM_COMPONENT_STRUCT_PTR sem_component_ptr;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the semaphore component pointer */
    sem_component_ptr = (SEM_COMPONENT_STRUCT_PTR)kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    /* Get the name table pointer */
    return sem_component_ptr->NAME_TABLE_HANDLE;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_sem_component_name_table
* Returned Value   : void
* Comments         :
*   Set the name table handler of semaphore component.
*   - [IN] handle_ptr: The pointer to be set.
*
*END*--------------------------------------------------------------------*/
void set_sem_component_name_table(void *handle_ptr)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    SEM_COMPONENT_STRUCT_PTR sem_component_ptr;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the semaphore component pointer */
    sem_component_ptr = (SEM_COMPONENT_STRUCT_PTR)kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    /* Disable all interrupts */
    _int_disable();
    /* Set the name table pointer */
    sem_component_ptr->NAME_TABLE_HANDLE = handle_ptr;
    /* Enable all interrupts */
    _int_enable();
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_sem_valid_field
* Returned Value   : _mqx_uint
* Comments         :
*   Get the VALID field of a semaphore which is identified by name.
*   - [IN] name_ptr: Name of semaphore.
*
*END*--------------------------------------------------------------------*/
_mqx_uint get_sem_valid_field(char *name_ptr)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    SEM_COMPONENT_STRUCT_PTR sem_component_ptr;
    SEM_STRUCT_PTR sem_ptr;
    _mqx_max_type tmp;
    _mqx_uint result;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the semaphore component pointer */
    sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    /* Get the semaphore pointer from name */
    result = _name_find_internal(sem_component_ptr->NAME_TABLE_HANDLE, name_ptr, &tmp);
    /* Return the VALID field of semaphore */
    if (result == MQX_OK)
    {
        sem_ptr = (SEM_STRUCT_PTR)tmp;
        return sem_ptr->VALID;
    }
    else
    {
        return 0;
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_sem_valid_field
* Returned Value   : void
* Comments         :
*   Set the VALID field of a semaphore which is identified by name.
*   - [IN] name_ptr: Name of semaphore.
*   - [IN] valid_value: The value to be set.
*
*END*--------------------------------------------------------------------*/
void set_sem_valid_field(char *name_ptr, _mqx_uint valid_value)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    SEM_COMPONENT_STRUCT_PTR sem_component_ptr;
    SEM_STRUCT_PTR sem_ptr;
    _mqx_max_type tmp;
    _mqx_uint result;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the semaphore component pointer */
    sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    /* Get the semaphore pointer from name */
    result = _name_find_internal(sem_component_ptr->NAME_TABLE_HANDLE, name_ptr, &tmp);
    /* Set the VALID field of semaphore */
    if (result == MQX_OK)
    {
        sem_ptr = (SEM_STRUCT_PTR)tmp;
        sem_ptr->VALID = valid_value;
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_sem_delayed_destroy_field
* Returned Value   : bool
* Comments         :
*   Get the DELAYED_DESTROY field of a semaphore which is identified by name.
*   - [IN] name_ptr: Name of semaphore.
*
*END*--------------------------------------------------------------------*/
bool get_sem_delayed_destroy_field(char *name_ptr)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    SEM_COMPONENT_STRUCT_PTR sem_component_ptr;
    SEM_STRUCT_PTR sem_ptr;
    _mqx_max_type tmp;
    _mqx_uint result;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the semaphore component pointer */
    sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    /* Get the semaphore pointer from name */
    result = _name_find_internal(sem_component_ptr->NAME_TABLE_HANDLE, name_ptr, &tmp);
    /* Get the DELAYED_DESTROY field of semaphore */
    if (result == MQX_OK)
    {
        sem_ptr = (SEM_STRUCT_PTR)tmp;
        return sem_ptr->DELAYED_DESTROY;
    }
    else
    {
        return FALSE;
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_sem_delayed_destroy_field
* Returned Value   : void
* Comments         :
*   Set the DELAYED_DESTROY field of a semaphore which is identified by name.
*   - [IN] name_ptr: Name of semaphore.
*   - [IN] delayed_destroy_value: The value to be set.
*
*END*--------------------------------------------------------------------*/
void set_sem_delayed_destroy_field(char *name_ptr, bool delayed_destroy_value)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    SEM_COMPONENT_STRUCT_PTR sem_component_ptr;
    SEM_STRUCT_PTR sem_ptr;
    _mqx_max_type tmp;
    _mqx_uint result;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the semaphore component pointer */
    sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    /* Get the semaphore pointer from name */
    result = _name_find_internal(sem_component_ptr->NAME_TABLE_HANDLE, name_ptr, &tmp);
    /* Set the DELAYED_DESTROY field of semaphore */
    if (result == MQX_OK)
    {
        sem_ptr = (SEM_STRUCT_PTR)tmp;
        sem_ptr->DELAYED_DESTROY = delayed_destroy_value;
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_sem_count_field
* Returned Value   : _mqx_uint
* Comments         :
*   Get the COUNT field of a semaphore which is identified by name
*   - [IN] name_ptr: Name of semaphore.
*
*END*--------------------------------------------------------------------*/
_mqx_uint get_sem_count_field(char *name_ptr)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    SEM_COMPONENT_STRUCT_PTR sem_component_ptr;
    SEM_STRUCT_PTR sem_ptr;
    _mqx_max_type tmp;
    _mqx_uint result;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the semaphore component pointer */
    sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    /* Get the semaphore pointer from name */
    result = _name_find_internal(sem_component_ptr->NAME_TABLE_HANDLE, name_ptr, &tmp);
    /* Return the VALID field of semaphore */
    if (result == MQX_OK)
    {
        sem_ptr = (SEM_STRUCT_PTR)tmp;
        return sem_ptr->COUNT;
    }
    else
    {
        return 0;
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_sem_count_field
* Returned Value   : void
* Comments         :
*   Set the COUNT field of a semaphore which is identified by name
*   - [IN] name_ptr: Name of semaphore.
*   - [IN] count_value: The value to be set.
*
*END*--------------------------------------------------------------------*/
void set_sem_count_field(char *name_ptr, _mqx_uint count_value)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    SEM_COMPONENT_STRUCT_PTR sem_component_ptr;
    SEM_STRUCT_PTR sem_ptr;
    _mqx_max_type tmp;
    _mqx_uint result;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the semaphore component pointer */
    sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    /* Get the semaphore pointer from name */
    result = _name_find_internal(sem_component_ptr->NAME_TABLE_HANDLE, name_ptr, &tmp);
    /* Set the VALID field of semaphore */
    if (result == MQX_OK)
    {
        sem_ptr = (SEM_STRUCT_PTR)tmp;
        sem_ptr->COUNT = count_value;
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_sem_valid_field_fast
* Returned Value   : _mqx_uint
* Comments         :
*   Get the VALID field of a semaphore which is identified by index.
*   - [IN] sem_index: Index of semaphore.
*
*END*--------------------------------------------------------------------*/
_mqx_uint get_sem_valid_field_fast(_mqx_uint sem_index)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    SEM_COMPONENT_STRUCT_PTR sem_component_ptr;
    SEM_STRUCT_PTR sem_ptr;
    _mqx_max_type tmp;
    _mqx_uint result;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the semaphore component pointer */
    sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    /* Get the semaphore pointer from index */
    result = _name_find_internal_by_index(sem_component_ptr->NAME_TABLE_HANDLE, sem_index, &tmp);
    /* Get the VALID field of semaphore */
    if (result == MQX_OK)
    {
        sem_ptr = (SEM_STRUCT_PTR)tmp;
        return sem_ptr->VALID;
    }
    else
    {
        return 0;
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_sem_valid_field_fast
* Returned Value   : void
* Comments         :
*   Set the VALID field of a semaphore which is identified by index.
*   - [IN] sem_index: Index of semaphore.
*   - [IN] valid_value: The value to be set.
*
*END*--------------------------------------------------------------------*/
void set_sem_valid_field_fast(_mqx_uint sem_index, _mqx_uint valid_value)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    SEM_COMPONENT_STRUCT_PTR sem_component_ptr;
    SEM_STRUCT_PTR sem_ptr;
    _mqx_max_type tmp;
    _mqx_uint result;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the semaphore component pointer */
    sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    /* Get the semaphore pointer from index */
    result = _name_find_internal_by_index(sem_component_ptr->NAME_TABLE_HANDLE, sem_index, &tmp);
    /* Set the VALID field of semaphore */
    if (result == MQX_OK)
    {
        sem_ptr = (SEM_STRUCT_PTR)tmp;
        sem_ptr->VALID = valid_value;
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_sem_delayed_destroy_field_fast
* Returned Value   : bool
* Comments         :
*   Get the DELAYED_DESTROY field of a semaphore which is identified by index.
*   - [IN] sem_index: Index of semaphore.
*
*END*--------------------------------------------------------------------*/
bool get_sem_delayed_destroy_field_fast(_mqx_uint sem_index)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    SEM_COMPONENT_STRUCT_PTR sem_component_ptr;
    SEM_STRUCT_PTR sem_ptr;
    _mqx_max_type tmp;
    _mqx_uint result;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the semaphore component pointer */
    sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    /* Get the semaphore pointer from index */
    result = _name_find_internal_by_index(sem_component_ptr->NAME_TABLE_HANDLE, sem_index, &tmp);
    /* Get the DELAYED_DESTROY field of semaphore */
    if (result == MQX_OK)
    {
        sem_ptr = (SEM_STRUCT_PTR)tmp;
        return sem_ptr->DELAYED_DESTROY;
    }
    else
    {
        return 0;
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_sem_delayed_destroy_field_fast
* Returned Value   : void
* Comments         :
*   Set the DELAYED_DESTROY field of a semaphore which is identified by index.
*   - [IN] sem_index: Index of semaphore.
*   - [IN] delayed_destroy_value: The value to be set.
*
*END*--------------------------------------------------------------------*/
void set_sem_delayed_destroy_field_fast(_mqx_uint sem_index, bool delayed_destroy_value)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    SEM_COMPONENT_STRUCT_PTR sem_component_ptr;
    SEM_STRUCT_PTR sem_ptr;
    _mqx_max_type tmp;
    _mqx_uint result;

    /* Get current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the semaphore component pointer */
    sem_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_SEMAPHORES];
    /* Get the semaphore pointer from index */
    result = _name_find_internal_by_index(sem_component_ptr->NAME_TABLE_HANDLE, sem_index, &tmp);
    /* Get the DELAYED_DESTROY field of semaphore */
    if (result == MQX_OK)
    {
        sem_ptr = (SEM_STRUCT_PTR)tmp;
        sem_ptr->DELAYED_DESTROY = delayed_destroy_value;
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_sem_handle_owner
* Returned Value   : void *
* Comments         :
*   Get the owner (task descriptor) of semaphore connection
*   - [IN] sem_ptr: Semaphore connection
*
*END*--------------------------------------------------------------------*/
void * get_sem_handle_owner(void *sem_ptr)
{
    SEM_CONNECTION_STRUCT_PTR sem_connection_ptr;

    /* Get the semaphore connection pointer */
    sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)sem_ptr;
    /* Set task desciptor pointer of semaphore connection */
    return (void*)sem_connection_ptr->TD_PTR;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_sem_handle_owner
* Returned Value   : void
* Comments         :
*   Set the owner (task descriptor) of semaphore connection
*   - [IN] sem_ptr: Semaphore connection
*   - [IN] td_ptr: The task descriptor to be set
*
*END*--------------------------------------------------------------------*/
void set_sem_handle_owner(void *sem_ptr, void *td_ptr)
{
    SEM_CONNECTION_STRUCT_PTR sem_connection_ptr;

    /* Get the semaphore connection pointer */
    sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)sem_ptr;
    /* Set task desciptor pointer of semaphore connection */
    sem_connection_ptr->TD_PTR = (TD_STRUCT_PTR)td_ptr;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_sem_handle_valid_field
* Returned Value   : _mqx_uint
* Comments         :
*   Get the VALID field of a semaphore connection.
*   - [IN] sem_ptr: Semaphore connection
*
*END*--------------------------------------------------------------------*/
_mqx_uint get_sem_handle_valid_field(void *sem_ptr)
{
    SEM_CONNECTION_STRUCT_PTR sem_connection_ptr;

    /* Get the semaphore connection pointer */
    sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)sem_ptr;
    /* Get the VALID field of semaphore connection */
    return sem_connection_ptr->VALID;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_sem_handle_valid_field
* Returned Value   : void
* Comments         :
*   Set the VALID field of a semaphore connection.
*   - [IN] sem_ptr: Semaphore connection
*   - [IN] handle_value: The value to be set
*
*END*--------------------------------------------------------------------*/
void set_sem_handle_valid_field(void *sem_ptr, _mqx_uint handle_value)
{
    SEM_CONNECTION_STRUCT_PTR sem_connection_ptr;

    /* Get the semaphore connection pointer */
    sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)sem_ptr;
    /* Set the VALID field of semaphore connection */
    sem_connection_ptr->VALID = handle_value;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : remove_waiting_task
* Returned Value   : void
* Comments         :
*   Remove the waiting task from the waiting list of semaphore.
*   - [IN] handle_ptr: Semaphore connection
*
*END*--------------------------------------------------------------------*/
void remove_waiting_task(void *handle_ptr)
{
    SEM_STRUCT_PTR sem_ptr;
    SEM_CONNECTION_STRUCT_PTR sem_connection_ptr;

    /* Get the semaphore connection pointer */
    sem_connection_ptr = (SEM_CONNECTION_STRUCT_PTR)handle_ptr;
    /* Get the semaphore from connection */
    sem_ptr = sem_connection_ptr->SEM_PTR;
    /* Remove the waiting task from the waiting list */
    _QUEUE_REMOVE(&sem_ptr->WAITING_TASKS, sem_connection_ptr);
}
