/*
 * messages.c
 *
 *  Created on: Mar 12, 2024
 *      Author: hungry5656
 */


#include "messages.h"

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

int isValidResponse(char* msgReceived) {
    char *idx = strstr(msgReceived, "sender\":");
    printf("%ld\n%c\n", idx, idx - msgReceived + 8, msgReceived[idx - msgReceived + 8]);
    // 31
    // if (msgReceived[31] != '1') {
    if (msgReceived[idx - msgReceived + 8] != '1') {
        return 0;
    }
    return 1;
}

int parseServerMoveMsg(char* msgReceived, unsigned int* nextMoveIdx, unsigned int currTurnIdx) {
    int i, tokenNum;
    jsmn_parser parser;
    jsmntok_t jsonTokens[128];
    char testStr[15];

    jsmn_init(&parser);
    tokenNum = jsmn_parse(&parser, msgReceived, strlen(msgReceived), jsonTokens, sizeof(jsonTokens) / sizeof(jsonTokens[0]));

    if (tokenNum < 1) {
        // ERROR: fail to parse
        return -1;
    }

    // find reported jsonToken first
    int isParsing = 0;

    for (i = 1; i < tokenNum; ++i) {
        if (isParsing == 0 && jsoneq(msgReceived, &jsonTokens[i], "reported") == 0) {
            // start parsing
            isParsing = 1;
            continue;
        }
        if (isParsing == 1) {
            if (jsoneq(msgReceived, &jsonTokens[i], "sender") == 0) {
                sprintf(testStr, "%.*s", jsonTokens[i + 1].end - jsonTokens[i + 1].start,
             msgReceived + jsonTokens[i + 1].start);
                UART_PRINT("%s\r\n", testStr);
            } else if (jsoneq(msgReceived, &jsonTokens[i], "messageType") == 0) {
                sprintf(testStr, "%.*s", jsonTokens[i + 1].end - jsonTokens[i + 1].start,
             msgReceived + jsonTokens[i + 1].start);
                UART_PRINT("%s\r\n", testStr);
            } else if (jsoneq(msgReceived, &jsonTokens[i], "currTurnIdx") == 0) {
                sprintf(testStr, "%.*s", jsonTokens[i + 1].end - jsonTokens[i + 1].start,
             msgReceived + jsonTokens[i + 1].start);
                UART_PRINT("%s\r\n", testStr);
            } else if (jsoneq(msgReceived, &jsonTokens[i], "message") == 0) {
                sprintf(testStr, "%.*s", jsonTokens[i + 1].end - jsonTokens[i + 1].start,
             msgReceived + jsonTokens[i + 1].start);
                UART_PRINT("%s\r\n", testStr);
                break;
            }
        }
    }
    return 0;
}

int buildToServerMsg(char* msgSend, unsigned int mainContent, unsigned int isCMD) {
    strcat(msgSend, JSON_MSG_1);
    if (isCMD == 0) {
        strcat(msgSend, MSG_TYPE_MOVE);
    } else {
        strcat(msgSend, MSG_TYPE_CMD);
    }
    strcat(msgSend, JSON_MSG_2);
    strcat(msgSend, SHADOW_NAME);
    strcat(msgSend, JSON_MSG_3);
    char numStr[5];
    // ltoa(mainContent, numStr, 10);
    sprintf(numStr, "%d", mainContent);
    strcat(msgSend, numStr);
    strcat(msgSend, JSON_MSG_4);
    return 0;
}
