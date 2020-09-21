DEPS=proc/*.h proc.h util.h linkedlist.h
CFLAGS=-std=c99 -Wall

.PHONY:clean

all: 537ps

proc: proc/*.o
	gcc -o proc proc/*.o

537ps: proc main.o util.o linkedlist.o
	gcc -o 537ps main.o proc.o util.o linkedlist.o

%.o: %.c $(DEPS)
ifeq ($(DEBUG),true)
	gcc -g -c -o $@ $< $(CFLAGS)
else
	gcc -c -o $@ $< $(CFLAGS)
endif

clean:
	rm *.o
	rm 537ps

