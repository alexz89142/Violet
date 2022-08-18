#ifndef VIOLET_H
#define VIOLET_H

#define TOKEN_ARRAY_SIZE 1024

typedef struct token_buffer_t
{
    token_t *tokens;
    uint32_t count;
} token_buffer_t;

#endif // VIOLET_H_