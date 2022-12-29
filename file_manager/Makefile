CC=gcc
CFLAGS=-c -Wall

all: file_manager file_client

file_manager: file_manager.o
	$(CC) file_manager.o -o file_manager

file_manager.o: file_manager.c
	$(CC) $(CFLAGS) file_manager.c

file_client: file_client.o
	$(CC) file_client.o -o file_client

file_client.o: file_client.c
	$(CC) $(CFLAGS) file_client.c

clean:
	rm -rf *.o file_manager file_client