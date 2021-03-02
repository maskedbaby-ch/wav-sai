
/* Settings specific for test module */
#ifndef __test_specific_h__
#define __test_specific_h__

#undef MQX_USE_EVENTS
#undef MQX_USE_LWEVENTS
#undef MQX_USE_LOGS
#undef MQX_USE_LWLOGS
#undef MQX_USE_MESSAGES
#undef MQX_USE_LWMSGQ
#undef MQX_USE_SEMAPHORES
#undef MQX_USE_MUTEXES
#undef MQX_USE_NAME
#undef MQX_USE_PARTITIONS
#undef MQX_USE_MEM
#undef MQX_USE_LWMEM
#undef MQX_USE_LWMEM_ALLOCATOR
#undef MQX_TASK_DESTRUCTION
#undef MQX_COMPONENT_DESTRUCTION
#undef MQX_KERNEL_LOGGING
#undef MQX_USE_UNCACHED_MEM 

#define MQX_USE_NAME        1
#define MQX_USE_EVENTS      1
#define MQX_USE_LWEVENTS    1
#define MQX_USE_LOGS        1
#define MQX_USE_LWLOGS      1
#define MQX_USE_MESSAGES    1
#define MQX_USE_LWMSGQ      1
#define MQX_USE_SEMAPHORES  1
#define MQX_USE_MUTEXES     1
#define MQX_USE_NAME        1
#define MQX_USE_PARTITIONS  1
#define MQX_USE_MEM             0
#define MQX_USE_LWMEM           1
#define MQX_USE_LWMEM_ALLOCATOR 1
#define MQX_TASK_DESTRUCTION        1 
#define MQX_COMPONENT_DESTRUCTION   1
#define MQX_KERNEL_LOGGING      1
#define MQX_USE_UNCACHED_MEM    0

#endif
