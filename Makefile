TARGETARCH := i686


SYSROOT := ./sysroot
DESTDIR := $(SYSROOT)

PREFIX := /usr

INCLUDEDIR := $(PREFIX)/include
BINDIR := /bin
BOOTDIR := /boot

PROJDIRS := kernel libc

CC := $(TARGETARCH)-elf-karacaos-gcc --sysroot=$(SYSROOT)
AS := $(TARGETARCH)-elf-karacaos-as
AR := $(TARGETARCH)-elf-karacaos-ar

LD := $(TARGETARCH)-elf-ld

CFLAGS := -isystem=$(INCLUDEDIR) -g -Wall -Wextra

include $(addsuffix /make.config,$(PROJDIRS))

.PHONY: all clean install

all: mkdirs $(PROJDIRS)

clean: $(addsuffix -clean,$(PROJDIRS))
	rm -rf $(SYSROOT)

install: mkdirs $(addsuffix -install,$(PROJDIRS))

mkdirs:
	mkdir -p $(SYSROOT)
	mkdir -p $(DESTDIR)/$(INCLUDEDIR)
	mkdir -p $(DESTDIR)/$(BINDIR)
	mkdir -p $(DESTDIR)/$(BOOTDIR)
