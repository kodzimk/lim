#include"lim.h"
#define LIM_IMPLEMENTATION

const char* limE = "lim n->-10: n";

int main()
{
	lim_translate_source(cstr_to_st(limE), &lim);
	lim_calculate_expressions(&lim);
	printf("%" PRId64 "\n", lim.result);
	return 0;
}