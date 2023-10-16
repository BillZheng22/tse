CFLAGS=-Wall -pedantic -std=c11 -I. -g

all: 			list_test

list.o:		list.c list.h
					gcc $(CFLAGS) -c list.c

list_test.o:	list_test.c list.h
							gcc $(CFLAGS) -c list_test.c

list_test:	list_test.o list.o
						gcc $(CFLAGS) list_test.o list.o -o list_test

clean:
				rm -f *.o list_test
