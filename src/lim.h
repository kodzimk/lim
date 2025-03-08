#ifndef LIM_H
#define LIM_H

#include<stdint.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include <ctype.h>
#include<string.h>
#include<assert.h>
#include <inttypes.h>

#define LIM_VALUE_MAX_COUNT 256
#define LIM_EXPRESS_BLOCK_MAX_COUNT 25
#define LIM_EXPRESS_MAX_COUNT 2

typedef enum {
	NONE = 0,
	ADD,
	MINUS,
	MULT,
	DIV,
}Oper_Types;

typedef enum {
	ERR_INVALID_STRUCT_OF_LIM,
	ERR_OKAY,
}Err;


typedef struct {
	const char* data;
	size_t count;
}String_t;

bool st_eq(String_t a, String_t b);
bool is_number(String_t st);
int st_to_int(String_t st);
int level_of_oper(Oper_Types type);
String_t int_to_st(int64_t number);
String_t cstr_to_st(const char* cstr);
String_t st_trim_left(String_t line);
String_t st_trim_right(String_t line);
String_t st_trim(String_t line);
String_t chop_by_delim(String_t* line, char delimmator);

typedef struct NodeExpr NodeExpr;

typedef struct {
	int64_t value;
	Oper_Types type;
}Expr;


struct NodeExpr {
	NodeExpr* next;
	Expr expr;
};

typedef struct {
	String_t var;
	int64_t var_value;

	int64_t expr_value[LIM_EXPRESS_MAX_COUNT];
	size_t value_size;

	NodeExpr* up_expr;
	NodeExpr* under_expr;

	int64_t result;
}Lim;

Lim lim = { 0 };

void lim_translate_source(String_t src, Lim* lim);
void set_expr_blocks(Lim* lim, String_t* expr_line,NodeExpr *up_expr);
Err check_struct_of_lim(String_t* src, Lim* lim);
Err lim_calculate_expressions(Lim* lim);
Err free_node_expr(NodeExpr* expr);
NodeExpr* lim_calculate_level_two(NodeExpr* exp, Lim* lim);
NodeExpr* lim_calculate_level_three(NodeExpr* exp, Lim* lim);

#endif

#ifndef LIM_IMPLEMENTATION

bool is_number(String_t st)
{
	for (size_t i = 0; i < st.count; i++)
	{
		if (!isdigit(st.data[i]))
		{
			return false;
		}
	}

	return true;
}

bool st_eq(String_t a, String_t b)
{
	if (a.count != b.count) {
		return 0;
	}
	else {
		return memcmp(a.data, b.data, a.count) == 0;
	}
}

int level_of_oper(Oper_Types type)
{
	switch (type) {
	case NONE: return -1;
	case ADD: return 1;
	case MINUS:return 1;
	case MULT: return 2;
	case DIV:return 2;
	}
	return -1;
}

Err free_node_expr(NodeExpr* expr)
{
	NodeExpr* temp;

	while (expr != NULL) {
		temp = expr;
		expr = expr->next;
		free(temp);
	}

	return ERR_OKAY;
}

NodeExpr* lim_calculate_level_three(NodeExpr* exp, Lim* lim) {
	return NULL;
}

NodeExpr* lim_calculate_level_two(NodeExpr* exp, Lim* lim) {
	while (exp->expr.type != NONE && level_of_oper(exp->expr.type) == 2) {
		if (level_of_oper(exp->next->expr.type) > 2) {
			NodeExpr* tmp = exp;
			tmp->next = lim_calculate_level_three(exp, lim);
			exp = tmp;
		}
		else {
			Expr expr = exp->expr;
			exp = exp->next;

			if (expr.type == MULT) {
				expr.value *= exp->expr.value;
			}
			else if (expr.type == DIV) {
				expr.value /= exp->expr.value;
			}

			exp->expr.value = expr.value;
		}
	}

	return exp;
}

Err lim_calculate_expressions(Lim* lim)
{
	NodeExpr* temp = lim->up_expr;
	while (temp->expr.type != NONE) {
		if (level_of_oper(temp->next->expr.type) == 2) {
			NodeExpr* tmp = temp;
			tmp->next = lim_calculate_level_two(temp->next, lim);
			temp = tmp;

			printf("VALUE: %" PRId64 "\n", tmp->next->expr.value);
		}
		else if (level_of_oper(temp->expr.type) == 2) {
			NodeExpr* tmp = temp;
			tmp = lim_calculate_level_two(temp, lim);
			temp = tmp;

			printf("VALUE: %" PRId64 "\n", tmp->next->expr.value);
		}

		if (level_of_oper(temp->next->expr.type) < 2) {
			Expr expr = temp->expr;
			temp = temp->next;

			if (expr.type == ADD) {
				expr.value += temp->expr.value;
			}
			else if (expr.type == MINUS) {
				expr.value -= temp->expr.value;
			}

			temp->expr.value = expr.value;
		}
	}

	lim->result = temp->expr.value;

	return ERR_OKAY;
}

int st_to_int(String_t st)
{

	int sum = 0;
	bool isMinus = false;
	if (*st.data == '-') {
		isMinus = true;
		st.data += 1;
		st.count -= 1;
	}

	if (!isdigit(*st.data))
		return -1;

	for (size_t i = 0; i < st.count && isdigit(st.data[i]); ++i)
	{
		sum = sum * 10 + st.data[i] - '0';
	}

	if (isMinus)
		sum = -sum;

	return sum;
}

String_t int_to_st(int64_t number)
{
	size_t size = 0;
	int64_t temp = number;

	while (temp > 0) {
		temp /= 10;
		size++;
	}

	if (number == 0)
		size = 1;

	char num[100];
	for (size_t i = 0; i < size; i++)
	{
		temp = number % 10;
		number /= 10;
		num[i] = temp + '0';
	}

	return (String_t) { .count = size, .data = num };
}

String_t cstr_to_st(const char* cstr)
{
	return (String_t) { .count = strlen(cstr), .data = cstr };
}

String_t st_trim_left(String_t st)
{
	size_t i = 0;
	while (i < st.count && isspace(st.data[i])) {
		i += 1;
	}

	return (String_t) {
		.count = st.count - i,
			.data = st.data + i,
	};
}

String_t st_trim_right(String_t st)
{
	size_t i = 0;
	while (i < st.count && isspace(st.data[st.count - 1 - i])) {
		i += 1;
	}

	return (String_t) {
		.count = st.count - i,
			.data = st.data
	};
}

String_t st_trim(String_t line)
{
	return st_trim_right(st_trim_left(line));
}

String_t chop_by_delim(String_t* st, char delimmator)
{
	size_t i = 0;
	while (i < st->count && st->data[i] != delimmator) {
		i += 1;
	}

	String_t result = {
		.count = i,
		.data = st->data,
	};

	if (i < st->count) {
		st->count -= i + 1;
		st->data += i + 1;
	}
	else {
		st->count -= i;
		st->data += i;
	}

	return result;
}

Err check_struct_of_lim(String_t* src, Lim* lim)
{
	String_t line = st_trim_left(chop_by_delim(src, ':'));
	if (line.count == src->count) {
		return ERR_INVALID_STRUCT_OF_LIM;
	}

	String_t lim_express = chop_by_delim(&line, ' ');
	if (!st_eq(lim_express, cstr_to_st("lim"))) {
		return ERR_INVALID_STRUCT_OF_LIM;
	}

	String_t var = st_trim(chop_by_delim(&line, '-'));
	if (*line.data != '>') {
		return ERR_INVALID_STRUCT_OF_LIM;
	}
	else {
		line.data += 1;
		line.count -= 1;
		lim->var = var;
	}

	lim->var_value = (int64_t)st_to_int(line);

	return  ERR_OKAY;
}

void set_expr_blocks(Lim* lim, String_t* expr_line,NodeExpr *up_expr)
{
	NodeExpr* temp = up_expr;
	while (expr_line->count > 0) {
		String_t block = chop_by_delim(expr_line, ' ');
		temp->expr = (Expr){ .value = 0,.type = NONE };

		if (is_number(block)) {
			temp->expr.value = st_to_int(block);
		}
		else {
			temp->expr.value = lim->var_value;
		}

		if (*expr_line->data == '+') {
			temp->expr.type = ADD;
			expr_line->data += 1;
			expr_line->count -= 1;
		}
		else if (*expr_line->data == '-') {
			temp->expr.type = MINUS;
			expr_line->data += 1;
			expr_line->count -= 1;
		}
		else if (*expr_line->data == '*') {
			temp->expr.type = MULT;
			expr_line->data += 1;
			expr_line->count -= 1;
		}
		else if (*expr_line->data == '/') {
			temp->expr.type = DIV;
			expr_line->data += 1;
			expr_line->count -= 1;
		}

		*expr_line = st_trim(*expr_line);
		temp->next = (NodeExpr*)malloc(sizeof(NodeExpr));
		temp = temp->next;
	}

}

void lim_translate_source(String_t src, Lim* lim)
{
	if (check_struct_of_lim(&src, lim) != ERR_OKAY) {
		fprintf(stderr, "ERR: PROBLEM WITH STRUCT OF LIM\n");
		exit(1);
	}

	NodeExpr* up_expr = (NodeExpr*)malloc(sizeof(NodeExpr));
	while (src.count > 0) {
		String_t expr_line = st_trim(chop_by_delim(&src, '\n'));
		set_expr_blocks(lim, &expr_line,up_expr);
		lim->up_expr = up_expr;
	}
}


#endif