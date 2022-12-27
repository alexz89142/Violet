#include "parser.h"

static bool violet_is_char_whitespace(char c)
{
    return (c == ' ' || c == '\t' || c == '\v' || c == '\f');
}

static bool violet_is_char_endspace(char c)
{
    return (c == '\n' || c == '\r');
}

static bool violet_is_char_symbol(char c)
{
    return (c == '*' || c == '`');
}

static bool violet_check_string_match(char *s1, char *s2, int match_num)
{
    while (match_num--)
    {
        if (*s1++ != *s2++)
        {
            return false;
        }
    }

    return true;
}

static bool violet_check_for_symbol_string(char *current_stream, char *symbol_stream)
{
    char c;
    while (c = *++current_stream, !violet_is_char_endspace(c))
    {
        if (c == *symbol_stream && violet_check_string_match(
            current_stream, symbol_stream, strlen(symbol_stream)))
        {
            return true;
        }
    }

    return false;
}

static void violet_shift_last_etb_element(parser_t *parser, token_type_t last_etb_element_type)
{
    token_t end = (token_t) {
        .type = last_etb_element_type,
        .is_end_node = true
    };

    sb_pop(parser->end_token_buffer);
    sb_push(parser->token_buffer, end);
    parser->should_push = false;
}

static void violet_finish_line(parser_t *parser)
{
    if (sb_len(parser->end_token_buffer) > 0)
    {
        for (int i = sb_len(parser->end_token_buffer) - 1; i >= 0; --i)
        {
            token_t ctt = (token_t) {
                .type = parser->end_token_buffer[i].type,
                .count = parser->end_token_buffer[i].count,
                .is_end_node = true
            };

            sb_push(parser->token_buffer, ctt);
        }

        sb_free(parser->end_token_buffer);
        parser->end_token_buffer = NULL;
    }

    parser->should_push = false;
}

static bool violet_check_if_valid_external_token(char *stream, external_token_type_t ett)
{
    int current_symbol_count = 1;
    int open_bracket = 0;
    int close_bracket = 0;
    int open_paren = 0;
    int close_paren = 0;

    char c;
    while (c = *stream++, !violet_is_char_endspace(c))
    {
        switch (c)
        {
            case '[':
            {
                open_bracket = current_symbol_count++;
            } break;

            case ']':
            {
                close_bracket = current_symbol_count++;

                // There is whitespace between the close bracket and open paren
                if (*++stream != '(')
                {
                    return false;
                }
            } break;

            case '(':
            {
                open_paren = current_symbol_count++;
            } break;

            case ')':
            {
                close_paren = current_symbol_count++;
            } break;

            case ' ':
            {
                // There cannot be a space in a link
                if (ett == Link && current_symbol_count > 2)
                {
                    return false;
                }
            } break;

            default: break;
        }
    }

    return (open_bracket == 1 && close_bracket == 2 &&
            open_paren == 3 && close_paren == 4);
}

static void violet_parse_stream(parser_t *parser, char *stream)
{
    while (*stream)
    {
        parser->current_token = (token_t){0};
        parser->should_push = true;

        switch (*stream)
        {
            case '\n':
            case '\r':
            {
                while (violet_is_char_endspace(*++stream));
                if (sb_len(parser->end_token_buffer) > 0)
                {
                    violet_finish_line(parser);
                }
            } break;

            case '#':
            {
                int symbol_count = 1;
                while (*++stream == '#')
                {
                    ++symbol_count;
                }

                if (*stream != ' ')
                {
                    parser->current_token = (token_t) {
                        .type = TT_paragraph,
                        .start = (stream -= symbol_count)
                    };
                }
                else
                {
                    parser->current_token = (token_t) {
                        .type = TT_header,
                        .count = symbol_count,
                        .start = ++stream
                    };
                }

                char c;
                while (c = *stream++,
                       !(violet_is_char_symbol(c) || violet_is_char_endspace(c)))
                {
                    ++parser->current_token.len;
                }
                --stream;
            } break;

            case '*':
            {
                bool two_asterix = (*++stream == '*');
                int etb_size = sb_len(parser->end_token_buffer);
                token_t last_etb_element = etb_size > 0 ?
                    parser->end_token_buffer[etb_size - 1] : (token_t){0};
                token_type_t last_etb_element_type = last_etb_element.type;

                if ((two_asterix && last_etb_element_type == TT_bold) ||
                    last_etb_element_type == TT_italics)
                {
                    if (last_etb_element_type == TT_bold)
                    {
                        ++stream;
                    }

                    violet_shift_last_etb_element(parser, last_etb_element_type);
                    break;
                }

                if (two_asterix && violet_check_for_symbol_string(++stream, "**"))
                {
                    parser->current_token = (token_t) {
                        .type = TT_bold,
                        .start = stream,
                        .count = 2
                    };
                }
                else if (violet_check_for_symbol_string(stream, "*"))
                {
                    parser->current_token = (token_t) {
                        .type = TT_italics,
                        .start = stream,
                        .count = 1
                    };
                }
                else
                {
                    int symbol_count = (int)two_asterix * 1 + 1;
                    parser->current_token = (token_t) {
                        .type = TT_continue,
                        .start = (stream -= symbol_count)
                    };
                }

                char c;
                while (c = *stream++,
                       !(violet_is_char_symbol(c) || violet_is_char_endspace(c)))
                {
                    ++parser->current_token.len;
                }
                --stream;
            } break;

            case '>':
            {
                if (*++stream != ' ')
                {
                    parser->current_token = (token_t) {
                        .type = TT_paragraph,
                        .start = --stream
                    };
                }
                else
                {
                    parser->current_token = (token_t) {
                        .type = TT_blockquote,
                        .start = ++stream
                    };
                }

                char c;
                while (c = *stream++,
                       !(violet_is_char_symbol(c) || violet_is_char_endspace(c)))
                {
                    ++parser->current_token.len;
                }
                --stream;
            } break;

            case '-':
            {
                int symbol_count = 1;
                while (*++stream == '-')
                {
                    ++symbol_count;
                }

                if (symbol_count == 1)
                {
                    // TODO: Unordered List
                }
                else if (symbol_count == 3)
                {
                    // TODO; Write test for break
                    parser->current_token = (token_t) {
                        .type = TT_paragraph,
                        .start = (stream -= symbol_count)
                    };

                    char c;
                    while (c = *stream++, violet_is_char_whitespace(c))
                    {
                        ++parser->current_token.len;
                    }

                    if (*stream == '\n')
                    {
                        if (violet_is_char_whitespace(*--stream))
                        {
                            parser->current_token.type = TT_break;
                        }
                        else
                        {
                            --parser->current_token.len;
                        }
                    }
                }
                else
                {
                    parser->current_token = (token_t) {
                        .type = TT_paragraph,
                        .start = (stream -= symbol_count)
                    };
                }

                char c;
                while (c = *stream++,
                       !(violet_is_char_symbol(c) || violet_is_char_endspace(c)))
                {
                    ++parser->current_token.len;
                }
                --stream;
            } break;

            case '!':
            case '[':
            {
                external_token_type_t ett = Link;
                if (*stream == '!')
                {
                    stream++;
                    ett = Image;
                }

                parser->current_token = (token_t) {
                    .type = TT_paragraph,
                    .start = --stream
                };

                if (violet_check_if_valid_external_token(stream, ett))
                {
                    parser->current_token.type = TT_link;
                    parser->current_token.external.secondary = ++stream;

                    char c;
                    while (c = *stream++, c != ']')
                    {
                        // TODO: Check if this count is correct
                        parser->current_token.external.secondary_count++;
                    }

                    parser->current_token.external.primary = ++stream;

                    while (c = *stream++, c != ')')
                    {
                        // TODO: Check if this count is correct
                        parser->current_token.external.primary_count++;
                    }

                    // TODO: Figure out if need to continue here
                }
                else
                {
                    // TODO: TT_paragraph
                }
            } break;

            case '`':
            {
                int etb_size = sb_len(parser->end_token_buffer);
                token_t last_etb_element = etb_size > 0 ?
                    parser->end_token_buffer[etb_size - 1] : (token_t){0};
                token_type_t last_etb_element_type = last_etb_element.type;

                if (last_etb_element_type == TT_code)
                {
                    violet_shift_last_etb_element(parser, last_etb_element_type);
                    ++stream;
                    break;
                }

                if (violet_check_for_symbol_string(++stream, "`"))
                {
                    parser->current_token = (token_t) {
                        .type = TT_code,
                        .start = stream,
                        .count = 1
                    };
                }
                else
                {
                    parser->current_token = (token_t) {
                        .type = TT_paragraph,
                        .start = --stream
                    };
                }

                char c;
                while (c = *stream++,
                       !(violet_is_char_symbol(c) || violet_is_char_endspace(c)))
                {
                    ++parser->current_token.len;
                }
                --stream;
            } break;

            case '1': case '2': case '3':
            case '4': case '5': case '6':
            case '7': case '8': case '9':
            {
            } break;

            default:
            {
                parser->current_token = (token_t) {
                    .type = (sb_len(parser->end_token_buffer) == 0) ?
                        TT_paragraph : TT_continue,
                    .start = stream
                };

                char c;
                while (c = *stream++,
                       !(violet_is_char_symbol(c) || violet_is_char_endspace(c)))
                {
                    ++parser->current_token.len;
                }
                --stream;
            } break;
        }

        if (parser->should_push && parser->current_token.type != TT_NULL)
        {
            sb_push(parser->token_buffer, parser->current_token);

            if (parser->current_token.type != TT_continue)
            {
                sb_push(parser->end_token_buffer, parser->current_token);
            }
        }
    }

    violet_finish_line(parser);
}
