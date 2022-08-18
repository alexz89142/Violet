#ifndef UTIL_H
#define UTIL_H

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

#endif // UTIL_H