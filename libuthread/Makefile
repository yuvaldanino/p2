# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -g
#-Werror  treats warning as errors (supposed to go after wall)
# Target library
lib := libuthread.a

# Source and object files for the library
srcs := queue.c uthread.c context.c preempt.c sem.c
objs := $(srcs:.c=.o)
deps := $(objs:.o=.d)

# Control verbosity
ifneq ($(V),1)
Q = @
endif

all: $(lib)

# Include dependency files
-include $(deps)

# Library compilation
$(lib): $(objs)
	@echo "AR $@"
	$(Q)ar rcs $@ $^

# Object file compilation with automatic dependency generation
%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -MMD -c $< -o $@

clean:
	@echo "Cleaning"
	$(Q)rm -f $(lib) $(objs) $(deps)

.PHONY: all clean
