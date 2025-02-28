#ifndef TRAINMC_H
#define TRAINMC_H

#include "node.h"
#include "util.h"
#include <array>
#include <bitset>
#include <random>
#include <vector>

using std::bitset;

class Game;

class TrainMC {

  // I think we have to initialize these
  inline static uintf max_iterations = 1600, searches_per_eval = 1;
  inline static float c_puct = 1.0, epsilon = 0.25;
  // Number of moves to use weighted random
  const uintf NUM_OPENING_MOVES = 6;

  Node *root, *cur;

  // Which index to write to for the searched node
  uintf eval_index;
  // Nodes we have searches this cycle
  std::vector<Node *> searched;
  // We want to evaluate these vectors
  float *to_eval;

  // Used to keep track of when to choose a move
  uintf iterations_done;

  bool testing, logging;

  std::mt19937 *generator;

  void receive_evaluation(float evaluation[], float probabilities[]);
  bool search();

  void move_down(Node *prev_node);

public:
  // Training
  TrainMC(std::mt19937 *generator);
  // Testing
  TrainMC(std::mt19937 *generator, bool);

  TrainMC(TrainMC &&) = default;
  ~TrainMC() = default;

  // First iterations are guaranteed not to end a turn
  // First iteration on starting position
  void do_first_iteration();

  void do_first_iteration(const Game &game, uintf depth);

  bool do_iteration(float evaluation[], float probabilities[]);

  // Choose the next child to visit
  uintf choose_move(float game_state[GAME_STATE_SIZE],
                    float probability_sample[NUM_MOVES]);

  bool receive_opp_move(uintf move_choice, const Game &game, uintf depth);

  // Accessors
  const Game &get_game() const;
  bool is_uninitialized() const;

  static void set_statics(uintf new_max_iterations, float new_c_puct,
                          float new_epsilon, uintf new_searches_per_eval);

  uintf count_nodes() const;

  friend class SelfPlayer;
};

#endif
