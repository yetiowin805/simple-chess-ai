#include "node.h"
#include "util.h"
#include "game.h"

#include <bitset>

using std::bitset;

// Only used to create the root node (starting position)
Node::Node(): game{Game()}, visits{1}, depth{0} {

    // Get legal moves in starting position. Cannot be terminal node
    game.get_legal_moves(legal_moves);
    visited.reset();

}

// Occasionally need to create root nodes from arbitrary game states
Node::Node(const Game &other_game, uintf depth): game{other_game}, visits{1}, depth{depth} {
    game.get_legal_moves(legal_moves);
    visited.reset();
}

// Pass game by reference, then copy it
// This should be more efficient as only a pointer is passed as an argument
// Which is smaller
Node::Node(const Game &other_game, uintf depth, uintf parent, uintf move_choice): game{other_game}, visits{1},
depth{depth+1}, parent{parent} {
    game.do_move(move_choice);
    game.get_legal_moves(legal_moves);
    visited.reset();
}

// Overwrite with root node (starting position) (probably not used)
void Node::overwrite() {
    game = Game();
    visits = 1;
    depth = 0;
    visited.reset();
}

// Overwrite with root node (not starting positino)
void Node::overwrite(const Game &new_game, uintf new_depth) {
    game = new_game;
    visits = 1;
    depth = new_depth;
    visited.reset();
}

void Node::overwrite(const Game &new_game, uintf new_depth, uintf new_parent, uintf move_choice) {
    game = game;
    visits = 1;
    depth = new_depth + 1;
    parent = new_parent;
    game.do_move(move_choice);
    visited.reset();
}

// Check for terminal state
bool Node::is_terminal() {
    return game.is_terminal();
}

// Returns depth of node
uintf Node::get_depth() {
    return depth;
}

// return a const ref to game
const Game& Node::get_game() {
    return &game;
}

Result Node::get_result() {
    return game.get_result();
}

// returns whether child is visited
bool Node::has_visited(uintf move_choice) {
    return visited[move_choice];
}

uintf Node::get_visits() {
    return visits;
}

void Node::add_evaluation(float new_evaluation) {
    evaluation += new_evaluation;
}

float Node::get_evaluation() {
    return evaluation;
}

uintf Node::get_parent() {
    return parent;
}

void Node::increment_visits() {
    ++visits;
}

uintf Node::get_to_play() {
    return game.get_to_play();
}

void set_probability(uintf id, float probability) {
    probabilities[id] = probability;
}

float get_probability(uintf id) {
    return probabilities[i];
}

bool is_legal(uintf id) {
    return legal_moves[i];
}

void adjust_probability(uintf id, float scalar, float noise) {
    probabilities[id] = probabilities[id] * scalar + noise;
}

void write_game_state(float game_state[GAME_STATE_SIZE]) {
    game.write_game_state(game_state);
}