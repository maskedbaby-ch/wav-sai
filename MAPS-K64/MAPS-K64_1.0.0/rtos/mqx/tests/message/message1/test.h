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

#define   MAIN_TASK   10
#define   TEST4_TASK  15

#if (MQX_USE_32BIT_MESSAGE_QIDS == 1)
#define   TEST_SYSTEM_QUEUE      257
#else
#define   TEST_SYSTEM_QUEUE      10
#endif

typedef struct test4_msg_struct 
{
   MESSAGE_HEADER_STRUCT   HEADER;
   uint32_t                 DATA1;
   uint16_t                 DATA2;
   char                    DATA3;
   uint16_t                 DATA4;
   uint32_t                 DATA5;
}TEST4_MSG_STRUCT, * TEST4_MSG_STRUCT_PTR; 


/* externs from test.c */
extern void main_task(uint32_t);
extern void test4_task(uint32_t);
extern void queue_not_function(void *qno);
extern void test4_not(void *task_td);

