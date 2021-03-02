#include "tc_00001.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"
#include "mqx.h"

#if   PSP_MQX_CPU_IS_KINETIS
  #include "lwgpio_kgpio.h"
#elif PSP_MQX_CPU_IS_MCF5225X
  #include "lwgpio_mcf5225.h"
#elif PSP_MQX_CPU_IS_MCF51CN
  #include "lwgpio_mcf51cn.h"
#elif PSP_MQX_CPU_IS_MCF5441X
  #include "lwgpio_mcf5441.h"
#elif PSP_MQX_CPU_IS_MPC512x
  #include "lwgpio_mpc5125.h"
#elif PSP_MQX_CPU_IS_MPX
  #include "lwgpio_siul.h"
#else
  #error lwgpio device driver not supported for processor.
#endif

#include "lwgpio.h"
#include "port_pin_list.h"


void tc_00001(void)
{
	uint32_t ret_value;

	EU_ASSERT(TRUE == lwgpio_init(&tst_port_pin_init_list[0], tst_port_pin_id_list[0], LWGPIO_DIR_NOCHANGE, LWGPIO_VALUE_NOCHANGE));
	lwgpio_set_functionality(&tst_port_pin_init_list[0], tst_port_pin_gpiofunc_list[0]);
#if (PSP_MQX_CPU_IS_MPC512x || PSP_MQX_CPU_IS_MCF5441X)
	#warning lwgpio device driver does not support appropriate set/get functionality
#else
  EU_ASSERT(tst_port_pin_gpiofunc_list[0] == lwgpio_get_functionality(&tst_port_pin_init_list[0]));
#endif
	lwgpio_set_direction(&tst_port_pin_init_list[0], LWGPIO_DIR_OUTPUT);
	lwgpio_set_value(&tst_port_pin_init_list[0], LWGPIO_VALUE_LOW);
	lwgpio_toggle_value(&tst_port_pin_init_list[0]);
	EU_ASSERT(LWGPIO_VALUE_HIGH == lwgpio_get_value(&tst_port_pin_init_list[0]));
#if (PSP_MQX_CPU_IS_MCF5441X || PSP_MQX_CPU_IS_MPX)
	#warning lwgpio device driver does not support appropriate get_raw functionality
#else
	EU_ASSERT(LWGPIO_VALUE_HIGH == lwgpio_get_raw(&tst_port_pin_init_list[0]));
#endif

	EU_ASSERT(TRUE == lwgpio_init(&tst_port_pin_init_list[1], tst_port_pin_id_list[1], LWGPIO_DIR_NOCHANGE, LWGPIO_VALUE_NOCHANGE));
	EU_ASSERT(TRUE == lwgpio_int_init(&tst_port_pin_init_list[1], LWGPIO_INT_MODE_RISING));
	ret_value = lwgpio_int_get_vector(&tst_port_pin_init_list[1]);
	lwgpio_int_enable(&tst_port_pin_init_list[1], TRUE);
	EU_ASSERT(FALSE == lwgpio_int_get_flag(&tst_port_pin_init_list[1]));
	lwgpio_int_clear_flag(&tst_port_pin_init_list[1]);
}
