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

#ifndef _DRIVER_TEST_H
#define _DRIVER_TEST_H


#ifdef __cplusplus
extern  "C"
{
#endif

#define MAIN_TASK                9
#define NAND_FLASH_WL_FILE       "nandflash_wl:"

#define METADATA_SIZE_54418   8
#define METADATA_SIZE_5125 56
#define METADATA_SIZE_K70 8
#define METADATA_SIZE_K60 8
#define SPARE_SIZE 64

extern const NANDFLASH_WL_INIT_STRUCT _bsp_nandflash_wl_init;

extern _mqx_uint nandwl_ID, nandwl_num_blocks, nandwl_virt_page_size, nandwl_block_size;
extern _mqx_uint nandwl_phy_page_spare_size;
extern _mqx_uint nandwl_phy_page_size, nandwl_page_metadata_size;
extern _mqx_uint nandwl_num_virt_pages, nandwl_num_virt_pages_per_block;
extern _mqx_uint nandwl_bus_width;

extern void main_task (uint32_t);

#ifdef __cplusplus
}
#endif


#endif /* _DRIVER_TEST_H */

/* EOF */
