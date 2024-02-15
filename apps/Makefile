# Target programs
programs := \
	queue_tester.x \
	uthread_hello.x \
	uthread_yield.x \
	sem_simple.x \
	sem_count.x \
	sem_buffer.x \
	sem_prime.x

# User-level thread library
UTHREADLIB := libuthread
UTHREADPATH := ../$(UTHREADLIB)
libuthread := $(UTHREADPATH)/$(UTHREADLIB).a

# Default rule
all: $(programs)

# Avoid builtin rules and variables
MAKEFLAGS += -rR

# Don't print the commands unless explicitly requested with `make V=1`
ifneq ($(V),1)
Q = @
V = 0
endif

# Current directory
CUR_PWD := $(shell pwd)

# Define compilation toolchain
CC	= gcc

# General gcc options
CFLAGS	:= -Wall -Wextra -Werror
CFLAGS	+= -pipe
## Debug flag
ifneq ($(D),1)
CFLAGS	+= -O2
else
CFLAGS	+= -g
endif
## Include path
CFLAGS 	+= -I$(UTHREADPATH)
## Dependency generation
CFLAGS	+= -MMD

# Linker options
LDFLAGS := -L$(UTHREADPATH) -luthread

# Application objects to compile
objs := $(patsubst %.x,%.o,$(programs))

# Include dependencies
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

# Rule for libuthread.a
$(libuthread): FORCE
	@echo "MAKE	$@"
	$(Q)$(MAKE) V=$(V) D=$(D) -C $(UTHREADPATH)

# Generic rule for linking final applications
%.x: %.o $(libuthread)
	@echo "LD	$@"
	$(Q)$(CC) -o $@ $< $(LDFLAGS)

# Generic rule for compiling objects
%.o: %.c
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

# Cleaning rule
clean: FORCE
	@echo "CLEAN	$(CUR_PWD)"
	$(Q)$(MAKE) V=$(V) D=$(D) -C $(UTHREADPATH) clean
	$(Q)rm -rf $(objs) $(deps) $(programs)

# Keep object files around
.PRECIOUS: %.o
.PHONY: FORCE
FORCE:




## Compiler
#CC = gcc
#
## Compiler flags
#CFLAGS = -Wall -Wextra -Werror -g
## Add any additional flags here
#
## Source files for queue tester example and uthread_hello
#QUEUE_TESTER_SRCS = queue_tester.c
#UTHREAD_HELLO_SRCS = uthread_hello.c
#
## Object files
#QUEUE_TESTER_OBJS = $(QUEUE_TESTER_SRCS:.c=.o)
#UTHREAD_HELLO_OBJS = $(UTHREAD_HELLO_SRCS:.c=.o)
#
## Targets
#all: queue_tester.x uthread_hello.x
#
## Compilation rules
#%.o: %.c
#	$(CC) $(CFLAGS) -c $< -o $@
#
## Linking rule for queue tester
#queue_tester.x: $(QUEUE_TESTER_OBJS)
#	$(CC) $(CFLAGS) $^ -o $@
#
## Linking rule for uthread_hello
#uthread_hello.x: $(UTHREAD_HELLO_OBJS)
#	$(CC) $(CFLAGS) $^ -o $@
#
## Clean rule
#clean:
#	rm -f $(QUEUE_TESTER_OBJS) $(UTHREAD_HELLO_OBJS) queue_tester.x uthread_hello.x

