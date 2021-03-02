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
#include "fsl_flexbus_hal.h"

/*******************************************************************************
 * Code
 ******************************************************************************/
/*FUNCTION**********************************************************************
 *
 * Function Name : FB_HAL_Init
 * Description   : This function initializes the module to a known state.
 *
 *END**************************************************************************/
static inline void FB_HAL_Init(uint32_t baseAddr)
{
    for (int chip = 1; chip < HW_FB_CSARn_COUNT; chip++)
    {
        /* set all chips not valid (disabled), only chip 0 valid */
        BW_FB_CSMRn_V(baseAddr, chip, false);
    }
    /* CHIP 0 setting */

    /* FB_CSAR0 register set */
    /* set default base addr */
    BW_FB_CSARn_BA(baseAddr, 0, 0x0000U);

    /* FB_CSMR0 register set */
    /* validation set to true */
    BW_FB_CSMRn_V(baseAddr, 0, true);
    /* set default base addr mask */
    BW_FB_CSMRn_BAM(baseAddr, 0, 0x0000U);
    /* write protection set to read & write access */
    BW_FB_CSMRn_WP(baseAddr, 0, false);

    /* FB_CSCR0 register setting */
    /* secondary wait states ignored */
    BW_FB_CSCRn_SWSEN(baseAddr, 0, false);
    /* FB_TS/FB_ALE asserts for one bus clock cycle */
    BW_FB_CSCRn_EXTS(baseAddr, 0, false);
    /* 11b Assert FB_CSn on fourth rising clock edge after address is asserted. (Default FB_CS0) */
    BW_FB_CSCRn_ASET(baseAddr, 0, kFlexbus_fourthRisingEdge);
    /* Read address hold or deselect */
    BW_FB_CSCRn_RDAH(baseAddr, 0, kFlexbus_hold4or3cycles);
    /* Write address hold or deselect */
    BW_FB_CSCRn_WRAH(baseAddr, 0, kFlexbus_hold4cycles);
    /* sets wait states to maximum value */
    BW_FB_CSCRn_WS(baseAddr, 0, 0x3F);
    /* byte-lane shift set to not shifted */
    BW_FB_CSCRn_BLS(baseAddr, 0, kFlexbus_notShifted);
    /* auto-acknowledge set to No internal FB_TA is asserted. Cycle is terminated externally */
    BW_FB_CSCRn_AA(baseAddr, 0, false);
    /* port size set to 32-bit */
    BW_FB_CSCRn_PS(baseAddr, 0, kFlexbus_4bytes);
    /* byte enable mode set to: The FB_BEn signals are not asserted for reads. The FB_BEn signals are asserted for data write only. */
    BW_FB_CSCRn_BEM(baseAddr, 0, false);
    /* burst-read disabled */
    BW_FB_CSCRn_BSTR(baseAddr, 0, false);
    /* burst-write disabled */
    BW_FB_CSCRn_BSTW(baseAddr, 0, false);

    /* FB_CSPMCR register set */
    /* all multiplex groups set to 0x0 */
    BW_FB_CSPMCR_GROUP1(baseAddr, kFlexbus_multiplex_group1_FB_ALE);
    BW_FB_CSPMCR_GROUP2(baseAddr, kFlexbus_multiplex_group2_FB_CS4);
    BW_FB_CSPMCR_GROUP3(baseAddr, kFlexbus_multiplex_group3_FB_CS5);
    BW_FB_CSPMCR_GROUP4(baseAddr, kFlexbus_multiplex_group4_FB_TBST);
    BW_FB_CSPMCR_GROUP5(baseAddr, kFlexbus_multiplex_group5_FB_TA);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/



