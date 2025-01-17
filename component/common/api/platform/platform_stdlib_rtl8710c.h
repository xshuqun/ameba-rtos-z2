#ifndef PLATFORM_STDLIB_RTL8710C_H
#define PLATFORM_STDLIB_RTL8710C_H

#ifdef __cplusplus
extern "C" {
#endif

#undef USE_RTL_ROM_CLIB
#define USE_RTL_ROM_CLIB	0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "basic_types.h"

#if 0 //because sscanf in rom lib is not working!
#include "rt_printf.h" //stdio_printf_stubs
#if !defined(CONFIG_BUILD_SECURE)
#undef sscanf
#define sscanf(buf,...)  (stdio_printf_stubs.rt_sscanf)(buf, __VA_ARGS__)
#endif
#endif

#define BUFFERED_PRINTF         0

#if defined(__ICCARM__)
#if defined(BUFFERED_PRINTF) && (BUFFERED_PRINTF == 1)
#undef printf //libc may redefine to puts/putchar
extern int buffered_printf(const char *fmt, ...);
#define printf                buffered_printf
#endif
#endif

#if defined (__GNUC__)
#define CONFIG_PLATFORM_AMEBA_X 1
char *__wrap_strtok(char *s, char const *ct);
#undef printf //libc may redefine to puts/putchar
#if defined(BUFFERED_PRINTF) && (BUFFERED_PRINTF == 1)
extern int buffered_printf(const char *fmt, ...);
#define printf                buffered_printf
#else
extern int __wrap_printf(const char *fmt, ...);
#define printf                __wrap_printf
#endif

#undef malloc //libc will redefine to malloc_r
#ifndef PLATFORM_OHOS
#define malloc pvPortMalloc
#endif
#undef strtok //libc will redefine to strtok_r
#define strtok __wrap_strtok
#endif

//This section is not defined in 8710c's section_config.h. Be compatible with 8195A
#define SRAM_BD_DATA_SECTION SECTION(".sram.bss")

#ifdef __cplusplus
}
#endif

#endif //PLATFORM_STDLIB_RTL8710C_H
