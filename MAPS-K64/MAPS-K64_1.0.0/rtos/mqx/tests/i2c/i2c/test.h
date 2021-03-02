#if defined BSP_TWR_VF65GS10_A5 || defined BSP_TWR_K60D100M || defined BSP_TWR_K70F120M
#define I2C_DEVICE_POLLED    "i2c0:"
#define I2C_DEVICE_INTERRUPT "ii2c0:"
#define ENABLE_I2C_INTERRUPT  BSPCFG_ENABLE_II2C0
#if ! BSPCFG_ENABLE_I2C0
#error This application requires BSPCFG_ENABLE_I2C0 defined non-zero in user_config.h. Please recompile BSP with this option.
#endif
#elif defined BSP_TWR_K21D50M || defined BSP_TWR_KL25Z48M
#define I2C_DEVICE_POLLED    "i2c1:"
#define I2C_DEVICE_INTERRUPT "ii2c1:"
#define ENABLE_I2C_INTERRUPT  BSPCFG_ENABLE_II2C1
#if ! BSPCFG_ENABLE_I2C1
#error This application requires BSPCFG_ENABLE_I2C1 defined non-zero in user_config.h. Please recompile BSP with this option.
#endif
#else
#define I2C_DEVICE_POLLED    "i2c0:"
#define I2C_DEVICE_INTERRUPT "ii2c0:"
#define ENABLE_I2C_INTERRUPT  BSPCFG_ENABLE_II2C0
#endif