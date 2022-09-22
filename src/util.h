#ifndef UTIL_H
#define UTIL_H

// Memory

static inline void *xmalloc(size_t size)
{
    void *ptr = malloc(size);
    if (!ptr)
    {
        perror("malloc failed");
        exit(1);
    }

    return ptr;
}

static inline void *xrealloc(void *ptr, size_t size)
{
    ptr = realloc(ptr, size);
    if (!ptr) 
    {
        perror("xrealloc failed");
        exit(1);
    }

    return ptr;
}

// IO Operations

static inline char *read_entire_file(const char *filename)
{
    char *file_contence;
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("File was not able to be opened");
        exit(1);
    }

    // Get size of the file
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    rewind(fp);

    // Allocate and file contence
    file_contence = xmalloc(file_size + 1);
    fread(file_contence, 1, file_size, fp);
    file_contence[file_size] = '\0';
        
    fclose(fp);
    return file_contence;
}

// Sean Barret Style Strechy Buffers

typedef struct strechy_buffer_t
{
    size_t len;
    size_t cap;
    uint8_t data[0];
} strechy_buffer_t;

#define sb_check_needs_grow(buff, n) (sb_len(buff) + (n) <= sb_cap(buff))
#define sb_maybe_grow(buff, n)       (sb_check_needs_grow(buff, n) ? 0 : ((buff) = __sb_grow((buff), sb_len(buff) + (n), sizeof(*(buff)))))

#define sb_push(buff, val)           (sb_maybe_grow(buff, 1), (buff)[sb_len(buff)] = (val), sb_meta_data(buff)->len++)
#define sb_free(buff)                ((buff) ? free(sb_meta_data(buff)) : 0)

#define sb_meta_data(buff)           ((strechy_buffer_t *)((uint8_t *)buff - offsetof(strechy_buffer_t, data)))
#define sb_len(buff)                 ((buff) ? sb_meta_data(buff)->len : 0)
#define sb_cap(buff)                 ((buff) ? sb_meta_data(buff)->cap : 0)

static inline void *__sb_grow(void *buff, size_t increment, size_t item_size)
{
    uint32_t double_cap = buff ? (2 * sb_cap(buff)) : 0;
    uint32_t min_needed = sb_cap(buff) + increment;
    uint32_t new_cap = (double_cap > min_needed) ? double_cap : min_needed;

    strechy_buffer_t *new_buff;
    if (buff)
    {
        new_buff = (strechy_buffer_t *)xrealloc(
            sb_meta_data(buff), item_size * new_cap + offsetof(strechy_buffer_t, data));
    }
    else
    {
        new_buff = (strechy_buffer_t *)xmalloc(
            item_size * new_cap + offsetof(strechy_buffer_t, data));
        new_buff->len = 0;
    }

    if (!new_buff)
    {
        perror("__sb_grow allocation failed");
        exit(1);
    }

    new_buff->cap = new_cap;
    return new_buff->data;
}

// Identify types

static inline bool is_space(char c)
{
    bool retval = false;
    switch (c)
    {
        case ' ':
        case '\t':
        case '\v':
        case '\r':
        case '\f':
        {
            retval = true;
        } break;

        default: break;
    }

    return retval;
}

static inline bool is_whitespace(char c)
{
    bool retval = false;
    switch (c)
    {
        case ' ':
        case '\t':
        case '\v':
        case '\r':
        case '\f':
        case '\n':
        {
            retval = true;
        } break;

        default: break;
    }

    return retval;
}

static inline bool is_endspace(char c)
{
    bool retval = false;
    switch (c)
    {
        case ' ':
        case '\t':
        case '\v':
        case '\r':
        case '\f':
        case '\n':
        case '\0':
        {
            retval = true;
        } break;

        default: break;
    }

    return retval;
}

static inline bool is_numeric(char c)
{
    return (c >= '0' && c <= '9');
}

static inline bool is_alpha(char c)
{
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

static inline bool is_alpha_numeric(char c)
{
    return (is_alpha(c) || is_numeric(c));
}

#endif