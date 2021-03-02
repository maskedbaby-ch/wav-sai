
/* Settings specific for test module */
#ifndef __test_specific_h__
#define __test_specific_h__

#undef MQX_USE_IO
#undef BSPCFG_ENABLE_I2S0
#undef BSPCFG_ENABLE_I2S1
#undef BSPCFG_ENABLE_II2S0
#undef BSPCFG_ENABLE_II2S1
#undef BSPCFG_SCI3_BAUD_RATE

#define MQX_USE_IO              1
#define BSPCFG_ENABLE_I2S0      1
#define BSPCFG_ENABLE_I2S1      1
#define BSPCFG_ENABLE_II2S0     1
#define BSPCFG_ENABLE_II2S1     1
#define BSPCFG_SCI3_BAUD_RATE   9600

#endif
