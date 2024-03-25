SUBDIRS := cpu-8080 emu-machine invaders 

.PHONY: $(TOPTARGETS) $(SUBDIRS)

TOPTARGETS := all clean re

$(TOPTARGETS): $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@/src $(MAKECMDGOALS)
