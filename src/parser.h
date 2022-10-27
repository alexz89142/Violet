#ifndef PARSER_H
#define PARSER_H

const char *token_string_list[] = { "NULL", "Header", "Paragraph", "Bold", "Italics", "Continue"};

typedef enum token_type_t
{
    TT_NULL, 
    TT_header,
    TT_paragraph,
    TT_bold,
    TT_italics,
    TT_continue,
    TT_MAX
} token_type_t;

typedef struct token_t
{
    token_type_t type;
    uint32_t count;
    uint32_t len;
    const char *start;
    bool is_end_node;
} token_t;

typedef struct parser_t
{
    token_t *token_buffer;
    token_t *end_token_buffer;
    token_t current_token;
    bool should_push;
} parser_t;

static bool violet_is_char_symbol(char c);
static bool violet_is_char_endspace(char c);
static void violet_parse_stream(parser_t *parser, char *stream);

#endif // PARSER_H