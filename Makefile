CFLAGS += -std=c99 -Wall -Wextra -pedantic -Wold-style-declaration -O2
CFLAGS += -Wno-unused-parameter
PREFIX  ?= /usr
BINDIR  ?= $(PREFIX)/bin
MANDIR  ?= $(PREFIX)/share/man
CC      ?= gcc

all: tlc

config.h:
	cp config.def.h config.h

tlc: main.c urls.h config.h Makefile
	$(CC) -O3 $(CFLAGS) -o $@ $< -lcurl -ljansson $(LDFLAGS)

install: all
	install -Dm755 tlc $(DESTDIR)$(BINDIR)/tlc
	install -Dm644 tlc.1 $(DESTDIR)$(MANDIR)/man1/tlc.1

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/tlc
	rm -f $(DESTDIR)$(MANDIR)/man1/tlc.1

clean:
	rm -f tlc *.o cookies.txt

.PHONY: all install uninstall clean

