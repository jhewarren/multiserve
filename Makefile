# make for multiserve

CC=gcc
CFLAGS=-Wall -ggdb
TH=-pthread
MP=-fopenmp
SW=lib/socketwrapper.c

all: mec ths polls eps

clean :
	rm -f *.o mec ths epoll polls core* *~

mec:
	$(CC) $(TH) $(CFLAGS) client.c -o mec

ths:
	$(CC) $(TH) $(CFLAGS) thread_server.c $(SW) -o ths

eps:
	$(CC) $(MP) $(CFLAGS) epoll.c -o eps

polls:
	$(CC) $(CFLAGS) poll_server.c $(SW) -o polls

