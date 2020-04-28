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
//#include "utils_tiva.h"

//! \brief initialize GPIO pin for 1-wire communication
//! \param pin GPIO port and pin.
//!
void avr_onewire_init(avr_PortPin_t pin);

void tiva_onewire_init();


//! \brief get address of the next slave on multi-drop bus
//! \param address slave's address
//!
uint8_t onewire_search(uint8_t address_box[][8]);


//! \brief get address of the slave on single-drop bus
//! \param address slave's address
//!
bool onewire_getSlaveAddress(uint8_t *address);


//! \brief select slave with specific address
//! \param address slave's address
//!
bool onewire_select(const uint8_t *address);

//! \brief select all slaves on bus
//!
bool onewire_selectAll(void);


//! \brief reset 1-wire bus
//!
bool onewire_reset(void);


//! \brief send 1 byte to slave
//! \param data 1 byte data
//!
void onewire_send(uint8_t data);


//! \brief send a buffer to slave
//! \param buffer pointer to data buffer
//! \param len the size of data buffer
//!
void onewire_sendBuffer(const void *buffer, uint16_t len);


//! \brief receive 1 byte from slave
//! \return 1 byte
//!
uint8_t onewire_receive(void);


//! \brief receive a buffer from slave
//! \param buffer pointer to data buffer
//! \param len the size of data buffer
//!
void onewire_receiveBuffer(void *buffer, uint16_t len);

//! \brief check the integrity of data with CRC-8
//! \param data pointer to data buffer
//! \param len the size of data buffer
//! \return true or false
//!
bool onewire_checkData(const uint8_t* data, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif