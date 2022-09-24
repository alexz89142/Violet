#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "violet.h"
#include "md_analyzer.c"

// Test Helper //

static void print_token_buffer(mda_token_t *token_buffer)
{
    for (uint32_t i = 0; i < (uint32_t)sb_len(token_buffer); ++i)
    {
        int token_type_int = (int)token_buffer[i].type;
        printf("%s\n", token_string_list[token_type_int]);
    }
    puts("------------");
}

static void print_parsed_token_buffer(mda_parsed_token_t *parsed_token_buffer)
{
    for (uint32_t i = 0; i < sb_len(parsed_token_buffer); ++i)
    {
        int token_type_int = (int)parsed_token_buffer[i].type;
        printf("%s\n", parsed_token_string_list[token_type_int]);
    }
    puts("------------");
}

static void print_html_from_parsed_token_buffer(mda_parsed_token_t *parsed_token_buffer)
{
    for (uint32_t i = 0; i < sb_len(parsed_token_buffer); ++i)
    {
        mda_parsed_token_t token = parsed_token_buffer[i];

        switch (token.type)
        {
            case PTT_header:
            {
                if (token.is_end_node)
                {
                    printf("</h%d>\n", token.count);
                    break;
                }

                printf("<h%d>\n", token.count);
                printf("%.*s\n", token.len, token.start);
            } break;

            case PTT_paragraph:
            {
                if (token.is_end_node)
                {
                    printf("</p>\n");
                    break;
                }
                
                printf("<p>\n");
                printf("%.*s", token.len, token.start);
                printf("\n");
            } break;

            default: break;
        }
    }
}

// Core //

int main(int argc, char **argv)
{
    char filename[512];
    if (argc > 1)
    {
        strcpy(filename, argv[1]);
    }
    else
    {
        // TODO: Only for testing
        strcpy(filename, "test/test.md");
    }

    char *file_data = read_entire_file(filename);
    mda_token_t *tb = mda_lex_stream(file_data);
    mda_parsed_token_t *ptb = mda_parse_lexed_tokens(tb);

    // Making sure output is correct
    assert((int)TT_MAX == arr_size(token_string_list));
    assert((int)PTT_MAX == arr_size(parsed_token_string_list));
    print_token_buffer(tb);
    print_parsed_token_buffer(ptb);
    print_html_from_parsed_token_buffer(ptb);

    free(file_data);
    return 0;
}
