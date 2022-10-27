#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "violet.h"
#include "parser.c"

// Test Helper //

static void print_token_buffer(token_t *ptb)
{
    for (uint32_t i = 0; i < (uint32_t)sb_len(ptb); ++i)
    {
        int token_type_int = (int)ptb[i].type;
        if (ptb[i].is_end_node)
        {
            printf("End ");
        }
        printf("%s\n", token_string_list[token_type_int]);
    }
}

static void print_html_from_token_buffer(token_t *ptb)
{
    for (uint32_t i = 0; i < sb_len(ptb); ++i)
    {
        token_t token = ptb[i];

        switch (token.type)
        {
            case TT_header:
            {
                if (token.is_end_node)
                {
                    printf("</h%d>\n", token.count);
                    break;
                }

                printf("<h%d>\n", token.count);
                printf("%.*s\n", token.len, token.start);
            } break;

            case TT_paragraph:
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

            case TT_continue:
            {
                printf("%.*s", token.len, token.start);
            }

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

    parser_t parser = (parser_t) {
        .token_buffer = NULL,
        .end_token_buffer = NULL,
        .current_token = (token_t){0},
        .should_push = true
    };

    char *file_data = read_entire_file(filename);
    violet_parse_stream(&parser, file_data);

    assert((int)TT_MAX == arr_size(token_string_list)); // TODO: Testing
    print_token_buffer(parser.token_buffer);

    free(file_data);
    sb_free(parser.token_buffer);
    sb_free(parser.end_token_buffer);
    return 0;
}
