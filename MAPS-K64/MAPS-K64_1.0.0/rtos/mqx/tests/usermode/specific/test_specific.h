
/* Settings specific for test module */
#ifndef __test_specific_h__
#define __test_specific_h__

#undef MQX_USE_LWEVENTS
#undef MQX_USE_LOGS
#undef MQX_USE_LWLOGS
#undef MQX_USE_LWMSGQ
#undef MQX_TASK_DESTRUCTION
#undef MQX_COMPONENT_DESTRUCTION
#undef MQX_KERNEL_LOGGING
#undef MQX_ENABLE_USER_MODE
#undef MQX_DEFAULT_USER_ACCESS_RW
#undef MQX_ENABLE_USER_STDAPI
#undef MQX_HAS_TICK

#define MQX_USE_LWEVENTS           1
#define MQX_USE_LOGS               1
#define MQX_USE_LWLOGS             1
#define MQX_USE_LWMSGQ             1
#define MQX_TASK_DESTRUCTION       1
#define MQX_COMPONENT_DESTRUCTION  1
#define MQX_KERNEL_LOGGING         1
#define MQX_ENABLE_USER_MODE       1
#define MQX_DEFAULT_USER_ACCESS_RW 1
#define MQX_ENABLE_USER_STDAPI     1
#define MQX_HAS_TICK               1

#endif
