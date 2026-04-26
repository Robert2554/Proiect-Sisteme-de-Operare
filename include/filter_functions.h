#ifndef FILTER_FUNCTIONS_H
#define FILTER_FUNCTIONS_H

#include "city_manager.h"
#include <stdlib.h>
#include <string.h>


int parse_condition(const char *input, char *field, char *op, char *value);
int compare_ints(long a, long b, const char *op);
int compare_strings(const char *a, const char *b, const char *op);
int match_condition(Report *r, const char *field, const char *op, const char *value);
#endif