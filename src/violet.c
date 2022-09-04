#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "violet.h"
#include "md_lexer.c"
#include "md_parser.c"

// NOTE: Tempory for testing only
void print_token_buffer(token_t *token_buffer)
{
    for (uint32_t i = 0; i < (uint32_t)sb_len(token_buffer); ++i)
    {
        int token_type_int = (int)token_buffer[i].type;
        printf("%s\n", token_string_list[token_type_int]);
    }
}

// Core //

int main(int argc, char **argv)
{
    if (argc < 1)
    {
        for (int i = 0; i < argc; ++i)
        {
            printf("%s\n", argv[i]);
        }
    }

    token_t *tb = ml_lex_file("test/test.md");
    print_token_buffer(tb);
    return 0;
}
