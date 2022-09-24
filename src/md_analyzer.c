#include "md_analyzer.h"

static const char *g_stream;

// Helper //

static bool mda_token_type_is_symbol(mda_token_type_t tt)
{
    // TODO: Update as more symbols are added
    return (tt == TT_astrix);
}

static bool mda_token_type_is_endspace(mda_token_type_t tt)
{
    // TODO: Update as more endspaces are added
    return (tt == TT_end || tt == TT_newline);
}

// Lexer //

static mda_token_t mda_get_next_token(void)
{
    mda_token_t token = {0};
    token.start = g_stream;

    switch (*g_stream)
    {
        case '\0':
        {
            token.type = TT_end;
        } break;

        case '\n':
        {
            token.type = TT_newline;
            g_stream++;
        } break;

        case ' ':
        {
            token.type = TT_space;
            while (is_space(*g_stream))
            {
                g_stream++;
                token.len++;
            }
        } break;

        case '#':
        {
            token.type = TT_hashtag;
            while (*g_stream == '#')
            {
                g_stream++;
                token.len++;
            }
        } break;

        case '0': case '1': 
        case '2': case '3': 
        case '4': case '5': 
        case '6': case '7': 
        case '8': case '9':
        {
            token.type = TT_digit;
            while (is_numeric(*g_stream))
            {
                g_stream++;
                token.len++;
            }
        } break;

        default:
        {
            token.type = TT_text;
            while (!is_endspace(*g_stream))
            {
                g_stream++;
                token.len++;
            }
        } break;
    }

    return token;
}

static mda_token_t *mda_lex_stream(char *stream)
{
    g_stream = stream;
    mda_token_t *token_buffer = NULL;

    mda_token_t token = {0};
    while (token.type != TT_end)
    {
        token = mda_get_next_token();
        sb_push(token_buffer, token);
    }

    return token_buffer;
}

// Parser

static mda_parsed_token_t *mda_parse_lexed_tokens(mda_token_t *token_buffer)
{
    mda_parsed_token_t *parsed_token_buffer = NULL;
    mda_parsed_token_t tracker_token = {0};

    for (uint32_t i = 0; i < sb_len(token_buffer); i++)
    {
        mda_token_t current_token = token_buffer[i];
        mda_parsed_token_t current_parsed_token = {0};

        switch (current_token.type)
        {
            case TT_end:
            {
                current_parsed_token.type = tracker_token.type;
                current_parsed_token.count = tracker_token.count;
                current_parsed_token.is_end_node = true;
                tracker_token = (mda_parsed_token_t){0};
            } break;

            case TT_newline:
            {
                current_parsed_token.type = tracker_token.type;
                current_parsed_token.count = tracker_token.count;
                current_parsed_token.is_end_node = true;
                tracker_token = (mda_parsed_token_t){0};

                while (current_token = token_buffer[++i], 
                       current_token.type == TT_newline);
                --i;
            } break;

            case TT_space:
            {
                // NOTE: This should never get here 
                // if current_type is not PTT_NULL
                assert(tracker_token.type == PTT_NULL);
            } break;

            case TT_hashtag:
            {
                // NOTE: This should never get here 
                // if current_type is not PTT_NULL
                assert(tracker_token.type == PTT_NULL);
                
                if (token_buffer[i+1].type != TT_space)
                {
                    current_parsed_token.type = PTT_paragraph;
                    current_parsed_token.start = current_token.start;
                }
                else
                {
                    current_parsed_token.type = PTT_header;
                    current_parsed_token.count = current_token.len;
                    i += 2; // Set to start of header content
                    current_token = token_buffer[i];
                    current_parsed_token.start = current_token.start;
                }

                while (current_token = token_buffer[i++],
                       !(mda_token_type_is_symbol(current_token.type) || 
                       mda_token_type_is_endspace(current_token.type)))
                {
                    current_parsed_token.len += current_token.len;
                }
                i -= 2; // Set it back, because for loop will increment
                tracker_token = current_parsed_token;
            } break;

            default: 
            {
                // NOTE: This should never get here 
                // if current_type is not PTT_NULL
                assert(tracker_token.type == PTT_NULL);
                
                current_parsed_token.type = PTT_paragraph;
                current_parsed_token.start = current_token.start;

                while (current_token = token_buffer[i++],
                       !(mda_token_type_is_symbol(current_token.type) || 
                       mda_token_type_is_endspace(current_token.type)))
                {
                    current_parsed_token.len += current_token.len;
                }
                i -= 2; // Set it back, because for loop will increment
                tracker_token = current_parsed_token;
            } break;
        }

        sb_push(parsed_token_buffer, current_parsed_token);
    }

    return parsed_token_buffer;
}
