#include <mqx.h>
#include <bsp.h> 

#include "EUnit.h"
#include "eunit_crit_secs.h"

/* Task IDs */
#define MAIN_TASK   5
#define USR_TASK    6

static void main_task(uint32_t);
static void usr_task(uint32_t);

#define MULTI_NOP _ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();_ASM_NOP();

const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{ 
   /* Task Index,   Function,   Stack,  Priority,        Name,                          Attributes, Param, Time Slice */
    { MAIN_TASK,   main_task,    1500,         8, "main_task",                 MQX_AUTO_START_TASK,     0,          0 },
    { USR_TASK,     usr_task,    1500,         9,  "usr_task", MQX_AUTO_START_TASK | MQX_USER_TASK,     0,          0 },
    { 0 }
};

/* one read-write var */
USER_RW_ACCESS volatile static int g_urw = 10;
/* one read-only var */
USER_RO_ACCESS volatile static int g_uro = 20;
/* one no-access var */
USER_NO_ACCESS volatile static int g_uno = 30;
/* one no-prefix var */
volatile static int g_udef = 40;

/* error counters */
USER_RW_ACCESS static int error = 0;
USER_RW_ACCESS static int old_error = 0;

void test_1_1(void)
{
    uint32_t val;
    old_error = error;
    val = g_urw;
    
    MULTI_NOP
    EU_ASSERT_REF_TC_MSG(test_1_1, old_error == error, "Test #1_1: should equal")
}

void test_1_2(void)
{
    uint32_t val;
    old_error = error;
    val++;

    MULTI_NOP
    EU_ASSERT_REF_TC_MSG(test_1_2, old_error == error, "Test #1_2: should equal")
}

void test_1_3(void)
{
    uint32_t val;
    old_error = error;
    g_urw = val;

    MULTI_NOP
    EU_ASSERT_REF_TC_MSG(test_1_3, old_error == error, "Test #1_3: should equal")
}

void test_2_1(void)
{
    uint32_t val;
    old_error = error;
    val = g_uro;

    MULTI_NOP
    EU_ASSERT_REF_TC_MSG(test_2_1, old_error == error, "Test #2_1: should equal")
}

void test_2_2(void)
{
    uint32_t val;
    old_error = error;
    val++;

    MULTI_NOP
    EU_ASSERT_REF_TC_MSG(test_2_2, old_error == error, "Test #2_2: should equal")
}

void test_2_3(void)
{
    uint32_t val;
    old_error = error;
    g_uro = val;
    
    MULTI_NOP
    EU_ASSERT_REF_TC_MSG(test_2_3, old_error != error, "Test #2_3: should not equal")
}

void test_3_1(void)
{
    uint32_t val;
    old_error = error;
    val = g_uno;
    
    MULTI_NOP
    EU_ASSERT_REF_TC_MSG(test_3_1, old_error != error, "Test #3_1: should not equal")
}

void test_3_2(void)
{
    uint32_t val;
    old_error = error;
    val++;

    MULTI_NOP
    EU_ASSERT_REF_TC_MSG(test_3_2, old_error == error, "Test #3_2: should equal")
}

void test_3_3(void)
{
    uint32_t val;
    old_error = error;
    g_uno = val;

    MULTI_NOP
    EU_ASSERT_REF_TC_MSG(test_3_3, old_error != error, "Test #3_3: should not equal")
}

void test_4_1(void)
{
    uint32_t val;
    old_error = error;
    val = g_udef;
    
    MULTI_NOP
    EU_ASSERT_REF_TC_MSG(test_4_1, old_error == error, "Test #4_1: should equal")
}

void test_4_2(void)
{
    uint32_t val;
    old_error = error;
    val++;

    MULTI_NOP
    EU_ASSERT_REF_TC_MSG(test_4_2, old_error == error, "Test #4_2: should equal")
}

void test_4_3(void)
{
    uint32_t val;
#if MQX_DEFAULT_USER_ACCESS_RW    
    old_error = error;
    g_udef = val;

    MULTI_NOP
    EU_ASSERT_REF_TC_MSG(test_4_3, old_error == error, "Test #4_3: should equal")
#else    
    old_error = error;
    g_udef = val;

    MULTI_NOP
    EU_ASSERT_REF_TC_MSG(test_4_3, old_error != error, "Test #1: should not equal")
#endif
}

EU_TEST_SUITE_BEGIN(suite_usermode_g_urw)
    EU_TEST_CASE_ADD(test_1_1, "val = g_urw"),
    EU_TEST_CASE_ADD(test_1_2, "val++"),
    EU_TEST_CASE_ADD(test_1_3, "g_urw = val"),
EU_TEST_SUITE_END(suite_usermode_g_urw)

EU_TEST_SUITE_BEGIN(suite_usermode_g_uro)
    EU_TEST_CASE_ADD(test_2_1, "val = g_uro"),
    EU_TEST_CASE_ADD(test_2_2, "val++"),
    EU_TEST_CASE_ADD(test_2_3, "g_uro = val"),
EU_TEST_SUITE_END(suite_usermode_g_uro)

EU_TEST_SUITE_BEGIN(suite_usermode_g_uno)
    EU_TEST_CASE_ADD(test_3_1, "val = g_uno"),
    EU_TEST_CASE_ADD(test_3_2, "val++"),
    EU_TEST_CASE_ADD(test_3_3, "g_uno = val"),
EU_TEST_SUITE_END(suite_usermode_g_uno)

EU_TEST_SUITE_BEGIN(suite_usermode_g_udef)
    EU_TEST_CASE_ADD(test_4_1, "val = g_udef"),
    EU_TEST_CASE_ADD(test_4_2, "val++"),
    EU_TEST_CASE_ADD(test_4_3, "g_udef = val"),
EU_TEST_SUITE_END(suite_usermode_g_udef)

EU_TEST_REGISTRY_BEGIN()
    EU_TEST_SUITE_ADD(suite_usermode_g_urw, "Test of usermode mem_access rw"),
    EU_TEST_SUITE_ADD(suite_usermode_g_uro, "Test of usermode mem_access ro"),
    EU_TEST_SUITE_ADD(suite_usermode_g_uno, "Test of usermode mem_access no"),
    EU_TEST_SUITE_ADD(suite_usermode_g_udef, "Test of usermode mem_access udef"),
EU_TEST_REGISTRY_END()

void fault_isr
(
  /*  [IN]  the parameter passed by the kernel */
  void   *parameter
)
{
    uint32_t *p;
    
    error++;
    
    if (SCB_CFSR & SCB_CFSR_PRECISERR_MASK) {
        p = (uint32_t*)__get_PSP();
        p[6] += 2;      // PC in stack, temporary solution, estimate 16bit instruction
    }
    
    SCB_CFSR |= (SCB_CFSR_PRECISERR_MASK | SCB_CFSR_IMPRECISERR_MASK);
}

static void main_task
(
  uint32_t initial_data
)
{
    _int_install_isr(4, fault_isr, NULL);
    _int_install_isr(5, fault_isr, NULL);
    
    SCB_SHCSR |= SCB_SHCSR_MEMFAULTENA_MASK | SCB_SHCSR_BUSFAULTENA_MASK;

    _ASM_NOP();
    _task_block();
    _ASM_NOP();
}

static void usr_task
(
  uint32_t initial_data
)
{   
    eunit_mutex_init();
    EU_RUN_ALL_TESTS(eunit_tres_array);
    _ASM_NOP();
    eunit_test_stop();
}
