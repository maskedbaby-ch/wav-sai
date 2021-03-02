
/* Settings specific for test module */
#ifndef __test_specific_h__
#define __test_specific_h__

#undef MQXCFG_ENABLE_FP
#undef MQX_INCLUDE_FLOATING_POINT_IO
#undef NEED_STRTOD

#define MQXCFG_ENABLE_FP                1
#define MQX_INCLUDE_FLOATING_POINT_IO   1
#define NEED_STRTOD                     1

#endif
