CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g

LIBS=-lutils -lcurl

all: mindexer

mindexer:
	gcc $(CFLAGS) mindexer.c $(LIBS) -o $@

clean:
	rm -f mindexer