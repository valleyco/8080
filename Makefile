SUBDIRS := cpu-8080 emu-machine invaders console

.PHONY: $(TOPTARGETS) $(SUBDIRS)

TOPTARGETS := all clean re

$(TOPTARGETS): $(SUBDIRS)

# LIBS_NAME_SUFFIX= -$(shell uname -m)-$(shell uname -s | tr [:upper:] [:lower:])

CFLAGS := -c -fpic -Wall -Werror -Wextra -Walloca \
		-fno-sanitize-recover=all -fsanitize=float-divide-by-zero \
		-fno-sanitize=null -fno-sanitize=alignment

ifeq ($(CC), gcc)
	CFLAGS += -std=gnu17 -Walloc-zero -fsanitize=undefined -fsanitize=float-cast-overflow
	LDLIBS := -lubsan
else ifeq ($(CC), clang)
	LDLIBS :=
endif

$(SUBDIRS):
	$(MAKE) -C $@/src $(MAKECMDGOALS) LIBS_NAME_SUFFIX="$(LIBS_NAME_SUFFIX)" CFLAGS="$(CFLAGS)" CC=$(CC) LDLIBS="$(LDLIBS)"
