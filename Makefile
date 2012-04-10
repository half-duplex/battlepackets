CC = g++
CCFLAGS = -Wall -g -std=c++0x -lboost_thread `pkg-config gtkmm-2.4 --cflags --libs`

all: client server
clean:
	rm -f *.core src/*/*.o src/*.o bin/client bin/server bin/object/*.o bin/object/*/*.o

client: bin/object/client_wrap.o bin/object/client.o bin/object/net/net.o bin/object/common.o Makefile
	${CC} ${CCFLAGS} -o bin/client bin/object/client_wrap.o bin/object/client.o bin/object/net/net.o bin/object/common.o
server: bin/object/server_wrap.o bin/object/server.o bin/object/net/net.o bin/object/common.o Makefile
	${CC} ${CCFLAGS} -o bin/server bin/object/server_wrap.o bin/object/server.o bin/object/net/net.o bin/object/common.o
check: bin/object/check.o bin/object/client.o bin/object/server.o bin/object/net/net.o bin/object/common.o Makefile
	${CC} ${CCFLAGS} -o bin/check bin/object/check.o bin/object/client.o bin/object/server.o bin/object/net/net.o bin/object/common.o
	bin/check

bin/object/check.o: Makefile
	${CC} ${CCFLAGS} -o bin/object/check.o -c src/check.cpp
bin/object/common.o: src/net/net.cpp Makefile
	${CC} ${CCFLAGS} -o bin/object/common.o -c src/common.cpp
bin/object/client.o: src/client.cpp Makefile
	${CC} ${CCFLAGS} -o bin/object/client.o -c src/client.cpp
bin/object/client_wrap.o: src/client_wrap.cpp Makefile
	${CC} ${CCFLAGS} -o bin/object/client_wrap.o -c src/client_wrap.cpp
bin/object/server.o: src/server.cpp Makefile
	${CC} ${CCFLAGS} -o bin/object/server.o -c src/server.cpp
bin/object/server_wrap.o: src/server_wrap.cpp Makefile
	${CC} ${CCFLAGS} -o bin/object/server_wrap.o -c src/server_wrap.cpp
bin/object/net/net.o: src/net/net.cpp Makefile
	${CC} ${CCFLAGS} -o bin/object/net/net.o -c src/net/net.cpp
