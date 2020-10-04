#ifndef interpreter_h_guard
#define interpreter_h_guard

typedef enum {
    i_ret = 0,
    i_push_int = 1,
    i_dup = 10,
    i_add = 2,
    i_sub = 3,
    i_mul = 4,
    i_div = 5,
    i_cmp = 6,
    i_goto = 7,
    i_goto_equal = 8,
    i_print_stack = 9
} Instruction;

int interpret_int(Instruction* instructions, int max_locals);

#endif