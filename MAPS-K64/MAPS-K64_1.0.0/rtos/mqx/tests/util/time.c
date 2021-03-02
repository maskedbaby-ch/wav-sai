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
* $FileName: time.c$
* $Version : 4.0.1.0$
* $Date    : Apr-18-2013$
*
* Comments:
*
*   This file contains the source functions for time related utilities.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "util.h"

/*FUNCTION*----------------------------------------------------------
*
* Function Name : time_check_struct
* Return Value  : void
* Comments      :
*    This function takes TIME_STRUCT and checks if it holds
*    required time including time tolerance
*END*----------------------------------------------------------*/

uint32_t time_check_struct
  (
    TIME_STRUCT time, uint32_t seconds, uint32_t miliseconds, uint32_t tolerance_mult
  )
{/* Body */
  uint32_t tolerance, req_ms, act_ms;

  req_ms = (seconds * MILLISECS_IN_SECOND) + miliseconds;
  tolerance = tolerance_mult * (MILLISECS_IN_SECOND / _time_get_ticks_per_sec());
  act_ms = (time.SECONDS * MILLISECS_IN_SECOND) + time.MILLISECONDS;

  return ( (act_ms >= req_ms) && (act_ms <= (req_ms + tolerance)) );
}


/*FUNCTION*----------------------------------------------------------
*
* Function Name : time_check
* Return Value  : void
* Comments      :
*    This function takes two time offsets and checks if they are in tolerance range
*END*----------------------------------------------------------*/

uint32_t time_check
  (
    uint32_t time, uint32_t req_time, uint32_t tolerance_mult
  )
{/* Body */
  uint32_t tolerance;

  tolerance = tolerance_mult * (MILLISECS_IN_SECOND / _time_get_ticks_per_sec());
  /* known risk: time+tolerance overflow in not taken into account 2^32 ms = 50 days */
  return ( (time >= req_time) && (time <= (req_time + tolerance)) );
}

/* EOF*/
