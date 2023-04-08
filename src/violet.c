#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>
#define _GNU_SOURCE

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

#define violet_error(err_msg, ...)                               \
    fprintf(stderr, "[VIOLET_ERROR] Could not generate site: "); \
    fprintf(stderr, err_msg, ##__VA_ARGS__);                     \
    fprintf(stderr, "\n");                                       \
    exit(1);

static int violet_build_filename(char *dest,
                                 const char *base,
                                 const char *secondary,
                                 char seperator,
                                 int dest_size)
{
    int base_size = strlen(base);
    int second_size = strlen(secondary);
    int total_size = base_size + second_size + 1;
    if (total_size >= dest_size)
    {
        return 0;
    }

    strncpy(dest, base, base_size);
    dest[base_size] = seperator;
    dest[base_size + 1] = '\0';
    strcat(dest, secondary);
    return 1;
}

static int violet_build_and_check_html_file(char *html_full_path,
                                            int html_full_path_size,
                                            const char *odp,
                                            const char *md_filename)
{
    // Build extract filename 
    char c;
    int pos = 0;
    char html_filename[1024];
    while (c = md_filename[pos], c != '.')
    {
        html_filename[pos++] = c;
    }

    if (pos + 5 >= 1024)
    {
        // TODO: More useful error message
        violet_error("violet_build_and_check_html_file failed");
    }

    // Append html to filename
    const char *ext = ".html";
    for (int i = 0; i < 5; ++i)
    {
        html_filename[pos + i] = ext[i];
    }
    html_filename[pos + 5] = '\0';

    // Generate full path
    violet_build_filename(
        html_full_path, odp, html_filename, '/', html_full_path_size);

    // Check if exists
    return check_file_exists(html_full_path);
}

// HTML Translation //

static void violet_translate_token_buffer_to_html(token_t *ptb,
                                                  const char *header_data,
                                                  const char *footer_data,
                                                  const char *out_filename)
{
    FILE *out_file = fopen(out_filename, "w");
    if (out_file == NULL)
    {
        // TODO: make violet_error be able to take varible lenght args
        // Need to pass the name of the file to the user.
        violet_error("Error open html file to write");
    }

    if (header_data != NULL)
    {
        fprintf(out_file, "%s\n", header_data);
    }

    for (uint32_t i = 0; i < sb_len(ptb); ++i)
    {
        token_t token = ptb[i];

        // Get open or close tags
        const char **tags = token_tags; 
        if (token.is_end_node)
        {
            tags = token_end_tags;
        }

        // Write tags
        switch (token.type)
        {
            case TT_NULL:
            case TT_MAX:
                // TODO: Should never be hit more detailed output
                violet_error("Invalid token type found in token buffer");
                break;

            case TT_header:
                fprintf(out_file, tags[token.type], token.count);
                break;

            case TT_paragraph:
            case TT_bold:
            case TT_italics:
            case TT_blockquote:
            case TT_unordered_list:
            case TT_ordered_list:
            case TT_code:
            case TT_break:
            case TT_continue:
                fprintf(out_file, tags[token.type], 0);
                break;
                
            case TT_link:
            case TT_image:
                // TODO: IMPLEMENT
                violet_error("Unimplemented");
                break;

            default: break;
        }

        // Write out contence
        fprintf(out_file, "%.*s\n", token.len, token.start);
    }

    if (footer_data != NULL)
    {
        fprintf(out_file, "%s\n", footer_data);
    }

    fclose(out_file);
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

static void violet_validate_config(settings_t *settings)
{
    if (settings->input_dir_path[0] == 0 || settings->output_dir_path[0] == 0)
    {
        violet_error("Input or output directory are required, missing from config");
    }
}

static void violet_close_settings(settings_t *settings)
{
    // Close directories
    closedir(settings->input_dir);
    closedir(settings->output_dir);

    // Free file data if exists
    if (settings->header_data != NULL)
    {
        free(settings->header_data);
    }

    if (settings->footer_data != NULL)
    {
        free(settings->footer_data);
    }

    if (settings->index_data != NULL)
    {
        free(settings->index_data);
    }
}

static void violet_load_settings(settings_t *settings)
{
    // Load directories
    
    settings->input_dir = opendir(settings->input_dir_path);
    if (settings->input_dir == NULL)
    {
        violet_error("Could not open input directory");
    }

    settings->output_dir = opendir(settings->output_dir_path);
    if (settings->output_dir == NULL)
    {
        violet_error("Could not open input directory");
    }

    // Read file data

    if (settings->header_path[0] != 0)
    {
        settings->header_data = read_entire_file(settings->header_path);

        if (settings->header_data == NULL)
        {
            violet_error("Could not open header file");
        }
    }

    if (settings->footer_path[0] != 0)
    {
        settings->footer_data = read_entire_file(settings->footer_path);

        if (settings->footer_data == NULL)
        {
            violet_error("Could not open footer file");
        }
    }

    if (settings->index_html_path[0] != 0)
    {
        settings->index_data = read_entire_file(settings->index_html_path);

        if (settings->index_data == NULL)
        {
            violet_error("Could not open index file");
        }
    }
}
        
static void violet_fill_settings(settings_t *settings, line_t line)
{
    if (line.invalid)
    {
        violet_error("Error in config file, incorrect keyword or formatting.");
    }
    else if (line.value_size > MAX_FILE_PATH_SIZE)
    {
        violet_error("Path size too big, must be under 512 charaters.");
    }

    size_t src_size = sizeof(char) * line.value_size;
    switch (line.keyword)
    {
        case KEYWORD_input:
        {
            memcpy(&settings->input_dir_path, line.value, src_size);
        } break;

        case KEYWORD_output:
        {
            memcpy(&settings->output_dir_path, line.value, src_size);
        } break;

        case KEYWORD_header:
        {
            memcpy(&settings->header_path, line.value, src_size);
        } break;

        case KEYWORD_footer:
        {
            memcpy(&settings->footer_path, line.value, src_size);
        } break;

        case KEYWORD_index:
        {
            memcpy(&settings->index_html_path, line.value, src_size);
        } break;

        case KEYWORD_overwrite:
        {
            settings->should_overwrite = line.value[0] == '1' ? 1 : 0;
        } break;

        default: break;
    }
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
        violet_fill_settings(settings, line);
        file_data += line.size;
    }

    violet_validate_config(settings);
    violet_load_settings(settings);
}

static void violet_close_config(settings_t *settings, char *file_data)
{
    free(file_data);
    violet_close_settings(settings);
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
        violet_error("Config file path required `./violet <config_file_path>`");
    }

    char *config_data = read_entire_file(config);
    if (config_data == NULL)
    {
        violet_error("Config file could not be open, verify file exists");
    }

    settings_t settings = {0};
    violet_parse_config(&settings, config_data);

    // printf("Input: %s, Output: %s\n", settings.input_dir_path, settings.output_dir_path);

    struct dirent *current_dir;
    while ((current_dir = readdir(settings.input_dir)) != NULL)
    {
        if (current_dir->d_type == DT_REG)
        {
            parser_t parser = (parser_t) {
                .token_buffer = NULL,
                .end_token_buffer = NULL,
                .current_token = (token_t){0},
                .should_push = true
            };

            // Build current file path
            const char *idp = settings.input_dir_path;
            const char *odp = settings.output_dir_path;
            const char *current_name = current_dir->d_name;

            // Check if html file aready exsits
            char html_full_path[1024];
            int html_file_exists = violet_build_and_check_html_file(
                html_full_path, 1024, odp, current_name);

            if (!settings.should_overwrite && html_file_exists)
            {
                continue;
            }

            char current_filename[1024];
            violet_build_filename(current_filename, idp, current_name, '/', 1024);

            // Read, Parse, and translate md file to html
            char *file_data = read_entire_file(current_filename);
            if (file_data == NULL)
            {
                printf("Skipping [%s] file, could not be opened", current_filename);
                continue;
            }

            // Parse and translate
            violet_parse_stream(&parser, file_data);
            violet_translate_token_buffer_to_html(parser.token_buffer,
                                                  settings.header_data,
                                                  settings.footer_data,
                                                  html_full_path);

            // TODO: TESTING
            {
                assert((int)TT_MAX == arr_size(token_string_list));
                print_token_buffer(parser.token_buffer);
                print_html_from_token_buffer(parser.token_buffer);
            }
            // TODO: TESTING

            // Clean up for current file
            free(file_data); 
            sb_free(parser.token_buffer);
            sb_free(parser.end_token_buffer);
        }
    }

    violet_close_config(&settings, config_data);
    return 0;
}
