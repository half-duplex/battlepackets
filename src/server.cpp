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

game_t * gamearray[10]; //to store the games

int main_server(int argc, char** argv) {
#ifdef DEBUG
    if (argc != 2) {
        std::cout << "Arguments:\n";
        for (int i = 0; i < argc; i++) {
            std::cout << argv[i] << std::endl;
        }
    }
#endif



    for (int i = 0; i < 10; i++) { //set pointers from gamearray to null
        gamearray[i] = NULL;
        std::cout << "gamearray[" << i << "] is " << gamearray[i] << std::endl;
    }

    int sockfd;
    player_t * player;


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

game_t::game_t() {
    players[1] = NULL;
    players[2] = NULL;
}

game_t::~game_t() {

}

bool game_t::addplayer(player_t * player) {
    if (this->players[1] == 0) { //this slot is empty 
        players[1] = player;
        return 1;
    }
    if (this->players[2] == 0) { //this slot is empty
        players[2] = player;
        return 1;
    }

    return 0; //if failed
}

player_t::player_t(int new_sockfd) {
    sockfd = new_sockfd;
    boost::thread waiter(wait_data, this);
}

player_t::~player_t() {

}

int player_t::get_sockid() {
    return sockfd;
}

void wait_data(player_t * player) {

    int socketid = player->get_sockid();
    char data[MAXDATASIZE];

    for (;;) {
        // wait for data on sockfd

        //        if(connection is dead){
        //            delete player;
        //        }

        int recvd = recv(socketid, data, MAXDATASIZE, 0);
        if (recvd < 0) {
            std::cout << "server error recieving data: recv ret " << recvd << std::endl;
        } else { //handle the data
            std::cout << "wait_data recv socket " << socketid << " data ";
            for (int i = 0; i < recvd; i++) {
                //                std::cout << (int) data[i] << data[i] << ",";
            }
            std::cout << "\n";
            if (recvd < 1) {
                std::cout << "Tripping return because of data length" << std::endl;
                return;
            }
            switch (data[0]) { //use the first byte of data in the data array to determine what kind of packet it is
                case 0: // Handshake
                    std::cout << "This is a handshake packet" << std::endl;
                    handshake_t * handshake;
                    handshake = new handshake_t(data, recvd);

                    if (strcmp(handshake->gameid, "new game") == 0) { //start a new game                     
                        game_t * game;
                        game = new game_t;
                        /*set the gameid*/
                        std::cout << "Trying to start a new game" << std::endl;
                        strcpy(game->gameid, "saul"); //!!!!SAUL NEEDS TO BE A RANDOM GAME ID!!!!

                        for (int i = 0; i < 10; i++) { //find a place to store the new game
                            if (gamearray[i] == NULL) { //that spot is empty and we can store the game there
                                std::cout << "found a spot!" << std::endl;
                                //                                gamearray[i]->addplayer(player, gamearray[i]);
                                gamearray[i] = game;
                                break;
                            } else { //that spot already contains a game
                                i = i;
                            }
                        }

                    } else { //join an existing game
                        std::cout << handshake->username << " is joining a game" << std::endl;
                        for (int i = 0; i < 10; i++) { //find the game
                            if (gamearray[i] != NULL) {
                                std::cout << "not null at gamearray " << i << std::endl;
                                if (strcmp(handshake->gameid, gamearray[i]->gameid) == 0) {
                                    std::cout << "found a game that matches" << std::endl;
                                    gamearray[i]->addplayer(player); //add player to game
                                    std::cout << "Found a game with gameid " << handshake->gameid << " and assigned player "
                                            << handshake->username << " to it" << std::endl;
                                    break;
                                }


                            }

                        }
                    }
                    // display game ID
                    break;
                default:
                    std::cout << "server Invalid packet recieved.\n";
                    break;
            }
        }
        // do stuff with the data ( see client.cpp wait_data )
    }
}
