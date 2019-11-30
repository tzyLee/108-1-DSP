#include "Ngram.h"
#include "big5char.hpp"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <unordered_map>
#include <vector>

struct Node {
  const char *character;
  LogP logProb = LogP_Zero;
  int prev = 0; // for backtrack

  Node(const char *c, LogP p = LogP_Zero) : character(c), logProb(p) {}
};

struct Disambig {
  std::unordered_map<Big5Char, std::vector<Big5Char>> map;
  std::vector<std::vector<Node>> delta;
  std::vector<Big5Char> line;
  Vocab voc;
  Ngram lm;
  int _order;

  Disambig(int order)
      : map(37), delta(), line(), voc(), lm(voc, order), _order{order} {
  } // there are 37 ZhuYin

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

  void getline(char *s) {
    Big5Char temp;
    line.clear();
    // Add <s> to the beginning of line
    temp.setValue(Vocab_SentStart);
    line.push_back(temp);
    // Use strtok to tokenize line
    char *token = std::strtok(s, " ");
    while (token != NULL) {
      temp.setChar(token);
      line.push_back(temp);
      token = std::strtok(NULL, " ");
    }
  }

  void initializeDelta() {
    delta.resize(line.size() + 1); // +1 for </s>
    int i = 0;
    for (const Big5Char &c : line) {
      if (!c.isZhuYin()) {
        delta[i].emplace_back(c);
      } else {
        auto &observations = map[c];
        delta[i].reserve(observations.size());
        for (const Big5Char &obs : observations)
          delta[i].emplace_back(obs);
      }
      ++i;
    }
    // <s> is always the start of sentence
    delta[0][0].logProb = LogP_One;
    // </s> is always the end of sentence
    delta.back().emplace_back(Vocab_SentEnd, LogP_Zero);
  }

  void viterbi(std::ostream &os) {
    // Initialization
    initializeDelta();
    VocabIndex context[_order];
    context[_order - 1] = Vocab_None;
    // Induction
    for (int j = 1; j < delta.size(); ++j)
      for (Node &cur : delta[j]) {
        auto &prevNodes = delta[j - 1];
        for (int i = 0; i < prevNodes.size(); ++i) {
          context[0] = index(prevNodes[i].character);
          LogP logProb =
              prevNodes[i].logProb + lm.wordProb(index(cur.character), context);
          if (logProb > cur.logProb) {
            // Use the path with max probability
            cur.logProb = logProb;
            cur.prev = i; // for backtrack
          }
        }
      }
    // Backtrack
    int cur = delta.back()[0].prev;
    for (int i = delta.size() - 2; i > 0; --i) {
      // Replace ZhuYin by character
      if (line[i].isZhuYin()) // isZhuYin
        line[i].setValue(delta[i][cur].character);
      cur = delta[i][cur].prev;
    }
    // Output
    for (int i = 0; i < line.size(); ++i)
      os << line[i] << ' ';
    os << Vocab_SentEnd << std::endl;
    delta.clear();
  }

  VocabIndex index(const char *c) {
    VocabIndex ind = voc.getIndex(c);
    if (ind == Vocab_None)
      ind = voc.unkIndex();
    return ind;
  }
};
