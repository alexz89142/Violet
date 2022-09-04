#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "violet.h"
#include "buffer.c"
#include "md_lexer.c"
#include "md_parser.c"

static token_buffer_t g_token_buffer;

// NOTE: Tempory for testing only
void print_token_buffer(token_t *token_buffer)
{
    // for (uint32_t i = 0; i < token_buffer.count; ++i)
    {
        // token_t current_token = *(token_t *)(token_buffer.tokens + i);
        // printf("%d\n", (int)current_token.type);
    }
}

// Core //

int main(int argc, char **argv)
{
    token_buffer_t *tb = ml_lex_file("../test/test.md");
    print_token_buffer(tb);
    return 0;
}