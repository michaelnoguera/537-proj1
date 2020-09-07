DEPS=proc.h
CFLAGS=-std=c99 -Wall

all: 537ps

537ps: proc.o main.o
	gcc -o 537ps main.o proc.o

%.o: %.c $(DEPS)
	gcc -c -o $@ $< $(CFLAGS)

clean:
	rm *.o
	rm 537ps
