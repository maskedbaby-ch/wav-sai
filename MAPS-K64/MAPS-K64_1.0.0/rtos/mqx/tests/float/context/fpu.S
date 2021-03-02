/*HEADER********************************************************************
*
* Copyright (c) 2010-2011 Freescale Semiconductor;
* All Rights Reserved
*
****************************************************************************
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
***************************************************************************
*
* $FileName: dispatch.S$
* $Version : 3.8.38.0$
* $Date    : Oct-5-2012$
*
* Comments:
*
*   This assembler file contains functions for floating test
*
*END***********************************************************************/

#include <asm_mac.h>
#include "mqx_cnfg.h"
#include "psp_cpudef.h"

 ASM_CODE_SECTION(.text)
 ASM_COMP_SPECIFIC_DIRECTIVES


 ASM_PUBLIC(fpu_fill)
 ASM_PUBLIC(fpu_check)
 ASM_PUBLIC(fpu_damage)

/*FUNCTION*--------------------------------------------------------------------

 Function Name    : fpu_fill
 Returned Value   : none
 Comments         : function fills all fpu registers with single precision
    representation of 1.5

 END*------------------------------------------------------------------------*/
 ASM_PUBLIC_BEGIN(fpu_fill)
 ASM_PUBLIC_FUNC( fpu_fill)
ASM_LABEL(fpu_fill)
#if PSP_MQX_CPU_IS_ARM_CORTEX_A5
    ldr r0, =0x9abcdef0
    ldr r1, =0x12345678

    vmov d0, r0, r1
    vmov d1, r0, r1
    vmov d2, r0, r1
    vmov d3, r0, r1
    vmov d4, r0, r1
    vmov d5, r0, r1
    vmov d6, r0, r1
    vmov d7, r0, r1
    vmov d8, r0, r1
    vmov d9, r0, r1
    vmov d10, r0, r1
    vmov d11, r0, r1
    vmov d12, r0, r1
    vmov d13, r0, r1
    vmov d14, r0, r1
    vmov d15, r0, r1
    vmov d16, r0, r1
    vmov d17, r0, r1
    vmov d18, r0, r1
    vmov d19, r0, r1
    vmov d20, r0, r1
    vmov d21, r0, r1
    vmov d22, r0, r1
    vmov d23, r0, r1
    vmov d24, r0, r1
    vmov d25, r0, r1
    vmov d26, r0, r1
    vmov d27, r0, r1
    vmov d28, r0, r1
    vmov d29, r0, r1
    vmov d30, r0, r1
    vmov d31, r0, r1
#elif PSP_MQX_CPU_IS_ARM_CORTEX_M4
    MOV  R0, #0x3fc00000
    VMOV S0, R0
    VMOV S1, R0
    VMOV S2, R0
    VMOV S3, R0
    VMOV S4, R0
    VMOV S5, R0
    VMOV S6, R0
    VMOV S7, R0
    VMOV S8, R0
    VMOV S9, R0
    VMOV S10, R0
    VMOV S11, R0
    VMOV S12, R0
    VMOV S13, R0
    VMOV S14, R0
    VMOV S15, R0
    VMOV S16, R0
    VMOV S17, R0
    VMOV S18, R0
    VMOV S19, R0
    VMOV S20, R0
    VMOV S21, R0
    VMOV S22, R0
    VMOV S23, R0
    VMOV S24, R0
    VMOV S25, R0
    VMOV S26, R0
    VMOV S27, R0
    VMOV S28, R0
    VMOV S29, R0
    VMOV S30, R0
    VMOV S31, R0
#else
#error Test is not ported to this architecture    
#endif
    BX LR
    ASM_PUBLIC_END(fpu_fill)

/*FUNCTION*--------------------------------------------------------------------

 Function Name    : fpu_check
 Returned Value   : none
 Comments         :     function checks that all fpu registers contain signle
    precision representation of 1.5

 END*------------------------------------------------------------------------*/
 ASM_PUBLIC_BEGIN(fpu_check)
 ASM_PUBLIC_FUNC(fpu_check)
ASM_LABEL(fpu_check)
#if PSP_MQX_CPU_IS_ARM_CORTEX_A5
    ldr r0, =0x9abcdef0
    ldr r1, =0x12345678

    vmov r2, r3, d0
    cmp r0, r2
    bne fail
    cmp r1, r3
    bne fail

    vceq.f32 d0, d0, d1
    bne fail
    vceq.f32 d1, d1, d2
    bne fail
    vceq.f32 d2, d2, d3
    bne fail
    vceq.f32 d3, d3, d4
    bne fail
    vceq.f32 d4, d4, d5
    bne fail
    vceq.f32 d5, d5, d6
    bne fail
    vceq.f32 d6, d6, d7
    bne fail
    vceq.f32 d7, d7, d8
    bne fail
    vceq.f32 d8, d8, d9
    bne fail
    vceq.f32 d9, d9, d10
    bne fail
    vceq.f32 d10, d10, d11
    bne fail
    vceq.f32 d11, d11, d12
    bne fail
    vceq.f32 d12, d12, d13
    bne fail
    vceq.f32 d13, d13, d14
    bne fail
    vceq.f32 d14, d14, d15
    bne fail
    vceq.f32 d15, d15, d16
    bne fail
    vceq.f32 d16, d16, d17
    bne fail
    vceq.f32 d17, d17, d18
    bne fail
    vceq.f32 d18, d18, d19
    bne fail
    vceq.f32 d19, d19, d20
    bne fail
    vceq.f32 d20, d20, d21
    bne fail
    vceq.f32 d21, d21, d22
    bne fail
    vceq.f32 d22, d22, d23
    bne fail
    vceq.f32 d23, d23, d24
    bne fail
    vceq.f32 d24, d24, d25
    bne fail
    vceq.f32 d25, d25, d26
    bne fail
    vceq.f32 d26, d26, d27
    bne fail
    vceq.f32 d27, d27, d28
    bne fail
    vceq.f32 d28, d28, d29
    bne fail
    vceq.f32 d29, d29, d30
    bne fail
    vceq.f32 d30, d30, d31
    bne fail

    vmov r0, r1, d31
#elif PSP_MQX_CPU_IS_ARM_CORTEX_M4
    MOV  R2, #0x3fc00000
    VMOV R1, S0
    CMP  R1, R2
    BNE  fail
    VMOV R1, S1
    CMP  R1, R2
    BNE  fail
    VMOV R1, S2
    CMP  R1, R2
    BNE  fail
    VMOV R1, S3
    CMP  R1, R2
    BNE  fail
    VMOV R1, S4
    CMP  R1, R2
    BNE  fail
    VMOV R1, S5
    CMP  R1, R2
    BNE  fail
    VMOV R1, S6
    CMP  R1, R2
    BNE  fail
    VMOV R1, S7
    CMP  R1, R2
    BNE  fail
    VMOV R1, S8
    CMP  R1, R2
    BNE  fail
    VMOV R1, S9
    CMP  R1, R2
    BNE  fail
    VMOV R1, S10
    CMP  R1, R2
    BNE  fail
    VMOV R1, S11
    CMP  R1, R2
    BNE  fail
    VMOV R1, S12
    CMP  R1, R2
    BNE  fail
    VMOV R1, S13
    CMP  R1, R2
    BNE  fail
    VMOV R1, S14
    CMP  R1, R2
    BNE  fail
    VMOV R1, S15
    CMP  R1, R2
    BNE  fail
    VMOV R1, S16
    CMP  R1, R2
    BNE  fail
    VMOV R1, S17
    CMP  R1, R2
    BNE  fail
    VMOV R1, S18
    CMP  R1, R2
    BNE  fail
    VMOV R1, S19
    CMP  R1, R2
    BNE  fail
    VMOV R1, S20
    CMP  R1, R2
    BNE  fail
    VMOV R1, S21
    CMP  R1, R2
    BNE  fail
    VMOV R1, S22
    CMP  R1, R2
    BNE  fail
    VMOV R1, S23
    CMP  R1, R2
    BNE  fail
    VMOV R1, S24
    CMP  R1, R2
    BNE  fail
    VMOV R1, S25
    CMP  R1, R2
    BNE  fail
    VMOV R1, S26
    CMP  R1, R2
    BNE  fail
    VMOV R1, S27
    CMP  R1, R2
    BNE  fail
    VMOV R1, S28
    CMP  R1, R2
    BNE  fail
    VMOV R1, S29
    CMP  R1, R2
    BNE  fail
    VMOV R1, S30
    CMP  R1, R2
    BNE  fail
    VMOV R1, S31
    CMP  R1, R2
    BNE  fail
#else
#error Test is not ported to this architecture
#endif

    MOV  R0, #1
    BX   LR

ASM_LABEL(fail)
    MOV R0, #0
    BX LR

 ASM_PUBLIC_END(fpu_check)

/*FUNCTION*--------------------------------------------------------------------

 Function Name    : _svc_handler
 Returned Value   : none
 Comments         : clears all fpu registers

END*-------------------------------------------------------------------------*/
    ASM_PUBLIC_BEGIN(fpu_damage)
    ASM_PUBLIC_FUNC(fpu_damage)
ASM_LABEL(fpu_damage)

#if PSP_MQX_CPU_IS_ARM_CORTEX_A5
    mov r0, #0
    mov r1, #0

    vmov d0, r0, r1
    vmov d1, r0, r1
    vmov d2, r0, r1
    vmov d3, r0, r1
    vmov d4, r0, r1
    vmov d5, r0, r1
    vmov d6, r0, r1
    vmov d7, r0, r1
    vmov d8, r0, r1
    vmov d9, r0, r1
    vmov d10, r0, r1
    vmov d11, r0, r1
    vmov d12, r0, r1
    vmov d13, r0, r1
    vmov d14, r0, r1
    vmov d15, r0, r1
    vmov d16, r0, r1
    vmov d17, r0, r1
    vmov d18, r0, r1
    vmov d19, r0, r1
    vmov d20, r0, r1
    vmov d21, r0, r1
    vmov d22, r0, r1
    vmov d23, r0, r1
    vmov d24, r0, r1
    vmov d25, r0, r1
    vmov d26, r0, r1
    vmov d27, r0, r1
    vmov d28, r0, r1
    vmov d29, r0, r1
    vmov d30, r0, r1
    vmov d31, r0, r1
#elif PSP_MQX_CPU_IS_ARM_CORTEX_M4
    MOV  R0, #0x0
    VMOV S0, R0
    VMOV S1, R0
    VMOV S2, R0
    VMOV S3, R0
    VMOV S4, R0
    VMOV S5, R0
    VMOV S6, R0
    VMOV S7, R0
    VMOV S8, R0
    VMOV S9, R0
    VMOV S10, R0
    VMOV S11, R0
    VMOV S12, R0
    VMOV S13, R0
    VMOV S14, R0
    VMOV S15, R0
    VMOV S16, R0
    VMOV S17, R0
    VMOV S18, R0
    VMOV S19, R0
    VMOV S20, R0
    VMOV S21, R0
    VMOV S22, R0
    VMOV S23, R0
    VMOV S24, R0
    VMOV S25, R0
    VMOV S26, R0
    VMOV S27, R0
    VMOV S28, R0
    VMOV S29, R0
    VMOV S30, R0
    VMOV S31, R0
#else
#error Test is not ported to this architecture
#endif

    BX LR
    ASM_PUBLIC_END(fpu_damage)

  ASM_ALIGN(4)
  ASM_END
