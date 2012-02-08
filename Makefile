CC = g++
CCFLAGS = -Wall -g -std=c++0x -lboost_thread

all: server client
clean: cleanserver cleanclient cleanlibs
	rm -f *.core
test: all
	server 12345 &
	client localhost 12345


server: src/server.o src/net/net.o  Makefile
	${CC} ${CCFLAGS} -o server src/server.o src/net/net.o
cleanserver:
	rm -f server src/server.o

client: src/client.o src/net/net.o Makefile
	${CC} ${CCFLAGS} -o client src/client.o src/net/net.o
cleanclient:
	rm -f client src/client.o

cleanlibs:
	rm -f src/*/*.o
src/net/net.o: src/net/net.cpp Makefile
	${CC} ${CCFLAGS} -o src/net/net.o -c src/net/net.cpp
