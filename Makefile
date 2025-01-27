# See README for notes about project organization

DEPS=proc.h linkedlist.h
CFLAGS=-std=c99 -Wall

.PHONY:clean

all: 537ps

537ps: proc.o main.o linkedlist.o
	gcc -o 537ps main.o proc.o linkedlist.o

%.o: %.c $(DEPS)
ifeq ($(DEBUG),true)
	gcc -g -c -o $@ $< $(CFLAGS)
else
	gcc -c -o $@ $< $(CFLAGS)
endif

clean:
	rm *.o
	rm 537ps
