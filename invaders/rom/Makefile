
.PHONY: clean re all

ROMS := invaders.h invaders.g invaders.f invaders.e

all: invaders.rom.h

invaders.rom.h: invaders.rom
	xxd -i $< $@

invaders.rom: $(ROMS)
	cat $^ > invaders.rom

clean:
	rm -f invaders.rom invaders.rom.h

re: clean all
