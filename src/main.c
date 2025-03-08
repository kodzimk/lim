#include"lim.h"
#define LIM_IMPLEMENTATION

#include<stdio.h>
#include<stdint.h>

const char* limE = "lim n->5: n * 2 + 1 - 2 + 2 * 2 / 4";

int main()
{
	lim_translate_source(cstr_to_st(limE), &lim);
	lim_calculate_expressions(&lim);
	printf("Value: %" PRId64 "\n", lim.result);
	free_node_expr(lim.up_expr);
	return 0;
}