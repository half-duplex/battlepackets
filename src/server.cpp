/**
 * server.cpp
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

#include <iostream>
#include <boost/thread.hpp>
#include "server.h"
#include "common.h"
#include "sys/socket.h"
#include <sys/types.h>
#include <netinet/in.h> //for INADDR_ANY

#define DEBUG


/*
 * Main server function
 * Arguments:
 *      int argc
 *              the number of arguments on the command line
 *      int argv
 *              the text of the arguments
 */

void serverhandler(int socket) {

    while(1) {
        //wait for data, then when it gets some process it
    }
}

int main_server(int argc, char** argv) {
#ifdef DEBUG
    if (argc != 2) {
        std::cout << "Arguments:\n";
        for (int i = 0; i < argc; i++) {
            std::cout << argv[i] << std::endl;
        }
    }
#endif



    player_t * player;
    int sockfd;

    int listensocket; // socket to be used to wait for connections from the client
    //    struct addrinfo host, *server;
    struct sockaddr_in server;
    int *ptr;
    socklen_t len = 0;
    //    pthread_t tid = 0;
    sockaddr_in svradr;
    //   socklen_t socksize = sizeof(struct sockaddr_in); //to be used in function call later
    //    pid_t child;



    /*set the server info*/
    //    memset(&host, 0, sizeof (host));
    server.sin_family = AF_INET;
    server.sin_port = htons(7777);
    memset(&(server.sin_zero), 0, 8);
    server.sin_addr.s_addr = INADDR_ANY; //get the IP of the host


    //    int status = getaddrinfo(NULL, "7777", &host, &server); //the 7777 needs to use "port" instead
    //    if (status < 0) {
    //        cout << "error getting info" << endl;
    //    }


    listensocket = socket(AF_INET, SOCK_STREAM, 0); //create a TCP socket on the server
    if (listensocket < 0) {
        std::cout << "error creating socket" << std::endl;
    }

    ptr = (int*) malloc(sizeof (int));
    *ptr = 1;

    if (setsockopt(listensocket, SOL_SOCKET, SO_KEEPALIVE, (char*) ptr, sizeof (int)) < 0) {
        std::cout << "error creating socket" << std::endl;
    }
    free(ptr);

    if (bind(listensocket, (sockaddr*) & server, sizeof (server)) < 0) { //bind a socket to the port
        std::cout << "error binding" << std::endl;
    }


    if (listen(listensocket, 50) < 0) {
        std::cout << "error listening" << std::endl;

    }

    len = sizeof (sockaddr_in);
    // make socket

    while (true) {
        std::cout << "server ready and waiting!" << std::endl;
//        sockfd = (int *) malloc(sizeof (int)); ->> I dont think this is needed
        if ((sockfd = accept(listensocket, (sockaddr*) & svradr, &len)) != -1) {
            std::cout << "got one!" << std::endl;
            player = new player_t(sockfd); //doing this will then create a thread in player_t
        } else {
            std::cout << "we're not very accepting here" << std::endl;
        }

    }
    for (;;) {
        // when a connection is made,
        // accept


        //        player = new player_t(sockfd);
    }

    return 0;
}

player_t::player_t(int new_sockfd) {
    sockfd = new_sockfd;
    boost::thread waiter(wait_data, this);
}

player_t::~player_t() {

}

void wait_data(player_t * player) {
    for (;;) {
        // wait for data on sockfd

        //        if(connection is dead){
        //            delete player;
        //        }

        // do stuff with the data ( see client.cpp wait_data )
    }
}

void player_t::send_data(void * data, int datalen) {
    // send on sockfd
}
