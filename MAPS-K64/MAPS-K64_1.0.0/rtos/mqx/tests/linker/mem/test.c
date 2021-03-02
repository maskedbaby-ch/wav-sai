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
* $FileName: test.c$
* $Version : 3.8.3.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file contains the source for linker file testing regarding usable RAM.
*
*END************************************************************************/
#include <mqx.h>
#include <mqx_inc.h>
#include <bsp.h>
#include "test.h"
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include <stdlib.h>
#define FILE_BASENAME   test
//------------------------------------------------------------------------
// Test suite function prototype
void tc_1_main_task(void);  

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
{ MAIN_TASK, main_task,  1000,   7, "Main_Task", MQX_AUTO_START_TASK},
{ 0,         0,          0,      0, 0,           0                  }
};

#ifndef LINKER_USED //for debug purposes when using IDE
#define LINKER_USED NONE
#endif

// Some symbols are not exported to BSP
#if defined(__CODEWARRIOR__)
   #if defined(BSP_M5208EVB)  || defined(BSP_M52277EVB)   || defined(BSP_M53015EVB)  || defined(BSP_M5329EVB) ||\
       defined(BSP_M54455EVB) || defined(BSP_TWRMCF54418) || defined(BSP_TWRMPC5125) || defined(BSP_M5235BCC)
      extern unsigned char __CACHED_DATA_START[];
   #else   
      extern unsigned char __INTERNAL_SRAM_BASE[];
   #endif
#endif

//this array holds expected RAM sizes
RAM_SIZE ram_sizes[]={
#if (MQX_CPU == PSP_CPU_MCF51AG128) || (MQX_CPU == PSP_CPU_MCF51EM256) || (MQX_CPU == PSP_CPU_MCF51JM128) ||\
    (MQX_CPU == PSP_CPU_MK20D50M)
   {INT_FLASH, 0x4000},
#endif
#if (MQX_CPU == PSP_CPU_MCF51CN128)
   {INT_FLASH, 0x6000},
#endif
#if (MQX_CPU == PSP_CPU_MCF51AC256) || (MQX_CPU == PSP_CPU_MCF51JE256) || (MQX_CPU == PSP_CPU_MCF51JF128) ||\
    (MQX_CPU == PSP_CPU_MCF51MM256) || (MQX_CPU == PSP_CPU_MCF51QM128)
   {INT_FLASH, 0x8000},
#endif
#if (MQX_CPU == PSP_CPU_MK20DX256) || (MQX_CPU == PSP_CPU_MK40DX256) || (MQX_CPU == PSP_CPU_MK40DX256Z)
   {INT_RAM,   0x8000},
   {INT_FLASH, 0x10000},
   {EXT_RAM,   0x10000},
#endif
#if (MQX_CPU == PSP_CPU_MK53DN512Z) || (MQX_CPU == PSP_CPU_MK60D100M) || (MQX_CPU == PSP_CPU_MK60DF120M) ||\
    (MQX_CPU == PSP_CPU_MK60N512)
   {INT_RAM,   0x10000},
   {INT_FLASH, 0x20000},
   {EXT_RAM,   0x20000},
#endif
#if (MQX_CPU == PSP_CPU_MK70F120M)
   {INT_RAM,   0x10000},
   {INT_FLASH, 0x20000},
#endif
#if (MQX_CPU == PSP_CPU_MCF52259)
   {INT_RAM,   0x1800},
   {INT_FLASH, 0x10000},
#endif
#if (MQX_CPU == PSP_CPU_MCF54418)
   {EXT_RAM,   0x2000000},
   {EXT_FLASH, 0x2000000},
#endif
#if (MQX_CPU == PSP_CPU_MPC5125)
   {EXT_RAM,   0x7c00000},
   {EXT_FLASH, 0x7c00000},
#endif
#if (MQX_CPU == PSP_CPU_MPXD10)
   {INT_RAM,   0x1D000},
   {INT_FLASH, 0x1E000},
#endif
#if (MQX_CPU == PSP_CPU_MPXN20)
   {INT_RAM,   0x3A000},
   {INT_FLASH, 0x3A000},
#endif
#if (MQX_CPU == PSP_CPU_MPXS20)
   {INT_RAM,   0x1D000},
   {INT_FLASH, 0x1D000},
#endif
#if (MQX_CPU == PSP_CPU_MPXS30)
   {INT_RAM,   0xD000},
   {INT_FLASH, 0x3B000},
#endif
#if (MQX_CPU == PSP_CPU_MCF5208)
   {EXT_RAM,   0xF00000},
   {EXT_FLASH, 0xF00000},
#endif
#if (MQX_CPU == PSP_CPU_MCF52223) || (MQX_CPU == PSP_CPU_MCF52233) || (MQX_CPU == PSP_CPU_MCF52235)
   {INT_RAM,   0x1800},
   {INT_FLASH, 0x8000},
#endif
#if (MQX_CPU == PSP_CPU_MCF52277)
   {EXT_RAM,   0x1F00000},
   {EXT_FLASH, 0x3F00000},
#endif
#if (MQX_CPU == PSP_CPU_MCF53015)
   {INT_RAM,   0x2800},
   {EXT_RAM,   0x1F00000},
   {EXT_FLASH, 0x3F00000},
#endif
#if (MQX_CPU == PSP_CPU_MCF5329)
   {EXT_RAM,   0xF00000},
   {EXT_FLASH, 0x1F00000},
#endif
#if (MQX_CPU == PSP_CPU_MCF54455)
   {INT_RAM,   0x2800},
   {EXT_RAM,   0x4000000},
   {EXT_FLASH, 0x8000000},
#endif
#if MQX_CPU == PSP_CPU_MPC8308
   {EXT_RAM,   0x100000},
   {EXT_FLASH, 0x100000},
#endif
#if (MQX_CPU == PSP_CPU_MK70P256)
   {INT_RAM,   0x10000},
   {INT_FLASH, 0x20000},
   {EXT_RAM,   0x20000},
#endif
   {NONE,0}
};


//function returns expected RAM size of chip based on MQX_CPU defined in user_config.h
//and linker file used (tapp_settings.mak -> LOAD_TO setting from command line)
//Expected sizes are in ram_sizes varable
_mqx_uint get_expected_mem_size(void)
{
   uint32_t  i;
   
   for(i=0; ram_sizes[i].LINKER_FILE != NONE; i++){
      if(ram_sizes[i].LINKER_FILE == LINKER_USED)
         return ram_sizes[i].SIZE;
   }
   return 0;
}

void tc_1_main_task(void)
{
   void     *from, *to;
   _mqx_uint   max_size, exp_size, diff_size, result, correction;

   from = BSP_DEFAULT_START_OF_KERNEL_MEMORY;
   
#if MQX_ENABLE_USER_MODE
   to = BSP_DEFAULT_END_OF_USER_HEAP;
#else
   to = BSP_DEFAULT_END_OF_KERNEL_MEMORY;
#endif

#if defined(BSP_INTERNAL_SRAM_BASE)
   correction = (_mqx_uint)BSP_INTERNAL_SRAM_BASE;
#elif defined(BSP_M5208EVB)  || defined(BSP_M52277EVB)   || defined(BSP_M53015EVB)  || defined(BSP_M5329EVB) ||\
      defined(BSP_M54455EVB) || defined(BSP_TWRMCF54418) || defined(BSP_TWRMPC5125) || defined(BSP_M5235BCC)
   correction = (_mqx_uint)__CACHED_DATA_START;
#elif defined(__CODEWARRIOR__)
   correction = (_mqx_uint)__INTERNAL_SRAM_BASE;
#else
   correction = (_mqx_uint)BSP_DEFAULT_START_OF_KERNEL_MEMORY;
#endif
  
   max_size = (_mqx_uint)to - (_mqx_uint)from;
   exp_size = get_expected_mem_size();   
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, exp_size, "Test #1: 1.1 Memory size definition avaliable")
   
   diff_size = abs(exp_size - max_size);
   diff_size -= (_mqx_uint)from - correction;

   //variation between expected RAM size a real allocable memory amount should be less then 1% of expected size
   result = (diff_size < (exp_size>>7));
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result, "Test #1: 1.2 Testing avaliable amount of memory")
}
//------------------------------------------------------------------------
// Define Test Suite
EU_TEST_SUITE_BEGIN(suite_3)
   EU_TEST_CASE_ADD(tc_1_main_task, " testing avaliable memory amount" )
EU_TEST_SUITE_END(suite_3)
// Add test suites

EU_TEST_REGISTRY_BEGIN()
   EU_TEST_SUITE_ADD(suite_3, " testing avaliable memory amount")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------


/*TASK*--------------------------------------------------------------
* 
* Task Name : main_task
* Comments  :
*
*END*--------------------------------------------------------------*/

void main_task
   (
      uint32_t dummy
  )
{
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();

}/* Endbody */

/* EOF */
