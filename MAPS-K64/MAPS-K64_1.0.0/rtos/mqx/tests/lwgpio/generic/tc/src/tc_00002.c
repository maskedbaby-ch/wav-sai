#include "tc_00002.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include "mqx.h"

#if MQX_CPU == PSP_CPU_MCF5282
  #include "lwgpio_mcf5235.h"
#elif PSP_MQX_CPU_IS_MCF5225X
  #include "lwgpio_mcf5225.h"
#elif PSP_MQX_CPU_IS_MCF5227X
  #include "lwgpio_mcf5227.h"
#elif PSP_MQX_CPU_IS_MCF51AC
  #include "lwgpio_mcf51ac.h" 
#elif PSP_MQX_CPU_IS_MCF51JM
  #include "lwgpio_mcf51jm.h"   
#elif PSP_MQX_CPU_IS_MCF51CN
  #include "lwgpio_mcf51cn.h"
#elif PSP_MQX_CPU_IS_MCF51EM
  #include "lwgpio_mcf51em.h"
#elif PSP_MQX_CPU_IS_MCF51MM
  #include "lwgpio_mcf51mm.h"  
#elif PSP_MQX_CPU_IS_MCF5222X
  #include "lwgpio_mcf5222.h"
#elif PSP_MQX_CPU_IS_MCF5223X
  #include "lwgpio_mcf5223.h"
#elif PSP_MQX_CPU_IS_MCF5441X
  #include "lwgpio_mcf5441.h"
#elif PSP_MQX_CPU_IS_MCF5301X
  #include "lwgpio_mcf5301.h"
  #elif PSP_MQX_CPU_IS_KINETIS
  #include "lwgpio_kgpio.h"
#else
  #error lwgpio device driver not supported for processor.
#endif

#include "lwgpio.h"
#include "port_pin_list.h"

#include "IOClient.h"
#include "IOClient_DMB_GetPortPinValue.h"
#include "IOClient_DMB_SetPortPinDirection.h"
#include "IOClient_DMB_SetPortPinValue.h"


void tc_00002(void)
{
  uint32_t ret_value;
  uint32_t pin_it;
  
  /* MAF used as input */
  for(pin_it=0; pin_it<TST_PORT_PIN_COUNT; pin_it++)
  {
    EU_ASSERT(TRUE == lwgpio_init(&tst_port_pin_init_list[pin_it], tst_port_pin_id_list[pin_it], LWGPIO_DIR_NOCHANGE, LWGPIO_VALUE_NOCHANGE));
    lwgpio_set_functionality(&tst_port_pin_init_list[pin_it], 0);
    EU_ASSERT(0 == lwgpio_get_functionality(&tst_port_pin_init_list[pin_it]));
    EU_ASSERT(0 == IOClient_DMB_SetPortPinDirection(tst_port_pin_maftp_list[pin_it], TL_max_input));
    lwgpio_set_direction(&tst_port_pin_init_list[pin_it], LWGPIO_DIR_OUTPUT);
    lwgpio_set_value(&tst_port_pin_init_list[pin_it], LWGPIO_VALUE_LOW);
    EU_ASSERT(STD_LOW == IOClient_DMB_GetPortPinValue(tst_port_pin_maftp_list[pin_it]));
    lwgpio_set_value(&tst_port_pin_init_list[pin_it], LWGPIO_VALUE_HIGH);
    EU_ASSERT(STD_HIGH == IOClient_DMB_GetPortPinValue(tst_port_pin_maftp_list[pin_it]));
    lwgpio_toggle_value(&tst_port_pin_init_list[pin_it]);
    EU_ASSERT(STD_LOW == IOClient_DMB_GetPortPinValue(tst_port_pin_maftp_list[pin_it]));
    lwgpio_toggle_value(&tst_port_pin_init_list[pin_it]);
    EU_ASSERT(STD_HIGH == IOClient_DMB_GetPortPinValue(tst_port_pin_maftp_list[pin_it]));      
  }

  /* MAF used as output */
  for(pin_it=0; pin_it<TST_PORT_PIN_COUNT; pin_it++)
  {
    lwgpio_set_functionality(&tst_port_pin_init_list[pin_it], 0);
    EU_ASSERT(0 == lwgpio_get_functionality(&tst_port_pin_init_list[pin_it]));
    lwgpio_set_direction(&tst_port_pin_init_list[pin_it], LWGPIO_DIR_INPUT);
    EU_ASSERT(0 == IOClient_DMB_SetPortPinDirection(tst_port_pin_maftp_list[pin_it], TL_max_output));
    EU_ASSERT(0 == IOClient_DMB_SetPortPinValue(tst_port_pin_maftp_list[pin_it], TL_max_low));
    EU_ASSERT(LWGPIO_VALUE_LOW == lwgpio_get_value(&tst_port_pin_init_list[pin_it]));
    EU_ASSERT(LWGPIO_VALUE_LOW == lwgpio_get_raw(&tst_port_pin_init_list[pin_it]));
    EU_ASSERT(0 == IOClient_DMB_SetPortPinValue(tst_port_pin_maftp_list[pin_it], TL_max_high));
    EU_ASSERT(LWGPIO_VALUE_HIGH == lwgpio_get_value(&tst_port_pin_init_list[pin_it]));
    EU_ASSERT(LWGPIO_VALUE_HIGH == lwgpio_get_raw(&tst_port_pin_init_list[pin_it]));
  }

  /* set used MAF pins back to the inputs */
  for(pin_it=0; pin_it<TST_PORT_PIN_COUNT; pin_it++)
  {
    EU_ASSERT(0 == IOClient_DMB_SetPortPinDirection(tst_port_pin_maftp_list[pin_it], TL_max_input));
  }  
}
