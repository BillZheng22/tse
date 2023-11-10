CFLAGS=-Wall -pedantic -std=c11 -I. -g

OFILES=queue.o hash.o webpage.o pageio.o index.o indexio.o lqueue.o

%.o: %.c %.h
	gcc $(CFLAGS) -c $<

all: 			$(OFILES)
	ar cr ../lib/libutils.a $(OFILES)

#tests for single-threaded lqueue
stest:	queue.o lqueue.o stest.o
	gcc $(CFLAGS) queue.o lqueue.o stest.o -o -lpthread $@

dtest:	queue.o lqueue.o dtest.o
	gcc $(CFLAGS) queue.o lqueue.o dtest.o -o dtest -lpthread $@

clean:
	rm -f webpage.o pageio.o index.o indexio.o lqueue.o ../lib/libutils.a
