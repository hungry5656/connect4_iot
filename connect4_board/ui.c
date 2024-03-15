/*
 * ui.c
 *
 *  Created on: Mar 2, 2024
 *      Author: hungry5656
 */

#include "ui.h"
#include "constants.h"

void uiInit() {
    // memcpy(g_ucTxBuff,MASTER_MSG,sizeof(MASTER_MSG));
    SPIReset(GSPI_BASE);
    MAP_SPIConfigSetExpClk(GSPI_BASE,MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                    SPI_IF_BIT_RATE,SPI_MODE_MASTER,SPI_SUB_MODE_0,
                    (SPI_SW_CTRL_CS |
                    SPI_4PIN_MODE |
                    SPI_TURBO_OFF |
                    SPI_CS_ACTIVEHIGH |
                    SPI_WL_8));
    MAP_SPIEnable(GSPI_BASE);
    MAP_SPICSEnable(GSPI_BASE);
    Adafruit_Init();
    fillScreen(BLACK);
}

void uiClose() {
    MAP_SPICSDisable(GSPI_BASE);
}

void drawString(unsigned int x,
                unsigned int y,
                unsigned char texts[],
                size_t strSize,
                unsigned int fontSize,
                unsigned int color) {
    unsigned int i = 0;

    for (i = 0; i < strSize; ++i){
        drawChar(x, y, texts[i], color, BLACK, fontSize);
        x += TXT_CHAR_WIDTHS * fontSize;
    }
}

void _drawMenuHuman() {
    drawRect(4, 38, 56, 56, YELLOW); // frame (4, 38) -> (59, 93)
    fillCircle(31, 50, 7, HUMAN_GREY); // head
    fillRect(18, 59, 28, 8, HUMAN_GREY); // body_top
    fillRect(17, 61, 6, 22, HUMAN_GREY); // arm_left
    fillRect(41, 61, 6, 22, HUMAN_GREY); // arm_right
    fillRect(25, 67, 14, 21, HUMAN_GREY); // body_bottom
}

void _eraseMenuHuman() {
    drawRect(4, 38, 56, 56, BLACK);
    fillCircle(31, 50, 7, BLACK);
    fillRect(18, 59, 28, 8, BLACK);
    fillRect(17, 61, 6, 22, BLACK);
    fillRect(41, 61, 6, 22, BLACK);
    fillRect(25, 67, 14, 21, BLACK);
}

void _drawMenuAI() {
    drawRect(68, 38, 56, 56, RED);
    drawRect(78, 58, 36, 26, AI_GREY); // head
    drawLine(83, 49, 86, 57, AI_GREY); // antenna_left
    drawCircle(83, 47, 2, AI_GREY);
    drawLine(109, 49, 106, 57, AI_GREY); // antenna_right
    drawCircle(109, 47, 2, AI_GREY);
    drawCircle(88, 67, 4, AI_GREY); // eye_left_frame
    fillCircle(88, 67, 3, ORANGE); // eye_left_fill
    drawFastHLine(87, 67, 3, BLACK); // eyeball_left_H
    drawFastVLine(88, 66, 3, BLACK); // eyeball_left_V
    drawCircle(104, 67, 4, AI_GREY); // eye_right_frame
    fillCircle(104, 67, 3, ORANGE); // eye_right_fill
    drawFastHLine(103, 67, 3, BLACK); // eyeball_right_H
    drawFastVLine(104, 66, 3, BLACK); // eyeball_right_V
    drawRect(90, 76, 13, 5, AI_GREY); // mouse_frame
    unsigned int i;
    for (i = 92; i < 101; i += 2) {
        drawFastVLine(i, 77, 3, AI_GREY); // mouse_teeth
    }
}

void _eraseMenuAI() {
    drawRect(68, 38, 56, 56, BLACK);
    drawRect(78, 58, 36, 26, BLACK); // head
    drawLine(83, 49, 86, 57, BLACK); // antenna_left
    drawCircle(83, 47, 2, BLACK);
    drawLine(109, 49, 106, 57, BLACK); // antenna_right
    drawCircle(109, 47, 2, BLACK);
    drawCircle(88, 67, 4, BLACK); // eye_left_frame
    fillCircle(88, 67, 3, BLACK); // eye_left_fill
    drawCircle(104, 67, 4, BLACK); // eye_right_frame
    fillCircle(104, 67, 3, BLACK); // eye_right_fill
    drawRect(90, 76, 13, 5, BLACK); // mouse_frame
    unsigned int i;
    for (i = 92; i < 101; i += 2) {
        drawFastVLine(i, 77, 3, BLACK); // mouse_teeth
    }
}

void drawCursor(unsigned int x, unsigned int y, unsigned int direction, unsigned int color) {
    unsigned int x1 = x, y1 = y, x2 = x, y2 = y, x_c = x, y_c = y;
    switch (direction)
    {
    case CURSOR_UP:
        x1 -= 3;
        x2 += 3;
        y1 += 6;
        y2 = y1;
        y_c += 4;
        break;
    
    case CURSOR_DOWN:
        x1 -= 3;
        x2 += 3;
        y1 -= 6;
        y2 = y1;
        y_c -= 4;
        break;

    case CURSOR_LEFT:
        y1 += 3;
        y2 -= 3;
        x1 += 6;
        x2 = x1;
        x_c += 4;
        break;

    case CURSOR_RIGHT:
        y1 += 3;
        y2 -= 3;
        x1 -= 6;
        x2 = x1;
        x_c -= 4;
        break;
    
    default:
        break;
    }
    
    drawLine(x, y, x1, y1, color);
    drawLine(x, y, x2, y2, color);
    drawLine(x_c, y_c, x1, y1, color);
    drawLine(x_c, y_c, x2, y2, color);
}

void eraseCursor(unsigned int x, unsigned int y, unsigned int direction) {
    drawCursor(x, y, direction, BLACK);
}

void updateCursor(unsigned int stateId, unsigned int oldIdx, unsigned int newIdx) {
    unsigned int x, y, direction, old_x, old_y;
    if (stateId == MENU_STATE) {
        direction = CURSOR_UP;
        old_y = MENU_CURSOR_Y;
        y = MENU_CURSOR_Y;
        old_x = menuCursorX[oldIdx];
        x = menuCursorX[newIdx];
    } else if (stateId == GAME_STATE) {
        direction = CURSOR_DOWN;
        old_y = BOARD_CURSOR_Y;
        y = BOARD_CURSOR_Y;
        old_x = BOARD_HOLE_X + oldIdx * 17;
        x = BOARD_HOLE_X  + newIdx * 17;
    } else {
        direction = CURSOR_RIGHT;
        old_y = LEVEL_CURSOR_Y + oldIdx * 13;
        y = LEVEL_CURSOR_Y + newIdx * 13;
        old_x = LEVEL_CURSOR_X;
        x = LEVEL_CURSOR_X;
    }
    eraseCursor(old_x, old_y, direction);
    drawCursor(x, y, direction, WHITE);
}

void drawMenu() {
    drawString(16, 0, STR_MENU_TITLE, 7, MENU_CHAR_SIZE, THEME_RED);
    drawString(100, 0, "4", 1, MENU_CHAR_SIZE, THEME_YELLOW);
    drawString(43, 18, STR_MENU_SUBTITLE, 7, TXT_CHAR_SIZE, ORANGE); // orig:16
    drawString(19, 26, STR_MENU_AUTHOR, 15, TXT_CHAR_SIZE, HELL_RED); // orig:24
    _drawMenuHuman();
    _drawMenuAI();
    drawString(4, 108, STR_MENU_TIPS_1, 20, TXT_CHAR_SIZE, MAGENTA); // orig:86
    drawString(13, 116, STR_MENU_TIPS_2, 17, TXT_CHAR_SIZE, MAGENTA); // orig:94
    drawCursor(MENU_CURSOR_LEFT_X, MENU_CURSOR_Y, CURSOR_UP, WHITE); // left coordinate (31, 97), right coordinate (95, 97)
}

void eraseMenu() {
    drawString(16, 0, STR_MENU_TITLE, 7, MENU_CHAR_SIZE, BLACK);
    drawString(100, 0, "4", 1, MENU_CHAR_SIZE, BLACK);
    drawString(43, 18, STR_MENU_SUBTITLE, 7, TXT_CHAR_SIZE, BLACK);
    drawString(19, 26, STR_MENU_AUTHOR, 15, TXT_CHAR_SIZE, BLACK);
    _eraseMenuHuman();
    _eraseMenuAI();
    drawString(4, 108, STR_MENU_TIPS_1, 20, TXT_CHAR_SIZE, BLACK);
    drawString(13, 116, STR_MENU_TIPS_2, 17, TXT_CHAR_SIZE, BLACK);
    eraseCursor(MENU_CURSOR_LEFT_X, MENU_CURSOR_Y, CURSOR_UP);
    eraseCursor(MENU_CURSOR_RIGHT_X, MENU_CURSOR_Y, CURSOR_UP);
}

void drawMessage(unsigned char message[], unsigned int color) {
    fillRect(0, 8, 128, 8, BLACK);
    drawString(0, 8, message, strlen(message), TXT_CHAR_SIZE, color); // print P1
}

void drawBoard() {
    fillRect(0, 25, 128, 103, THEME_BLUE); // board frame
    unsigned int x = BOARD_HOLE_X, y = BOARD_HOLE_Y; // starting coordinates of the circle
    for (y = BOARD_HOLE_Y; y < 123; y += 17) {
        for (x = BOARD_HOLE_X; x < 123; x += 17) {
            fillCircle(x, y, HOLE_RADIUS, BLACK);
        }
        x = BOARD_HOLE_X;
    }

    // test:
    // const char* messageArray[] = {MSG_CONNECTING, MSG_MAKE_MOVE, MSG_WAIT, MSG_INVALID, MSG_WIN, MSG_LOSE, MSG_TIE};
    // int i = 0;
    // for (i = 0; i < 7; ++i) {
    //     delay(100);
    //     drawMessage(messageArray[i], HELL_RED);
    // }
}

void drawWinLine(unsigned int row_first, unsigned int col_first, unsigned int row_last, unsigned int col_last) {
    drawLine(BOARD_HOLE_X + 17 * col_first,
             BOARD_HOLE_Y + 17 * row_first,
             BOARD_HOLE_X + 17 * col_last,
             BOARD_HOLE_Y + 17 * row_last,
             WHITE);
}

// type the coordinate to place token
void drawToken(unsigned int row, unsigned int col, unsigned int color) {
    fillCircle(BOARD_HOLE_X + 17 * col, BOARD_HOLE_Y + 17 * row, HOLE_RADIUS, color);
}

// call after the connection is established - receive a command that set the 
void drawPlayerInfo(unsigned char player1[], unsigned char player2[]){
    drawCursor(PLAYER_CURSOR_X, PLAYER_CURSOR_Y, CURSOR_RIGHT, ORANGE); // draw cursor left
    drawString(9, 0, STR_PLAYER1, 2, TXT_CHAR_SIZE, THEME_RED); // print P1
    drawString(27, 0, player1, strlen(player1), TXT_CHAR_SIZE, ORANGE); // print player1 name
    // drawCursor(PLAYER_CURSOR_X + 64, PLAYER_CURSOR_Y, CURSOR_RIGHT, ORANGE); // draw cursor right
    drawString(73, 0, STR_PLAYER2, 2, TXT_CHAR_SIZE, THEME_YELLOW); // print P2
    drawString(91, 0, player2, strlen(player2), TXT_CHAR_SIZE, CYAN); // print player2 name

    drawCursor(BOARD_CURSOR_X, BOARD_CURSOR_Y, CURSOR_DOWN, WHITE); // arrow_down
    // drawMessage(MSG_CONNECTING, HELL_RED); // an example of drawing a message
}

void eraseBoard() {
    fillScreen(BLACK);
}

void drawLevelPage() {
    drawString(19, 13, STR_LEVEL_TITLE_1, 15, TXT_CHAR_SIZE, THEME_BLUE);
    drawString(34, 21, STR_LEVEL_TITLE_2, 10, TXT_CHAR_SIZE, THEME_BLUE);
    
    drawCursor(LEVEL_CURSOR_X, LEVEL_CURSOR_Y, CURSOR_RIGHT, WHITE); // draw cursor
    int i =0;
    for (i = 0; i < 5; ++i){
        unsigned char* currLevel = levels[i];
        unsigned int stringLen = strlen(currLevel);
        drawRect(36, 37 + (i * 13), 59, 11, WHITE); // 13 gap, 2 pixel ->
        drawString((DISPLAY_WIDTH - stringLen * TXT_CHAR_WIDTHS)/2 + 2, 39 + (i * 13), currLevel, stringLen, TXT_CHAR_SIZE, colors[i]);
    }
}

void eraseLevelPage() {
    drawString(19, 13, STR_LEVEL_TITLE_1, 15, TXT_CHAR_SIZE, BLACK);
    drawString(34, 21, STR_LEVEL_TITLE_2, 10, TXT_CHAR_SIZE, BLACK);
    
    int i =0;
    for (i = 0; i < 5; ++i){
        unsigned char* currLevel = levels[i];
        unsigned int stringLen = strlen(currLevel);
        drawCursor(34, 42 + (i * 13), CURSOR_RIGHT, BLACK); // draw cursor
        drawRect(36, 37 + (i * 13), 59, 11, BLACK); // 13 gap, 2 pixel ->
        drawString((DISPLAY_WIDTH - stringLen * TXT_CHAR_WIDTHS)/2 + 2, 39 + (i * 13), currLevel, stringLen, TXT_CHAR_SIZE, BLACK);
    }
}
