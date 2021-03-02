/*!
    @file IOClient_FunctionID.h
    @version 0.0.69.0

    \brief IO Client Remote Functions
    \project MAF
    \author V&V Team
    \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

    \platform
    \peripheral
    \prerequisites
    \arversion 2.1.0
    \arrevision Rel.2.1, Rev.0017
    \swversion 2.1.0
    @lastmodusr B22656
    @lastmoddate Mar-10-2011

    Freescale Semiconductor Inc.
    (c) Copyright 2008 Freescale Semiconductor Inc.
    ALL RIGHTS RESERVED.
*/

#ifndef IOCLIENT_FUNCTIONID_H
#define IOCLIENT_FUNCTIONID_H

/******************* type definitions ****************************************/

/*!
    \brief Supported remote function's return codes
*/
    #define E_IOCLIENT_OK   ((uint8)0x00)   /* Measurement OK, Test Passed */
    #define E_IOCLIENT_FAIL ((uint8)0x01)   /* Measurement OK, Test Failed */
    #define E_IOCLIENT_ERR  ((uint8)0x02)   /* Measurement Failure */

/*!
    \brief Supported remote function calls
*/
typedef enum {
    /**< Test whether digital signal has given period and duty cycle  */
    /* !!! DO NOT COMMENT OUT or ERASE ANY PART of FunctionIDType enum below */
    /* ADC testing functions */
    IOClient_AO_SetVoltage_ID = 0,
    IOClient_AO_SetVoltageMultiple_ID,
    /* CAN testing functions */
    IOClient_CAN_SendDelayTransmitter_ID,
    IOClient_CAN_SetReceiverStart_ID,
    IOClient_CAN_SetReceiverStop_ID,
    IOClient_CAN_SendGroupTransmitter_ID,
    IOClient_CAN_SendRepeatTransmitter_ID,
    IOClient_CAN_TestMessageFrame_ID,
    IOClient_CAN_Transmitter_ID,
    IOClient_CAN_Receiver_ID,
    /* CLK testing functions */
    IOClient_CLK_SetFrequency_ID,
    IOClient_CLK_InitTimeoutReset_ID,
    IOClient_CLK_SetTimeoutReset_ID,
    IOClient_CLK_StopTimeoutReset_ID,
    /* CMD execution functions */
    IOClient_CMD_Executor_ID,
    /* DIO testing and control functions */
    IOClient_DMB_GetPortPinValue_ID,
    IOClient_DMB_SetPortValue_ID,
    IOClient_DMB_SetPortPinDirection_ID,
    IOClient_DMB_SetPortDirection_ID,
    IOClient_DMB_SetPortPinValue_ID,
    IOClient_DMB_TestPortPinValue_ID,
    IOClient_MAX_SetAllToDefault_ID,
    /* DMB control functions */
    IOClient_DMB_CanControl_ID,
    IOClient_DMB_FlexRayControl_ID,
    IOClient_DMB_LinControl_ID,
    IOClient_DMB_SciControl_ID,
    IOClient_DMB_CpuReset_ID,
    IOClient_DMB_SetStatus_ID,
    IOClient_DMB_Control_ID,
    IOClient_DMB_GetMezzanineID_ID,
    IOClient_DMB_UsbRecovery_ID,
    /* DMB ver.I control functions */
    IOClient_SYS_DmbInit_ID,
    IOClient_DMB_PowerControl_ID,
    /* EUNIT testing and control functions */
    IOClient_EUNIT_Management_ID,
    /* FPGA testing functions */
    IOClient_FPGA_BaselineWobbling_ID,
    IOClient_FPGA_HighLowState_ID,
    IOClient_FPGA_RealTouchEmulation_ID,
    IOClient_FPGA_RealTouchEmulationWithNoise_ID,
    IOClient_FPGA_TouchEmulation_ID,
    IOClient_FPGA_TSITouchEmulation_ID,
    IOClient_FPGA_TSITouchEmulationMulti_ID,
    /* LIN testing and control functions */
    IOClient_LIN_Analyzer_ID,
    /* POWER control functions */
    IOClient_PWR_SwitchPowerOff_ID,
    IOClient_PWR_SetVoltage_ID,
    /* PWM testing and control functions */
    IOClient_HSDIO_SetVoltage_ID,
    IOClient_HSDIO_TestPwmIdle_ID,
    IOClient_HSDIO_TestPeriodAndDuty_ID,
    IOClient_HSDIO_GetCountEdgesResult_ID,
    IOClient_HSDIO_GetCountEdgesSetup_ID,
    IOClient_HSDIO_GetSignalLevelResult_ID,
    IOClient_HSDIO_GetSignalLevelSetup_ID,
    IOClient_HSDIO_TestLogicLevelResult_ID,
    IOClient_HSDIO_TestLogicLevelSetup_ID,
    IOClient_HSDIO_TestPeriodAndDutyResult_ID,
    IOClient_HSDIO_TestPeriodAndDutySetup_ID,
    /* RELAY control functions */
    IOClient_MUX_RelayCard_ID,
    /* SCI communication testing functions */
    IOClient_SCI_TestConnection_ID,
    /* SPI testing and control functions */
    IOClient_DMB_ProcessSPISlaveCmd_ID,
    IOClient_DMB_SetSPISlaveData_ID,
    IOClient_DMB_TestSPISlaveData_ID,
    IOClient_DMB_SetupSPISlave_ID,
    IOClient_DMB_SetSPITopology_ID,
    /* SPI timing measurement testing functions */
    IOClient_SPI_GetCs2ClkTimingSetup_ID,
    IOClient_SPI_GetCs2ClkTimingResult_ID,
    IOClient_SPI_GetClkTimingSetup_ID,
    IOClient_SPI_GetClkTimingResult_ID,
    IOClient_SPI_GetCs2ClkTimingMultipleSetup_ID,
    IOClient_SPI_GetCs2ClkTimingMultipleResult_ID,
    IOClient_SPI_MeasureTimingSetup_ID,
    /* SYS communication testing functions */
    IOClient_SYS_EchoScalar_ID,
    IOClient_SYS_EchoArray_ID,
    /* TC testing functions */
    IOClient_TC_TestPulseWidthSetup_ID,
    IOClient_TC_TestPulseWidthResult_ID,
    IOClient_TC_TestPulseWidthMultipleSetup_ID,
    IOClient_TC_TestPulseWidthMultipleResult_ID,
    IOClient_TC_TestPulseWidthCfgTrig_ID,
    IOClient_TC_TestPulseWidthSetWidth_ID,
    IOClient_TC_GenPulseTrainSetup_ID,
    IOClient_TC_GenPulseTrainStop_ID,
    IOClient_TC_TimeoutResetSet_ID,
    IOClient_TC_TimeoutResetStop_ID,
    /* PWM testing and control functions */
    IOClient_HSDIO_TestWfmAlignSetup_ID,
    IOClient_HSDIO_TestWfmAlignResult_ID,
    /* Fr testing and control functions */
    IOClient_FR_TestCntrlSetup_ID,
    IOClient_FR_TestCntrlStart_ID,
    IOClient_FR_TestCntrlClose_ID,
    IOClient_FR_TestCntrlSlotResult_ID,
    IOClient_FR_TestCntrlGetState_ID,
    IOClient_FR_SendWakeUp_ID,
    IOClient_FR_TestCntrlSleep_ID,
    IOClient_FR_SendMTSStart_ID,
    IOClient_FR_SendMTSStop_ID,
    IOClient_FR_TestMTS_ID

} IOClient_FunctionIDType;

#endif /* IOCLIENT_FUNCTIONID_H */
