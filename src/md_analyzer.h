#ifndef MD_ANALYZER_H
#define MD_ANALYZER_H

const char *token_string_list[] = { "NULL", "End", "New Line", "Space", "Hashtag", "Astrix", "Digit", "Text" };
const char *parsed_token_string_list[] = { "NULL", "Header", "Paragraph" };

typedef enum mda_token_type_t
{
    TT_NULL, 
    TT_end,
    TT_newline,
    TT_space,
    TT_hashtag,
    TT_astrix,
    TT_digit,
    TT_text,
    TT_MAX
} mda_token_type_t;

typedef struct mda_token_t
{
    mda_token_type_t type;
    uint32_t len;
    const char *start;
} mda_token_t;

typedef enum mda_parsed_token_type_t
{
    PTT_NULL, 
    PTT_header,
    PTT_paragraph,
    PTT_MAX
} mda_parsed_token_type_t;

typedef struct mda_parsed_token_t
{
    mda_parsed_token_type_t type;
    uint32_t count;
    uint32_t len;
    const char *start;
    bool is_end_node;
} mda_parsed_token_t;

// Helper functions
static bool mda_token_type_is_symbol(mda_token_type_t tt);
static bool mda_token_type_is_endspace(mda_token_type_t tt);

// Lexer / Parser functions
static mda_token_t mda_get_next_token(void);
static mda_token_t *mda_lex_stream(char *stream);
static mda_parsed_token_t *mda_parse_lexed_tokens(mda_token_t *token_buffer);

#endif // MD_ANALYZER_H