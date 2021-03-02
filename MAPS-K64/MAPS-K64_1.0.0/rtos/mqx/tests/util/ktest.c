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
* $FileName: ktest.c$
* $Version : 3.0.1.0$
* $Date    : Jun-25-2009$
*
* Comments:
*
*   This file contains the source functions printing out an error message.
*
*END************************************************************************/

#include <mqx.h>
#if MQX_USE_IO_OLD
#include <fio.h>
#include <io.h>
#else
#include <nio.h>
#include <stdio.h>
#endif
#include "util.h"

uint32_t overall_error;

/*FUNCTION*-----------------------------------------------------
 *
 * Function Name    : ktest_start
 * Returned Value   : none
 * Comments         :
 *   this function prints out the starting message for the test
 *
 *END*--------------------------------------------------------*/

void ktest_start
   (
      char *str
   )
{ /* Body */

   printf("\n***********************************************************\n");
   printf("                  MQX2.50 KERNEL TEST SUITE\n");
   printf("%s\n", str);
   printf("***********************************************************\n");
   overall_error = 0;

} /* Endbody */


/*FUNCTION*-----------------------------------------------------
 *
 * Function Name    : ktest_end
 * Returned Value   : none
 * Comments         :
 *   this function prints out the starting message for the test
 *
 *END*--------------------------------------------------------*/

void ktest_end
   (
      void
   )
{ /* Body */

   _int_disable();
   printf("\n\n*********************************************************\n");
   printf("Overall Result: ");
   if (overall_error) {
      printf("+++++ Test FAILED %d times +++++ \n", overall_error);
   } else {
      printf("Test PASSED\n");
   } /* Endif */
   printf("***********************************************************\n");

   _time_delay_ticks(200);
   _task_block();

//   _mqx_exit(0);

} /* Endbody */


/*FUNCTION*-----------------------------------------------------
 *
 * Function Name    : ktest_error
 * Returned Value   : none
 * Comments         :
 *   this function prints out the error message
 *
 *END*--------------------------------------------------------*/

void ktest_error
   (
      char *fmt_ptr,
      ...
   )
{ /* Body */
   va_list ap;

   va_start(ap, fmt_ptr);

   _int_disable();
   printf("+++++ ERROR +++++ ");
   vprintf(fmt_ptr, ap);
   printf("\n");
   overall_error++;
   _int_enable();

   va_end(ap);

} /* Endbody */


/*FUNCTION*-----------------------------------------------------
 *
 * Function Name    : ktest_fatal
 * Returned Value   : none
 * Comments         :
 *   this function prints out the error message, then exits
 *
 *END*--------------------------------------------------------*/

void ktest_fatal
   (
      char *fmt_ptr,
      ...
   )
{ /* Body */
   va_list ap;

   va_start(ap, fmt_ptr);

   _int_disable();
   printf("+++++ FATAL ERROR +++++ ");
   vprintf(fmt_ptr, ap);
   printf("\n");
   overall_error++;

   va_end(ap);

   ktest_end();

} /* Endbody */


/*FUNCTION*-----------------------------------------------------
 *
 * Function Name    : ktest_warn
 * Returned Value   : none
 * Comments         :
 *   this function prints out the warning message
 *
 *END*--------------------------------------------------------*/

void ktest_warn
   (
      char *fmt_ptr,
      ...
   )
{ /* Body */
   va_list ap;

   va_start(ap, fmt_ptr);

   printf("--- Warning --- ");
   vprintf(fmt_ptr, ap);
   printf("\n");

   va_end(ap);

} /* Endbody */

/* EOF */
