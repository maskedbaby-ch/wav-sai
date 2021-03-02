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
* $FileName: work.c$
* $Version : 3.8.2.0$
* $Date    : Sep-8-2011$
*
* Comments:
*
*   This file contains the source functions for doing busywait work.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "util.h"

/*FUNCTION*----------------------------------------------------------
*
* Function Name : work
* Return Value  : void
* Comments      :  
*    This function remains active for timeout number of
*    milliseconds.  It gets the current time and adds timeout
*    number of milliseconds.  The current time is polled until
*    the timeout time has been reached. 
*END*----------------------------------------------------------*/

void work
   (
      uint32_t timeout
   )
{/* Body */
   TIME_STRUCT time;
   TIME_STRUCT com_time;

   _time_get (&time);
   time.MILLISECONDS = time.MILLISECONDS + timeout;
      while (time.MILLISECONDS >= 1000) {
         time.MILLISECONDS = time.MILLISECONDS - 1000;
         time.SECONDS++;
      }/* Endwhile */

   while (TRUE) {
      _time_get (&com_time);
      if (com_time.SECONDS > time.SECONDS) {
         break;
      }/* Endif */
      if (com_time.SECONDS == time.SECONDS && \
         com_time.MILLISECONDS >= time.MILLISECONDS) {
         break;
      }/* Endif */
   }/* Endwhile */
   
}/* Endbody */


/*FUNCTION*----------------------------------------------------------
*
* Function Name : work_ticks
* Return Value  : void
* Comments      :  
*    This function remains active for timeout number of ticks.
*    It gets the current time and adds timeout.  The current time is
*    polled until the timeout time has been reached. 
*END*----------------------------------------------------------*/

void work_ticks
   (
      _mqx_uint timeout
   )
{/* Body */
   MQX_TICK_STRUCT time;
   MQX_TICK_STRUCT com_time;
   _mqx_int        result = -1;

   _time_get_ticks(&time);
   PSP_ADD_TICKS_TO_TICK_STRUCT(&time, timeout, &time);

   while (result < 0) {
      _time_get_ticks(&com_time);
      result = PSP_CMP_TICKS(&com_time, &time);
   }/* Endwhile*/
   
}/* Endbody*/

/* EOF*/
