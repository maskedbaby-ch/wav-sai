/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: user_config.h$
* $Version : 3.8.1.0$
* $Date    : Aug-14-2012$
*
* Comments:
*
*   User configuration for MQX components
*
*END************************************************************************/

#ifndef __user_config_h__
#define __user_config_h__

/* mandatory CPU identification */
#define MQX_CPU                           PSP_CPU_MPXS30
#define MQX_USE_INTERRUPTS                1

#define BSPCFG_ENABLE_IO_SUBSYSTEM        1

/* ttyb CORE_0 - on board serial console over OSBDM (DEFAULT) */
/* ttyd CORE_1 - TWR-SER RS232 serial console (PCS9)          */
#define BSP_DEFAULT_IO_CHANNEL            "ttyb:"
#define BSP_DEFAULT_IO_CHANNEL_DEFINED    1

// Assign console to each core
#define BSP_CORE_0_IO_CHANNEL           "ttyb:"
#define CORECFG_LINFLEX_1               CORE_0

#define BSP_CORE_1_IO_CHANNEL           "ttyd:"
#define CORECFG_LINFLEX_3               CORE_1

/* Uncomenting these line causes PIT to be used as system timer */
//#define BSPCFG_TIMER_PIT_DEVICE         (0)
//#define BSPCFG_TIMER_PIT_CHANNEL        (0)
//#define BSPCFG_TIMER_INT_LEVEL          (1)

#define BSPCFG_ENABLE_ENET_STATS          0
#define BSPCFG_ENABLE_CPP                 0
#define BSPCFG_ENABLE_TTYA                0 
#define BSPCFG_ENABLE_ITTYA               1               
#define BSPCFG_ENABLE_TTYB                0
#define BSPCFG_ENABLE_ITTYB               1
#define BSPCFG_ENABLE_TTYC                0
#define BSPCFG_ENABLE_ITTYC               0
#define BSPCFG_ENABLE_TTYD                0
#define BSPCFG_ENABLE_ITTYD               0
#define BSPCFG_ENABLE_TTYE                0
#define BSPCFG_ENABLE_ITTYE               0
#define BSPCFG_ENABLE_TTYF                0
#define BSPCFG_ENABLE_ITTYF               0
#define BSPCFG_ENABLE_TTYG                0
#define BSPCFG_ENABLE_ITTYG               0
#define BSPCFG_ENABLE_TTYH                0
#define BSPCFG_ENABLE_ITTYH               0
#define BSPCFG_ENABLE_TTYI                0
#define BSPCFG_ENABLE_ITTYI               0
#define BSPCFG_ENABLE_TTYJ                0
#define BSPCFG_ENABLE_ITTYJ               0

#define BSPCFG_ENABLE_SPI0                0
#define BSPCFG_ENABLE_ISPI0               0
#define BSPCFG_ENABLE_SPI1                0
#define BSPCFG_ENABLE_ISPI1               0
#define BSPCFG_ENABLE_SPI2                0
#define BSPCFG_ENABLE_ISPI2               0
#define BSPCFG_ENABLE_SPI3                0
#define BSPCFG_ENABLE_ISPI3               0
#define BSPCFG_ENABLE_SPI4                0
#define BSPCFG_ENABLE_ISPI4               0
#define BSPCFG_ENABLE_SPI5                0
#define BSPCFG_ENABLE_ISPI5               0
#define BSPCFG_ENABLE_SPI6                0
#define BSPCFG_ENABLE_ISPI6               0
#define BSPCFG_ENABLE_SPI7                0
#define BSPCFG_ENABLE_ISPI7               0
#define BSPCFG_ENABLE_SPI8                0
#define BSPCFG_ENABLE_ISPI8               0
#define BSPCFG_ENABLE_SPI9                0
#define BSPCFG_ENABLE_ISPI9               0

#define BSPCFG_ENABLE_NANDFLASH           0
#define BSPCFG_ENABLE_GPIODEV             0
#define BSPCFG_ENABLE_RTCDEV              0
#define BSPCFG_ENABLE_PCFLASH             0
#define BSPCFG_ENABLE_SDHC                0
#define BSPCFG_ENABLE_I2C0                0
#define BSPCFG_ENABLE_II2C0               0
#define BSPCFG_ENABLE_FLASHX              0

/*
** board-specific MQX settings - see for defaults mqx\source\include\mqx_cnfg.h
*/

#define MQXCFG_ENABLE_FP                  0
#define MQX_INCLUDE_FLOATING_POINT_IO     0

/*
** board-specific RTCS settings - see for defaults rtcs\source\include\rtcscfg.h
*/

#define SHELLCFG_USES_RTCS                0
#define RTCSCFG_ENABLE_LWDNS              0
#define TELNETDCFG_NOWAIT                 FALSE 

/*
** include common settings
*/

/* and enable verification checks in kernel */
#include "verif_enabled_config.h"

#endif
