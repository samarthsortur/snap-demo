all: process1

process1: process1.o
	gcc -o process1 process1.o

process1.o: process1.c
	gcc -c process1.c

clean:
	rm -f process1 process1.o
