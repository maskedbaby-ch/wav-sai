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
* $Version : 4.0.2$
* $Date    : Sep-12-2013$
*
* Comments:
*
*   This file contains definitions, externs and structs for
*   test suite suite_mqx2 of MQX main functions testing module.
*
*END************************************************************************/

#ifndef __NO_SETJMP
#include <setjmp.h>
#endif

#define MAIN_TASK    (10)
#define TEST_VALUE_0 (0)
#define TEST_VALUE_1 (1)
#define TEST_ADDRESS (0x55AA)

/* Main task prototype */
void main_task(uint32_t);

/* Exit handler function */
void exit_handler_func(void);
/* Get the kernel's user error code */
_mqx_uint get_kernel_user_error(void);
/* Set the kernel's user error code */
void set_kernel_user_error(_mqx_uint);

/* The startup environment of MQX before initializing */
extern jmp_buf _mqx_exit_jump_buffer_internal;
