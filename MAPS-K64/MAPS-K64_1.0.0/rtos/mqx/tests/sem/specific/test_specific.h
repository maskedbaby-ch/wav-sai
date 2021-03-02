
/* Settings specific for test module */
#ifndef __test_specific_h__
#define __test_specific_h__

#undef MQX_HAS_TIME_SLICE
#undef MQX_USE_MEM
#undef MQX_USE_SEMAPHORES
#undef MQX_USE_NAME
#undef MQX_COMPONENT_DESTRUCTION
#undef MQX_TASK_DESTRUCTION

#define MQX_HAS_TIME_SLICE    1
#define MQX_USE_MEM           1
#define MQX_USE_SEMAPHORES    1
#define MQX_USE_NAME          1
#define MQX_COMPONENT_DESTRUCTION   1
#define MQX_TASK_DESTRUCTION        1
#endif
