#include "md_lexer.h"

static const char *g_stream;

static void ml_init_stream(char *stream)
{
    g_stream = stream;
}

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
    ml_init_stream(file_data);

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

/*
static token_t ml_get_next_token_2(void)
{
    token_t token;

    switch (*g_stream)
    {
        case '\0':
        {
            token.type = TT_end;
        } break;

        case '\n':
        {
            token.type = TT_newline;
            token.start = g_stream;
            token.end = ++g_stream;
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

        case '*':
        {
            token.type = TT_astrix;
            token.start = g_stream;
            token.end = ++g_stream;
        } break;

        case '_':
        {
            token.type = TT_underscore;
            token.start = g_stream;
            token.end = ++g_stream;
        } break;

        case '-':
        {
            token.type = TT_dash;
            token.start = g_stream;
            token.end = ++g_stream;
        } break;

        case '>':
        {
            token.type = TT_close_chevron;
            token.start = g_stream;
            token.end = ++g_stream;
        } break;

        case '`':
        {
            token.type = TT_grave_accent;
            token.start = g_stream;
            token.end = ++g_stream;
        } break;

        case '#':
        {
            token.start = g_stream;

            int count = 0;
            while (*g_stream == '#')
            {
                g_stream++;
                count++;
            }

            if (*g_stream == ' ' && count >= 1 && count <= 3)
            {
                token.end = g_stream;
                switch (count)
                {
                    case 1: token.type = TT_h1; break;
                    case 2: token.type = TT_h2; break;
                    case 3: token.type = TT_h3; break;
                }
            }
            else
            {
                while (is_alpha_numeric(*g_stream))
                {
                    g_stream++;
                }
                token.end = g_stream;
                token.type = TT_text;
            }
        } break;

        default:
        {
            token.start = g_stream;
            // TODO: The space check is only for now
            while (is_alpha_numeric(*g_stream))
            {
                g_stream++;
            }
            token.end = g_stream;
            token.type = TT_text;
        } break;
    }

    return token;
}
*/
