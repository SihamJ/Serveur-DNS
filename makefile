CC=gcc
CFLAGS=-Wall -O
SSRC=functions.c server_functions.c
SOBJ=$(SSRC:.c=.o)
CSRC=functions.c client_functions.c
COBJ=$(CSRC:.c=.o)
SH=server_functions.h functions.h
CH=client_functions.h functions.h
EXEC=client R1 R2 UK MA1 MA2 FR US sapienceUS aouniUS queenmaryUK

all:
	make client R1 R2 UK MA1 MA2 FR US sapienceUS aouniUS queenmaryUK

client: client.o $(COBJ)
	$(CC) -o $@ $^

R1: R1.o $(SOBJ)
	$(CC) -o $@ $^

R2: R2.o $(SOBJ)
	$(CC) -o $@ $^

US: US.o $(SOBJ)
	$(CC) -o $@ $^

UK: UK.o $(SOBJ)
	$(CC) -o $@ $^

MA1: MA1.o $(SOBJ)
	$(CC) -o $@ $^

MA2: MA2.o $(SOBJ)
	$(CC) -o $@ $^

FR: FR.o $(SOBJ)
	$(CC) -o $@ $^

sapienceUS: sapienceUS.o $(SOBJ)
	$(CC) -o $@ $^

aouniUS: aouniUS.o $(SOBJ)
	$(CC) -o $@ $^

queenmaryUK: queenmaryUK.o $(SOBJ)
	$(CC) -o $@ $^

%.o: %.c $(CH)
	$(CC) -o $@ -c $<

%.o: %.c %.h
	$(CC) -o $@ -c $<

%.o: %.c $(SH)
	$(CC) -o $@ -c $<
#lance tous les serveurs
main:
	./R1 files/serveursNomsR& ./R2 files/serveursNomsR&	./US files/serveursUS&	./UK files/serveursUK&	./MA1 files/serveursMA&	./MA2 files/serveursMA&	./FR files/serveursFR&	./sapienceUS files/serveursSapienceUS&	./queenmaryUK files/serveursQueenmaryUK&	./aouniUS files/serveursAouniUS&

clear:
	rm *.o $(EXEC)
#ferme les ports ouverts
close:
	lsof -t -i:8050,8051,8090,8030,8031,8032,8033,8034,8035,8060,8066,8067,8001,8002,8070,8080,8061,8062,8063,8064,8065,8040,8041,8043,8044,8045,8050,8051,8000,8053,8054,8055 | xargs kill -9
