#include "port_pin_list.h"

LWGPIO_STRUCT tst_port_pin_init_list[TST_PORT_PIN_COUNT];
LWGPIO_PIN_ID tst_port_pin_id_list[TST_PORT_PIN_COUNT]=
{
  (LWGPIO_PORT_UC | LWGPIO_PIN1), /* LWGPIO: UC_01 */
  (LWGPIO_PORT_UC | LWGPIO_PIN0), /* LWGPIO: UC_00 */
  (LWGPIO_PORT_TA | LWGPIO_PIN3), /* LWGPIO: TA_03 */
  (LWGPIO_PORT_TA | LWGPIO_PIN2), /* LWGPIO: TA_02 */
  (LWGPIO_PORT_TA | LWGPIO_PIN1), /* LWGPIO: TA_01 */
  (LWGPIO_PORT_TA | LWGPIO_PIN0), /* LWGPIO: TA_00 */
};
IOClientTestPointType tst_port_pin_maftp_list[TST_PORT_PIN_COUNT]=
{
  (IOClient_TestPoint_Port_A41),    /* LWGPIO: UC_01 */
  (IOClient_TestPoint_Port_A42),    /* LWGPIO: UC_00 */
  (IOClient_TestPoint_Port_A58),    /* LWGPIO: TA_03 */
  (IOClient_TestPoint_Port_A59),    /* LWGPIO: TA_02 */
  (IOClient_TestPoint_Port_A60),    /* LWGPIO: TA_01 */
  (IOClient_TestPoint_Port_A61),    /* LWGPIO: TA_00 */
};
