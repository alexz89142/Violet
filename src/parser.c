#include "parser.h"

static bool violet_is_char_symbol(char c)
{
    return (c == '*');
}

static bool violet_is_char_endspace(char c)
{
    return (c == '\0' || c == '\n');
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
        // TODO: rename endspace
        bool endspace = false;

        // TODO: For testing purposes only
        int tracker_buffer_size = sb_len(tracker_buffer);

        switch (*stream)
        {
            case '\n':
            {
                if (tracker_buffer_size == 0)
                {
                    while (*++stream == '\n');
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
                --stream;
            } break;

            case '*':
            {
                bool two_asterix = *++stream == '*';
                token_type_t last_tracker_buffer_element_type = tracker_buffer ?
                    tracker_buffer[sb_len(tracker_buffer) - 1].type : TT_NULL;

                if (two_asterix && tracker_buffer && 
                    last_tracker_buffer_element_type == TT_bold)
                {
                    sb_pop(tracker_buffer);
                    tracker_buffer_size = sb_len(tracker_buffer);
                    token_t end_token = {0};
                    end_token.type = TT_bold;
                    end_token.is_end_node = true;
                    sb_push(token_buffer, end_token);
                    endspace = true;
                    ++stream;
                    break;
                }
                else if (tracker_buffer && last_tracker_buffer_element_type == TT_italics)
                {
                    sb_pop(tracker_buffer);
                    tracker_buffer_size = sb_len(tracker_buffer);
                    token_t end_token = {0};
                    end_token.type = TT_italics;
                    end_token.is_end_node = true;
                    sb_push(token_buffer, end_token);
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
                --stream;
            } break;
        }

        if (!endspace && current_token.type != TT_NULL)
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
