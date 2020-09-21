all: proc.o
	
proc.o: stat.o
	gcc -r *.o -o ../proc.o

%.o: %.c $(DEPS)
ifeq ($(DEBUG),true)
	gcc -g -c -o $@ $< $(CFLAGS)
else
	gcc -c -o $@ $< $(CFLAGS)
endif