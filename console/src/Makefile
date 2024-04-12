# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CC = gcc

override CFLAGS += -I../../cpu-8080/src/ -I../../emu-machine/src/

EMU_LIBS = ../../emu-machine/libemu-machine$(LIBS_NAME_SUFFIX).a
EMU_LIBS += ../../cpu-8080/libemu-cpu-8080$(LIBS_NAME_SUFFIX).a

CFILES := $(wildcard *.c)

OBJS = $(CFILES:.c=.o)

DEPDIR = .deps

# The build target 
TARGET = console

all: $(TARGET)

-include $(OBJS:%.o=$(DEPDIR)/%.Po)

%.o: %.c 
	@mkdir -p $(DEPDIR)
	$(CC) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $(CFLAGS) $<
	@mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po

%.s19: %.asm 
	asmx -C 8080 -s19 -e -c -l -- $< > $@

diag.s19: page0.s19 diag-bdos.s19 cpudiag.s19
	cat $^ > $@

test-bdos-image.s19:page0.s19 diag-bdos.s19 test-bdos.s19
	cat $^ > $@


$(TARGET): $(OBJS) diag.s19
	$(CC) -o $(TARGET) $(OBJS) $(EMU_LIBS)

clean:
	$(RM) $(TARGET) *.o *.bin *.lst *.s19

re: clean all
