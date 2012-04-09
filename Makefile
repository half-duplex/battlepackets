CC = g++
CCFLAGS = -Wall -g -std=c++0x -lboost_thread `pkg-config gtkmm-2.4 --cflags --libs`

all: server client
clean: cleanserver cleanclient cleancheck cleanlibs
	rm -f *.core

server: src/server.o src/net/net.o src/common.o Makefile
	${CC} ${CCFLAGS} -o server src/server.o src/net/net.o src/common.o
src/server.o: Makefile
	${CC} ${CCFLAGS} -o src/server.o -c src/server.cpp
cleanserver:
	rm -f server src/server.o

client: src/client.o src/net/net.o src/common.o Makefile
	${CC} ${CCFLAGS} -o client src/client.o src/net/net.o src/common.o
src/client.o: Makefile
	${CC} ${CCFLAGS} -o src/client.o -c src/client.cpp
cleanclient:
	rm -f client src/client.o

check: src/check.o src/client.o src/server.o src/net/net.o src/common.o Makefile
	${CC} ${CCFLAGS} -o check src/check.o src/client.o src/server.o src/net/net.o src/common.o
	./check
src/check.o: Makefile
	${CC} ${CCFLAGS} -o src/check.o -c src/check.cpp
cleancheck:
	rm -f check src/check.o


cleanlibs:
	rm -f src/*/*.o src/common.o
src/net/net.o: src/net/net.cpp Makefile
	${CC} ${CCFLAGS} -o src/net/net.o -c src/net/net.cpp
src/common.o: src/net/net.cpp Makefile
	${CC} ${CCFLAGS} -o src/common.o -c src/common.cpp
