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
* $Version : 3.8.12.0$
* $Date    : Sep-6-2011$
*
* Comments:
*
*   This file contains the source functions for the preemption
*   control example.
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include "util.h"
//#include <test_module.h>

#include "EUnit.h"
#include "eunit_crit_secs.h"

#define FILE_BASENAME   test

#define MAIN_TASK     10
#define PREEMPT_TASK  11

#if MQX_INT_SIZE_IN_BITS == 16
#define L_VALUE 0xabcd
#define M_VALUE 0x1234
#define N_VALUE 0x1111
#define O_VALUE 0x2222
#define P_VALUE 0x3333
#define Q_VALUE 0x4444
#define R_VALUE 0x5555
#define S_VALUE 0x6666
#define T_VALUE 0x7777
#define U_VALUE 0x8888
#define V_VALUE 0x9999
#define W_VALUE 0xaaaa
#define X_VALUE 0xbbbb
#define Y_VALUE 0xcccc
#define Z_VALUE 0xdddd
#elif MQX_INT_SIZE_IN_BITS == 24
#define L_VALUE 0xabcdef00
#define M_VALUE 0x12345678
#define N_VALUE 0x11111111
#define O_VALUE 0x22222222
#define P_VALUE 0x33333333
#define Q_VALUE 0x44444444
#define R_VALUE 0x55555555
#define S_VALUE 0x66666666
#define T_VALUE 0x77777777
#define U_VALUE 0x88888888
#define V_VALUE 0x99999999
#define W_VALUE 0xaaaaaaaa
#define X_VALUE 0xbbbbbbbb
#define Y_VALUE 0xcccccccc
#define Z_VALUE 0xdddddddd
#elif MQX_INT_SIZE_IN_BITS >= 32
#define L_VALUE 0xabcdef00
#define M_VALUE 0x12345678
#define N_VALUE 0x11111111
#define O_VALUE 0x22222222
#define P_VALUE 0x33333333
#define Q_VALUE 0x44444444
#define R_VALUE 0x55555555
#define S_VALUE 0x66666666
#define T_VALUE 0x77777777
#define U_VALUE 0x88888888
#define V_VALUE 0x99999999
#define W_VALUE 0xaaaaaaaa
#define X_VALUE 0xbbbbbbbb
#define Y_VALUE 0xcccccccc
#define Z_VALUE 0xdddddddd
#endif

#define TIME_TRESH_TICKS    5 //number of ticks for active wait with stoped preemption

#if PSP_ENDIAN == MQX_BIG_ENDIAN
#define TIME_TICKS_LSB_INDEX MQX_NUM_TICK_FIELDS-1
#else
#define TIME_TICKS_LSB_INDEX 0
#endif

extern void main_task(uint32_t dummy);
extern void preempt_task(uint32_t dummy);

void    tc_1_main_task();

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
   { MAIN_TASK,    main_task,    2000,  8, "Main",   MQX_AUTO_START_TASK},
   { PREEMPT_TASK, preempt_task, 2000, 10, "Preempt",0},
   { 0,            0,            0,     0,  0,       0}
};

volatile uint32_t main_count = 0;
MQX_TICK_STRUCT   time_ticks;


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : preempt_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void preempt_task
   (
      uint32_t dummy
   )
{ /* Body */
   register _mqx_uint l, m, n, o, p;
   register _mqx_uint q, r, s, t, u;
   register _mqx_uint v, w, x, y, z;
   uint8_t lf0=0;
   uint8_t ea_flag_0 = FALSE;
   uint8_t ea_flag_1 = FALSE;
   uint8_t ea_flag_2 = FALSE;
   uint32_t count;

   l = L_VALUE;
   m = M_VALUE;
   n = N_VALUE;
   o = O_VALUE;
   p = P_VALUE;
   q = Q_VALUE;
   r = R_VALUE;
   s = S_VALUE;
   t = T_VALUE;
   u = U_VALUE;
   v = V_VALUE;
   w = W_VALUE;
   x = X_VALUE;
   y = Y_VALUE;
   z = Z_VALUE;
   
   _task_stop_preemption();
   count = main_count;
   if ((l != L_VALUE) || (m != M_VALUE) || (n != N_VALUE) ||(o != O_VALUE) || (p != P_VALUE))
   {
        lf0=0;
   }
    else
   {
        lf0=1;
   }        
        
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (lf0==1), "Test #1 Testing: 1.1: registers should be restored correctly")
   
   /* Endif */
   // for (i = 0; i < (BSP_SYSTEM_CLOCK/10000); ++i) {
      // j = i * k + 1345;
   // } /* Endfor */
   _mem_zero(&time_ticks, sizeof(MQX_TICK_STRUCT));
   _time_set_ticks(&time_ticks);
   
   while(time_ticks.TICKS[TIME_TICKS_LSB_INDEX] < TIME_TRESH_TICKS){
      _time_get_ticks(&time_ticks);
   }
   //if (count != main_count)
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (count == main_count), "Test #1 Testing: 1.2: preempt should be stopped")
   /* Endif */
   _task_start_preemption();
   count = main_count;
   
   _mem_zero(&time_ticks, sizeof(MQX_TICK_STRUCT));
   _time_set_ticks(&time_ticks);
   
   while(time_ticks.TICKS[TIME_TICKS_LSB_INDEX] < TIME_TRESH_TICKS){
      if ((l != L_VALUE) || (m != M_VALUE) || (n != N_VALUE) ||
          (o != O_VALUE) || (p != P_VALUE))
      {
         ea_flag_0 = FALSE;
         break;
      } else {
         ea_flag_0 = TRUE;
      }
       /* Endif */
      if ((q != Q_VALUE) || (r != R_VALUE) || (s != S_VALUE) ||
          (t != T_VALUE) || (u != U_VALUE))
      {
         ea_flag_1 = FALSE;         
         break;
      } else {
         ea_flag_1 = TRUE;
      }
      /* Endif */
      if ((v != V_VALUE) || (w != W_VALUE) || (x != X_VALUE) ||
          (y != Y_VALUE) || (z != Z_VALUE))
      {
         ea_flag_2 = FALSE;
         break;
      } else {
         ea_flag_2 = TRUE;
      }
      /* Endif */
      _time_get_ticks(&time_ticks);
   } 

   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ea_flag_0, "Test #1 Testing: 1.3: registers should be restored correctly")
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ea_flag_1, "Test #1 Testing: 1.4: registers should be restored correctly")
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ea_flag_2, "Test #1 Testing: 1.5: registers should be restored correctly")
   ea_flag_0 =TRUE;
   ea_flag_1 =TRUE;
   ea_flag_2 =TRUE;
   /* Endfor */
   if (count == main_count){
      ea_flag_0 = FALSE;
   }
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, ea_flag_0, "Test #1 Testing: 1.6: preemption should started again"); 
   /* Endif */
    eunit_test_stop();
   _time_delay_ticks(200);
   _mqx_exit(0);
} /* Endbody */ 

void tc_1_main_task() {

    main_count = 0;
   _task_create(0, PREEMPT_TASK, 0);

   while (1) {
      main_count++;
      _time_delay_ticks(2);
   } /* Endwhile */
}

//------------------------------------------------------------------------
// Define Test Suite    
EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " Test # 1 Test of starting and stopping preemption")
EU_TEST_SUITE_END(suite_1)
// Add test suites
EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_1, "Testing preemption")
EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : main_task
* Comments     :
*   This task creates a data log, then waits for info to
*   be deposited.
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t dummy
   )
{ /* Body */
   //test_initialize();
   //EU_RUN_ALL_TESTS(test_result_array);
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   /* test_stop(); */
   eunit_test_stop();



} /* Endbody */

/* EOF */
