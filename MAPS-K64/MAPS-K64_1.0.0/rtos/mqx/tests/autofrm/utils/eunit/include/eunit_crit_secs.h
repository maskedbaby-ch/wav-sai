#ifndef EUNIT_CRIT_SECS_H
#define EUNIT_CRIT_SECS_H

#include <mqx.h>
#include <mutex.h>
#include <mqx_inc.h>

#include "Std_Types.h"
/* PRINT_ASSERT = 1 Full information is printed out per assert  */
/* PRINT_ASSERT = 0 Information is printed out after test       */
#ifndef PRINT_ASSERT
    #define PRINT_ASSERT 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if MQX_ENABLE_USER_MODE 
   #define EU_ENTER_CRITICAL_SECTION \
      if(assert_kernel_data->IN_ISR == 0) \
         _lwsem_wait(&assert_lwsem); \
      else \
         _int_disable();
         
   #define EU_EXIT_CRITICAL_SECTION \
      if(assert_kernel_data->IN_ISR == 0) \
         _lwsem_post(&assert_lwsem); \
      else \
         _int_enable();
         
#else
   #define EU_ENTER_CRITICAL_SECTION _int_disable();
   #define EU_EXIT_CRITICAL_SECTION _int_enable();
#endif


#define EU_DO_WHEN_ASSERT_FAIL

#ifdef EU_PRINT_DETAILS
#undef EU_DO_INSIDE_ASSERTION
#define EU_DO_INSIDE_ASSERTION(value, file_id, line_num, message) eunit_send_details(value, line_num, message)
#endif

extern volatile uint8 eunit_tres_array[EUNIT_TRES_ARRAY_SIZE];

#if MQX_ENABLE_USER_MODE
   extern USER_RO_ACCESS LWSEM_STRUCT assert_lwsem;
   extern KERNEL_DATA_STRUCT_PTR assert_kernel_data;
#endif

extern void eunit_mutex_init(void);
extern void eunit_test_stop(void);
extern void eunit_send_details(uint8 value, uint16 line_num, char* message);

#ifdef __cplusplus
}
#endif

#endif /* EUNIT_CRIT_SECS_H */
