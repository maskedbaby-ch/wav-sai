#ifndef __usb_file_h__
#define __usb_file_h__
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
* $FileName: usb_file.h$
* $Version : 3.8.4.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This file contains the Test specific data structures and defines
*
*END************************************************************************/
#include <stdarg.h>
#include <mqx.h>
#include <fio.h>
#include <mfs.h>
#include <usbmfs.h>
#include <part_mgr.h>

typedef struct {
   MQX_FILE_PTR DEV_FD_PTR;
   MQX_FILE_PTR PM_FD_PTR;
   MQX_FILE_PTR FS_FD_PTR;
   char    *DEV_NAME;
   char    *PM_NAME;
   char    *FS_NAME;
} USB_FILESYSTEM_STRUCT, * USB_FILESYSTEM_STRUCT_PTR;



#ifdef __cplusplus
extern "C" {
#endif
extern void *usb_filesystem_install( 
   void       *usb_handle,
   char    *block_device_name,
   char    *partition_manager_name,
   char    *file_system_name );
extern void usb_filesystem_uninstall( USB_FILESYSTEM_STRUCT_PTR  usb_fs_ptr);
extern MQX_FILE_PTR usb_filesystem_handle( USB_FILESYSTEM_STRUCT_PTR  usb_fs_ptr);

   
#ifdef __cplusplus
}
#endif

#endif
/* EOF */
