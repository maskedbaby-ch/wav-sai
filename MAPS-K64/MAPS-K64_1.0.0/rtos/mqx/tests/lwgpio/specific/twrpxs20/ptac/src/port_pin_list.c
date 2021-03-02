#include "port_pin_list.h"

LWGPIO_STRUCT tst_port_pin_init_list[TST_PORT_PIN_COUNT];
LWGPIO_PIN_ID tst_port_pin_id_list[TST_PORT_PIN_COUNT]=
{
  (42),  /* LWGPIO: ELEV B21 */
  (49),  /* LWGPIO: ELEV B22 */
  (50),  /* LWGPIO: ELEV B23 */
  (73),  /* LWGPIO: ELEV B35 */
  (52),  /* LWGPIO: ELEV B52 */
  (74),  /* LWGPIO: ELEV A35 */
  (93),  /* LWGPIO: ELEV A11 */
  (51),  /* LWGPIO: ELEV A10 */
  (53),  /* LWGPIO: ELEV A9 */
};
IOClientTestPointType tst_port_pin_maftp_list[TST_PORT_PIN_COUNT]=
{
  (IOClient_TestPoint_Port_B21),    /* LWGPIO: ELEV B21 */
  (IOClient_TestPoint_Port_B22),    /* LWGPIO: ELEV B22 */
  (IOClient_TestPoint_Port_B23),    /* LWGPIO: ELEV B23 */
  (IOClient_TestPoint_Port_B35),    /* LWGPIO: ELEV B35 */
  (IOClient_TestPoint_Port_B52),    /* LWGPIO: ELEV B52 */
  (IOClient_TestPoint_Port_A35),    /* LWGPIO: ELEV A35 */
  (IOClient_TestPoint_Port_A11),    /* LWGPIO: ELEV A11 */
  (IOClient_TestPoint_Port_A10),    /* LWGPIO: ELEV A10 */
  (IOClient_TestPoint_Port_A9),     /* LWGPIO: ELEV A9 */
};