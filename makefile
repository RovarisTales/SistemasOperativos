all: server client

server: bin/sdstored

client: bin/sdstore

bin/sdstored: obj/sdstored.o
	gcc -g sdstored.o -o bin/sdstored

obj/sdstored.o: bin/sdstored.c
	gcc -Wall -g -c sdstored.c obj/sdstored.o

bin/sdstore: obj/sdstore.o
	gcc -g sdstored.o -o bin/sdstored

obj/sdstore.o: bin/sdstore.c
	gcc -Wall -g -c sdstore.c obj/sdstore.o

clean:
	rm obj/* bin/sdstored