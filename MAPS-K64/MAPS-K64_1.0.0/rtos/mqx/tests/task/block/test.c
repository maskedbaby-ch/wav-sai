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
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file tests the scheduler block function.
*
*END************************************************************************/

#include <mqx_inc.h>
#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <bsp.h>
#include <lwlog.h>
#include "util.h"
//#include <test_module.h>
#include "EUnit.h"
#include "eunit_crit_secs.h"


//------------------------------------------------------------------------
// required user_config settings
//------------------------------------------------------------------------
#if !MQX_USE_LOGS
    #error "Required MQX_USE_LOGS"
#endif


#define FILE_BASENAME test
#define PRINT_ON    0

#define NO_KLOG_IO
#define MAIN_TASK    10
#define BLOCK_TASK   11

#if PSP_MQX_CPU_IS_KINETIS_L
#define TEST_CYCLES  10 
#else
#define TEST_CYCLES  40 
#endif

#define LOGS_PER_CYCLE  8 //depends on _klog_control setting in main_task
#define LOG_ENTRY_CNT   TEST_CYCLES * LOGS_PER_CYCLE

extern void main_task(uint32_t parameter);
extern void block_task(uint32_t dummy);
extern bool _klog_display_psp_test(void);
void tc_1_main_task(void);
void tc_1_block_task(void);

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{
#if PSP_MQX_CPU_IS_KINETIS_L
   { MAIN_TASK,   main_task,  1000, 6, "Main",   MQX_AUTO_START_TASK},
   { BLOCK_TASK,  block_task, 1000, 6, "Block",  MQX_AUTO_START_TASK},
#else
   { MAIN_TASK,   main_task,  4000, 6, "Main",   MQX_AUTO_START_TASK},
   { BLOCK_TASK,  block_task, 4000, 6, "Block",  MQX_AUTO_START_TASK},
#endif
   { 0,           0,          0,    0,  0,       0}
};

void *block_td_ptr;
void   *main_td_ptr;

/* ---------------- 68k KERNELS -------------------------- */

#if defined(_PSP_GET_SP)

   /* do nothing, it's already defined */

#elif (defined(PSP_CPU32) || defined(PSP_68000)) && defined(__ITOOLS__)

_CASM uint32_t _PSP_GET_SP() { 
   MOVE.L A7,D0 
}


/* ---------------- ARC KERNELS -------------------------- */

#elif (defined(PSP_ARC) || defined(PSP_ARC5))

_Asm uint32_t _PSP_GET_SP() {
   mov %r0,%sp
}   

#else

#define _PSP_GET_SP()   0

#endif


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : block_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void block_task
   (
      uint32_t dummy
   )
{
    tc_1_block_task();
}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1: Test _task_block function.
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
   _mqx_int  i;
   void     *my_sp;
   main_td_ptr = _task_get_td(0);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, main_td_ptr != NULL, "Test #1: Testing 1.1: _task_get_td() operation, value return should not be null")

   for ( i = 0; i < TEST_CYCLES; i++) {
#if PRINT_ON
      putchar('M');
#endif
      _int_disable();
      my_sp = (void *)_PSP_GET_SP();
      _task_block();
      if (i > 0) {
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, my_sp == (void *)_PSP_GET_SP(), "Test #1: Testing 1.2: Check stack pointer restored correctly")
         if (my_sp != (void *)_PSP_GET_SP()) {
            _int_enable();
            break;
         } /* Endif */
      } /* Endif */
      _int_enable();
      _task_ready(block_td_ptr);
   } 
#if MQX_KERNEL_LOGGING
   _klog_control( 0xffffffff, FALSE);
   _klog_display_psp_test();
#endif

}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_block_task
* Comments     :
*
*END*---------------------------------------------------------------------*/
void tc_1_block_task(void)
{
   block_td_ptr = _task_get_td(0);

   while (TRUE) {
#if PRINT_ON
      putchar('B');
#endif
      _task_ready(main_td_ptr);
      _task_block();
   } /* Endfor */
}

//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_1)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST # 1 - _task_block")
 EU_TEST_SUITE_END(suite_1)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_1, "Test of _task_block")
 EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------
 
/*TASK*-------------------------------------------------------------------
* 
* Task Name    : main_task
* Comments     : Test of _task_block
*                Please wait for 2000 MB characters to be displayed
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t parameter
   )
{
   uint32_t klog_size;
   _int_install_unexpected_isr();
   
   eunit_mutex_init();
#if MQX_KERNEL_LOGGING

   /* Calculate klog size */
   klog_size = LOG_ENTRY_CNT * sizeof(LWLOG_ENTRY_STRUCT);
   klog_size /= sizeof(_mqx_max_type);
   /* Create large kernel log, not overwritten */
   _klog_create(klog_size, 0); 
   /* Enable kernel logging */
   _klog_control( KLOG_ENABLED |
      KLOG_CONTEXT_ENABLED |
      KLOG_FUNCTIONS_ENABLED |
      KLOG_TASKING_FUNCTIONS,
      TRUE);    

#endif
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _klog_display_psp_test
* Returned Value   : none
* Comments         :
*   This function prints out one kernel log entry
*
*END*----------------------------------------------------------------------*/


bool _klog_display_psp_test
   (
      void
   )
{ /* Body */
   LWLOG_ENTRY_STRUCT     log_entry;
   LWLOG_ENTRY_STRUCT_PTR log_ptr;
   _mqx_uint              result;
   _mqx_uint              counter=0;
#if MQX_KERNEL_LOGGING

#ifndef NO_KLOG_IO
   _mqx_int               i;
#endif
   bool                block,  cs;
   block = FALSE;
   cs = FALSE;
   log_ptr = &log_entry;
   result = _lwlog_read(LOG_KERNEL_LOG_NUMBER, LOG_READ_OLDEST_AND_DELETE,
      log_ptr);
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, result == MQX_OK, "Test #1: Testing 1.3: Read in formation in the lightweight log")
   if (result != MQX_OK) {
      return FALSE;
   } /* Endif */

   while (result == MQX_OK) {
     counter++;
      /* Got the log record */
#if MQX_LWLOG_TIME_STAMP_IN_TICKS == 0
      log_ptr->MILLISECONDS += log_ptr->MICROSECONDS / 1000;
      log_ptr->MICROSECONDS  = log_ptr->MICROSECONDS % 1000;
      log_ptr->SECONDS      += log_ptr->MILLISECONDS / 1000;
      log_ptr->MILLISECONDS  = log_ptr->MILLISECONDS % 1000;
#endif


#ifndef NO_KLOG_IO
#if MQX_LWLOG_TIME_STAMP_IN_TICKS == 0
      printf("%ld. %ld:%03ld%03ld -> ",
         log_ptr->SEQUENCE_NUMBER,
         log_ptr->SECONDS,
         log_ptr->MILLISECONDS, 
         log_ptr->MICROSECONDS);
#else
      printf("%ld. ", log_ptr->SEQUENCE_NUMBER);
      _psp_print_ticks(&log_ptr->TIMESTAMP);
      printf(" -> ");
#endif
#endif

      switch (log_ptr->DATA[0]) {

         case KLOG_FUNCTION_ENTRY:
         case KLOG_FUNCTION_EXIT:
            if (log_ptr->DATA[1] == KLOG_task_block) {
               block = TRUE;
            } /* Endif */
#ifndef NO_KLOG_IO
            printf("%s %22.22s ", 
               (log_ptr->DATA[0] == KLOG_FUNCTION_ENTRY) ? "FUN " : "XFUN",
               _klog_get_function_name_internal(log_ptr->DATA[1]));
            for (i = 2; i < LWLOG_MAXIMUM_DATA_ENTRIES; ++i) {
               printf("%08lx ", log_ptr->DATA[i]);
            } /* Endfor */
            printf("\n");
#endif
            break;

         case KLOG_CONTEXT_SWITCH:
            cs = TRUE;
#ifndef NO_KLOG_IO
            printf("NEW TASK TD 0x%08lx ID 0x%08lx STATE 0x%lx STACK 0x%lx\n",
               log_ptr->DATA[1], log_ptr->DATA[2], log_ptr->DATA[3],
               log_ptr->DATA[4]);
#endif
               break;

         default:
#ifndef NO_KLOG_IO
            printf("UNKOWN: 0x%lx:", log_ptr->DATA[0]);
            for (i = 1; i < LWLOG_MAXIMUM_DATA_ENTRIES; ++i) {
               printf("%08lx ", log_ptr->DATA[i]);
            } /* Endfor */
            printf("\n");
#endif
            break;
      } /* Endswitch */

      log_ptr = &log_entry;
      result = _lwlog_read(LOG_KERNEL_LOG_NUMBER, LOG_READ_OLDEST_AND_DELETE,
         log_ptr);

   } /* Endwhile */

   result = TRUE;
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, block, "Test #1: Testing 1.4: Check for logging of PSP blocking enabled")
   if (!block) {
      result = FALSE;
   } /* Endif */
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, cs, "Test #1: Testing 1.5: Check for logging of PSP context switching enabled")
   if (!cs) {
      result = FALSE;
   } /* Endif */
   EU_ASSERT_REF_TC_MSG(tc_1_main_task, (counter == LOG_ENTRY_CNT), "Test #1: Testing 1.6: Check for logged elements count")
   if (counter != LOG_ENTRY_CNT) {
      result = FALSE;
   } /* Endif */
   
#endif
   return result;

} /* Endbody */

/* EOF */
