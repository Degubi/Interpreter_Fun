#include "assert.h"
#include "interface.h"

static void run_test(const char* filePath, int result) {
    printf("Running test: %s\n", filePath);

    FILE* source_file = fopen(filePath, "r");
    Function functions[MAX_FUNCTION_COUNT];
    CompilationInfo compilation_info = compile_file(source_file, filePath, functions);

    assert(interpret_int_function(functions, compilation_info.function_count, compilation_info.main_index, filePath) == result);

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

    printf("\nAll done!");
    return 0;
}