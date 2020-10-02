#ifndef compiler_h_guard
#define compiler_h_guard

#include "interpreter.h"

Instruction* compile_file(const char* filePath);

#endif