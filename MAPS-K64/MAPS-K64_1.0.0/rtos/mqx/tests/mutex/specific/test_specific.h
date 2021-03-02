
/* Settings specific for test module */
#ifndef __test_specific_h__
#define __test_specific_h__

#undef MQX_USE_MUTEXES
#undef MQX_MUTEX_HAS_POLLING
#undef MQX_COMPONENT_DESTRUCTION
#undef MQX_TASK_DESTRUCTION

#define MQX_USE_MUTEXES         1
#define MQX_MUTEX_HAS_POLLING   1
#define MQX_COMPONENT_DESTRUCTION     1
#define MQX_TASK_DESTRUCTION          1

#endif
