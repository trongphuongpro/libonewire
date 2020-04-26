//! \file onewire_avr.c
//! \brief Implementation for 1-wire protocol
//! \author Nguyen Trong Phuong
//! \date 2020 April 25

#include "onewire.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/crc16.h>

#define SEARCH_ROM  0xF0
#define READ_ROM    0x33
#define MATCH_ROM   0x55
#define SKIP_ROM    0xCC

static avr_PortPin_t data_pin;

static void holdBus();
static void releaseBus();
static bool sampleBus();
static void writeBit0();
static void writeBit1();
static bool readBit();


//! \brief check the integrity of data with CRC-8
//! \param data pointer to data buffer
//! \param len the size of data buffer
//! \return true or false
//!
bool onewire_checkData(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0;

    while (len--) {
        crc = _crc_ibutton_update(crc, *data++);
    }

    return !crc;
}


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
    while (timeout--) {
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


void onewire_readROMSingle(uint8_t *address) {
    onewire_send(READ_ROM);
    onewire_receiveBuffer(address, 8);
}

//! \brief select slave with specific address
//! \param address slave's address
//!
void onewire_select(const uint8_t *address) {
    onewire_send(MATCH_ROM);
    onewire_sendBuffer(address, 8);
}


void onewire_selectAll() {
    onewire_send(SKIP_ROM);
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


//! \brief send a buffer to slave
//! \param buffer pointer to data buffer
//! \param len the size of data buffer
//!
void onewire_sendBuffer(const void *buffer, uint16_t len) {
    const uint8_t *data = (const uint8_t*)buffer;

    for (int i = 0; i < len; i++) {
        onewire_send(data[i]);
    }
}


//! \brief receive 1 byte from slave
//! \return 1 byte
//!
uint8_t onewire_receive() {
    uint8_t data = 0;

    for (uint8_t bit = 0; bit < 8; bit++) {
        if (readBit()) {
            data |= (1 << bit);
        }
    }

    return data;
}


//! \brief receive a buffer from slave
//! \param buffer pointer to data buffer
//! \param len the size of data buffer
//!
void onewire_receiveBuffer(void *buffer, uint16_t len) {
    uint8_t *data = (uint8_t*)buffer;

    for (int i = 0; i < len; i++) {
        data[i] = onewire_receive();
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

    return bit;
}


void holdBus() {
    // config GPIO pin as OUTPUT with LOW signal.
    *(data_pin.ddr) |= (1 << data_pin.pin);
    *(data_pin.port) &= ~(1 << data_pin.pin);
}


void releaseBus() {
    // config GPIO pin as tri-state
    *(data_pin.ddr) &= ~(1 << data_pin.pin);
}


bool sampleBus() {
    return (*(data_pin.value) & (1 << data_pin.pin));
}