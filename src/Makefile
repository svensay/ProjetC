CC=gcc
CFLAGS = -g -Wall -pedantic -std=c11 

ALL= union select tri moyenne_ponderee fusion formatage

all: $(ALL)

union: union.o fonction.o
	$(CC) -o union fonction.o union.o $(CFLAGS) 
union.o: union.c fonction.h
	$(CC) -o union.o -c union.c $(CFLAGS)

select: select.o fonction.o
	$(CC) -o select fonction.o select.o $(CFLAGS) 
select.o: select.c fonction.h
	$(CC) -o select.o -c select.c $(CFLAGS)

tri: tri.o fonction.o
	$(CC) -o tri fonction.o tri.o $(CFLAGS) 
tri.o: tri.c fonction.h
	$(CC) -o tri.o -c tri.c $(CFLAGS)

moyenne_ponderee: moyenne_ponderee.o fonction.o
	$(CC) -o moyenne_ponderee fonction.o moyenne_ponderee.o $(CFLAGS) 
moyenne_ponderee.o: moyenne_ponderee.c fonction.h
	$(CC) -o moyenne_ponderee.o -c moyenne_ponderee.c $(CFLAGS)

fusion: fusion.o fonction.o
	$(CC) -o fusion fonction.o fusion.o $(CFLAGS) 
fusion.o: fusion.c fonction.h
	$(CC) -o fusion.o -c fusion.c $(CFLAGS)

formatage: formatage.o fonction.o
	$(CC) -o formatage fonction.o formatage.o $(CFLAGS) 
formatage.o: formatage.c fonction.h
	$(CC) -o formatage.o -c formatage.c $(CFLAGS)

fonction.o: fonction.c fonction.h
	$(CC) -o fonction.o -c fonction.c $(CFLAGS)

.PHONY=clean all

clean:
	rm -rf *~ $(ALL) *.o
