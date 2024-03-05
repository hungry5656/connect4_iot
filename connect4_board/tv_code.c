/*
 * tv_code.c
 *
 *  Created on: Feb 15, 2024
 *      Author: hungry5656
 */
#include "tv_code.h"

char buttonKeyArray[12][33] = {
    BUTTON_ZERO_CODE,
    BUTTON_ONE_CODE,   
    BUTTON_TWO_CODE,
    BUTTON_THREE_CODE,
    BUTTON_FOUR_CODE,
    BUTTON_FIVE_CODE,
    BUTTON_SIX_CODE,
    BUTTON_SEVEN_CODE,
    BUTTON_EIGHT_CODE,
    BUTTON_NINE_CODE,
    BUTTON_LAST_CODE,
    BUTTON_MUTE_CODE};

char charValueArray[12][5] = {
    {1, ' '},
    {0},
    {3, 'a', 'b', 'c'},
    {3, 'd', 'e', 'f'},
    {3, 'g', 'h', 'i'},
    {3, 'j', 'k', 'l'},
    {3, 'm', 'n', 'o'},
    {4, 'p', 'q', 'r', 's'},
    {3, 't', 'u', 'v'},
    {4, 'w', 'x', 'y', 'z'},
    {1, ENTER_CHAR}, // last is enter
    {1, DELETE_CHAR}, // mute is delete
};

void whichButton(volatile char* parsedCode, volatile char* result_str) {
    char* nvParsedCode = (char*) parsedCode;
    char* nvResult_Str = (char*) result_str;

    if (strcmp(nvParsedCode, BUTTON_ZERO_CODE) == 0) {
        strcpy(nvResult_Str, "Button Zero");
    } else if (strcmp(nvParsedCode, BUTTON_ONE_CODE) == 0) {
        strcpy(nvResult_Str, "Button One");
    } else if (strcmp(nvParsedCode, BUTTON_TWO_CODE) == 0) {
        strcpy(nvResult_Str, "Button Two");
    } else if (strcmp(nvParsedCode, BUTTON_THREE_CODE) == 0) {
        strcpy(nvResult_Str, "Button Three");
    } else if (strcmp(nvParsedCode, BUTTON_FOUR_CODE) == 0) {
        strcpy(nvResult_Str, "Button Four");
    } else if (strcmp(nvParsedCode, BUTTON_FIVE_CODE) == 0) {
        strcpy(nvResult_Str, "Button Five");
    } else if (strcmp(nvParsedCode, BUTTON_SIX_CODE) == 0) {
        strcpy(nvResult_Str, "Button Six");
    } else if (strcmp(nvParsedCode, BUTTON_SEVEN_CODE) == 0) {
        strcpy(nvResult_Str, "Button Seven");
    } else if (strcmp(nvParsedCode, BUTTON_EIGHT_CODE) == 0) {
        strcpy(nvResult_Str, "Button Eight");
    } else if (strcmp(nvParsedCode, BUTTON_NINE_CODE) == 0) {
        strcpy(nvResult_Str, "Button Nine");
    } else if (strcmp(nvParsedCode, BUTTON_LAST_CODE) == 0) {
        strcpy(nvResult_Str, "Button Last");
    } else if (strcmp(nvParsedCode, BUTTON_MUTE_CODE) == 0) {
        strcpy(nvResult_Str, "Button Mute");
    } else {
        strcpy(nvResult_Str, "Button NULL");
    }
}

int getButtonIdx(char* parsedCode){
    int retIdx = -1; // set default idx to be unrecognized;
    int i;
    for (i = 0; i < 12; ++i) {
        if (strcmp(parsedCode, buttonKeyArray[i]) == 0) {
            retIdx = i;
            break;
        }
    }
    return retIdx;
}
/*
 * currentCharIdx ranges from 0 to 1,3,4
 */
char getNextChar(volatile char* parsedCode, int* lastCharIdx, int* currentCharIdx, int idx_cnt) {
    char* nvParsedCode = (char*) parsedCode;
    int idxInCharArray = getButtonIdx(nvParsedCode);
    if ((idxInCharArray == -1) || (idxInCharArray == 1)) {
        // if the command cannot be recognized or being recognize as button 1, return invalid_char
        *lastCharIdx = -1;
        return INVALID_CHAR;
    }
    if (*lastCharIdx != idxInCharArray || idx_cnt > MULTITAP_TIMEOUT) {
        // different button is typed
        (*currentCharIdx) = 0;
        int updatedColIdx = *currentCharIdx + 1;
        *lastCharIdx = idxInCharArray;
        return charValueArray[idxInCharArray][updatedColIdx];
    }
    if (idx_cnt <= MULTITAP_TIMEOUT) {
        *currentCharIdx = (*currentCharIdx + 1) % charValueArray[idxInCharArray][0];
        return charValueArray[idxInCharArray][*currentCharIdx + 1];
    }
}
