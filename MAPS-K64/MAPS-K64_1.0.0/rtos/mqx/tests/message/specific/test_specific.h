/* Settings specific for Message module */
#ifndef __test_specific_h__
#define __test_specific_h__

#undef MQX_CHECK_ERRORS
#define MQX_CHECK_ERRORS 1

#undef MQX_CHECK_VALIDITY
#define MQX_CHECK_VALIDITY 1

#undef MQX_USE_MESSAGES
#define MQX_USE_MESSAGES 1

#undef MQXCFG_ENABLE_MSG_TIMEOUT_ERROR
#define MQXCFG_ENABLE_MSG_TIMEOUT_ERROR 1

#endif
