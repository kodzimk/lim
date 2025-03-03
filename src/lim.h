#ifndef LIM_H
#define LIM_H

#include<stdint.h>
#include<stdbool.h>
#include<stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include<string.h>

typedef enum {
	ERR_INVALID_STRUCT_OF_LIM,
	ERR_OKAY,
}Err;

#define LIM_VALUE_MAX_COUNT 256


typedef struct  {
	const char* data;
	size_t count;
}String_t;

int st_to_int(String_t *st);
String_t cstr_to_st(const char* cstr);
String_t st_trim_left(String_t line);
String_t st_trim_right(String_t line);
String_t st_trim(String_t line);
bool st_eq(String_t a, String_t b);
String_t chop_by_delim(String_t *line, char delimmator);

typedef struct {
	int64_t value[LIM_VALUE_MAX_COUNT];
	size_t values_size;

	String_t var;
	int64_t lim_var_value;
}Lim;

Lim lim = { 0 };

Err check_struct_of_lim(String_t *src,Lim* lim);
void lim_translate_source(String_t src,Lim *lim);

#endif


#ifndef LIM_IMPLEMENTATION

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

	lim->lim_var_value = (int64_t)st_to_int(&line);
	printf("Value: %d\n", (int)lim->lim_var_value);
    
	return  ERR_OKAY;
}

void lim_translate_source(String_t src, Lim* lim)
{
	if (check_struct_of_lim(&src,lim) != ERR_OKAY) {
		fprintf(stderr, "ERR: PROBLEM WITH STRUCT OF LIM\n");
		exit(1);
	}
}


#endif