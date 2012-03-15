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

// EXAMPLE OF HOW TO DECIDE IF IT'S WINDOWS OR NOT:
// DO NOT EDIT stuff that is not your OS, because you won't be able to test it!
#if defined(__WIN32__) || defined(_WIN32)
// windows stuff TODO
#else
// linux and mac stuff
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> //for memset
#include <cstdlib> //for exit 
#endif
// common stuff
// iostream is not allowed. no cout or cin here.
#define MAXCONNECTIONS 10

    int socket; //The socket (file descriptor)
    int createsocket; //The socket to be created when there's a new connection 
    socklen_t sinsize;
    int getaddrinfo(const char *node, //The IP of the server
        const char *service, //The port to run on
        const struct addrinfo *server, //Instance to contain server's information
            struct addrinfo **res); //What is returned
    
    int status;
    
    struct addrinfo server;
    struct addrinfo *serverinfo; //Pointer to server's information
    struct addrinfo *p; //Port
    struct sockaddr_storage clientinfo; //The client's information 

int netconnect(char * addr[], int addrlen, int port) {
    
    
    
    memset(&server, 0, sizeof server); //To clear the memory of server
    server.ai_family = AF_INET; //AF_UNSPEC if we want to try for IPv4/6
    server.ai_socktype = SOCK_STREAM; //TCP
    server.ai_flags = AI_PASSIVE; // fills the IP automatically
    
    if ((status = getaddrinfo(NULL, "7777", &server, &serverinfo)) != 0) { //Gets the server's address information
        //printf("There was an error setting up the server");
        exit(-1);
    }
    
    for (p = serverinfo; p != NULL; p->ai_next) { //Keep going until it's able to bind to a port
        if ((socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) != 0) { //Creates a socket
            //printf("Socket creation error");
            continue; //Because if we exit, what's the point of the for loop?
        }
        
        if (bind(socket, p->ai_addr, p->ai_addrlen) != 0) {
            //printf("Bind error");
            close(socket); //So that we can try again 
            continue; //Ditto
        }
        
        break; //If we are successful 
            
    }
    
    freeaddrinfo(serverinfo); //Free the information about the server, because we already have our connection
    
}

int netlisten(int port) {
    if (listen(socket, MAXCONNECTIONS) != 0) {
        //printf("Listen error");
        exit(-1);
    }
    
    while(1) {
        sinsize = sizeof clientinfo;
        
        
        
    }
}

bool netsend(int sockfd, char * data[], int datalen) {
    ;
}

void netrecv(void (*handler)(int sockfd, char * data[], int datalen)) {
    ;
}
