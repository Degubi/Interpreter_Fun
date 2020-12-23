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

    Function functions[MAX_FUNCTION_COUNT];
    CompilationInfo compilation_info = compile_file(file_to_execute, argv[1], functions);
    int result = interpret_int_function(functions, compilation_info.function_count, compilation_info.main_index, argv[1]);

    printf("Function returned: %d\n", result);
    fclose(file_to_execute);
    return 0;
}