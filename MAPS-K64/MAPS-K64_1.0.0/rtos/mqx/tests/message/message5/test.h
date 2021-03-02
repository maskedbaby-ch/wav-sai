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
* $Version : 4.0.1.2$
* $Date    : Apr-09-2013$
*
* Comments:
*
*   This file contains definitions, externs and structs for
*   test suite suite_5 of Message testing module.
*
*END************************************************************************/

#define MAIN_TASK           (10)
#define TEST_TASK           (11)
#define DELAY_MS            (40)
#define DELAY_TICKS         (10)
#define NUM_OF_MSGS         (5)
#define NUM_OF_QUEUES       (5)
#define QUEUE_NUMBER_1      (MSGQ_FIRST_USER_QUEUE)
#define QUEUE_NUMBER_2      (MSGQ_FIRST_USER_QUEUE + 1)
#define VALID_PRIORITY      (5)
#define INVALID_PRIORITY    (20)
#define INVALID_VALUE       (1)
#define INVALID_PROCESSOR   (2 * MQX_MAX_PROCESSOR_NUMBER)

/* Message struct definition */
typedef struct test_msg_struct
{
    MESSAGE_HEADER_STRUCT HEADER;
    uint32_t              DATA1;
    uint16_t              DATA2;
}TEST_MSG_STRUCT, * TEST_MSG_STRUCT_PTR;

typedef struct my_isr_struct
{
   void                 *OLD_ISR_DATA;
   void      (_CODE_PTR_ OLD_ISR)(void *);
   LWSEM_STRUCT          INT_EVENT;
   _mqx_uint             ERROR;
   void                 *MSG_PTR;
   _mqx_uint             TEST_NUMBER;
   _queue_id             MSG_QUEUE_TEST;
} MY_ISR_STRUCT, * MY_ISR_STRUCT_PTR;

/* Main task prototype */
extern void main_task(uint32_t);

/* Test task prototype */
extern void test_task(uint32_t);

extern void new_tick_isr(void *);

extern void *test_msg_in_isr(_queue_id, _mqx_uint, _mqx_uint *);
