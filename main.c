#include "interface.h"

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

    CompilationResult compilation_result = compile_file(file_to_execute, argv[1]);
    Function* functions = compilation_result.functions;
    int result = interpret_int_function(functions, compilation_result.function_count, compilation_result.main_index, argv[1]);

    printf("Function returned: %d\n", result);

    for(int i = 0; i < compilation_result.function_count; ++i) {
        free(functions[i].instructions);
    }

    free(functions);
    fclose(file_to_execute);
    return 0;
}