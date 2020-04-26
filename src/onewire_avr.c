//! \file onewire_avr.c
//! \brief Implementation for 1-wire protocol
//! \author Nguyen Trong Phuong
//! \date 2020 April 25

#include "onewire.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

static avr_PortPin_t data_pin;

static void holdBus();
static void releaseBus();
static bool sampleBus();
static void writeBit0();
static void writeBit1();

//! \brief initialize GPIO pin for 1-wire communication
//! \param pin GPIO port and pin.
//!
void avr_onewire_init(avr_PortPin_t pin) {
    data_pin = pin;
}


//! \brief reset 1-wire bus
//!
bool onewire_reset() {
    bool status;
    uint8_t timeout = 100;

    cli();
    while (timeout) {
        if (sampleBus()) {
            break;
        }
        _delay_us(2);
    }

    if (timeout == 0) {
        return false;
    }

    holdBus();
    _delay_us(480);
    releaseBus();
    _delay_us(70);
    status = !sampleBus();
    _delay_us(410);
    sei();

    return status;
}

//! \brief send 1 byte to slave
//! \param data 1 byte data
//!
void onewire_send(uint8_t data) {
    for (uint8_t bit = 0; bit < 8; bit++) {
        if (data & (1 << bit)) {
            writeBit1();
        }
        else {
            writeBit0();
        }
    }
}


//! \brief write '0' bit
//!
void writeBit0() {
    cli();
    holdBus();
    _delay_us(60);
    releaseBus();
    _delay_us(10);
    sei();
}


//! \brief write '1' bit
//!
void writeBit1() {
    cli();
    holdBus();
    _delay_us(6);
    releaseBus();
    _delay_us(64);
    sei();
}


//! \brief read a bit from bus
//!
bool readBit() {
    holdBus();
    _delay_us(6);
    releaseBus();
    _delay_us(9);

    bool bit = sampleBus();
    _delay_us(55);

    return status;
}


void holdBus() {
    // config GPIO pin as OUTPUT with LOW signal.
    *(data_pin.ddr) |= (1 << data_pin.pin);
    *(data_pin.port) &= ~(1 << data_pin.pin);
}


void releaseBus() {
    // config GPIO pin as INPUT with built-in pull-up resistor
    *(data_pin.ddr) &= ~(1 << data_pin.pin);
    *(data_pin.port) |= (1 << data_pin.pin);
}


bool sampleBus() {
    return (data_pin.value & (1 << data_pin.pin));
}