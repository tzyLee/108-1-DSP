#include "disambig.hpp"
#include <fstream>
#include <string>

int main(int argc, const char *argv[]) {
  std::ifstream segmented(argv[1]);
  std::ofstream ofs(argv[4]);
  std::string buf;
  Disambig dis(2); // bigram

  dis.readLanguageModel(argv[3]);
  dis.readMap(argv[2]);

  while (std::getline(segmented, buf)) {
    buf.back() = '\0'; // Make the resulting pointer null terminated
    dis.getline(&buf[0]);
    dis.viterbi(ofs);
  }
  ofs.flush();
  return 0;
}