#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include "main.h"
#include "example_entry.h"
#include "fault_handler.h"

extern void console_init(void);

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	/* Read last fault data and redeclare fault handler to use ram code */
	read_last_fault_log();
	fault_handler_override(fault_log, bt_log);

#if defined(CONFIG_BUILD_SECURE) || defined(CONFIG_BUILD_NONSECURE)
	fault_handler_set_ns_func();
	secure_main();
#endif

	/* Initialize log uart and at command service */
	console_init();

	/* pre-processor of application example */
	pre_example_entry();

	/* wlan intialization */
	wlan_network();

	/* Execute application example */
	example_entry();

	/* Enable Schedule, Start Kernel */
	vTaskStartScheduler();

	/* Should NEVER reach here */
	return 0;
}