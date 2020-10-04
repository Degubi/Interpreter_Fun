#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "compiler.h"

typedef struct {
    const char* name;
    Instruction instruction;
    int stack_bump;
    int argument_count;
} InstructionInfo;

static const InstructionInfo instructions[] = {
    { .name = "return",      .instruction = i_ret,         .stack_bump = -1,  .argument_count = 0 },
    { .name = "push",        .instruction = i_push_int,    .stack_bump = 1,   .argument_count = 1 },
    { .name = "dup",         .instruction = i_dup,         .stack_bump = 1,   .argument_count = 0 },
    { .name = "add",         .instruction = i_add,         .stack_bump = -1,  .argument_count = 0 },
    { .name = "sub",         .instruction = i_sub,         .stack_bump = -1,  .argument_count = 0 },
    { .name = "mul",         .instruction = i_mul,         .stack_bump = -1,  .argument_count = 0 },
    { .name = "div",         .instruction = i_div,         .stack_bump = -1,  .argument_count = 0 },
    { .name = "cmp",         .instruction = i_cmp,         .stack_bump = -1,  .argument_count = 0 },
    { .name = "goto",        .instruction = i_goto,        .stack_bump = 0,   .argument_count = 1 },
    { .name = "goto_equal",  .instruction = i_goto_equal,  .stack_bump = 0,   .argument_count = 1 },
    { .name = "print_stack", .instruction = i_print_stack, .stack_bump = 0,   .argument_count = 0 }
};

static InstructionInfo convert_to_instruction_info(char* str) {
    for(int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); ++i) {
        if(strcmp(instructions[i].name, str) == 0) {
            return instructions[i];
        }
    }
    
    fprintf(stderr, "Unknown instruction found! '%s'\n", str);
    exit(420);
}

typedef struct {
    char* name;

    union {
        int replacement_index;
        int insertion_index;
    };
} Label;

static char* str_strip(char *str) {
    while(isspace(*str)) str++;

    char* back = str + strlen(str);
    while(isspace(*--back));
    *(back + 1) = '\0';

    return str; 
}

static bool str_is_blank(char* str) {
    for(char current; (current = *str) != '\0'; ++str) {
        if(!isspace(current)) {
            return false;
        }
    }
    return true;
}

static bool is_number(char* kek) {
    for (char current; (current = *kek) != '\0'; ++kek) {
        if (!isdigit(current)) {
            return false;
        }
    }
    return true;
}

CompileResult compile_file(const char* filePath) {
    Instruction* instructions_to_return = malloc(255 * sizeof(Instruction));
    int instruction_index = 0;

    Label label_relocation_table[10];
    int label_relocation_table_index = 0;
    Label label_rewrite_positions[10];
    int label_rewrite_positions_index = 0;

    int current_max_locals = 0;
    int final_max_locals = 0;

    char line[64];
    FILE* file = fopen(filePath, "r");

    while(fgets(line, sizeof(line), file)) {
        if(!str_is_blank(line)) {
            char* first_instruction = str_strip(strtok(line, " "));
            int instruction_length = strlen(first_instruction);

            if(first_instruction[instruction_length - 1] == ':'){
                char* instruction_copy = malloc(instruction_length * sizeof(char));
                strncpy(instruction_copy, first_instruction, instruction_length - 1);
                instruction_copy[instruction_length - 1] = '\0';

                label_relocation_table[label_relocation_table_index++] = (Label){ .name = instruction_copy, .replacement_index = instruction_index };
            }else{
                InstructionInfo info = convert_to_instruction_info(first_instruction);

                instructions_to_return[instruction_index++] = info.instruction;

                current_max_locals += info.stack_bump;
                if(current_max_locals > final_max_locals) {
                    final_max_locals = current_max_locals;
                }

                int parsed_argument_count = 0;
                while(1) {
                    char* optional_token = strtok(NULL, " ");

                    if(optional_token == NULL) {
                        break;
                    }

                    char* arg_or_label = str_strip(optional_token);
                    ++parsed_argument_count;

                    if(is_number(arg_or_label)) {
                        instructions_to_return[instruction_index++] = atoi(arg_or_label);
                    }else{
                        char* label_copy = malloc((strlen(arg_or_label) + 1) * sizeof(char));
                        strcpy(label_copy, arg_or_label);

                        label_rewrite_positions[label_rewrite_positions_index++] = (Label){ .name = label_copy, .insertion_index = instruction_index++ };
                    }
                }

                if(parsed_argument_count != info.argument_count) {
                    fprintf(stderr, "Found instruction '%s' with wrong number of arguments! Expected: '%d' got '%d'\n", first_instruction, info.argument_count, parsed_argument_count);
                    exit(420);
                }
            }
        }
    }

    for(int i = 0; i < label_rewrite_positions_index; ++i) {
        Label to_rewrite = label_rewrite_positions[i];

        for(int k = 0; k < label_relocation_table_index; ++k) {
            Label current_relocation = label_relocation_table[k];

            // TODO: Handle undefined labels
            if(strcmp(to_rewrite.name, current_relocation.name) == 0) {
                instructions_to_return[to_rewrite.insertion_index] = current_relocation.replacement_index;
                break;
            }
        }
    }

    fclose(file);
    return (CompileResult){ .instructions = instructions_to_return, .max_locals = final_max_locals };
}