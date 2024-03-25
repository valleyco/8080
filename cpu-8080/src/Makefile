# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CC = gcc

.PHONY: test

LIBRARY = emu-cpu-8080

NAME_SUFFIX= $(shell uname -m)-$(shell uname -s | tr [:upper:] [:lower:])

NAME    :=  lib$(LIBRARY)-$(NAME_SUFFIX)
SNAME   :=  ../$(NAME).a
DNAME   :=  ../$(NAME).so

CFLAGS  := -fpic -Wall -Werror -Wextra  -Walloca \
		-fno-sanitize-recover=all -fsanitize=float-divide-by-zero \
		-fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment -c

ifeq ($(CC), gcc)
  CFLAGS += -std=gnu17 -Walloc-zero -fsanitize=undefined 
else
  CFLAGS += -O3 
endif

LDFLAGS :=  -L.

SRCS := $(wildcard *.c)

OBJS = $(SRCS:.c=.o)

DEPDIR = .deps

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

all: $(SNAME) $(DNAME) test

LDLIBS = -lubsan

%.o: %.c
%.o: %.c $(DEPDIR)/%.d | $(DEPDIR)
	$(CC) $(DEPFLAGS) -o $@ $(CFLAGS) $<


$(DEPDIR): ; @mkdir -p $@

$(SNAME): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(DNAME): $(OBJS)
	$(CC) $(LDFLAGS) -shared $^ -o $@ $(LDLIBS)

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)

$(DEPFILES):

include $(wildcard $(DEPFILES))



clean:
	$(RM) -fr $(SNAME) $(DNAME) *.o $(DEPDIR)

re: clean all

test:
	make -C ../test CPULIB=$(DNAME)