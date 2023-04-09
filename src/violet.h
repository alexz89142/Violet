#ifndef VIOLET_H
#define VIOLET_H

#include "util.h"
#include "parser.h"

// TODO: Think about adding custom path length fields

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

typedef struct line_t {
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

// Util
static int violet_build_filename(char *dest, const char *base, const char *secondary,
                                 char seperator, int dest_size);

static int violet_build_and_check_html_file(char *html_full_path,
                                            int html_full_path_size,
                                            const char *odp,
                                            const char *md_filename);

// HTML Translation
static void violet_translate_token_buffer_to_html(token_t *ptb,
                                                  const char *header_data,
                                                  const char *footer_data,
                                                  const char *out_filename);

// Config Parsing
static keyword_t voilet_get_keyword_type(char *word);
static bool violet_is_line_comment(char *data);
static void violet_validate_config(settings_t *settings);
static void violet_close_settings(settings_t *settings);
static void violet_load_settings(settings_t *settings);
static void violet_fill_settings(settings_t *settings, line_t line);
static line_t violet_parse_config_line(char *data);
static void violet_parse_config(settings_t *settings, char *file_data);
static void violet_close_config(settings_t *settings, char *file_data);

#endif // VIOLET_H
