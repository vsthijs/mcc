#ifndef MCC_SCAN_H
#define MCC_SCAN_H

#include <stddef.h>

typedef enum {
    TOK_eof,
    TOK_err,

    TOK_ident,

    /* keywords */
    TOK_auto,
    TOK_double,
    TOK_int,
    TOK_struct,
    TOK_break,
    TOK_else,
    TOK_long,
    TOK_switch,
    TOK_case,
    TOK_enum,
    TOK_register,
    TOK_typedef,
    TOK_char,
    TOK_extern,
    TOK_return,
    TOK_union,
    TOK_const,
    TOK_float,
    TOK_short,
    TOK_unsigned,
    TOK_continue,
    TOK_for,
    TOK_signed,
    TOK_void,
    TOK_default,
    TOK_goto,
    TOK_sizeof,
    TOK_volatile,
    TOK_do,
    TOK_if,
    TOK_static,
    TOK_while,

    TOK_COUNT
} TokenType;

extern const char *tokentype_name[];

typedef struct {
    TokenType type;
    char *src;
    size_t row, col;
} Token;

#define free_token(tok)                                                        \
    do {                                                                       \
        if ((tok).src != NULL) free((tok).src);                                \
    } while (0)

typedef struct {
    size_t row, col, idx;
    const char *source;
    size_t len;
} Lexer;

/* length = 0 means strlen */
Lexer lexer_from_source(const char *source, size_t length);

Lexer lexer_from_file(const char *file);
Token lexer_next(Lexer *lexer);

#endif /* MCC_SCAN_H */
