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
*   test suite suite_usr_lwmsgq3 of Usermode testing module.
*
*END************************************************************************/

#define SUPER_TASK      (10)
#define MAIN_TASK       (11)
#define ISR_TASK        (12)
#define PRIV_TASK       (13)
#define EVENT_MASK_MAIN (1)
#define EVENT_MASK_ISR  (2)
#define EVENT_MASK_PRIV (4)
#define DELAY_TICKS     (50)
#define MSG_SIZE        (10)
#define NUM_OF_MSG      (10)
#define INVALID_VALUE   (1)

/* User ReadOnly Lightweight message queue */
USER_RO_ACCESS unsigned char usr_lwmsgq_ro[sizeof(LWMSGQ_STRUCT) + sizeof(_mqx_max_type) * MSG_SIZE * NUM_OF_MSG];
/* User ReadWrite Lightweight message queue */
USER_RW_ACCESS unsigned char usr_lwmsgq_rw[sizeof(LWMSGQ_STRUCT) + sizeof(_mqx_max_type) * MSG_SIZE * NUM_OF_MSG];
/* Global lightweight event */
USER_RO_ACCESS LWEVENT_STRUCT isr_lwevent;
/* Lightweight message array */
_mqx_max_type msg[NUM_OF_MSG][MSG_SIZE];
/* Temporary message */
_mqx_max_type temp_msg[MSG_SIZE];
/* Functions' returned value */
_mqx_uint result, ret_val;
/* Test case number */
_mqx_uint tc_number;

/* Task prototypes */
void super_task(uint32_t param);
void main_task(uint32_t param);
void isr_task(uint32_t param);
void priv_task(uint32_t param);
