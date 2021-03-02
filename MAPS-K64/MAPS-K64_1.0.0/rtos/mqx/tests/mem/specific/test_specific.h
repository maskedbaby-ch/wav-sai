
/* Settings specific for test module */
#ifndef __test_specific_h__
#define __test_specific_h__

#undef MQX_USE_MEM
#undef MQX_USE_LWMEM
#undef MQX_USE_LWMEM_ALLOCATOR
#undef MQX_HAS_TIME_SLICE
#undef MQX_USE_LWEVENTS

#define MQX_USE_MEM             1
#define MQX_USE_LWMEM           0
#define MQX_USE_LWMEM_ALLOCATOR 0
#define MQX_HAS_TIME_SLICE      1
#define MQX_USE_LWEVENTS        1

#endif
