CC=mpicc
CFLAG= -Wall -I. -O2

TARGETS=main # add your target here

all: $(TARGETS)

microtime.o: microtime.c microtime.h
	$(CC) $(CFLAG) -c $<

main: main.o microtime.o
	$(CC) -o $@ $^ -lm

main.o: main.c microtime.h
	$(CC) $(CFLAG) -c $<


clean:
	rm -f *.o *~ core $(TARGETS)
