CC=gcc
CFLAGS=-g -Wall

all: TTT ttt


TTT: TTT.o msg.o
	$(CC) -o TTT TTT.o msg.o
	

ttt: ttt.o msg.o
	$(CC) -o ttt ttt.o msg.o

TTT.o ttt.o msg.o: common.h

clean:
	rm -f *.o TTT ttt
