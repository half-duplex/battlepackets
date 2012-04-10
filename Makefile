CC = g++
CCFLAGS = -Wall -g -std=c++0x -lboost_thread `pkg-config gtkmm-2.4 --cflags --libs`

all: client server
clean:
	rm -f *.core src/*/*.o src/*.o bin/client bin/server bin/object/*.o

client: src/client_wrap.o src/client.o src/net/net.o src/common.o Makefile
	${CC} ${CCFLAGS} -o client src/client_wrap.o src/client.o src/net/net.o src/common.o
server: src/server_wrap.o src/server.o src/net/net.o src/common.o Makefile
	${CC} ${CCFLAGS} -o server src/server_wrap.o src/server.o src/net/net.o src/common.o
check: src/check.o src/client.o src/server.o src/net/net.o src/common.o Makefile
	${CC} ${CCFLAGS} -o check src/check.o src/client.o src/server.o src/net/net.o src/common.o
	./check

src/check.o: Makefile
	${CC} ${CCFLAGS} -o src/check.o -c src/check.cpp
src/common.o: src/net/net.cpp Makefile
	${CC} ${CCFLAGS} -o src/common.o -c src/common.cpp
src/client.o: src/client.cpp Makefile
	${CC} ${CCFLAGS} -o src/client.o -c src/client.cpp
src/client_wrap.o: src/client_wrap.cpp Makefile
	${CC} ${CCFLAGS} -o src/client_wrap.o -c src/client_wrap.cpp
src/server.o: src/server.cpp Makefile
	${CC} ${CCFLAGS} -o src/server.o -c src/server.cpp
src/server_wrap.o: src/server_wrap.cpp Makefile
	${CC} ${CCFLAGS} -o src/server_wrap.o -c src/server_wrap.cpp
src/net/net.o: src/net/net.cpp Makefile
	${CC} ${CCFLAGS} -o src/net/net.o -c src/net/net.cpp
