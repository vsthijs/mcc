#include "scan.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* tokentype_name[TOK_COUNT] = {
    /*[TOK_eof] =*/ "eof",
    /*[TOK_err] =*/ "err",
    /*[TOK_ident] =*/ "ident",
    /*[TOK_auto] =*/ "auto",
    /*[TOK_double] =*/ "double",
    /*[TOK_int] =*/ "int",
    /*[TOK_struct] =*/ "struct",
    /*[TOK_break] =*/ "break",
    /*[TOK_else] =*/ "else",
    /*[TOK_long] =*/ "long",
    /*[TOK_switch] =*/ "switch",
    /*[TOK_case] =*/ "case",
    /*[TOK_enum] =*/ "enum",
    /*[TOK_register] =*/ "register",
    /*[TOK_typedef] =*/ "typedef",
    /*[TOK_char] =*/ "char",
    /*[TOK_extern] =*/ "extern",
    /*[TOK_return] =*/ "return",
    /*[TOK_union] =*/ "union",
    /*[TOK_const] =*/ "const",
    /*[TOK_float] =*/ "float",
    /*[TOK_short] =*/ "short",
    /*[TOK_unsigned] =*/ "unsigned",
    /*[TOK_continue] =*/ "continue",
    /*[TOK_for] =*/ "for",
    /*[TOK_signed] =*/ "signed",
    /*[TOK_void] =*/ "void",
    /*[TOK_default] =*/ "default",
    /*[TOK_goto] =*/ "goto",
    /*[TOK_sizeof] =*/ "sizeof",
    /*[TOK_volatile] =*/ "volatile",
    /*[TOK_do] =*/ "do",
    /*[TOK_if] =*/ "if",
    /*[TOK_static] =*/ "static",
    /*[TOK_while] =*/ "while",
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

static TokenType ident_type(const char *value) {
    /* TODO: optimize with a switch tree (?). */
    if (strcmp("auto", value) == 0)
        return TOK_auto;
    else if (strcmp("double", value) == 0)
        return TOK_double;
    else if (strcmp("int", value) == 0)
        return TOK_int;
    else if (strcmp("struct", value) == 0)
        return TOK_struct;
    else if (strcmp("break", value) == 0)
        return TOK_break;
    else if (strcmp("else", value) == 0)
        return TOK_else;
    else if (strcmp("long", value) == 0)
        return TOK_long;
    else if (strcmp("switch", value) == 0)
        return TOK_switch;
    else if (strcmp("case", value) == 0)
        return TOK_case;
    else if (strcmp("enum", value) == 0)
        return TOK_enum;
    else if (strcmp("register", value) == 0)
        return TOK_register;
    else if (strcmp("typedef", value) == 0)
        return TOK_typedef;
    else if (strcmp("char", value) == 0)
        return TOK_char;
    else if (strcmp("extern", value) == 0)
        return TOK_extern;
    else if (strcmp("return", value) == 0)
        return TOK_return;
    else if (strcmp("union", value) == 0)
        return TOK_union;
    else if (strcmp("const", value) == 0)
        return TOK_const;
    else if (strcmp("float", value) == 0)
        return TOK_float;
    else if (strcmp("short", value) == 0)
        return TOK_short;
    else if (strcmp("unsigned", value) == 0)
        return TOK_unsigned;
    else if (strcmp("continue", value) == 0)
        return TOK_continue;
    else if (strcmp("for", value) == 0)
        return TOK_for;
    else if (strcmp("signed", value) == 0)
        return TOK_signed;
    else if (strcmp("void", value) == 0)
        return TOK_void;
    else if (strcmp("default", value) == 0)
        return TOK_default;
    else if (strcmp("goto", value) == 0)
        return TOK_goto;
    else if (strcmp("sizeof", value) == 0)
        return TOK_sizeof;
    else if (strcmp("volatile", value) == 0)
        return TOK_volatile;
    else if (strcmp("do", value) == 0)
        return TOK_do;
    else if (strcmp("if", value) == 0)
        return TOK_if;
    else if (strcmp("static", value) == 0)
        return TOK_static;
    else if (strcmp("while", value) == 0)
        return TOK_while;
    else
        return TOK_ident;
}

Token lexer_next(Lexer *lexer) {
    Token t = {0};
    const char *begin = lexer->source + lexer->idx;
    size_t len = 0;
    skip_ws(lexer);
    t.row = lexer->row;
    t.col = lexer->col;
    t.src = NULL;

    if (is_at_end(lexer)) {
        t.type = TOK_eof;
        return t;
    }

    if (isalpha(current(lexer))) {
        while (!is_at_end(lexer) &&
               (current(lexer) == '_' || isalnum(current(lexer)))) {
            if (t.type == 0 && !islower(consume(lexer))) t.type = TOK_ident;
            len++;
        }
        t.src = malloc(len + 1);
        strncpy(t.src, begin, len); /* strncpy also does sentinel */
        if (t.type == 0) t.type = ident_type(t.src);
        return t;
    }

    t.type = TOK_err;
    t.src = malloc(2);
    t.src[0] = consume(lexer);
    t.src[1] = 0;
    return t;
}
