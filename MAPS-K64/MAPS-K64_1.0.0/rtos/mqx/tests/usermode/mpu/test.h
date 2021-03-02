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
* $FileName: test.h$
* $Version : 3.8.2.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This include file is used to define constants and declarations fro test application
*
*END************************************************************************/

/* General defines */
#define MAIN_TASK    10
#define PRIV_TASK    11
#define USER_TASK    12

#define TEST_REC_CNT CORTEX_MPU_REC+2
#define POOL_SIZE    128

#define FIRE_PRIV    1
#define FIRE_USER    2

#define MULTI_NOP _ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();

void main_task(uint32_t parameter);
void priv_task(uint32_t parameter);
void user_task(uint32_t parameter);

_mqx_uint mpu_get_free(void);
void mpu_clear_rec(_mqx_uint index);
void mpu_clear_all_rec(void);
void mpu_modify_rec(_mqx_uint index, _mqx_uint flags);
void fault_isr(void *parameter);

//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);
void tc_2_main_task(void);
void tc_3_main_task(void);
