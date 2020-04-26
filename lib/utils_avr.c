#include <avr/io.h>
#include <stdint.h>
#include "utils_avr.h"


uint16_t showMemory() {

	// extern unsigned int __data_start;
	// extern unsigned int __data_end;
	// extern unsigned int __bss_start;
	// extern unsigned int __bss_end;
	extern unsigned int __heap_start;
	extern void *__brkval;

	// printf(".data start: %p\n", &__data_start);
	// printf(".data end: %p\n", &__data_end);
	// printf(".bss start: %p\n", &__bss_start);
	// printf(".bss end: %p\n", &__bss_end);
	// printf("heap start: %p\n", &__heap_start);
	// printf("heap end: %p\n", __brkval);
	// printf("SP: %p\n", (void*)SP);
	// printf("RAMEND: %p\n", (void*)RAMEND);

	return SP - ((uint16_t)(__brkval == 0) ? 
								(uint16_t)&__heap_start : (uint16_t)__brkval);
}