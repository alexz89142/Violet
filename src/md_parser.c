#include "md_parser.h"

/*
# Hello  *World*\n what is up you\n this is crazy\n\0

current_tag -> h1

temp -> Hello, Space, 

while ( we dont hit new line)
{
    switch (type)
    {
        if (symbol)
        {
            find closing symbol
        }
        else
        {
            add to temp
        }
    }
}
*/

static inline bool token_type_is_whitespace(token_type_t t)
{
    return (t == TT_space || t == TT_newline || t == TT_end);
}

static inline bool token_type_is_endspace(token_type_t t)
{
    return (t == TT_newline || t == TT_end);
}

static void mp_parse_token_buffer_to_html(FILE *html_out_file, token_buffer_t *lexed_token_buffer)
{
    token_type_t current_tag = TT_NULL;
    bool unordered_list = false;
    bool ordered_list = false;
    bool underlining = false;

    for (uint32_t i = 0; i < lexed_token_buffer->capacity; ++i)
    {
        token_t *current_token = (token_t *)(lexed_token_buffer->tokens + i);
        switch (current_token->type)
        {
            case TT_end:
            {
            } break;

            case TT_newline:
            {
            } break;

            case TT_space:
            {
                if (current_tag == TT_NULL)
                {
                    continue;
                }
                else
                {
                    // TODO: print space
                }
            } break;

            case TT_astrix:
            {
            } break;

            case TT_underscore:
            {
                if (current_tag == TT_NULL)
                {
                    // TODO: print paragraph
                }

                int current_offset = 0;
                token_t *offset_token = (token_t *)(lexed_token_buffer + i + offset_token + 1);
                while (!token_type_is_whitespace(offset_token->type));
                token_t *final_offset_token = (token_t *)(lexed_token_buffer + i + offset_token);
                if (final_offset_token != TT_underscore)
                {
                    // TODO: print normal
                }
                else
                {
                    // TODO: print underlined
                }
            } break;

            case TT_dash:
            {
                int current_offset = 0;
                token_t *offset_token = (token_t *)(lexed_token_buffer + i + offset_token + 1);
                while (!token_type_is_whitespace(offset_token->type));
                token_t *final_offset_token = (token_t *)(lexed_token_buffer + i + offset_token);
                if (current_offset == 0 && TT_NULL)
                {
                    if (!underlining)
                    {
                        underlining = true;
                        // TODO: print ul
                    }

                    // TODO: print li
                }
                else if (final_offset_token == TT_dash)
                {
                    // TODO: print strike
                }
                else
                {
                    // TODO: print normal
                }
            } break;

            case TT_close_chevron:
            {
                if (current_tag == )
            } break;

            case TT_grave_accent:
            {
                /*
                int current_offset = 0;
                token_t *offset_token = (token_t *)(lexed_token_buffer + i + offset_token + 1);
                while (!token_type_is_whitespace(offset_token->type));
                token_t *final_offset_token = (token_t *)(lexed_token_buffer + i + offset_token);
                */
            } break;

            case TT_h1:
            case TT_h2:
            case TT_h3:
            {
                if (current_tag == TT_NULL)
                {
                    // TODO: print header
                }
                else
                {
                    // TODO: print normal
                }
            } break;

            case TT_text:
            {
            } break;

            default:
            {
            } break;
        }
    }
}
