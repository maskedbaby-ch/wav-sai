#include "port_pin_list.h"

LWGPIO_STRUCT tst_port_pin_init_list[TST_PORT_PIN_COUNT];
LWGPIO_PIN_ID tst_port_pin_id_list[TST_PORT_PIN_COUNT]=
{
    (LWGPIO_PORT_G | LWGPIO_PIN0),  /* one BSP LED pin */
    (LWGPIO_PORT_C | LWGPIO_PIN1)   /* one BSP IRQ capable pin */
};
uint32_t tst_port_pin_gpiofunc_list[TST_PORT_PIN_COUNT]=
{
    LWGPIO_MUX_G0_GPIO,
    LWGPIO_MUX_C1_GPIO
};
