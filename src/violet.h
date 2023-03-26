#ifndef VIOLET_H
#define VIOLET_H

#include "util.h"
#include "parser.h"

// Commands which can be found in config file
typedef enum keyword_t
{
    KEYWORD_invalid,
    KEYWORD_comment,
    KEYWORD_input,
    KEYWORD_output,
    KEYWORD_header,
    KEYWORD_footer,
    KEYWORD_index,
    KEYWORD_overwrite
} keyword_t;

typedef struct line_t
{
    bool invalid;
    keyword_t keyword;
    uint32_t size;
    uint32_t value_size;
    char *value;
} line_t;

#define MAX_FILE_PATH_SIZE 512
typedef struct settings_t
{
    // Settings
    char input_dir_path[MAX_FILE_PATH_SIZE];
    char output_dir_path[MAX_FILE_PATH_SIZE];
    char header_path[MAX_FILE_PATH_SIZE];
    char footer_path[MAX_FILE_PATH_SIZE];
    char index_html_path[MAX_FILE_PATH_SIZE];
    int should_overwrite;

    // Loaded
    DIR *input_dir;
    DIR *output_dir;
    char *header_data;
    char *footer_data;
    char *index_data;
} settings_t;

typedef struct result_t
{
    int success;
    char message[512];
} result_t;

// Util
static void violet_error(const char *err_msg);

// Config Parsing
static keyword_t voilet_get_keyword_type(char *word);
static bool violet_is_line_comment(char *data);
static void violet_handle_invalid_result(result_t *res);
static void violet_validate_config(settings_t *settings);
static void violet_close_settings(settings_t *settings);
static void violet_load_settings(settings_t *settings);
static result_t violet_fill_settings(settings_t *settings, line_t line);
static line_t violet_parse_config_line(char *data);
static void violet_parse_config(settings_t *settings, char *file_data);
static void violet_close_config(settings_t *settings, char *file_data);

#endif // VIOLET_H
