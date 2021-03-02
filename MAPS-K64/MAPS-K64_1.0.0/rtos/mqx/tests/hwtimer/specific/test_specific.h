
/* Settings specific for test module */
#ifndef __test_specific_h__
#define __test_specific_h__

#undef  BSPCFG_HWTIMER_PIT_FREEZE
#define BSPCFG_HWTIMER_PIT_FREEZE   (0)

#define ID_ARRAY_MAX                (9) //maximum number of id's in id_array
#define HWTIMER_DEFAULT_PRIORITY    (3) 

#define DEVIF_ARRAY_TERMINATOR      (NULL)
#define ID_ARRAY_TERMINATOR         (-1) /* We use -1 as terminator */


#endif
