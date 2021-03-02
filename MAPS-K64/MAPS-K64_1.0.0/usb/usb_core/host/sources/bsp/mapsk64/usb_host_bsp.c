/**HEADER********************************************************************
* 
* Copyright (c) 2013 - 2014 Freescale Semiconductor;
* All Rights Reserved
*
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
* Comments:  
*
*END************************************************************************/
#include "adapter.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#define SIM_SOPT2_IRC48MSEL_MASK                 0x30000u
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
    #if (defined(CPU_MK64F12))
#include "MK64F12/MK64F12_sim.h"
#include "MK64F12/MK64F12_usb.h"
//#include "MK64F12/MK64F12_gpio.h"
//#include "MK64F12/MK64F12_port.h"
#include "MK64F12/MK64F12_mpu.h"
    #endif
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#include "MK64F12.h"
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#define BSP_USB_INT_LEVEL                (4)
#define BSPCFG_USB_USE_IRC48M                    (0)
static int32_t bsp_usb_host_io_init
(
    int32_t i
)
{
	int32_t ret = 0;
	
    if (i == 0)
    {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)      
    	/* USB clock divider */
		if(kClockManagerSuccess != CLOCK_SYS_SetDivider(kClockDividerUsbDiv, 4U))
		{
			ret = -1;
		}
		if(kClockManagerSuccess != CLOCK_SYS_SetDivider(kClockDividerUsbFrac, 1U))
		{
			ret = -1;
		}
    	/* PLL/FLL selected as CLK source */
		if(kClockManagerSuccess != CLOCK_SYS_SetSource(kClockUsbSrc, 1U))
		{
			ret = -1;
		}
		if(kClockManagerSuccess != CLOCK_SYS_SetSource(kClockPllfllSel, 1U))
		{
			ret = -1;
		}
		/* USB Clock Gating */
		CLOCK_SYS_EnableUsbClock(i);
		/* Enable clock gating to all ports C*/
		CLOCK_SYS_EnablePortClock(3);
        /* Weak pull downs */
        HW_USB_USBCTRL_WR(USB0_BASE, 0x40);

        /* Souce the P5V0_MAPS_K64_USB. Set PTA6 to high */
        BW_PORT_PCRn_MUX(PORTA_BASE, 6, 1); /* GPIO mux */
        HW_GPIO_PDDR_SET(PTA_BASE, 1<<6);        /* Set output */
        HW_GPIO_PCOR_WR(PTA_BASE, HW_GPIO_PCOR_RD(PTA_BASE) | 1<<6);   /* Output low */        
        
#else
	 /* Configure USB to be clocked from PLL0 */
	 HW_SIM_SOPT2_SET(SIM_SOPT2_USBSRC_MASK );
	 BW_SIM_SOPT2_PLLFLLSEL(1);
	 /* Configure USB divider to be 120MHz * 2 / 5 = 48 MHz */
	 BW_SIM_CLKDIV2_USBFRAC(2 - 1);
	 BW_SIM_CLKDIV2_USBDIV(5 - 1);
	 /* Enable USB-OTG IP clocking */
	 HW_SIM_SCGC4_SET(SIM_SCGC4_USBOTG_MASK);

      SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	  BW_PORT_PCRn_MUX(HW_PORTA, 6, 1); /* GPIO mux */
	  HW_GPIO_PDDR_SET(0, 1<<6);        /* Set output */
	  HW_GPIO_PCOR_WR(0, HW_GPIO_PCOR(0).U | 1<<6);       /* Output low */
      
#endif
    }
    else
    {
        ret = -1; //unknow controller
    }

    return ret;
    
}


int32_t bsp_usb_host_init(uint8_t controller_id)
{
    int32_t result = 0;

    result = bsp_usb_host_io_init(controller_id);
    if (result != 0)
    {
        return result;
    }
    if (0 == controller_id)
    {
#if (OS_ADAPTER_ACTIVE_OS != OS_ADAPTER_SDK)
        /* MPU is disabled. All accesses from all bus masters are allowed */
        HW_MPU_CESR_WR(0);
        /* Do not configure enable USB regulator for host */
        /* reset USB CTRL register */
        HW_USB_USBCTRL_WR(0);

        /* setup interrupt */
        OS_intr_init(soc_get_usb_vector_number(0), BSP_USB_INT_LEVEL, 0, TRUE);
#else
       //Disable MPU so the module can access RAM
       //HW_MPU_CESR_CLR(MPU_CESR, MPU_CESR_VLD_MASK);
        MPU_CESR=0;
        /* reset USB CTRL register */
	      HW_USB_USBCTRL_WR(USB0_BASE, 0);
        /* setup interrupt */
        OS_intr_init(soc_get_usb_vector_number(0), BSP_USB_INT_LEVEL, 0, TRUE);
#endif
    }
    else
    {
        /* unknown controller */
        result = -1;
    }

    return result;
}
#endif
/* EOF */
