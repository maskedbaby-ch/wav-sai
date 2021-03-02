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
* $Version : 3.8.2.0$
* $Date    : Oct-2-2012$
*
* Comments:
*
*   This file contains the source code for 
*   PACKED_STRUCT_BEGIN/PACKED_STRUCT_END macros testing.
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <string.h>

#include "test.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   /* Task Index,  Function,       Stack,  Priority,  Name,         Attributes,           Param,  Time Slice */
    { MAIN_TASK,   main_task,      2000,   10,        "Main",       MQX_AUTO_START_TASK,  0,      0 },
    { 0 }
};

PACKED_STRUCT_BEGIN
struct _my_struct_packed
{
   uint8_t   A;
   uint16_t  B;
   uint32_t  C;
   uint8_t   D;
   uint32_t  E;
   uint16_t  F;
   bool      G;
} PACKED_STRUCT_END;

typedef struct _my_struct_packed MY_STRUCT_PACKED, * MY_STRUCT_PACKED_PTR;

typedef struct _my_struct
{
   uint8_t   A;
   uint16_t  B;
   uint32_t  C;
   uint8_t   D;
   uint32_t  E;
   uint16_t  F;
   bool      G;
} MY_STRUCT, * MY_STRUCT_PTR;

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Testing PACKED_STRUCT_BEGIN/PACKED_STRUCT_END
*                - Declare two structures, both with the same members. One 
*                  structure is packed while the second structure is not packed.
*                - Compare sizes of both structures.
*                - Check the correct size of the packed structure.
*                - Check correct offsets within the packed structure.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    MY_STRUCT_PACKED   my_structure_packed;
    MY_STRUCT          my_structure_notpacked;

#if PRINT_ON
    printf("Size of the packed struct: %i\n", sizeof(my_structure_packed));
    printf("Size of the non-packed struct: %i\n", sizeof(my_structure_notpacked));
#endif

    my_structure_packed.G = TRUE;
    
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, sizeof(my_structure_packed) < sizeof(my_structure_notpacked), "TEST #1: 1.1 Checking that the size of the packed structure is lower than the not packed structure");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, sizeof(my_structure_packed) == 2*sizeof(uint8_t) + 2*sizeof(uint16_t) + 2*sizeof(uint32_t) + sizeof(bool), "TEST #1: 1.2 Checking correct size of the packed structure");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, (_mqx_uint)&(((MY_STRUCT_PACKED*)0)->B) == sizeof(uint8_t), "TEST #1: 1.3 Checking correct B member offset from the start of the packed structure");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, (_mqx_uint)&(((MY_STRUCT_PACKED*)0)->C) == sizeof(uint8_t) + sizeof(uint16_t), "TEST #1: 1.4 Checking correct C member offset from the start of the packed structure");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, (_mqx_uint)&(((MY_STRUCT_PACKED*)0)->D) == sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t), "TEST #1: 1.5 Checking correct D member offset from the start of the packed structure");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, (_mqx_uint)&(((MY_STRUCT_PACKED*)0)->E) == 2*sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t), "TEST #1: 1.6 Checking correct E member offset from the start of the packed structure");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, (_mqx_uint)&(((MY_STRUCT_PACKED*)0)->F) == 2*sizeof(uint8_t) + sizeof(uint16_t) + 2*sizeof(uint32_t), "TEST #1: 1.7 Checking correct F member offset from the start of the packed structure");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, (_mqx_uint)&(((MY_STRUCT_PACKED*)0)->G) == 2*sizeof(uint8_t) + 2*sizeof(uint16_t) + 2*sizeof(uint32_t), "TEST #1: 1.8 Checking correct G member offset from the start of the packed structure");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, TRUE == *(bool *)((_mqx_uint)&my_structure_packed + (_mqx_uint)(2*sizeof(uint8_t) + 2*sizeof(uint16_t) + 2*sizeof(uint32_t))), "TEST #1: 1.9 Checking correct G member value");
}

//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_packed_struct)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - PACKED_STRUCT_BEGIN/PACKED_STRUCT_END macros usage"),
 EU_TEST_SUITE_END(suite_packed_struct)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_packed_struct, " - PACKED_STRUCT_BEGIN/PACKED_STRUCT_END suite")
 EU_TEST_REGISTRY_END()

void main_task
   (
      /* [IN] The MQX task parameter */
      uint32_t param
   )
{ /* Body */
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* Endbody */
