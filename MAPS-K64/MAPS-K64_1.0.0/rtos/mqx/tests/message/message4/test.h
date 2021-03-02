/**HEADER********************************************************************
*
* Copyright (c) 2008-2013 Freescale Semiconductor;
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
* $FileName: test.h$
* $Version : 4.0.1.2$
* $Date    : Apr-09-2013$
*
* Comments:
*
*   This file contains definitions, externs and structs for
*   test suite suite_4 of Message testing module.
*
*END************************************************************************/

#define MAIN_TASK           (10)
#define TEST_MAX_MSG_POOLS  (10)
#define TEST_MAX_MSG_QUEUES (15)
#define NUM_OF_MSGS         (10)
#define POOL_VALID_SIZE     (sizeof(MESSAGE_HEADER_STRUCT) + 32)
#define POOL_INVALID_SIZE   (sizeof(MESSAGE_HEADER_STRUCT) - 1)
#define QUEUE_VALID_NUM     (MSGQ_FIRST_USER_QUEUE)
#define QUEUE_INVALID_NUM   (TEST_MAX_MSG_QUEUES + 1)
#define QUEUE_INVALID_ID    (-1)
#define INVALID_VALUE       (1)
#define INVALID_PROCESSOR   (2 * MQX_MAX_PROCESSOR_NUMBER)

/* Main task prototype */
extern void main_task(uint32_t);

/* Notification function */
void queue_not_function(void *param);
