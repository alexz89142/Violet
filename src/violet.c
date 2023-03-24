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

// Util //

static void violet_error(const char *err_msg)
{
    fprintf(stderr, "[VIOLET_ERROR] Could not generate site: %s\n", err_msg);
    exit(1);
}

// Config Parsing // 

static keyword_t voilet_get_keyword_type(char *word)
{
    if (strcmp("input_dir", word) == 0)
    {
        return KEYWORD_input;
    }
    else if (strcmp("output_dir", word) == 0)
    {
        return KEYWORD_output;
    }
    else if (strcmp("header_html", word) == 0)
    {
        return KEYWORD_header;
    }
    else if (strcmp("footer_html", word) == 0)
    {
        return KEYWORD_footer;
    }
    else if (strcmp("index_html", word) == 0)
    {
        return KEYWORD_index;
    }
    else if (strcmp("should_overwrite", word) == 0)
    {
        return KEYWORD_overwrite;
    }

    return KEYWORD_invalid;
}

static bool violet_is_line_comment(char *data)
{
    return (*data == ':' && *(++data) == ':');
}

static void violet_handle_invalid_result(result_t *res)
{
    if (!res->success)
    {
        violet_error(res->message);
    }
}

static void violet_validate_config(settings_t *settings)
{
    if (settings->input_directory[0] == 0 || settings->output_directory[0] == 0)
    {
        violet_error("Input or output directory are required, missing from config");
    }
}

static result_t violet_fill_settings(settings_t *settings, line_t line)
{
    result_t res = {0};
    
    if (line.invalid)
    {
        const char *err_msg = "Error in config file, incorrect keyword or formatting";
        strncpy((char *)&res.message, err_msg, 512);
        return res;
    }
    else if (line.value_size > MAX_FILE_PATH_SIZE)
    {
        const char *err_msg = "Path size too big, must be under 512 charaters!";
        strncpy((char *)&res.message, err_msg, 512);
        return res;
    }

    size_t src_size = sizeof(char) * line.value_size;
    switch (line.keyword)
    {
        case KEYWORD_input:
        {
            memcpy(&settings->input_directory, line.value, src_size);
        } break;

        case KEYWORD_output:
        {
            memcpy(&settings->output_directory, line.value, src_size);
        } break;

        case KEYWORD_header:
        {
            memcpy(&settings->header, line.value, src_size);
        } break;

        case KEYWORD_footer:
        {
            memcpy(&settings->footer, line.value, src_size);
        } break;

        case KEYWORD_index:
        {
            memcpy(&settings->index_html, line.value, src_size);
        } break;

        case KEYWORD_overwrite:
        {
            settings->should_overwrite = line.value[0] == '1' ? 1 : 0;
        } break;

        default: break;
    }

    res.success = 1;
    strncpy((char *)&res.message, "Value udpated in settings!", 512);
    return res;
}


static line_t violet_parse_config_line(char *data)
{
    line_t current_line = {0};

    if (*data == '\n')
    {
        current_line.keyword = KEYWORD_comment; // Not a comment, but treated same way
        current_line.size++;
        return current_line;
    }

    if (violet_is_line_comment(data))
    {
        char c;
        while (c = *data++, c != '\n' && c != '\0')
        {
            current_line.size++;
        }

        current_line.keyword = KEYWORD_comment;
        return current_line;
    }

    char keyword[1024] = {0};
    int count = 0;

    char c;
    while (c = *data++, c != ' ' && count < 1023)
    {
        keyword[count++] = c;
    }
    keyword[count] = '\0';

    keyword_t current_keyword = voilet_get_keyword_type(keyword);
    if (current_keyword == KEYWORD_invalid || *data != '=')
    {
        current_line.invalid = 1;
        return current_line;
    }

    current_line.keyword = current_keyword;
    current_line.value = (data += 2);
    int space_and_equal_size = 3; // " = "
    current_line.size = (count + space_and_equal_size);
    while (c = *data++, c != '\n' && c != '\0')
    {
        current_line.size++;
        current_line.value_size++;
    }

    return current_line;
}

static void violet_parse_config(settings_t *settings, char *file_data)
{
    while (*file_data)
    {
        line_t line = violet_parse_config_line(file_data);
        result_t res = violet_fill_settings(settings, line);
        violet_handle_invalid_result(&res);
        file_data += line.size;
    }

    violet_validate_config(settings);
}


// Core //

int main(int argc, char **argv)
{
    char config[512];
    if (argc > 1)
    {
        strcpy(config, argv[1]);
    }
    else
    {
        char *err_msg = "Config file path required `./violet <config_file_path>`";
        violet_error(err_msg);
    }

    char *file_data = read_entire_file(config);

    settings_t settings = {0};
    violet_parse_config(&settings, file_data);

    printf("Input: %s, Output: %s\n",
           settings.input_directory, settings.output_directory);

    parser_t parser = (parser_t) {
        .token_buffer = NULL,
        .end_token_buffer = NULL,
        .current_token = (token_t){0},
        .should_push = true
    };

    #if 0 
    violet_parse_stream(&parser, file_data);

    assert((int)TT_MAX == arr_size(token_string_list)); // TODO: Testing
    print_token_buffer(parser.token_buffer);
    print_html_from_token_buffer(parser.token_buffer);
    #endif

    free(file_data);
    sb_free(parser.token_buffer);
    sb_free(parser.end_token_buffer);
    return 0;
}
