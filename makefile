all: server client

server: sdstored

client: sdstore

sdstored: sdstored.c
	gcc -Wall sdstored.c -o sdstored

sdstore: sdstore.c
	gcc -Wall sdstore.c -o sdstore

clean:
	rm {sdstore,sdstored}