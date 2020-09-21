DEPS=proc/*.h proc.h util.h linkedlist.h
CFLAGS=-std=c99 -Wall

.PHONY:clean

all: 537ps

537ps: proc.o main.o util.o linkedlist.o
	gcc -o 537ps proc.o main.o util.o linkedlist.o

proc.o:
	make -C proc/

%.o: %.c $(DEPS)
ifeq ($(DEBUG),true)
	gcc -g -c -o $@ $< $(CFLAGS)
else
	gcc -c -o $@ $< $(CFLAGS)
endif

clean:
	rm *.o
	rm proc/*.o
	rm 537ps
