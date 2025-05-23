#include <basic_types.h>
/* Realtek test code start */

//TODO: remove section when combine BD and BF
//8710C uses the linker symbol to determine the SRAM region and PSRAM region
#if ((defined CONFIG_PLATFORM_8195A) || (defined CONFIG_PLATFORM_8711B))
#include "section_config.h"
SRAM_BF_DATA_SECTION
static unsigned char ucHeap[ configTOTAL_HEAP_SIZE ];
#endif


#if (defined CONFIG_PLATFORM_8195A)
HeapRegion_t xHeapRegions[] = {
	{ (uint8_t *)0x10002300, 0x3D00 },	// Image1 recycle heap
	{ ucHeap, sizeof(ucHeap) }, 		// Defines a block from ucHeap
#if 0
	{ (uint8_t *)0x301b5000, 300 * 1024 }, // SDRAM heap
#endif
	{ NULL, 0 } 							// Terminates the array.
};

#elif (defined CONFIG_PLATFORM_8711B)
#include "rtl8710b_boot.h"
extern BOOT_EXPORT_SYMB_TABLE boot_export_symbol;
HeapRegion_t xHeapRegions[] = {
	{ 0, 0},	// Image1 reserved ,length will be corrected in pvPortMalloc()
	{ ucHeap, sizeof(ucHeap) }, 	// Defines a block from ucHeap
#if (CONFIG_ENABLE_RDP == 0)
	{ (uint8_t *)0x1003f000, 0x1000},	// RDP reserved
#endif
	{ NULL, 0 } 					// Terminates the array.
};
#elif (defined CONFIG_PLATFORM_8710C)

#include <stdio.h>
#include "memory.h"
#include "hal_api.h"
#include "hal_lpcram.h"
#include "hal_sys_ctrl.h"
#if defined(__ICCARM__)
extern uint8_t SramEnd[];
extern uint8_t RAM_BSS$$Limit[];
extern uint32_t RAM_STACK$$Base[];
extern uint8_t EramEnd[];
extern uint8_t ERAM_BSS$$Limit[];
#ifdef CONFIG_PLATFORM_Z2PLUS
extern uint8_t SramExtendStart[];
extern uint8_t SramExtendEnd[];
#endif

#undef configTOTAL_HEAP_SIZE
#undef configTOTAL_HEAP_ext_SIZE

#define configTOTAL_HEAP0_SIZE ((uint32_t)RAM_STACK$$Base - ((uint32_t)RAM_BSS$$Limit))
#define HEAP0_START (uint8_t *)RAM_BSS$$Limit

#define configTOTAL_HEAP1_SIZE ((uint32_t)EramEnd - ((uint32_t)ERAM_BSS$$Limit))
#define HEAP1_START (uint8_t *)ERAM_BSS$$Limit

#ifdef CONFIG_PLATFORM_Z2PLUS
#define configTOTAL_HEAP2_SIZE ((uint32_t)SramExtendEnd - ((uint32_t)SramExtendStart))
#define HEAP2_START (uint8_t *)SramExtendStart
#endif

#elif defined(__GNUC__)

#undef configTOTAL_HEAP_SIZE
#undef configTOTAL_HEAP_ext_SIZE

extern uint8_t __sram_end__[];
extern uint8_t __eram_end__[];
extern uint8_t __bss_end__[];
extern uint8_t __eram_bss_end__[];
#ifdef CONFIG_PLATFORM_Z2PLUS
extern uint8_t __sram_extend_start__[];
extern uint8_t __sram_extend_end__[];
#endif

#define configTOTAL_HEAP0_SIZE ((uint32_t)__sram_end__ - ((uint32_t)__bss_end__))
#define HEAP0_START (uint8_t*)__bss_end__

#define configTOTAL_HEAP1_SIZE ((uint32_t)__eram_end__ - ((uint32_t)__eram_bss_end__))
#define HEAP1_START (uint8_t*)__eram_bss_end__

#ifdef CONFIG_PLATFORM_Z2PLUS
#define configTOTAL_HEAP2_SIZE ((uint32_t)__sram_extend_end__ - ((uint32_t)__sram_extend_start__))
#define HEAP2_START (uint8_t*)__sram_extend_start__
#endif

#endif

/**/
HeapRegion_t xHeapRegions[] = {
	{ 0, 0 }, // Defines a block from SRAM heap, but size will be determined by linker to use the rest of free SRAM as heap
	{ 0, 0 }, 	// Defines a block from PSRAM heap
#ifdef CONFIG_PLATFORM_Z2PLUS
	{ 0, 0 }, 	// Defines a block from SRAM heap
#endif
	{ NULL, 0 }                     // Terminates the array.
};
#else
#error NOT SUPPORT CHIP
#endif
/* Realtek test code end */

/*No OS in secure world, for RTK modified FreeRTOS 10.0.1*/
#if defined(CONFIG_BUILD_SECURE) && (CONFIG_BUILD_SECURE == 1)
__WEAK void vTaskSuspendAll(void)
{
	__disable_irq();
}

__WEAK BaseType_t xTaskResumeAll(void)
{
	__enable_irq();
	return pdTRUE;
}

void vApplicationMallocFailedHook(void)
{
	printf("Malloc failed [free secure heap size: %d]\r\n", xPortGetFreeHeapSize());
	__disable_irq();
	for (;;);
}

NS_ENTRY void secure_heap_init(void)
{
#if defined(CONFIG_PLATFORM_8710C)
	xHeapRegions[ 0 ].xSizeInBytes = configTOTAL_HEAP0_SIZE;
	xHeapRegions[ 0 ].pucStartAddress = (uint8_t *)HEAP0_START;
	if (hal_get_flash_port_cfg() != FLASH_PORTB) { // not a flash MCM package, so PSRAM on B port is possible
		if (hal_lpcram_is_valid() == HAL_OK) {
			xHeapRegions[ 1 ].xSizeInBytes = configTOTAL_HEAP1_SIZE;
			xHeapRegions[ 1 ].pucStartAddress = (uint8_t *)HEAP1_START;
		}
	}
#endif
	vPortDefineHeapRegions(xHeapRegions);
}

NS_ENTRY size_t xPortGetFreeHeapSizeNSC(void)
{
	return xPortGetFreeHeapSize();
}
#endif
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
NS_ENTRY void secure_heap_init(void);
#endif
void os_heap_init(void)
{
#if (defined CONFIG_PLATFORM_8711B)
	xHeapRegions[ 0 ].xSizeInBytes = (uint32_t)((uint8_t *)0x10005000 - (uint8_t *)boot_export_symbol.boot_ram_end);
	xHeapRegions[ 0 ].pucStartAddress = (uint8_t *)boot_export_symbol.boot_ram_end;
#elif defined(CONFIG_PLATFORM_8710C)
	xHeapRegions[ 0 ].xSizeInBytes = configTOTAL_HEAP0_SIZE;
	xHeapRegions[ 0 ].pucStartAddress = (uint8_t *)HEAP0_START;
#ifdef CONFIG_PLATFORM_Z2PLUS
	if (hal_get_chip_ver() >= CHIP_f_CUT) {	//support extension sram
		xHeapRegions[ 1 ].xSizeInBytes = configTOTAL_HEAP2_SIZE;
		xHeapRegions[ 1 ].pucStartAddress = (uint8_t *)HEAP2_START;
	}
#endif
	if (hal_get_flash_port_cfg() != FLASH_PORTB) { // not a flash MCM package, so PSRAM on B port is possible
		if (hal_lpcram_is_valid() == HAL_OK) {
#ifdef CONFIG_PLATFORM_Z2PLUS
			if (xHeapRegions[ 1 ].xSizeInBytes > 0) {
				xHeapRegions[ 2 ].xSizeInBytes = configTOTAL_HEAP1_SIZE;
				xHeapRegions[ 2 ].pucStartAddress = (uint8_t *)HEAP1_START;
			} else
#endif
			{
				xHeapRegions[ 1 ].xSizeInBytes = configTOTAL_HEAP1_SIZE;
				xHeapRegions[ 1 ].pucStartAddress = (uint8_t *)HEAP1_START;
			}
		}
	}
#endif
	vPortDefineHeapRegions(xHeapRegions);
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	secure_heap_init();
#endif
}

#if defined(CONFIG_MATTER) && CONFIG_MATTER
size_t xPortGetTotalHeapSize(void)
{
	if (hal_get_flash_port_cfg() != FLASH_PORTB) // not a flash MCM package, so PSRAM on B port is possible
	{
		if (hal_lpcram_is_valid() == HAL_OK)
		{
#ifdef CONFIG_PLATFORM_Z2PLUS
			return configTOTAL_HEAP0_SIZE + configTOTAL_HEAP1_SIZE + configTOTAL_HEAP2_SIZE;
#else
			return configTOTAL_HEAP0_SIZE + configTOTAL_HEAP1_SIZE;
#endif
		}
	}
	else
	{
#ifdef CONFIG_PLATFORM_Z2PLUS
		return configTOTAL_HEAP0_SIZE + configTOTAL_HEAP2_SIZE;
#else
		return configTOTAL_HEAP0_SIZE;
#endif
}
/*-----------------------------------------------------------*/
#endif
