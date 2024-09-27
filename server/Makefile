CC = gcc
CFLAGS = $(shell pkg-config --cflags dbus-1)
LDFLAGS = $(shell pkg-config --libs dbus-1)

server: server.c
	$(CC) -o server server.c $(CFLAGS) $(LDFLAGS)

clean:
	rm -f server
