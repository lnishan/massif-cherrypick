#include <cherrypick-core.h>

int main(int argc, char* argv[]) {
  cp_picker me;

  int res_parse = me.parse_args(argc, argv);
  if (res_parse)  // invalid arguments
    return res_parse;

  me.initialize();

  printf("* Starts cherrypick-ing the input file ... ");
  me.cherrypick();
  puts("Done");

  // starts forging the new massif file
  printf("* Starts forging the new massif file ... ");
  me.forge();
  puts("Done");
  printf("Peak memory = %llu\n", me.get_mem_peak());

  return 0;
}
