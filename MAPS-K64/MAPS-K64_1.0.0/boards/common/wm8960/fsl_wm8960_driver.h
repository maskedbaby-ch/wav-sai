/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __FSL_WM8960_DRIVER_H__
#define __FSL_WM8960_DRIVER_H__

#include "fsl_i2c_master_driver.h"

/*!
 * @addtogroup WM8960
 * @{
 */

/*! @file */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Define the register address of WM8960. */
#define WM8960_LINVOL       0x0
#define WM8960_RINVOL       0x1
#define WM8960_LOUT1        0x2
#define WM8960_ROUT1        0x3
#define WM8960_CLOCK1       0x4
#define WM8960_DACCTL1      0x5
#define WM8960_DACCTL2      0x6
#define WM8960_IFACE1       0x7
#define WM8960_CLOCK2       0x8
#define WM8960_IFACE2       0x9
#define WM8960_LDAC         0xa
#define WM8960_RDAC         0xb

#define WM8960_RESET        0xf
#define WM8960_3D           0x10
#define WM8960_ALC1         0x11
#define WM8960_ALC2         0x12
#define WM8960_ALC3         0x13
#define WM8960_NOISEG       0x14
#define WM8960_LADC         0x15
#define WM8960_RADC         0x16
#define WM8960_ADDCTL1      0x17
#define WM8960_ADDCTL2      0x18
#define WM8960_POWER1       0x19
#define WM8960_POWER2       0x1a
#define WM8960_ADDCTL3      0x1b
#define WM8960_APOP1        0x1c
#define WM8960_APOP2        0x1d

#define WM8960_LINPATH      0x20
#define WM8960_RINPATH      0x21
#define WM8960_LOUTMIX      0x22

#define WM8960_ROUTMIX      0x25
#define WM8960_MONOMIX1     0x26
#define WM8960_MONOMIX2     0x27
#define WM8960_LOUT2        0x28
#define WM8960_ROUT2        0x29
#define WM8960_MONO         0x2a
#define WM8960_INBMIX1      0x2b
#define WM8960_INBMIX2		0x2c
#define WM8960_BYPASS1		0x2d
#define WM8960_BYPASS2		0x2e
#define WM8960_POWER3		0x2f
#define WM8960_ADDCTL4		0x30
#define WM8960_CLASSD1		0x31

#define WM8960_CLASSD3		0x33
#define WM8960_PLL1		0x34
#define WM8960_PLL2		0x35
#define WM8960_PLL3		0x36
#define WM8960_PLL4		0x37

/* Cache register number */
#define WM8960_CACHEREGNUM   56

/*
 * Field Definitions.
 */

/* */
#define LEFT_INPUT_VOL_LINVOL

/*! @brief WM8960_IFACE1 FORMAT bits */
#define WM8960_IFACE1_FORMAT_MASK        0x03
#define WM8960_IFACE1_FORMAT_SHIFT       0x00
#define WM8960_IFACE1_FORMAT_RJ          0x00
#define WM8960_IFACE1_FORMAT_LJ          0x01
#define WM8960_IFACE1_FORMAT_I2S         0x02
#define WM8960_IFACE1_FORMAT_DSP         0x03
#define WM8960_IFACE1_FORMAT(x)          ((x << WM8960_IFACE1_FORMAT_SHIFT) & WM8960_IFACE1_FORMAT_MASK)
 
/*! @brief WM8960_IFACE1 WL bits */
#define WM8960_IFACE1_WL_MASK            0x0C
#define WM8960_IFACE1_WL_SHIFT           0x02
#define WM8960_IFACE1_WL_16BITS          0x00 
#define WM8960_IFACE1_WL_20BITS          0x01
#define WM8960_IFACE1_WL_24BITS          0x02
#define WM8960_IFACE1_WL_32BITS          0x03
#define WM8960_IFACE1_WL(x)              ((x << WM8960_IFACE1_WL_SHIFT) & WM8960_IFACE1_WL_MASK)

/*! @brief WM8960_IFACE1 LRP bit */
#define WM8960_IFACE1_LRP_MASK           0x10
#define WM8960_IFACE1_LRP_SHIFT          0x04 
#define WM8960_IFACE1_LRCLK_NORMAL_POL   0x00
#define WM8960_IFACE1_LRCLK_INVERT_POL   0x01
#define WM8960_IFACE1_DSP_MODEA          0x00
#define WM8960_IFACE1_DSP_MODEB          0x01
#define WM8960_IFACE1_LRP(x)             ((x << WM8960_IFACE1_LRP_SHIFT) & WM8960_IFACE1_LRP_MASK)

/*! @brief WM8960_IFACE1 DLRSWAP bit */
#define WM8960_IFACE1_DLRSWAP_MASK       0x20 
#define WM8960_IFACE1_DLRSWAP_SHIFT      0x05
#define WM8960_IFACE1_DACCH_NORMAL       0x00
#define WM8960_IFACE1_DACCH_SWAP         0x01
#define WM8960_IFACE1_DLRSWAP(x)         ((x << WM8960_IFACE1_DLRSWAP_SHIFT) & WM8960_IFACE1_DLRSWAP_MASK)

/*! @brief WM8960_IFACE1 MS bit */
#define WM8960_IFACE1_MS_MASK            0x40
#define WM8960_IFACE1_MS_SHIFT           0x06
#define WM8960_IFACE1_SLAVE              0x00
#define WM8960_IFACE1_MASTER             0x01
#define WM8960_IFACE1_MS(x)              ((x << WM8960_IFACE1_MS_SHIFT) & WM8960_IFACE1_MS_MASK)

/*! @brief WM8960_IFACE1 BCLKINV bit */
#define WM8960_IFACE1_BCLKINV_MASK       0x80
#define WM8960_IFACE1_BCLKINV_SHIFT      0x07
#define WM8960_IFACE1_BCLK_NONINVERT     0x00
#define WM8960_IFACE1_BCLK_INVERT        0x01
#define WM8960_IFACE1_BCLKINV(x)         ((x << WM8960_IFACE1_BCLKINV_SHIFT) & WM8960_IFACE1_BCLKINV_MASK)

/*! @brief WM8960_IFACE1 ALRSWAP bit */
#define WM8960_IFACE1_ALRSWAP_MASK       0x100
#define WM8960_IFACE1_ALRSWAP_SHIFT      0x08
#define WM8960_IFACE1_ADCCH_NORMAL       0x00
#define WM8960_IFACE1_ADCCH_SWAP         0x01
#define WM8960_IFACE1_ALRSWAP(x)         ((x << WM8960_IFACE1_ALRSWAP_SHIFT) & WM8960_IFACE1_ALRSWAP_MASK)

/*! @brief WM8960_POWER1 */
#define WM8960_POWER1_VREF_MASK        0x40
#define WM8960_POWER1_VREF_SHIFT       0x06 

#define WM8960_POWER1_AINL_MASK        0x20
#define WM8960_POWER1_AINL_SHIFT       0x05

#define WM8960_POWER1_AINR_MASK        0x10
#define WM8960_POWER1_AINR_SHIFT       0x04

#define WM8960_POWER1_ADCL_MASK        0x08
#define WM8960_POWER1_ADCL_SHIFT       0x03

#define WM8960_POWER1_ADCR_MASK        0x04
#define WM8960_POWER1_ADCR_SHIFT       0x02

/*! @brief WM8960_POWER2 */
#define WM8960_POWER2_DACL_MASK        0x100
#define WM8960_POWER2_DACL_SHIFT       0x08

#define WM8960_POWER2_DACR_MASK        0x80
#define WM8960_POWER2_DACR_SHIFT       0x07

#define WM8960_POWER2_LOUT1_MASK       0x40
#define WM8960_POWER2_LOUT1_SHIFT      0x06

#define WM8960_POWER2_ROUT1_MASK       0x20
#define WM8960_POWER2_ROUT1_SHIFT      0x05

#define WM8960_POWER2_SPKL_MASK        0x10
#define WM8960_POWER2_SPKL_SHIFT       0x04

#define WM8960_POWER2_SPKR_MASK        0x08
#define WM8960_POWER2_SPKR_SHIFT       0x03
 
/*! @brief WM8960 I2C address. */
#define WM8960_I2C_ADDR 0x1A

/*! @brief WM8960 return status. */
typedef enum _WM8960_status
{
    kStatus_WOLFSON_Success = 0x0,
    kStatus_WOLFSON_I2CFail = 0x1,
    kStatus_WOLFSON_Fail = 0x2
} wolfson_status_t;

/*! @brief Modules in WM8960 board. */
typedef enum _WM8960_module
{
    kWolfsonModuleADC = 0x0,
    kWolfsonModuleDAC = 0x1,
    kWolfsonModuleVREF = 0x2,
	kWolfsonModuleHP = 0x03,
    kWolfsonModuleLineIn = 0x6,
    kWolfsonModuleLineOut = 0x7,
	kWolfsonModuleSpeaker = 0x8
} wolfson_module_t;

/*! 
* @brief WM8960 data route.
* Only provide some typical data route, not all route listed.
* Note: Users cannot combine any routes, once a new route is set, the previous one would be replaced.
*/
typedef enum _wolfson_route
{
    kWolfsonRouteBypass = 0x0, /*!< LINEIN->Headphone. */
    kWolfsonRoutePlayback = 0x1, /*!<  I2SIN->DAC->Headphone. */
    kWolfsonRoutePlaybackandRecord = 0x2, /*!< I2SIN->DAC->Headphone, LINEIN->ADC->I2SOUT. */
    kWolfsonRoutePlaybackwithDAP = 0x3, /*!< I2SIN->DAP->DAC->Headphone. */
    kWolfsonRoutePlaybackwithDAPandRecord = 0x4, /*!< I2SIN->DAP->DAC->HP, LINEIN->ADC->I2SOUT. */
    kWolfsonRouteRecord = 0x5/*!< LINEIN->ADC->I2SOUT. */
} wolfson_route_t;

/*! 
* @brief The audio data transfer protocol choice.
* WM8960 only supports I2S format and PCM format.
*/
typedef enum _wolfson_protocol
{
    kWolfsonBusI2S = 0x0,
    kWolfsonBusI2SLeft = 0x1,
    kWolfsonBusI2SRight = 0x2,
    kWolfsonBusPCMA = 0x3,
    kWolfsonBusPCMB = 0x4
} wolfson_protocol_t;

/*! @brief EQ type in DAP, only one EQ can be used at the same time. */
typedef enum _wolfson_EQ_type
{
    kWolfsonEQType7band,
    kWolfsonEQtype5band,
    kWolfsonEQtypeVolumeControl
} wolfson_EQtype_t;

/*! @brief wolfson configure definition. */
typedef struct wolfson_handler
{
    /* I2C relevant definition. */
    uint32_t i2c_instance; /*!< I2C instance. */
    i2c_device_t device; /*!< I2C device setting */
    i2c_master_state_t state; /*!< I2C internal state space. */
} wolfson_handler_t;

/*! @brief Initialize structure of WM8960 */
typedef struct wolfson_init
{
    wolfson_route_t route; /*!< Audio data route.*/
    wolfson_protocol_t bus; /*!< Audio transfer protocol */
    bool master_slave; /*!< Master or slave. */
} wolfson_init_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief WM8960 initialize function.
 *
 * This function would call wolfson_i2c_init(), and in this function, some configurations
 * are fixed. The second parameter is NULL to WM8960 in this version. If users want
 * to change the settings, they have to use wolfson_write_reg() or wolfson_modify_reg()
 * to set the register value of WM8960.
 * Note: If the codec_config is NULL, it would initialize WM8960 using default settings.
 * The default setting:
 * codec_config->route = kWolfsonRoutePlaybackandRecord
 * codec_config->bus = kWolfsonBusI2S
 * codec_config->master = slave
 * @param handler WM8960 handler structure.
 * @param codec_config WM8960 configuration structure.
 */
wolfson_status_t WOLFSON_Init(wolfson_handler_t *handler, wolfson_init_t *codec_config);

/*!
 * @brief Set audio data route in WM8960.
 *
 * This function would set the data route according to route. The route cannot be combined,
 * as all route would enable different modules.
 * Note: If a new route is set, the previous route would not work.
 * @param handler WM8960 handler structure.
 * @param route Audio data route in WM8960.
 */
wolfson_status_t WOLFSON_SetDataRoute(wolfson_handler_t *handler, wolfson_route_t route);

/*! 
 * @brief Set the audio transfer protocol.
 *
 * WM8960 only supports I2S, I2S left, I2S right, PCM A, PCM B format.
 * @param handler WM8960 handler structure.
 * @param bus Audio data transfer protocol.
 */
wolfson_status_t WOLFSON_SetProtocol(wolfson_handler_t *handler, wolfson_protocol_t protocol);

/*!
 * @brief Set WM8960 as master or slave.
 *
 * @param handler WM8960 handler structure.
 * @param master 1 represent master, 0 represent slave.
 */
wolfson_status_t WOLFSON_SetMasterSlave(wolfson_handler_t *handler, bool master);
/*!
 * @brief Configure PLL to get the expected MCLK and frame sync.
 *
 * While sglt5000 as master, PLL should be used.
 * @param handler WM8960 handler structure.
 * @param mclk MCLK frequency expected to get.
 * @param fs Frame sync value expected to get, equals to sample rate.
 */
wolfson_status_t WOLFSON_SetPLL(wolfson_handler_t *handler, uint32_t mclk, uint32_t fs);

/*!
 * @brief Set the volume of different modules in WM8960.
 *
 * This function would set the volume of WM8960 modules. Uses need to appoint the module.
 * The function assume that left channel and right channel has the same volume.
 * @param handler WM8960 handler structure.
 * @param module Module to set volume, it can be ADC, DAC, Headphone and so on.
 * @param volume Volume value need to be set.
 */
wolfson_status_t WOLFSON_SetVolume(wolfson_handler_t *handler, wolfson_module_t module, uint32_t volume);

/*!
 * @brief Get the volume of different modules in WM8960.
 *
 * This function gets the volume of WM8960 modules. Uses need to appoint the module.
 * The function assume that left channel and right channel has the same volume.
 * @param handler WM8960 handler structure.
 * @param module Module to set volume, it can be ADC, DAC, Headphone and so on.
 * @return Volume value of the module.
 */
uint32_t WOLFSON_GetVolume(wolfson_handler_t *handler, wolfson_module_t module);

/*!
 * @brief Mute modules in WM8960.
 *
 * @param handler WM8960 handler structure.
 * @param module Modules need to be mute.
 * @param isEnabled Mute or unmute, 1 represent mute.
 */
wolfson_status_t WOLFSON_SetMuteCmd(wolfson_handler_t *handler, wolfson_module_t module, bool isEnabled);

/*!
 * @brief Enable/disable expected devices.
 * @param handler WM8960 handler structure.
 * @param module Module expected to enable.
 * @param isEnabled Enable or disable moudles.
 */
wolfson_status_t WOLFSON_SetModuleCmd(wolfson_handler_t *handler, wolfson_module_t module, bool isEnabled);

/*!
 * @brief Initialize the I2C module in wolfson.
 *
 * Wolfson now uses i2c to write/read the registers in it.
 * @param handler WM8960 handler structure.
 */
wolfson_status_t WOLFSON_I2CInit(wolfson_handler_t *handler);

/*!
 * @brief Deinit the WM8960 codec. Mainly used to close the I2C controller.
 * @param handler WM8960 handler structure pointer.
 */
wolfson_status_t WOLFSON_Deinit(wolfson_handler_t *handler);

/*!
 * @brief Configure the data format of audio data.
 *
 * This function would configure the registers about the sample rate, bit depths.
 * @param handler WM8960 handler structure pointer.
 * @param mclk Master clock frequency of I2S.
 * @param sample_rate Sample rate of audio file running in WM8960. WM8960 now
 * supports 8k, 11.025k, 12k, 16k, 22.05k, 24k, 32k, 44.1k, 48k and 96k sample rate.
 * @param bits Bit depth of audio file (WM8960 only supports 16bit, 20bit, 24bit
 * and 32 bit in HW).
 */
wolfson_status_t WOLFSON_ConfigDataFormat(wolfson_handler_t *handler, uint32_t mclk, uint32_t sample_rate, uint8_t bits);
 
/*!
 * @brief Write register to wolfson using I2C.
 * @param handler WM8960 handler structure.
 * @param reg The register address in wolfson.
 * @param val Value needs to write into the register.
 */
wolfson_status_t WOLFSON_WriteReg(wolfson_handler_t *handler, uint8_t reg, uint16_t val);

/*!
 * @brief Read register from wolfson using I2C.
 * @param handler WM8960 handler structure.
 * @param reg The register address in wolfson.
 * @param val Value written to.
 */
wolfson_status_t WOLFSON_ReadReg(uint8_t reg, uint16_t *val);

/*!
 * @brief Modify some bits in the register using I2C.
 * @param handler WM8960 handler structure.
 * @param reg The register address in wolfson.
 * @param mask The mask code for the bits want to write. The bit you want to write should be 0.
 * @param val Value needs to write into the register.
 */
wolfson_status_t WOLFSON_ModifyReg(wolfson_handler_t * handler, uint8_t reg,  uint16_t mask, uint16_t val);

/*!
 * @brief Set the volume of DAC modules in WM8960.
 *
 * This function would set the volume of WM8960 DAC.
 * The function assume that left channel and right channel has the same volume.
 * @param handler WM8960 handler structure.
 * @param volume Volume value need to be set.
 */
static inline wolfson_status_t WOLFSON_SetDACVoulme(wolfson_handler_t * handler, uint32_t volume) 
{
    return WOLFSON_SetVolume(handler, kWolfsonModuleDAC, volume);
}

/*!
 * @brief Get the volume of DAC modules in WM8960.
 *
 * This function gets the volume of WM8960 modules.
 * The function assume that left channel and right channel has the same volume.
 * @param handler WM8960 handler structure.
 * @return Volume value of the module.
 */
static inline uint32_t WOLFSON_GetDACVolume(wolfson_handler_t * handler) 
{
    return WOLFSON_GetVolume(handler,kWolfsonModuleDAC);
}

/*!
 * @brief Mute DAC in WM8960.
 *
 * @param handler WM8960 handler structure.
 * @param isEnabled Mute or unmute, true represent mute.
 */
static inline wolfson_status_t WOLFSON_SetDACMute(wolfson_handler_t * handler, bool enable)   
{
    return WOLFSON_SetMuteCmd(handler,kWolfsonModuleDAC, enable);
}

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif/* __FSL_WM8960_DRIVER_H__ */

/*******************************************************************************
 * API
 ******************************************************************************/

