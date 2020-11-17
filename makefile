CC=gcc
CFLAGS=-Wall -O
SSRC=functions.c server_functions.c
SOBJ=$(SSRC:.c=.o)
CSRC=functions.c client_functions.c
COBJ=$(CSRC:.c=.o)
SH=server_functions.h functions.h
CH=client_functions.h functions.h
EXEC=client racine domaine racine_sleep domaine_sleep


all:
	make $(EXEC)

client: client.o $(COBJ)
	$(CC) -o $@ $^

racine: racine.o $(SOBJ)
	$(CC) -o $@ $^

racine_sleep: racine_sleep.o $(SOBJ)
	$(CC) -o $@ $^

domaine: domaine.o $(SOBJ)
	$(CC) -o $@ $^

domaine_sleep: domaine_sleep.o $(SOBJ)
	$(CC) -o $@ $^

%.o: %.c $(CH)
	$(CC) -o $@ -c $<

%.o: %.c %.h
	$(CC) -o $@ -c $<

%.o: %.c $(SH)
	$(CC) -o $@ -c $<

clear:
	rm *.o $(EXEC) siham-janati.tar.gz

tar:
	tar -czvf siham-janati.tar.gz *.c *.h files makefile tests "doc + rapport"
