CFLAGS=-ansi -Wpedantic -Wall -Werror -D_THREAD_SAFE -D_REENTRANT -D_POSIX_C_SOURCE=200112L
LIBRARIES=-lpthread
LFLAGS=

all: cani.exe

cani.exe: cani.o DBGpthread.o
	gcc ${LFLAGS} -o cani.exe cani.o DBGpthread.o ${LIBRARIES}

cani.o: cani.c DBGpthread.h
	gcc -c ${CFLAGS} cani.c

DBGpthread.o: DBGpthread.c printerror.h
	gcc -c ${CFLAGS} DBGpthread.c

.PHONY: clean run

clean:
	rm -f *.exe *.o *~ core

run: cani.exe
	./cani.exe


