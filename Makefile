DEPS=proc.h util.h
CFLAGS=-std=c99 -Wall

all: 537ps

537ps: proc.o main.o util.o
	gcc -o 537ps main.o proc.o util.o

%.o: %.c $(DEPS)
	gcc -c -o $@ $< $(CFLAGS)

.PHONY:clean

clean:
	rm *.o
	rm 537ps

