
/* Settings specific for test module */
#ifndef __board_specific_h__
#define __board_specific_h__

#undef BSPCFG_ENABLE_TTYD
#define BSPCFG_ENABLE_TTYD      1

#undef IOCLIENT_SCI_CHANNEL
#define IOCLIENT_SCI_CHANNEL    3 /* ttyd:" */

#endif
