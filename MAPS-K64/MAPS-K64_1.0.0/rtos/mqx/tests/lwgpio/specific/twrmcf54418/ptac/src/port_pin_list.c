#include "port_pin_list.h"

LWGPIO_STRUCT tst_port_pin_init_list[TST_PORT_PIN_COUNT];
LWGPIO_PIN_ID tst_port_pin_id_list[TST_PORT_PIN_COUNT]=
{
  (LWGPIO_PORT_G | LWGPIO_PIN2),  /* LWGPIO: ELEV B21 */
  (LWGPIO_PORT_B | LWGPIO_PIN6),  /* LWGPIO: ELEV B35 */
  (LWGPIO_PORT_G | LWGPIO_PIN3),  /* LWGPIO: ELEV B52 */
  (LWGPIO_PORT_B | LWGPIO_PIN5),  /* LWGPIO: ELEV A35 */
  (LWGPIO_PORT_G | LWGPIO_PIN1),  /* LWGPIO: ELEV A11 */
  (LWGPIO_PORT_G | LWGPIO_PIN0),  /* LWGPIO: ELEV A9 */
};
IOClientTestPointType tst_port_pin_maftp_list[TST_PORT_PIN_COUNT]=
{
  (IOClient_TestPoint_Port_B21),    /* LWGPIO: ELEV B21 */
  (IOClient_TestPoint_Port_B35),    /* LWGPIO: ELEV B35 */
  (IOClient_TestPoint_Port_B52),    /* LWGPIO: ELEV B52 */
  (IOClient_TestPoint_Port_A35),    /* LWGPIO: ELEV A35 */
  (IOClient_TestPoint_Port_A11),    /* LWGPIO: ELEV A11 */
  (IOClient_TestPoint_Port_A9),     /* LWGPIO: ELEV A9 */
};