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
* $FileName: msd_commands.c$
* $Version : 3.8.38.0$
* $Date    : Oct-4-2012$
*
* Comments:
*
*   This file contains device driver for mass storage class. This code tests
*   the UFI set of commands.
*
*END************************************************************************/

/**************************************************************************
Include the OS and BSP dependent files that define IO functions and
basic types. You may like to change these files for your board and RTOS 
**************************************************************************/
/**************************************************************************
Include the USB stack header files.
**************************************************************************/
#include <mqx.h>
#include <usb.h>

#include <hostapi.h>

#include "usb_types.h"
#ifdef __USB_OTG__
#include "otgapi.h"
#include "devapi.h"
#else
#include "hostapi.h"
#endif


/**************************************************************************
Local header files for this application
**************************************************************************/
#include "msd_commands.h"

/**************************************************************************
Class driver files for this application
**************************************************************************/

#include "usb_host_msd_bo.h"
#include "usb_host_msd_ufi.h"
#include <usb_host_hub_sm.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


/**************************************************************************
A driver info table defines the devices that are supported and handled
by file system application. This table defines the PID, VID, class and
subclass of the USB device that this application listens to. If a device
that matches this table entry, USB stack will generate a attach callback.
As noted, this table defines a UFI class device and a USB
SCSI class device (e.g. high-speed hard disk) as supported devices.
see the declaration of structure USB_HOST_DRIVER_INFO for details
or consult the software architecture guide.
**************************************************************************/


static  USB_HOST_DRIVER_INFO DriverInfoTable[] =
{
   /* Floppy drive */
   {
      {0x00, 0x00},                 /* Vendor ID per USB-IF             */
      {0x00, 0x00},                 /* Product ID per manufacturer      */
      USB_CLASS_MASS_STORAGE,       /* Class code                       */
      USB_SUBCLASS_MASS_UFI,        /* Sub-Class code                   */
      USB_PROTOCOL_MASS_BULK,       /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_mass_device_event    /* Application call back function   */
   },

   /* USB 2.0 hard drive */
   {

      {0x00, 0x00},                 /* Vendor ID per USB-IF             */
      {0x00, 0x00},                 /* Product ID per manufacturer      */
      USB_CLASS_MASS_STORAGE,       /* Class code                       */
      USB_SUBCLASS_MASS_SCSI,       /* Sub-Class code                   */
      USB_PROTOCOL_MASS_BULK,       /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_mass_device_event    /* Application call back function   */
   },

   /* USB 1.1 hub */
   {

      {0x00, 0x00},                 /* Vendor ID per USB-IF             */
      {0x00, 0x00},                 /* Product ID per manufacturer      */
      USB_CLASS_HUB,                /* Class code                       */
      USB_SUBCLASS_HUB_NONE,        /* Sub-Class code                   */
      USB_PROTOCOL_HUB_ALL,          /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_hub_device_event     /* Application call back function   */
   },

   {
      {0x00,0x00},                  /* All-zero entry terminates        */
      {0x00,0x00},                  /*    driver info list.             */
      0,
      0,
      0,
      0,
      NULL
   }

};

/**************************************************************************
   Global variables
**************************************************************************/
volatile DEVICE_STRUCT mass_device = { 0 };   /* mass storage device struct */
volatile bool bCallBack = FALSE;
volatile USB_STATUS bStatus       = USB_OK;
volatile uint32_t  dBuffer_length = 0;
bool  Attach_Notification = FALSE;
_usb_host_handle        host_handle;         /* global handle for calling host   */

/* the following is the mass storage class driver object structure. This is
used to send commands down to  the class driver. See the Class API document
for details */
int event_detach=0;
COMMAND_OBJECT_PTR pCmd;

/*some handles for communicating with USB stack */
_usb_host_handle     host_handle; /* host controller status etc. */

static unsigned char *buff_out, *buff_in;

/**************************************************************************
The following is the way to define a multi tasking system in MQX RTOS.
Remove this code and use your own RTOS way of defining tasks (or threads).
**************************************************************************/

#define MAIN_TASK          (10)
extern void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK,      Main_Task,      3000L,  9L, "Main",      MQX_AUTO_START_TASK},
   { 0L,             0L,             0L,     0L, 0L,          0L }
};

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : main
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*--------------------------------------------------------------------*/

void Main_Task ( uint32_t param )
{ /* Body */
   USB_STATUS status = USB_OK;
   //_mqx_uint  i=0;

   _int_disable();
   //int test_done;
   //test_done=0;

   //_int_install_unexpected_isr();
   if (MQX_OK != _usb_host_driver_install(USBCFG_DEFAULT_HOST_CONTROLLER)) {
      printf("\n Driver installation failed");
      _task_block();
   }

   status = _usb_host_init(USBCFG_DEFAULT_HOST_CONTROLLER, &host_handle);

   if (status != USB_OK) {
      _int_enable();
      printf("\nUSB Host Initialization failed. STATUS: %x", status);
      fflush(stdout);
      _task_block();
   } /* Endif */

   /*
   ** Since we are going to act as the host driver, register
   ** the driver information for wanted class/subclass/protocols
   */
   status = _usb_host_driver_info_register(host_handle, DriverInfoTable);
   if (status != USB_OK) {
      _int_enable();
      printf("\nDriver Registration failed. STATUS: %x", status);
      fflush(stdout);
      _task_block();
   }

   _int_enable();

   printf("\nPlease insert Mass Storage Device.\n");
   /*----------------------------------------------------**
   ** Infinite loop, waiting for events requiring action **
   **----------------------------------------------------*/
   for ( ; ; ) {
         switch ( mass_device.dev_state ) {
            case USB_DEVICE_IDLE:
               break ;
            case USB_DEVICE_ATTACHED:
               printf("Mass Storage Device attached\n");
               fflush(stdout);
               mass_device.dev_state = USB_DEVICE_SET_INTERFACE_STARTED;
               status = _usb_hostdev_select_interface(mass_device.dev_handle,
                    mass_device.intf_handle, (void *)&mass_device.class_intf);
               break ;
            case USB_DEVICE_SET_INTERFACE_STARTED:
               break;
            case USB_DEVICE_INTERFACED:
               mass_device.dev_state = USB_DEVICE_IDLE;
               usb_host_mass_test_storage();
               break ;
            case USB_DEVICE_DETACHED:
               printf ("\nMass Storage Device detached\n");
               fflush(stdout);
               mass_device.dev_state = USB_DEVICE_IDLE;
               break;
            case USB_DEVICE_OTHER:
               break ;
            default:
               printf("\nUnknown Mass Storage Device State = %d\n", mass_device.dev_state );
               fflush(stdout);
               break ;
         } /* Endswitch */
      } /* Endfor */
   
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_device_event
* Returned Value : None
* Comments       :
*     called when mass storage device has been attached, detached, etc.
*END*--------------------------------------------------------------------*/

static void usb_host_mass_device_event
   (
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint32_t           event_code
   )
{ /* Body */
   //int test_done;
   switch (event_code) {
      case USB_CONFIG_EVENT:
         /* Drop through into attach, same processing */
      case USB_ATTACH_EVENT:    	  
    	 if ((mass_device.dev_handle == dev_handle)||(mass_device.dev_handle != NULL)){
    		 return;
    	 }   	        
         if (mass_device.dev_state == USB_DEVICE_IDLE) {
            mass_device.dev_handle  = dev_handle;
            mass_device.intf_handle = intf_handle;
            mass_device.dev_state   = USB_DEVICE_ATTACHED;
		 } else {
            printf("Mass Storage Device Is Already Attached\n");
            fflush(stdout);
         } /* EndIf */
         break;
      case USB_INTF_EVENT:   	 
    	 if (mass_device.dev_handle!=dev_handle) {
    	             /* No such device found. */
    	     return;
    	  } 
         mass_device.dev_state = USB_DEVICE_INTERFACED;
         break;
      case USB_DETACH_EVENT:    	 
    	  fflush(stdout);
    	  event_detach=1;
    	  mass_device.dev_handle = NULL;
    	  mass_device.intf_handle = NULL;
    	  mass_device.dev_state = USB_DEVICE_DETACHED;
         break;
      default:
         mass_device.dev_state = USB_DEVICE_IDLE;
         break;
   } /* EndSwitch */  
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_ctrl_callback
* Returned Value : None
* Comments       :
*     Called on completion of a control-pipe transaction.
*
*END*--------------------------------------------------------------------*/

static void usb_host_mass_ctrl_callback
   (
      /* [IN] pointer to pipe */
      _usb_pipe_handle  pipe_handle,

      /* [IN] user-defined parameter */
      void             *user_parm,

      /* [IN] buffer address */
      unsigned char         *buffer,

      /* [IN] length of data transferred */
      uint32_t           buflen,

      /* [IN] status, hopefully USB_OK or USB_DONE */
      uint32_t           status
   )
{ /* Body */

   bCallBack = TRUE;
   bStatus = status;

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_bulk_callback
* Returned Value : None
* Comments       :
*     Called on completion of a bulk transaction.
*
*END*--------------------------------------------------------------------*/

void usb_host_mass_bulk_callback
   (
      /* [IN] Status of this command */
      USB_STATUS status,

      /* [IN] pointer to USB_MASS_BULK_ONLY_REQUEST_STRUCT*/
      void   *p1,

      /* [IN] pointer to the command object*/
      void   *p2,

      /* [IN] Length of data transmitted */
      uint32_t buffer_length
   )
{ /* Body */

   dBuffer_length = buffer_length;
   bCallBack = TRUE;
   bStatus = status;

} /* Endbody */

void callback_test
   (
      /* [IN] Status of this command */
      USB_STATUS status
   )
{/* Body */
   if ((status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
   {
	  printf ("\n...ERROR\n");
	  return;
   }
   else
   {
	 /* Wait till command comes back */
	  while((!bCallBack)&&(!event_detach)) {;}
	  if (event_detach){
	     event_detach=0;
	  }
	  if (!bStatus) {
	     printf("...OK\n");
	  }
	  else {
	     printf("...Command failed, the callback status is: (bStatus=0x%x)\n", bStatus);
	  }
   }
}/* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_test_storage
* Returned Value : None
* Comments       :
*     Calls the UFI command set for testing
*END*--------------------------------------------------------------------*/

static void usb_host_mass_test_storage
   (
      /* [IN] Index to the table of DEVICE_STRUCT */
      //_mqx_uint i   
   )
{ /* Body */
   USB_STATUS                                 status = USB_OK;
   uint8_t                                     *bLun;
   _mqx_int                                   block_len;
   DEVICE_DESCRIPTOR_PTR                      pdd;
   MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO *read_capacity;
   INQUIRY_DATA_FORMAT                        *inquiry;
   _usb_device_instance_handle                dev_handle = mass_device.dev_handle;
   //PIPE_INIT_PARAM_STRUCT_PTR                 pBulk_pipe = NULL;
   //uint8_t                                    direction;
   //USB_MASS_CLASS_INTF_STRUCT_PTR             intf_ptr ;
   //CLASS_CALL_STRUCT_PTR                      ccs_ptr;
   //USB_MASS_CLASS_INTF_STRUCT_PTR             plocal_intf = NULL;
   //unsigned char                              ch9status[2]={0,0};
   int                                        test_loop;  
   
   _usb_hostdev_get_buffer(dev_handle, sizeof(COMMAND_OBJECT_STRUCT), (void **) &pCmd);
   if (pCmd == NULL)
   {
      printf ("\nUnable to allocate Command Object");
      fflush (stdout);
      return;
   }
   _mem_zero(pCmd, sizeof(COMMAND_OBJECT_STRUCT));

   _usb_hostdev_get_buffer(dev_handle, sizeof(CBW_STRUCT), (void **) &pCmd->CBW_PTR);
   if (pCmd->CBW_PTR == NULL)
   {
      printf ("\nUnable to allocate Command Block Wrapper");
      fflush (stdout);
      return;
   }
   _mem_zero(pCmd->CBW_PTR, sizeof(CBW_STRUCT));

   _usb_hostdev_get_buffer(dev_handle, sizeof(CSW_STRUCT), (void **) &pCmd->CSW_PTR);
   if (pCmd->CSW_PTR == NULL)
   {
      printf ("\nUnable to allocate Command Status Wrapper");
      fflush (stdout);
      return;
   }
   _mem_zero(pCmd->CSW_PTR, sizeof(CSW_STRUCT));

   _usb_hostdev_get_buffer(dev_handle, BUFF_SIZE, (void **) &buff_in);
   if (buff_in == NULL)
   {
      printf ("\nUnable to allocate Input Buffer");
      fflush (stdout);
      return;
   }
   _mem_zero(buff_in, BUFF_SIZE);

   _usb_hostdev_get_buffer(dev_handle, 0x0F, (void **) &buff_out);
   if (buff_out == NULL)
   {
      printf ("\nUnable to allocate Output Buffer");
      fflush (stdout);
      return;
   }
   _mem_zero(buff_out, 0x0F);

   _usb_hostdev_get_buffer(dev_handle, 0x01, (void **) &bLun);
   if (bLun == NULL)
   {
      printf ("\nUnable to allocate LUN number");
      fflush (stdout);
      return;
   }
   _mem_zero(bLun, 0x01);

   printf("\n =============START OF A NEW SESSION==================");

   if (USB_OK != _usb_hostdev_get_descriptor(
      dev_handle,
      NULL,
      USB_DESC_TYPE_DEV,
      0,
      0,
      (void **) &pdd))
   {
      printf ("\nCould not retrieve device descriptor.");
      return;
   }
   else {
      printf("\nVID = 0x%04x, PID = 0x%04x", SHORT_LE_TO_HOST(*(uint16_t*)pdd->idVendor), SHORT_LE_TO_HOST(*(uint16_t*)pdd->idProduct));
   }
   for(test_loop=0;test_loop<1;test_loop++){// test_loop
   /* Test the GET MAX LUN command */ 
   printf("\nTesting: GET MAX LUN Command\n");
   fflush(stdout);
   bCallBack = FALSE;
   
   status = usb_class_mass_getmaxlun_bulkonly(
      (void *)&mass_device.class_intf, bLun,
      usb_host_mass_ctrl_callback, NULL);
   
   if ((status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
   {
	  printf ("\n...ERROR");
	  return;
   }
   else
   {
	 /* Wait till command comes back */
	  while((!bCallBack)&&(!event_detach)) {;}
	  if (event_detach){
	     event_detach=0;
	  }
	  if (!bStatus) {
	     printf("...(%d)...OK\n", *bLun);
	  }
	  else {
	     printf("...Command failed, the callback status is: (bStatus=0x%x)\n", bStatus);
	  }
   }

   pCmd->LUN = 0;
   pCmd->CALL_PTR = (void *)&mass_device.class_intf;
   pCmd->CALLBACK = usb_host_mass_bulk_callback;
   
   /* Test the REQUEST SENSE command */
   printf("Testing: REQUEST SENSE Command\n");
   fflush(stdout);
   bCallBack = FALSE;
   pCmd->RETRY_COUNT=0;
   status = usb_mass_ufi_request_sense(pCmd, buff_in, sizeof(REQ_SENSE_DATA_FORMAT));
   callback_test(status);

   /* Test the TEST UNIT READY command */
   printf("Testing: TEST UNIT READY Command\n");
   fflush(stdout);
   bCallBack = FALSE;
   pCmd->RETRY_COUNT=0;
   status =  usb_mass_ufi_test_unit_ready(pCmd);  
   callback_test(status);

   /* Test the INQUIRY command */
   printf("Testing: INQUIRY Command\n");
   fflush(stdout);

   bCallBack = FALSE;
   pCmd->RETRY_COUNT=0;
   status = usb_mass_ufi_inquiry(pCmd, (unsigned char *) buff_in, sizeof(INQUIRY_DATA_FORMAT));

   if ((status != USB_OK) && (status != USB_STATUS_TRANSFER_QUEUED))
   {
      printf ("\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while ((!bCallBack)&&(!event_detach)){;}
      if (event_detach){
          	   event_detach=0;
             }
      if (!bStatus) {
         printf("...OK\n");
         /* Get the vendor, product and revision information and display it */
         inquiry = (INQUIRY_DATA_FORMAT_PTR)buff_in;
         printf("\n************************************************************************\n");
         printf("Vendor Information:     %-1.8s\n", inquiry->BVID);
         printf("Product Identification: %-1.16s\n",inquiry->BPID);
         printf("Product Revision Level: %-1.4s\n",inquiry->BPRODUCT_REV);
         printf("************************************************************************\n");
      }
      else {
         printf("...Command failed, the callback status is: (bStatus=0x%x)\n", bStatus);
      }
   }
   
   /* Test the REQUEST SENSE command */
   printf("Testing: REQUEST SENSE Command\n");
   fflush(stdout);
   bCallBack = FALSE;
   pCmd->RETRY_COUNT=0;
   status = usb_mass_ufi_request_sense(pCmd, buff_in, sizeof(REQ_SENSE_DATA_FORMAT));
   callback_test(status);
   
   /* Test the READ FORMAT CAPACITY command */
   printf("Testing: READ FORMAT CAPACITIES Command\n");
   fflush(stdout);
   bCallBack = FALSE;
   pCmd->RETRY_COUNT=0;
   status = usb_mass_ufi_format_capacity(pCmd, buff_in, sizeof(CAPACITY_LIST));
   callback_test(status);
   
   /* Test the READ CAPACITY command */
   printf("Testing: READ CAPACITY Command\n");
   fflush(stdout);
   bCallBack = FALSE;
   pCmd->RETRY_COUNT=0;
   status = usb_mass_ufi_read_capacity(pCmd, (unsigned char *) buff_in, sizeof(MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO));
   callback_test(status);
   read_capacity = (MASS_STORAGE_READ_CAPACITY_CMD_STRUCT_INFO_PTR) buff_in;
   block_len = LONG_BE_TO_HOST(*(uint32_t*)&read_capacity->BLENGTH);
   //_time_delay(40000);
   
   /* Test the REQUEST SENSE command */
   printf("Testing: REQUEST SENSE Command\n");
   fflush(stdout);
   bCallBack = FALSE;
   pCmd->RETRY_COUNT=0;
   status = usb_mass_ufi_request_sense(pCmd, buff_in, sizeof(REQ_SENSE_DATA_FORMAT));
   callback_test(status);

    /* Test the READ(10) command */
    printf("Testing: READ(10) Command\n");
    fflush(stdout);
    bCallBack = FALSE;
    pCmd->RETRY_COUNT=0;
    /* Read block_len bytes, but not more than BUFF_SIZE */
    status = usb_mass_ufi_read_10(pCmd, 0, buff_in, block_len > BUFF_SIZE ?  BUFF_SIZE : block_len, 1);
    callback_test(status);
    
    /* Test the READ(12) command */
    printf("Testing: READ(12) Command\n");
    fflush(stdout);
    bCallBack = FALSE;
    pCmd->RETRY_COUNT=0;
    /* Read block_len bytes, but not more than BUFF_SIZE */
    status = usb_mass_ufi_read_12(pCmd, 0, buff_in, block_len > BUFF_SIZE ?  BUFF_SIZE : block_len, 1);
    callback_test(status);
    
    /* Test the MODE SENSE command */
    printf("Testing: MODE SENSE Command\n");
    fflush(stdout);
    bCallBack = FALSE;
    pCmd->RETRY_COUNT=0;
    status = usb_mass_ufi_mode_sense(pCmd,
       2, //PC
       0x3F, //page code
       buff_in,
       (uint32_t)0x08);
    callback_test(status);

    /* Test the MODE SELECT command */
    printf("Testing: MODE SELECT Command\n");
    fflush(stdout);
    bCallBack = FALSE;
    pCmd->RETRY_COUNT=0;
    status = usb_mass_ufi_mode_select(pCmd,
       buff_in,
       (uint32_t)0x08);
    callback_test(status);
    
    /* Test the PREVENT ALLOW command */
    printf("Testing: PREVENT-ALLOW MEDIUM REMOVAL Command\n");
    fflush(stdout);
    bCallBack = FALSE;
    pCmd->RETRY_COUNT=0;
    status = usb_mass_ufi_prevent_allow_medium_removal(
       pCmd,
       1 // prevent
       );
    callback_test(status);
    
    /* Test the REQUEST SENSE command */
    printf("Testing: REQUEST SENSE Command\n");
    fflush(stdout);
    bCallBack = FALSE;
    pCmd->RETRY_COUNT=0;
    status = usb_mass_ufi_request_sense(pCmd, buff_in, sizeof(REQ_SENSE_DATA_FORMAT));
    callback_test(status);
    
    /* Test the VERIFY command */
    printf("Testing: VERIFY Command\n");
    fflush(stdout);
    bCallBack = FALSE;
    pCmd->RETRY_COUNT=0;
    status = usb_mass_ufi_verify(
       pCmd,
       0x400, // block address
       1 //length to be verified
       );
    callback_test(status);
    
 #if TEST_WRITE 
    /* Test the WRITE(10) command */
    printf("Testing: WRITE(10) Command\n");
    fflush(stdout);
    bCallBack = FALSE;
    pCmd->RETRY_COUNT=0;
    /* Write random data stored in buff_in */
    status = usb_mass_ufi_write_10(pCmd, 8, buff_in, block_len > BUFF_SIZE ?  BUFF_SIZE : block_len , 1);
    callback_test(status);

    /* Test the WRITE(12) command */
    printf("Testing: WRITE(12) Command\n");
    fflush(stdout);
    bCallBack = FALSE;
    pCmd->RETRY_COUNT=0;
    /* Write random data stored in buff_in */
    status = usb_mass_ufi_write_12(pCmd, 8, buff_in, block_len > BUFF_SIZE ?  BUFF_SIZE : block_len , 1);
    callback_test(status);
    
    /* Test the WRITE and VERIFY command */
    printf("Testing: WRITE and VERIFY Command\n");
    fflush(stdout);
    bCallBack = FALSE;
    pCmd->RETRY_COUNT=0;
    /* Write random data stored in buff_in */
    status = usb_mass_ufi_write_and_verify(pCmd, 8, buff_in, block_len > BUFF_SIZE ?  BUFF_SIZE : block_len , 1);
    callback_test(status);
    
    /* Test the REQUEST SENSE command */
    printf("Testing: REQUEST SENSE Command\n");
    fflush(stdout);
    bCallBack = FALSE;
    pCmd->RETRY_COUNT=0;
    status = usb_mass_ufi_request_sense(pCmd, buff_in, sizeof(REQ_SENSE_DATA_FORMAT));
    callback_test(status);

 #endif 

    /* Test the START-STOP UNIT command */
    printf("Testing: START-STOP UNIT Command\n");
    fflush(stdout);
    bCallBack = FALSE;
    pCmd->RETRY_COUNT=0;
    status = usb_mass_ufi_start_stop(pCmd, 0, 1);
    callback_test(status);
    
    printf("Testloop is:%d\n", test_loop);    
   } //end of the test loop 
  printf("\nTest done!");
  fflush(stdout);
} /* Endbody */
