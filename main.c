#include "stdio.h"
#include "stdlib.h"
#include "interpreter.h"
#include "compiler.h"

int main(int argc, char* argv[]) {
    if(argc == 1) {
        puts("No file to run was given!\n");
        return 1;
    }

    FILE* file_to_execute = fopen(argv[1], "r");

    if(file_to_execute == NULL) {
        puts("The given file doesn't exist!\n");
        return 2;
    }

    CompileResult compile_result = compile_file(file_to_execute);
    int result = interpret_int(compile_result.instructions, compile_result.max_locals);

    printf("Function returned: %d\n", result);

    free(compile_result.instructions);
    fclose(file_to_execute);
    return 0;
}