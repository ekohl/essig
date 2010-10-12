CC=avr-gcc
#CC=gcc
CFLAGS=-g -Wall -O2

all: empty factorial

empty:
	$(CC) $(CFLAGS) -o empty empty.c

factorial:
	$(CC) $(CFLAGS) -o factorial factorial.c

clean:
	git clean -d -X -f
