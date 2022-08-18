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
