# simple-chess-ai

An AI that plays a simplified version of chess. Created mostly to test changes for the corintho-ai repo.

# To-do

- [x] Implement Corintho's rules
- [x] Implement MCTS
- [x] Optimize legal move generator
- [x] Add neural network for evaluation
- [x] Implement concurrent gameplay and batch evaluation for neural network
- [x] Make neural network training pipeline
- [x] Optimize code using C++ and Cython
- [x] Make config parser for hyperparameters and learning rate schedule
- [x] Add training on samples from previous generations
- [x] Add Leela-Zero-style nodes
- [x] Add validation tests
- [x] Check for memory leaks
- [x] Add board symmetries in training samples
- [x] Make testing such that only only neural network is queried each time
- [x] Remove offset for searches during testing
- [ ] Validate correctness of Dirichlet distribution approximation
- [x] Add the option for multiple searches per evaluation
- [ ] Profile code
- [ ] Add Boost memory pool for allocating nodes and edges
- [ ] Try tcmalloc for fasting dynamic allocation
- [ ] Train neural network
- [ ] Make front end
- [ ] Assess rating for best player
