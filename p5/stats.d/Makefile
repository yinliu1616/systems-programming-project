CC=gcc
CFLAGS=-g -Wall -D_REENTRANT
LIBS=-lpthread

refstats: refstats.c
	$(CC) $(CFLAGS) -o refstats refstats.c $(LIBS)

clean:
	rm -f refstats core.*

