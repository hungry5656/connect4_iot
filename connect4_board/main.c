//*****************************************************************************
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************



#include <stdio.h>

// Simplelink includes
#include "simplelink.h"

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
#include "common.h"
#include "uart_if.h"
#include "tls_simplink.h"

#define MAX_URI_SIZE 128
#define URI_SIZE MAX_URI_SIZE + 1

// external libraries
#include "tv_code.h"
#include "aws_credential.h"

// previous part MACRO
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX_STRING_LENGTH    168
#define SYSTICK_RELOAD_VAL   8000UL // set to 0.1ms
#define CMD_NUM_BITS         32
// end previous part MACRO

// #define DATA1 "{\"state\": {\r\n\"desired\" : {\r\n\"color\" : \"green\"\r\n}}}\r\n\r\n"
// #define DATA1 "{\"state\": {\"desired\" : {\"color\" : \"green\"}}}"


//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
volatile unsigned long  g_ulStatus = 0;//SimpleLink Status
unsigned long  g_ulPingPacketsRecv = 0; //Number of Ping Packets received
unsigned long  g_ulGatewayIP = 0; //Network Gateway IP address
unsigned char  g_ucConnectionSSID[SSID_LEN_MAX+1]; //Connection SSID
unsigned char  g_ucConnectionBSSID[BSSID_LEN_MAX]; //Connection BSSID
signed char    *g_Host = SERVER_NAME;
SlDateTime g_time;
#if defined(ccs) || defined(gcc)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

// From Lab 3
volatile int systick_cnt = 0; // Systick counter
volatile int systick_cnt_diff = -1; // Systick counter
volatile int idx_cnt = 0;
volatile char command_data[CMD_NUM_BITS + 1]; // command bits;

// flag bits
volatile int startDecodeFlag = 0; // set to 1 when a falling edge is detected
volatile int verifiedStartFlag = 0; // set to 1 if first 9ms is verified
volatile int verifiedStartFlag2 = 0; // set to 1 if seccond 4.5ms is verified
volatile int signalLevel = 1; // represent the signal level at Pin64, 1 is high
volatile int intFlag = 0; // set to one during interrupt handler
volatile int finishFlag = 0; // if the parsing is finished, set the finish flag to 1
volatile int errorFlag = 0;
// End From Lab 3

//*****************************************************************************
//                 GLOBAL VARIABLES -- End: df
//*****************************************************************************


//****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
//****************************************************************************
// static long WlanConnect();
// static int set_time();
static void BoardInit(void);
// static long InitializeAppVariables();
// static int tls_connect();
// static int connectToAccessPoint();
// static int http_post(int, char*);
// static int http_get(int iTLSSockID);
// char* jsonWrapMessage(char*);


//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void BoardInit(void) {
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

// From Lab 3
static void ResetFlag(void) {
    // signalLevel = 1;
    startDecodeFlag = 0;
    verifiedStartFlag = 0;
    verifiedStartFlag2 = 0;
}
static inline void SysTickReset(void) {
    // any write to the ST_CURRENT register clears it
    // after clearing it automatically gets reset without
    // triggering exception logic
    // see reference manual section 3.2.1
    HWREG(NVIC_ST_CURRENT) = 1;

    // clear the global count variable
    systick_cnt = 0;
}
static void GPIOA1IntHandler(void) {
    //Report("\n\rGPIO Interrupt");
    unsigned long ulStatus;

    ulStatus = MAP_GPIOIntStatus (GPIOA1_BASE, true);
    intFlag = 1;

    if (finishFlag == 1){
        // nop
    } else if (signalLevel == 1) {
        // falling edge
        signalLevel = 0;
        if (startDecodeFlag == 0) {
            // start of decode process
            startDecodeFlag = 1;
            systick_cnt_diff = systick_cnt;
            SysTickReset();

        } else {
            if (verifiedStartFlag == 1 && systick_cnt > 40 && systick_cnt < 50) {
                verifiedStartFlag2 = 1;
                SysTickReset();

            } else if (verifiedStartFlag2 == 1) {
                if (systick_cnt >= 6 && systick_cnt <= 15){
                    command_data[idx_cnt] = 48; // character 0
                    idx_cnt++;
                } else if (systick_cnt >= 18 && systick_cnt < 27) {
                    command_data[idx_cnt] = 49; // character 1
                    idx_cnt++;
                } else {
                    // Error: not recognized bit
                    errorFlag = 1;
                    ResetFlag();
                }
                SysTickReset();
            } else {
                // the signal does not match
                ResetFlag();
            }
        }

    } else {
        //rising edge
        signalLevel = 1;
        if (startDecodeFlag == 1) {
            if (systick_cnt > 85 && systick_cnt < 95) {
                verifiedStartFlag = 1;
                SysTickReset();
            } else if (verifiedStartFlag2 == 1) {
//                if (systick_cnt > 30) {
//                    ResetFlag();
//                }
                // ignore
            } else {
                // not a normal signal
                ResetFlag();
            }
        }
//        ResetFlag();
        if (idx_cnt == 32) {
            // the parse is finished, reset the flag and everything
            idx_cnt = 0;
            ResetFlag();
            finishFlag = 1;
        }
    }

    MAP_GPIOIntClear(GPIOA1_BASE, ulStatus);        // clear interrupts on GPIOA1
    intFlag = 0;
}

static void systick_int_handler(void) {
    systick_cnt++;
}
// End From Lab 3


//*****************************************************************************
//
//! Main 
//!
//! \param  none
//!
//! \return None
//!
//*****************************************************************************
void main() {
    // From Lab 3
    char result_str[MAX_STRING_LENGTH+1];
    char messageToSend[MAX_STRING_LENGTH+1];
    int msg_idx = 0;
    unsigned long ulStatus;
    // End From Lab 3

    long lRetVal = -1;
    //
    // Initialize board configuration
    //
    BoardInit();

    PinMuxConfig();

    // From Lab 3
    PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);
    //

    InitTerm();
    ClearTerm();
    UART_PRINT("My terminal works!\n\r");

    // From Lab 3
    // interrupt config
    GPIOIntRegister(GPIOA1_BASE, GPIOA1IntHandler); // register handler
    GPIOIntTypeSet(GPIOA1_BASE, 0x2, GPIO_BOTH_EDGES);    // GPIO 64 both edge

    ulStatus = MAP_GPIOIntStatus (GPIOA1_BASE, false);
    GPIOIntClear(GPIOA1_BASE, ulStatus); // clear interrupts on GPIOA1
    GPIOIntEnable(GPIOA1_BASE, 0x2); // enable interrupt

    // systick config
    SysTickPeriodSet(SYSTICK_RELOAD_VAL);
    SysTickIntRegister(systick_int_handler);
    SysTickEnable();
    SysTickIntEnable();
    int cmd_cnt = 0;

    int lastCharIdx = -1, currentCharIdx = 0;
    // End From Lab 3

    //Connect the CC3200 to the local access point
    lRetVal = connectToAccessPoint();
    //Set time so that encryption can be used
    lRetVal = set_time();
    if(lRetVal < 0) {
        UART_PRINT("Unable to set time in the device");
        LOOP_FOREVER();
    }

    //Connect to the website with TLS encryption
    lRetVal = tls_connect();
    if(lRetVal < 0) {
        ERR_PRINT(lRetVal);
    }
    //http_post(lRetVal);


    // From Lab 3
    while(1)
    {
        // if (errorFlag == 1) {
        //     Report("\n\rAn Error Occurred!");
        //     errorFlag = 0;
        // }

        if (finishFlag == 1) {
            MAP_UtilsDelay(4000000);
            cmd_cnt++;
            command_data[32] = '\0';
            whichButton(command_data, result_str);
            Report("\n\rcmd %d binary: %s; It is %s", cmd_cnt, command_data, result_str);
            int prevCharIdx = currentCharIdx;
            int prevLastCharIdx = lastCharIdx;
            char thisChar = getNextChar(command_data, &lastCharIdx, &currentCharIdx, systick_cnt_diff);

            if (thisChar == ENTER_CHAR) {
                // enter char
                messageToSend[msg_idx] = '\0';

                msg_idx = 0; // reset the msg_idx to 0;

                // send to uart
                //cleanTextOnSend();
                //transmitTextMessage(messageToSend);

                // send to IoT
                UART_PRINT("\n\r MESSAGE:\n\r");
                UART_PRINT(messageToSend);
                char jsonContent[200];
                sprintf(jsonContent, "{\"state\": {\"desired\" : {\"message\" : \"%s\"}}}", messageToSend);

                UART_PRINT("\n\r JSON CONTENT: \n\r");
                UART_PRINT(jsonContent);

                http_post(lRetVal, jsonContent);
                //break

            } else if (thisChar == DELETE_CHAR) {
                // delete char
                //deleteChar();
                msg_idx = MIN(0, msg_idx - 1);

            } else if (thisChar == INVALID_CHAR) {
                // nop
            } else {
                if (prevLastCharIdx == lastCharIdx && prevCharIdx != currentCharIdx) {
                    //deleteChar();
                    msg_idx = MIN(0, msg_idx - 1);
                }
                messageToSend[msg_idx] = thisChar;
                msg_idx++;
                //writeChar(thisChar);
            }
            ResetFlag();
            finishFlag = 0;
        }
    }
    // End From Lab 3

    sl_Close(lRetVal);
    sl_Stop(SL_STOP_TIMEOUT);
    //LOOP_FOREVER();
}
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
