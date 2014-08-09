CFLAGS += -Wall -Wextra -O3

SRCS=$(wildcard src/*.c) $(wildcard src/md5/*.c)
DEPS=$(wildcard src/*.h) $(wildcard src/md5/*.h) $(wildcard include/*.h)
OBJS=$(SRCS:.c=.o)

LIBS = libbsmp.a libbsmp.so
HDRS = include/bsmp.h include/server.h include/client.h
INSTALL ?= /usr/bin/install
INSTALL_FLAGS = -c -m 644
LDCONFIG ?= /sbin/ldconfig
PREFIX ?= /usr
LDIR = $(PREFIX)/lib
IDIR = $(PREFIX)/include/bsmp

.phony: all install clean distclean

all: libbsmp.a libbsmp.so

install: $(LIBS) $(HDRS)
	$(INSTALL) $(INSTALL_FLAGS) $(LIBS) $(LDIR)
	$(LDCONFIG) -n $(LDIR)
	mkdir -p $(IDIR)
	$(INSTALL) $(INSTALL_FLAGS) $(HDRS) $(IDIR)

libbsmp.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

libbsmp.so: $(OBJS)
	$(CC) -shared -Wl,-soname,$@ -o $@ $(OBJS)

%.o: %.c $(DEPS)
	$(CC) -c -fPIC -o $@ $< $(CFLAGS)

clean:
	@- $(RM) libbsmp.a libbsmp.so $(OBJS)

uninstall:
	$(RM) $(LDIR)/libbsmp.a $(LDIR)/libbsmp.so
	$(RM) -r $(IDIR)

distclean: clean
