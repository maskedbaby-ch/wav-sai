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
* $FileName: $
* $Version : 3.8.1.0$
* $Date    : Jul-20-2012$
*
* Comments:
*
*   
*   
*
*END************************************************************************/

#ifndef _TESTING_MAIN_INTERNAL_H_
#define _TESTING_MAIN_INTERNAL_H_

#include "wl_common.h"
#include "nandflash_wl_testing.h"

#define is_func_ret_ok(f)  \
   {  \
      if (!(is_status_ok(f) ) )   \
      {  \
         printf("\n Function %s FAILED \n", #f);   \
      }  \
   }  
   
#define IF_ERROR_EXIT(c,x) {if (c) { error = x; goto EXIT; }}

#define WL_TEST_LOG(...) WL_LOG(WL_MODULE_NANDWL_TESTING, WL_LOG_ERROR, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

bool is_status_ok(RtStatus_t status);   
extern _mqx_uint nandwl_ID, nandwl_num_blocks, nandwl_virt_page_size, nandwl_block_size;
extern _mqx_uint nandwl_phy_page_spare_size;
extern _mqx_uint nandwl_phy_page_size, nandwl_page_metadata_size;
extern _mqx_uint nandwl_num_virt_pages, nandwl_num_virt_pages_per_block;
extern _mqx_uint nandwl_bus_width;

bool fill_continous_buff(uint8_t * buf, _mem_size size);
bool size_compare( MQX_FILE_PTR flash_hdl, _mqx_int i, _mqx_int read_write_size );
bool compare_test( unsigned char * write_buffer, unsigned char * read_buffer, _mem_size read_write_size) ;
unsigned char * allocate_buffer(_mem_size buffer_size, char * string) ;
//int memcmp(void *, void*, _mem_size);

bool dump_buff( uint8_t * buf, _mem_size size, char * buff_name);
bool zero_buff( uint8_t * buf, _mem_size  size);

bool fill_random_buff(uint8_t * buf, _mem_size  size);
_mqx_int nandwl_testing_get_info(MQX_FILE_PTR nand_wl_ptr);
bool nandwl_erase_entire_chip(MQX_FILE_PTR nand_wl_ptr, uint32_t start_address, uint32_t end_address);

RtStatus_t nandwl_datadrive_erase(MQX_FILE_PTR nand_wl_ptr);
bool nandwl_erase_force_block( MQX_FILE_PTR nand_wl_ptr, int from, int to);

RtStatus_t nandwl_datadrive_repair(MQX_FILE_PTR nand_wl_ptr);
RtStatus_t nandwl_datadrive_repair_with_scan_bad(MQX_FILE_PTR nand_wl_ptr);

#define P 0x6C77
#define Q 0x12E1B

static uint32_t seed   = 0;
static uint32_t reseed = 0;
static uint32_t state  = 4;

uint32_t simple_rand(void);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
