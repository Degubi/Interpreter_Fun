#include "interpreter.h"

int compare(int firstArg, int secondArg) {
    return firstArg < secondArg ? -1 : firstArg > secondArg ? 1 : 0;
}

// Inspired by: https://youtu.be/IAdLwUXRUvg?t=1885

int interpret_int(Instruction* instructions, int max_locals) {
    int stack[max_locals];
    int stack_ptr = 0;
    Instruction* instruction_ptr = instructions;
    const void* label_lookup[] = {
        [i_ret] = &&iret,
        [i_push_int] = &&ipush_int,
        [i_add] = &&iadd,
        [i_sub] = &&isub,
        [i_mul] = &&imul,
        [i_div] = &&idiv,
        [i_goto] = &&igoto,
        [i_goto_equal] = &&i_goto_equal,
        [i_cmp] = &&icmp,
        [i_print_stack] = &&iprint_stack
    };

    goto* label_lookup[*instruction_ptr];

    // Stack ops
    ipush_int:
        stack[stack_ptr++] = *(++instruction_ptr);
        goto* label_lookup[*(++instruction_ptr)];

    // Math ops
    iadd:
        stack[stack_ptr - 2] = stack[stack_ptr - 2] + stack[stack_ptr - 1];
        --stack_ptr;
        goto* label_lookup[*(++instruction_ptr)];
    isub:
        stack[stack_ptr - 2] = stack[stack_ptr - 2] - stack[stack_ptr - 1];
        --stack_ptr;
        goto* label_lookup[*(++instruction_ptr)];
    imul:
        stack[stack_ptr - 2] = stack[stack_ptr - 2] * stack[stack_ptr - 1];
        --stack_ptr;
        goto* label_lookup[*(++instruction_ptr)];
    idiv:
        stack[stack_ptr - 2] = stack[stack_ptr - 2] / stack[stack_ptr - 1];
        --stack_ptr;
        goto* label_lookup[*(++instruction_ptr)];

    // Control flow ops
    igoto:
        instruction_ptr = instructions + *(++instruction_ptr);
        goto* label_lookup[*instruction_ptr];
    i_goto_equal:
        if(stack[--stack_ptr] == 0) {
            goto igoto;
        }else{
            goto* label_lookup[*(++instruction_ptr)]; 
        }
    icmp:
        stack[stack_ptr - 2] = compare(stack[stack_ptr - 2], stack[stack_ptr - 1]);
        --stack_ptr;
        goto* label_lookup[*(++instruction_ptr)];
    iret:
        if(stack_ptr != 1) {
            printf("Stack was not empty at return!");
        }

        return stack[stack_ptr - 1];

    // Debug ops
    iprint_stack:
        printf("Stack depth: %d\n", stack_ptr);
        printf("Current instruction: %d\n", *instruction_ptr);

        for(int i = stack_ptr - 1; i >= 0; --i) {
            printf("Index: %d, element: %d\n", i, stack[i]);
        }
        goto* label_lookup[*(++instruction_ptr)];
}