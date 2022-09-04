#ifndef MD_LEXER_H
#define MD_LEXER_H

const char *token_string_list[6] = { "NULL", "End", "Space", "Hashtag", "Digit", "Text" };

typedef enum token_type_t
{
    TT_NULL, 
    TT_end,
    TT_space,
    TT_hashtag,
    TT_digit,
    TT_text
} token_type_t;

typedef struct token_t
{
    token_type_t type;
    const char *start;
    const char *end;
} token_t;

static token_t ml_get_next_token(void);
static token_t *ml_lex_file(const char *filename);

#endif // MD_LEXER_H