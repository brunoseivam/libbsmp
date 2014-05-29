CFLAGS += -Wall -Wextra -O3

SRCS=$(wildcard src/*.c) $(wildcard src/md5/*.c)
DEPS=$(wildcard src/*.h) $(wildcard src/md5/*.h) $(wildcard include/*.h)
OBJS=$(SRCS:.c=.o)

INSTALL ?= /usr/bin/install
INSTALL_FLAGS = -c -m 644
LDCONFIG ?= /sbin/ldconfig
PREFIX ?= /usr
LDIR = $(PREFIX)/lib

.phony: all install clean distclean

all: libbsmp.a libbsmp.so

install: libbsmp.a libbsmp.so
	$(INSTALL) $(INSTALL_FLAGS) libbsmp.a libbsmp.so $(LDIR)
	$(LDCONFIG) -n $(LDIR)

libbsmp.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

libbsmp.so: $(OBJS)
	$(CC) -shared -Wl,-soname,$@ -o $@ $(OBJS)

%.o: %.c $(DEPS)
	$(CC) -c -fPIC -o $@ $< $(CFLAGS)

clean:
	@- $(RM) libbsmp.a libbsmp.so $(OBJS)

distclean: clean
