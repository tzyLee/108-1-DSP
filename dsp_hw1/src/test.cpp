#include "hmm.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
  if (argc < 4) {
    std::cerr << "Not enough arguments\n";
    return -1;
  }
  const char *model_list_path = argv[1];
  const char *seq_path = argv[2];
  const char *output_result_path = argv[3];

  char sequences[2500][51];
  char modelPaths[5][13];
  FILE *seqFile = open_or_die(seq_path, "r");
  fread(sequences, 1, sizeof(sequences), seqFile);
  fclose(seqFile);

  FILE *modelListFile = open_or_die(model_list_path, "r");
  fread(modelPaths, 1, sizeof(modelPaths), modelListFile);
  fclose(seqFile);

  HiddenMarkovModel model(50);
  FILE *outFile = open_or_die(output_result_path, "w");
  for (int i = 0; i < 5; ++i) {
    model.loadParam(modelPaths[i]);
    double p = 0;
    for (int seq = 0; seq < 2500; ++seq) {
      p += model.viterbiAlgorithm(sequences[seq]);
    }
    model.release();
    fprintf(outFile, "%s %f\n", modelPaths[i], p / 2500);
  }
  fflush(outFile);
  fclose(outFile);
  return 0;
}
