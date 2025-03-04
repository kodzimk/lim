#ifndef LIM_H
#define LIM_H

#include<stdint.h>
#include<stdbool.h>
#include<stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include<string.h>
#include<assert.h>
#include <inttypes.h>

#define LIM_VALUE_MAX_COUNT 256
#define LIM_EXPRESS_BLOCK_MAX_COUNT 256
#define LIM_EXPRESS_MAX_COUNT 2

typedef enum {
	NONE = 0,
	ADD,
	MINUS,
}Oper_Types;

typedef enum {
	ERR_INVALID_STRUCT_OF_LIM,
	ERR_OKAY,
}Err;


typedef struct  {
	const char* data;
	size_t count;
}String_t;

bool st_eq(String_t a, String_t b);
bool is_number(String_t st);
int st_to_int(String_t *st);
String_t cstr_to_st(const char* cstr);
String_t st_trim_left(String_t line);
String_t st_trim_right(String_t line);
String_t st_trim(String_t line);
String_t chop_by_delim(String_t *line, char delimmator);

typedef struct {
	String_t value;
	Oper_Types oper_type;
}Block;

typedef struct {
	Block expr_block[LIM_EXPRESS_BLOCK_MAX_COUNT];
	size_t block_count;
}Express;

typedef struct {
	String_t var;
	int64_t var_value;

	Express expr[LIM_EXPRESS_MAX_COUNT];
	size_t expr_size;

	int64_t expr_value[LIM_EXPRESS_MAX_COUNT];
	size_t value_size;

	int64_t result;
}Lim;

Lim lim = { 0 };

void print_expr(Lim* lim, int index);
void lim_translate_source(String_t src,Lim *lim);
void set_expr_blocks(Lim *lim, String_t* expr_line);
Err check_struct_of_lim(String_t* src, Lim* lim);
Err lim_calculate_expressions(Lim* lim);
int64_t lim_calc_expr(Express* expr, String_t var, int64_t var_value);

#endif


#ifndef LIM_IMPLEMENTATION

bool is_number(String_t st)
{
	for (size_t i = 0; i < st.count; i++)
	{
		if (!isdigit(st.data[i]))
		{ return false; }
	}

	return true;
}

void print_expr(Lim* lim,int index)
{
		for (size_t i = 0; i < lim->expr[index].block_count; ++i)
		{
			printf("Index: %d  Value: ", (int)i);
			for (size_t j = 0; j < lim->expr[index].expr_block[i].value.count; j++)
			{
				printf("%c", lim->expr[index].expr_block[i].value.data[j]);
			}
		    printf("\n");
		}
}

int64_t lim_calc_expr(Express* expr,String_t var,int64_t var_value)
{
	int64_t value = 0;
	for (size_t i = 0; i < expr->block_count; i++)
	{
		int64_t temp = 0;
		if (st_eq(expr->expr_block[i].value, var)) {
			printf("VAR\n");
			temp = var_value;
		}
		else if (is_number(expr->expr_block[i].value)) {
			printf("DIGIT\n");
		    temp += st_to_int(&expr->expr_block[i].value);
		}

		if (i != 0) {
			if (expr->expr_block[i].oper_type == ADD) {
				value += temp;
			}
			else if (expr->expr_block[i].oper_type == MINUS) {
				value -= temp;
			}
		}
		else {
			value += temp;
		}
	}
	return value;
}

Err lim_calculate_expressions(Lim* lim)
{
	for (size_t i = 0; i < lim->expr_size; ++i)
	{
		lim->expr_value[lim->value_size++] = lim_calc_expr(&lim->expr[i], lim->var, lim->var_value);
		lim->result += lim->expr_value[lim->value_size - 1];
	}
	
	return ERR_OKAY;
}

int st_to_int(String_t *st)
{

	int sum = 0;
	bool isMinus = false;
	if (*st->data == '-') {
		isMinus = true;
		st->data += 1;
		st->count -= 1;
	}

	if (!isdigit(*st->data))
		return -1;

	for (size_t i = 0; i < st->count && isdigit(st->data[i]); ++i)
	{
		sum = sum * 10 + st->data[i] - '0';
	}

	if(isMinus)
	   sum = -sum;

	return sum;
}

String_t cstr_to_st(const char* cstr)
{
	return (String_t) { .count = strlen(cstr), .data = cstr };
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

String_t chop_by_delim(String_t *st, char delimmator)
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

Err check_struct_of_lim(String_t *src,Lim *lim)
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

	lim->var_value = (int64_t)st_to_int(&line);
    
	return  ERR_OKAY;
}

void set_expr_blocks(Lim *lim,String_t *expr_line)
{
	size_t block_size = 0;
	while (expr_line->count > 0) {
		String_t block = chop_by_delim(expr_line, ' ');
		lim->expr[lim->expr_size].expr_block[block_size]= (Block){ .value = block,.oper_type = NONE };     

		if (*expr_line->data == '+') {
			lim->expr[lim->expr_size].expr_block[block_size].oper_type = ADD;
			expr_line->data += 1;
			expr_line->count -= 1;
		}
		else if (*expr_line->data == '-') {
			lim->expr[lim->expr_size].expr_block[block_size].oper_type = MINUS;
			expr_line->data += 1;
			expr_line->count -= 1;
		}
		*expr_line = st_trim(*expr_line);
		block_size += 1;
	}
	lim->expr[lim->expr_size].block_count = block_size;
	lim->expr_size += 1;
}

void lim_translate_source(String_t src, Lim* lim)
{
	if (check_struct_of_lim(&src,lim) != ERR_OKAY) {
		fprintf(stderr, "ERR: PROBLEM WITH STRUCT OF LIM\n");
		exit(1);
	}

	while (src.count > 0) {
		assert(lim->expr_size < LIM_EXPRESS_MAX_COUNT);
		String_t expr_line = st_trim(chop_by_delim(&src, '('));
		set_expr_blocks(lim, &expr_line);
		print_expr(lim,lim->expr_size - 1);
	}
}


#endif