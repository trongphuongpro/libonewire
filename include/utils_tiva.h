//! \file utils_tiva.h
//! \brief Utility functions, structs for Tiva c.
//! \author Nguyen Trong Phuong (aka trongphuongpro)
//! \date 2020 April 8

#ifndef __UTILS_TIVA__
#define __UTILS_TIVA__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

//! \brief Data type that contains GPIO port and pin.
//!
typedef struct tiva_PortPin {
    uint32_t base; //!< Memory base of GPIO port
    uint8_t pin; //!< GPIO pin
} tiva_PortPin_t;


//! \brief Initialize SysTick.
//! \return nothing.
//!
void delay_ms_init();


//! \brief Initialize SysTick.
//! \return nothing.
//!
void delay_us_init();


//! \brief Delay in millisecond.
//! \param ms millisecond.
//! \return nothing.
//!
void delay_ms(uint16_t ms);


void delay_us(uint16_t us);

uint8_t timing(uint16_t us, uint8_t (*f)(void));

#ifdef __cplusplus
}
#endif

#endif

/********************* End of File *******************************************/