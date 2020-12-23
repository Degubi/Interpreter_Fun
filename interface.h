#ifndef common_h_guard
#define common_h_guard

#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"

#define MAX_INSTRUCTION_COUNT 255
#define MAX_FUNCTION_COUNT 10

typedef enum {
    i_ret = 0,
    i_push_int = 1,
    i_dup = 10,
    i_call = 11,
    i_add = 2,
    i_sub = 3,
    i_mul = 4,
    i_div = 5,
    i_cmp = 6,
    i_goto = 7,
    i_goto_equal = 8,
    i_print_stack = 9
} Instruction;

typedef struct {
    char* name;
    Instruction instructions[MAX_INSTRUCTION_COUNT];
    int max_locals;
} Function;

typedef struct {
    int function_count;
    int main_index;
} CompilationInfo;

int interpret_int_function(Function* functions, int function_count, int function_index, const char* source_file_name);
CompilationInfo compile_file(FILE* source_file, const char* source_file_path, Function* outFunctions);

#endif