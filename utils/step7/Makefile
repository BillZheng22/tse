CFLAGS=-Wall -pedantic -std=c11 -I. -g

all:	hash_test

%.o: %.c %.h
	gcc $(CFLAGS) -c $<

hash_test: hash.o hash_test.o queue.o
	gcc $(CFLAGS) hash.o hash_test.o queue.o -o $@

clean:
	rm -f *.o hash_test
