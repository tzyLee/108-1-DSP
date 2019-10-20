#include "hmm.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  if (argc < 4) {
    std::cerr << "Not enough arguments\n";
    return -1;
  }
  std::ifstream modelPathFile(argv[1]), sequencesFile(argv[2]);
  if (!modelPathFile) {
    perror(argv[1]);
    exit(1);
  }
  if (!sequencesFile) {
    perror(argv[2]);
    exit(1);
  }
  std::vector<std::string> sequences;
  sequences.reserve(2500);
  const char *output_result_path = argv[3];
  std::string temp;

  while (std::getline(sequencesFile, temp))
    sequences.emplace_back(std::move(temp));

  std::vector<HiddenMarkovModel> models;
  models.reserve(5);
  for (int i = 0; std::getline(modelPathFile, temp); ++i) {
    models.emplace_back(50);
    models[i].loadParam(temp.c_str());
    models[i].prepareDelta();
  }

  FILE *outFile = open_or_die(output_result_path, "w");
  for (auto &&seq : sequences) {
    double max_p = 0;
    const char *max_name = nullptr;
    for (auto &&model : models) {
      double p = model.viterbiAlgorithm(seq.c_str());
      if (p > max_p) {
        max_p = p;
        max_name = model.model_name;
      }
    }
    fprintf(outFile, "%s %g\n", max_name, max_p);
  }
  fflush(outFile);
  fclose(outFile);
  for (int i = 0; i < 5; ++i)
    models[i].release();
  return 0;
}
