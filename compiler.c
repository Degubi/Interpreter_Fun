#include "interface.h"

typedef struct {
    const char* name;
    Instruction instruction;
    int stack_bump;
    int argument_count;
} InstructionInfo;

typedef struct {
    char* name;
    int source_line_number;
    int index;
} Label;

typedef struct {
    Label original_positions[10];
    Label rewrite_positions[10];
    int instructions_index;
    int current_max_locals;
    int original_positions_index;
    int rewrite_positions_index;
} FunctionCompilationState;

static const InstructionInfo instructions[] = {
    { .name = "return",      .instruction = i_ret,         .stack_bump = -1,  .argument_count = 0 },
    { .name = "call",        .instruction = i_call,        .stack_bump = 1,   .argument_count = 1 },
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

static char* str_strip(char *str) {
    while(isspace(*str)) str++;

    char* back = str + strlen(str);
    while(isspace(*--back));
    *(back + 1) = '\0';

    return str; 
}

static bool str_is_number(char* str) {
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

static char* copy_str_n(char* original, int length) {
    char* copy = malloc(length * sizeof(char));
    strncpy(copy, original, length - 1);
    copy[length - 1] = '\0';
    return copy;
}

static char* copy_str(char* original) {
    char* copy = malloc((strlen(original) + 1) * sizeof(char));
    strcpy(copy, original);
    return copy;
}

CompilationResult compile_file(FILE* source_file, const char* source_file_path) {
    Function* functions = malloc(10 * sizeof(Function));
    FunctionCompilationState function_compilation_states[10];

    char line[64];
    int line_counter = 0;
    int function_counter = -1;

    while(fgets(line, sizeof(line), source_file)) {
        char* stripped_line = str_strip(line);
        ++line_counter;

        if(stripped_line[0] != '\0') {
            int line_length = strlen(stripped_line);
            char last_character = stripped_line[line_length - 1];

            if(last_character == '{') {
                ++function_counter;
                stripped_line[line_length - 1] = '\0';

                functions[function_counter] = (Function){
                    .name = copy_str(str_strip(stripped_line)),  // Re-Strip after removing '{'
                    .max_locals = 0,
                    .instructions = malloc(255 * sizeof(Instruction))
                };

                function_compilation_states[function_counter] = (FunctionCompilationState){
                    .instructions_index = 0,
                    .original_positions_index = 0,
                    .rewrite_positions_index = 0,
                    .current_max_locals = 0
                };
            }else if(last_character == '}') {
                
            }else if(last_character == ':') {
                FunctionCompilationState* state = &function_compilation_states[function_counter];
                
                state->original_positions[state->original_positions_index++] = (Label){
                    .name = copy_str_n(stripped_line, line_length),
                    .index = state->instructions_index,
                    .source_line_number = line_counter
                };
            }else{
                char* instruction = strtok(stripped_line, " ");
                InstructionInfo info = convert_to_instruction_info(instruction, line_counter);
                FunctionCompilationState* state = &function_compilation_states[function_counter];
                Function* function = &functions[function_counter];

                function->instructions[state->instructions_index++] = info.instruction;

                state->current_max_locals += info.stack_bump;
                if(state->current_max_locals > function->max_locals) {
                    function->max_locals = state->current_max_locals;
                }

                int parsed_argument_count = 0;
                while(1) {
                    char* optional_token = strtok(NULL, " ");
                    if(optional_token == NULL) {
                        break;
                    }

                    char* argument = str_strip(optional_token);
                    ++parsed_argument_count;

                    if(str_is_number(argument)) {
                        function->instructions[state->instructions_index++] = atoi(argument);
                    }else if(strcmp(instruction, "call") == 0) {
                        
                    }else{
                        state->rewrite_positions[state->rewrite_positions_index++] = (Label){
                            .name = copy_str(argument),
                            .index = state->instructions_index++,
                            .source_line_number = line_counter
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


    int main_index = -1;
    for(int f = 0; f <= function_counter; ++f) {
        Function* function = &functions[f];
        FunctionCompilationState* state = &function_compilation_states[f];

        if(strcmp(function->name, "main") == 0) {
            main_index = f;
        }

        for(int i = 0; i < state->rewrite_positions_index; ++i) {
            Label to_rewrite = state->rewrite_positions[i];
            Label replacement_label = find_label_by_name(state->original_positions, state->original_positions_index, to_rewrite.name);

            if(replacement_label.name == NULL) {
                fprintf(stderr, "Unknown label found on line %d: '%s'\n", to_rewrite.source_line_number, to_rewrite.name);
                exit(420);
            }

            function->instructions[to_rewrite.index] = replacement_label.index;
            free(to_rewrite.name);
        }

        for(int i = 0; i < state->original_positions_index; ++i) {
            free(state->original_positions[i].name);
        }
    }

    if(main_index == -1) {
        fprintf(stderr, "No main function found in file: %s\n", source_file_path);
        exit(420);
    }

    return (CompilationResult){ .functions = functions, .function_count = function_counter + 1 };
}