#
# Import the build engine first
__nmk_dir=$(CURDIR)/../../scripts/
export __nmk_dir

include $(__nmk_dir)/include.mk
include $(__nmk_dir)/macro.mk

$(eval $(call gen-built-in,src))

clean:
	$(Q) $(MAKE) $(build)=src clean

all: src/built-in.o
	@true

.DEFAULT_GOAL := all
