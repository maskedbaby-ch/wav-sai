
/* Settings specific for test module */
#ifndef __test_specific_h__
#define __test_specific_h__

#undef MQX_EXIT_ENABLED
#undef MQX_USE_IDLE_TASK
#undef MQX_TAD_RESERVED_ENABLE
#undef MQX_USE_IO_COMPONENTS

#define MQX_EXIT_ENABLED        1
#define MQX_USE_IDLE_TASK       1
#define MQX_TAD_RESERVED_ENABLE 1
#define MQX_USE_IO_COMPONENTS   1

#endif
