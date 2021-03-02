
/* Settings specific for test module */
#ifndef __test_specific_h__
#define __test_specific_h__

#undef TEST_LWLOGS
#undef MQX_USE_LOGS
#undef MQX_USE_LWLOGS
#undef MQX_KERNEL_LOGGING

#define TEST_LWLOGS          1
#define MQX_USE_LOGS         0
#define MQX_USE_LWLOGS       1
#define MQX_KERNEL_LOGGING   1

#endif
