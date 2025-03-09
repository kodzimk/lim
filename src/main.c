#include"lim.h"
#define LIM_IMPLEMENTATION

#include<stdio.h>
#include<stdint.h>

const char* limE = "lim n->5: n * 2 - 3 / 10 | 4 / 2 * 4 + 2";

int main()
{
	lim_translate_source(cstr_to_st(limE), &lim);
	lim.up_result = lim_calculate_expressions(&lim,lim.up_expr);
	lim.down_result = lim_calculate_expressions(&lim, lim.under_expr);

	if(lim.down_result != 0)
	printf("Value: %f\n", lim.up_result / lim.down_result);
	else
		printf("Value: %f\n", lim.up_result);

	free_node_expr(lim.up_expr);
	free_node_expr(lim.under_expr);
	return 0;
}