#include "hmm.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 5) {
    std::cerr << "Not enough arguments\n";
    return -1;
  }
  int iteration = std::atoi(argv[1]);
  const char *model_init_path = argv[2];
  const char *seq_path = argv[3];
  const char *output_model_path = argv[4];

  HiddenMarkovModel model(50);
  model.loadParam(model_init_path);
  model.baumWelchAlgorithm(iteration, 10000, seq_path);
  model.dumpParam(output_model_path);
  model.release();
  return 0;
}