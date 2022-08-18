#ifndef VIOLET_H
#define VIOLET_H

#include "util.h"
#include "md_lexer.h"

#define DEFAULT_TB_SIZE 1024

typedef struct token_buffer_t
{
    token_t *tokens;
    uint32_t count;
    uint32_t capacity;
} token_buffer_t;

#endif // VIOLET_H