include Makefile.ini

OBJS=test.o $(INI_OBJS)
CFLAGS=-Wall -g $(INI_CFLAGS)

all: test

%.o: %.c
	$(CC) -c $< -Wall -o $@

test: $(OBJS)

.PHONY: clean
clean:
	rm -f test $(OBJS)
