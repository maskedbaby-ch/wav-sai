#include "port_pin_list.h"

LWGPIO_STRUCT tst_port_pin_init_list[TST_PORT_PIN_COUNT];
LWGPIO_PIN_ID tst_port_pin_id_list[TST_PORT_PIN_COUNT]=
{
    (111), /* one BSP LED pin */
    (131)  /* one BSP IRQ capable pin */
};
uint32_t tst_port_pin_gpiofunc_list[TST_PORT_PIN_COUNT]=
{
    SIUL_PCR_SMC_MASK,
    0
};
