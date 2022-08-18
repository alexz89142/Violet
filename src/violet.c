#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "violet.h"
#include "md_lexer.c"

static token_buffer_t g_token_buffer;

// Token Buffer //

static void init_token_buffer(token_buffer_t *tb)
{
    tb->tokens = (token_t *)xmalloc(sizeof(token_t) * DEFAULT_TB_SIZE);
    tb->capacity = DEFAULT_TB_SIZE;
    tb->count = 0;
}

static void insert_token_to_token_buffer(token_buffer_t *tb, token_t t)
{
    if (tb->count == tb->capacity)
    {
        tb->capacity *= 2;
        tb->tokens = (token_t *)xrealloc(tb->tokens, sizeof(token_t) * DEFAULT_TB_SIZE);
    }

    memcpy(tb->tokens + tb->count++, &t, sizeof(token_t));
}

// Tests //

void test_lexer(void)
{
    init_token_buffer(&g_token_buffer);
    char *source = "# Hello  *World*\n\0";
    ml_init_stream(source);
    token_t token = {0};
    while (token.type != TT_end)
    {
        token = ml_get_next_token();
        insert_token_to_token_buffer(&g_token_buffer, token);
    }
}

void print_token_buffer(void)
{
    for (uint32_t i = 0; i < g_token_buffer.count; ++i)
    {
        token_t current_token = *(token_t *)(g_token_buffer.tokens + i);
        printf("%d\n", (int)current_token.type);
    }
}

void test_parser(void)
{
}

// Core //

int main(int argc, char **argv)
{
    test_lexer();
    print_token_buffer();
    test_parser();
    return 0;
}