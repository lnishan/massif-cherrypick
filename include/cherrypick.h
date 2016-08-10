#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <regex>

using std::vector;
using std::string;
using std::max;
using std::regex;
using std::regex_match;

typedef unsigned long long lnNo_t;
typedef unsigned long long dtSz_t;

struct cp_state {
	cp_state(lnNo_t _line, bool _match, bool _isLeaf = true): line(_line), match(_match), isLeaf(_isLeaf) {}

	bool isLeaf;
	lnNo_t line;
	bool match;
};

class cp_picker {
	public:
		cp_picker(): 
			OPT_MERGE_STACKS(false),
			OPT_CLEAR_HEAP_EXTRA(false) {

		}
		int parse_args(int argc, char *argv[]);
		void initialize();
		void cherrypick();
		void forge();

		dtSz_t get_mem_peak();
	
	private:
static const int MAX_LEN  = 1002;

		FILE *fi, *fo;

		char *filename;
		char *pattern;
		bool OPT_MERGE_STACKS;
		bool OPT_CLEAR_HEAP_EXTRA;

		lnNo_t lns;

		vector<dtSz_t> sz_o;
		vector<dtSz_t> sz_f;
		vector<dtSz_t> mem_heap;
		vector<dtSz_t> mem_stacks;
		dtSz_t mem_peak;
};

inline dtSz_t getSz(const char *s);
inline void printSz(FILE *fo, const char *s, dtSz_t sz_new);

