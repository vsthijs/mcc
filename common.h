#ifndef MCC_COMMON_H
#define MCC_COMMON_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define todo(msg)                                                              \
    do {                                                                       \
        fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, msg);         \
        abort();                                                               \
    } while (0)

#endif /* MCC_COMMON_H */
