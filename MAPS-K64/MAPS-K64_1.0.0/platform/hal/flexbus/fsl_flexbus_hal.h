/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
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
#ifndef __FSL_FLEXBUS_HAL_H__
#define __FSL_FLEXBUS_HAL_H__

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include "fsl_device_registers.h"

/*!
 * @addtogroup flexbus_hal
 * @{
 */

/*! @file*/

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Flexbus status return codes.*/
typedef enum _flexbus_status
{
    kStatus_FlexbusSuccess = 0,
    kStatus_FlexbusOutOfRange,
    kStatus_FlexbusInvalidArgument,
    kStatus_FlexbusFail,
} flexbus_status_t;

/*! @brief Defines port size for Flexbus peripheral.*/
typedef enum _flexbus_port_size
{
    kFlexbus_4bytes  = 0x00, /* 32-bit port size. */
    kFlexbus_1byte   = 0x01, /* 8-bit port size. */
    kFlexbus_2bytes  = 0x02  /* 16-bit port size. */
} flexbus_port_size_t;

/*! @brief Defines number of cycles to hold address and attributes for Flexbus peripheral.*/
typedef enum _flexbus_write_address_hold
{
    kFlexbus_hold1cycle  = 0x00, /* Hold address and attributes one cycle after FB_CSn negates on writes. */
    kFlexbus_hold2cycles = 0x01, /* Hold address and attributes two cycle after FB_CSn negates on writes. */
    kFlexbus_hold3cycles = 0x02, /* Hold address and attributes three cycle after FB_CSn negates on writes. */
    kFlexbus_hold4cycles = 0x03  /* Hold address and attributes four cycle after FB_CSn negates on writes. */
} flexbus_write_address_hold_t;


/*! @brief Defines number of cycles to hold address and attributes for Flexbus peripheral.*/
typedef enum _flexbus_read_address_hold
{
    kFlexbus_hold4or3cycles = 0x03,     /* Depends on FS_AA register */
    kFlexbus_hold3or2cycles = 0x02,
    kFlexbus_hold2or1cycle  = 0x01,
    kFlexbus_hold1or0cycle  = 0x00
} flexbus_read_address_hold_t;


/*! @brief Address setup for Flexbus peripheral.*/
typedef enum _flexbus_address_setup
{
    kFlexbus_firstRisingEdge  = 0x00, /* Assert FB_CSn on first rising clock edge after address is asserted. */
    kFlexbus_secondRisingEdge = 0x01, /* Assert FB_CSn on second rising clock edge after address is asserted. */
    kFlexbus_thirdRisingEdge  = 0x02, /* Assert FB_CSn on third rising clock edge after address is asserted. */
    kFlexbus_fourthRisingEdge = 0x03, /* Assert FB_CSn on fourth rising clock edge after address is asserted. */
} flexbus_address_setup_t;

/*! @brief Defines byte-lane shift for Flexbus peripheral.*/
typedef enum _flexbus_bytelane_shift
{
    kFlexbus_notShifted = 0x00, /* Not shifted. Data is left-justfied on FB_AD. */
    kFlexbus_shifted    = 0x01, /* Shifted. Data is right justified on FB_AD. */
} flexbus_bytelane_shift_t;

/*! @brief Defines multiplex group1 valid signals.*/
typedef enum _flexbus_multiplex_group1_signal
{
    kFlexbus_multiplex_group1_FB_ALE = 0x0,
    kFlexbus_multiplex_group1_FB_CS1 = 0x1,
    kFlexbus_multiplex_group1_FB_TS  = 0x2,
} flexbus_multiplex_group1_t;

/*! @brief Defines multiplex group2 valid signals.*/
typedef enum _flexbus_multiplex_group2_signal
{
    kFlexbus_multiplex_group2_FB_CS4      = 0x0,
    kFlexbus_multiplex_group2_FB_TSIZ0    = 0x1,
    kFlexbus_multiplex_group2_FB_BE_31_24 = 0x2,
} flexbus_multiplex_group2_t;

/*! @brief Defines multiplex group3 valid signals.*/
typedef enum _flexbus_multiplex_group3_signal
{
    kFlexbus_multiplex_group3_FB_CS5      = 0x0,
    kFlexbus_multiplex_group3_FB_TSIZ1    = 0x1,
    kFlexbus_multiplex_group3_FB_BE_23_16 = 0x2,
} flexbus_multiplex_group3_t;

/*! @brief Defines multiplex group4 valid signals.*/
typedef enum _flexbus_multiplex_group4_signal
{
    kFlexbus_multiplex_group4_FB_TBST    = 0x0,
    kFlexbus_multiplex_group4_FB_CS2     = 0x1,
    kFlexbus_multiplex_group4_FB_BE_15_8 = 0x2,
} flexbus_multiplex_group4_t;

/*! @brief Defines multiplex group5 valid signals.*/
typedef enum _flexbus_multiplex_group5_signal
{
    kFlexbus_multiplex_group5_FB_TA     = 0x0,
    kFlexbus_multiplex_group5_FB_CS3    = 0x1,
    kFlexbus_multiplex_group5_FB_BE_7_0 = 0x2,
} flexbus_multiplex_group5_t;


/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Configuration
 * @{
 */

/*!
 * @brief Initialization to known values.
 *
 * Only chip 0 validated and set to known values. Other chips disabled.
 *
 * @param   baseAddr Flexbus module baseAddr number.
*/
static inline void FB_HAL_Init(uint32_t baseAddr);

/*!
 * @brief Write chip-select base address.
 *
 * The CSARn registers specify the chip-select base addresses.
 * NOTE: Because the FlexBus module is one of the slaves connected to the crossbar switch, it is only
 * accessible within a certain memory range. Refer to the device memory map for the applicable
 * FlexBus "expansion" address range for which the chip-selects can be active. Set the CSARn
 * registers appropriately.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 * @param   addr chip-select base address.
*/
static inline void FB_HAL_WriteBaseAddr(uint32_t baseAddr, uint8_t chip, uint16_t addr)
{
    assert(chip < HW_FB_CSARn_COUNT);
    BW_FB_CSARn_BA(baseAddr, chip, addr);
}

/*!
 * @brief Read chip-select base address.
 *
 * The CSARn registers specify the chip-select base addresses.
 * NOTE: Because the FlexBus module is one of the slaves connected to the crossbar switch, it is only
 * accessible within a certain memory range. Refer to the device memory map for the applicable
 * FlexBus "expansion" address range for which the chip-selects can be active. Set the CSARn
 * registers appropriately.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 * @return  Chip-select base address
*/
static inline uint16_t FB_HAL_ReadBaseAddr(uint32_t baseAddr, uint8_t chip)
{
    assert(chip < HW_FB_CSARn_COUNT);
    return (uint16_t)BR_FB_CSARn_BA(baseAddr, chip);
}

/*!
 * @brief Sets chip-selects valid bit or not.
 *
 * Indicates whether the corresponding CSAR, CSMR, and CSCR contents are valid.
 * NOTE: At reset, no chip-select other than FB_CS0 can be used until the CSMR0[V]
 * is set. Afterward, FB_CS[5:0] functions as programmed.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 * @param   valid Validation for chip-selects or not.
 *          - true: chip-select is valid
 *          - false: chip-select is invalid
*/
static inline void FB_HAL_SetChipSelectValidCmd(uint32_t baseAddr, uint8_t chip, bool valid)
{
    assert(chip < HW_FB_CSARn_COUNT);
    BW_FB_CSMRn_V(baseAddr, chip, valid);
}

/*!
 * @brief Reads chip validation bit.
 *
 * If validation chip is set, all selected chips are validated.
 * Indicates whether the corresponding CSAR, CSMR, and CSCR contents are valid.
 * NOTE: At reset, no chip-select other than FB_CS0 can be used until the CSMR0[V]
 * is set. Afterward, FB_CS[5:0] functions as programmed.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 *
 * @return chip validation value.
 *         0: Chip-select not validated
 *         1: chip-select validated
*/
static inline bool FB_HAL_IsChipSelectValid(uint32_t baseAddr, uint8_t chip)
{
    assert(chip < HW_FB_CSARn_COUNT);
    return (bool)BR_FB_CSMRn_V(baseAddr, chip);
}

/*!
 * @brief Enables or disables write protection function for Flexbus.
 *
 * Controls write accesses to the address range in the corresponding CSAR.
 * 0: Read and write accesses are allowed
 * 1: Only read accesses are allowed
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 * @param   enable Enables or disables write protection.
*/
static inline void FB_HAL_SetWriteProtectionCmd(uint32_t baseAddr, uint8_t chip, bool enable)
{
    assert(chip < HW_FB_CSARn_COUNT);
    BW_FB_CSMRn_WP(baseAddr, chip, enable);
}

/*!
 * @brief Reads write protection function bit on Flexbus.
 *
 * Controls write accesses to the address range in the corresponding CSAR.
 * 0: Read and write accesses are allowed
 * 1: Only read accesses are allowed
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 *
 * @return Write protection bits value.
 *         0: Disabled
 *         1: Enabled
*/
static inline bool FB_HAL_IsWriteProtectionEnabled(uint32_t baseAddr, uint8_t chip)
{
    assert(chip < HW_FB_CSARn_COUNT);
    return (bool)BR_FB_CSMRn_WP(baseAddr, chip);
}

/*!
 * @brief Writes base address mask (BAM) for Flexbus.
 *
 * Defines the chip-select block size by masking address bits. Setting a BAM bit
 * causes the corresponding CSAR bit to be a don't care in the decode.
 *
 * 0: Read and write accesses are allowed
 * 1: Only read accesses are allowed
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 * @param   address Base Address mask for.
*/
static inline void FB_HAL_WriteBaseAddrMask(uint32_t baseAddr, uint8_t chip, uint16_t address)
{
    assert(chip < HW_FB_CSARn_COUNT);
    BW_FB_CSMRn_BAM(baseAddr, chip, address);
}

/*!
 * @brief Reads base address mask (BAM) for Flexbus.
 *
 * Defines the chip-select block size by masking address bits. Setting a BAM bit
 * causes the corresponding CSAR bit to be a don't care in the decode.
 *
 * 0: Read and write accesses are allowed
 * 1: Only read accesses are allowed
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 *
 * @return base address mask.
*/
static inline uint16_t FB_HAL_ReadBaseAddrMask(uint32_t baseAddr, uint8_t chip)
{
    assert(chip < HW_FB_CSARn_COUNT);
    return (uint16_t)BR_FB_CSMRn_BAM(baseAddr, chip);
}

/*!
 * @brief Enables or disables burst-write on Flexbus.
 *
 * Specifies whether burst writes are used for memory associated with each FB_CSn.
 *
 * 0: Break data larger than the specified port size into individual, port-sized,
 *    non-burst writes. For example, a longword write to an 8-bit port takes four
 *    byte writes.
 * 1: Enables burst write of data larger than the specified port size, including
 *    longword writes to 8 and 16-bit ports, word writes to 8-bit ports, and line
 *    writes to 8-, 16-, and 32-bit ports.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 * @param   enable Enables or disables burst-write.
*/
static inline void FB_HAL_SetBurstWriteCmd(uint32_t baseAddr, uint8_t chip, bool enable)
{
    assert(chip < HW_FB_CSCRn_COUNT);
    BW_FB_CSCRn_BSTW(baseAddr, chip, enable);
}

/*!
 * @brief Reads burst-write bit on Flexbus.
 *
 * Specifies whether burst writes are used for memory associated with each FB_CSn.
 *
 * 0: Break data larger than the specified port size into individual, port-sized,
 *    non-burst writes. For example, a longword write to an 8-bit port takes four
 *    byte writes.
 * 1: Enables burst write of data larger than the specified port size, including
 *    longword writes to 8 and 16-bit ports, word writes to 8-bit ports, and line
 *    writes to 8-, 16-, and 32-bit ports.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 *
 * @return Value of burst-write function bit
 *         0: Disabled
 *         1: Enabled
*/
static inline bool FB_HAL_IsBurstWriteEnabled(uint32_t baseAddr, uint8_t chip)
{
    assert(chip < HW_FB_CSARn_COUNT);
    return (bool)BR_FB_CSCRn_BSTW(baseAddr, chip);
}

/*!
 * @brief Enables or disables burst-read bit on Flexbus.
 *
 * Specifies whether burst reads are used for memory associated with each FB_CSn.
 *
 * 0: Data exceeding the specified port size is broken into individual, port-sized,
 *    non-burst reads. For example, a longword read from an 8-bit port is broken into
 *    four 8-bit reads.
 * 1: Enables data burst reads larger than the specified port size, including longword
 *    reads from 8- and 16-bit ports, word reads from 8-bit ports, and line reads from 8,
 *    16-, and 32-bit ports.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 * @param   enable Enables or disables burst-read.
*/
static inline void FB_HAL_SetBurstReadCmd(uint32_t baseAddr, uint8_t chip, bool enable)
{
    assert(chip < HW_FB_CSCRn_COUNT);
    BW_FB_CSCRn_BSTR(baseAddr, chip, enable);
}

/*!
 * @brief Reads burst-read bit on Flexbus.
 *
 * Specifies whether burst reads are used for memory associated with each FB_CSn.
 *
 * 0: Data exceeding the specified port size is broken into individual, port-sized,
 *    non-burst reads. For example, a longword read from an 8-bit port is broken into
 *    four 8-bit reads.
 * 1: Enables data burst reads larger than the specified port size, including longword
 *    reads from 8- and 16-bit ports, word reads from 8-bit ports, and line reads from 8,
 *    16-, and 32-bit ports.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 *
 * @return Value of burst-write function bit.
 *         0: Disabled
 *         1: Enabled
*/
static inline bool FB_HAL_IsBurstReadEnabled(uint32_t baseAddr, uint8_t chip)
{
    assert(chip < HW_FB_CSARn_COUNT);
    return (bool)BR_FB_CSCRn_BSTR(baseAddr, chip);
}

/*!
 * @brief Enables or disables byte-enable support on Flexbus.
 *
 * Specifies the byte enable operation. Certain memories have byte enables that must
 * be asserted during reads and writes. BEM can be set in the relevant CSCR to provide
 * the appropriate mode of byte enable support for these SRAMs.
 *
 * The FB_BEn signals are asserted for read and write accesses.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 * @param   enable Enables or disables byte-enable support
*/
static inline void FB_HAL_SetByteModeCmd(uint32_t baseAddr, uint8_t chip, bool enable)
{
    assert(chip < HW_FB_CSARn_COUNT);
    BW_FB_CSCRn_BEM(baseAddr, chip, enable);
}

/*!
 * @brief Reads byte-enabled mode bit on Flexbus.
 *
 * Specifies the byte enable operation. Certain memories have byte enables that must
 * be asserted during reads and writes. BEM can be set in the relevant CSCR to provide
 * the appropriate mode of byte enable support for these SRAMs.
 *
 * 0: The FB_BEn signals are not asserted for reads. The FB_BEn signals are asserted
 *    for data write only.
 * 1: The FB_BEn signals are asserted for read and write accesses.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 *
 * @return Value of byte enabled mode bit on Flexbus.
*/
static inline bool FB_HAL_IsByteEnableModeEnabled(uint32_t baseAddr, uint8_t chip)
{
    assert(chip < HW_FB_CSARn_COUNT);
    return (bool)BR_FB_CSCRn_BEM(baseAddr, chip);
}

/*!
 * @brief Sets port size on Flexbus.
 *
 * Specifies the data port width associated with each chip-select. It determines where
 * data is driven during write cycles and where data is sampled during read cycles.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 * @param   size Size of port.
*/
static inline void FB_HAL_SetPortSize(uint32_t baseAddr, uint8_t chip, flexbus_port_size_t size)
{
    assert(chip < HW_FB_CSARn_COUNT);
    BW_FB_CSCRn_PS(baseAddr, chip, size);
}

/*!
 * @brief Gets port size on Flexbus.
 *
 * Specifies the data port width associated with each chip-select. It determines where
 * data is driven during write cycles and where data is sampled during read cycles.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 *
 * @return  Size of port.
*/
static inline flexbus_port_size_t FB_HAL_GetPortSize(uint32_t baseAddr, uint8_t chip)
{
    return (flexbus_port_size_t)BR_FB_CSCRn_PS(baseAddr, chip);
}

/*!
 * @brief Enables auto-acknowledge on Flexbus.
 *
 * Determines the assertion of the internal transfer acknowledge for accesses specified by the
 * chip-select address.
 *
 * NOTE: If AA is set for a corresponding FB_CSn and the external system asserts an external FB_TA
 * before the wait-state countdown asserts the internal FB_TA, the cycle is terminated. Burst cycles
 * increment the address bus between each internal termination.
 * NOTE: This bit must be set if CSPMCR disables FB_TA.
 *
 * enable value:
 * 0: No internal FB_TA is asserted. Cycle is terminated externally
 * 1: Internal transfer acknowledge is asserted as specified by WS
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 * @param   enable Enables or disables Auto-acknowledge.
*/
static inline void FB_HAL_SetAutoAcknowledgeCmd(uint32_t baseAddr, uint8_t chip, bool enable)
{
    assert(chip < HW_FB_CSARn_COUNT);
    BW_FB_CSCRn_AA(baseAddr, chip, enable);
}


/*!
 * @brief Enables auto-acknowledge on Flexbus.
 *
 * Determines the assertion of the internal transfer acknowledge for accesses specified by the
 * chip-select address.
 *
 * NOTE: If AA is set for a corresponding FB_CSn and the external system asserts an external FB_TA
 * before the wait-state countdown asserts the internal FB_TA, the cycle is terminated. Burst cycles
 * increment the address bus between each internal termination.
 * NOTE: This bit must be set if CSPMCR disables FB_TA.
 *
 * enable value:
 * 0: No internal FB_TA is asserted. Cycle is terminated externally
 * 1: Internal transfer acknowledge is asserted as specified by WS
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 *
 * @return  Enable or disable Auto-acknowledge.
*/
static inline bool FB_HAL_IsAutoAcknowledgeEnabled(uint32_t baseAddr, uint8_t chip)
{
    return (bool)BR_FB_CSCRn_AA(baseAddr, chip);
}

/*!
 * @brief Enables byte-lane shift on Flexbus.
 *
 * Determines if data on FB_AD appears left-justified or right-justified during the data phase
 * of a FlexBus access.
 *
 * 0: Not shifted. Data is left-justfied on FB_AD.
 * 1: Shifted. Data is right justified on FB_AD.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 * @param   shift Selects left-justified or right-justified data
*/
static inline void FB_HAL_SetByteLaneShift(uint32_t baseAddr, uint8_t chip, flexbus_bytelane_shift_t shift)
{
    assert(chip < HW_FB_CSARn_COUNT);
    BW_FB_CSCRn_BLS(baseAddr, chip, shift);
}

/*!
 * @brief Enables byte-lane shift on Flexbus.
 *
 * Determines if data on FB_AD appears left-justified or right-justified during the data phase
 * of a FlexBus access.
 *
 * 0: Not shifted. Data is left-justfied on FB_AD.
 * 1: Shifted. Data is right justified on FB_AD.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 *
 * @return  shift Selects left-justified or right-justified data
*/
static inline flexbus_bytelane_shift_t FB_HAL_GetByteLaneShift(uint32_t baseAddr, uint8_t chip)
{
    return (flexbus_bytelane_shift_t)BR_FB_CSCRn_BLS(baseAddr, chip);
}

/*!
 * @brief Sets number of wait states on Flexbus.
 *
 * The number of wait states inserted after FB_CSn asserts and before an internal transfer
 * acknowledge is generated (WS = 0 inserts zero wait states, WS = 0x3F inserts 63 wait states).
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 * @param   waitStates Defines value of wait states
*/
static inline void FB_HAL_SetWaitStates(uint32_t baseAddr, uint8_t chip, uint8_t waitStates)
{
    assert(chip < HW_FB_CSARn_COUNT);
    assert(waitStates <= 0x3F);
    BW_FB_CSCRn_WS(baseAddr, chip, waitStates);
}

/*!
 * @brief Gets number of wait states on Flexbus.
 *
 * The number of wait states inserted after FB_CSn asserts and before an internal transfer
 * acknowledge is generated (WS = 0 inserts zero wait states, WS = 0x3F inserts 63 wait states).
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 *
 * @return  Value of wait states
*/
static inline uint8_t FB_HAL_GetWaitStates(uint32_t baseAddr, uint8_t chip)
{
    assert(chip < HW_FB_CSARn_COUNT);
    return (uint8_t)BR_FB_CSCRn_WS(baseAddr, chip);
}

/*!
 * @brief Sets write address hold or deselect.
 *
 * Write address hold or deselect. This field controls the address, data, and attribute hold time
 * after the termination of a write cycle that hits in the chip-select address space.
 * NOTE: The hold time applies only at the end of a transfer. Therefore, during a burst transfer
 * or a transfer to a port size smaller than the transfer size, the hold time is only added after
 * the last bus cycle.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 * @param   addrHold Value of cycles to hold write address.
*/
static inline void FB_HAL_SetWriteAddrHoldOrDeselect(uint32_t baseAddr, uint8_t chip, flexbus_write_address_hold_t addrHold)
{
    assert(chip < HW_FB_CSARn_COUNT);
    BW_FB_CSCRn_WRAH(baseAddr, chip, addrHold);
}

/*!
 * @brief Gets write address hold or deselect.
 *
 * Write address hold or deselect. This field controls the address, data, and attribute hold time
 * after the termination of a write cycle that hits in the chip-select address space.
 * NOTE: The hold time applies only at the end of a transfer. Therefore, during a burst transfer
 * or a transfer to a port size smaller than the transfer size, the hold time is only added after
 * the last bus cycle.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 *
 * @return  Value of cycles to hold write address.
*/
static inline flexbus_write_address_hold_t FB_HAL_GetWriteAddrHoldOrDeselect(uint32_t baseAddr, uint8_t chip)
{
    assert(chip < HW_FB_CSARn_COUNT);
    return (flexbus_write_address_hold_t)BR_FB_CSCRn_WRAH(baseAddr, chip);
}

/*!
 * @brief Sets read address hold or deselect.
 *
 * This field controls the address and attribute hold time after the termination during a read cycle
 * that hits in the chip-select address space.
 * NOTE: The hold time applies only at the end of a transfer. Therefore, during a burst transfer
 * or a transfer to a port size smaller than the transfer size, the hold time is only added after
 * the last bus cycle.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 * @param   addrHold Value of cycles to hold read address.
*/
static inline void FB_HAL_SetReadAddrHoldOrDeselect(uint32_t baseAddr, uint8_t chip, flexbus_read_address_hold_t addrHold)
{
    assert(chip < HW_FB_CSARn_COUNT);
    BW_FB_CSCRn_RDAH(baseAddr, chip, addrHold);
}

/*!
 * @brief Gets read address hold or deselect.
 *
 * This field controls the address and attribute hold time after the termination during a read cycle
 * that hits in the chip-select address space.
 * NOTE: The hold time applies only at the end of a transfer. Therefore, during a burst transfer
 * or a transfer to a port size smaller than the transfer size, the hold time is only added after
 * the last bus cycle.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 *
 * @return  Value of cycles to hold read address.
*/
static inline flexbus_read_address_hold_t FB_HAL_GetReadAddrHoldOrDeselect(uint32_t baseAddr, uint8_t chip)
{
    assert(chip < HW_FB_CSARn_COUNT);
    return (flexbus_read_address_hold_t)(BR_FB_CSCRn_RDAH(baseAddr, chip));
}

/*!
 * @brief Set address setup
 *
 * Controls the assertion of the chip-select with respect to assertion of a valid address and
 * attributes. The address and attributes are considered valid at the same time FB_TS/FB_ALE asserts.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 * @param   delay Value of delay.
*/
static inline void FB_HAL_SetAddrSetup(uint32_t baseAddr, uint8_t chip, flexbus_address_setup_t delay)
{
    assert(chip < HW_FB_CSARn_COUNT);
    BW_FB_CSCRn_ASET(baseAddr, chip, delay);
}

/*!
 * @brief Get address setup
 *
 * Controls the assertion of the chip-select with respect to assertion of a valid address and
 * attributes. The address and attributes are considered valid at the same time FB_TS/FB_ALE asserts.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 *
 * @return  Value of delay.
*/
static inline flexbus_address_setup_t FB_HAL_GetAddrSetup(uint32_t baseAddr, uint8_t chip)
{
    return (flexbus_address_setup_t)BR_FB_CSCRn_ASET(baseAddr, chip);
}

/*!
 * @brief Enables extended address latch.
 *
 * Extended address latch enable
 *
 * 0: FB_TS/FB_ALE asserts for one bus clock cycle.
 * 1: FB_TS/FB_ALE remains asserted until the first positive clock edge after FB_CSn asserts.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 * @param   enable Enables or disables extended address latch.
*/
static inline void FB_HAL_SetExtendedAddrLatchCmd(uint32_t baseAddr, uint8_t chip, bool enable)
{
    assert(chip < HW_FB_CSARn_COUNT);
    BW_FB_CSCRn_EXTS(baseAddr, chip, enable);
}

/*!
 * @brief Reads extended address latch bit.
 *
 * Extended address latch enable
 *
 * 0: FB_TS/FB_ALE asserts for one bus clock cycle.
 * 1: FB_TS/FB_ALE remains asserted until the first positive clock edge after FB_CSn asserts.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 *
 * @return  Enabled or disabled state od latch.
*/
static inline bool FB_HAL_IsExtendedAddrLatchEnabled(uint32_t baseAddr, uint8_t chip)
{
    assert(chip < HW_FB_CSARn_COUNT);
    return (bool)BR_FB_CSCRn_EXTS(baseAddr, chip);
}

/*!
 * @brief Enables secondary wait state.
 *
 * Secondary wait state enable.
 *
 * 0: The WS value inserts wait states before an internal transfer acknowledge is generated
 *    for all transfers.
 * 1: The SWS value inserts wait states before an internal transfer acknowledge is generated
 *    for burst transfer secondary terminations.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 * @param   enable Enables or disables wait state
*/
static inline void FB_HAL_SetSecondaryWaitStateCmd(uint32_t baseAddr, uint8_t chip, bool enable)
{
    assert(chip < HW_FB_CSARn_COUNT);
    BW_FB_CSCRn_SWSEN(baseAddr, chip, enable);
}

/*!
 * @brief Reads secondary wait state.
 *
 * Secondary wait state enable check.
 *
 * 0: The WS value inserts wait states before an internal transfer acknowledge is generated
 *    for all transfers.
 * 1: The SWS value inserts wait states before an internal transfer acknowledge is generated
 *    for burst transfer secondary terminations.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   chip Flexbus chip for validation.
 *
 * @return  Enabled or disabled state of wait state.
 *
*/
static inline bool FB_HAL_IsSecondaryWaitStateEnabled(uint32_t baseAddr, uint8_t chip)
{
    assert(chip < HW_FB_CSARn_COUNT);
    return (bool)BR_FB_CSCRn_SWSEN(baseAddr, chip);
}

/*!
 * @brief Multiplex group1 set
 *
 * GROUP1 Controls the multiplexing of the FB_ALE, FB_CS1 , and FB_TS signals.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   controls Flexbus multiplex settings for Group1.
 *
 * @return  Flexbus status.
*/
static inline void FB_HAL_SetMultiplexControlGroup1(uint32_t baseAddr, flexbus_multiplex_group1_t controls)
{
    BW_FB_CSPMCR_GROUP1(baseAddr, controls);
}

/*!
 * @brief Multiplex group1 get
 *
 * GROUP1 Controls the multiplexing of the FB_ALE, FB_CS1 , and FB_TS signals.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 *
 * @return  Flexbus multiplex settings for Group1.
*/
static inline flexbus_multiplex_group1_t FB_HAL_GetMultiplexControlGroup1(uint32_t baseAddr)
{
    return (flexbus_multiplex_group1_t)BR_FB_CSPMCR_GROUP1(baseAddr);
}

/*!
 * @brief Multiplex group2 set
 *
 * GROUP2 Controls the multiplexing of the FB_TA , FB_CS3 , and FB_BE_7_0 signals. When
 *        GROUP5 is not 0000b, you must write 1b to the CSCR[AA] bit. Otherwise, the
 *        bus hangs during a transfer.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   controls Flexbus multiplex settings for Group2.
 *
 * @return  Flexbus status.
 *
*/
static inline void FB_HAL_SetMultiplexControlGroup2(uint32_t baseAddr, flexbus_multiplex_group2_t controls)
{
    BW_FB_CSPMCR_GROUP2(baseAddr, controls);
}

/*!
 * @brief Multiplex group2 get
 *
 * GROUP2 Controls the multiplexing of the FB_TA , FB_CS3 , and FB_BE_7_0 signals. When
 *        GROUP5 is not 0000b, you must write 1b to the CSCR[AA] bit. Otherwise, the
 *        bus hangs during a transfer.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 *
 * @return  Flexbus multiplex settings for Group2.
*/
static inline flexbus_multiplex_group2_t FB_HAL_GetMultiplexControlGroup2(uint32_t baseAddr)
{
    return (flexbus_multiplex_group2_t)BR_FB_CSPMCR_GROUP2(baseAddr);
}

/*!
 * @brief Multiplex group3 set
 *
 * GROUP3 Controls the multiplexing of the FB_CS4 , FB_TSIZ0, and FB_BE_31_24 signals.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   controls Flexbus multiplex settings for Group3.
 *
 * @return  Flexbus status.
 *
*/
static inline void FB_HAL_SetMultiplexControlGroup3(uint32_t baseAddr, flexbus_multiplex_group3_t controls)
{
    BW_FB_CSPMCR_GROUP3(baseAddr, controls);
}

/*!
 * @brief Multiplex group3 get
 *
 * GROUP3 Controls the multiplexing of the FB_CS4 , FB_TSIZ0, and FB_BE_31_24 signals.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 *
 * @return  Flexbus multiplex settings for Group3.
 *
*/
static inline flexbus_multiplex_group3_t FB_HAL_GetMultiplexControlGroup3(uint32_t baseAddr)
{
    return (flexbus_multiplex_group3_t)BR_FB_CSPMCR_GROUP3(baseAddr);
}

/*!
 * @brief Multiplex group4 set
 *
 * GROUP4 Controls the multiplexing of the FB_TBST, FB_CS2, and FB_BE_15_8 signals.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   controls Flexbus multiplex settings for Group4.
 *
 * @return  Flexbus status.
 *
*/
static inline void FB_HAL_SetMultiplexControlGroup4(uint32_t baseAddr, flexbus_multiplex_group4_t controls)
{
    BW_FB_CSPMCR_GROUP4(baseAddr, controls);
}

/*!
 * @brief Multiplex group4 get
 *
 * GROUP4 Controls the multiplexing of the FB_TBST, FB_CS2, and FB_BE_15_8 signals.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 *
 * @return  Flexbus multiplex settings for Group4.
 *
*/
static inline flexbus_multiplex_group4_t FB_HAL_GetMultiplexControlGroup4(uint32_t baseAddr)
{
    return (flexbus_multiplex_group4_t)BR_FB_CSPMCR_GROUP4(baseAddr);
}

/*!
 * @brief Multiplex group5 set
 *
 * GROUP5 Controls the multiplexing of the FB_TA, FB_CS3, and FB_BE_7_0 signals.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 * @param   controls Flexbus multiplex settings for Group5.
 *
 * @return  Flexbus status.
 *
*/
static inline void FB_HAL_SetMultiplexControlGroup5(uint32_t baseAddr, flexbus_multiplex_group5_t controls)
{
    BW_FB_CSPMCR_GROUP5(baseAddr, controls);
}

/*!
 * @brief Multiplex group5 get
 *
 * GROUP5 Controls the multiplexing of the FB_TA, FB_CS3, and FB_BE_7_0 signals.
 *
 * @param   baseAddr Flexbus module baseAddr number.
 *
 * @return  Flexbus multiplex settings for Group5.
 *
*/
static inline flexbus_multiplex_group5_t FB_HAL_GetMultiplexControlGroup5(uint32_t baseAddr)
{
    return (flexbus_multiplex_group5_t)BR_FB_CSPMCR_GROUP5(baseAddr);
}

/*! @}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* __FSL_FLEXCAN_HAL_H__*/

/*******************************************************************************
 * EOF
 ******************************************************************************/

