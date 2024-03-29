/*
 * c4.h
 *
 *  Created on: Mar 7, 2024
 *      Author: Nathan D'Cruz
 */

#ifndef C4_H_
#define C4_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "uart_if.h"
#include "constants.h"

#include "messages.h"
#include "tls_simplink.h"

// MACROS
// OPTIONS
#define BOARD_WIDTH     7 // MAX 255
#define BOARD_HEIGHT    6 // MAX 255

// Game state
#define GAME_INACTIVE   0
#define GAME_ACTIVE     1
#define GAME_OVER       2

// Board state
#define CELL_EMPTY      0

// Player state
#define NO_PLAYER_ID    0
#define TIE_PLAYER_ID   255
#define AI_PLAYER_ID    36

// Null pointer
// #define NULL 0
// Max error string length
#define MAX_ERR_LEN 1024


// "CLASSES" LOL
/*
 * A struct to encapsulate the game state
 */
struct C4_GAME_OBJ;
struct C4_PLAYER;

// Alias `struct C4_GAME_OBJ` to `connect4_t`
typedef struct C4_GAME_OBJ  connect4_t;

// Alias `struct C4_PLAYER` to `player_t`
typedef struct C4_PLAYER    player_t;

// PRIVATE MEMBERS

/*
 * Standard integer max/min methods
 */
// static int imax(int a, int b);
// static int imin(int a, int b);

/*
 * Sent a formatted error message over UART
 * Messages must be MAX_ERR_LEN char or less in length
 */
// static void fmt_err_c4();

/*
 * Modify game state for a move
 */
// static bool do_move(player_t* player, unsigned int selected_column);

/*
 * Transition from player 1 turn to player 2
 */
// static void turn_transition();

/*
 * Play a turn - run when it is the local player's turn
 */
// static unsigned int play_turn();

/*
 * Wait a turn - run when it is the non-local player's turn
 */
// static unsigned int wait_turn();

/*
 * Check the end-conditions for the game.
 * Returns 1 if there is a winner
 * Returns 2 if there is a tie
 * Returns 0 otherwise
 */
// static unsigned int check_game_over_c4(unsigned int move_col, player_t* last_move_player);

// PUBLIC MEMBERS
/*
 * Initialize a C4_GAME_OBJ
 */
bool init_c4t(unsigned int isAI, unsigned int levelIdx, int iTLSSockID);

/*
 * Check if a game is initialized.
 */
bool        game_initialized_c4();

/*
 * Check if a game is active.
 * Returns 0 for INACTIVE, 1 for ACTIVE
 */
bool        game_active_c4();

/*
 * Start a game of Connect 4
 */
bool        start_game_c4();

/*
 * Reset the Connect 4 game
 * Requires that game state be GAME_INACTIVE or GAME_OVER
 */
bool        reset_game_c4();

// void set_board_state();

#endif /* C4_H_ */
