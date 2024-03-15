/*
 * c4.c
 *
 *  Created on: Mar 7, 2024
 *      Author: Nathan D'Cruz
 *      Adapted from connect4.py by Henry Sun
 */

#include "c4.h"

static int imax(int a, int b);
static int imin(int a, int b);
static void fmt_err_c4();
static bool do_move(player_t* player, unsigned int selected_column);
static void turn_transition();
static unsigned int play_turn();
static unsigned int wait_turn(unsigned int *moveReceived);
static unsigned int check_game_over_c4(unsigned int move_col, player_t* last_move_player);


// DEBUG
//#include "inputter.c"
//
static int iTLSSockID;


// GLOBALS DECLARATION
struct C4_PLAYER {
    player_t* opponent;
    unsigned int id;
    uint16_t color;
    unsigned int name[8];

}; // player_t

struct C4_GAME_OBJ {
    // GAME STATE
    player_t*   winner;
    unsigned int     status;

    // BOARD STATE
    unsigned int     board[BOARD_HEIGHT][BOARD_WIDTH];
    unsigned int     lowest_empty_col_index[BOARD_WIDTH];
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
    char fmt_err_c4[MAX_ERR_LEN + 3]; // Accommodate \n\r
    sprintf(fmt_err_c4, "%s\n\r", err_msg);

    // Write to UART
    Report(fmt_err_c4);
    

   //printf("%s\n", err_msg);
}

static void init_player(player_t* player, player_t* opponent, unsigned int player_id, char* player_name) {
    player->opponent = opponent;
    player->id = player_id;
    player->color = player_id % 2 == 0 ? THEME_RED : THEME_YELLOW;
    strcpy(player->name, player_name);
}

static bool set_winner(bool is_tie){
    if (!(GAME.status == GAME_ACTIVE)) {
        // fmt_err_c4("Cannot set winner: no active game");
        return true;
    }

    GAME.winner = is_tie ? &TIE_PLAYER : GAME.player_turn;
    return false;
}

static bool do_move(player_t* player, unsigned int selected_column) {
    unsigned int row = GAME.lowest_empty_col_index[selected_column];

    unsigned int cell = GAME.board[row][selected_column];
    if (!(cell == CELL_EMPTY)) {
        // fmt_err_c4("Cannot do move: cell already in use");
        return true;
    }

    GAME.board[row][selected_column] = player->id;
    drawToken(row, selected_column, player->color);

    GAME.lowest_empty_col_index[selected_column]--;
    GAME.num_moves++;

    return false;
}

static void turn_transition() {
    // TODO: update cursor
    if (GAME.player_turn->id == 1) {
        eraseCursor(PLAYER_CURSOR_X, PLAYER_CURSOR_Y, CURSOR_RIGHT);
        drawCursor(PLAYER_CURSOR_X + 64, PLAYER_CURSOR_Y, CURSOR_RIGHT, ORANGE);
    } else {
        eraseCursor(PLAYER_CURSOR_X + 64, PLAYER_CURSOR_Y, CURSOR_RIGHT);
        drawCursor(PLAYER_CURSOR_X, PLAYER_CURSOR_Y, CURSOR_RIGHT, ORANGE);
    }

    GAME.player_turn = GAME.player_turn->opponent;
}

static bool check_board_full() { return (GAME.num_moves >= MAX_MOVES); }

static unsigned int play_turn() {
    unsigned int selected_column; // TODO: poll for input
                             // Listen for game quit
    bool valid_move = false;
    drawMessage(MSG_MAKE_MOVE, YELLOW);
    drawCursor(BOARD_CURSOR_X, BOARD_CURSOR_Y, CURSOR_DOWN, WHITE); // arrow_down

    while (!valid_move) {
        // selected_column = 2; // DUMMY VALUE until above is satisfied

        selected_column = getUsrInput(GAME_STATE);
        
        // use unsigned negative overflow to detect when the column is full
        valid_move = !(GAME.lowest_empty_col_index[selected_column] > BOARD_HEIGHT - 1);
    } 

    do_move(GAME.player_turn, selected_column);
    return selected_column;
}

static unsigned int wait_turn(unsigned int *moveReceived) {
    drawMessage(MSG_WAIT, YELLOW);
    // Poll AWS for state update
    char msgReceived[4096];
    *moveReceived = 300;
    pollMsg(iTLSSockID, msgReceived);
    unsigned int codeP = parseServerMsg(msgReceived, moveReceived, GAME.local_player->id, 0);
    do_move(GAME.local_player->opponent, *moveReceived);
    // Report("\r\nlocal id : %d\r\nOpponent Move: %d\r\n", GAME.local_player->opponent->id, moveReceived);
    // fmt_err_c4("Waiting turn...");
    return 0;
    // return 0;
}

static unsigned int check_game_over_c4(unsigned int move_col, player_t* last_move_player) {
    unsigned int move_row = GAME.lowest_empty_col_index[move_col] + 1;

    unsigned int min_row_index = imax(move_col - 3, 0);
    unsigned int max_row_index = imin(move_col + 3, BOARD_HEIGHT - 1);
    unsigned int max_col_index = imax(move_row - 3, 0);
    unsigned int min_col_index = imin(move_row + 3, BOARD_WIDTH - 1);

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
            int first = s - 3;
            int last = s;
            drawWinLine(move_row, first, move_row, last);

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
            int first = s - 3;
            int last = s;
            drawWinLine(first, move_col, last, move_col);
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
        drawWinLine(move_row, move_col, row, col);
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
        drawWinLine(move_row, move_col, row, col);
        return 1;
    }

    if (check_board_full()) {
        return 2;
    }

    return 0;
}

// STATIC DEBUG METHODS
static void print_player_state() {
    // printf("LOCAL PLAYER: %d\n", GAME.local_player->id); 
    // printf("    PLAYER 1: %d\n", GAME.player1.id);
    // printf("    PLAYER 2: %d\n", GAME.player2.id);
    // printf(" PLAYER TURN: %d\n", GAME.player_turn->id);
}

static void print_board() {
    unsigned int row = 0, col = 0;
    for (row = 0; row < BOARD_HEIGHT; row++) {
        for (col = 0; col < BOARD_WIDTH; col++) {
            // printf("%d ", GAME.board[row][col]);
        }
        // printf("\n");
    }
}

static void print_game_state() {
    // printf(" GAME STATUS: %d\n", GAME.status); 
    // printf("      WINNER: %p\n", GAME.winner);
}
// 

// PUBLIC METHODS
bool init_c4t(unsigned int isAI, unsigned int levelIdx, int socketId) {
    // Initialize player state
    if (GAME_INIT) {
        // fmt_err_c4("Cannot init: connect4 game object is already initialized.");
        return 0;
    }

    // send request to server
    char msgSend[200] = "";
    char msgReceived[512];
    char player_name1[8] = "";
    char player_name2[8] = "";
    unsigned int cmdReceived = 300;
    unsigned int player1_id, player2_id;
    bool player1_is_local;

    player1_id = 1;
    player2_id = 2;
    iTLSSockID = socketId;

    if (isAI) {
        buildToServerMsg(msgSend, levelIdx, 1);
    } else {
        buildToServerMsg(msgSend, START_GAME, 1);
    }
    // display connecting msg
    drawMessage(MSG_CONNECTING, ORANGE);
    http_post(iTLSSockID, msgSend);
    pollMsg(iTLSSockID, msgReceived);
    parseServerMsg(msgReceived, &cmdReceived, 0, 1);
    // Report("DEBUG: %d\r\n", cmdReceived);
    
    if (cmdReceived == SET_PLAYER0) {
        // Report("SET_PLAYER0\r\n");
        player1_is_local == 1;
    } else {
        if (cmdReceived != SET_PLAYER1) {
            return true;
        }
        player1_is_local == 0;
    }

    // Input validation
    switch(player1_id) {
    case(NO_PLAYER_ID):
    case(TIE_PLAYER_ID):
    case(AI_PLAYER_ID):
        // fmt_err_c4("Cannot init: Invalid player1_id. Did you use a reserved player id?");
        return true;
    }

    switch(player2_id) {
    case(NO_PLAYER_ID):
    case(TIE_PLAYER_ID):
    case(AI_PLAYER_ID):
        // fmt_err_c4("Cannot init: Invalid player2_id. Did you use a reserved player id?");
        return true;
    }

    // Dummy players for special player states
    NO_PLAYER.id = NO_PLAYER_ID;           // For when player is "null"
    NO_PLAYER.opponent = NULL;
    TIE_PLAYER.id = TIE_PLAYER_ID;        // For when the game is a tie
    TIE_PLAYER.opponent = NULL;


    if (player1_is_local) {
        GAME.local_player = &GAME.player1;
        strcpy(player_name1, STR_PLAYER_SELF);
        if (isAI) {
            strcpy(player_name2, STR_PLAYER_AI);
        } else {
            strcpy(player_name2, STR_PLAYER_HUMAN);
        }
    } else {
        GAME.local_player = &GAME.player2;
        strcpy(player_name2, STR_PLAYER_SELF);
        if (isAI) {
            strcpy(player_name1, STR_PLAYER_AI);
        } else {
            strcpy(player_name1, STR_PLAYER_HUMAN);
        }
    }

    init_player(&GAME.player1, &GAME.player2, player1_id, player_name1);
    init_player(&GAME.player2, &GAME.player1, player2_id, player_name2);

    drawPlayerInfo(player_name1, player_name2);

    GAME.player_turn = &NO_PLAYER;

    // Initialize board
    int col;
    int row;
    for (col = 0; col < BOARD_WIDTH; col++) {
        for (row = 0; row < BOARD_HEIGHT; row++) {
            GAME.board[row][col] = 0;
        }
        GAME.lowest_empty_col_index[col] = BOARD_HEIGHT - 1;
    }
    GAME.num_moves = 0;


    // Initialize game state
    GAME.winner = &NO_PLAYER;
    GAME.status = GAME_INACTIVE;

    // Set game initialization flag
    GAME_INIT = true;

    return 0;
}

bool game_active_c4() { return ( GAME_INIT && (GAME.status == GAME_ACTIVE) ); }
bool game_initialized_c4() { return (GAME_INIT); }

bool start_game_c4() {
    if (!GAME_INIT) {
        // fmt_err_c4("Game has not been initialized. Please call init_c4t() first.");
        return true;
    } else if (game_active_c4()) {
        // fmt_err_c4("There is already a game in progress.");
        return true;
    }

    GAME.status = GAME_ACTIVE;

    GAME.player_turn = &GAME.player1;
    bool first_turn = true;

    // Game loop
    bool game_over = false;
    bool is_tie = false;

    // set_board_state();

    while (!game_over) {
        if (!first_turn) { turn_transition(); }
        first_turn = false;

        // DEBUG
        // printf("====\n");
        print_player_state();
        //
        unsigned int last_move;
        if (GAME.player_turn == GAME.local_player) {
            last_move = play_turn();
            // Report("Received the user input\r\n");
            char msgSend[200] = "";
            buildToServerMsg(msgSend, last_move, 0);
            http_post(iTLSSockID, msgSend);
        } else {
            wait_turn(&last_move);
            Report("last_move: %d\r\n", last_move);
        }
        // Report("start checking gameover\r\n");
        int game_over_status = check_game_over_c4(last_move, GAME.player_turn);
        // Report("isGameChecked? YES!\r\n");
        game_over = game_over_status > 0;
        is_tie = game_over_status > 1;
        // DEBUG
        Report("GAME OVER: %d\n", game_over); 
        Report("IS TIE: %d\n", is_tie);
        print_board();
        //
    }

    if (game_over) {
        set_winner(is_tie);
        if(is_tie) {
            drawMessage(MSG_TIE, WHITE);
        } else {
            if(GAME.winner->id == GAME.local_player->id) {
                drawMessage(MSG_WIN, GREEN);
            } else {
                drawMessage(MSG_LOSE, RED);
            }
        }

        GAME.status = GAME_OVER;

        // DEBUG
        print_game_state();
        // 5 second
        MAP_UtilsDelay(40000000);
    }

    return false;
}

bool reset_game_c4() {
    if (!GAME_INIT) { 
        // fmt_err_c4("Cannot reset game: game is not initialized, please call init_c4t() instead."); 
        return true; }
    if (GAME.status == GAME_ACTIVE) { 
        // fmt_err_c4("Cannot reset game: game is currently active."); 
        return true; };

    GAME_INIT = false;
    return true;
}
