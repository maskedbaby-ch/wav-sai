/**HEADER********************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
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
* $FileName: test.c$
* $Version : 4.0.1.1$
* $Date    : Mar-27-2013$
*
* Comments:
*
*   This file contains definitions, extern and structs for
*   message test program.
*
*END************************************************************************/

#define  MAIN_TASK   10
#define  TEST_TASK   11
#define  TEST_TASK2  12
#define  TEST_TASK3  13
#define  SUPER_TASK  14

#define  TEST_COUNT  10
/* externs from test.c */

extern void test_task
   (
      uint32_t param
      /* [IN] The task index */
   );

extern void test_task2
   (
      uint32_t param
      /* [IN] The task index */
   );

extern void test_task3
   (
      uint32_t param
      /* [IN] The task index */
   );

extern void main_task
   (
      uint32_t param
      /* [IN] The task index */
   );

extern void super_task
   (
      uint32_t param
      /* [IN] The task index */
   );

void test_ISR(void *isr_ptr);
