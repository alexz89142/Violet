#include "parser.h"

static bool violet_is_char_whitespace(char c)
{
    return (c == ' ' || c == '\t' || c == '\v' || 
        c == '\f' || c == '\n' || c == '\r');
}

static bool violet_is_char_endspace(char c)
{
    return (c == '\0' || c == '\n' || c == '\r');
}

static bool violet_is_char_symbol(char c)
{
    return (c == '*');
}

static bool violet_check_for_bold(char *current_stream)
{
    char c;
    while (c = *++current_stream, !violet_is_char_endspace(c))
    {
        if (c == '*' && *++current_stream == '*')
        {
            return true;
        }
    }

    return false;
}

static bool violet_check_for_italics(char *current_stream)
{
    char c;
    while (c = *++current_stream, !violet_is_char_endspace(c))
    {
        if (c == '*')
        {
            return true;
        }
    }

    return false;
}

static void violet_finish_line(token_t **token_buffer, token_t **end_token_buffer)
{
    if (sb_len(*end_token_buffer) > 0)
    {
        for (int i = sb_len(*end_token_buffer) - 1; i >= 0; --i)
        {
            token_t ctt = {0};
            ctt.type = end_token_buffer[i]->type;
            ctt.count = end_token_buffer[i]->count;
            ctt.is_end_node = true;

            sb_push(*token_buffer, ctt);
        }

        sb_free(*end_token_buffer);
        *end_token_buffer = NULL;
    }
}

static token_t *violet_parse_stream(char *stream)
{
    token_t *token_buffer = NULL;
    token_t *end_token_buffer = NULL;

    while (*stream)
    {
        token_t current_token = {0};
        // TODO: rename endspace
        bool endspace = false;

        switch (*stream)
        {
            case '\n':
            case '\r':
            {
                if (sb_len(end_token_buffer) == 0)
                {
                    while (violet_is_char_endspace(*++stream));
                    break;
                }

                endspace = true;
                violet_finish_line(&token_buffer, &end_token_buffer);
                while (violet_is_char_endspace(*++stream));
            } break;

            case ' ':
            case '\t':
            case '\v':
            case '\f':
            {
                while (violet_is_char_whitespace(*++stream));
            } break;

            case '#':
            {
                ++current_token.count;
                while (*++stream == '#')
                {
                    ++current_token.count;
                }

                if (*stream != ' ')
                {
                    current_token.type = TT_paragraph;
                    stream -= current_token.count;
                    current_token.count = 0;
                    current_token.start = stream;
                }
                else
                {
                    current_token.type = TT_header;
                    current_token.start = ++stream;
                }

                char c;
                while (c = *stream++, 
                       !(violet_is_char_symbol(c) || violet_is_char_endspace(c)))
                {
                    current_token.len++;
                }
                --stream;
            } break;

            case '*':
            {
                bool two_asterix = *++stream == '*';
                token_type_t last_etb_element_type = end_token_buffer ?
                    end_token_buffer[sb_len(end_token_buffer) - 1].type : TT_NULL;

                if (two_asterix && end_token_buffer && 
                    last_etb_element_type == TT_bold)
                {
                    sb_pop(end_token_buffer);
                    sb_push(token_buffer, (token_t) {
                        .type = TT_bold, 
                        .is_end_node = true
                    });

                    endspace = true;
                    ++stream;
                    break;
                }
                else if (end_token_buffer && 
                         last_etb_element_type == TT_italics)
                {
                    sb_pop(end_token_buffer);
                    sb_push(token_buffer, (token_t) {
                        .type = TT_italics, 
                        .is_end_node = true
                    });

                    endspace = true;
                    break;
                }
                
                if (two_asterix)
                {
                    if (violet_check_for_bold(++stream))
                    {
                        current_token.type = TT_bold;
                        current_token.start = stream;
                        current_token.count = 2;
                    }
                    else
                    {
                        stream -= 2;
                        current_token.type = TT_continue;
                        current_token.start = stream;
                        current_token.count = 0;
                    }
                }
                else
                {
                    if (violet_check_for_italics(stream))
                    {
                        current_token.type = TT_italics;
                        current_token.start = stream;
                        current_token.count = 1;
                    }
                    else
                    {
                        current_token.type = TT_continue;
                        current_token.start = --stream;
                        current_token.count = 0;
                    }
                }

                char c;
                while (c = *stream++,
                       !(violet_is_char_symbol(c) || violet_is_char_endspace(c)))
                {
                    current_token.len++;
                }
                --stream;
            } break;

            case '0': case '1': 
            case '2': case '3': 
            case '4': case '5': 
            case '6': case '7': 
            case '8': case '9':
            {
            } break;

            default:
            {
                if (sb_len(end_token_buffer) == 0)
                {
                    current_token.type = TT_paragraph;
                }
                else
                {
                    current_token.type = TT_continue;
                }

                current_token.start = stream;

                char c;
                while (c = *stream++, 
                       !(violet_is_char_symbol(c) || violet_is_char_endspace(c)))
                {
                    current_token.len++;
                }
                --stream;
            } break;
        }

        if (!endspace && current_token.type != TT_NULL)
        {
            sb_push(token_buffer, current_token);

            if (current_token.type != TT_continue)
            {
                sb_push(end_token_buffer, current_token);
            }
        }
    }

    violet_finish_line(&token_buffer, &end_token_buffer);
    return token_buffer;
}
