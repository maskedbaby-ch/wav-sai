#ifndef __test_h__
#define __test_h__

#include "IOClient.h"
#include "IOClient_PTAC_OpenTestingModule.h"
#include "IOClient_PTAC_CloseTestingModule.h"

#if defined BSP_KWIKSTIK_K40X256 || defined BSP_TWR_K40X256 || BSP_TWR_K40D100M  || defined BSP_TWR_K21D50M || BSP_TWR_K53N512
#define I2C_DEVICE_POLLED    "i2c1:"
#define I2C_DEVICE_INTERRUPT "ii2c1:"
#define ENABLE_I2C_INTERRUPT  BSPCFG_ENABLE_II2C1
#if ! BSPCFG_ENABLE_I2C1
#error This application requires BSPCFG_ENABLE_I2C1 defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#elif defined BSP_TWRPXS30
#define I2C_DEVICE_POLLED    "i2c2:"
#define I2C_DEVICE_INTERRUPT "ii2c2:"
#define ENABLE_I2C_INTERRUPT  BSPCFG_ENABLE_II2C2
#if ! BSPCFG_ENABLE_I2C2
#error This application requires BSPCFG_ENABLE_I2C2 defined non-zero in user_config.h. Please recompile BSP with this option.
#endif
#else
#define I2C_DEVICE_POLLED    "i2c0:"
#define I2C_DEVICE_INTERRUPT "ii2c0:"
#define ENABLE_I2C_INTERRUPT  BSPCFG_ENABLE_II2C0
#endif

#define BUFFER_SIZE 256
#define TEST_STRING "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

#define UART0 0
#define UART1 1
#define UART2 2
#define UART3 3
#define UART4 4

#endif