#ifndef MD_LEXER_H
#define MD_LEXER_H

// Type definitions //

typedef enum token_type_t
{
    TT_NULL,
    TT_end,

    // Symbols
    TT_newline,
    TT_space,
    TT_astrix,
    TT_underscore,
    TT_dash,
    TT_close_chevron,
    TT_grave_accent,

    // HTML types
    TT_h1,
    TT_h2,
    TT_h3,
    TT_text,
    TT_MAX
} token_type_t;

typedef struct token_t
{
    token_type_t type;
    const char *start;
    const char *end;
} token_t;

// Function Declarations //

static void ml_init_stream(char *stream);
static token_t ml_get_next_token(void);

#endif // MD_LEXER_H