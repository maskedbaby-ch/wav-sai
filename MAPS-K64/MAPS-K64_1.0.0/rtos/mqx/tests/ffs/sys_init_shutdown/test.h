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
* $Version : 3.7.2.0$
* $Date    : Mar-17-2011$
*
* Comments:
*
*   This include file is used for the testing software.
*
*END************************************************************************/

#ifndef _NAND_C_WRAPPER_TESTING_H_
#define _NAND_C_WRAPPER_TESTING_H_


#ifdef __cplusplus
extern  "C" {
#endif

#define MAIN_TASK                9
#define NAND_FLASH_WL_FILE       "nandflash_wl:"

extern const NANDFLASH_WL_INIT_STRUCT _bsp_nandflash_wl_init;

extern void main_task (uint32_t);

#ifdef __cplusplus
}
#endif

#endif /* _NAND_C_WRAPPER_TESTING_H_ */

/* EOF */