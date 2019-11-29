#include "disambig.hpp"

int main(int argc, const char *argv[]) {
  const char *segmented = argv[1];
  const char *output = argv[4];
  Disambig dis(2);
  //   dis.readLanguageModel(argv[3]);
  dis.readMap(argv[2]);
  return 0;
}