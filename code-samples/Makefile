CC=avr-gcc
#CC=gcc
OBJDUMP=avr-objdump
#OBJDUMP=objdump
CFLAGS=-g -Wall -mmcu=atmega16
ALL=empty factorial

.PHONY: all
all: $(ALL)

.PHONY: dump
dump: factorial
	$(OBJDUMP) -d factorial | less

.PHONY: empty-dump
empty-dump: empty
	$(OBJDUMP) -d empty | less

.PHONY: clean
clean:
	git clean -d -X -f
