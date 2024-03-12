/*
 * c4.c
 *
 *  Created on: Mar 7, 2024
 *      Author: Nathan D'Cruz
 *      Adapted from connec4.py by Henry Sun
 */

#include "c4.h"

// GLOBALS DECLARATION
struct C4_PLAYER {
    player_t* opponent;
    uint8_t id;
}; // player_t

struct C4_GAME_OBJ {
    // GAME STATE
    player_t*   winner;
    uint8_t     status;

    // BOARD STATE
    uint8_t     board[BOARD_HEIGHT][BOARD_WIDTH];
    uint8_t     lowest_empty_col_slot[BOARD_WIDTH];
    long        num_moves;

    // PLAYER STATE
    player_t    player1;
    player_t    player2;
    player_t*   local_player;
    player_t*   player_turn;

}; // connect4_t

static connect4_t GAME;

static bool GAME_INIT = false;
static const int  MAX_MOVES = BOARD_WIDTH * BOARD_HEIGHT;

static player_t TIE_PLAYER;
static player_t NO_PLAYER;

// PRIVATE METHODS
static int imax(int a, int b) { return (a == b ? a : (a > b ? a : b)); }
static int imin(int a, int b) { return (a == b ? a : (a < b ? a : b)); }

static void fmt_err_c4(const char* err_msg) {
    // Enforce max length
    char err_msg_buf[MAX_ERR_LEN];
    strncpy(err_msg_buf, err_msg, MAX_ERR_LEN);

    // Format message
    char fmt_err_msg[MAX_ERR_LEN + 3]; // Accommodate \n\r\0
    sprintf(fmt_err_msg, "%s\n\r\0", err_msg);

    // Write to UART
    Report(fmt_err_msg);
}

static void init_player(player_t* player, player_t* opponent, uint8_t player_id) {
    // TODO: consider local player vs. nonlocal player
    player->opponent = opponent;
    player->id = player_id;
}

static bool init_c4t(uint8_t player1_id, uint8_t player2_id, bool player1_is_local) {
    // Input validation
    switch(player1_id) {
    case(NO_PLAYER.id):
    case(TIE_PLAYER.id):
    case(AI_PLAYER_ID):
        fmt_err_msg("Cannot init: Invalid player1_id. Did you use a reserved player id?");
        return true;
    }

    switch(player2_id) {
    case(NO_PLAYER.id):
    case(TIE_PLAYER.id):
    case(AI_PLAYER_ID):
        fmt_err_msg("Cannot init: Invalid player2_id. Did you use a reserved player id?");
        return true;
    }

    // Initialize player state
    if (GAME_INIT) {
        fmt_err_msg("Cannot init: connect4 game object is already initialized.");
        return 0;
    }

    // Dummy players for special player states
    NO_PLAYER.id = NO_PLAYER_ID;           // For when player is "null"
    NO_PLAYER.opponent = NULL;
    TIE_PLAYER.id = TIE_PLAYER_ID;        // For when the game is a tie
    TIE_PLAYER.opponent = NULL;

    init_player(&GAME.player1, &GAME.player2, player1_id);
    init_player(&GAME.player2, &GAME.player1, player2_id);

    if (player1_is_local) {
        GAME.local_player = &GAME.player1;
    } else {
        GAME.local_player = &GAME.player2;
    }

    GAME.player_turn = &NO_PLAYER;

    // Initialize board
    int col;
    int row;
    for (col = 0; col < BOARD_WIDTH; col++) {
        for (row = 0; row < BOARD_HEIGHT; row++) {
            GAME.board[row][col] = 0;
        }
        GAME.lowest_empty_col_slot[col] = BOARD_HEIGHT - 1;
    }
    GAME.num_moves = 0;


    // Initialize game state
    GAME.winner = &NO_PLAYER;
    GAME.status = GAME_INACTIVE;

    // Set game initialization flag
    GAME_INIT = true;

    return 0;
}

static bool set_winner(bool is_tie){
    if (!(GAME.status == GAME_ACTIVE)) {
        fmt_err_c4("Cannot set winner: no active game");
        return true;
    }

    GAME.winner = is_tie ? &TIE_PLAYER : GAME.player_turn;

    // TODO: winner post-logic
    //...

    return false;
}

static bool do_move(player_t* player, uint8_t selected_column) {
    uint8_t row = GAME.lowest_empty_col_slot[selected_column];

    uint8_t cell = GAME.board[row][selected_column];
    if (!(cell == CELL_EMPTY)) {
        fmt_err_c4("Cannot do move: cell already in use");
        return true;
    }

    GAME.board[row][selected_column] = player->id;
    GAME.lowest_empty_col_slot[selected_column]--;
    GAME.num_moves++;

    return false;
}

static void turn_transition() { GAME.player_turn = GAME.player_turn->opponent; }
static bool check_board_full() { return (GAME.num_moves >= MAX_MOVES); }

static uint8_t play_turn() {
    uint8_t selected_column; // TODO: poll for input
                             // Listen for game quit
    bool valid_move = false;

    while (!valid_move) {
        selected_column = 2; // DUMMY VALUE until above is satisfied

        // use unsigned negative overflow to detect when the column is full
        valid_move = !(GAME.lowest_empty_col_slot[selected_column] > BOARD_HEIGHT - 1);
    }

    do_move(GAME.player_turn, selected_column);
    return selected_column;
}

static uint8_t wait_turn() {
    // Poll AWS for state update
}

// PUBLIC METHODS
connect4_t get_game_c4(){
   if (!GAME_INIT) { init_c4t(); }
   return GAME;
}

bool game_active_c4() { return ( GAME_INIT && (GAME.status == GAME_ACTIVE) ); }
bool game_initalized_c4() { return (GAME_INIT); }

bool start_game_c4() {
    if (!GAME_INIT) {
        fmt_err_c4("Game has not been initialized. Please call get_game_c4() first.");
        return true;
    } else if (game_active_c4()) {
        fmt_err_c4("There is already a game in progress.");
        return true;
    }

    GAME.status = GAME_ACTIVE;

    GAME.player_turn = &GAME.player1;
    bool first_turn = true;

    // Game loop
    bool game_over = false;
    bool is_tie = false;

    while (!game_over) {
        if (!first_turn) { turn_transition(); }

        if (GAME.player_turn == GAME.local_player) {
            uint8_t last_move = play_turn();
            first_turn = false;

            int game_over_status = check_game_over_c4(last_move, GAME.player_turn);
            game_over = game_over_status > 0;
            is_tie = game_over_status > 1;
        } else {
            wait_turn();
        }
    }

    if (game_over) {
        set_winner(is_tie);
        GAME.status = GAME_OVER;
    }

    return false;
}

bool reset_game_c4() {
    if (!GAME_INIT) { fmt_err_msg("Cannot reset game: game is not initialized, please call init_game_c4() instead."); return true; }
    if (GAME.status == GAME_ACTIVE) { fmt_err_msg("Cannot reset game: game is currently active."); return true; };

    GAME_INIT = false;
    init_game_c4();

    return true;
}

uint8_t check_game_over_c4(uint8_t move_col, player_t* last_move_player) {
    uint8_t move_row = GAME.lowest_empty_col_slot[move_col] + 1;

    uint8_t min_row_index = imax(move_col - 3, 0);
    uint8_t max_row_index = imin(move_col + 3, BOARD_HEIGHT - 1);
    uint8_t max_col_index = imax(move_row - 3, 0);
    uint8_t min_col_index = imin(move_row + 3, BOARD_WIDTH - 1);

    int min_left_diag[2] = {imax(move_col - 3, move_col), imin(move_row + 3, BOARD_WIDTH - 1)};
    int max_left_diag[2] = {imin(move_col + 3, BOARD_HEIGHT - 1), imax(move_row - 3, 0)};
    int min_right_diag[2] = {imin(move_col + 3, move_col), imin(move_row + 3, BOARD_WIDTH - 1)};
    int max_right_diag[2] = {imax(move_col - 3, 0), imax(move_row - 3, 0)};

    // Horizontal
    int count = 0;
    int s;
    for(s = min_row_index; s <= max_row_index; s++) {
        int pos_owner = GAME.board[move_row][s];

        if (pos_owner == last_move_player->id) {
            count++;
        } else {
            count = 0;
        }

        if (count == 4) {
            // TODO: draw line
            return 1;
        }
    }

    // Vertical
    count = 0;
    for(s = max_col_index; s <= min_col_index; s++) {
        int pos_owner = GAME.board[s][move_col];

        if (pos_owner == last_move_player->id) {
            count++;
        } else {
            count = 0;
        }

        if (count == 4) {
            // TODO: draw line
            return 1;
        }
    }

    // Left diagonal
    int row = move_row;
    int col = move_col;
    count = 0;
    int up = 0; // unused??
    int down_count = 0; // used for drawing the line

    while (row > -1 && col > -1 && GAME.board[row][col] == last_move_player->id) {
        count++;
        row--;
        col--;
    }
    down_count = count;
    row = move_row + 1;
    col = move_col + 1;

    while (row < BOARD_WIDTH && col < BOARD_HEIGHT && GAME.board[row][col] == last_move_player->id) {
        count++;
        row++;
        col++;
    }
    if (count >= 4) {
        // TODO: draw line
        return 1;
    }

    // Right diagonal
    row = move_row;
    col = move_col;
    count = 0;
    up = 0;
    down_count = 0;

    while (row > -1 && col > -1 && GAME.board[row][col] == last_move_player->id) {
        count++;
        row++;
        col--;
    }
    down_count = count;
    row = move_row + 1;
    col = move_col + 1;

    while (row < BOARD_WIDTH && col < BOARD_HEIGHT && GAME.board[row][col] == last_move_player->id) {
        count++;
        row--;
        col++;
    }
    if (count >= 4) {
        // TODO: draw line
        return 1;
    }

    if (check_board_full()) {
        return 2;
    }

    return 0;
}
