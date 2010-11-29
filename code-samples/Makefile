CC=avr-gcc
#CC=gcc
CFLAGS=-g -Wall -O1 -mmcu=atmega16

all: empty factorial

dump: factorial
	avr-objdump -d factorial | less

empty-dump: empty
	avr-objdump -d empty | less

clean:
	git clean -d -X -f
