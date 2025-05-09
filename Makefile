CC ?= gcc

CFLAGS ?= -Wall -pedantic -ansi
CPPFLAGS ?=

SOURCE_FILES := $(wildcard *.c)

mcc: $(SOURCE_FILES)
	$(CC) -o $@ $^ $(CFLAGS) $(CPPFLAGS)

.PHONY: clean
clean:
	rm -f mcc
