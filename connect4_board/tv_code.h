/*
 * tv_code.h
 *
 *  Created on: Feb 8, 2024
 *      Author: hungry5656
 */

#ifndef TV_CODE_H_
#define TV_CODE_H_

#define BUTTON_ONE_CODE     "00100000110111111000100001110111"
#define BUTTON_TWO_CODE     "00100000110111110100100010110111"
#define BUTTON_THREE_CODE   "00100000110111111100100000110111"
#define BUTTON_FOUR_CODE    "00100000110111110010100011010111"
#define BUTTON_FIVE_CODE    "00100000110111111010100001010111"
#define BUTTON_SIX_CODE     "00100000110111110110100010010111"
#define BUTTON_SEVEN_CODE   "00100000110111111110100000010111"
#define BUTTON_EIGHT_CODE   "00100000110111110001100011100111"
#define BUTTON_NINE_CODE    "00100000110111111001100001100111"
#define BUTTON_ZERO_CODE    "00100000110111110000100011110111"
#define BUTTON_MUTE_CODE    "00100000110111110101000010101111"

#define BUTTON_LAST_CODE    "00100000110111110101100010100111"
#define BUTTON_UP_CODE      "00100000110111110100100010110111" // temp 2
#define BUTTON_DOWN_CODE    "00100000110111110001100011100111" // temp 8
#define BUTTON_LEFT_CODE    "00100000110111110010100011010111" // temp 4
#define BUTTON_RIGHT_CODE   "00100000110111110110100010010111" // temp 6
#define BUTTON_OK_CODE      "00100000110111111010100001010111" // temp 5

#define UP_CHAR             'u'
#define DOWN_CHAR           'd'
#define LEFT_CHAR           'l'
#define RIGHT_CHAR          'r'
#define OK_CHAR             'o'
#define BACK_CHAR           'b'
#define INVALID_CHAR        '*'

#define MULTITAP_TIMEOUT    8000 // 500ms

// Standard includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "utils.h"
#include "uart.h"

// Common interface includes
#include "uart_if.h"
#include "i2c_if.h"

char getButtonChar(char* parsedCode, char* result_str);

#endif /* TV_CODE_H_ */
