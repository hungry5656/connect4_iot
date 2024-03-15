/*
 * usr_input.c
 *
 *  Created on: Mar 7, 2024
 *      Author: hungry5656
 */

#include "usr_input.h"

volatile int systick_cnt = 0; // Systick counter
volatile int systick_cnt_diff = -1; // Systick counter
volatile int idx_cnt = 0;
volatile char command_data[CMD_NUM_BITS + 1]; // command bits;
unsigned long ulStatus;

// flag bits
volatile int startDecodeFlag = 0; // set to 1 when a falling edge is detected
volatile int verifiedStartFlag = 0; // set to 1 if first 9ms is verified
volatile int verifiedStartFlag2 = 0; // set to 1 if seccond 4.5ms is verified
volatile int signalLevel = 1; // represent the signal level at Pin64, 1 is high
volatile int intFlag = 0; // set to one during interrupt handler
volatile int finishFlag = 0; // if the parsing is finished, set the finish flag to 1
volatile int inputFlag = 0; // if input function is called, enable the interrupt
volatile int errorFlag = 0;


static void ResetFlag(void) {
    // signalLevel = 1;
    startDecodeFlag = 0;
    verifiedStartFlag = 0;
    verifiedStartFlag2 = 0;
}
static inline void SysTickReset(void) {
    HWREG(NVIC_ST_CURRENT) = 1;

    // clear the global count variable
    systick_cnt = 0;
}
static void GPIOA1IntHandler(void) {
    unsigned long ulStatus;

    ulStatus = MAP_GPIOIntStatus (GPIOA1_BASE, true);
    intFlag = 1;

    if (finishFlag == 1){
        // nop
    } else if (inputFlag == 0){
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

void setupInterrupt() {
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
}

unsigned int getUsrInput(unsigned int stateId) {
    char result_str[MAX_STRING_LENGTH+1];
    int cmd_cnt = 0, cursorIdx, minIdx, maxIdx, currIdx;
    unsigned int modeCursor = HORIZONTAL_MODE; // 0 - horizontal mode or 1 - vertical mode

    minIdx = MIN_IDX_MENU;
    maxIdx = maxIdxArray[stateId];
    currIdx = MIN_IDX_MENU;

    // give the limit provided with such stateId
    if (stateId == GAME_STATE) {
        currIdx = 3;
    }
    if (stateId == DIFFICULTY_STATE) {
        modeCursor = VERTICAL_MODE;
    }

    cursorIdx = currIdx;

    // enable interrupt
    inputFlag = 1;

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
            char thisChar = getButtonChar(command_data, result_str);
            Report("\n\rDEBUG: cmd %d binary: %s; It is %s", cmd_cnt, command_data, result_str);
            unsigned int isUpdated = 0;
            if (thisChar == OK_CHAR) {
                // confirm the input, and exit the input stage
                ResetFlag();
                finishFlag = 0;
                inputFlag = 0;
                return currIdx;

            } else if (thisChar == BACK_CHAR) {
                // go back logic, returning BACK_NUM - the special index
                // TODO
                ResetFlag();
                finishFlag = 0;
                inputFlag = 0;
                return BACK_NUM;

            } else if (thisChar == INVALID_CHAR) {
                // nop
            } else if (modeCursor == HORIZONTAL_MODE){
                isUpdated = 1;
                if (thisChar == LEFT_CHAR) {
                    currIdx = MAX(minIdx, currIdx - 1);
                } else if (thisChar == RIGHT_CHAR) {
                    currIdx = MIN(maxIdx, currIdx + 1);
                }
            } else if (modeCursor == VERTICAL_MODE){
                isUpdated = 1;
                if (thisChar == UP_CHAR) {
                    currIdx = MAX(minIdx, currIdx - 1);
                } else if (thisChar == DOWN_CHAR) {
                    currIdx = MIN(maxIdx, currIdx + 1);
                }
            }
            if (isUpdated == 1) {
                // update cursor on the oled
                Report("\n\rDEBUG: current cursor Idx %d", currIdx);
                updateCursor(stateId, cursorIdx, currIdx);
                cursorIdx = currIdx;
            }
            ResetFlag();
            finishFlag = 0;
        } else {
            // read from accelerometer

        }
    }
}
