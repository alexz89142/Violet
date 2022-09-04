#include "md_lexer.h"

static const char *g_stream;

static token_t ml_get_next_token(void)
{
    token_t token;
    switch (*g_stream)
    {
        case '\0':
        {
            token.type = TT_end;
        } break;

        case ' ':
        {
            token.type = TT_space;
            token.start = g_stream;
            while (is_space(*g_stream))
            {
                g_stream++;
            }
            token.end = g_stream;
        } break;

        case '#':
        {
            token.type = TT_hashtag;
            token.start = g_stream;
            while (*g_stream == '#')
            {
                g_stream++;
            }
            token.end = g_stream;
        } break;

        case '0': case '1': 
        case '2': case '3': 
        case '4': case '5': 
        case '6': case '7': 
        case '8': case '9':
        {
            token.type = TT_digit;
            token.start = g_stream;
            while (is_numeric(*g_stream))
            {
                g_stream++;
            }
            token.end = g_stream;
        } break;

        default:
        {
            token.type = TT_text;
            token.start = g_stream;
            while (!is_endspace(*g_stream))
            {
                g_stream++;
            }
            token.end = g_stream;
        } break;
    }

    return token;
}

static token_t *ml_lex_file(const char *filename)
{
    char *file_data = read_entire_file(filename);
    g_stream = file_data;

    token_t *token_buffer = NULL;
    token_t token = {0};

    while (token.type != TT_end)
    {
        token = ml_get_next_token();
        sb_push(token_buffer, token);
    }

    free(file_data);
    return token_buffer;
}
