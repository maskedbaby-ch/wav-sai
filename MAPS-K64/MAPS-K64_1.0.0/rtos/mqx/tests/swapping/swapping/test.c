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
*   This file contains the source code for byte order convert macros testing.
*   (converting values between host and big-/little-endian byte order)
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

typedef struct _my_struct
{
   uint8_t   A[1];
   uint8_t   B[2];
   uint8_t   C[4];
   uint16_t  D[1];
   uint32_t  E[1];
   uint8_t   F[8];
} MY_STRUCT, * MY_STRUCT_PTR;

/*TASK*--------------------------------------------------------------------
*
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Testing all swapping macros we have in mqx.h
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
    MY_STRUCT          *my_structure_ptr;
    uint16_t           temp_short = SHORT_B1_PTRN<<8 | SHORT_B0_PTRN;
    uint32_t           temp_long = LONG_B3_PTRN<<24 | LONG_B2_PTRN<<16 | LONG_B1_PTRN<<8 | LONG_B0_PTRN;
    uint32_t           i;

    my_structure_ptr = (MY_STRUCT_PTR) _mem_alloc_system_zero(sizeof(MY_STRUCT));

    /* Unaligned memory host to network swap testing */
    mqx_htons(&my_structure_ptr->B, temp_short);
    mqx_htonl(&my_structure_ptr->C, temp_long);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((my_structure_ptr->B[0] == SHORT_B1_PTRN) && (my_structure_ptr->B[1] == SHORT_B0_PTRN)), "TEST #1: 1.1 mqx_htons test");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((my_structure_ptr->C[0] == LONG_B3_PTRN) && (my_structure_ptr->C[1] == LONG_B2_PTRN) && (my_structure_ptr->C[2] == LONG_B1_PTRN) && (my_structure_ptr->C[3] == LONG_B0_PTRN)), "TEST #1: 1.2 mqx_htonl test");
    
    for(i=0; i<4; i++) {
    	mqx_htons(&my_structure_ptr->F[i], temp_short);
    	EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((my_structure_ptr->F[i] == SHORT_B1_PTRN) && (my_structure_ptr->F[i+1] == SHORT_B0_PTRN)), "TEST #1: 1.3 mqx_htons test in a loop");
    }

    for(i=0; i<4; i++) {
    	mqx_htonl(&my_structure_ptr->F[i], temp_long);
    	EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((my_structure_ptr->F[i] == LONG_B3_PTRN) && (my_structure_ptr->F[i+1] == LONG_B2_PTRN) && (my_structure_ptr->F[i+2] == LONG_B1_PTRN) && (my_structure_ptr->F[i+3] == LONG_B0_PTRN)), "TEST #1: 1.4 mqx_htonl test in a loop");
    }

    /* Unaligned memory network to host swap testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    *(uint16_t*)&my_structure_ptr->B[0] = mqx_ntohs(&temp_short);
    *(uint32_t*)&my_structure_ptr->C[0] = mqx_ntohl(&temp_long);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((my_structure_ptr->B[0] == SHORT_B1_PTRN) && (my_structure_ptr->B[1] == SHORT_B0_PTRN)), "TEST #1: 1.5 mqx_ntohs test");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((my_structure_ptr->C[0] == LONG_B3_PTRN) && (my_structure_ptr->C[1] == LONG_B2_PTRN) && (my_structure_ptr->C[2] == LONG_B1_PTRN) && (my_structure_ptr->C[3] == LONG_B0_PTRN)), "TEST #1: 1.6 mqx_ntohl test");
    
    for(i=0; i<4; i++) {
    	*(uint16_t*)&my_structure_ptr->F[i] = mqx_ntohs(&temp_short);
    	EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((my_structure_ptr->F[i] == SHORT_B1_PTRN) && (my_structure_ptr->F[i+1] == SHORT_B0_PTRN)), "TEST #1: 1.7 mqx_ntohs test in a loop");
    }

    for(i=0; i<4; i++) {
    	*(uint32_t*)&my_structure_ptr->F[i] = mqx_ntohl(&temp_long);
    	EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((my_structure_ptr->F[i] == LONG_B3_PTRN) && (my_structure_ptr->F[i+1] == LONG_B2_PTRN) && (my_structure_ptr->F[i+2] == LONG_B1_PTRN) && (my_structure_ptr->F[i+3] == LONG_B0_PTRN)), "TEST #1: 1.8 mqx_ntohl test in a loop");
    }

    /* Unaligned memory host to disk swap testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    mqx_htods(&my_structure_ptr->B, temp_short);
    mqx_htodl(&my_structure_ptr->C, temp_long);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((my_structure_ptr->B[0] == SHORT_B0_PTRN) && (my_structure_ptr->B[1] == SHORT_B1_PTRN)), "TEST #1: 1.9 mqx_htods test");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((my_structure_ptr->C[0] == LONG_B0_PTRN) && (my_structure_ptr->C[1] == LONG_B1_PTRN) && (my_structure_ptr->C[2] == LONG_B2_PTRN) && (my_structure_ptr->C[3] == LONG_B3_PTRN)), "TEST #1: 1.10 mqx_htodl test");
    
    for(i=0; i<4; i++) {
    	mqx_htods(&my_structure_ptr->F[i], temp_short);
    	EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((my_structure_ptr->F[i] == SHORT_B0_PTRN) && (my_structure_ptr->F[i+1] == SHORT_B1_PTRN)), "TEST #1: 1.11 mqx_htods test in a loop");
    }

    for(i=0; i<4; i++) {
    	mqx_htodl(&my_structure_ptr->F[i], temp_long);
    	EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((my_structure_ptr->F[i] == LONG_B0_PTRN) && (my_structure_ptr->F[i+1] == LONG_B1_PTRN) && (my_structure_ptr->F[i+2] == LONG_B2_PTRN) && (my_structure_ptr->F[i+3] == LONG_B3_PTRN)), "TEST #1: 1.12 mqx_htodl test in a loop");
    }

    /* Unaligned memory disk to host swap testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    *(uint16_t*)&my_structure_ptr->B[0] = mqx_dtohs(&temp_short);
    *(uint32_t*)&my_structure_ptr->C[0] = mqx_dtohl(&temp_long);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((my_structure_ptr->B[0] == SHORT_B0_PTRN) && (my_structure_ptr->B[1] == SHORT_B1_PTRN)), "TEST #1: 1.13 mqx_dtohs test");
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((my_structure_ptr->C[0] == LONG_B0_PTRN) && (my_structure_ptr->C[1] == LONG_B1_PTRN) && (my_structure_ptr->C[2] == LONG_B2_PTRN) && (my_structure_ptr->C[3] == LONG_B3_PTRN)), "TEST #1: 1.14 mqx_dtohl test");
    
    for(i=0; i<4; i++) {
    	*(uint16_t*)&my_structure_ptr->F[i] = mqx_dtohs(&temp_short);
    	EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((my_structure_ptr->F[i] == SHORT_B0_PTRN) && (my_structure_ptr->F[i+1] == SHORT_B1_PTRN)), "TEST #1: 1.15 mqx_dtohs test in a loop");
    }

    for(i=0; i<4; i++) {
    	*(uint32_t*)&my_structure_ptr->F[i] = mqx_dtohl(&temp_long);
    	EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((my_structure_ptr->F[i] == LONG_B0_PTRN) && (my_structure_ptr->F[i+1] == LONG_B1_PTRN) && (my_structure_ptr->F[i+2] == LONG_B2_PTRN) && (my_structure_ptr->F[i+3] == LONG_B3_PTRN)), "TEST #1: 1.16 mqx_dtohl test in a loop");
    }

    /* HOST_TO_BE_SHORT macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->D[0] = HOST_TO_BE_SHORT(temp_short);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->D[0] == SHORT_B1_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->D[0]) == SHORT_B0_PTRN)), "TEST #1: 1.17 HOST_TO_BE_SHORT test");

    /* HOST_TO_BE_SHORT_CONST macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->D[0] = HOST_TO_BE_SHORT_CONST(SHORT_B1_PTRN<<8 | SHORT_B0_PTRN);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->D[0] == SHORT_B1_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->D[0]) == SHORT_B0_PTRN)), "TEST #1: 1.18 HOST_TO_BE_SHORT_CONST test");

    /* HOST_TO_BE_UNALIGNED_SHORT macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    HOST_TO_BE_UNALIGNED_SHORT(temp_short, &my_structure_ptr->D[0]);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->D[0] == SHORT_B1_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->D[0]) == SHORT_B0_PTRN)), "TEST #1: 1.19 HOST_TO_BE_UNALIGNED_SHORT test");

    
    /* HOST_TO_LE_SHORT macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->D[0] = HOST_TO_LE_SHORT(temp_short);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->D[0] == SHORT_B0_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->D[0]) == SHORT_B1_PTRN)), "TEST #1: 1.20 HOST_TO_LE_SHORT test");

    /* HOST_TO_LE_SHORT_CONST macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->D[0] = HOST_TO_LE_SHORT_CONST(SHORT_B1_PTRN<<8 | SHORT_B0_PTRN);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->D[0] == SHORT_B0_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->D[0]) == SHORT_B1_PTRN)), "TEST #1: 1.21 HOST_TO_LE_SHORT_CONST test");

    /* HOST_TO_LE_UNALIGNED_SHORT macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    HOST_TO_LE_UNALIGNED_SHORT(temp_short, &my_structure_ptr->D[0]);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->D[0] == SHORT_B0_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->D[0]) == SHORT_B1_PTRN)), "TEST #1: 1.22 HOST_TO_LE_UNALIGNED_SHORT test");


    /* SHORT_BE_TO_HOST macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->D[0] = SHORT_BE_TO_HOST(temp_short);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->D[0] == SHORT_B1_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->D[0]) == SHORT_B0_PTRN)), "TEST #1: 1.23 SHORT_BE_TO_HOST test");

    /* SHORT_BE_TO_HOST_CONST macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->D[0] = SHORT_BE_TO_HOST_CONST(SHORT_B1_PTRN<<8 | SHORT_B0_PTRN);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->D[0] == SHORT_B1_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->D[0]) == SHORT_B0_PTRN)), "TEST #1: 1.24 SHORT_BE_TO_HOST_CONST test");

    /* SHORT_UNALIGNED_BE_TO_HOST macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->D[0] = SHORT_UNALIGNED_BE_TO_HOST(&temp_short);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->D[0] == SHORT_B1_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->D[0]) == SHORT_B0_PTRN)), "TEST #1: 1.25 SHORT_UNALIGNED_BE_TO_HOST test");

    
    /* SHORT_LE_TO_HOST macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->D[0] = SHORT_LE_TO_HOST(temp_short);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->D[0] == SHORT_B0_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->D[0]) == SHORT_B1_PTRN)), "TEST #1: 1.26 SHORT_LE_TO_HOST test");

    /* SHORT_LE_TO_HOST_CONST macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->D[0] = SHORT_LE_TO_HOST_CONST(SHORT_B1_PTRN<<8 | SHORT_B0_PTRN);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->D[0] == SHORT_B0_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->D[0]) == SHORT_B1_PTRN)), "TEST #1: 1.27 SHORT_LE_TO_HOST_CONST test");

    /* SHORT_UNALIGNED_LE_TO_HOST macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->D[0] = SHORT_UNALIGNED_LE_TO_HOST(&temp_short);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->D[0] == SHORT_B0_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->D[0]) == SHORT_B1_PTRN)), "TEST #1: 1.28 SHORT_UNALIGNED_LE_TO_HOST test");



    /* HOST_TO_BE_LONG macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->E[0] = HOST_TO_BE_LONG(temp_long);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->E[0] == LONG_B3_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B2_PTRN) && (*(2+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B1_PTRN) && (*(3+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B0_PTRN)), "TEST #1: 1.29 HOST_TO_BE_LONG test");

    /* HOST_TO_BE_LONG_CONST macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->E[0] = HOST_TO_BE_LONG_CONST(LONG_B3_PTRN<<24 | LONG_B2_PTRN<<16 | LONG_B1_PTRN<<8 | LONG_B0_PTRN);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->E[0] == LONG_B3_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B2_PTRN) && (*(2+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B1_PTRN) && (*(3+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B0_PTRN)), "TEST #1: 1.30 HOST_TO_BE_LONG_CONST test");

    /* HOST_TO_BE_UNALIGNED_LONG macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    HOST_TO_BE_UNALIGNED_LONG(temp_long, &my_structure_ptr->E[0]);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->E[0] == LONG_B3_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B2_PTRN) && (*(2+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B1_PTRN) && (*(3+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B0_PTRN)), "TEST #1: 1.31 HOST_TO_BE_UNALIGNED_LONG test");

    
    /* HOST_TO_LE_LONG macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->E[0] = HOST_TO_LE_LONG(temp_long);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->E[0] == LONG_B0_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B1_PTRN) && (*(2+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B2_PTRN) && (*(3+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B3_PTRN)), "TEST #1: 1.32 HOST_TO_LE_LONG test");

    /* HOST_TO_LE_LONG_CONST macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->E[0] = HOST_TO_LE_LONG_CONST(LONG_B3_PTRN<<24 | LONG_B2_PTRN<<16 | LONG_B1_PTRN<<8 | LONG_B0_PTRN);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->E[0] == LONG_B0_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B1_PTRN) && (*(2+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B2_PTRN) && (*(3+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B3_PTRN)), "TEST #1: 1.33 HOST_TO_LE_LONG_CONST test");

    /* HOST_TO_LE_UNALIGNED_LONG macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    HOST_TO_LE_UNALIGNED_LONG(temp_long, &my_structure_ptr->E[0]);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->E[0] == LONG_B0_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B1_PTRN) && (*(2+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B2_PTRN) && (*(3+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B3_PTRN)), "TEST #1: 1.34 HOST_TO_LE_UNALIGNED_LONG test");


    /* LONG_BE_TO_HOST macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->E[0] = LONG_BE_TO_HOST(temp_long);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->E[0] == LONG_B3_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B2_PTRN) && (*(2+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B1_PTRN) && (*(3+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B0_PTRN)), "TEST #1: 1.35 LONG_BE_TO_HOST test");

    /* LONG_BE_TO_HOST_CONST macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->E[0] = LONG_BE_TO_HOST_CONST(LONG_B3_PTRN<<24 | LONG_B2_PTRN<<16 | LONG_B1_PTRN<<8 | LONG_B0_PTRN);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->E[0] == LONG_B3_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B2_PTRN) && (*(2+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B1_PTRN) && (*(3+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B0_PTRN)), "TEST #1: 1.36 LONG_BE_TO_HOST_CONST test");

    /* LONG_UNALIGNED_BE_TO_HOST macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->E[0] = LONG_UNALIGNED_BE_TO_HOST(&temp_long);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->E[0] == LONG_B3_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B2_PTRN) && (*(2+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B1_PTRN) && (*(3+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B0_PTRN)), "TEST #1: 1.37 LONG_UNALIGNED_BE_TO_HOST test");

    
    /* LONG_LE_TO_HOST macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->E[0] = LONG_LE_TO_HOST(temp_long);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->E[0] == LONG_B0_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B1_PTRN) && (*(2+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B2_PTRN) && (*(3+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B3_PTRN)), "TEST #1: 1.38 LONG_LE_TO_HOST test");

    /* LONG_LE_TO_HOST_CONST macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->E[0] = LONG_LE_TO_HOST_CONST(LONG_B3_PTRN<<24 | LONG_B2_PTRN<<16 | LONG_B1_PTRN<<8 | LONG_B0_PTRN);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->E[0] == LONG_B0_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B1_PTRN) && (*(2+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B2_PTRN) && (*(3+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B3_PTRN)), "TEST #1: 1.39 LONG_LE_TO_HOST_CONST test");

    /* LONG_UNALIGNED_LE_TO_HOST macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->E[0] = LONG_UNALIGNED_LE_TO_HOST(&temp_long);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->E[0] == LONG_B0_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B1_PTRN) && (*(2+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B2_PTRN) && (*(3+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B3_PTRN)), "TEST #1: 1.40 LONG_UNALIGNED_LE_TO_HOST test");




    /* htons macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->D[0] = htons(temp_short);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->D[0] == SHORT_B1_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->D[0]) == SHORT_B0_PTRN)), "TEST #1: 1.41 htons test");

    /* htonl macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->E[0] = htonl(temp_long);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->E[0] == LONG_B3_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B2_PTRN) && (*(2+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B1_PTRN) && (*(3+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B0_PTRN)), "TEST #1: 1.42 htonl test");

    /* ntohs macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->D[0] = ntohs(temp_short);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->D[0] == SHORT_B1_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->D[0]) == SHORT_B0_PTRN)), "TEST #1: 1.43 ntohs test");

    /* ntohl macro testing */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->E[0] = ntohl(temp_long);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->E[0] == LONG_B3_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B2_PTRN) && (*(2+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B1_PTRN) && (*(3+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B0_PTRN)), "TEST #1: 1.44 ntohl test");


    
    /* htons macro testing with const as input */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->D[0] = htons(SHORT_B1_PTRN<<8 | SHORT_B0_PTRN);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->D[0] == SHORT_B1_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->D[0]) == SHORT_B0_PTRN)), "TEST #1: 1.45 htons test with const as input");

    /* htonl macro testing with const as input */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->E[0] = htonl(LONG_B3_PTRN<<24 | LONG_B2_PTRN<<16 | LONG_B1_PTRN<<8 | LONG_B0_PTRN);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->E[0] == LONG_B3_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B2_PTRN) && (*(2+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B1_PTRN) && (*(3+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B0_PTRN)), "TEST #1: 1.46 htonl test with const as input");

    /* ntohs macro testing with const as input */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->D[0] = ntohs(SHORT_B1_PTRN<<8 | SHORT_B0_PTRN);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->D[0] == SHORT_B1_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->D[0]) == SHORT_B0_PTRN)), "TEST #1: 1.47 ntohs test with const as input");

    /* ntohl macro testing with const as input */
    _mem_zero((void *)my_structure_ptr, (_mem_size)sizeof(MY_STRUCT));
    my_structure_ptr->E[0] = ntohl(LONG_B3_PTRN<<24 | LONG_B2_PTRN<<16 | LONG_B1_PTRN<<8 | LONG_B0_PTRN);
    EU_ASSERT_REF_TC_MSG( tc_1_main_task, ((*(uint8_t*)&my_structure_ptr->E[0] == LONG_B3_PTRN) && (*(1+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B2_PTRN) && (*(2+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B1_PTRN) && (*(3+(uint8_t*)&my_structure_ptr->E[0]) == LONG_B0_PTRN)), "TEST #1: 1.48 ntohl test with const as input");
}

/*------------------------------------------------------------------------*/
/* Define Test Suite */
 EU_TEST_SUITE_BEGIN(suite_swapping1)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1 - Swapping macros usage"),
 EU_TEST_SUITE_END(suite_swapping1)
/* Add test suites */
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_swapping1, " - Swapping macros suite")
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
