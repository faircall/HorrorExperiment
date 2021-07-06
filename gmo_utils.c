#define _CRT_SECURE_NO_WARNINGS
#include "gmo_utils.h"


char *read_file(char *file_name)
{
    char *result;
    FILE *fp = fopen(file_name, "rb");

    fseek(fp, 0, SEEK_END);
    int file_length = ftell(fp);
    
    rewind(fp);

    result = (char*)malloc(sizeof(char) * file_length);

    int bytes_read = fread((void*)result, sizeof(char), file_length, fp);
    if (bytes_read != file_length) {
	printf("something went horribly wrong reading the file\n");
	return NULL;
    }    
    
    fclose(fp);
    result[file_length] = 0;
    //do I need to write a terminator character? fucking always
    return result;
}


void test_read_file(char *file_name)
{
    char *test_result = read_file(file_name);

    int counter = 0;
    char c;
    while ((c = test_result[counter]) != '\0') {
	printf("%c", c);
	counter++;
    }

    free(test_result);
}

char *string_concatenate(char *existing, char *to_add)
{
    int len_orig = 0;
    int len_to_add = 0;
    char *result;
    //get the length of each,
    //make space a new string that can hold them together
    //then do the join
    
    char c;

    while ((c = existing[len_orig]) != '\0') {
	len_orig++;
    }
    
    while ((c = existing[len_to_add]) != '\0') {
	len_to_add++;
    }

    int len_new = len_orig + len_to_add + 1;//space for the null
    
    result = (char*)malloc(sizeof(char) * len_new);

    for (int i = 0; i < len_orig; i++) {
	result[i] = existing[i];
    }
    for (int i = 0; i < len_to_add; i++) {
	result[len_orig + i] = to_add[i];
    }
    result[len_new - 1] = '\0';
    return result;
}

void test_concatenate(char *existing, char *to_add)
{
    char *test = string_concatenate(existing, to_add);
    int counter = 0;
    char c;
    while ((c = test[counter]) != '\0') {
	printf("%c", c);
	counter++;
    }
    free(test);
}
