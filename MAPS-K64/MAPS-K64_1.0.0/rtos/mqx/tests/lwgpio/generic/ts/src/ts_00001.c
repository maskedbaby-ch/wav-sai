#include "ts_00001.h"
#include "tc_00001.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

EU_TEST_SUITE_BEGIN(ts_00001)
	EU_TEST_CASE_ADD(tc_00001,  "tc_00001_all_api")
EU_TEST_SUITE_END(ts_00001)

EU_TEST_REGISTRY_BEGIN()
	EU_TEST_SUITE_ADD(ts_00001, "ts_00001_lwgpio_swonly")
EU_TEST_REGISTRY_END()

TASK_TEMPLATE_STRUCT MQX_template_list[] = 
{
	{0x100, main_task, 512, 5, "main_task", MQX_AUTO_START_TASK, 0L, 0},
	{    0,         0,   0, 0,           0,                   0, 0L, 0}
};

void main_task(uint32_t initial_data)
{
	eunit_mutex_init();
	EU_RUN_ALL_TESTS(eunit_tres_array);
	eunit_test_stop();
}
