CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g

LIBS=-lutils -lcurl

all: dtest stest

#tests for single-threaded lqueue
stest:	
	gcc $(CFLAGS) stest.c $(LIBS) -o $@

#tests for dual-threaded lqueue
dtest:
	gcc $(CFLAGS) dtest.c $(LIBS) -o $@

clean:
	rm -f dtest stest