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
* $FileName: testing_main_internal.cpp$
* $Version : 3.8.0.0$
* $Date    : Mar-17-2011$
*
* Comments:
*
*   
*
*END************************************************************************/

#include "wl_common.h"
#include "nandflash_wl_testing.h"
#include "testing_main_internal.h"

_mqx_uint nandwl_ID, nandwl_num_blocks, nandwl_virt_page_size, nandwl_block_size;
_mqx_uint nandwl_phy_page_spare_size;
_mqx_uint nandwl_phy_page_size, nandwl_page_metadata_size;
_mqx_uint nandwl_num_virt_pages, nandwl_num_virt_pages_per_block;
_mqx_uint nandwl_bus_width;

static int memcmp(void *b1, void*b2, _mem_size s);

_mqx_int nandwl_testing_get_info(MQX_FILE_PTR nand_wl_ptr)
{

    /* Get NAND Flash organization data */
    /*puts("\nObtaining NAND Flash organization data ...");*/
    if (MQX_OK != ioctl(nand_wl_ptr, NANDFLASH_IOCTL_GET_ID, &nandwl_ID)){
        WL_TEST_LOG("\nID not obtained");
        return IO_ERROR;
    }
    if (MQX_OK != ioctl(nand_wl_ptr, NANDFLASH_IOCTL_GET_PHY_PAGE_SIZE,
            &nandwl_phy_page_size)){
        WL_TEST_LOG("\nPhysical page size not obtained");
        return IO_ERROR;
    }
    if (MQX_OK != ioctl(nand_wl_ptr, NANDFLASH_IOCTL_GET_SPARE_AREA_SIZE,
            &nandwl_phy_page_spare_size)) {
        WL_TEST_LOG("\nSpare area size not obtained");
        return IO_ERROR;
    }
    if (MQX_OK != ioctl(nand_wl_ptr, NANDFLASH_IOCTL_GET_BLOCK_SIZE,
            &nandwl_block_size)) {
        WL_TEST_LOG("\nBlock size not obtained");
        return IO_ERROR;
    }

    if (MQX_OK != ioctl(nand_wl_ptr, NANDFLASH_IOCTL_GET_NUM_BLOCKS,
            &nandwl_num_blocks)) {
        WL_TEST_LOG("\nNumber of blocks not obtained");
        return IO_ERROR;
    }
    if (MQX_OK != ioctl(nand_wl_ptr, NANDFLASH_IOCTL_GET_WIDTH, &nandwl_bus_width)) {
        WL_TEST_LOG("\nWidth not obtained");
        return IO_ERROR;
    }
    if (MQX_OK != ioctl(nand_wl_ptr, NANDFLASH_IOCTL_GET_NUM_VIRT_PAGES, &nandwl_num_virt_pages)) {
        WL_TEST_LOG("\nNumber of virtual pages not obtained");
        return IO_ERROR;
    }
    if (MQX_OK != ioctl(nand_wl_ptr, NANDFLASH_IOCTL_GET_VIRT_PAGE_SIZE,
            &nandwl_virt_page_size)) {
        WL_TEST_LOG("\nVirtual page size not obtained");
        return IO_ERROR;
    }
    if (MQX_OK != ioctl(nand_wl_ptr, NANDFLASH_IOCTL_GET_SPARE_AREA_SIZE,
            &nandwl_phy_page_spare_size)) {
        WL_TEST_LOG("\nPage spare size not obtained");
        return IO_ERROR;
    }
    if (MQX_OK != ioctl(nand_wl_ptr,
            NANDFLASH_IOCTL_GET_PAGE_METADATA_AREA_SIZE, &nandwl_page_metadata_size)) {
        WL_TEST_LOG("\nVirtual Page metadata size not obtained");
        return IO_ERROR;
    }
    nandwl_num_virt_pages_per_block = nandwl_block_size / nandwl_virt_page_size;
    return MQX_OK;
}

/*FUNCTION*-----------------------------------------------------
*
* Task Name    : memcmp
* Comments     :
*    
*
*END*-----------------------------------------------------*/
int memcmp(void *b1, void*b2, _mem_size s)
{
   _mqx_int i;

   for (i=0; i< s; i++)
   {
      if  (((uint8_t *)b1)[i] != ((uint8_t *)b2)[i] )
      {
         return 1;
      }
   }

   return 0;
}

/*FUNCTION*-----------------------------------------------------
*
* Task Name    : allocate_buffer
* Comments     :
*    Allocate the buffer
*
*END*-----------------------------------------------------*/
unsigned char * allocate_buffer(_mem_size buffer_size, char * string)
{
    unsigned char * buffer_ptr;

    buffer_ptr = (unsigned char *)_mem_alloc_zero(buffer_size);
    if (buffer_ptr == NULL) {
        WL_TEST_LOG("\nFailed to get %s buffer", string);
    } /* Endif */
    return buffer_ptr;
}


/*FUNCTION*-----------------------------------------------------
*
* Task Name    : compare_test
* Comments     :
*    function compare write_buffer and read_buffer
*
*END*-----------------------------------------------------*/
bool compare_test( unsigned char * write_buffer, unsigned char * read_buffer, _mem_size read_write_size)
{
   WL_TEST_LOG("Compare buff 0x%0x with 0x%0x with size %d\n", write_buffer, read_buffer, read_write_size);
    if (memcmp(write_buffer, read_buffer, read_write_size)) {
         WL_TEST_LOG("Compare failed\n");
         return FALSE;
    }
    else {
        return TRUE;
    }
}

/*FUNCTION*-----------------------------------------------------
*
* Task Name    : size_compare
* Comments     :
*    function compare i and read_write_size
*
*END*-----------------------------------------------------*/
bool size_compare( MQX_FILE_PTR flash_hdl, _mqx_int i, _mqx_int read_write_size )
{
    if (i != read_write_size ) {
        WL_TEST_LOG("\nFailed to write flash, size returned:%d expected %d", i,
        read_write_size);
        return FALSE;
        WL_TEST_LOG("\nTEC:0x%X FERROR:0x%X", _task_get_error(), ferror(flash_hdl));
    }
    else {
        WL_TEST_LOG("Done");
        return TRUE;
    }/* Endif */    
}


/*FUNCTION*-----------------------------------------------------
*
* Task Name    : dump_buff
* Comments     :
*    
*
*END*-----------------------------------------------------*/
bool dump_buff( uint8_t * buf, _mem_size size, char * buff_name)
{
    int i = 0;
    printf("\n Buffer \"%s\" dump:\n", buff_name);

    for (i=0; i< size; i++)
    {
        if ((i % 16) == 0) printf ("\n%04d: ", i);
        if ((i % 8) == 0)  printf(" | ");

        printf("%02x ", (uint8_t)buf[i] );
    }

    printf("\n");
}

/*FUNCTION*-----------------------------------------------------
*
* Task Name    : zero_buff
* Comments     :
*    
*
*END*-----------------------------------------------------*/
bool zero_buff( uint8_t * buf, _mem_size size)
{
    int i;
    for (i=0; i< size; i++) buf[i] = 0x00;
    return TRUE;
}

/*FUNCTION*-----------------------------------------------------
*
* Task Name    : fill_continous_buff
* Comments     :
*    
*
*END*-----------------------------------------------------*/
bool fill_continous_buff(uint8_t * buf, _mem_size size)
{
    int i;
    for (i=0; i < size; i++)
    {
        buf[i] = i% 0xFF;
    }
    return TRUE;
}

/*FUNCTION*-----------------------------------------------------
*
* Task Name    : fill_random_buff
* Comments     :
*    
*
*END*-----------------------------------------------------*/
bool fill_random_buff(uint8_t * buf, _mem_size size)
{
    int i;
    uint32_t rand;
    for (i=0; i < size; i+= 4)
    {
       // TODO: rand in kernel
       rand = simple_rand();
        buf[i] = rand & 0xFF;
        buf[i+1] = (rand >> 8) & 0xFF;
        buf[i+2] = (rand >> 16) & 0xFF;
        buf[i+3] = (rand >> 24) & 0xFF;
    }
    return TRUE;
}

/*FUNCTION*-----------------------------------------------------
*
* Task Name    : is_status_ok
* Comments     :
*    
*
*END*-----------------------------------------------------*/
bool is_status_ok(RtStatus_t status)
{
   if ( SUCCESS != status )
   {
      WL_TEST_LOG("\n  ** Error with status =%d  **\n");
      return FALSE;
   }

   return TRUE;
}

/*FUNCTION*-----------------------------------------------------
*
* Task Name    : simple_rand
* Comments     :
*    
*
*END*-----------------------------------------------------*/

uint32_t simple_rand(void)
{ /* Body */
   uint32_t i,j;
   uint32_t pq = P*Q;
   uint32_t square;
   uint32_t randnum;

   if (reseed) {
      uint32_t realseed = seed ^ state;
      uint32_t resp, resq;
      while (realseed >= pq) {
         realseed -= pq;
      } /* Endwhile */
      for (;;) {
         resp = realseed % P;
         resq = realseed % Q;
         if ((resp != 0) && (resp != 1) && (resp != P-1)
               && (resq != 0) && (resq != 1) && (resq != Q-1)) {
            break;
         } /* Endif */
         realseed += 3;
         if (realseed >= pq) realseed -= pq;
      } /* Endfor */
      state = realseed;
      reseed = 0;
   } /* Endif */

   randnum = 0;
   for (i=0; i<32; i++) {
      /* square = state*state mod pq */
      square = 0;
      for (j=0x40000000UL; j; j>>=1) {
         square <<= 1;
         if (square >= pq) square -= pq;
         if (state & j)    square += state;
         if (square >= pq) square -= pq;
      } /* Endfor */
      state = square;
      randnum = (randnum << 1) | (state & 1);
   } /* Endfor */

   return randnum;
} /* Endbody */

/*FUNCTION*-----------------------------------------------------
*
* Task Name    : nandwl_erase_entire_chip
* Comments     :
*    
*
*END*-----------------------------------------------------*/
bool nandwl_erase_entire_chip(MQX_FILE_PTR nand_wl_ptr, uint32_t start_address, uint32_t end_address)
{
  int i;
   WL_LOG(WL_MODULE_NANDWL_TESTING, WL_LOG_ERROR,"Erasing entire chip ... \n");
   for (i = start_address; i < end_address; i++)
   {
      if (MQX_OK == ioctl(nand_wl_ptr, NANDFLASH_IOCTL_ERASE_BLOCK_FORCE, (void*) i))
      {
         WL_LOG(WL_MODULE_NANDWL_TESTING, WL_LOG_ERROR,"erase block %d done\n", i);
      }
      else
      {
         WL_LOG(WL_MODULE_NANDWL_TESTING, WL_LOG_ERROR,"erase block %d FAILED\n", i);
         return FALSE;
      }
   }

   return TRUE;

}

/*FUNCTION*-----------------------------------------------------
*
* Task Name    : nandwl_erase_force_block
* Comments     :
*    
*
*END*-----------------------------------------------------*/

bool nandwl_erase_force_block( MQX_FILE_PTR nand_wl_ptr, int from, int to)
{
  int i;
   WL_TEST_LOG("Erasing block %d to %d... \n", from, to);
   for (i = from; i < to; i++)
   {
      if (MQX_OK == ioctl(nand_wl_ptr, NANDFLASH_IOCTL_ERASE_BLOCK_FORCE, (void*) i))
      {
         WL_TEST_LOG("erase block %d done\n", i);
      }
      else
      {
         WL_TEST_LOG("erase block %d FAILED\n", i);
         return FALSE;
      }
   }
   return TRUE;

}

/*FUNCTION*-----------------------------------------------------
*
* Task Name    : nandwl_datadrive_erase
* Comments     :
*    
*
*END*-----------------------------------------------------*/
RtStatus_t nandwl_datadrive_erase(MQX_FILE_PTR nand_wl_ptr)
{
   RtStatus_t status  ;

   status = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_ERASE, NULL);

   return status;
}

/*FUNCTION*-----------------------------------------------------
*
* Task Name    : nandwl_datadrive_repair
* Comments     :
*    
*
*END*-----------------------------------------------------*/

RtStatus_t nandwl_datadrive_repair(MQX_FILE_PTR nand_wl_ptr)
{
   RtStatus_t status  ;

   status = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_REPAIR, NULL);

   return status;
}

/*FUNCTION*-----------------------------------------------------
*
* Task Name    : nandwl_datadrive_repair_with_scan_bad
* Comments     :
*    
*
*END*-----------------------------------------------------*/
RtStatus_t nandwl_datadrive_repair_with_scan_bad(MQX_FILE_PTR nand_wl_ptr)
{
   RtStatus_t status  ;

   status = ioctl(nand_wl_ptr, NANDFLASH_IOCTL_REPAIR_WITH_BAD_SCAN, NULL);

   return status;
}
