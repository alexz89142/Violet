#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "violet.h"
#include "util.h"
#include "md_lexer.c"

static token_buffer_t g_token_buffer;

// Tests //

void test_lexer(void)
{
    char *source = "# Hello  *World*\n\0";
    ml_init_stream(source);
    token_t token = {0};
    while (token.type != TT_end)
    {
        token = ml_get_next_token();
        printf("%d\n", (int)token.type);
    }
}

// Core //

int main(int argc, char **argv)
{
    test_lexer();
    return 0;
}