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

    char *file_data = read_entire_file(filename);
    token_t *ptb = violet_parse_stream(file_data);

    // Making sure output is correct
    assert((int)TT_MAX == arr_size(token_string_list));
    print_token_buffer(ptb);
    puts("-------------");
    print_html_from_token_buffer(ptb);

    free(file_data);
    return 0;
}
