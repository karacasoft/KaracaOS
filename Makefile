TARGETARCH := i686

SYSROOT := ./sysroot
DESTDIR := $(SYSROOT)

PREFIX := /usr

INCLUDEDIR := $(PREFIX)/include
BINDIR := /bin
BOOTDIR := /boot

PROJDIRS := kernel

CC := $(TARGETARCH)-elf-gcc --sysroot=$(SYSROOT)
AS := $(TARGETARCH)-elf-as
AR := $(TARGETARCH)-elf-ar

LD := $(TARGETARCH)-elf-ld

CFLAGS := -isystem=$(INCLUDEDIR) -g -std=gnu11 -Wall -Wextra -fvar-tracking

include $(addsuffix /make.config,$(PROJDIRS))

.PHONY: all clean install

all: mkdirs $(PROJDIRS)

clean: $(addsuffix -clean,$(PROJDIRS))
	rm -rf $(SYSROOT)

install: $(addsuffix -install,$(PROJDIRS))

mkdirs:
	mkdir -p $(SYSROOT)
	mkdir -p $(DESTDIR)/$(INCLUDEDIR)
	mkdir -p $(DESTDIR)/$(BINDIR)
	mkdir -p $(DESTDIR)/$(BOOTDIR)
