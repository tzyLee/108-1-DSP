#include "Ngram.h"
#include "big5char.hpp"
#include <cstdio>
#include <fstream>
#include <unordered_map>
#include <vector>

struct Disambig {
  std::unordered_map<Big5Char, std::vector<Big5Char>> map;
  Vocab voc;
  Ngram lm;

  Disambig(int order) : map(37), voc(), lm(voc, order) {} // there are 37 ZhuYin

  void readLanguageModel(const char *filename) {
    File lmfile(filename, "r");
    lm.read(lmfile);
    lmfile.close();
  }

  void readMap(const char *filename) {
    std::ifstream ifs(filename);
    Big5Char charBuf;
    while (ifs) {
      ifs >> charBuf;
      if (charBuf.isZhuYin()) {
        map[charBuf].reserve(256); // heuristic size
        for (Big5Char temp; ifs.get() != '\n';) {
          ifs >> temp;
          map[charBuf].push_back(temp);
        }
      } else {
        ifs.ignore(4); // ' ' + Big5Char + '\n'
      }
    }
  }
};
