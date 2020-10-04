#include "stdio.h"
#include "stdlib.h"
#include "assert.h"

#include "interpreter.h"
#include "compiler.h"

static void run_test(const char* filePath, int result) {
    CompileResult compile_result = compile_file(filePath);

    assert(interpret_int(compile_result.instructions, compile_result.max_locals) == result);

    free(compile_result.instructions);
}

int main() {
    run_test("tests/push.byt", 10);
    run_test("tests/add.byt", 60);
    run_test("tests/add2.byt", 100);
    run_test("tests/add_sub.byt", 25);
    run_test("tests/goto.byt", 30);
    run_test("tests/equals.byt", 40);

    printf("All done!");
    return 0;
}