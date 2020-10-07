#include "assert.h"
#include "interface.h"

static void run_test(const char* filePath, int result) {
    FILE* source_file = fopen(filePath, "r");
    CompilationResult compilation_result = compile_file(source_file, filePath);
    Function* functions = compilation_result.functions;

    assert(interpret_int_function(functions, compilation_result.function_count, compilation_result.main_index, filePath) == result);

    for(int i = 0; i < compilation_result.function_count; ++i) {
        free(functions[i].instructions);
    }

    free(functions);
    fclose(source_file);
}

int main() {
    run_test("tests/push.byt", 10);
    run_test("tests/add.byt", 60);
    run_test("tests/add2.byt", 100);
    run_test("tests/add_sub.byt", 25);
    run_test("tests/goto.byt", 30);
    run_test("tests/equals.byt", 40);
    run_test("tests/functions.byt", 30);

    printf("All done!");
    return 0;
}