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
* $FileName: test.h$
* $Version : 4.0.2.0$
* $Date    : Jun-14-2013$
*
* Comments:
*
*   This file contains definitions, prototypes and global variables for
*   test suite suite_usr_lwsem5 of Usermode testing module.
*
*END************************************************************************/

#define SUPER_TASK       (10)
#define MAIN_TASK        (11)
#define ISR_TASK         (12)
#define PRIV_TASK        (13)
#define EVENT_MASK_MAIN  (1)
#define EVENT_MASK_ISR   (2)
#define EVENT_MASK_PRIV  (4)
#define DELAY_TICKS      (50)
#define LONG_DELAY_TICKS (1000)
#define INVALID_VALUE    (1)

/* User ReadOnly Lightweight semaphore */
USER_RO_ACCESS LWSEM_STRUCT usr_lwsem_ro;
/* User ReadWrite Lightweight semaphore */
USER_RW_ACCESS LWSEM_STRUCT usr_lwsem_rw;
/* Global lightweight event */
USER_RO_ACCESS LWEVENT_STRUCT isr_lwevent;
/* Tick struct */
MQX_TICK_STRUCT ticks;
/* Functions' returned value */
_mqx_uint result, ret_val;
/* Test case number */
_mqx_uint tc_number;

/* Task prototypes */
void super_task(uint32_t);
void main_task(uint32_t);
void isr_task(uint32_t);
void priv_task(uint32_t);
