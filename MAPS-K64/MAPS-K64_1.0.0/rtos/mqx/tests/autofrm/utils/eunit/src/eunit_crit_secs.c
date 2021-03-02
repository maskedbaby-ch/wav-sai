#include <mqx_inc.h>
//#include <stdio.h>
#include "eunit_crit_secs.h"
#include "fio.h"

volatile uint16 eunit_fails_array[EUNIT_TRES_ARRAY_SIZE] = {0};
volatile uint8 eunit_tres_array[EUNIT_TRES_ARRAY_SIZE];
volatile uint16 eunit_passed=0, eunit_failed=0, eunit_overflow=0;

#if MQX_ENABLE_USER_MODE
   USER_RO_ACCESS LWSEM_STRUCT assert_lwsem;
   KERNEL_DATA_STRUCT_PTR assert_kernel_data;
#endif

void eunit_mutex_init(void)
{
#if MQX_ENABLE_USER_MODE
   _usr_lwsem_create(&assert_lwsem,1);
   _GET_KERNEL_DATA(assert_kernel_data);
#endif
}

void eunit_test_stop(void)
{
    _ASM_NOP();
    printf("Passed count: %4d, failed count: %4d, total: %4d\n", eunit_passed, eunit_failed + (eunit_overflow * EUNIT_TRES_ARRAY_SIZE), (eunit_passed+(eunit_failed + (eunit_overflow * EUNIT_TRES_ARRAY_SIZE))));
    if (eunit_failed || eunit_overflow)
    {
        int i = 0;
        printf("Last %d failing asserts :\n",(eunit_overflow)? EUNIT_TRES_ARRAY_SIZE : eunit_failed);
        if (eunit_overflow)
        {
            for (i = eunit_failed; i < EUNIT_TRES_ARRAY_SIZE; i++)
            {
                printf("Line : %4i\n",eunit_fails_array[i]);
            }            
            for (i = 0; i < eunit_failed; i++)
            {
                printf("Line : %4i\n",eunit_fails_array[i]);
            }
        }
        else
        {
            while (eunit_fails_array[i] != 0)
            {
                printf("Line : %4i\n",eunit_fails_array[i++]);
            }
        }
    }
/* 
 *inject SW breakpoint, keil(cc_arm) doesn't need this
 */
#if ((MQX_CPU & (PSP_CPU_ARCH(PSP_CPU_ARCH_MASK))) == (PSP_CPU_ARCH(PSP_CPU_ARCH_ARM)) && !defined(__CC_ARM))
    asm("SWI 0x11");
#endif

#if (MQX_CPU & (PSP_CPU_ARCH(PSP_CPU_ARCH_MASK))) == (PSP_CPU_ARCH(PSP_CPU_ARCH_COLDFIRE))
    asm
    {
        halt
    }
#endif
}

void eunit_send_details(uint8 value, uint16 line_num, char* message)
{
  if(value)
  {
#if PRINT_ASSERT == 1
    printf("EU_ASSERT_*: line_num:%4i: PASSED ,\" %s\"\n", line_num, message);
#endif
    eunit_passed++;
  }
  else
  {
#if PRINT_ASSERT == 1
    printf("EU_ASSERT_*: line_num:%4i: FAILED ,\" %s\"\n", line_num, message);
#endif
    eunit_fails_array[eunit_failed++] = line_num;
    if(eunit_failed == EUNIT_TRES_ARRAY_SIZE)
    {
        eunit_overflow++;
        eunit_failed = 0;
    }
  }
}
