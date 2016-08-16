#include <cherrypick-core.h>


int cp_picker::parse_args(int argc, char *argv[]) {
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
	bool OPT_MERGE_STACKS = false;
	bool OPT_CLEAR_HEAP_EXTRA = false;
	for (int j = 3; j < argc; ++j)
		if (strstr(argv[j], "--merge-stacks") != NULL)
			OPT_MERGE_STACKS = true;
		else if (strstr(argv[j], "--clear-heap-extra") != NULL)
			OPT_CLEAR_HEAP_EXTRA = true;

	filename = argv[1];
	pattern = argv[2];

	fi = fopen(filename, "r");
	if (!fi) return 3;
	
	return 0;
}

void cp_picker::initialize() {
	char s[MAX_LEN];
	lns = 0;
	while (fgets(s, MAX_LEN, fi)) ++lns;
	fclose(fi);
	fi = fopen(filename, "r");
	printf("Total lines = %llu\n", lns);
	
	mem_peak = 0;
	mem_heap.reserve(1000);
	mem_stacks.reserve(1000);
}

void cp_picker::cherrypick() {
	lnNo_t i, len;
	sz_o.resize(lns);
	sz_f.resize(lns);
	lnNo_t iter_heap = 0;
	lnNo_t iter_snap = 0;
	char s[MAX_LEN], nm_snap[MAX_LEN];
	bool rd_success = fgets(s, MAX_LEN, fi);
	do {
		vector<cp_state> stk;
		while (rd_success && s[0] != '#') rd_success = fgets(s, MAX_LEN, fi);
		if (!rd_success) break;
		mem_heap.push_back(0);
		mem_stacks.push_back(0);
		fgets(nm_snap, MAX_LEN, fi);
//		puts(nm_snap);
		while (fgets(s, MAX_LEN, fi) && s[0] == '#') ;
		while (fgets(s, MAX_LEN, fi) && strstr(s, "=") != NULL) {
			if (OPT_MERGE_STACKS) {
				if (strstr(s, "mem_stacks_B") != NULL) {
					for (i = 0; s[i] != '='; ++i) ;
					mem_stacks[iter_snap] = 0;
					for (i = i + 1; s[i] != '\n'; ++i)
						mem_stacks[iter_snap] = mem_stacks[iter_snap] * 10 + s[i] - 48;
				}
			}
		}
		if (feof(fi)) // empty heap tree and end-of-file
			break;
		if (s[0] == '#') // empty heap tree, move on to the next snapshot
			continue;
		int cd = -1, d;
		bool match;
		do {
			for (d = 0; s[d] == ' '; ++d) ;
			for (len = d; s[len]; ++len) ;
			char *s_real = s + d;
//			printf(s_real);
			while (d <= cd) {
				if (!stk[cd].match) {
					if (stk[cd].isLeaf)
						sz_f[stk[cd].line] = 0;
					if (cd > 0)
						sz_f[stk[cd - 1].line] -= (sz_o[stk[cd].line] - sz_f[stk[cd].line]);
				}	
				--cd;
				stk.pop_back();
			}
			sz_f[iter_heap] = sz_o[iter_heap] = getSz(s_real);
			match = (cd >= 0 ? stk[cd].match : false) || regex_search(s_real, regex(pattern));
			if (cd >= 0) stk[cd].isLeaf = false;
			stk.push_back( cp_state(iter_heap, match, true) );
			++iter_heap;
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
		mem_heap[iter_snap++] = sz_f[stk[0].line];
	} while (rd_success);
	mem_heap.push_back(0);
	mem_stacks.push_back(0);
//	for (i = 0; i < iter_heap; ++i) printf("%llu\n", sz_f[i]);
	fclose(fi);
}

void cp_picker::forge() {
	lnNo_t i, len;
	fo = fopen((string(filename) + ".cherry").c_str(), "w");
	fi = fopen(filename, "r");
	lnNo_t iter_heap = 0;
	lnNo_t iter_snap = 0;
	char s[MAX_LEN], nm_snap[MAX_LEN];
	bool rd_success = fgets(s, MAX_LEN, fi);
	do {
		while (rd_success && s[0] != '#') {
			fprintf(fo, s);
			rd_success = fgets(s, MAX_LEN, fi);
		}
		if (!rd_success) break;
		fprintf(fo, s);

		fgets(nm_snap, MAX_LEN, fi);
		fprintf(fo, nm_snap);
		
		while (fgets(s, MAX_LEN, fi) && s[0] == '#')
			fprintf(fo, s);
		fprintf(fo, s); // mem_heap_B not on the first line, so just replace below
		while (fgets(s, MAX_LEN, fi) && strstr(s, "=") != NULL) {
			if (strstr(s, "mem_heap_B") != NULL) {
				dtSz_t mem_tot = mem_heap[iter_snap] + (OPT_MERGE_STACKS ? mem_stacks[iter_snap] : 0);
				fprintf(fo, "mem_heap_B=%llu\n", mem_tot);
				mem_peak = max(mem_peak, mem_tot);
			} else if (strstr(s, "mem_heap_extra_B") != NULL && OPT_CLEAR_HEAP_EXTRA)
				fprintf(fo, "mem_heap_extra_B=0\n");
			else if (strstr(s, "mem_stacks_B") != NULL && OPT_MERGE_STACKS)//
				fprintf(fo, "mem_stacks_B=0\n");
			else
				fprintf(fo, s);
		}
		if (feof(fi)) // empty heap tree and end-of-file
			break;
		if (s[0] == '#') // empty heap tree
			continue;
		
		do {
//			printf("%llu %s\n", sz_f[iter_heap], s);
			printSz(fo, s, sz_f[iter_heap] + (s[0] != ' ' && OPT_MERGE_STACKS ? mem_stacks[iter_snap] : 0));
			++iter_heap;
		} while ( (rd_success = fgets(s, MAX_LEN, fi)) && s[0] != '#');
		++iter_snap;
	} while (rd_success);
}

dtSz_t cp_picker::get_mem_peak() {
	return mem_peak;
}


inline dtSz_t getSz(const char *s) {
	int i;
	dtSz_t ret = 0;
	for (i = 0; s[i] != ':'; ++i) ;
	for ( ; s[i] != ' '; ++i) ;
	for ( ; s[i] < 48 || s[i] > 57; ++i) ;
	for ( ; s[i] >= 48 && s[i] <= 57; ++i)
		ret = ret * 10 + s[i] - 48;
//	printf(s);
//	printf("%llu\n", ret);
	return ret;
}

inline void printSz(FILE *fo, const char *s, dtSz_t sz_new) {
	int i;
	char dgts[50];
	if (sz_new > 0) {
		for (i = 0; sz_new; sz_new /= 10)
			dgts[i++] = sz_new % 10;
	} else {
		dgts[0] = 0;
		i = 1;
	}
	int top = i - 1;
	for (i = 0; s[i] != ':'; ++i) fputc(s[i], fo);
	for ( ; s[i] != ' '; ++i) fputc(s[i], fo);  fputc(s[i], fo);
	int pos = i + 1;
	for (i = top; i >= 0; --i) fputc(dgts[i] + 48, fo);
	for (i = pos; s[i] >= 48 && s[i] <= 57; ++i) ;
	for ( ; s[i]; ++i) fputc(s[i], fo);
}

