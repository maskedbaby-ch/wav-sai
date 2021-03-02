#ifndef _nandflash_wl_testing_h_
#define _nandflash_wl_testing_h_
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
* $FileName: nandflash_wl_testing.h$
* $Version : 3.6.0.1$
* $Date    : Aug-9-2012$
*
* Comments:
*
*   The file contains functions prototype, defines, structure 
*   definitions specific for the NAND flash driver.
*
*END************************************************************************/

#include "ioctl.h"
#include "nandflash.h"
#include "nandflash_wl.h"
// #include "nandhal_spy.h"
/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

/*
** NAND Flash IOCTL testing command calls
*/

#define NANDFLASH_IOCTL_WL_TESTING_NR_START_BASE			_IOC_NR(NANDFLASH_IOCTL_WL_END_BASE)
#define NANDFLASH_WL_IOCTL_TESTING		 		            _IO(IO_TYPE_NANDFLASH, NANDFLASH_IOCTL_WL_TESTING_NR_START_BASE + 1)

/*----------------------------------------------------------------------*/
/*
 *							STRUCTURE 
 */
enum
{
	kTC_Automatic =0,
	kTC_Manual,
	kTC_NA	
} ;

typedef struct {
	uint32_t run_type;
	uint32_t caseID;
	uint32_t in_val;
	uint32_t out_val;
} NANDFLASH_WL_TESTING_PARAM, * NANDFLASH_WL_TESTING_PARAM_PTR;

typedef struct {
	uint32_t tc_type;
	uint32_t testcase_id;
	void (_CODE_PTR_ test_func)(NANDFLASH_WL_TESTING_PARAM_PTR);
} NANDFLASH_WL_TESTING_CASE, *  NANDFLASH_WL_TESTING_CASE_PTR;

extern NANDFLASH_WL_TESTING_CASE g_testcases[];

extern MQX_FILE_PTR g_nand_wl_ptr;

// NandHalSpy nandHalSpy;

/*----------------------------------------------------------------------*/
/*
**                    FUNCTION PROTOTYPES
*/

#ifdef __cplusplus
extern "C" {
#endif
  
extern _mqx_uint _io_nandflash_wl_testing_install(NANDFLASH_INIT_STRUCT_CPTR);
extern _mqx_int  _io_nandflash_wl_testing_uninstall(IO_DEVICE_STRUCT_PTR);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
