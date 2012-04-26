CC = g++
CCFLAGS = -Wall -g -std=c++0x -lboost_thread `pkg-config gtkmm-2.4 --cflags --libs`

all: client server
clean:
	rm -rf *.core src/*/*.o src/*.o bin/
commonstuff:
	mkdir -p bin/object/net/

client: client_wrap.o client.o common.o Makefile commonstuff
	${CC} ${CCFLAGS} -o bin/client bin/object/client_wrap.o bin/object/client.o bin/object/common.o
server: server_wrap.o server.o common.o Makefile commonstuff
	${CC} ${CCFLAGS} -o bin/server bin/object/server_wrap.o bin/object/server.o bin/object/common.o
check: check.o client.o server.o common.o Makefile commonstuff
	${CC} ${CCFLAGS} -o bin/check bin/object/check.o bin/object/client.o bin/object/server.o bin/object/common.o
	bin/check

check.o: Makefile commonstuff
	${CC} ${CCFLAGS} -o bin/object/check.o -c src/check.cpp
common.o: Makefile commonstuff
	${CC} ${CCFLAGS} -o bin/object/common.o -c src/common.cpp
client.o: src/client.cpp Makefile commonstuff
	${CC} ${CCFLAGS} -o bin/object/client.o -c src/client.cpp
client_wrap.o: src/client_wrap.cpp Makefile commonstuff
	${CC} ${CCFLAGS} -o bin/object/client_wrap.o -c src/client_wrap.cpp
server.o: src/server.cpp Makefile commonstuff
	${CC} ${CCFLAGS} -o bin/object/server.o -c src/server.cpp
server_wrap.o: src/server_wrap.cpp Makefile commonstuff
	${CC} ${CCFLAGS} -o bin/object/server_wrap.o -c src/server_wrap.cpp
