CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g

LIBS=-lutils -lcurl

all: dhtest

#tests for single-threaded lhash
#stest:	
#	gcc $(CFLAGS) shtest.c $(LIBS) -o $@

#tests for dual-threaded lhash
dhtest:
	gcc $(CFLAGS) dhtest.c $(LIBS) -o $@

clean:
	rm -f dhtest 