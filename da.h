#ifndef MCC_DA_H
#define MCC_DA_H

/*
da_reserve, da_append and da_free are stolen from github.com/tsoding/nob.h and
modified (only removed NOB_ prefix).
*/

#include <assert.h>
#include <stdlib.h>

#ifndef DA_INIT_CAP
#define DA_INIT_CAP 256
#endif

#define da_reserve(da, expected_capacity)                                      \
    do {                                                                       \
        if ((expected_capacity) > (da)->capacity) {                            \
            if ((da)->capacity == 0) {                                         \
                (da)->capacity = DA_INIT_CAP;                                  \
            }                                                                  \
            while ((expected_capacity) > (da)->capacity) {                     \
                (da)->capacity *= 2;                                           \
            }                                                                  \
            (da)->items =                                                      \
                realloc((da)->items, (da)->capacity * sizeof(*(da)->items));   \
            assert((da)->items != NULL && "Buy more RAM lol");                 \
        }                                                                      \
    } while (0)

#define da_append(da, item)                                                    \
    do {                                                                       \
        da_reserve((da), (da)->count + 1);                                     \
        (da)->items[(da)->count++] = (item);                                   \
    } while (0)

#define da_free(da) free((da).items)

typedef struct {
    const char **items;
    size_t count;
    size_t capacity;
} StringArray;

#endif /* MCC_DA_H */
