/*
 * messages.h
 *
 *  Created on: Mar 12, 2024
 *      Author: hungry5656
 */

#ifndef MESSAGES_H_
#define MESSAGES_H_

// standard library
#include <string.h>
#include <stdlib.h>

// sdk json library
#include "json/jsmn.h"

// external library
#include "aws_credential.h"
#include "constants.h"

int isValidResponse(char* msgReceived);
int parseServerMsg(char* msgReceived, unsigned int* mainContent, unsigned int currTurnIdx, unsigned int isCMD);
int buildToServerMsg(char* msgSend, unsigned int mainContent, unsigned int isCMD);

#endif /* MESSAGES_H_ */
