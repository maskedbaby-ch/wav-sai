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
* $FileName: test.h$
* $Version : 3.5.1.0$
* $Date    : Nov-2-2009$
*
* Comments:
*
*   This file contains definitions, extern and structs for 
*   message test program.
*
*END************************************************************************/

#define MAIN_TASK               10
#define SUB_TASK                11

#define TEST_MAX_MSG_POOLS      10
#define TEST_MAX_MSG_QUEUES     15
#define QUEUE_VALID_NUM         10
#define QUEUE_VALID_NUM_2       11
#define NUM_OF_MSG              5

#if (MQX_USE_32BIT_MESSAGE_QIDS == 0)
   #define INVALID_PROCESSOR    270
#else
   #define INVALID_PROCESSOR    70000
#endif

/* Message struct */
typedef struct test_msg_struct 
{
    MESSAGE_HEADER_STRUCT   HEADER;
    uint32_t                 DATA1;
    uint16_t                 DATA2;
}TEST_MSG_STRUCT, * TEST_MSG_STRUCT_PTR; 

/* Extern from test.c */
extern void main_task(uint32_t);
extern void sub_task(uint32_t);