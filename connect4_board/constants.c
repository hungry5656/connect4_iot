/*
 * constants.c
 *
 *  Created on: Mar 2, 2024
 *      Author: hungry5656
 */

#include "constants.h"

unsigned int menuCursorX[] = {MENU_CURSOR_LEFT_X, MENU_CURSOR_RIGHT_X};
unsigned char* levels[] = {STR_LEVEL_EASY, STR_LEVEL_MEDIUM, STR_LEVEL_HARD, STR_LEVEL_EXTREME, STR_LEVEL_NIGHTMARE};
unsigned int colors[] = {GREEN, YELLOW, ORANGE, THEME_RED, ORANGE};
unsigned int maxIdxArray[] = {MAX_IDX_MENU, MAX_IDX_BOARD, MAX_IDX_DIFFICULTY};
char* jsonTokenArrays[] = {"state", "reported", "sender", "messageType", "currTurnIdx", "message"};