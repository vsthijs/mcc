#include "common.h"
#include "da.h"
#include "scan.h"
#include <stdio.h>

int main(int argc, char **argv) {
    int arg = 1; /* skip argv[0] */
    int ii;
    StringArray files = {0};
    char *curr;
    Lexer lexer;
    Token t;

    while (arg < argc) {
        curr = argv[arg++];
        if (curr[0] == '-') {
            todo("parse CLI flags");
        } else {
            da_append(&files, curr);
        }
    }
    for (ii = 0; ii < files.count; ii++) {
        printf("file %s\n", files.items[ii]);
        lexer = lexer_from_file(files.items[ii]);
        while (t.type != TOK_eof) {
            t = lexer_next(&lexer);
            printf("%s:%lu:%lu: `%s` (%s)\n", files.items[ii], t.row, t.col, t.src, tokentype_name[t.type]);
            free_token(t);
        }
    }
    da_free(files);
    return 0;
}
