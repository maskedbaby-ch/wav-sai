#ifndef _mfs_init_h_
#define _mfs_init_h_
/**HEADER********************************************************************
* 
* Copyright (c) 2008-2013 Freescale Semiconductor;
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
* $FileName: mfs_init.h$
* $Version : 3.8.2.0$
* $Date    : Jul-19-2011$
*
* Comments:
*
*   This file contains the include information for the MFS test utility
*   functions.
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <string.h>
#include <fio.h>
#include <mfs.h>
#include "util.h"

#define RAM_DISK		1
#define USB_DISK		2 
#define SDCARD_DISK		4
extern MFS_FORMAT_DATA MFS_format;

#if DISK_FOR_MFS_TEST & RAM_DISK

extern MQX_FILE_PTR Init_MFS_RAM(uint32_t size);

#ifdef USE_BLOCK_MODE
#define MEM_OPEN_MODE    "b"
#else
#define MEM_OPEN_MODE    0
#endif


#endif

#if DISK_FOR_MFS_TEST & USB_DISK

#include <usb.h>
#include <host_common.h>
#include <host_cnfg.h>
#include <hostapi.h>
#include <lwmsgq.h>
#include <mqx_host.h>
#include "usb_host_msd_bo.h"
#include "usb_file.h"

typedef struct {
    CLASS_CALL_STRUCT_PTR ccs;     /* class call struct of MSD instance */
    uint8_t                body;    /* message body one of USB_EVENT_xxx as defined above */
} usb_msg_t;

typedef struct device_struct {
   /* This must be the first member of this structure, because sometimes we can use it
   ** as pointer to CLASS_CALL_STRUCT, other time as a pointer to DEVICE_STRUCT
   */
   CLASS_CALL_STRUCT                ccs;
   /* Mount point, 'c' for "c:" */
   char                             mount;
} DEVICE_STRUCT, * DEVICE_STRUCT_PTR;

extern void usb_host_mass_device_event(_usb_device_instance_handle, _usb_interface_descriptor_handle, uint32_t);
/* Table of driver capabilities this application want to use */

static const USB_HOST_DRIVER_INFO ClassDriverInfoTable[] =
{  
   /* Vendor ID Product ID Class Sub-Class Protocol Reserved Application call back */
   /* Floppy drive */
   {{0x00,0x00}, {0x00,0x00}, USB_CLASS_MASS_STORAGE, USB_SUBCLASS_MASS_UFI, USB_PROTOCOL_MASS_BULK, 0, usb_host_mass_device_event },
   /* USB 2.0 hard drive */
   {{0x00,0x00}, {0x00,0x00}, USB_CLASS_MASS_STORAGE, USB_SUBCLASS_MASS_SCSI, USB_PROTOCOL_MASS_BULK, 0, usb_host_mass_device_event},
   /* End of list */
   {{0x00,0x00}, {0x00,0x00}, 0,0,0,0, NULL}
};


#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACED             (4)
#define  USB_DEVICE_DETACHED               (5)
#define  USB_DEVICE_OTHER                  (6)

#define USB_EVENT_ATTACH    (1)
#define USB_EVENT_DETACH    (2)
#define USB_EVENT_INTF      (3)
#define USB_END_TEST		(4)

extern USB_FILESYSTEM_STRUCT_PTR Init_MFS_USB(void);

#endif /* if DISK_FOR_MFS_TEST = "USB" */

#if DISK_FOR_MFS_TEST & SDCARD_DISK

#include <part_mgr.h>
#include <sdcard.h>
#include <spi.h>

extern MQX_FILE_PTR Init_MFS_SDCARD(void);

#endif /* if DISK_FOR_MFS_TEST = "SDCARD" */
 

#endif /*_mfs_init_h_*/

/* EOF */
