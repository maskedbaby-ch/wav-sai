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
*   PACKED_UNION_BEGIN/PACKED_UNION_END macros testing.
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

PACKED_UNION_BEGIN
union _my_union_packed
{
   uint16_t  A;
   struct {
      char  X;
      char  Y;
      char  Z;
   } B;
} PACKED_UNION_END ;

typedef union _my_union_packed _MY_UNION_PACKED;

union _my_union
{
   uint16_t  A;
   struct {
      char  X;
      char  Y;
      char  Z;
   } B;
} ;

typedef union _my_union _MY_UNION;

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Testing PACKED_UNION_BEGIN/PACKED_UNION_END
*                - Declare two unions, both with the same members. One 
*                  union is packed while the second union is not packed.
*                - Compare sizes of both union.
*                - Check the correct size of the packed union.
*                - Check correct offsets within the packed union.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    union _my_union_packed my_packed_union[2];
    union _my_union        my_union[2];

#if PRINT_ON
    printf("Size of the packed union : %i\n", sizeof(my_packed_union));
    printf("Size of the non-packed union: %i\n", sizeof(my_union));
#endif

    EU_ASSERT_REF_TC_MSG( tc_1_main_task, sizeof(my_packed_union) < sizeof(my_union), "TEST #1: 1.1 Checking that the size of the packed union is lower than the not packed union");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, sizeof(my_packed_union) == 6*sizeof(char), "TEST #1: 1.2 Checking correct size of the packed union");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, (_mqx_uint)&(((_MY_UNION_PACKED*)0)->B.X) == 0, "TEST #1: 1.3 Checking correct X member offset from the start of the packed union");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, (_mqx_uint)&(((_MY_UNION_PACKED*)0)->B.Y) == sizeof(char), "TEST #1: 1.4 Checking correct Y member offset from the start of the packed union");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, (_mqx_uint)&(((_MY_UNION_PACKED*)0)->B.Z) == 2*sizeof(char), "TEST #1: 1.5 Checking correct Z member offset from the start of the packed union");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((_mqx_uint)&my_packed_union[1] - (_mqx_uint)&my_packed_union[0]) == 3*sizeof(char), "TEST #1: 1.6 Checking correct offset between my_packed_union[0] and my_packed_union[1]");
}


//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_packed_union)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - PACKED_UNION_BEGIN/PACKED_UNION_END macros usage"),
 EU_TEST_SUITE_END(suite_packed_union)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_packed_union, " - PACKED_UNION_BEGIN/PACKED_UNION_END suite")
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
