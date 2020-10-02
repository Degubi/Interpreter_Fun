#include "interpreter.h"
#include "assert.h"

Instruction PUSH_TEST_CODE[] = {
    i_push_int, 10,
    i_ret
};

Instruction ADD_TEST_CODE[] = {
    i_push_int, 10,
    i_push_int, 50,
    i_add,
    i_ret
};

Instruction ADD2_TEST_CODE[] = {
    i_push_int, 10,
    i_push_int, 50,
    i_add,
    i_push_int, 40,
    i_add,
    i_ret
};

Instruction ADD_SUB_TEST_CODE[] = {
    i_push_int, 10,
    i_push_int, 20,
    i_add,
    i_push_int, 5,
    i_sub,
    i_ret
};

Instruction GOTO_TEST_CODE[] = {
    i_push_int, 10,
    i_push_int, 20,
    i_add,
    i_goto, 10,
    i_push_int, 5,
    i_sub,
    i_ret  // Jumping here
};

Instruction EQUAL_TEST_CODE[] = {
    i_push_int, 20,
    i_push_int, 20,
    i_push_int, 20,
    i_push_int, 20,
    i_cmp,
    i_goto_equal, 12,
    i_sub,
    i_add,  // Jumping here
    i_ret
};

int main() {
    assert(interpret_int(PUSH_TEST_CODE, 1) == 10);
    assert(interpret_int(ADD_TEST_CODE, 2) == 60);
    assert(interpret_int(ADD2_TEST_CODE, 2) == 100);
    assert(interpret_int(ADD_SUB_TEST_CODE, 2) == 25);
    assert(interpret_int(GOTO_TEST_CODE, 2) == 30);
    assert(interpret_int(EQUAL_TEST_CODE, 4) == 40);

    printf("All done!");
    return 0;
}