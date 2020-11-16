CC=gcc
CFLAGS=-Wall -O
SSRC=functions.c server_functions.c
SOBJ=$(SSRC:.c=.o)
CSRC=functions.c client_functions.c
COBJ=$(CSRC:.c=.o)
SH=server_functions.h functions.h
CH=client_functions.h functions.h
EXEC=client racine domaine
SERVERS=racine domaine

all:
	make client $(SERVERS)

client: client.o $(COBJ)
	$(CC) -o $@ $^

racine: racine.o $(SOBJ)
	$(CC) -o $@ $^

domaine: domaine.o $(SOBJ)
	$(CC) -o $@ $^

%.o: %.c $(CH)
	$(CC) -o $@ -c $<

%.o: %.c %.h
	$(CC) -o $@ -c $<

%.o: %.c $(SH)
	$(CC) -o $@ -c $<
#lance tous les serveurs
clear:
	rm *.o $(EXEC)
#ferme les ports ouverts
close:
	lsof -t -i:8050,8051,8090,8000,8004 | xargs kill -9
