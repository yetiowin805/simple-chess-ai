#include "manager.h"

Manager::Manager(uintf num_games, uintf num_logged, uintf num_iterations,
                 float c_puct, float epsilon, const std::string &logging_folder,
                 uintf random_seed, uintf processes)
    : games_each{num_games / processes} {
  trainers.push_back(new Trainer{games_each, num_logged, num_iterations, c_puct,
                                 epsilon, logging_folder, random_seed});
  is_done.push_back(false);
  for (uintf i = 0; i < processes - 1; ++i) {
    trainers.push_back(new Trainer{games_each, 0, num_iterations, c_puct,
                                   epsilon, logging_folder, random_seed});
    is_done.push_back(false);
  }
}

bool Manager::do_iteration(const float evaluations[],
                           const float probabilities[], float game_states[]) {
#pragma omp parallel for
  for (uintf i = 0; i < trainers.size(); ++i) {
    if (!is_done[i]) {
      is_done[i] = trainers[i]->do_iteration(
          evaluations + i * games_each,
          probabilities + NUM_MOVES * i * games_each,
          game_states + GAME_STATE_SIZE * i * games_each);
    }
  }
  bool done_all = true;
  for (uintf i = 0; i < trainers.size(); ++i) {
    done_all &= trainers[i]->is_all_done();
  }
  return done_all;
}

Manager::~Manager() {
  for (uintf i = 0; i < trainers.size(); ++i) {
    delete trainers[i];
  }
}

uintf Manager::count_samples() const {
  uintf num_samples = 0;
  for (uintf i = 0; i < trainers.size(); ++i) {
    num_samples += trainers[i]->count_samples();
  }
  return num_samples;
}

void Manager::write_samples(float *game_states, float *evaluation_samples,
                            float *probability_samples) const {
  uintf counter = 0;
  for (uintf i = 0; i < trainers.size(); ++i) {
    trainers[i]->write_samples(game_states + GAME_STATE_SIZE * counter,
                               evaluation_samples + counter,
                               probability_samples + NUM_MOVES * counter);
    counter += trainers[i]->count_samples();
  }
}