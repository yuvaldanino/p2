# Target library
lib := libuthread.a
CC		:= gcc
# CFLAGS	:= -Wall -Wextra -Werror
CFLAGS	+= -g

all: $(lib)

$(lib): queue.o uthread.o
	ar rcs $@ $^

# TODO: use -MMD and $(deps) to automatically track .h dependencies
queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c -o $@ $<

uthread.o: uthread.c uthread.h private.h # TODO: fix this line
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(lib) queue.o uthread.o