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
* $Date    : Apr-12-2013$
*
* Comments:
*
*   This file contains the source functions for the Message testing utility,
*   includes of get/set message's components parameters.
*
*END************************************************************************/
#include "msg_util.h"

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : get_msg_component
* Returned Value   : pointer
* Comments         :
*   Gets the system message component pointer
*
*END*--------------------------------------------------------------------*/
void *get_msg_component(void)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;
    void   *msg_comp_ptr;

    /* Get the current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Get the system message component's pointer */
    msg_comp_ptr = kernel_data->KERNEL_COMPONENTS[KERNEL_MESSAGES];
    return msg_comp_ptr;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_msg_component
* Returned Value   : void
* Comments         :
*   Sets the system message component pointer
*   - [IN] msg_comp_ptr: The pointer to be set.
*
*END*--------------------------------------------------------------------*/
void set_msg_component(void *msg_comp_ptr)
{
    KERNEL_DATA_STRUCT_PTR kernel_data;

    /* Get the current kernel data */
    _GET_KERNEL_DATA(kernel_data);
    /* Disable all interrupts */
    _int_disable();
    /* Set the system message component's pointer */
    kernel_data->KERNEL_COMPONENTS[KERNEL_MESSAGES] = msg_comp_ptr;
    /* Enable all interrupts */
    _int_enable();
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_pool_valid
* Returned Value   : void
* Comments         :
*   Sets the value of VALID field of message pool
*   - [IN] msg_pool_ptr: Indicate the memory pool that will be changed
*   - [IN] value: The value to be set
*
*END*--------------------------------------------------------------------*/
void set_pool_valid(MSGPOOL_STRUCT_PTR msg_pool_ptr, uint16_t value)
{
    /* Set the VALID field's value of message pool */
    msg_pool_ptr->VALID = value;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : set_msg_valid
* Returned Value   : void
* Comments         :
*   Sets the value of VALID field of a message
*   - [IN] msg_ptr: Indicate the message that will be changed
*   - [IN] value: The value to be set
*
*END*--------------------------------------------------------------------*/
void set_msg_valid(MESSAGE_HEADER_STRUCT_PTR msg_ptr, uint16_t value)
{
    INTERNAL_MESSAGE_STRUCT_PTR imsg_ptr;
    /* Get message's internal struct */
    imsg_ptr = GET_INTERNAL_MESSAGE_PTR(msg_ptr);
    /* Set the VALID field's value of message */
    imsg_ptr->VALID = value;
}
