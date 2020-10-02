#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

#include "compiler.h"

inline static char* strip_str(char *s) {
    while(isspace(*s)) s++;

    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back + 1) = '\0';

    return s; 
}

#define instr_case(str_to_check_against, cmp_str, ret) if(strcmp(str_to_check_against, cmp_str) == 0) return ret;

static Instruction convert_to_instruction(char* str) {
    instr_case(str, "return", i_ret)
    instr_case(str, "push", i_push_int);
    instr_case(str, "add", i_add);
    instr_case(str, "sub", i_sub);
    instr_case(str, "mul", i_mul);
    instr_case(str, "div", i_div);
    instr_case(str, "cmp", i_cmp);
    instr_case(str, "goto", i_goto);
    instr_case(str, "goto_if_equal", i_goto_equal);
    instr_case(str, "print_stack", i_print_stack);

    printf("Unknown instruction found! '%s'\n", str);
    exit(420);
}

Instruction* compile_file(const char* filePath) {
    Instruction* instructions = malloc(255 * sizeof(Instruction));
    int count = 0;

    char line[128];
    FILE* file = fopen(filePath, "r");

    while(fgets(line, sizeof(line), file)) {
        char* firstToken = strtok(line, ",");
        char* firstInstruction = strip_str(firstToken);

        // TODO: Kimenteni változóba és lehet ellenőrizni, hogy megfelelő számú arg van e
        instructions[count++] = convert_to_instruction(firstInstruction);

        while(1) {
            char* optionalArgToken = strtok(NULL, ",");

            if(optionalArgToken == NULL) {
                break;
            }

            instructions[count++] = atoi(optionalArgToken);
        }
    }

    fclose(file);
    return instructions;
}