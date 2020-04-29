/**
 * @file utils_avr.h
 * @brief Utility functions for AVR programming.
 * @author Nguyen Trong Phuong (aka trongphuongpro)
 * @date 2020 Feb, 15
 */

#ifndef __UTILS_AVR__
#define __UTILS_AVR__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct avr_PortPin {
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *value;
    uint8_t pin;
} avr_PortPin_t;


/**
 * @brief Show free SRAM.
 * @return free SRAM in bytes.
 */
uint16_t showMemory(void);


#ifdef __cplusplus
}
#endif

#endif /* __UTILS_AVR__ */