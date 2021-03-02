/**HEADER********************************************************************
* 
* Copyright (c) 2004 - 2010, 2013 - 2014 Freescale Semiconductor;
* All Rights Reserved
*
*
*************************************************************************** 
*
* @file composite_app.c
*
* @author
*
* @version
*
* @date 
*
* @brief  
*****************************************************************************/

/******************************************************************************
* Includes
*****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "virtual_com.h"    /* CDC Application Header File */
#include "disk.h"           /* MSD Application Header File */
#include "usb_class_composite.h"
#include "composite_app.h"

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
//#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_gpio_common.h"

#include "fsl_port_hal.h"
#include <stdio.h>
#include <stdlib.h>
#endif

#if ! USBCFG_DEV_COMPOSITE
#error This application requires USBCFG_DEV_COMPOSITE defined non-zero in usb_device_config.h. Please recompile usbd with this option.
#endif

/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
 #define COMPOSITE_CFG_MAX 2

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void TestApp_Init(void);
extern void Main_Task(uint32_t param);
/****************************************************************************
 * Global Variables
 ****************************************************************************/              
extern disk_struct_t g_disk;
extern cdc_handle_t g_vcom_handle;
extern usb_desc_request_notify_struct_t  desc_callback;
comosite_handle_t   g_app_handle;
static class_config_struct_t composite_cfg[COMPOSITE_CFG_MAX] = {0};

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
extern void Main_Task(uint32_t param);
#define MAIN_TASK       10

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK, Main_Task, 2*3000L, 7L, "Main", MQX_AUTO_START_TASK, 0, 0},
   { 0L, 0L, 0L, 0L, 0L, 0L, 0, 0}
};
#endif
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Local Variables 
 *****************************************************************************/

/*****************************************************************************
 * Local Functions
 *****************************************************************************/
/*****************************************************************************
*  
*    @name         APP_init
* 
*    @brief         This function do initialization for APP.
* 
*    @param         None
* 
*    @return      None
**                  
*****************************************************************************/
void APP_init()
{
    composite_config_struct_t usb_composite_callback;
    class_config_struct_t vcom_config_callback;
    class_config_struct_t disk_config_callback;
    /* Initialize Vcom and Disk App */
    VCom_TestApp_Init();
    Disk_TestApp_Init();
    vcom_config_callback.composite_application_callback.callback = VCom_USB_App_Callback;
    vcom_config_callback.composite_application_callback.arg = &g_vcom_handle;
    vcom_config_callback.class_specific_callback.callback = (usb_class_specific_handler_func)VCom_USB_Notif_Callback;
    vcom_config_callback.class_specific_callback.arg = &g_vcom_handle;
    vcom_config_callback.desc_callback_ptr = &desc_callback;
    vcom_config_callback.type = USB_CLASS_CDC;
    
    disk_config_callback.composite_application_callback.callback = Disk_USB_App_Callback;
    disk_config_callback.composite_application_callback.arg = &g_disk.app_handle;
    disk_config_callback.class_specific_callback.callback = (usb_class_specific_handler_func)MSD_Event_Callback;
    disk_config_callback.class_specific_callback.arg = &g_disk.app_handle;
    disk_config_callback.desc_callback_ptr = &desc_callback;
    disk_config_callback.type = USB_CLASS_MSC;
     
    usb_composite_callback.count = 2;
    usb_composite_callback.class_app_callback = composite_cfg;
    usb_composite_callback.class_app_callback[0] = vcom_config_callback;
    usb_composite_callback.class_app_callback[1] = disk_config_callback;
    
    /* Initialize the USB interface */
    USB_Composite_Init(CONTROLLER_ID, &usb_composite_callback, &g_app_handle);
    USB_Composite_Get_Class_Handle(g_app_handle, USB_CLASS_CDC, &g_vcom_handle);
    USB_Composite_Get_Class_Handle(g_app_handle, USB_CLASS_MSC, &g_disk.app_handle);
}
/*****************************************************************************
*  
*    @name         APP_task
* 
*    @brief         This function runs APP task.
*    @param         None
* 
*    @return      None
**                  
*****************************************************************************/
void APP_task()
{
    while(TRUE)
    {
        VCom_TestApp_Task();
        Disk_TestApp_Task();
    }
}

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : Main_Task
* Returned Value : None
* Comments       :
*     First function called.  Calls the Test_App
*     callback functions.
* 
*END*--------------------------------------------------------------------*/
void Main_Task( uint32_t param )
{   
    UNUSED_ARGUMENT (param)
    APP_init();
}
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { 1L,     Main_Task,      2500L,  MQX_MAIN_TASK_PRIORITY, "Main",      MQX_AUTO_START_TASK},
   { 0L,     0L,             0L,    0L, 0L,          0L }
};
#endif

static void Task_Start(void *arg)
{
    APP_init();

    APP_task();
    OSA_TaskDestroy(OSA_TaskGetHandler());
}

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param)
#else
void main(void)
#endif
{
    OSA_Init();
    hardware_init();
    dbg_uart_init();
    
    OS_Task_create(Task_Start, NULL, 9L, 3000L, "task_start", NULL);

    OSA_Start();
}
#endif
/* EOF */
