#include "game.h"
#include "move.h"
#include "util.h"
#include <vector>
#include <string>
#include <array>

using std::vector;
using std::string;

// Used to represent vertical and horizontal lines in a position

// Is there a better way to do this?
const uint8 RL = 0;
const uint8 RR = 1;
const uint8 RB = 2;
const uint8 CU = 3;
const uint8 CD = 4;
const uint8 CB = 5;

const uint8 D0U = 0;
const uint8 D0D = 1;
const uint8 D0B = 2;
const uint8 D1U = 3;
const uint8 D1D = 4;
const uint8 D1B = 5;
const uint8 S0 = 6;
const uint8 S1 = 7;
const uint8 S2 = 8;
const uint8 S3 = 9;

std::array<bitset<NUM_MOVES>, 102> line_breakers = {
    bitset<NUM_MOVES>(string("001000000000111000000000000000000000111000000000000100000000000011100000000000000000000000000000")),
    bitset<NUM_MOVES>(string("001000000000111000000000000000000000111000000000000000000000000000010000000000001110000000000000")),
    bitset<NUM_MOVES>(string("001000000000111000000000000000000000111000000000000000000000000000000000000000000001000000000000")),
    bitset<NUM_MOVES>(string("000001000000111011100000000000000000111011100000000000010000000000001110000000000000000000000000")),
    bitset<NUM_MOVES>(string("000001000000111011100000000000000000111011100000000000000000000000000001000000000000111000000000")),
    bitset<NUM_MOVES>(string("000001000000111011100000000000000000111011100000000000000000000000000000000000000000000100000000")),
    bitset<NUM_MOVES>(string("000000001000000011101110000000000000000011101110000000000001000000000000111000000000000000000000")),
    bitset<NUM_MOVES>(string("000000001000000011101110000000000000000011101110000000000000000000000000000100000000000011100000")),
    bitset<NUM_MOVES>(string("000000001000000011101110000000000000000011101110000000000000000000000000000000000000000000010000")),
    bitset<NUM_MOVES>(string("000000000001000000001110000000000000000000001110000000000000000100000000000011100000000000000000")),
    bitset<NUM_MOVES>(string("000000000001000000001110000000000000000000001110000000000000000000000000000000010000000000001110")),
    bitset<NUM_MOVES>(string("000000000001000000001110000000000000000000001110000000000000000000000000000000000000000000000001")),
    bitset<NUM_MOVES>(string("000000000000011100000000100000000000011100000000100000000000000001110000000000000000000000000000")),
    bitset<NUM_MOVES>(string("000000000000011100000000100000000000011100000000000000000000000010000000000000000111000000000000")),
    bitset<NUM_MOVES>(string("000000000000011100000000100000000000011100000000000000000000000000000000000000001000000000000000")),
    bitset<NUM_MOVES>(string("000000000000011101110000000100000000011101110000000010000000000000000111000000000000000000000000")),
    bitset<NUM_MOVES>(string("000000000000011101110000000100000000011101110000000000000000000000001000000000000000011100000000")),
    bitset<NUM_MOVES>(string("000000000000011101110000000100000000011101110000000000000000000000000000000000000000100000000000")),
    bitset<NUM_MOVES>(string("000000000000000001110111000000100000000001110111000000001000000000000000011100000000000000000000")),
    bitset<NUM_MOVES>(string("000000000000000001110111000000100000000001110111000000000000000000000000100000000000000001110000")),
    bitset<NUM_MOVES>(string("000000000000000001110111000000100000000001110111000000000000000000000000000000000000000010000000")),
    bitset<NUM_MOVES>(string("000000000000000000000111000000000100000000000111000000000000100000000000000001110000000000000000")),
    bitset<NUM_MOVES>(string("000000000000000000000111000000000100000000000111000000000000000000000000000010000000000000000111")),
    bitset<NUM_MOVES>(string("000000000000000000000111000000000100000000000111000000000000000000000000000000000000000000001000")),
    bitset<NUM_MOVES>(string("000000000000011000000000000000000000011000000000000000000000000001100000000000000000000000000000")),
    bitset<NUM_MOVES>(string("000000000000011000000000000000000000011000000000000000000000000000000000000000000110000000000000")),
    bitset<NUM_MOVES>(string("000000000000011000000000000000000000011000000000000000000000000000000000000000000000000000000000")),
    bitset<NUM_MOVES>(string("000000000000011001100000000000000000011001100000000000000000000000000110000000000000000000000000")),
    bitset<NUM_MOVES>(string("000000000000011001100000000000000000011001100000000000000000000000000000000000000000011000000000")),
    bitset<NUM_MOVES>(string("000000000000011001100000000000000000011001100000000000000000000000000000000000000000000000000000")),
    bitset<NUM_MOVES>(string("000000000000000001100110000000000000000001100110000000000000000000000000011000000000000000000000")),
    bitset<NUM_MOVES>(string("000000000000000001100110000000000000000001100110000000000000000000000000000000000000000001100000")),
    bitset<NUM_MOVES>(string("000000000000000001100110000000000000000001100110000000000000000000000000000000000000000000000000")),
    bitset<NUM_MOVES>(string("000000000000000000000110000000000000000000000110000000000000000000000000000001100000000000000000")),
    bitset<NUM_MOVES>(string("000000000000000000000110000000000000000000000110000000000000000000000000000000000000000000000110")),
    bitset<NUM_MOVES>(string("000000000000000000000110000000000000000000000110000000000000000000000000000000000000000000000000")),
    bitset<NUM_MOVES>(string("100100100000000000001000100100100000000000000000000000000000100010001000100000000000000000000000")),
    bitset<NUM_MOVES>(string("100100100000000000001000100100100000000000000000000000000000000000000000000010001000100010000000")),
    bitset<NUM_MOVES>(string("100100100000000000001000100100100000000000000000000000000000000000000000000000000000000000001000")),
    bitset<NUM_MOVES>(string("110110110000000000000100110110110000000000000000000000000000010001000100010000000000000000000000")),
    bitset<NUM_MOVES>(string("110110110000000000000100110110110000000000000000000000000000000000000000000001000100010001000000")),
    bitset<NUM_MOVES>(string("110110110000000000000100110110110000000000000000000000000000000000000000000000000000000000000100")),
    bitset<NUM_MOVES>(string("011011011000000000000010011011011000000000000000000000000000001000100010001000000000000000000000")),
    bitset<NUM_MOVES>(string("011011011000000000000010011011011000000000000000000000000000000000000000000000100010001000100000")),
    bitset<NUM_MOVES>(string("011011011000000000000010011011011000000000000000000000000000000000000000000000000000000000000010")),
    bitset<NUM_MOVES>(string("001001001000000000000001001001001000000000000000000000000000000100010001000100000000000000000000")),
    bitset<NUM_MOVES>(string("001001001000000000000001001001001000000000000000000000000000000000000000000000010001000100010000")),
    bitset<NUM_MOVES>(string("001001001000000000000001001001001000000000000000000000000000000000000000000000000000000000000001")),
    bitset<NUM_MOVES>(string("000100100100000000000000000100100100100000000000100000000000000000001000100010000000000000000000")),
    bitset<NUM_MOVES>(string("000100100100000000000000000100100100100000000000000000000000000010000000000000000000100010001000")),
    bitset<NUM_MOVES>(string("000100100100000000000000000100100100100000000000000000000000000000000000000000001000000000000000")),
    bitset<NUM_MOVES>(string("000110110110000000000000000110110110010000000000010000000000000000000100010001000000000000000000")),
    bitset<NUM_MOVES>(string("000110110110000000000000000110110110010000000000000000000000000001000000000000000000010001000100")),
    bitset<NUM_MOVES>(string("000110110110000000000000000110110110010000000000000000000000000000000000000000000100000000000000")),
    bitset<NUM_MOVES>(string("000011011011000000000000000011011011001000000000001000000000000000000010001000100000000000000000")),
    bitset<NUM_MOVES>(string("000011011011000000000000000011011011001000000000000000000000000000100000000000000000001000100010")),
    bitset<NUM_MOVES>(string("000011011011000000000000000011011011001000000000000000000000000000000000000000000010000000000000")),
    bitset<NUM_MOVES>(string("000001001001000000000000000001001001000100000000000100000000000000000001000100010000000000000000")),
    bitset<NUM_MOVES>(string("000001001001000000000000000001001001000100000000000000000000000000010000000000000000000100010001")),
    bitset<NUM_MOVES>(string("000001001001000000000000000001001001000100000000000000000000000000000000000000000001000000000000")),
    bitset<NUM_MOVES>(string("000100100000000000000000000100100000000000000000000000000000000000001000100000000000000000000000")),
    bitset<NUM_MOVES>(string("000100100000000000000000000100100000000000000000000000000000000000000000000000000000100010000000")),
    bitset<NUM_MOVES>(string("000100100000000000000000000100100000000000000000000000000000000000000000000000000000000000000000")),
    bitset<NUM_MOVES>(string("000110110000000000000000000110110000000000000000000000000000000000000100010000000000000000000000")),
    bitset<NUM_MOVES>(string("000110110000000000000000000110110000000000000000000000000000000000000000000000000000010001000000")),
    bitset<NUM_MOVES>(string("000110110000000000000000000110110000000000000000000000000000000000000000000000000000000000000000")),
    bitset<NUM_MOVES>(string("000011011000000000000000000011011000000000000000000000000000000000000010001000000000000000000000")),
    bitset<NUM_MOVES>(string("000011011000000000000000000011011000000000000000000000000000000000000000000000000000001000100000")),
    bitset<NUM_MOVES>(string("000011011000000000000000000011011000000000000000000000000000000000000000000000000000000000000000")),
    bitset<NUM_MOVES>(string("000001001000000000000000000001001000000000000000000000000000000000000001000100000000000000000000")),
    bitset<NUM_MOVES>(string("000001001000000000000000000001001000000000000000000000000000000000000000000000000000000100010000")),
    bitset<NUM_MOVES>(string("000001001000000000000000000001001000000000000000000000000000000000000000000000000000000000000000")),
    bitset<NUM_MOVES>(string("000110011000011001000010000110011000010001100010000000000000000000000100001000000000000000000000")),
    bitset<NUM_MOVES>(string("000110011000011001000010000110011000010001100010000000000000000000000000000000000000010000100000")),
    bitset<NUM_MOVES>(string("000110011000011001000010000110011000010001100010000000000000000000000000000000000000000000000000")),
    bitset<NUM_MOVES>(string("000110011000011001000010000110011000010001100010000000000000000000000100001000000000000000000000")),
    bitset<NUM_MOVES>(string("000110011000011001000010000110011000010001100010000000000000000000000000000000000000010000100000")),
    bitset<NUM_MOVES>(string("000110011000011001000010000110011000010001100010000000000000000000000000000000000000000000000000")),
    bitset<NUM_MOVES>(string("000110011000011001000010000110011000010001100010000000000000000000000100001000000000000000000000")),
    bitset<NUM_MOVES>(string("000110011000011001000010000110011000010001100010000000000000000000000000000000000000010000100000")),
    bitset<NUM_MOVES>(string("000110011000011001000010000110011000010001100010000000000000000000000000000000000000000000000000")),
    bitset<NUM_MOVES>(string("000011110000011000100100000011110000001001100100000000000000000000000010010000000000000000000000")),
    bitset<NUM_MOVES>(string("000011110000011000100100000011110000001001100100000000000000000000000000000000000000001001000000")),
    bitset<NUM_MOVES>(string("000011110000011000100100000011110000001001100100000000000000000000000000000000000000000000000000")),
    bitset<NUM_MOVES>(string("000011110000011000100100000011110000001001100100000000000000000000000010010000000000000000000000")),
    bitset<NUM_MOVES>(string("000011110000011000100100000011110000001001100100000000000000000000000000000000000000001001000000")),
    bitset<NUM_MOVES>(string("000011110000011000100100000011110000001001100100000000000000000000000000000000000000000000000000")),
    bitset<NUM_MOVES>(string("000011110000011000100100000011110000001001100100000000000000000000000010010000000000000000000000")),
    bitset<NUM_MOVES>(string("000011110000011000100100000011110000001001100100000000000000000000000000000000000000001001000000")),
    bitset<NUM_MOVES>(string("000011110000011000100100000011110000001001100100000000000000000000000000000000000000000000000000")),
    bitset<NUM_MOVES>(string("011110100000011011001000011110100000011011001000000000000000000000100100100000000000000000000000")),
    bitset<NUM_MOVES>(string("011110100000011011001000011110100000011011001000000000000000000000000000000000000010010010000000")),
    bitset<NUM_MOVES>(string("011110100000011011001000011110100000011011001000000000000000000000000000000000000000000000000000")),
    bitset<NUM_MOVES>(string("110011001000011000110001110011001000011000110001000000000000000001000010000100000000000000000000")),
    bitset<NUM_MOVES>(string("110011001000011000110001110011001000011000110001000000000000000000000000000000000100001000010000")),
    bitset<NUM_MOVES>(string("110011001000011000110001110011001000011000110001000000000000000000000000000000000000000000000000")),
    bitset<NUM_MOVES>(string("000001011110000100110110000001011110000100110110000000000000000000000001001001000000000000000000")),
    bitset<NUM_MOVES>(string("000001011110000100110110000001011110000100110110000000000000000000000000000000000000000100100100")),
    bitset<NUM_MOVES>(string("000001011110000100110110000001011110000100110110000000000000000000000000000000000000000000000000")),
    bitset<NUM_MOVES>(string("000100110011100011000110000100110011100011000110000000000000000000001000010000100000000000000000")),
    bitset<NUM_MOVES>(string("000100110011100011000110000100110011100011000110000000000000000000000000000000000000100001000010")),
    bitset<NUM_MOVES>(string("000100110011100011000110000100110011100011000110000000000000000000000000000000000000000000000000"))
};

Game::Game(): board{bitset<3*BOARD_SIZE>().set()}, frozen{bitset<BOARD_SIZE>().set()}, to_play{0},
pieces{4, 4, 4, 4, 4, 4} {}

bool Game::is_empty(uint8 row, uint8 col) {
    return !(
        board.test(row * 12 + col * 3 + 0) ||
	board.test(row * 12 + col * 3 + 1) ||
	board.test(row * 12 + col * 3 + 2)
    ); // Is there a faster way to test this given the consecutive addresses?
}

bool Game::can_place(uint8 ptype, uint8 row, uint8 col) {

    // Check if space is empty
    // This is more common than frozen spaces
    // An empty space cannot be frozen
    if (is_empty(row, col)) return true;
    // Check if player has the piece left
    if (pieces[to_play * 3 + ptype]  == 0) return false;
    // Check if space is frozen
    if (frozen.test(row * 4 + col)) return false;
    // Bases can only be placed on empty spaces
    if (ptype == 0) return false; // is if (!ptype) faster?
    // Column
    // Check for absence of column and capital
    if (ptype == 1) {
        return !(
            board.test(row * 12 + col * 3 + 1) ||
	    board.test(row * 12 + col * 3 + 2)
	);
    }
    // Capital
    // Check for absence of base without column and capital
    return !(
        board.test(row * 12 + col * 3 + 2) ||
	(board.test(row * 12 + col * 3 + 0) && !board.test(row * 12 + col * 3 + 1))
    );
}

// Returns an int representing the bottom piece of a stack, 3 if empty
// Used to determine the legality of move moves
int8 Game::get_bottom(uint8 row, uint8 col) {
    if (board.test(row * 12 + col * 3 + 0)) return 0;
    if (board.test(row * 12 + col * 3 + 1)) return 1;
    if (board.test(row * 12 + col * 3 + 2)) return 2;
    // Empty
    return 3;
}

// Returns an int representing the top of a stack, -1 if empty
// Used to determine the legality of move moves
int8 Game::get_top(uint8 row, uint8 col) {
    if (board.test(row * 12 + col * 3 + 2)) return 2;
    if (board.test(row * 12 + col * 3 + 1)) return 1;
    if (board.test(row * 12 + col * 3 + 0)) return 0;
    // Empty
    return -1;
}

// Returns whether it is legal to move a stack between spaces
bool Game::can_move(uint8 row1, uint8 col1, uint8 row2, uint8 col2) {
    // Empty spaces, move moves not possible
    if (is_empty(row1, col1) || is_empty(row2, col2)) return false;
    // Frozen spaces
    if (frozen.test(row1 * 4 + col1) || frozen.test(row2 * 4 + col2)) return false;
    // The bottom of the first stack must go on the top of the second
    return get_bottom(row1, col1) - get_top(row2, col2) == 1;
}

// Returns whether a move is legal
bool Game::is_legal(uint8 move_id) {
    Move move{move_id};
    // Place
    if (move.mtype) return can_place(move.ptype, move.row1, move.col1);
    // Move
    return can_move(move.row1, move.col1, move.row2, move.col2);    
}

void apply_line(bitset<NUM_MOVES> &legal_moves, uint8 line) {
    legal_moves &= line_breakers[uint8];
}

// Finds lines and moves that break all lines
void Game::get_line_breakers(bitset<NUM_MOVES> &legal_moves) {

    int8 space_0, space_1, space_2, space_3;

    // Rows
    for (uint8 i = 0; i < 4; ++i) {
        space_1 = get_top(i, 1);
        if (space_1 != -1) { // If not empty
            space_2 = get_top(i, 2);
            if (space_1 == space_2) {
                space_0 = get_top(i, 0), space_3 = get_top(i, 3);
                if (space_0 == space_2) {
                    if (space_0 == space_3) {
                        apply_line(legal_moves, RB * 12 + i * 3 + space_0);
                    }
                    else {
                        apply_line(legal_moves, RL * 12 + i * 3 + space_0);
                    }
                }
                else if (space_2 == space_3) {
                    apply_line(legal_moves, RR * 12 + i * 3 + space_3);
                }
            }
        }
    }

    // Columns
    for (uint8 j = 0; j < 4; ++j) {
        space_1 = get_top(1, j);
        if (space_1 != -1) { // If not empty
            space_2 = get_top(2, j);
	        if (space_1 == space_2) {
	            space_0 = get_top(0, j), space_3 = get_top(3, j);
	            if (space_0 == space_2) {
	                if (space_0 == space_3) {
		                apply_line(legal_moves, CB * 12 + j * 3 + space_0);
		            }
		            else {
		                apply_line(legal_moves, CU * 12 + j * 3 + space_0);
		            }
                }
	            else if (space_2 == space_3) {
	                apply_line(legal_moves, CD * 12 + j * 3 + space_3);
	            }
	        }
	    }
    }

    // Upper left to lower right long diagonal
    space_1 = get_top(1, 1);
    if (space_1 != -1) { // If not empty
        space_2 = get_top(2, 2);
	    if (space_1 == space_2) {
            space_0 = get_top(0, 0), space_3 = get_top(3, 3);
	        if (space_0 == space_2) {
	            if (space_0 == space_3) {
	                apply_line(legal_moves, 72 + D0B * 3 + space_0);
		        }
                else {
                    apply_line(legal_moves, 72 + D0U * 3 + space_0);
                }
	        }
            else if (space_2 == space_3) {
                apply_line(legal_moves, 72 + D0D * 3 + space_3);
            }
	    }
    }

    // Upper right to lower left long diagonal
    space_1 = get_top(1, 2);
    if (space_1 != -1) { // If not empty
        space_2 = get_top(2, 1);
	    if (space_1 == space_2) {
	        space_0 = get_top(0, 3), space_3 = get_top(3, 0);
	        if (space_0 == space_2) {
	            if (space_0 == space_3) {
		            apply_line(legal_moves, 72 + D1B * 3 + space_0);
		        }
		        else {
                    apply_line(legal_moves, 72 + D1U * 3 + space_0);
		        }
	        }
	        else if (space_2 == space_3) {
                apply_line(legal_moves, 72 + D1D * 3 + space_3);
	        }
	    }
    }

    // Top left short diagonal
    space_1 = get_top(1, 1);
    if (space_1 != -1 && space_1 == get_top(0, 2) && space_1 == get_top(2, 0)) {
        apply_line(legal_moves, 72 + S0 * 3 + space_1);
    }

    // Top right short diagonal
    space_1 = get_top(1, 2);
    if (space_1 != -1 && space_1 == get_top(0, 1) && space_1 == get_top(2, 3)) {
        apply_line(legal_moves, 72 + S1 * 3 + space_1);
    }

    // Bottom right short diagonal
    space_1 = get_top(2, 2);
    if (space_1 != -1 && space_1 == get_top(1, 3) && space_1 == get_top(3, 1)) {
        apply_line(legal_moves, 72 + S2 * 3 + space_1);
    }

    // Bottom left short diagonal
    space_1 = get_top(2, 1);
    if (space_1 != -1 && space_1 == get_top(1, 0) && space_1 == get_top(3, 2)) {
        apply_line(legal_moves, 72 + S3 * 3 + space_1);
    }

}

// Find lines and then filters through remaining moves to find legal moves
void Game::get_legal_moves(bitset<NUM_MOVES> &legal_moves) {

    // Filter out moves that don't break lines
    get_line_breakers(legal_moves);

    for (uint8 i = 0; i < NUM_MOVES; ++i) {
        if (legal_moves.test(i) && !is_legal(i)) {
            legal_moves.flip(i);
	    }
    }

}

// Apply move to game. Does not check for legality
void Game::do_move(uint8 move_id) {
    
    Move move{move_id};

    // Reset which space is frozen
    frozen.reset();

    // Place
    if (move.mtype) {
        pieces[to_play * 3 + move.ptype] -= 1;
	    board.set(move.row1 * 12 + move.col1 * 3 + move.ptype);
    }
    // Move
    else {
        for (uint8 i = 0; i < 3; ++i) {
	        board.set(
	            move.row2 * 12 + move.col2 * 3 + i,
		        board.test(move.row1 * 12 + move.col1 * 3 + i) ||
		        board.test(move.row2 * 12 + move.col2 & 3 + i)
	        );
            board.reset(move.row1 * 12 + move.col1 * 3 + i);
	}
    }
    to_play = 1 - to_play;
}
