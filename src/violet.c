#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "violet.h"
#include "md_analyzer.c"

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

    free(file_data);
    return 0;
}
