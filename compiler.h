#ifndef compiler_h_guard
#define compiler_h_guard

#include "interpreter.h"

typedef struct {
    Instruction* instructions;
    int max_locals;
} CompileResult;

CompileResult compile_file(FILE* source_file);

#endif