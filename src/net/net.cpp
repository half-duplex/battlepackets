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
#include <unistd.h> // for fork
#endif

#define DEBUG
#ifdef DEBUG
#include <iostream>
using namespace std;
#endif


// common stuff
enum pktType {
    AUTH,
    MOVE,
    UPDATE1,
    UPDATE2
};

struct pktHeader {
    short type;
    unsigned long clientID;
    unsigned long gameID;
    unsigned long headLength; //why 100?
};

struct packet {
    pktHeader header; //14 bytes
    char data[100]; //100 bytes
};

union datapacket {
    packet pkt;
    char pktArr[114];
};

 
  
// iostream is not allowed. no cout or cin here.

bool netsend(int sockfd, char * data[], int datalen) {
     datapacket dpkt;
     dpkt.pkt.header.headLength = 100;
  
    send(sockfd, data, sizeof(pktHeader) + dpkt.pkt.header.headLength, 0);
}



void netrecv(int sockfd, char * data[], int datalen) {
  
     datapacket dpkt;
   
  
    recv(sockfd, data, datalen, 0); //this 0 may have to change 
    
    if (dpkt.pkt.header.type == AUTH) { //pull out UID, GID and see if they match up
        
        
    }

}
int netconnect(char * addr[], int addrlen, int port){ //create a socket for the client to talk to the server 
    
    int clientsocket;
    struct sockaddr_in server; //will contain information about the server the client will connect to
    
    clientsocket = socket(AF_INET, SOCK_STREAM, 0); //creates a socket for the client 
    
    memset(&server, 0, sizeof(server)); //to ensure theres no garbage data in the struct
    server.sin_family = AF_INET; 
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); //Local loopback for now, will need to be set to 
                                                     //server's address using the agrs in the function 
    server.sin_port = htons(port); 
    
    connect(clientsocket, (struct sockaddr *)&server, sizeof(struct sockaddr));
    
    

    return clientsocket;
    
}

int netlisten(int port) {
    
     datapacket dpkt;
     dpkt.pkt.header.headLength = 100;
   
    int serversocket; // socket to be used to wait for connections from the client
    struct sockaddr_in server; //for server info
    struct sockaddr_in client; //for client info
    socklen_t socksize = sizeof(struct sockaddr_in); //to be used in function call later
    pid_t child;
    
    /*set the server info*/
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port); 
    
    /*have to add error checking for the functions below*/
    serversocket = socket(AF_INET, SOCK_STREAM, 0); //create a TCP socket on the server
    
    bind(serversocket, (struct sockaddr *)& server, sizeof(struct sockaddr)); //bind a socket to the 
    
    listen(serversocket, 50); //50 = number of allowed connections 
    
    
    
    while(1) { //wait for auth info to be sent from the client
        
        int authSocket = accept(serversocket, (struct sockaddr *)&client, &socksize); //blocked until data is sent
        if ((child = fork()) == 0) {
            close(serversocket);
            //handle the connection (authSocket) with 
            netrecv(authSocket, dpkt.pkt.data, dpkt.pkt.header.headLength);
            close(authSocket);
            exit(0);
        }
        close(serversocket);
         
   }
    
    
    
    
    
}



