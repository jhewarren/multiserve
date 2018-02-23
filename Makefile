# make for mping

CC=gcc
CFLAGS=-Wall -ggdb

mping: mping.o proc_v4.o send_v4.o
	$(CC) $(CFLAGS) mping.o proc_v4.o send_v4.o -o mping

clean:
	rm -f *.o mping core* *~

mping.o: mping.c
	$(CC) $(CFLAGS) -O -c mping.c

proc_v4.o: proc_v4.c
	$(CC) $(CFLAGS) -O -c proc_v4.c

send_v4.o: send_v4.c
	$(CC) $(CFLAGS) -O -c send_v4.c


