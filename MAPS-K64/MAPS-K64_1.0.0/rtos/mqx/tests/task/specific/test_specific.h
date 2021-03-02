
/* Settings specific for test module */
#ifndef __test_specific_h__
#define __test_specific_h__

#undef MQX_USE_LOGS
#undef MQX_KERNEL_LOGGING
#undef MQX_TASK_DESTRUCTION
#undef MQX_ENABLE_USER_MODE
#undef MQX_USE_TIMER

#define MQX_USE_TIMER         1
#define MQX_USE_LOGS          1
#define MQX_KERNEL_LOGGING    1
#define MQX_TASK_DESTRUCTION  1
#define MQX_ENABLE_USER_MODE  0

#endif
