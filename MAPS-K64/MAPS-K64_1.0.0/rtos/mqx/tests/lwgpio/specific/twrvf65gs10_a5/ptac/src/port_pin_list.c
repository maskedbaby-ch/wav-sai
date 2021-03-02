#include "port_pin_list.h"

LWGPIO_STRUCT tst_port_pin_init_list[TST_PORT_PIN_COUNT];
LWGPIO_PIN_ID tst_port_pin_id_list[TST_PORT_PIN_COUNT]=
{
  (LWGPIO_PIN_PTB10),     /* LWGPIO: ELEV B21 */
  (LWGPIO_PIN_PTB23),     /* LWGPIO: ELEV B22 */
  (LWGPIO_PIN_PTA20),     /* LWGPIO: ELEV B52 */
  (LWGPIO_PIN_PTB18),     /* LWGPIO: ELEV A35 */
  (LWGPIO_PIN_PTB28),     /* LWGPIO: ELEV A11 */
};

IOClientTestPointType tst_port_pin_maftp_list[TST_PORT_PIN_COUNT]=
{
  (IOClient_TestPoint_Port_B21),    /* LWGPIO: ELEV B21 */
  (IOClient_TestPoint_Port_B22),    /* LWGPIO: ELEV B22 */
  (IOClient_TestPoint_Port_B52),    /* LWGPIO: ELEV B52 */
  (IOClient_TestPoint_Port_A35),    /* LWGPIO: ELEV A35 */
  (IOClient_TestPoint_Port_A11),    /* LWGPIO: ELEV A11 */
};
