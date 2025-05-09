#include "scan.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *tokentype_name[TOK_COUNT] = {
    /*[TOK_eof] =*/"eof",
    /*[TOK_err] =*/"err",
    /*[TOK_ident] =*/"ident",
    /*[TOK_auto] =*/"auto",
    /*[TOK_double] =*/"double",
    /*[TOK_int] =*/"int",
    /*[TOK_struct] =*/"struct",
    /*[TOK_break] =*/"break",
    /*[TOK_else] =*/"else",
    /*[TOK_long] =*/"long",
    /*[TOK_switch] =*/"switch",
    /*[TOK_case] =*/"case",
    /*[TOK_enum] =*/"enum",
    /*[TOK_register] =*/"register",
    /*[TOK_typedef] =*/"typedef",
    /*[TOK_char] =*/"char",
    /*[TOK_extern] =*/"extern",
    /*[TOK_return] =*/"return",
    /*[TOK_union] =*/"union",
    /*[TOK_const] =*/"const",
    /*[TOK_float] =*/"float",
    /*[TOK_short] =*/"short",
    /*[TOK_unsigned] =*/"unsigned",
    /*[TOK_continue] =*/"continue",
    /*[TOK_for] =*/"for",
    /*[TOK_signed] =*/"signed",
    /*[TOK_void] =*/"void",
    /*[TOK_default] =*/"default",
    /*[TOK_goto] =*/"goto",
    /*[TOK_sizeof] =*/"sizeof",
    /*[TOK_volatile] =*/"volatile",
    /*[TOK_do] =*/"do",
    /*[TOK_if] =*/"if",
    /*[TOK_static] =*/"static",
    /*[TOK_while] =*/"while",
};

Lexer lexer_from_source(const char *source, size_t length) {
    Lexer l = {0};
    l.source = source;
    l.len = length;
    if (l.len == 0) l.len = strlen(l.source);
    return l;
}

Lexer lexer_from_file(const char *file) {
    size_t filesz;
    char *source;
    FILE *fd = fopen(file, "r");
    if (fd == NULL) {
        fprintf(stderr, "err: could not open file '%s'.\n", file);
        return lexer_from_source(NULL, 0);
    }

    fseek(fd, 0, SEEK_END);
    filesz = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    source = malloc(filesz + 1);
    if (source == NULL) {
        fprintf(stderr, "err: could not allocate memory to read file '%s'.\n",
                file);
        fclose(fd);
        return lexer_from_source(NULL, 0);
    }

    if (fread(source, 1, filesz, fd) < filesz) {
        fprintf(stderr, "err: could not read file '%s'.\n", file);
        free(source);
        fclose(fd);
        return lexer_from_source(NULL, 0);
    }
    source[filesz] = 0;

    fclose(fd);
    return lexer_from_source(source, filesz);
}

static char consume(Lexer *lexer) {
    char c = lexer->source[lexer->idx++];
    if (c == '\n') {
        lexer->row++;
        lexer->col = 0;
    } else {
        lexer->col++;
    }
    return c;
}

static bool is_at_end(Lexer *lexer) { return lexer->idx >= lexer->len; }

static void skip_ws(Lexer *lexer) {
    while (lexer->idx < lexer->len && isspace(lexer->source[lexer->idx]))
        consume(lexer);
}

static char current(Lexer *lexer) { return lexer->source[lexer->idx]; }

static TokenType ident_type(const char *value, size_t len) {
    /* TODO: optimize with a switch tree (?). */
    if (strncmp("auto", value, len) == 0)
        return TOK_auto;
    else if (strncmp("double", value, len) == 0)
        return TOK_double;
    else if (strncmp("int", value, len) == 0)
        return TOK_int;
    else if (strncmp("struct", value, len) == 0)
        return TOK_struct;
    else if (strncmp("break", value, len) == 0)
        return TOK_break;
    else if (strncmp("else", value, len) == 0)
        return TOK_else;
    else if (strncmp("long", value, len) == 0)
        return TOK_long;
    else if (strncmp("switch", value, len) == 0)
        return TOK_switch;
    else if (strncmp("case", value, len) == 0)
        return TOK_case;
    else if (strncmp("enum", value, len) == 0)
        return TOK_enum;
    else if (strncmp("register", value, len) == 0)
        return TOK_register;
    else if (strncmp("typedef", value, len) == 0)
        return TOK_typedef;
    else if (strncmp("char", value, len) == 0)
        return TOK_char;
    else if (strncmp("extern", value, len) == 0)
        return TOK_extern;
    else if (strncmp("return", value, len) == 0)
        return TOK_return;
    else if (strncmp("union", value, len) == 0)
        return TOK_union;
    else if (strncmp("const", value, len) == 0)
        return TOK_const;
    else if (strncmp("float", value, len) == 0)
        return TOK_float;
    else if (strncmp("short", value, len) == 0)
        return TOK_short;
    else if (strncmp("unsigned", value, len) == 0)
        return TOK_unsigned;
    else if (strncmp("continue", value, len) == 0)
        return TOK_continue;
    else if (strncmp("for", value, len) == 0)
        return TOK_for;
    else if (strncmp("signed", value, len) == 0)
        return TOK_signed;
    else if (strncmp("void", value, len) == 0)
        return TOK_void;
    else if (strncmp("default", value, len) == 0)
        return TOK_default;
    else if (strncmp("goto", value, len) == 0)
        return TOK_goto;
    else if (strncmp("sizeof", value, len) == 0)
        return TOK_sizeof;
    else if (strncmp("volatile", value, len) == 0)
        return TOK_volatile;
    else if (strncmp("do", value, len) == 0)
        return TOK_do;
    else if (strncmp("if", value, len) == 0)
        return TOK_if;
    else if (strncmp("static", value, len) == 0)
        return TOK_static;
    else if (strncmp("while", value, len) == 0)
        return TOK_while;
    else
        return TOK_ident;
}

#define isident(ch) ((ch) == '_' || isalnum(ch))

Token lexer_next(Lexer *lexer) {
    Token t = {0};
    char ch;
    skip_ws(lexer);
    t.row = lexer->row;
    t.col = lexer->col;
    t.src = &lexer->source[lexer->idx];

    if (is_at_end(lexer)) {
        t.type = TOK_eof;
        return t;
    }

    ch = current(lexer);

    if (isalpha(ch)) {
        while (!is_at_end(lexer) && isident(current(lexer))) {
            t.len++;
            consume(lexer);
        }
        t.type = ident_type(t.src, t.len);
        return t;
    }

    t.type = TOK_err;
    consume(lexer);
    t.len = 1;
    return t;
}
