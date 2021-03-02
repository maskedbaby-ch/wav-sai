
/* Settings specific for test module */
#ifndef __test_specific_h__
#define __test_specific_h__

#undef BSPCFG_ENABLE_FLASHX

#define BSPCFG_ENABLE_FLASHX  1

/* disable unused drivers to save space for intram targets */
#define BSPCFG_ENABLE_ESDHC     0
#define BSPCFG_ENABLE_SAI       0
#define BSPCFG_ENABLE_ADC0      0
#define BSPCFG_ENABLE_ADC1      0
#define BSPCFG_ENABLE_ADC2      0
#define BSPCFG_ENABLE_ADC3      0
#define BSPCFG_ENABLE_RTCDEV    0
#define BSPCFG_ENABLE_I2C0      0
#define BSPCFG_ENABLE_II2C0     0
#define BSPCFG_ENABLE_I2C1      0
#define BSPCFG_ENABLE_II2C1     0
#define BSPCFG_ENABLE_SPI0      0
#define BSPCFG_ENABLE_SPI1      0
#define BSPCFG_ENABLE_SPI2      0

#endif
