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
* $FileName: test.c$
* $Version : 4.0.1.0$
* $Date    : Apr-22-2013$
*
* Comments:
*
*   This file contains code for the MQX Cplus verification program,
*   includes a function which is used to test performance between C++ and C.
*
*END************************************************************************/
#include "mqx_inc.h"
#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif

/*FUNCTION*----------------------------------------------------------------
*
* Function Name    : loop_test
* Returned Value   : None.
* Comments         :
*   Loop to test performance
*   - [IN] ticks: number of ticks to wait
*
*END*--------------------------------------------------------------------*/
void loop_test(_mqx_uint ticks)
{
    work_ticks(ticks);
}

#ifdef __cplusplus
}
#endif
