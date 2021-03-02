#include "mqx.h"
#include "lwgpio_kgpio.h"
#include "lwgpio.h"

#define TST_PORT_PIN_COUNT 2

extern LWGPIO_STRUCT tst_port_pin_init_list[TST_PORT_PIN_COUNT];
extern LWGPIO_PIN_ID tst_port_pin_id_list[TST_PORT_PIN_COUNT];
extern uint32_t tst_port_pin_gpiofunc_list[TST_PORT_PIN_COUNT];
