//! \file onewire_tiva.c
//! \brief Implementation for 1-wire protocol
//! \author Nguyen Trong Phuong
//! \date 2020 April 25

#include "onewire.h"

#include <driverlib/gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/sysctl.h>
#include <utils/uartstdio.h>

static tiva_PortPin_t data_pin;
static uint8_t last_conflict_bit;
static bool is_last_device_found;
static uint8_t ROM[8];

static void holdBus();
static void releaseBus();
static uint8_t sampleBus();
static void writeBit0();
static void writeBit1();
static uint8_t readBit();

static void onewire_initSearchRoutine();
static int8_t onewire_searchNextDevice(uint8_t *address);


//! \brief check the integrity of data with CRC-8
//! \param data pointer to data buffer
//! \param len the size of data buffer
//! \return true or false
//!
bool onewire_checkData(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0;

    while (len--) {
        // code here
    }

    return !crc;
}


//! \brief initialize GPIO pin for 1-wire communication
//! \param pin GPIO port and pin.
//!
void tiva_onewire_init(tiva_PortPin_t pin) {
    data_pin = pin;

    delay_us_init();
}


//! \brief reset 1-wire bus
//!
bool onewire_reset() {
    bool status;
    uint8_t timeout = 100;

    // IntMasterDisable();
    while (timeout--) {
        if (sampleBus()) {
            break;
        }

        // delay 2us
        //SysCtlDelay(2*SysCtlClockGet()/3000000);
        delay_us(2);
    }

    if (timeout == 0) {
        return false;
    }

    holdBus();
    //SysCtlDelay(480*SysCtlClockGet()/3000000);
    delay_us(480);
    releaseBus();
    //SysCtlDelay(70*SysCtlClockGet()/3000000);
    //delay_us(70);
    status = !timing(240, sampleBus);
    //SysCtlDelay(410*SysCtlClockGet()/3000000);
    //delay_us(410);
    // IntMasterEnable();

    return status;
}


bool onewire_getSlaveAddress(uint8_t *address) {
    if (!onewire_reset()) {
        return false;
    }
    onewire_send(READ_ROM);
    onewire_receiveBuffer(address, 8);
    return true;
}


uint8_t onewire_search(uint8_t address_box[][8], uint8_t number) {
    uint8_t counter = 0;
    int8_t status;

    onewire_initSearchRoutine();

    for (uint8_t i = 0; i < number; i++) {
        status = onewire_searchNextDevice(address_box[counter]);
        UARTprintf("status: %d\n", status);
        
        if (status == 1) {
            counter++;
        }
        else {
            break;
        }
    }

    return counter;
}


void onewire_initSearchRoutine() {
    last_conflict_bit = 0;
    is_last_device_found = false;

    for (uint8_t i = 0; i < 8; i++) {
        ROM[i] = 0;
    }
}

//! \return 0: fail, 1: success, -1: invalid ROM
int8_t onewire_searchNextDevice(uint8_t *address) {
    uint8_t bit_A, bit_B;
    uint8_t bit_index = 1;
    uint8_t tmp_bit_index;

    uint8_t conflict_marker = 0;

    if (is_last_device_found) {
        is_last_device_found = false;
        return 3;
    }

    if (!onewire_reset()) {
        return 0;
    }

    onewire_send(SEARCH_ROM);

    while (bit_index <= 64) {
        bit_A = readBit();
        bit_B = readBit();

        // if both of them are '1'
        if (bit_A && bit_B) {
            last_conflict_bit = 0;
            return 4;
        }

        tmp_bit_index = bit_index - 1;
        
        // if either of them is '1'
        if (bit_A || bit_B) {
            if (bit_A) {
                ROM[tmp_bit_index / 8] |= (1 << (tmp_bit_index % 8));
                writeBit1();
            }
            else {
                ROM[tmp_bit_index / 8] &= ~(1 << (tmp_bit_index % 8));
                writeBit0();
            }
        }
        // if both of them are '0'
        else {
            if (bit_index == last_conflict_bit) {
                ROM[tmp_bit_index / 8] |= (1 << (tmp_bit_index % 8));
                writeBit1();
            }
            else if (bit_index > last_conflict_bit) {
                conflict_marker = bit_index;

                ROM[tmp_bit_index / 8] &= ~(1 << (tmp_bit_index % 8));
                writeBit0();
            }
            else {
                uint8_t old_bit_value = ROM[tmp_bit_index / 8] & (1 << (tmp_bit_index % 8));
                if (old_bit_value == 0) {
                    conflict_marker = bit_index;
                    writeBit0();
                }
                else {
                    writeBit1();
                }
            }
        }
        bit_index++;
    }

    last_conflict_bit = conflict_marker;

    if (last_conflict_bit == 0) {
        is_last_device_found = true;
    }

    if (onewire_checkData(ROM, 8)) {
        // copy ROM to address
        for (uint8_t i = 0; i < 8; i++) {
            address[i] = ROM[i];
        }
        return 1; 
    }
    else {
        return 2;
    } 
}


//! \brief select slave with specific address
//! \param address slave's address
//!
bool onewire_select(const uint8_t *address) {
    if (!onewire_reset()) {
        return false;
    }
    onewire_send(MATCH_ROM);
    onewire_sendBuffer(address, 8);
    return true;
}


bool onewire_selectAll() {
    if (!onewire_reset()) {
        return false;
    }
    onewire_send(SKIP_ROM);
    return true;
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
        data |= (readBit() << bit);
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
    // IntMasterDisable();
    holdBus();
    //SysCtlDelay(60*SysCtlClockGet()/3000000);
    delay_us(60);

    releaseBus();
    //SysCtlDelay(10*SysCtlClockGet()/3000000);
    delay_us(10);

    // IntMasterEnable();
}


//! \brief write '1' bit
//!
void writeBit1() {
    // IntMasterDisable();
    holdBus();
    //SysCtlDelay(6*SysCtlClockGet()/3000000);
    delay_us(6);

    releaseBus();
    //SysCtlDelay(64*SysCtlClockGet()/3000000);
    delay_us(64);

    // IntMasterEnable();
}


//! \brief read a bit from bus
//!
uint8_t readBit() {
    // IntMasterDisable();
    holdBus();
    //SysCtlDelay(6*SysCtlClockGet()/3000000);
    delay_us(6);

    releaseBus();
    //SysCtlDelay(9*SysCtlClockGet()/3000000);
    delay_us(9);

    uint8_t bit = timing(45, sampleBus);
    //SysCtlDelay(55*SysCtlClockGet()/3000000);
    delay_us(10);

    // IntMasterEnable();

    return bit;
}


void holdBus() {
    // config GPIO pin as OUTPUT with LOW signal.
    GPIOPinTypeGPIOOutput(data_pin.base, data_pin.pin);
    GPIOPinWrite(data_pin.base, data_pin.pin, 0);

    // alternative
    // GPIOPinTypeGPIOOutputOD(data_pin.base, data_pin.pin);
    // GPIOPadConfigSet(data_pin.base, data_pin.pin,
    //                 GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_OD);
    // GPIOPinWrite(data_pin.base, data_pin.pin, 1);
}


void releaseBus() {
    // config GPIO pin as INPUT w/ PULL-UP
    GPIOPinTypeGPIOInput(data_pin.base, data_pin.pin);
    // GPIOPadConfigSet(data_pin.base, data_pin.pin, 
    //                 GPIO_STRENGTH_2MA,
    //                 GPIO_PIN_TYPE_STD_WPU);

    // alternative
    // GPIOPinTypeGPIOOutputOD(data_pin.base, data_pin.pin);
    // GPIOPadConfigSet(data_pin.base, data_pin.pin,
    //                 GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_OD);
    // GPIOPinWrite(data_pin.base, data_pin.pin, 0);
}


uint8_t sampleBus() {
    return (GPIOPinRead(data_pin.base, data_pin.pin) ? 1 : 0);
}