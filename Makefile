DEPS=proc.h util.h linkedlist.h
CFLAGS=-std=c99 -Wall

all: 537ps

537ps: proc.o main.o util.o linkedlist.o
	gcc -o 537ps main.o proc.o util.o linkedlist.o

%.o: %.c $(DEPS)
ifeq ($(DEBUG),true)
	gcc -g -c -o $@ $< $(CFLAGS)
else
	gcc -c -o $@ $< $(CFLAGS)
endif

.PHONY:clean

clean:
	rm *.o
	rm 537ps

