CC = g++
CCFLAGS = -Wall -g -std=c++0x

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

client: src/client.o src/net/net.o src/frontend/cli.o src/game/game.o Makefile
	${CC} ${CCFLAGS} -o client src/client.o src/frontend/cli.o src/net/net.o src/game/game.o
cleanclient:
	rm -f client src/client.o

cleanlibs:
	rm -f src/*/*.o
src/net/net.o: src/net/net.cpp Makefile
	${CC} ${CCFLAGS} -o src/net/net.o -c src/net/net.cpp
src/game/game.o: src/game/game.cpp Makefile
	${CC} ${CCFLAGS} -o src/game/game.o -c src/game/game.cpp
src/frontend/cli.o: src/frontend/cli.cpp Makefile
	${CC} ${CCFLAGS} -o src/frontend/cli.o -c src/frontend/cli.cpp
src/frontend/gtk.o: src/frontend/gtk.cpp Makefile
	${CC} ${CCFLAGS} -o src/frontend/gtk.o -c src/frontend/gtk.cpp
