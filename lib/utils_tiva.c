//! \file utils_tiva.c
//! \brief Utility functions, structs for Tiva c.
//! \author Nguyen Trong Phuong (aka trongphuongpro)
//! \date 2020 April 8

#include "utils_tiva.h"

#include <stdbool.h>

#include "driverlib/sysctl.h"
#include "driverlib/systick.h"


volatile uint16_t counter = 0;


static void systick_isr();

void systick_isr() {
    if (counter > 0) {
        counter--;
    }
}

void delay_ms_init() {
    SysTickPeriodSet(SysCtlClockGet()/1000); // period = 1 ms
    SysTickIntRegister(systick_isr);
    SysTickEnable();
}

void delay_ms(uint16_t ms) {
    counter = ms;

    while (counter);
}


void delay_us_init() {
    SysTickPeriodSet(SysCtlClockGet()/1000000); // period = 1 us
    SysTickIntRegister(systick_isr);
    SysTickEnable();
}

void delay_us(uint16_t us) {
    counter = us;

    while (counter);
}

uint8_t timing(uint16_t us, uint8_t (*f)(void)) {
    counter = us;
    uint8_t status = 1;

    while (counter) {
        status = f();

        if (status == 0) {
            break;
        }
    }

    return status;
}

/********************* End of File *******************************************/