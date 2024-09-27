CC = gcc
CFLAGS = $(shell pkg-config --cflags dbus-1)
LDFLAGS = $(shell pkg-config --libs dbus-1)

client: client.c
	$(CC) -o client client.c $(CFLAGS) $(LDFLAGS)

clean:
	rm -f client

