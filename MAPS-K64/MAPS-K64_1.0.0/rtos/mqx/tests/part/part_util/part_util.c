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
*   This file contains the source functions for the Partition testing utility,
*   includes of get/set partition's components parameters.
*
*END************************************************************************/
#include <mqx_inc.h>
#include <bsp.h>
#include <partition.h>
#include <partition_prv.h>
#include "part_util.h"

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_partition_component_ptr
* Return Value  : void
* Comments      : gets the pointer to kernel component
*
*END*----------------------------------------------------------------------*/
void *get_partition_component_ptr
   (
      void
   )
{
    KERNEL_DATA_STRUCT_PTR        kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    return  (void *)kernel_data->KERNEL_COMPONENTS[KERNEL_PARTITIONS];
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_partition_component_ptr
* Return Value  : void
* Comments      : set the kernel component pointer.
*
*END*----------------------------------------------------------------------*/
void set_partition_component_ptr
   (
      void   *part_ptr
   )
{
    KERNEL_DATA_STRUCT_PTR     kernel_data;

    _GET_KERNEL_DATA(kernel_data);

    /* Disable all interrupt */
    _int_disable();

    kernel_data->KERNEL_COMPONENTS[KERNEL_PARTITIONS] = (PARTITION_COMPONENT_STRUCT_PTR)part_ptr;
    /* Enable all interrupt */
    _int_enable();
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_partition_valid_field
* Return Value  : Value of VALID field
* Comments      : get the VALID field value of partition pool
*
*END*----------------------------------------------------------------------*/
_mqx_uint  get_partition_valid_field
   (
      PARTPOOL_STRUCT_PTR partpool_ptr
   )
{
    /* Save the current value of VALID field */
    return partpool_ptr->POOL.VALID;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_partition_valid_field
* Return Value  : void
* Comments      : set the VALID field value of memory pool
*
*END*----------------------------------------------------------------------*/
void set_partition_valid_field
   (
      PARTPOOL_STRUCT_PTR partpool_ptr,
      _mqx_uint           temp
   )
{
    _int_disable();
    /* Change the task number to another value */
    partpool_ptr->POOL.VALID = temp;
    _int_enable();
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_partition_available_field
* Return Value  : Value of AVAILABLE field
* Comments      : get the AVAILABLE field value of partition pool
*
*END*----------------------------------------------------------------------*/
_mqx_uint  get_partition_available_field
   (
      PARTPOOL_STRUCT_PTR partpool_ptr
   )
{
    /* Save the current value of VALID field */
    return partpool_ptr->AVAILABLE;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_partition_available_field
* Return Value  : void
* Comments      : set the AVAILABLE field value of memory pool
*
*END*----------------------------------------------------------------------*/
void set_partition_available_field
   (
       PARTPOOL_STRUCT_PTR partpool_ptr,
       _mqx_uint           temp
   )
{
    _int_disable();
    /* Change the task number to another value */
    partpool_ptr->AVAILABLE = temp;
    _int_enable();
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_partition_checksum_field
* Return Value  : Value of VALID field
* Comments      : get the CHECKSUM field value of free list
*
*END*----------------------------------------------------------------------*/
_mqx_uint  get_partition_checksum_field
   (
       PARTPOOL_STRUCT_PTR partpool_ptr
   )
{
    INTERNAL_PARTITION_BLOCK_STRUCT_PTR block_ptr;
    block_ptr = partpool_ptr->FREE_LIST_PTR;
    /* Save the current value of CHECKSUM field */
    return block_ptr->CHECKSUM;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_partition_checksum_field
* Return Value  : void
* Comments      : set the CHECKSUM field value of free list
*
*END*----------------------------------------------------------------------*/
void set_partition_checksum_field
   (
       PARTPOOL_STRUCT_PTR partpool_ptr,
       _mqx_uint           temp
   )
{
    INTERNAL_PARTITION_BLOCK_STRUCT_PTR block_ptr;

    block_ptr = partpool_ptr->FREE_LIST_PTR;
    block_ptr->CHECKSUM = temp;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_partition_type_field
* Return Value  : void
* Comments      : set the PARTITIONTYPE field value of partition
*
*END*----------------------------------------------------------------------*/
void set_partition_type_field
   (
       PARTPOOL_STRUCT_PTR partpool_ptr,
       _mqx_uint           temp
   )
{
    partpool_ptr->PARTITION_TYPE= temp;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : get_block_checksum_field
* Return Value  : Value of VALID field
* Comments      : get the CHECKSUM field value of block
*
*END*----------------------------------------------------------------------*/
_mqx_uint  get_block_checksum_field
   (
       void   *mem_ptr
   )
{
    INTERNAL_PARTITION_BLOCK_STRUCT_PTR block_ptr;

    block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)((unsigned char *) mem_ptr - sizeof(INTERNAL_PARTITION_BLOCK_STRUCT));

    /* Save the current value of CHECKSUM field */
    return block_ptr->CHECKSUM;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name : set_block_checksum_field
* Return Value  : void
* Comments      : set the CHECKSUM field value of block
*
*END*----------------------------------------------------------------------*/
void set_block_checksum_field
   (
       void      *mem_ptr,
       _mqx_uint  temp
   )
{
    INTERNAL_PARTITION_BLOCK_STRUCT_PTR block_ptr;

    block_ptr = (INTERNAL_PARTITION_BLOCK_STRUCT_PTR)((unsigned char *) mem_ptr - sizeof(INTERNAL_PARTITION_BLOCK_STRUCT));

    block_ptr->CHECKSUM = temp;
}

