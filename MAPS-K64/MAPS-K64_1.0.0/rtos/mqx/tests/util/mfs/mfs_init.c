#include "mfs_init.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#if DISK_FOR_MFS_TEST == RAM_DISK

MQX_FILE_PTR Init_MFS_RAM(uint32_t size)
{
   unsigned char      *ramdisk_ptr;
   MQX_FILE_PTR   dev_handle,mfs_fd_ptr;
   uint32_t        error_code;
   MFS_IOCTL_FORMAT_PARAM     format_struct;
   
   format_struct.FORMAT_PTR = &MFS_format;
   
   ramdisk_ptr = (unsigned char *)_mem_alloc_system_zero(size);     
   error_code = _io_mem_install("MFSRAM:", ramdisk_ptr, size);
   
   EU_ASSERT(error_code == MQX_OK);
   if ( error_code != MQX_OK ) {
      return NULL;
   }
   
   /* Open the device which MFS will be installed on */
   dev_handle = fopen("MFSRAM:", MEM_OPEN_MODE);

   EU_ASSERT(dev_handle != NULL);
   if ( dev_handle == NULL ) {
      return NULL;
   }
   
   /* Install MFS  */
   error_code = _io_mfs_install(dev_handle, "MFS:", 0);
   
   EU_ASSERT(error_code == MFS_NO_ERROR);
   if (error_code != MFS_NO_ERROR) {
      return NULL;
   }
   
   /* Open the filesystem. Should need to format it */
   mfs_fd_ptr = fopen("MFS:", 0);
   
   EU_ASSERT(mfs_fd_ptr !=0);
   if ( !mfs_fd_ptr ) {
      return NULL;
   }
   
   //printf("2.2 ferror ");
   error_code = ferror(mfs_fd_ptr);
   EU_ASSERT(error_code == MFS_NOT_A_DOS_DISK);
   if ( error_code != MFS_NOT_A_DOS_DISK ) {
      return NULL;
   }
  
   //printf("2.3 Formatting MFS ");
   error_code = ioctl(mfs_fd_ptr, IO_IOCTL_FORMAT, (uint32_t *) &format_struct);
   
   EU_ASSERT(error_code == MFS_NO_ERROR);
   if (error_code != MFS_NO_ERROR) {
      return NULL;
   }
   
   return mfs_fd_ptr;
}

#endif // if DISK_FOR_MFS_TEST == RAM_DISK

#if DISK_FOR_MFS_TEST == USB_DISK

/* The granularity of message queue is one message. Its size is the multiplier of _mqx_max_type. Get that multiplier */
#define USB_TASKQ_GRANM ((sizeof(usb_msg_t) - 1) / sizeof(_mqx_max_type) + 1)
_mqx_max_type  usb_taskq[20 * USB_TASKQ_GRANM * sizeof(_mqx_max_type)]; /* prepare message queue for 20 events */
void   *usb_dev_handle; //used for install-USB test as extern
USB_FILESYSTEM_STRUCT_PTR Init_MFS_USB(void)
{
   uint32_t              error;
   _usb_host_handle     host_handle;
   usb_msg_t            msg;   
   uint32_t              fs_mountp = 0;   
   void                *usb_fs_handle = NULL; 
   MFS_IOCTL_FORMAT_PARAM     format_struct;
   
   format_struct.FORMAT_PTR = &MFS_format;  
   
   if (MQX_OK != _lwmsgq_init(usb_taskq, 20, USB_TASKQ_GRANM)) {
       // lwmsgq_init failed
       _mqx_exit(0);
   }
   
    USB_lock();
    _int_install_unexpected_isr();
    error = _usb_host_driver_install(USBCFG_DEFAULT_HOST_CONTROLLER);
    EU_ASSERT_FATAL(error == MQX_OK );
    if (error != USB_OK) {
      USB_unlock();
      return NULL;
    }

    error = _usb_host_init(USBCFG_DEFAULT_HOST_CONTROLLER, &host_handle);
    EU_ASSERT_FATAL(error == MQX_OK );
    if (error != USB_OK) {
      USB_unlock();
      return NULL;
    }
    error = _usb_host_driver_info_register(host_handle, (void *)ClassDriverInfoTable);
    EU_ASSERT(error == MQX_OK );
    if (error != USB_OK) {
      USB_unlock();
      return NULL;
    }
    error = _usb_host_register_service(host_handle, USB_SERVICE_HOST_RESUME,NULL);
    EU_ASSERT(error == MQX_OK );    
    if (error != USB_OK) {
      USB_unlock();
      return NULL;
    }
    USB_unlock();
    
    while (1) {
		/* Wait for event sent as a message */
		_lwmsgq_receive(&usb_taskq, (_mqx_max_type *) &msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0);

		switch (msg.body) {
			case USB_EVENT_INTF:{
				unsigned char   drive[3];
				/* get free mountpoint as a string like "c:" */
				for (drive[0] = 'c'; drive[0] <= 'z'; drive[0]++)
					if (!(fs_mountp & (1 << (drive[0] - 'a'))))
						break;
				drive[1] = ':';
				drive[2] = 0;
				
				// Install the file system, use device->ccs as a handle
				usb_dev_handle = (void *)msg.ccs;
				usb_fs_handle = (void *)usb_filesystem_install( (void *)msg.ccs, "USB:", "PM_C1:", "c:" );
				if(usb_fs_handle){
				  //printf("2.3 Formatting MFS ");
               error = ioctl(((USB_FILESYSTEM_STRUCT_PTR)usb_fs_handle)->FS_FD_PTR, IO_IOCTL_FORMAT, (uint32_t *) &format_struct);
               EU_ASSERT(error == MFS_NO_ERROR);
               if (error != MFS_NO_ERROR) {
                  return NULL;
               } else {
                  return ((USB_FILESYSTEM_STRUCT_PTR)usb_fs_handle);
               }
				     
				}else
				  return NULL;
				
			}break;
			
			case USB_EVENT_DETACH:{
				DEVICE_STRUCT_PTR dsp = (DEVICE_STRUCT_PTR) msg.ccs;
				if (dsp->mount >= 'a' && dsp->mount <= 'z') {

					// Remove the file system 
					usb_filesystem_uninstall(usb_fs_handle);
					// Mark file system as unmounted
					fs_mountp &= ~(1 << (dsp->mount - 'a'));
				}

				/* Here, the device finishes its lifetime */            
				_mem_free(dsp);
			}break;
			
			default:
				break;
		} /* end switch */
	}/* end for */
} /* Endbody */

void usb_host_mass_device_event
   (
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint32_t           event_code
   )
{
   DEVICE_STRUCT_PTR          device;
   usb_msg_t                  msg;

   switch (event_code) {
      case USB_CONFIG_EVENT:
         /* Drop through into attach, same processing */
      case USB_ATTACH_EVENT:
         /* Here, the device starts its lifetime */
         device = (DEVICE_STRUCT_PTR) _mem_alloc_zero(sizeof(DEVICE_STRUCT));
         if (device == NULL)
            break;

         if (USB_OK != _usb_hostdev_select_interface(dev_handle, intf_handle, &device->ccs))
            break;
         msg.ccs = &device->ccs;
         msg.body = USB_EVENT_ATTACH;
         if (LWMSGQ_FULL == _lwmsgq_send(usb_taskq, (uint32_t *) &msg, 0)) {
            printf("Could not inform USB task about device attached\n");
         }
         break;

      case USB_INTF_EVENT:
         if (USB_OK != usb_class_mass_get_app(dev_handle, intf_handle, (CLASS_CALL_STRUCT_PTR *) &device))
            break;
         msg.ccs = &device->ccs;
         msg.body = USB_EVENT_INTF;
         if (LWMSGQ_FULL == _lwmsgq_send(usb_taskq, (uint32_t *) &msg, 0)) {
            printf("Could not inform USB task about device interfaced\n");
         }
         break;

      case USB_DETACH_EVENT:
         if (USB_OK != usb_class_mass_get_app(dev_handle, intf_handle, (CLASS_CALL_STRUCT_PTR *) &device))
            break;
         msg.ccs = &device->ccs;
         msg.body = USB_EVENT_DETACH;
         if (LWMSGQ_FULL == _lwmsgq_send(usb_taskq, (uint32_t *) &msg, 0)) {
            printf("Could not inform USB task about device detached\n");
         }
         break;

      default:
         break;
   } 
} 

#endif // if DISK_FOR_MFS_TEST == USB_DISK

#if DISK_FOR_MFS_TEST == SDCARD_DISK

#if defined BSP_SDCARD_ESDHC_CHANNEL
#if ! BSPCFG_ENABLE_ESDHC
#error "This application requires BSPCFG_ENABLE_ESDHC defined non-zero in user_config.h. Please recompile libraries with this option."
#else
#define SDCARD_COM_CHANNEL BSP_SDCARD_ESDHC_CHANNEL
#endif

#elif defined BSP_SDCARD_SDHC_CHANNEL
#if ! BSPCFG_ENABLE_SDHC
#error "This application requires BSPCFG_ENABLE_SDHC defined non-zero in user_config.h. Please recompile libraries with this option."
#else
#define SDCARD_COM_CHANNEL BSP_SDCARD_SDHC_CHANNEL
#endif

#elif defined BSP_SDCARD_SPI_CHANNEL
#define SDCARD_COM_CHANNEL BSP_SDCARD_SPI_CHANNEL
#else
#error "SDCARD low level communication device not defined!"
#endif

MQX_FILE_PTR Init_MFS_SDCARD(void)
{
   MQX_FILE_PTR               com_handle,
                              sdcard_handle,
                              mfs_handle;
   uint32_t                   error_code;
   MFS_IOCTL_FORMAT_PARAM     format_struct;

   format_struct.FORMAT_PTR = &MFS_format;

   /* Open low level communication device */
   com_handle = fopen(SDCARD_COM_CHANNEL, (void *)(SPI_FLAG_FULL_DUPLEX));
   EU_ASSERT(com_handle != NULL);
   if (NULL == com_handle)
   {
       printf("Error opening communication handle %s, check your user_config.h.\n", SDCARD_COM_CHANNEL);
       return NULL;
   }

   /* Install SD card device */
   error_code = _io_sdcard_install("SDCARD:", (void *)&_bsp_sdcard0_init, com_handle);
   EU_ASSERT(error_code == MQX_OK);
   if ( error_code != MQX_OK )
   {
      return NULL;
   }

   /* Open the device which MFS will be installed on */
   sdcard_handle = fopen("SDCARD:", 0);
   EU_ASSERT(sdcard_handle != NULL);
   if (sdcard_handle == NULL)
   {
      return NULL;
   }

   /* Install MFS over the SD card */
   error_code = _io_mfs_install(sdcard_handle, "MFS:", 0);
   EU_ASSERT(error_code == MFS_NO_ERROR);
   if (error_code != MFS_NO_ERROR)
   {
      return NULL;
   }

   /* Open filesystem on the partition */
   mfs_handle = fopen("MFS:", NULL);
   EU_ASSERT(mfs_handle != NULL);
   if (mfs_handle == NULL)
   {
      return NULL;
   }

   /* Format */
   /* Whether the SD card has already been formatted will not be checked.
    * The reason why to do so is to ensure the test case can be repeated in the sense that
    * files or subdirectories created last time will be automatically deleted via formatting
    */
   error_code = ioctl(mfs_handle, IO_IOCTL_FORMAT, (uint32_t *) &format_struct);
   EU_ASSERT(error_code == MFS_NO_ERROR);
   if (error_code != MFS_NO_ERROR)
   {
      return NULL;
   }

   return mfs_handle;

}

#endif // if DISK_FOR_MFS_TEST == RAM_DISK
