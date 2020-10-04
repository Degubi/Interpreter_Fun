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

static InstructionInfo convert_to_instruction_info(char* str, int line_counter) {
    for(int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); ++i) {
        if(strcmp(instructions[i].name, str) == 0) {
            return instructions[i];
        }
    }
    
    fprintf(stderr, "Unknown instruction found on line %d: '%s'\n", line_counter, str);
    exit(420);
}

typedef struct {
    char* name;

    int line_number;
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

static bool is_number(char* str) {
    for (char current; (current = *str) != '\0'; ++str) {
        if (!isdigit(current)) {
            return false;
        }
    }
    return true;
}

static Label find_label_by_name(Label* relocation_table, int relocation_table_size, char* to_find) {
    for(int i = 0; i < relocation_table_size; ++i) {
        if(strcmp(to_find, relocation_table[i].name) == 0) {
            return relocation_table[i];
        }
    }
    return (Label){ .name = NULL };
}

static char* copy_label_declaration(char* original, int length) {
    char* copy = malloc(length * sizeof(char));
    strncpy(copy, original, length - 1);
    copy[length - 1] = '\0';
    return copy;
}

static char* copy_label_usage(char* original) {
    char* copy = malloc((strlen(original) + 1) * sizeof(char));
    strcpy(copy, original);
    return copy;
}

CompileResult compile_file(FILE* source_file) {
    Instruction* instructions_to_return = malloc(255 * sizeof(Instruction));
    int instructions_index = 0;

    Label label_relocation_table[10];
    int label_relocation_table_index = 0;
    Label label_rewrite_positions[10];
    int label_rewrite_positions_index = 0;

    int current_max_locals = 0;
    int final_max_locals = 0;

    char line[64];
    int line_counter = 0;

    while(fgets(line, sizeof(line), source_file)) {
        ++line_counter;

        if(!str_is_blank(line)) {
            char* instruction = str_strip(strtok(line, " "));
            int instruction_length = strlen(instruction);

            if(instruction[instruction_length - 1] == ':') {
                label_relocation_table[label_relocation_table_index++] = (Label){
                    .name = copy_label_declaration(instruction, instruction_length),
                    .replacement_index = instructions_index,
                    .line_number = line_counter
                };
            }else{
                InstructionInfo info = convert_to_instruction_info(instruction, line_counter);

                instructions_to_return[instructions_index++] = info.instruction;

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

                    char* argument = str_strip(optional_token);
                    ++parsed_argument_count;

                    if(is_number(argument)) {
                        instructions_to_return[instructions_index++] = atoi(argument);
                    }else{
                        label_rewrite_positions[label_rewrite_positions_index++] = (Label){
                            .name = copy_label_usage(argument),
                            .insertion_index = instructions_index++,
                            .line_number = line_counter
                        };
                    }
                }

                if(parsed_argument_count != info.argument_count) {
                    fprintf(stderr, "Found instruction '%s' with wrong number of arguments on line %d, expected: '%d' got '%d'\n",
                                    instruction, line_counter, info.argument_count, parsed_argument_count);
                    exit(420);
                }
            }
        }
    }

    for(int i = 0; i < label_rewrite_positions_index; ++i) {
        Label to_rewrite = label_rewrite_positions[i];
        Label replacement_label = find_label_by_name(label_relocation_table, label_relocation_table_index, to_rewrite.name);

        if(replacement_label.name == NULL) {
            fprintf(stderr, "Unknown label found on line %d: '%s'\n", to_rewrite.line_number, to_rewrite.name);
            exit(420);
        }

        instructions_to_return[to_rewrite.insertion_index] = replacement_label.replacement_index;
        free(to_rewrite.name);
    }

    for(int i = 0; i < label_relocation_table_index; ++i) {
        free(label_relocation_table[i].name);
    }

    return (CompileResult){ .instructions = instructions_to_return, .max_locals = final_max_locals };
}