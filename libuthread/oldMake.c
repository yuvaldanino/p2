# Target library and executable
lib := libuthread.a
executable := uthread_hello.x

# Compiler and flags
CC := gcc
CFLAGS := -g # Add your desired flags here

# Application directory
APP_DIR := ../apps

# Include directory for uthread.h
INCLUDE_DIR := . # Adjust if uthread.h is in a different directory

# Default target
all: $(lib) $(executable)

# Library compilation
$(lib): queue.o uthread.o context.o preempt.o
	ar rcs $@ $^

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c $<

uthread.o: uthread.c uthread.h private.h
	$(CC) $(CFLAGS) -c $<

context.o: context.c private.h
	$(CC) $(CFLAGS) -c $<

preempt.o: preempt.c # Add any headers this depends on
	$(CC) $(CFLAGS) -c $<

# Executable compilation
$(executable): $(APP_DIR)/uthread_hello.o
	$(CC) $(CFLAGS) $< -L. -luthread -o $@

$(APP_DIR)/uthread_hello.o: $(APP_DIR)/uthread_hello.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clean
clean:
	rm -f $(lib) *.o $(APP_DIR)/*.o $(executable)

.PHONY: clean
