/*
 * ui.h
 *
 *  Created on: Mar 2, 2024
 *      Author: hungry5656
 */

#ifndef UI_H_
#define UI_H_

#include "hw_types.h"
#include "hw_ints.h"
#include "hw_nvic.h"
#include "spi.h"
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
#include "common.h"
#include "uart_if.h"

// external libraries
#include "usr_input.h"
#include "tv_code.h"
#include "aws_credential.h"

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1351.h"
#include "glcdfont.h"
#include "string.h"

void uiInit();
void uiClose();
void drawString(unsigned int x, unsigned int y, unsigned char texts[], size_t strSize, unsigned int fontSize, unsigned int color);
void drawCursor(unsigned int x, unsigned int y, unsigned int direction, unsigned int color);
void eraseCursor(unsigned int x, unsigned int y, unsigned int direction);
void updateCursor(unsigned int stateId, unsigned int oldIdx, unsigned int newIdx);
void drawMenu();
void eraseMenu();
void drawMessage(unsigned char message[], unsigned int color);
void drawBoard();
void drawWinLine(unsigned int row_first, unsigned int col_first, unsigned int row_last, unsigned int col_last);
void drawToken(unsigned int row, unsigned int col, unsigned int color);
void drawPlayerInfo(unsigned char player1[], unsigned char player2[]);
void eraseBoard();
void drawLevelPage();
void eraseLevelPage();

#endif /* UI_H_ */
