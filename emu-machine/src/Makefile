# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CC = gcc

LIBRARY = emu-machine

LIBS_NAME_SUFFIX= $(shell uname -m)-$(shell uname -s | tr [:upper:] [:lower:])

NAME    :=  lib$(LIBRARY)-$(LIBS_NAME_SUFFIX)

TARGET   :=  ../$(NAME).a

CFLAGS  := -fpic -Wall -Werror -Wextra -Walloca \
		-fno-sanitize-recover=all -fsanitize=float-divide-by-zero \
		-fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment \
		-I../wav/ -c \
		-I../../cpu-8080/src/ -c

ifeq ($(CC), gcc)
  CFLAGS += -std=gnu17 -fsanitize=undefined 
else ifeq ($(CC), clang)
  CFLAGS += -O3 
endif

LIBS :=

LDLIBS =

LDFLAGS :=  -L.

SRCS := $(wildcard *.c)

OBJS = $(SRCS:.c=.o)

DEPDIR := .deps

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

all: $(TARGET)

%.o: %.c
%.o: %.c $(DEPDIR)/%.d | $(DEPDIR)
	make -C ../wav
	$(CC) $(DEPFLAGS) -o $@ $(CFLAGS) $<

../wav/sound_data.h:
	make -C ../wav

$(DEPDIR): ; @mkdir -p $@

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)

$(DEPFILES):

include $(wildcard $(DEPFILES))


clean:
	$(RM) -fr $(TARGET) *.o $(DEPDIR)
	make -C ../wav clean

re: clean all
