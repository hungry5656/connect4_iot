/*
 * tv_code.c
 *
 *  Created on: Feb 15, 2024
 *      Author: hungry5656
 */
#include "tv_code.h"

char getButtonChar(char* parsedCode, char* result_str){
    char retChar;
    if (strcmp(parsedCode, BUTTON_UP_CODE) == 0) {
        retChar = UP_CHAR;
        strcpy(result_str, "Button Up");
    } else if (strcmp(parsedCode, BUTTON_DOWN_CODE) == 0) {
        retChar = DOWN_CHAR;
        strcpy(result_str, "Button Down");
    } else if (strcmp(parsedCode, BUTTON_LEFT_CODE) == 0) {
        retChar = LEFT_CHAR;
        strcpy(result_str, "Button Left");
    } else if (strcmp(parsedCode, BUTTON_RIGHT_CODE) == 0) {
        retChar = RIGHT_CHAR;
        strcpy(result_str, "Button Right");
    } else if (strcmp(parsedCode, BUTTON_OK_CODE) == 0) {
        retChar = OK_CHAR;
        strcpy(result_str, "Button Ok");
    } else if (strcmp(parsedCode, BUTTON_MUTE_CODE) == 0) {
        retChar = BACK_CHAR;
        strcpy(result_str, "Button Back");
    } else {
        retChar = '*';
        strcpy(result_str, "Button NULL");
    }
    return retChar;
}
