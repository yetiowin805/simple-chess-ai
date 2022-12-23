#include "trainer.h"
#include "node.h"
#include "trainmc.h"
#include "util.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <omp.h>
#include <queue>
#include <string>
#include <vector>

using std::cerr;
using std::string;
using std::vector;

Trainer::Trainer(uintf num_games, uintf num_logged, uintf num_iterations,
                 float c_puct, float epsilon, const string &logging_folder,
                 uintf random_seed)
    : num_iterations{std::max(num_iterations, (uintf)2)}, iterations_done{0},
      games_done{0}, is_done{vector<bool>(num_games, false)}, generator{
                                                                  random_seed} {
  initialize(false, num_games, num_logged, c_puct, epsilon, logging_folder);
}

Trainer::Trainer(uintf num_games, uintf num_logged, uintf num_iterations,
                 float c_puct, float epsilon, const string &logging_folder,
                 uintf random_seed, bool)
    : num_iterations{std::max(num_iterations, (uintf)2)}, iterations_done{0},
      games_done{0}, is_done{vector<bool>(num_games, false)}, generator{
                                                                  random_seed} {
  initialize(true, num_games, num_logged, c_puct, epsilon, logging_folder);
}

Trainer::~Trainer() {
  for (uintf i = 0; i < games.size(); ++i) {
    delete games[i];
  }
}

bool Trainer::do_iteration(float evaluations[], float probabilities[],
                           float game_states[]) {

  omp_set_num_threads(16);
#pragma omp parallel for
  for (uintf i = 0; i < games.size(); ++i) {
    if (!is_done[i]) {
      // Avoid division by 0 in the rare case than num_games < num_iterations /
      // 2, which sometimes occurs when testing small runs
      if (i / std::max((uintf)1, (games.size() / num_iterations)) <
          iterations_done) {
        bool is_completed = games[i]->do_iteration(
            evaluations[i], &probabilities[i * NUM_MOVES],
            &game_states[i * GAME_STATE_SIZE]);
        if (is_completed) {
          is_done[i] = true;
          ++games_done;
        }
      } else if (i / std::max((uintf)1, (games.size() / num_iterations)) ==
                 iterations_done) {
        games[i]->do_first_iteration(&game_states[i * GAME_STATE_SIZE]);
      }
    }
  }
  if (games_done == games.size()) {
    return true;
  }
  ++iterations_done;

  return false;
}

bool Trainer::do_iteration(float evaluations_1[], float probabilities_1[],
                           float evaluations_2[], float probabilities_2[],
                           float game_states[]) {
#pragma omp parallel for
  for (uintf i = 0; i < games.size(); ++i) {
    if (!is_done[i]) {
      // Avoid division by 0 in the rare case than num_games < num_iterations /
      // 2
      if (i / std::max((uintf)1, (games.size() / (num_iterations / 2))) <
          iterations_done) {
        bool is_completed = games[i]->do_iteration(
            evaluations_1[i], &probabilities_1[i * NUM_MOVES], evaluations_2[i],
            &probabilities_2[i * NUM_MOVES], &game_states[i * GAME_STATE_SIZE]);
        if (is_completed) {
          is_done[i] = true;
          ++games_done;
        }
      } else if (i / std::max((uintf)1,
                              (games.size() / (num_iterations / 2))) ==
                 iterations_done) {
        games[i]->do_first_iteration(&game_states[i * GAME_STATE_SIZE]);
      }
    }
  }
  if (games_done == games.size()) {
    return true;
  }
  ++iterations_done;

  return false;
}

void Trainer::initialize(bool testing, uintf num_games, uintf num_logged,
                         float c_puct, float epsilon,
                         const string &logging_folder) {

  games.reserve(num_games);

  if (num_games < num_logged)
    num_logged = num_games;

  if (testing) {
    for (uintf i = 0; i < num_logged; ++i) {
      games.emplace_back(
          new SelfPlayer{i % 2, new std::mt19937(generator()),
                         new std::ofstream{logging_folder + "/game_" +
                                               std::to_string(i) + ".txt",
                                           std::ofstream::out}});
    }
    for (uintf i = num_logged; i < num_games; ++i) {
      games.emplace_back(new SelfPlayer{i % 2, new std::mt19937(generator())});
    }
  } else {
    for (uintf i = 0; i < num_logged; ++i) {
      games.emplace_back(
          new SelfPlayer{new std::mt19937(generator()),
                         new std::ofstream{logging_folder + "/game_" +
                                               std::to_string(i) + ".txt",
                                           std::ofstream::out}});
    }
    for (uintf i = num_logged; i < num_games; ++i) {
      games.emplace_back(new SelfPlayer{new std::mt19937(generator())});
    }
  }

  // Set TrainMC static variables
  TrainMC::set_statics(num_iterations, c_puct, epsilon);
}

uintf Trainer::count_nodes() const {
  uintf counts[games.size()];
  //#pragma omp parallel for
  for (uintf i = 0; i < games.size(); ++i) {
    counts[i] = games[i]->count_nodes();
  }
  uintf counter = 0;
  for (uintf i = 0; i < games.size(); ++i) {
    counter += counts[i];
  }
  return counter;
}

uintf Trainer::count_samples() const {
  uintf counts[games.size()];
#pragma omp parallel for
  for (uintf i = 0; i < games.size(); ++i) {
    counts[i] = games[i]->count_samples();
  }
  uintf counter = 0;
  for (uintf i = 0; i < games.size(); ++i) {
    counter += counts[i];
  }
  return counter;
}

void Trainer::write_samples(float *game_states, float *evaluation_samples,
                            float *probability_samples) const {
  uintf offset = 0;
  for (uintf i = 0; i < games.size(); ++i) {
    uintf num_samples = games[i]->write_samples(
        game_states + offset * GAME_STATE_SIZE, evaluation_samples + offset,
        probability_samples + offset * NUM_MOVES);
    offset += num_samples;
  }
}

float Trainer::get_score() const {
  float scores[games.size()];
#pragma omp parallel for
  for (uintf i = 0; i < games.size(); i += 2) {
    scores[i] = games[i]->get_score();
  }
#pragma omp parallel for
  for (uintf i = 1; i < games.size(); i += 2) {
    scores[i] = 1.0 - games[i]->get_score();
  }
  float score = 0;
  for (uintf i = 0; i < games.size(); ++i) {
    score += scores[i];
  }
  return score / (float)games.size();
}

bool Trainer::is_all_done() const { return games_done == games.size(); }