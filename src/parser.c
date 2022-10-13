#include "parser.h"

static bool violet_is_char_symbol(char c)
{
    return (c == '*');
}

static bool violet_is_char_endspace(char c)
{
    return (c == '\0' || c == '\n');
}

static void violet_finish_line(token_t **token_buffer, token_t **tracker_buffer)
{
    if (sb_len(*tracker_buffer) > 0)
    {
        for (int i = sb_len(*tracker_buffer) - 1; i >= 0; --i)
        {
            token_t ctt = {0};
            ctt.type = tracker_buffer[i]->type;
            ctt.count = tracker_buffer[i]->count;
            ctt.is_end_node = true;

            sb_push(*token_buffer, ctt);
        }

        sb_free(*tracker_buffer);
        *tracker_buffer = NULL;
    }
}

static token_t *violet_parse_stream(char *stream)
{
    token_t *token_buffer = NULL;
    token_t *tracker_buffer = NULL;

    while (*stream)
    {
        token_t current_token = {0};
        bool endspace = false;

        switch (*stream)
        {
            case '\n':
            {
                if (sb_len(tracker_buffer) == 0)
                {
                    break;
                }

                endspace = true;
                violet_finish_line(&token_buffer, &tracker_buffer);
                while (*++stream == '\n');
            } break;

            case ' ':
            {
                while (*++stream == ' ');
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
            } break;

            case '*':
            {
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
                if (sb_len(tracker_buffer) == 0)
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
            } break;
        }

        if (!endspace)
        {
            sb_push(token_buffer, current_token);

            if (current_token.type != TT_continue)
            {
                sb_push(tracker_buffer, current_token);
            }
        }
    }

    violet_finish_line(&token_buffer, &tracker_buffer);
    return token_buffer;
}
