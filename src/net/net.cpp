/**
 * net.cpp
 * Battlepackets
 * Norwich University IS460 Spring 2012
 * Battlepackets Group - battlepackets@googlegroups.com

    Copyright 2012 Battlepackets Group

    This file is part of Battlepackets.

    Battlepackets is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Battlepackets is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Battlepackets.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "net.h"
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> //for memset
#include <cstdlib> //for exit
#include <unistd.h> // for fork
#include <pthread.h>
#include <iostream> // for cout (debugging)
using namespace std;

bool netsend(int sockfd, char * data[], int datalen) {
    send(sockfd, data, datalen, 0);
    return true;
}

int netconnect(char * addr, int addrlen, int port, void (*callback)(int sockfd, char * data, int datalen)) { //create a socket for the client to talk to the server
    char* server = "127.0.0.1"; //need to change // TODO: need to make nonstatic: should be static in the client
    struct sockaddr_in client;

    char buffer[1024];
    int buffer_len = 0;
    int bytes;
    int clientsocket;
    int * ptr;

    clientsocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientsocket < 0) {
        cout << "client socket creation error" << endl;
    }

    ptr = (int*) malloc(sizeof (int));
    *ptr = 1;

    if (setsockopt(clientsocket, SOL_SOCKET, SO_KEEPALIVE, (char*) ptr, sizeof (int)) < 0) {
        cout << "client set options error" << endl;
    }

    free(ptr);

    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    memset(&(client.sin_zero), 0, 8);
    client.sin_addr.s_addr = inet_addr(server);

    if (connect(clientsocket, (struct sockaddr*) &client, sizeof (client)) < 0) {
        cout << "error connecting socket" << endl;
    }



}

void* handleclient(void* socket) {

    int *clientsocket = (int*) socket;

    char buffer[1024]; //may need to adjust
    char buffer_len = 1024;
    int bytes;

    memset(buffer, 0, buffer_len);
    if ((bytes = recv(*clientsocket, buffer, buffer_len, 0)) < 0) {
        cout << "recv error in handleclient" << endl;
    }

    strcat(buffer, " Server ECHO");


    free(clientsocket);
}

int netlisten(int port) {


    int serversocket; // socket to be used to wait for connections from the client
    //    struct addrinfo host, *server;
    struct sockaddr_in server;
    int *ptr;
    socklen_t len = 0;
    pthread_t tid = 0;
    sockaddr_in svradr;
    int* clientsocket;

    //   socklen_t socksize = sizeof(struct sockaddr_in); //to be used in function call later
    //    pid_t child;



    /*set the server info*/
    //    memset(&host, 0, sizeof (host));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    memset(&(server.sin_zero), 0, 8);
    server.sin_addr.s_addr = INADDR_ANY; //get the IP of the host


    //    int status = getaddrinfo(NULL, "7777", &host, &server); //the 7777 needs to use "port" instead
    //    if (status < 0) {
    //        cout << "error getting info" << endl;
    //    }


    serversocket = socket(AF_INET, SOCK_STREAM, 0); //create a TCP socket on the server
    if (serversocket < 0) {
        cout << "error creating socket" << endl;
    }

    ptr = (int*) malloc(sizeof (int));
    *ptr = 1;

    if (setsockopt(serversocket, SOL_SOCKET, SO_KEEPALIVE, (char*) ptr, sizeof (int)) < 0) {
        cout << "error creating socket" << endl;
    }
    free(ptr);

    if (bind(serversocket, (sockaddr*) & server, sizeof (server)) < 0) { //bind a socket to the port
        cout << "error binding" << endl;
    }




    if (listen(serversocket, 50) < 0) {
        cout << "error listening" << endl;

    }

    len = sizeof (sockaddr_in);

    while (true) {
        cout << "server ready and waiting!" << endl;
        clientsocket = (int*) malloc(sizeof (int));
        if ((*clientsocket = accept(serversocket, (sockaddr*) & svradr, &len)) != -1) {
            cout << "got one!" << endl;

            pthread_create(&tid, 0, &handleclient, (void*) clientsocket);
            pthread_detach(tid);
        } else {
            cout << "we're not very accepting here" << endl;
        }

    }


}

// Packets

handshake_t::handshake_t() {
}

handshake_t::handshake_t(char * data, int datalen) {
    if (datalen != sizeof (handshake_t)) return;
    memcpy((void*) data, (void*) this, sizeof (handshake_t));
    if (protover != PROTOVERSION) {
        cout << "Old protocol version!\n";
        return;
    }
    if (boardsize != BOARDSIZE) {
        cout << "Different board size!\n";
        return;
    }
    username[19 - 3] = '\0';
    // gameid is restricted to printable characters less ' '
    gameid[52 - 20] = '\0';
}

move_t::move_t(){
}

move_t::move_t(char* data, int datalen){
    if (datalen != sizeof (move_t)) return;
    memcpy((void*) data, (void*) this, sizeof (move_t));
    if (loc.x>=BOARDSIZE||loc.y>=BOARDSIZE){
        cout << "Invalid coordinates\n";
    }
    // action needs checking
}
