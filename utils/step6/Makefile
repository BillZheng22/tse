CFLAGS=-Wall -pedantic -std=c11 -I. -g

all:	queue_test

%.o: %.c %.h
	gcc $(CFLAGS) -c $<

queue_test: queue.o queue_test.o
	gcc $(CFLAGS) queue.o queue_test.o -o $@

clean:
				rm -f *.o queue_test
