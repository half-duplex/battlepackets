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

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> //for memset
#include <cstdlib> //for exit
#include "iostream"
#include "../server.h"
#include <boost/thread.hpp>





// common stuff


// iostream is not allowed. no cout or cin here.

bool netsend(int sockfd, char * data, int datalen) {


    send(sockfd, data, datalen, 0);
}

void netrecv(int sockfd, char * data, int datalen) {



    recv(sockfd, data, datalen, 0); //this 0 may have to change


}

/* this belongs in net because its common
 need to add a function (callback) as an argument
 then depending on whether or not its the client
 or the server requesting to use it they will
 pass it a different function to use*/

void handleconnection(int socket, void (*handler)(int socket)) {

    //        int *clientsocket = (int*)socket;
    //
    //        char buffer[1024]; //may need to adjust
    //        char buffer_len = 1024;
    //        int bytes;
    //
    //        memset(buffer, 0, buffer_len);
    //        if ((bytes = recv(*clientsocket, buffer, buffer_len, 0)) < 0) {
    //            std::cout << "recv error in handleclient" << std::endl;
    //        }
    //
    //        strcat(buffer, " Server ECHO");
    //
    //
    //        free(clientsocket);
}


//int netconnect(char * addr, int addrlen, int port) { //create a socket for the client to talk to the server 

void* netconnect(void*) {
    char* server = "127.0.0.1"; //need to change
    struct sockaddr_in client;

    //    char buffer[1024];
    //    int buffer_len = 0;
    //    int bytes;
    int port = 7777;
    int clientsocket;
    int * ptr;

    clientsocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientsocket < 0) {
        std::cout << "client socket creation error" << std::endl;
    }

    ptr = (int*) malloc(sizeof (int));
    *ptr = 1;

    if (setsockopt(clientsocket, SOL_SOCKET, SO_KEEPALIVE, (char*) ptr, sizeof (int)) < 0) {
        std::cout << "client set options error" << std::endl;
    }

    free(ptr);

    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    memset(&(client.sin_zero), 0, 8);
    client.sin_addr.s_addr = inet_addr(server);

    if (connect(clientsocket, (struct sockaddr*) &client, sizeof (client)) < 0) {
        std::cout << "error connecting socket" << std::endl;
    }

    boost::thread clientthread(handleconnection, *clientsocket, clienthandler); //in this case, clientsocket is the socket ON THE CLIENT that data
                                                                                //will be sent to from the server 


}

int netlisten(int port) {

    int serversocket; // socket to be used to wait for connections from the client
    //    struct addrinfo host, *server;
    struct sockaddr_in server;
    int *ptr;
    socklen_t len = 0;
    //    pthread_t tid = 0;
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
        std::cout << "error creating socket" << std::endl;
    }

    ptr = (int*) malloc(sizeof (int));
    *ptr = 1;

    if (setsockopt(serversocket, SOL_SOCKET, SO_KEEPALIVE, (char*) ptr, sizeof (int)) < 0) {
        std::cout << "error creating socket" << std::endl;
    }
    free(ptr);

    if (bind(serversocket, (sockaddr*) & server, sizeof (server)) < 0) { //bind a socket to the port
        std::cout << "error binding" << std::endl;
    }


    if (listen(serversocket, 50) < 0) {
        std::cout << "error listening" << std::endl;

    }

    len = sizeof (sockaddr_in);

    while (true) {
        std::cout << "server ready and waiting!" << std::endl;
        clientsocket = (int*) malloc(sizeof (int));
        if ((*clientsocket = accept(serversocket, (sockaddr*) & svradr, &len)) != -1) { //in this case, clientsocket is the client socket ON THE SERVER
            //serversocket is the listening socket on the server
            std::cout << "got one!" << std::endl;

            //            pthread_create(&tid, 0, &handleclient, (void*)clientsocket);
            //            pthread_detach(tid);
            boost::thread serverthread(handleconnection, *clientsocket, serverhandler); //the handler in this case will be a function that sits on the 
            //server and loops as it waits for data... i think
        } else {
            std::cout << "we're not very accepting here" << std::endl;
        }

    }


}




