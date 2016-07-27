#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <vector>

using std::vector;
using std::string;

typedef unsigned long long lnNo_t;
typedef unsigned long long dtSz_t;

struct cp_state {
	cp_state(lnNo_t _line, bool _match): line(_line), match(_match) {}

	lnNo_t line;
	bool match;
};

inline dtSz_t getSz(const char *s) {
	int i;
	dtSz_t ret = 0;
	for (i = 0; s[i] != ':'; ++i) ;
	for (i = 0; s[i] != ' '; ++i) ;
	for ( ; (s[i] < 48 || s[i] > 57) && s[i]; ++i) ;
	for ( ; s[i] >= 48 && s[i] <= 57; ++i)
		ret = ret * 10 + s[i] - 48;
//	printf(s);
//	printf("%llu\n", ret);
	return ret;
}

const int MAX_LEN  = 1002;

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "* Please specify the massif output file!\n");
		fprintf(stderr, "Usage: ./cherrypick {massif_output_file} {pattern}\n");
		return 1;
	}
	if (argc < 3) {
		fprintf(stderr, "* Please specify pattern!\n");
		fprintf(stderr, "Usage: ./cherrypick {massif_output_file} {pattern}\n");
		return 2;
	}

	FILE *fi = fopen(argv[1], "r");
	if (!fi) return 3;
	FILE *fo = fopen((string(argv[1]) + ".cherry").c_str(), "w");
	int i, len;
	char s[MAX_LEN], nm_snap[MAX_LEN];

	lnNo_t iter = 0, lns = 0;
	while (fgets(s, MAX_LEN, fi)) ++lns;
	fclose(fi);
	fi = fopen(argv[1], "r");
	printf("Total lines = %llu\n", lns);
	
	vector<dtSz_t> sz_o(lns);
	vector<dtSz_t> sz_f(lns);
	bool rd_success;

	rd_success = fgets(s, MAX_LEN, fi);
	do {
		vector<cp_state> stk;
		while (rd_success && s[0] != '#') rd_success = fgets(s, MAX_LEN, fi);
		if (!rd_success) break;
		fgets(nm_snap, MAX_LEN, fi);
//		puts(nm_snap);
		while (fgets(s, MAX_LEN, fi) && s[0] == '#') ;
		while (fgets(s, MAX_LEN, fi) && strstr(s, "=") != NULL) ;
		int cd = -1, d;
		bool match;
		do {
			for (d = 0; s[d] == ' '; ++d) ;
			for (len = d; s[len]; ++len) ;
			char *s_real = s + d;
//			printf(s_real);
			if (d <= cd && !stk[cd].match) {
				sz_f[stk[cd].line] -= sz_o[stk[cd].line];
				while (d <= cd && cd > 0 && !stk[cd].match) {
					sz_f[stk[cd - 1].line] -= sz_o[stk[cd].line];
					--cd;
					stk.pop_back();
				}
			}
			while (d <= cd) {
				--cd;
				stk.pop_back();
			}
			sz_f[iter] = sz_o[iter] = getSz(s_real);
			match = (cd >= 0 ? stk[cd].match : false) || ( strstr(s_real, argv[2]) != NULL );
			stk.push_back( cp_state(iter, match) );
			++iter;
			cd = d;
//			puts("");
		} while ( (rd_success = fgets(s, MAX_LEN, fi)) && s[0] != '#');
		if (cd > 0 && !stk[cd].match) {
			sz_f[stk[cd].line] -= sz_o[stk[cd].line];
			while (cd > 0 && !stk[cd].match) {
				sz_f[stk[cd - 1].line] -= sz_o[stk[cd].line];
				--cd;
				stk.pop_back();
			}
		}
	} while (rd_success);
	for (i = 0; i < iter; ++i)
		printf("%llu\n", sz_f[i]);
	return 0;
}
