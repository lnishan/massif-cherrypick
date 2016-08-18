#ifndef MASSIF_CHERRYPICK_CORE_H
#define MASSIF_CHERRYPICK_CORE_H

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

using std::vector;
using std::string;
using std::max;
using std::regex;
using std::regex_match;

typedef unsigned long long lineNo_t;
typedef unsigned long long dataSz_t;

struct cp_state {
  cp_state(lineNo_t _line, bool _match, bool _isLeaf = true)
      : line(_line), match(_match), isLeaf(_isLeaf) {}

  bool isLeaf;
  lineNo_t line;
  bool match;
};

class cp_picker {
 public:
  cp_picker() : OPT_MERGE_STACKS(false), OPT_CLEAR_HEAP_EXTRA(false) {}
  int parse_args(int argc, char* argv[]);
  void initialize();
  void cherrypick();
  void forge();

  dataSz_t get_mem_peak();

 private:
  static const int MAX_LEN = 1002;

  FILE *fi, *fo;

  char* filename;
  char* pattern;
  bool OPT_MERGE_STACKS;
  bool OPT_CLEAR_HEAP_EXTRA;

  lineNo_t lns;

  vector<dataSz_t> sz_o;
  vector<dataSz_t> sz_f;
  vector<dataSz_t> mem_heap;
  vector<dataSz_t> mem_stacks;
  dataSz_t mem_peak;
};

inline dataSz_t getSz(const char* s);
inline void printSz(FILE* fo, const char* s, dataSz_t sz_new);

#endif
