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
* $Version : 4.0.1.0$
* $Date    : Jun-1-2013$
*
* Comments:
*
*   This file contains the source functions for the event testing utility,
*   includes of get/set event's components parameters.
*
*END************************************************************************/
#include <mqx_inc.h>
#include <bsp.h>
#include <name.h>
#include <name_prv.h>
#include <event.h>
#include <event_prv.h>
#include "event_util.h"

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_event_component_capacity
* Return Value  : the maximum number of event groups can be created.
* Comments      :
*
*END*----------------------------------------------------------------------*/
_mqx_uint get_event_component_capacity
   (
      void
   )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    EVENT_COMPONENT_STRUCT_PTR event_component_ptr;

    _GET_KERNEL_DATA(kernel_data);
    event_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
    return  event_component_ptr->MAXIMUM_NUMBER;

}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_event_component_valid_field
* Return Value  : Value of VALID field
* Comments      : get the value of VALID field of event component structure
*
*END*----------------------------------------------------------------------*/
_mqx_uint  get_event_component_valid_field
   (
       void
   )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;

    EVENT_COMPONENT_STRUCT_PTR event_comp_ptr;

    /* Get the current kernel data */
    _GET_KERNEL_DATA(kernel_data);

    event_comp_ptr = (EVENT_COMPONENT_STRUCT_PTR)kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
    /* Save the current value of VALID field */
    return event_comp_ptr->VALID;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_event_component_valid_field
* Return Value  : void
* Comments      : set the VALID field value of event component structure
*
*END*----------------------------------------------------------------------*/
void set_event_component_valid_field
   (
       _mqx_uint  temp
   )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    EVENT_COMPONENT_STRUCT_PTR event_comp_ptr;

    /* Get the current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    event_comp_ptr = (EVENT_COMPONENT_STRUCT_PTR)kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];

    _int_disable();
    /* Change the task number to another value */
    event_comp_ptr->VALID = temp;
    _int_enable();
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_event_component_ptr
* Return Value  : Value of VALID field
* Comments      : get the value of VALID field of event component structure
*
*END*----------------------------------------------------------------------*/
void *get_event_component_ptr
   (
      void
   )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    EVENT_COMPONENT_STRUCT_PTR event_component_ptr;

    _GET_KERNEL_DATA(kernel_data);
    event_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS];
    return  event_component_ptr;

}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_event_component_ptr
* Return Value  : void
* Comments      : set the pointer to event component
*
*END*----------------------------------------------------------------------*/
void set_event_component_ptr
   (
      void   *evt_ptr
   )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    _GET_KERNEL_DATA(kernel_data);

    _int_disable();
    kernel_data->KERNEL_COMPONENTS[KERNEL_EVENTS] = (EVENT_COMPONENT_STRUCT_PTR)evt_ptr;
    _int_enable();

}
/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_event_handle_valid_field
* Return Value  : void
* Comments      : get the value of VALID field
*
*END*----------------------------------------------------------------------*/
_mqx_uint get_event_handle_valid_field
   (
      void   *users_event_ptr
   )
{
    EVENT_CONNECTION_STRUCT_PTR event_connection_ptr = users_event_ptr;

    return event_connection_ptr->VALID;

}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_event_handle_valid_field
* Return Value  : void
* Comments      : set the value of VALID field
*
*END*----------------------------------------------------------------------*/
void set_event_handle_valid_field
   (
      void      *users_event_ptr,
      _mqx_uint  handle_field
   )
{
    EVENT_CONNECTION_STRUCT_PTR event_connection_ptr = users_event_ptr;
    _int_disable();
    event_connection_ptr->VALID = handle_field;
    _int_enable();
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_event_handle_valid_field
* Return Value  : void
* Comments      : get the value of VALID field
*
*END*----------------------------------------------------------------------*/
_mqx_uint get_event_valid_field
   (
      void   *users_event_ptr
   )
{
    EVENT_CONNECTION_STRUCT_PTR event_connection_ptr = users_event_ptr;
    EVENT_STRUCT_PTR            event_ptr;

    event_ptr = event_connection_ptr->EVENT_PTR;

    return event_ptr->VALID;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_event_handle_valid_field
* Return Value  : void
* Comments      : set the value of VALID field
*
*END*----------------------------------------------------------------------*/
void set_event_valid_field
   (
      void      *users_event_ptr,
      _mqx_uint  handle_field
   )
{
    EVENT_CONNECTION_STRUCT_PTR event_connection_ptr = users_event_ptr;
    EVENT_STRUCT_PTR            event_ptr;

    event_ptr = event_connection_ptr->EVENT_PTR;
    _int_disable();
    event_ptr->VALID = handle_field;
    _int_enable();
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_event_queue_ptr
* Return Value  : void
* Comments      : gets the pointer to the event queue
*
*END*----------------------------------------------------------------------*/
void *get_event_queue_ptr
   (
       void   *users_event_ptr
   )
{
    EVENT_CONNECTION_STRUCT_PTR event_connection_ptr = users_event_ptr;
    QUEUE_ELEMENT_STRUCT_PTR    element_ptr;
    EVENT_STRUCT_PTR            event_ptr;
    QUEUE_STRUCT_PTR            q_ptr;

    event_ptr = event_connection_ptr->EVENT_PTR;
    q_ptr = &event_ptr->WAITING_TASKS;
    element_ptr = q_ptr->NEXT;

    return (void *)element_ptr->PREV;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_event_queue_ptr
* Return Value  : void
* Comments      : sets the pointer to the event queue
*
*END*----------------------------------------------------------------------*/
void set_event_queue_ptr
   (
       void   *users_event_ptr,
       void   *temp_ptr
   )
{
    EVENT_CONNECTION_STRUCT_PTR event_connection_ptr = users_event_ptr;
    QUEUE_ELEMENT_STRUCT_PTR    element_ptr;
    EVENT_STRUCT_PTR            event_ptr;
    QUEUE_STRUCT_PTR            q_ptr;

    event_ptr = event_connection_ptr->EVENT_PTR;
    q_ptr = &event_ptr->WAITING_TASKS;

    element_ptr = q_ptr->NEXT;

    _int_disable();
    element_ptr->PREV = temp_ptr;
    _int_enable();
}
