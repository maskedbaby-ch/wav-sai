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
* $FileName: util.c$
* $Version : 3.8.2.0$
* $Date    : Jul-19-2011$
*
* Comments:
*
*   This file contains utility functions for the MFS test suite
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <mfs.h>
#include <part_mgr.h>
#include "util.h"

/*FUNCTION*-----------------------------------------------------
 * 
 * Function Name    : _ftest_partition_calculate_fat_type
 * Returned Value   : none
 * Comments         :
 *   this function determines a partitions FAT type
 *
 *END*--------------------------------------------------------*/

uint32_t _ftest_partition_calculate_fat_type
   (
      uint32_t size
   )
{ /* Body */
   uint32_t fat_type;

   if (size < SECTOR_BOUND5) {
      fat_type = PMGR_PARTITION_FAT_12_BIT;
   } else if (size < SECTOR_BOUND7) {
      fat_type = PMGR_PARTITION_FAT_16_BIT;
   } else {
      fat_type = PMGR_PARTITION_FAT32;
   } /* Endif */

   return fat_type;

} /* Endbody */
uint32_t overall_error_mfs;

/*FUNCTION*-----------------------------------------------------
 * 
 * Function Name    : mtest_start
 * Returned Value   : none
 * Comments         :
 *   this function prints out the starting message for the test
 *
 *END*--------------------------------------------------------*/

void mtest_start
   (
      char *str
   )
{ /* Body */

  // printf("\n***********************************************************\n");
  // printf("                  MFS TEST SUITE (0x%X)\n", MFS_VERSION);
  // printf("%s\n", str);
  // printf("***********************************************************\n");
   overall_error_mfs = 0;

} /* Endbody */


/*FUNCTION*-----------------------------------------------------
 * 
 * Function Name    : mtest_end  
 * Returned Value   : none
 * Comments         :
 *   this function prints out the starting message for the test
 *
 *END*--------------------------------------------------------*/

void mtest_end  
   (
      void
   )
{ /* Body */

   _int_disable();
  // printf("\n\n*********************************************************\n");
  // printf("Overall Result: ");
  // if (overall_error_mfs) {
  //    printf("+++++ Test FAILED %d times +++++ \n", overall_error_mfs);
  // } else {
  //    printf("Test PASSED\n");
  // } /* Endif */
  // printf("***********************************************************\n");
   _time_delay(1000);
   _mqx_exit(overall_error_mfs);

} /* Endbody */


/*FUNCTION*-----------------------------------------------------
 * 
 * Function Name    : mtest_error
 * Returned Value   : none
 * Comments         :
 *   this function prints out the error message
 *
 *END*--------------------------------------------------------*/

void mtest_error
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
   overall_error_mfs++;
   _int_enable();

   va_end(ap);

} /* Endbody */


/*FUNCTION*-----------------------------------------------------
 * 
 * Function Name    : mtest_fatal
 * Returned Value   : none
 * Comments         :
 *   this function prints out the error message, then exits
 *
 *END*--------------------------------------------------------*/

void mtest_fatal
   (
      char *fmt_ptr,
      ...
   )
{ /* Body */
   va_list ap;

   va_start(ap, fmt_ptr);

   _int_disable();
   printf("+++++ FATAL ERROR +++++ \n");
   vprintf(fmt_ptr, ap);
   printf("\n");
   overall_error_mfs++;

   va_end(ap);

   //mtest_end();
   //test_stop();
    return;
} /* Endbody */


/*FUNCTION*-----------------------------------------------------
 * 
 * Function Name    : mtest_warn
 * Returned Value   : none
 * Comments         :
 *   this function prints out the warning message
 *
 *END*--------------------------------------------------------*/

void mtest_warn
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
