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
* $FileName: dump.c$
* $Version : 3.8.2.0$
* $Date    : Sep-8-2011$
*
* Comments:
*
*   This file contains the source functions for dumping memory.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "util.h"
#if !MQX_USE_IO_OLD
#include <stdio.h>
#endif

/*FUNCTION*-----------------------------------------------------
 *
 * Function Name    : dump_memory
 * Returned Value   : none
 * Comments         :
 *   this function dumps a memory buffer at the given address
 *   in HEX with ASCII translation.
 *
 *END*--------------------------------------------------------*/

void dump_memory
   (
      unsigned char  *address,
      uint32_t size
   )
{ /* Body */
   uint32_t     i;
   unsigned char       c;

   printf("\n");
   while ( size > 0 ) {
      printf("%08x: ",address);
      for ( i = 0; i < 16; i++ ) {
         if ( i > size ) {
            printf("   ");
         } else {
            printf("%02x ",(uint32_t)address[i] & 0xff);
         } /* Endif */
      } /* Endfor */
      printf("   ");
      for ( i = 0; i < 16; i++ ) {
         if ( i > size ) {
            printf(" ");
         } else {
            c = address[i] & 0x7f;
            if ( (c == 0x7f) || (c < ' ') ) {
               c = '.';
            } /* Endif */
            printf("%c",c);
         } /* Endif */
      } /* Endfor */
      size -= 16;
      address += 16;
      if ( (i % 16) == 0 ) {
         printf("\n");
      } /* Endif */
   } /* Endfor */
   printf("\n");

} /* Endbody */

/* EOF */
