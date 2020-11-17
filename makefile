CC=gcc
CFLAGS=-Wall -O
SSRC=functions.c server_functions.c
SOBJ=$(SSRC:.c=.o)
CSRC=functions.c client_functions.c
COBJ=$(CSRC:.c=.o)
SH=server_functions.h functions.h
CH=client_functions.h functions.h
EXEC=client racine domaine racine_sleep
SERVERS=racine domaine racine_sleep

all:
	make client $(SERVERS)

client: client.o $(COBJ)
	$(CC) -o $@ $^

racine: racine.o $(SOBJ)
	$(CC) -o $@ $^

racine_sleep: racine_sleep.o $(SOBJ)
	$(CC) -o $@ $^

domaine: domaine.o $(SOBJ)
	$(CC) -o $@ $^

%.o: %.c $(CH)
	$(CC) -o $@ -c $<

%.o: %.c %.h
	$(CC) -o $@ -c $<

%.o: %.c $(SH)
	$(CC) -o $@ -c $<

clear:
	rm *.o $(EXEC)

tar:
	tar -czvf sihamJANATI.tar.gz *.c *.h files makefile tests
