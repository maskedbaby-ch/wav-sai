/*HEADER********************************************************************
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
 * $FileName: test_specific_vybrid.c$
 * $Version : 4.0.1$
 * $Date : Jan-18-2013$
 *
 * Comments:
 *
 *   This file contains functions of the hwtimer component.
 *
 *END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <hwtimer.h>
#include "test_specific.h"

/* Array of low level device interfaces */
HWTIMER_DEVIF_STRUCT_PTR devif_array[] = 
{
    (HWTIMER_DEVIF_STRUCT_PTR)&pit_devif,
    DEVIF_ARRAY_TERMINATOR //terminated devif  
};

/* 2 dimension array determines ids usable for testing for each devif */
int id_array[][ID_ARRAY_MAX] =
{
    {0, 1, 2, 3, 4, 5, 6, 7, ID_ARRAY_TERMINATOR} /* Channels which can be used for testing with pit_devif*/
};

/* Clock source id for each devif*/
CM_CLOCK_SOURCE clock_id_array[] = {CM_CLOCK_SOURCE_IPG}; 

