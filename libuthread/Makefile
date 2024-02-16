CC := gcc
CFLAGS := -Wall -Wextra -Werror -g
#-Werror  treats warning as errors (supposed to go after wall)
# Target 
lib := libuthread.a

# Source and object files 
srcs := queue.c uthread.c context.c preempt.c sem.c
objs := $(srcs:.c=.o)
deps := $(objs:.o=.d)

# Control verbosity
ifneq ($(V),1)
Q = @
endif

#build target 
all: $(lib)

# Include dependency files
-include $(deps)

# Library compilation
#use ar to create static lib 
# got a little help online for this as i was confused how to link this makefile and the apps one 
#  https://www.mkssoftware.com/docs/man1/ar.1.asp , https://stackoverflow.com/questions/29714300/what-does-the-rcs-option-in-ar-do
$(lib): $(objs)
	@echo "AR $@"
	$(Q)ar rcs $@ $^

# Object file compilation with automatic dependency generation
%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -MMD -c $< -o $@

clean:
	@echo "CLEAN"
	$(Q)rm -f $(lib) $(objs) $(deps)

#got help one this step, saw it recommended online, makes sure if files named clean or all theyll execute, make it so we can do make all/ make clean 
#https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html , https://stackoverflow.com/questions/2145590/what-is-the-purpose-of-phony-in-a-makefile
.PHONY: all clean
