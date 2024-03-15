/*
 * usr_input.h
 *
 *  Created on: Mar 7, 2024
 *      Author: hungry5656
 */

#ifndef USR_INPUT_H_
#define USR_INPUT_H_


#include <stdio.h>

//Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_nvic.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "utils.h"
#include "uart.h"
#include "gpio.h"

//Common interface includes
#include "pin_mux_config.h"
#include "gpio_if.h"
#include "uart_if.h"
#include "i2c_if.h"

// external libraries
#include "tv_code.h"
#include "constants.h"

// previous part MACRO
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MAX_STRING_LENGTH    168
#define SYSTICK_RELOAD_VAL   8000UL // set to 0.1ms
#define CMD_NUM_BITS         32

void setupInterrupt();
unsigned int getUsrInput(unsigned int stateId);

#endif /* USR_INPUT_H_ */
