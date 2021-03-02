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
* $Version : 4.0.1.1$
* $Date    : Jun-03-2013$
*
* Comments:
*
*   This file contains definitions, extern and structs for
*   test suite suite_align of Memory testing module.
*
*END************************************************************************/

#define MAIN_TASK     (10)
#define POOL_TYPE     (TRUE)
#define TEST_SIZE_1   (16)
#define ALLOC_SIZE    (16)
#define INVALID_VALUE (1)
#define TEST_MEM_ALIGN(mem_ptr, align) \
    (((_mem_size)(mem_ptr) & (align - 1)) ? FALSE : TRUE)

/* Main task prototype */
extern void main_task(uint32_t);

/* Global variables */
void         *pool_alloc_ptr;
_mem_pool_id  ext_pool_id;
uint32_t       valid_alignment;
uint32_t       POOL_SIZE;

