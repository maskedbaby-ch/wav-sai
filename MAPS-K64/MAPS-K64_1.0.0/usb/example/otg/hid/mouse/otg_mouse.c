/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
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
* $FileName: otg_mouse.c$
* $Version :
* $Date    :
*
* Comments:
*
*
*END************************************************************************/

/******************************************************************************
* Includes
*****************************************************************************/
#include "usb.h"
#include "usb_otg_main.h"
#include "host_mouse_api.h"
#include "dev_mouse_api.h"
#include "otg_mouse.h"
#include "usb_host_config.h"
#include "usb_device_config.h"
#include "usb_host_stack_interface.h"
#include "usb_host_hid.h"
#include "host_mouse.h"

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#include "fsl_port_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "uart/fsl_uart_driver.h"
#endif 
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
#include "sci.h"
#include "adapter_bm.h"
#endif
/*****************************************************************************
* Constant and Macro's 
*****************************************************************************/
#define USBCFG_DEFAULT_OTG_CONTROLLER 0
#if  USBCFG_DEV_COMPOSITE
#error This application requires USBCFG_DEV_COMPOSITE defined zero in usb_device_config.h. Please recompile USB library with this option.
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#if ! BSPCFG_ENABLE_I2C0
#error This application requires BSPCFG_ENABLE_I2C0 defined non-zero in user_config.h. Please recompile BSP with this option.
#endif
#endif
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#define MAIN_TASK                       (10)
#endif
/*****************************************************************************
* Local Types 
*****************************************************************************/
typedef enum
{
    dev_b = 0,
    dev_a
} dev_type_t;
/*****************************************************************************
* Global Functions Prototypes
*****************************************************************************/
extern void     OTG_App_Init(void);
extern void     DEV_APP_init(void);
extern void     HOST_APP_init(void);
extern void     DEV_APP_uninit(void);
extern void     HOST_APP_uninit(void);
void            OTG_App_Periodic_Task(void);
extern void     HOST_APP_task_stun( uint32_t param );
extern void     DEV_APP_task_stun( uint32_t param );
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && (defined (FSL_RTOS_MQX)))
void            Main_Task(uint32_t param);
#endif
/****************************************************************************
* Global Variables
****************************************************************************/
extern                  device_struct_t        g_mouse_hid_device; 
extern                  hid_command_t*         g_mouse_hid_com;
usb_otg_handle          g_otg_handle;
uint32_t                g_otg_state ;
dev_type_t              g_dev_type;               /* dev_type = 0 (device B); dev_type = 1 (device A) */
bool                    g_sess_vld ;              /* TRUE if session is valid */
bool                    g_vbus_err ;              /* VBUS overcurrent */
os_event_handle         g_otg_app_event_handle;
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
TASK_TEMPLATE_STRUCT    MQX_template_list[] =
{
    /* { TASK_TEMPLATE_INDEX, TASK_ADDRESS, TASK_STACKSIZE, TASK_PRIORITY, TASK_NAME, TASK_ATTRIBUTES}, */    
    { MAIN_TASK,           (TASK_FPTR)Main_Task,         3000L,           10L,            "Main",    MQX_AUTO_START_TASK },
    { DEV_APP_TASK_INDEX,  (TASK_FPTR)DEV_APP_task_stun, 2000L,           11L,            "Device", 0},
    { HOST_APP_TASK_INDEX, (TASK_FPTR)HOST_APP_task_stun,3000L,           11L,            "Host",   0},
    { 0L,                   0L,                          0L,              0L,             0L,       0L }
};
#endif

/*****************************************************************************
* Local Functions Prototypes
*****************************************************************************/
static void     App_OtgCallback(usb_otg_handle handle, os_event_handle event);
static void     App_HandleUserInput(void);
static void     App_PrintMenu(void);
static void     App_Print_Event(usb_otg_handle handle, os_event_handle event);
/*****************************************************************************
* Local Variables 
*****************************************************************************/
static const    otg_int_struct_t       g_otg_init =
{
    App_OtgCallback,
    HOST_APP_load,
    DEV_APP_load,
    HOST_APP_unload,
    DEV_APP_unload,
};

/*****************************************************************************
* Local Functions
*****************************************************************************/
/************************************************************************************
**
** Globals
************************************************************************************/

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : OTG_App_Load_Unload_Task 
* Returned Value : none
* Comments       :
*    
*
*END*--------------------------------------------------------------------*/
void OTG_App_Load_Unload_Task(os_event_handle event)
{
    if(OS_Event_check_bit(event, OTG_UNLOAD_DEVICE)) 
    {
        OS_Lock();
        DEV_APP_uninit();
        OS_Event_clear(event,OTG_UNLOAD_DEVICE);   
        OS_Unlock();
    }
    if(OS_Event_check_bit(event, OTG_UNLOAD_HOST)) 
    {
        OS_Lock();
        HOST_APP_uninit();
        OS_Event_clear(event,OTG_UNLOAD_HOST); 
        OS_Unlock();
    }
    if(OS_Event_check_bit(event, OTG_LOAD_DEVICE)) 
    {
        OS_Lock();
        DEV_APP_init();
        OS_Event_clear(event,OTG_LOAD_DEVICE);  
        OS_Unlock();
    }
    if(OS_Event_check_bit(event, OTG_LOAD_HOST)) 
    {
        OS_Lock();
        HOST_APP_init();
        OS_Event_clear(g_otg_app_event_handle,OTG_LOAD_HOST);  
        OS_Unlock();
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : OTG_App_Periodic_Task 
* Returned Value : none
* Comments       :
*    
*
*END*--------------------------------------------------------------------*/
void APP_task()
{
    uint32_t otg_app_event_value;
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)||((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)&& USE_RTOS))  
    OS_Event_wait(g_otg_app_event_handle,OTG_LOAD_UNLOAD_EVENT_MARK, FALSE, 10);
#endif  

    if(OS_Event_check_bit(g_otg_app_event_handle, OTG_LOAD_UNLOAD_EVENT_MARK)) 
    {
        OTG_App_Load_Unload_Task(g_otg_app_event_handle);
    }
    App_HandleUserInput();
}
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : OTG_App_Init
* Returned Value : none
* Comments       :
*
*
*END*--------------------------------------------------------------------*/
void APP_init()
{
    /* Initialize the current platform. Call for the _bsp_platform_init which is specific to each processor family */
    OS_Lock();
    g_otg_handle  = NULL;
    g_otg_state = 0;
    g_dev_type = dev_b;        
    g_sess_vld = FALSE;    
    g_vbus_err = FALSE;

    g_otg_app_event_handle = OS_Event_create(0);
    printf("\n\r otg module is initilalizing:");
    _usb_otg_init(USBCFG_DEFAULT_OTG_CONTROLLER, (otg_int_struct_t *)&g_otg_init, &g_otg_handle);
    printf("\n\rPress P to print the menu:");
    OS_Unlock();
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : App_OtgCallback
* Returned Value : none
* Comments       :
*
*
*END*--------------------------------------------------------------------*/
void App_OtgCallback(usb_otg_handle handle, os_event_handle event)
{
    uint8_t device_state;
    device_state = _usb_otg_get_state(handle);
    if(device_state == USB_OTG_DEVSTATE_B)
    {
        if(OS_Event_check_bit(event, OTG_B_IDLE)) 
        {
            g_dev_type = dev_b;               /* Device type: B */
            g_sess_vld = FALSE;               /* session not valid */
            g_otg_state = OTG_B_IDLE;
        }
        if(OS_Event_check_bit(event, OTG_B_IDLE_SRP_READY)) 
        {
            g_otg_state = OTG_B_IDLE_SRP_READY;
        }
        if(OS_Event_check_bit(event, OTG_B_PERIPHERAL)) 
        {
            g_otg_state = OTG_B_PERIPHERAL;
            if (g_sess_vld == FALSE)
            {
                g_sess_vld = TRUE;                /* session valid */
            }    
        }
        if(OS_Event_check_bit(event, OTG_B_HOST)) 
        {     
            g_otg_state = OTG_B_HOST;
        }
        if(OS_Event_check_bit(event, OTG_B_A_HNP_REQ)) 
        {
            (void)_usb_otg_bus_release(g_otg_handle); 
        }
    }
   else if(device_state == USB_OTG_DEVSTATE_A)
   {
        if(OS_Event_check_bit(event, OTG_A_WAIT_BCON_TMOUT)) 
        {
            _usb_otg_set_a_bus_req(g_otg_handle , FALSE);
        }
        if(OS_Event_check_bit(event, OTG_A_BIDL_ADIS_TMOUT)) 
        {
            _usb_otg_set_a_bus_req(g_otg_handle, TRUE);    
        }
        if(OS_Event_check_bit(event, OTG_A_B_HNP_REQ)) 
        {
            _usb_otg_set_a_bus_req( handle , FALSE);
        }
        if(OS_Event_check_bit(event, OTG_A_IDLE)) 
        {
            g_dev_type = dev_a;                 /* Device type: A */
            g_sess_vld = FALSE;
            g_otg_state = OTG_A_IDLE;
        }
        if(OS_Event_check_bit(event, OTG_A_WAIT_VRISE)) 
        {
            g_otg_state = OTG_A_WAIT_VRISE;
            _usb_otg_set_a_bus_req( handle , TRUE);
        }
        if(OS_Event_check_bit(event, OTG_A_WAIT_BCON)) 
        {
            g_sess_vld = TRUE;
            g_otg_state = OTG_A_WAIT_BCON;
        }
        if(OS_Event_check_bit(event, OTG_A_HOST)) 
        {
            g_otg_state = OTG_A_HOST;
        }
        if(OS_Event_check_bit(event, OTG_A_SUSPEND)) 
        {
            g_otg_state = OTG_A_SUSPEND;
        }
        if(OS_Event_check_bit(event, OTG_A_PERIPHERAL))     
        {     
            g_otg_state = OTG_A_PERIPHERAL;
        }
        if(OS_Event_check_bit(event, OTG_A_WAIT_VFALL))     
        {
            if (g_vbus_err == TRUE)
            {
                g_vbus_err = FALSE;
            }
            g_otg_state = OTG_A_WAIT_VFALL;
        }
        if(OS_Event_check_bit(event, OTG_A_VBUS_ERR)) 
        {
            g_vbus_err = TRUE;
            g_otg_state = OTG_A_VBUS_ERR;
        }
    }
    /* print the states */
    App_Print_Event(handle, event);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : App_HandleUserInput
* Returned Value : none
* Comments       :
*
*
*END*--------------------------------------------------------------------*/
extern  const uint32_t g_uartBaseAddr[];
static void App_HandleUserInput(void)
{
    uint8_t character = 0;
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    if (status())
    {
        printf("\n\r");
        character = getchar();
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
    uart_getchar_no_block(&character);
    if(character)  
    {
        printf("\n\r %c", character);        
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
    uint32_t baseAddr = g_uartBaseAddr[BOARD_DEBUG_UART_INSTANCE];
    if(UART_HAL_IsRxDataRegFull(baseAddr))
    UART_HAL_Getchar(baseAddr, &character);
    if(character != (int8_t)0x00)  
    {
        printf("\n\r %c", character);
#endif    
        switch (character)
        {

        case '1':
            if (_usb_otg_session_request(g_otg_handle) == USB_OK)
            {
                printf("\n\rSRP request");
            }
            else
            {
                printf("\n\rError Requesting SRP");
            }
            break;
        case '2': 
            if (_usb_otg_bus_request(g_otg_handle) == USB_OK)
            {
                printf("\n\rB bus request");
            }
            else
            {
                printf("\n\rError requesting the bus");
            }
            break;

        case '3':
            if (_usb_otg_bus_release(g_otg_handle) == USB_OK)
            {
                printf("\n\rB bus release");
            }
            else
            {
                printf("\n\rError releasing the bus");
            }
            break;
        case '4': 
            _usb_otg_set_a_bus_req(g_otg_handle , TRUE);
            printf("\n\rA bus request");
            break;
        case '5': 
            _usb_otg_set_a_bus_req(g_otg_handle , FALSE);
            printf("\n\rA bus release");
            break;
        case '6': 
            _usb_otg_set_a_bus_drop(g_otg_handle , TRUE);
            printf("\n\rA set a bus drop true");
            break;
        case '7': 
            _usb_otg_set_a_bus_drop(g_otg_handle , FALSE);
            printf("\n\rA set a bus drop false");
            break;
        case '8':
            _usb_otg_set_a_clear_err(g_otg_handle);
            break;
        case 'p':
        case 'P': App_PrintMenu();
            break;
        default: break;
        }    
    }    
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : App_PrintMenu
* Returned Value : none
* Comments       :
*
*
*END*--------------------------------------------------------------------*/
static void App_PrintMenu(void)
{
    bool a_bus_req;
    bool a_bus_drop;
    printf("\n\r  OTG App User Input Menu");
    if (g_dev_type == dev_a)
    {
        if (g_vbus_err == FALSE)
        {
            if (g_sess_vld == TRUE)
            {
                _usb_otg_get_a_bus_req(g_otg_handle, &a_bus_req);
                if(FALSE == a_bus_req)
                {
                    printf("\n\r      4. A bus request ");
                }
                else
                {
                    printf("\n\r      5. A bus release ");
                }
                printf("\n\r      6. A set a bus drop true (session end)");
            }
            else  /* session not valid */
            {
                _usb_otg_get_a_bus_req(g_otg_handle, &a_bus_req);
                if(FALSE == a_bus_req)
                {
                    printf("\n\r      4. A bus request ");
                }
                else
                {
                    printf("\n\r      5. A bus release ");
                }
                _usb_otg_get_a_bus_drop(g_otg_handle, &a_bus_drop);
                if (TRUE == a_bus_drop)
                {
                    printf("\n\r      7. A set a bus drop false");
                }
             }
        }
        else    /* no VBUS error */
        {
            printf("\n\r      6. A set a bus drop true (session end)");
            printf("\n\r      8. A clear error");
        }
    }
    else if (g_dev_type == dev_b)
    {
        if (g_sess_vld == TRUE)
        {
            if (g_otg_state == OTG_B_PERIPHERAL)
            {
                printf("\n\r      2. B bus request (HNP start)");
            }
            if (g_otg_state == OTG_B_HOST)
            {
                printf("\n\r      3. B bus release");
            }
        }
        else
        {
            printf("\n\r      1. B session request (SRP start)");
        }
    }
}
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : App_Print_Event
* Returned Value : none
* Comments       :
*
*
*END*--------------------------------------------------------------------*/
static void App_Print_Event(usb_otg_handle handle, os_event_handle event)
{
    uint8_t device_state;
    device_state = _usb_otg_get_state(handle);
    if(device_state == USB_OTG_DEVSTATE_B)
    {
        if(OS_Event_check_bit(event, OTG_B_IDLE)) 
        {
            printf("\n\r>B: OTG state change to B idle");
        }
        if(OS_Event_check_bit(event, OTG_B_IDLE_SRP_READY)) 
        {
            printf("\n\r>B: OTG is ready to initialize SRP");

        }
        if(OS_Event_check_bit(event, OTG_B_SRP_INIT)) 
        {
            printf("\n\r>B: OTG has initialized SRP");
        }    
        if(OS_Event_check_bit(event, OTG_B_SRP_FAIL)) 
        {
            printf("\n\r>B: OTG SRP failed to get a response from the Host");
        }    
        if(OS_Event_check_bit(event, OTG_B_PERIPHERAL)) 
        {
            printf("\n\r>B: OTG state change to B peripheral.");
            printf("\n\r>B: USB peripheral stack initialized.");
            App_PrintMenu();
        }
        if(OS_Event_check_bit(event, OTG_B_PERIPHERAL_LOAD_ERROR))    
        {
            printf("\n\r>B: OTG state change to B peripheral.");
            printf("\n\r>B: USB peripheral stack initialization failed.");
        }
        if(OS_Event_check_bit(event, OTG_B_PERIPHERAL_HNP_READY)) 
        {
            printf("\n\r>B: OTG is ready to initialize HNP.");    
        }
        if(OS_Event_check_bit(event, OTG_B_PERIPHERAL_HNP_START)) 
        {
            printf("\n\r>B: OTG has initialized the HNP to request the bus from Host");    
        }
        if(OS_Event_check_bit(event, OTG_B_PERIPHERAL_HNP_FAIL)) 
        {
            printf("\n\r>B: HNP failed. OTG is back into peripheral state");
        }
        if(OS_Event_check_bit(event, OTG_B_HOST)) 
        {     
            printf("\n\r>B: OTG is in the Host state");
            printf("\n\r>B: USB host stack initialized.");

        }
        if(OS_Event_check_bit(event, OTG_B_HOST_LOAD_ERROR)) 
        {
            printf("\n\r>B: OTG is in the Host state");
            printf("\n\r>B: USB host stack initialization failed.");
        }
        if(OS_Event_check_bit(event, OTG_B_A_HNP_REQ)) 
        {

            printf("\n\r>B: OTG_B_A_HNP_REQ");

        }
    }
    else if (device_state == USB_OTG_DEVSTATE_A)
    {
        if(OS_Event_check_bit(event, OTG_A_WAIT_BCON_TMOUT)) 
        {
            printf("\n\r>A: OTG_A_WAIT_BCON_TMOUT");
        }
        if(OS_Event_check_bit(event, OTG_A_BIDL_ADIS_TMOUT)) 
        {
            printf("\n\r>A: OTG_A_BIDL_ADIS_TMOUT");
        }
        if(OS_Event_check_bit(event, OTG_A_AIDL_BDIS_TMOUT)) 
        {
            printf("\n\r>A: OTG_A_AIDL_BDIS_TMOUT");
        }
        if(OS_Event_check_bit(event, OTG_A_ID_TRUE)) 
        {
            printf("\n\r>A: ID = TRUE ");
        }
        if(OS_Event_check_bit(event, OTG_A_WAIT_VRISE_TMOUT)) 
        {
            printf("\n\r>A: VBUS rise failed");
        }
        if(OS_Event_check_bit(event, OTG_A_B_HNP_REQ)) 
        {
            printf("\n\r>A: OTG_A_B_HNP_REQ");
        }
        if(OS_Event_check_bit(event, OTG_A_IDLE)) 
        {
            printf("\n\r>A: OTG state change to A_IDLE");
        }
        if(OS_Event_check_bit(event, OTG_A_WAIT_VRISE)) 
        {
            printf("\n\r>A: OTG state change to A_WAIT_VRISE");
        }
        if(OS_Event_check_bit(event, OTG_A_WAIT_BCON)) 
        {
            printf("\n\r>A: OTG state change to A_WAIT_BCON");
        }
        if(OS_Event_check_bit(event, OTG_A_HOST)) 
        {
            printf("\n\r>A: OTG state change to OTG_A_HOST");
            printf("\n\r>A: USB host stack initialized.");
        }
        if(OS_Event_check_bit(event, OTG_A_HOST_LOAD_ERROR)) 
        {
            printf("\n\r>A: OTG state change to OTG_A_HOST");
            printf("\n\r>A: USB host stack initialization failed.");
        }
        if(OS_Event_check_bit(event, OTG_A_SUSPEND)) 
        {
            printf("\n\r>A: OTG state change to A_SUSPEND");
        }    
        if(OS_Event_check_bit(event, OTG_A_PERIPHERAL)) 
        {     
            printf("\n\r>A: OTG state change to A_PERIPHERAL    ");
            printf("\n\r>A: USB peripheral stack initialized.");
            App_PrintMenu();
        }    
        if(OS_Event_check_bit(event, OTG_A_PERIPHERAL_LOAD_ERROR)) 
        {
            printf("\n\r>A: USB peripheral stack initialization failed.");
            printf("\n\r>A: OTG state change to A_PERIPHERAL    ");
        }    
        if(OS_Event_check_bit(event, OTG_A_WAIT_VFALL)) 
        {
            printf("\n\r>A: OTG state change to OTG_A_WAIT_VFALL");
        }
        if(OS_Event_check_bit(event, OTG_A_VBUS_ERR)) 
        {
            printf("\n\r>A: VBUS falls below VBUS_Valid treshold");
            printf("\n\r>A: OTG state change to A_VBUS_ERR");
        }
    }
    OS_Event_clear(event,OTG_A_B_STATE_EVENT_MARK);    
}

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : main (Main_Task if using MQX)
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*--------------------------------------------------------------------*/
void Main_Task ( uint32_t param )
{
    OSA_Init();
    init_debug_uart_hardware();
    APP_init();
    /*
    ** Infinite loop, waiting for events requiring action
    */
    for ( ; ; ) {
    	APP_task();
    } /* Endfor */
} /* Endbody */

#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { 1L,     Main_Task,      3000L,  MQX_MAIN_TASK_PRIORITY, "Main",      MQX_AUTO_START_TASK},
   { 0L,     0L,             0L,    0L, 0L,          0L }
};
#endif

static void Task_Start(void *arg)
{
#if (USE_RTOS)
    APP_init();

    for ( ; ; ) {
#endif
    	APP_task();
#if (USE_RTOS)
    } /* Endfor */
#endif
}

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param)
#else
int main(void)
#endif
{
    OSA_Init();
    hardware_init();
    dbg_uart_init();
    
#if !(USE_RTOS)
    APP_init();
#endif

    OS_Task_create(Task_Start, NULL, 10L, 3000L, "task_start", NULL);
    OSA_Start();
#if !defined(FSL_RTOS_MQX)
    return 1;
#endif
}
#endif


