/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
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
* $FileName: nandflash_wl_testing.c$
* $Version : 3.7.0.1$
* $Date    : Aug-9-2012$
*
* Comments:
*
*   This file contains NAND flash driver functions.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "fio.h"
#include "fio_prv.h"
#include "io.h"
#include "io_prv.h"
#include "../nandflash_wl_testing/nandflash_wl_testing.h"
#include "../nandflash_wl_testing/nandflashprv_wl_testing.h"
#include "nandflash_wl_prv.h"
#include "media_buffer_manager.h"
#include "ddi_media.h"
#include "wl_testing/testing_main.h"
#include "wl_testing/testing_main_internal.h"
#include "ddi_NandHalSpy.h"


#if !NANDFLASH_SUPPORT_READ_WRITE_METADATA  
#error NANDFlash WearLeveling requires this option
#endif
extern NANDFLASH_INIT_STRUCT _bsp_nandflash_wl_init;

MQX_FILE_PTR g_nand_wl_ptr;


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_wl_testing_install
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         :
*    Install a NAND flash driver.
*
*END*----------------------------------------------------------------------*/

_mqx_uint _io_nandflash_wl_testing_install
(
/* [IN] The initialization structure for the device */
NANDFLASH_INIT_STRUCT_CPTR  init_ptr
)
{ /* Body */
   IO_NANDFLASH_STRUCT_PTR     dev_ptr;

   if (NULL == init_ptr )
   {
      
      init_ptr = &_bsp_nandflash_wl_init;
   }
   
   dev_ptr = (IO_NANDFLASH_STRUCT_PTR)_mem_alloc_system_zero(
   (_mem_size)sizeof(IO_NANDFLASH_STRUCT));

#if MQX_CHECK_MEMORY_ALLOCATION_ERRORS
   if (dev_ptr == NULL) {
      return MQX_OUT_OF_MEMORY;
   } /* Endif */
#endif

   dev_ptr->INIT                 = init_ptr->INIT;
   dev_ptr->DEINIT               = init_ptr->DEINIT;
   dev_ptr->CHIP_ERASE           = init_ptr->CHIP_ERASE;
   dev_ptr->BLOCK_ERASE          = init_ptr->BLOCK_ERASE;
   dev_ptr->PAGE_READ            = init_ptr->PAGE_READ;
   dev_ptr->PAGE_PROGRAM         = init_ptr->PAGE_PROGRAM;
   dev_ptr->WRITE_PROTECT        = init_ptr->WRITE_PROTECT;
   dev_ptr->IS_BLOCK_BAD         = init_ptr->IS_BLOCK_BAD;
   dev_ptr->MARK_BLOCK_AS_BAD    = init_ptr->MARK_BLOCK_AS_BAD;
   dev_ptr->IOCTL                = init_ptr->IOCTL;
   dev_ptr->NANDFLASH_INFO_PTR   = init_ptr->NANDFLASH_INFO_PTR;
   dev_ptr->VIRTUAL_PAGE_SIZE    = BSP_VIRTUAL_PAGE_SIZE;
   dev_ptr->NUM_VIRTUAL_PAGES    = (init_ptr->NANDFLASH_INFO_PTR->BLOCK_SIZE / BSP_VIRTUAL_PAGE_SIZE) * init_ptr->NANDFLASH_INFO_PTR->NUM_BLOCKS;
   dev_ptr->PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO    
   = (init_ptr->NANDFLASH_INFO_PTR->PHY_PAGE_SIZE / BSP_VIRTUAL_PAGE_SIZE);
   dev_ptr->ECC_SIZE             = BSP_ECC_SIZE;
   dev_ptr->WRITE_VERIFY         = init_ptr->WRITE_VERIFY;
   dev_ptr->DEVICE_SPECIFIC_DATA = init_ptr->DEVICE_SPECIFIC_DATA;
   
#if NANDFLASH_SUPPORT_READ_WRITE_PHYSICAL_PAGE
   dev_ptr->PHY_PAGE_READ_RAW		= init_ptr->PHY_PAGE_READ_RAW;
   dev_ptr->PHY_PAGE_WRITE_RAW		= init_ptr->PHY_PAGE_WRITE_RAW;
#endif

   dev_ptr->PAGE_READ_METADATA		= init_ptr->PAGE_READ_METADATA;
   dev_ptr->PAGE_READ_WITH_METADATA	= init_ptr->PAGE_READ_WITH_METADATA;
   dev_ptr->PAGE_WRITE_WITH_METADATA	= init_ptr->PAGE_WRITE_WITH_METADATA;
   _lwsem_create(&dev_ptr->LWSEM, 1);
   
   return (_io_dev_install_ext(
   //      init_ptr->ID_PTR,
   "nandflash_wl_testing:",
   _io_nandflash_wl_testing_open,
   _io_nandflash_wl_testing_close,
   _io_nandflash_wl_read,
   _io_nandflash_wl_write,
   _io_nandflash_wl_testing_ioctl, /* Override this IOCTL function */
   _io_nandflash_wl_uninstall, 
   (void *)dev_ptr)); 
} /* Endbody */


/*FUNCTION*-------------------------------------------------------------------
* 
* Function Name    : _io_nandflash_wl_open
* Returned Value   : _mqx_uint a task error code or MQX_OK
* Comments         : Opens and initializes NAND flash driver.
* 
*END*----------------------------------------------------------------------*/

_mqx_int _io_nandflash_wl_testing_open
(
/* [IN] the file handle for the device being opened */
MQX_FILE_PTR fd_ptr,

/* [IN] the remaining portion of the name of the device */
char *   open_name_ptr,

/* [IN] the flags to be used during operation: */
char *   flags
)
{ /* Body */
   // return _io_nandflash_open(fd_ptr, open_name_ptr, flags);
   
   IO_DEVICE_STRUCT_PTR      io_dev_ptr = fd_ptr->DEV_PTR;
   IO_NANDFLASH_STRUCT_PTR   handle_ptr = 
   (IO_NANDFLASH_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
   uint32_t                   chip_initialized;
   uint32_t                   nand_id, block_size_kB, density, num_ecc_bytes;   
   _mqx_int status;
   
   if (handle_ptr->COUNT) {
      /* Device is already opened */
      fd_ptr->FLAGS = handle_ptr->FLAGS;
      handle_ptr->COUNT++;
      return MQX_OK;
   } /* Endif */
   
   if (handle_ptr->INIT) {
      chip_initialized = (*handle_ptr->INIT)(handle_ptr);
   }/* Endif */

#if MQX_CHECK_ERRORS
   if (chip_initialized != NANDFLASHERR_NO_ERROR) {
      return(IO_ERROR);
   } /* Endif */
#endif   

/* Check if the sum of virtual page size (incl. ECC bytes) per one physical page 
      is not greater than the physical page size plus the number of physical spare bytes. */
   num_ecc_bytes = NANDFLASH_ECC_SIZE_TO_NUM_BYTES_CONV(handle_ptr->ECC_SIZE);
#if !(BSP_M5329EVB || BSP_TWRMPC5125)
   if(((handle_ptr->VIRTUAL_PAGE_SIZE + num_ecc_bytes) * handle_ptr->PHY_PAGE_SIZE_TO_VIRTUAL_PAGE_SIZE_RATIO) > 
         (handle_ptr->NANDFLASH_INFO_PTR->PHY_PAGE_SIZE + handle_ptr->NANDFLASH_INFO_PTR->SPARE_AREA_SIZE))
   {
      return ((_mqx_int)NANDFLASHERR_IMPROPER_ECC_SIZE);
   }
#endif
   
   handle_ptr->COUNT++;
   
   /* Init paramters for nandwl testing */
   g_nand_wl_ptr = fd_ptr;
   
   nandwl_testing_get_info();
   return MQX_OK;  
} /* Endbody */

_mqx_int _io_nandflash_wl_testing_close
(
/* [IN] the file handle for the device being closed */
MQX_FILE_PTR fd_ptr
)
{
   return _io_nandflash_close(fd_ptr);
}

/*FUNCTION*****************************************************************
* 
* Function Name    : _io_nandflash_wl_testing_ioctl
* Returned Value   : int32_t
* Comments         :
*    Returns result of ioctl operation.
*
*END*********************************************************************/

_mqx_int _io_nandflash_wl_testing_ioctl
(
/* [IN] the file handle for the device */
MQX_FILE_PTR fd_ptr,

/* [IN] the ioctl command */
_mqx_uint  cmd,

/* [IN/OUT] the ioctl parameters */
void *    param_ptr
)
{ /* Body */
   IO_DEVICE_STRUCT_PTR               io_dev_ptr = fd_ptr->DEV_PTR;
   IO_NANDFLASH_STRUCT_PTR            handle_ptr = 
   (IO_NANDFLASH_STRUCT_PTR)io_dev_ptr->DRIVER_INIT_PTR;
   _mqx_int                           result = MQX_OK;
   _mqx_uint                          i;
   _mqx_uint_ptr                      uparam_ptr;
   NANDFLASH_WL_TESTING_PARAM_PTR testing_ptr;
   
   
   switch (cmd) {
   case NANDFLASH_WL_IOCTL_TESTING:
      testing_ptr = (NANDFLASH_WL_TESTING_PARAM_PTR)param_ptr;
      for (i=0; i< NANDWL_TC_COUNT; i++ )
      {
      	 if ((g_testcases[i].testcase_id == testing_ptr->caseID)
      	 	&&(g_testcases[i].test_func == NULL))      	 	
      	 {
      	 	testing_ptr->out_val = kTestCaseNA;
      	 	continue;
      	 }
         if ((g_testcases[i].testcase_id == testing_ptr->caseID )
         		&&(g_testcases[i].tc_type == testing_ptr->run_type)
               /*&& (g_testcases[i].test_func != NULL)*/)
         {
            g_testcases[i].test_func(testing_ptr);
            break;
         }
      }
      
      /* Can't found the given test case */
      if (i==NANDWL_TC_COUNT)
      {
      	testing_ptr->out_val = kTestCaseNA;
      }
      
      break;

   default:
      result = _io_nandflash_wl_ioctl(fd_ptr, cmd, param_ptr);  
      break;
   } /* Endswitch */
   return result;

} /* Endbody */

/* EOF */
