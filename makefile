all: server main

server: server.o
	gcc -o server server.o

main: main.o tcp.o
	gcc -o main main.o tcp.o

server.o: server.c
	gcc -c server.c

main.o: main.c
	gcc -c main.c

tcp.o: tcp.c
	gcc -c tcp.c

clean:
	rm -rf *.o server main