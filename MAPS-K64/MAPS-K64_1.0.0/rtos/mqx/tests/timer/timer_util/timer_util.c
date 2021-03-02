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
* $Date    : May-13-2013$
*
* Comments:
*
*   This file contains the source functions for the Timer testing utility,
*   includes of get/set timer's components parameters.
*
*END************************************************************************/
#include <mqx_inc.h>
#include <bsp.h>
#include <timer.h>
#include <timer_prv.h>
#include "timer_util.h"

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : time_adjust
* Return Value  : void
* Comments      : Adds a specified number of ms to the time
*
*END*----------------------------------------------------------------------*/

void time_adjust
   (
       TIME_STRUCT_PTR time_ptr,
       uint32_t         ms
   )
{/* Body */
    /* Add the number of ms */
    time_ptr->MILLISECONDS += ms;

    /* nomalize the time struct */
    while (time_ptr->MILLISECONDS >= 1000) {
       time_ptr->SECONDS++;
       time_ptr->MILLISECONDS -= 1000;
    } /* Endwhile */

} /* Endbody */

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_timer_component_valid_field
* Return Value  : Value of VALID field
* Comments      : get the value of VALID field of timer component structure
*
*END*----------------------------------------------------------------------*/
_mqx_uint  get_timer_component_valid_field
   (
       void
   )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    TIMER_COMPONENT_STRUCT_PTR timer_comp_ptr;

    /* Get the current kernel data */
    _GET_KERNEL_DATA(kernel_data);

    timer_comp_ptr = (TIMER_COMPONENT_STRUCT_PTR)kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];
    /* Save the current value of VALID field */
    return timer_comp_ptr->VALID;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_timer_component_valid_field
* Return Value  : void
* Comments      : set the VALID field value of timer component structure
*
*END*----------------------------------------------------------------------*/
void set_timer_component_valid_field
   (
       _mqx_uint  temp
   )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    TIMER_COMPONENT_STRUCT_PTR timer_comp_ptr;

    /* Get the current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    timer_comp_ptr = (TIMER_COMPONENT_STRUCT_PTR)kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];

    _int_disable();
    /* Change the task number to another value */
    timer_comp_ptr->VALID = temp;
    _int_enable();
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_timer_component_ptr
* Return Value  : void
* Comments      : gets the pointer to kernel component
*
*END*----------------------------------------------------------------------*/
void *get_timer_component_ptr
   (
       void
   )
{
    KERNEL_DATA_STRUCT_PTR  kernel_data;

    _GET_KERNEL_DATA(kernel_data);
    /* return timer component in kernel data */
    return  (void *)kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];

}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_timer_component_ptr
* Return Value  : void
* Comments      : set the kernel component pointer.
*
*END*----------------------------------------------------------------------*/
void set_timer_component_ptr
   (
       void   *tim_ptr
   )
{
    KERNEL_DATA_STRUCT_PTR  kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    /* Disable all interrupt for critical access */
    _int_disable();

    kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER] = (TIMER_COMPONENT_STRUCT_PTR)tim_ptr;

    /* Enable all interrupt */
    _int_enable();
}

/*FUNCTION*-----------------------------------------------------
*
* Function Name : get_timer_queue_ptr
* Comments      : get the pointer to queue
*
*END*-----------------------------------------------------*/
void *get_timer_queue_ptr
    (
        void
    )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    QUEUE_ELEMENT_STRUCT_PTR   element_ptr;
    TIMER_COMPONENT_STRUCT_PTR timer_component_ptr;

    _GET_KERNEL_DATA(kernel_data);
    timer_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];

    QUEUE_STRUCT_PTR q_ptr = (QUEUE_STRUCT_PTR)&timer_component_ptr->ELAPSED_TIMER_ENTRIES;

    element_ptr = q_ptr->NEXT;

    return element_ptr->PREV;
}

/*FUNCTION*-----------------------------------------------------
*
* Function Name : set_timer_queue_ptr
* Comments      : set the pointer to queue
*
*END*-----------------------------------------------------*/
void set_timer_queue_ptr
    (
       void    *save_elem
    )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    QUEUE_ELEMENT_STRUCT_PTR   element_ptr;
    TIMER_COMPONENT_STRUCT_PTR timer_component_ptr;

    _GET_KERNEL_DATA(kernel_data);
    timer_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];

    QUEUE_STRUCT_PTR q_ptr = (QUEUE_STRUCT_PTR)&timer_component_ptr->ELAPSED_TIMER_ENTRIES;

    element_ptr = q_ptr->NEXT;

    _int_disable();

    element_ptr->PREV = save_elem;

    _int_enable();
}

/*FUNCTION*-----------------------------------------------------
*
* Function Name : get_timer_valid_field
* Comments      : get the value of VALID field
*
*END*-----------------------------------------------------*/
_mqx_uint get_timer_valid_field
    (
        void
    )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    TIMER_ENTRY_STRUCT_PTR     element_ptr;
    TIMER_COMPONENT_STRUCT_PTR timer_component_ptr;

    _GET_KERNEL_DATA(kernel_data);
    timer_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];

    QUEUE_STRUCT_PTR q_ptr = (void *)&timer_component_ptr->ELAPSED_TIMER_ENTRIES;
    element_ptr = (void *)q_ptr->NEXT;

    return element_ptr->VALID;

}

/*FUNCTION*-----------------------------------------------------
*
* Function Name : set_timer_valid_field
* Comments      : set the value of VALID field
*
*END*-----------------------------------------------------*/
void set_timer_valid_field
    (
       _mqx_uint save_elem
    )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;
    TIMER_ENTRY_STRUCT_PTR     element_ptr;
    TIMER_COMPONENT_STRUCT_PTR timer_component_ptr;

    _GET_KERNEL_DATA(kernel_data);
    timer_component_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_TIMER];

    QUEUE_STRUCT_PTR q_ptr = (QUEUE_STRUCT_PTR)&timer_component_ptr->ELAPSED_TIMER_ENTRIES;

    element_ptr = (void *)q_ptr->NEXT;

    _int_disable();

    element_ptr->VALID = save_elem;

    _int_enable();
}
