#include <stdio.h>
#include <stdlib.h>

#ifndef _GMO_UTILS_H_
#define _GMO_UTILS_H_



char *read_file(char *file_name);

void test_read_file(char *file_name);

char *string_concatenate(char *existing, char *to_add);
void test_concatenate(char *existing, char *to_add);
#endif
