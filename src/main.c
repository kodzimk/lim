#include"lim.h"
#define LIM_IMPLEMENTATION

const char* limE = "lim n->2: n";

int main()
{
	lim_translate_source(cstr_to_st(limE), &lim);
	return 0;
}