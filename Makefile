__nmk_dir=scripts/nmk/scripts/
export __nmk_dir

PROGRAM := flog_reader

OPTS=-ggdb3 -Wall -Werror
export OPTS

include $(__nmk_dir)include.mk
include $(__nmk_dir)macro.mk

$(eval $(call gen-built-in,src))

built-in.o: src/built-in.o
	$(call msg-gen, $@)
	$(Q) $(LD) -r -o $@ built-in.S $^

$(PROGRAM): built-in.o
	$(call msg-gen, $@)
	$(Q) $(CC) $(OPTS)  -lffi -o $@ $^

all: src $(PROGRAM)
	@true
.PHONY: all

clean:
	$(call msg-gen, $@)
	$(Q) $(MAKE) $(build)=src clean
	$(Q) $(RM) built-in.o
	$(Q) $(RM) $(PROGRAM)
.PHONY: clean

test:
	./tests/test00

.DEFAULT_GOAL := all
