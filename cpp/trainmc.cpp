#include "trainmc.h"
#include "game.h"
#include "move.h"
#include <memory>
#include <bitset>
#include <math.h>
#include <random>

using std::unique_ptr;
using std::bitset;
using std::memmove;

// Used to initialize tree with root node
TrainMC::TrainMC(bool testing): root{shared_ptr(new Node{})}, iterations_done{0}, cur_node{nullptr}, testing{testing} {}

// First search on root node
void first_search() {

    cur_node = root.get();
    ++(cur_node->visits);
    ++iterations_done;
    // Here we should write the game vector into the numpy array we use for the evaluation

}

// Choose the next child to visit
int choose_next() {

    float max_value = -2;
    int move_choice;

    for (int i = 0; i < LEGAL_MOVE_NUM; ++i) {
        float u = 0;
        // Check if it is a legal move
        if (cur_node->legal_moves.get(i)) {
            // if not visited, set action value to 0
            if (!cur_node->children[i]) {
                u = cur_node->probabilities[i] * pow(cur_node->visits - 1, 0.5);
            }
            else {
                u = -1 * cur_node->children[i]->evaluation / cur_node->children[i]->visits +
                    cur_node->probabilities[i] * pow(cur_node->visits - 1, 0.5) / (cur_node->children[i]->visits + 1);
            }
            if u > max_value {
                max_value = u;
                move_choice = i;
            }
        }
    }

    return move_choice;

}

bool search(float evaluation, float &noisy_probabilities[LEGAL_MOVE_NUM]) {

    // Receive move probabilities
    std::memmove(&noisy_probabilities, &probabilities, LEGAL_MOVE_NUM);

    // Propagate evaluation
    float cur_evaluation = evaluation * pow(-1, to_play);
    while (cur_node != nullptr) {
        cur_node->evaluation += cur_evaluation;
        // Increment visit count when the evaluation from the visit is received
        ++(cur_node->visits);
        cur_evaluation *= -1;
        cur_node = cur_node->parent;
    }

    bool need_evaluation = false;
    while (!need_evaluation && iterations_done < max_iterations) {
        ++iterations_done;
        while (true) {

            // First time searching this node
            if (cur_node->children.size() == 0) {
                cur_node->children.reserve(96);
            }

            int move_choice = choose_next();

            // Exploring a new node
            if (cur_node->children[move_choice].get() == nullptr) {
                // Create the new node
                cur_node->children[move_choice] = shared_ptr<Node>(new Node{cur_node->game, cur_node->depth+1, cur_node});
                cur_node = cur_node->children[move_choice];
                cur_node->game.do_move_and_rotation(move_choice);
                cur_node->game.get_legal_moves(*(cur_node->legal_moves));
                break;
            }
            // Otherwise, move down normally
            else {
                cur_node = cur_node->children[move_choice];
            }
        }
        // Check for terminal state, otherwise evaluation is needed
        if (cur_node->legal_moves->none()) {
            // Don't propagate if value is 0
            if (cur_node->game.outcome != 0) {
                // Propagate evaluation
                float cur_evaluation = cur_node->game.outcome;
                while (cur_node != nullptr) {
                    cur_node->evaluation += cur_evaluation;
                    // Increment visit count when the evaluation from the visit is received
                    ++(cur_node->visits);
                    cur_evaluation *= -1;
                    cur_node = cur_node->parent;
                }
            }
        }
        // Otherwise, request an evaluation
        else {
            // Game state should be written into memoryview/pointer location here
            need_evaluation = true;
        }
    }

    // If true, self player should request an evaluation
    // If false, number of searches is reached and move can be chosen
    return need_evaluation;

}

// Choose the best move once searches are done
int choose_move() {

    // Choose weighted random
    if (root->depth < 4 && !testing) {
        int total = 0;
        for (int i = 0; i < LEGAL_MOVE_NUM; ++i) {
            if (root->children[i]) total += root->children[i]->visits;
        }
        int id = rand() % total;
        for (int i = 0; i < LEGAL_MOVE_NUM; ++i) {
            if (root->children[i]) {
                id -= root->children[i]->visits;
                if (id <= 0) return i;
            }
        }
        return LEGAL_MOVE_NUM - 1;
    }
    // Otherwise, choose randomly between the moves with the most visits/searches
    // Random offset is the easiest way to randomly break ties
    int id = rand() % LEGAL_MOVE_NUM, max_visits = 0, move_choice = -1;
    for (int i = 0; i < LEGAL_MOVE_NUM; ++i) {
        Node *cur_child = root->children[(id + i) % LEGAL_MOVE_NUM].get();
        if (cur_child && cur_child->visits > max_visits) {
            max_visists = cur_child->visits;
            move_choice = (id + i) % LEGAL_MOVE_NUM;
        }
    }

    return move_choice;
}

// Move the tree down a level
// Used when opponent moves
void receive_opp_move(int move_choice) {

    // Unexplored state (this should really only happen for the first move)
    if (!root->children[move_choice]) {
        // Create the new node
        root = shared_ptr<Node>(new Node{root->game, root->depth+1, nullptr});
        cur_node->game.do_move_and_rotation(move_choice);
        cur_node->game.get_legal_moves(*(cur_node->legal_moves)); // Can we delay finding legal moves?
    }
    else {
        root = root->children[move_choice];
    }
}