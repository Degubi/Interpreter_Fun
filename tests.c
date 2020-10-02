#include "interpreter.h"
#include "compiler.h"
#include "assert.h"
#include "stdlib.h"

static void run_test(const char* filePath, int maxLocals, int result) {
    Instruction* instructions = compile_file(filePath);

    assert(interpret_int(instructions, maxLocals) == result);

    free(instructions);
}

int main() {
    run_test("tests/push.byt", 1, 10);
    run_test("tests/add.byt", 2, 60);
    run_test("tests/add2.byt", 2, 100);
    run_test("tests/add_sub.byt", 2, 25);
    run_test("tests/goto.byt", 2, 30);
    run_test("tests/equals.byt", 4, 40);

    printf("All done!");
    return 0;
}