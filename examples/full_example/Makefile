CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-lbsmp

all: client.c server.c
	$(CC) $(CFLAGS) server.c client.c -o example $(LDFLAGS)

clean: example
	@rm example