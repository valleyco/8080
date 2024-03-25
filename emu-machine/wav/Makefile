SRCS := $(wildcard *.wav)

HEADERS = $(SRCS:.wav=.h)

all: sound_data.h sound_data_len.h

sound_data.h: $(HEADERS)
	cat $(HEADERS) >> $@

sound_data_len.h: sound_data.h
	grep "unsigned int .*_len = " $< | tr '[:lower:]' '[:upper:]' | sed  's/UNSIGNED INT/#define/g' | sed 's/[=;]/ /g' > $@

%.h: %.wav
	xxd -i -s 44 $< $@ 
clean:
	rm -f *.h