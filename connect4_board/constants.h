/*
 * constants.h
 *
 *  Created on: Mar 2, 2024
 *      Author: hungry5656
 */

// Default Parameter
#define TXT_CHAR_SIZE       1
#define TXT_CHAR_WIDTHS     6
#define TXT_CHAR_HEIGHT     8
#define DISPLAY_WIDTH       128
#define STR_PLAYER_SELF     "YOU"
#define STR_PLAYER_HUMAN    "HUMAN"
#define STR_PLAYER_AI       "AI"
#define SPI_IF_BIT_RATE     800000
#define TR_BUFF_SIZE        100

// Common Color
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define	RED             0xF800
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

// Customized Color
#define THEME_RED       0xE8E4
#define THEME_YELLOW    0xFF80
#define THEME_BLUE      0x001F
#define ORANGE          0xEC26
#define HELL_RED        0x88C6
#define HUMAN_GREY      0x4A29
#define AI_GREY         0xB596

// Notification Message
#define MAXIMUM_MSG_SIZE   21
#define MAXIMUM_MSG        "111111111111111111111"
#define MSG_CONNECTING     "CONNECTING..."          // Size: 13
#define MSG_MAKE_MOVE      "MAKE YOUR MOVE"         // Size: 14
#define MSG_WAIT           "WAIT YOUR OPPONENT"     // Size: 18
#define MSG_INVALID        "INVALID MOVE!"          // Size: 13
#define MSG_WIN            "======!YOU WIN!======"  // Size: 21
#define MSG_LOSE           "======&YOU LOSE&====="  // Size: 21
#define MSG_TIE            "======^TIE GAME^====="  // Size: 21

// Cursor Param
// takes 7 * 7 pixels
#define CURSOR_UP       0
#define CURSOR_DOWN     1
#define CURSOR_LEFT     2
#define CURSOR_RIGHT    3

// Menu Param
#define MENU_CHAR_SIZE          2
#define STR_MENU_TITLE          "CONNECT"
#define STR_MENU_SUBTITLE       "MADE BY"
#define STR_MENU_AUTHOR         "SIZHUO & NATHAN"
#define STR_MENU_TIPS_1         "PLEASE SELECT A MODE"
#define STR_MENU_TIPS_2         "TO START THE GAME"
#define MENU_CURSOR_LEFT_X      31
#define MENU_CURSOR_RIGHT_X     95
#define MENU_CURSOR_Y           97

// Board Param
#define HOLE_RADIUS             7
#define BOARD_HOLE_X            12
#define BOARD_HOLE_Y            33
#define BOARD_CURSOR_X          63
#define BOARD_CURSOR_Y          23
#define PLAYER_CURSOR_X         6
#define PLAYER_CURSOR_Y         3
#define STR_PLAYER1             "P1"
#define STR_PLAYER2             "P2"

// Difficulty Page Param
#define STR_LEVEL_TITLE_1       "SELECT LEVEL OF"
#define STR_LEVEL_TITLE_2       "DIFFICULTY"
#define STR_LEVEL_EASY          "EASY"
#define STR_LEVEL_MEDIUM        "MEDIUM"
#define STR_LEVEL_HARD          "HARD"
#define STR_LEVEL_EXTREME       "EXTREME"
#define STR_LEVEL_NIGHTMARE     "NIGHTMARE"
#define LEVEL_CURSOR_X          34
#define LEVEL_CURSOR_Y          42
#define LEVELS_GAP              13 // pixel

// UI page id
// #define PAGE_MENU               0
// #define PAGE_BOARD              1
// #define PAGE_DIFFICULTY         2

#define MIN_IDX_MENU            0

#define MAX_IDX_MENU            1
#define MAX_IDX_BOARD           6
#define MAX_IDX_DIFFICULTY      4

// state id
#define MENU_STATE              0
#define GAME_STATE              1
#define DIFFICULTY_STATE        2
#define BACK_NUM                10

#define HORIZONTAL_MODE         0
#define VERTICAL_MODE           1

// To Server CMD
#define START_GAME              100
#define QUIT_GAME               101

// To Shadow CMD
#define GAME_CLOSE              0
#define SERVER_DOWN             1
#define SERVER_REFUSED          2
#define INVALID_INPUT           3
#define OPPONENT_QUIT           4
#define SET_PLAYER0             5
#define SET_PLAYER1             6

#define JSON_MSG_1              "{\"state\": {\r\n\"reported\": {\r\n\"sender\": 0,\r\n\"messageType\": \""
#define JSON_MSG_2              "\",\r\n\"shadowName\": \""
#define JSON_MSG_3              "\",\r\n\"message\": "
#define JSON_MSG_4              "\r\n}}}\r\n\r\n"
#define MSG_TYPE_CMD            "cmd"
#define MSG_TYPE_MOVE           "move"


extern unsigned int menuCursorX[];
extern unsigned char* levels[];
extern unsigned int colors[];
extern unsigned int maxIdxArray[];
extern char* jsonTokenArrays[];
