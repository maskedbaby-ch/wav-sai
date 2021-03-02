
/* Settings specific for test module */
#ifndef __board_specific_h__
#define __board_specific_h__

#undef BSPCFG_ENABLE_TTYA
#define BSPCFG_ENABLE_TTYA      1

#undef IOCLIENT_SCI_CHANNEL
#define IOCLIENT_SCI_CHANNEL    0 /* ttya:" */

#endif
