﻿#include"lim.h"
#define LIM_IMPLEMENTATION

#include<stdio.h>
#include<stdint.h>

const char* limE = "lim n->i: n  + 2 * 2 | n + (1 * 2)";

int main()
{
	lim_translate_source(cstr_to_st(limE), &lim);

	if (lim.var_value == INT_MAX &&
		((!lim.up_has_var && lim.down_has_var) ||
			(lim.up_has_var && !lim.down_has_var))) {
		fprintf(stderr, "There is no answer result will be infinity!!!\n");
		return 1;
	}

	lim.up_result = lim_calculate_expressions(&lim, lim.up_expr);
	lim.down_result = lim_calculate_expressions(&lim, lim.under_expr);

	if (lim.down_has_result && lim.down_result == 0) {
		fprintf(stderr, "Number cant be divided by 0!!!\n");
		free_node_expr(lim.up_expr);
		free_node_expr(lim.under_expr);
		return -1;
	}
	else {
		printf("Value: %f\n", lim.up_result);
	}

	free_node_expr(lim.up_expr);
	free_node_expr(lim.under_expr);
	return 0;
}