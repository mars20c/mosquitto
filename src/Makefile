CC=cc
CFLAGS=-I. -Wall -ggdb
LDFLAGS=-lsqlite3 -nopie

.PHONY: all clean

all : mosquitto

mosquitto : mosquitto.o context.o database.o managed_send.o messages.o net.o raw_send.o read_handle.o read_handle_server.o util.o
	${CC} $^ -o $@ ${LDFLAGS}

mosquitto.o : mosquitto.c mqtt3.h
	${CC} $(CFLAGS) -c $< -o $@

context.o : context.c mqtt3.h
	${CC} $(CFLAGS) -c $< -o $@

database.o : database.c mqtt3.h
	${CC} $(CFLAGS) -c $< -o $@

managed_send.o : managed_send.c mqtt3.h
	${CC} $(CFLAGS) -c $< -o $@

messages.o : messages.c mqtt3.h
	${CC} $(CFLAGS) -c $< -o $@

net.o : net.c mqtt3.h
	${CC} $(CFLAGS) -c $< -o $@

raw_send.o : raw_send.c mqtt3.h
	${CC} $(CFLAGS) -c $< -o $@

read_handle.o : read_handle.c mqtt3.h
	${CC} $(CFLAGS) -c $< -o $@

read_handle_server.o : read_handle_server.c mqtt3.h
	${CC} $(CFLAGS) -c $< -o $@

util.o : util.c mqtt3.h
	${CC} $(CFLAGS) -c $< -o $@

clean : 
	-rm -f *.o mosquitto