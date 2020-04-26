//! \file onewire.h
//! \brief Implementation for 1-wire protocol
//! \author Nguyen Trong Phuong
//! \date 2020 April 25

#ifndef __ONEWRITE__
#define __ONEWRITE__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "utils_avr.h"
#include "utils_tiva.h"

void avr_onewire_init(avr_PortPin_t pin);

void tiva_onewire_init();


bool onewire_reset();

void onewire_send(uint8_t data);

#ifdef __cplusplus
}
#endif

#endif