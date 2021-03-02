/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName:
* $Version :
* $Date    :
*
* Comments:
*
*   With this test case we are able to test cache working in low power mode (WFI is called)
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>

#if (MQX_USE_IDLE_TASK == 0)
#error This application requires MQX_USE_IDLE_TASK defined to 1 in user_config.h. Please recompile BSP and PSP with this option.
#endif


/* Task IDs */
#define TEST_TASK 5

extern void test_task(uint_32);
void lptmr_isr_example(void);

#define TEST_POOL_START 0x60000000 //0x80000000 = ddr 0x60000000 = mram
#define TEST_POOL_SIZE  0x00005000

#define TEST_ARRAY_SIZE 16
#define TEST_ARRAY_ALIGN 0x1000

unsigned char *ta1, *ta2, *ta3;

LWGPIO_STRUCT led1, led2;

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    /* Task Index,   Function,   Stack,  Priority,   Name,           Attributes, Param, Time Slice */
    { TEST_TASK,   test_task,     1500,         8, "test",  MQX_AUTO_START_TASK,     0,     0 },
    { 0 }
};


/*TASK*-----------------------------------------------------
*
* Task Name    : TEST_task
* Comments     :
*   Initialize test condition
*
*END*-----------------------------------------------------*/
void test_task
    (
        uint_32 initial_data
    )
{
    int compare_value=10; //5ms //value must be less than 0xFFFF
    _mem_pool_id test_pool;
    VCORTEX_NVIC_STRUCT_PTR nvic = (VCORTEX_NVIC_STRUCT_PTR)&(((CORTEX_SCS_STRUCT_PTR)CORTEX_PRI_PERIPH_IN_BASE)->NVIC);
    
    /* Enable LPT clock */
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;
    /* Reset lptmr */
    LPTMR0_CSR = 0x00; 
    /* Clear pending interrupt in NVIC for LPTMR0 */
    nvic->CLR[(INT_LPTimer - 16) >> 5] = 1 << ((INT_LPTimer - 16) & 0x1f);
    /* Enable LPT Interrupt in NVIC*/
    _int_install_kernel_isr(INT_LPTimer, lptmr_isr_example);
    _bsp_int_init(INT_LPTimer, 2, 0, TRUE);

    /* Configure LPT */
    LPTMR0_CMR = LPTMR_CMR_COMPARE(compare_value);  //Set compare value
    LPTMR0_PSR = LPTMR_PSR_PCS(0x1)|LPTMR_PSR_PBYP_MASK;  //Use LPO clock and bypass prescale
    LPTMR0_CSR = LPTMR_CSR_TIE_MASK;  //Enable LPT interrupt

    /* Change sleep mode */
    _lpm_idle_sleep_setup(TRUE);

    /* Create pool for testing */
    test_pool = _mem_create_pool((pointer)TEST_POOL_START, TEST_POOL_SIZE);
    if (test_pool == NULL)
    {
        printf("Create pool failed !\n");
        _task_block();
    }
    /* Allocate test arrays */
    ta1 = _mem_alloc_align_from(test_pool, TEST_ARRAY_SIZE, TEST_ARRAY_ALIGN);
    ta2 = _mem_alloc_align_from(test_pool, TEST_ARRAY_SIZE, TEST_ARRAY_ALIGN);
    ta3 = _mem_alloc_align_from(test_pool, TEST_ARRAY_SIZE, TEST_ARRAY_ALIGN);

    if ((NULL == ta1) || (NULL == ta1) || (NULL == ta1))
    {
        printf("Test arrays allocation failed!\n");
        _task_block();
    }

    /* Initialize compared values */
    ta2[15] = 0x22;
    ta3[15] = 0xdd;

    /* Initialization of LED */
    /* initialize lwgpio handle (led1, led2) for BSP_LED1, BSP_LED2 pin (defined in mqx/source/bsp/<bsp_name>/<bsp_name>.h file) */
    if (!lwgpio_init(&led1, BSP_LED1, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing LED1 GPIO as output failed.\n");
        _task_block();
    }
    if (!lwgpio_init(&led2, BSP_LED2, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing LED2 GPIO as output failed.\n");
        _task_block();
    }
    /* swich pin functionality (MUX) to GPIO mode */
    lwgpio_set_functionality(&led1, BSP_LED1_MUX_GPIO);
    lwgpio_set_functionality(&led2, BSP_LED2_MUX_GPIO);

    /* write logical 1 to the pins */
    lwgpio_set_value(&led1, LWGPIO_VALUE_HIGH); /* set pin to 1 */
    lwgpio_set_value(&led2, LWGPIO_VALUE_HIGH); /* set pin to 1 */

    printf(" Test initialization complete. Test starts:\n");

    LPTMR0_CSR |= LPTMR_CSR_TEN_MASK; //Turn on LPTMR and start counting

    _task_block();
}

void lptmr_isr_example(void)
{
    volatile uint_32 bar;
    static uint_32 tmp = 0;

    LPTMR0_CSR |= LPTMR_CSR_TCF_MASK;  //Clear LPT Compare flag

    if ((ta2[15] != 0x22) || (ta3[15] != 0xdd))
    {
        /* If code pass here we reproduced the issue */
        printf("%d",ta3[15]);
        lwgpio_set_value(&led2, LWGPIO_VALUE_LOW); /* set pin to 1 */
    }

    if (!(tmp++ % 10))
    {
        lwgpio_toggle_value(&led1);
    }

    for (int i = 0; i < 16 ; i++)
    {
        ta1[i]=0xAA;
        ta2[i]=0x11;
        ta3[i]=0xCC;
    }
    /* Invalidate by physical address */
    LMEM_PSCLCR = LMEM_PSCLCR_LADSEL_MASK | (0x1u << LMEM_PSCLCR_LCMD_SHIFT);
    /* Set physical address and activate command */
    LMEM_PSCSAR = ((uint_32)ta1 & ~0x03u) | LMEM_PSCSAR_LGO_MASK;
    /* wait until the command completes */
    while (LMEM_PSCSAR & LMEM_PSCSAR_LGO_MASK)
    {}

    ta2[15] = 0x22;
    ta3[15] = 0xDD;
    bar = ta1[15];
}

/* EOF */
