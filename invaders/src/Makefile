# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CC = gcc

TARGET = ../invaders

LIBS_NAME_SUFFIX= $(shell uname -m)-$(shell uname -s | tr [:upper:] [:lower:])

CFLAGS  := -c -fpic -Wall -Werror -Wextra -Walloca \
		-fno-sanitize-recover=all -fsanitize=float-divide-by-zero \
		-fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment \
		-I../../cpu-8080/src/ \
		-I../../emu-machine/src/ \
		-I ../rom/ \
		 $(shell pkg-config --cflags gtk+-3.0 sdl2)

ifeq ($(CC), gcc)
  CFLAGS += -std=gnu17 -fsanitize=undefined 
else ifeq ($(CC), clang)
  CFLAGS += -O3 
endif


LDLIBS = $(shell pkg-config --libs gtk+-3.0 sdl2) -lubsan

DEPDIR := .deps

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

SRCS := $(wildcard *.c)

OBJS = $(SRCS:.c=.o)

DEPDIR = .deps

all: $(TARGET) 

%.o: %.c
%.o: %.c $(DEPDIR)/%.d | $(DEPDIR)
	$(CC) $(DEPFLAGS) -o $@ $(CFLAGS) $<

invaders_glade.h: invaders.glade
	xxd -i $< $@	

$(DEPDIR): ; @mkdir -p $@

EMU_LIBS += ../../emu-machine/libemu-machine-$(LIBS_NAME_SUFFIX).a
EMU_LIBS += ../../cpu-8080/libemu-cpu-8080-$(LIBS_NAME_SUFFIX).a

$(TARGET): ../rom/invaders.rom.h invaders_glade.h $(EMU_LIBS) $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS) $(EMU_LIBS) $(LDLIBS) 

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))

../rom/invaders.rom.h:
	make -C ../rom

clean:
	$(RM) -fr $(TARGET) *.o $(DEPDIR)

re: clean all
