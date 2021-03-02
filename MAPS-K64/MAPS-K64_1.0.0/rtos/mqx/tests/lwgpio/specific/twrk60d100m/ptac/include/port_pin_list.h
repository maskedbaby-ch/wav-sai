#include "mqx.h"
#include "lwgpio_kgpio.h"
#include "lwgpio.h"
#include "TL_TestPoints.h"

#define LWGPIO_PIN_MUX_GPIO    (1)

#define TST_PORT_PIN_COUNT     9

extern LWGPIO_STRUCT tst_port_pin_init_list[TST_PORT_PIN_COUNT];
extern LWGPIO_PIN_ID tst_port_pin_id_list[TST_PORT_PIN_COUNT];
extern IOClientTestPointType tst_port_pin_maftp_list[TST_PORT_PIN_COUNT];
