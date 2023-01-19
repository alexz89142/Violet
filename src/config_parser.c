typedef enum keyword_t
{
    KEYWORD_something
} keyword_t;

typedef struct line_t
{
    keyword_t keyword;
    uint32_t size;
    uint32_t value_size;
    char *value;
} line_t;

static line_t violet_parse_line(char *data)
{
}

static void violet_parse_config(char *file_data)
{
    while (*file_data)
    {
        line_t line = violet_parse_line(file_data);
        file_data += line.size;
    }
}
