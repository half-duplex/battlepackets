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
    }

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

    if (setsockopt(listensocket, SOL_SOCKET, SO_REUSEADDR + SO_KEEPALIVE, (char*) ptr, sizeof (int)) < 0) {
        std::cout << "error creating socket" << std::endl;
    }
    free(ptr);

    if (bind(listensocket, (sockaddr*) & server, sizeof (server)) < 0) { //bind a socket to the port
        std::cout << "error binding: " << errno << std::endl;
        switch (errno) {
            case EACCES:
                std::cout << "The address is protected, and the user is not the superuser.\n";
                std::cout << "Search permission is denied on a component of the path prefix. (See also path_resolution(7).)\n";
                break;
            case EADDRINUSE:
                std::cout << "The given address is already in use.\n";
                break;
            case EBADF:
                std::cout << "sockfd is not a valid descriptor.\n";
                break;
            case EINVAL:
                std::cout << "The socket is already bound to an address.\n";
                std::cout << "The addrlen is wrong, or the socket was not in the AF_UNIX family.\n";
                break;
            case ENOTSOCK:
                std::cout << "sockfd is a descriptor for a file, not a socket.\nThe following errors are specific to Unix domain(AF_UNIX) sockets:\n";
                break;
            case EADDRNOTAVAIL:
                std::cout << "A nonexistent interface was requested or the requested address was not local.\n";
                break;
            case EFAULT:
                std::cout << "addr points outside the user's accessible address space.\n";
                break;
            case ELOOP:
                std::cout << "Too many symbolic links were encountered in resolving addr.\n";
                break;
            case ENAMETOOLONG:
                std::cout << "addr is too long.\n";
                break;
            case ENOENT:
                std::cout << "The file does not exist.\n";
                break;
            case ENOMEM:
                std::cout << "Insufficient kernel memory was available.\n";
                break;
            case ENOTDIR:
                std::cout << "A component of the path prefix is not a directory.\n";
                break;
            case EROFS:
                std::cout << "The socket inode would reside on a read - only file system.\n";
                break;
        }
        return 1; // nothing else we can do, aside from maybe have a wait-retry loop
    }

    if (listen(listensocket, 50) < 0) {
        std::cout << "error listening" << std::endl;

    }

    len = sizeof (sockaddr_in);
    // make socket

    int sockfd;
    while (true) {
        std::cout << "Waiting for connection" << std::endl;
        if ((sockfd = accept(listensocket, (sockaddr*) & svradr, &len)) != -1) {
            player_t * player;
            std::cout << "New connection, fd: " << sockfd << '\n';
            player = new player_t(sockfd); //doing this will then create a thread in player_t
            std::cout << "main: new player: " << player << "\n";
        } else {
            std::cout << "we're not very accepting here" << std::endl;
        }

    }

    return 0;
}

game_t::game_t() {
    players[0] = NULL;
    players[1] = NULL;
}

game_t::~game_t() {

}

bool game_t::addplayer(player_t * player) {
    if (this->players[0] == NULL) { //this slot is empty
        players[0] = player;
        // is player->setgame(this); needed, or done before addplayer is called?
        return true;
    }
    if (this->players[1] == NULL) { //this slot is empty
        players[1] = player;
        // player->setgame(this); may be needed
        return true;
    }
    return false; // failed: players already exist
}

player_t::player_t(int new_sockfd) {
    std::cout << "New player, fd: " << new_sockfd << " player: " << this << "\n";
    sockfd = new_sockfd;
    boost::thread waiter(wait_data, this);
}

player_t::~player_t() {

}

int player_t::get_sockid() {
    return sockfd;
}

void wait_data(player_t * player) {
    std::cout << "New wait_data, fd: " << player->get_sockid() << " player: " << player << "\n";

    int socketid = player->get_sockid();
    char data[MAXDATASIZE];

    for (;;) {
        // wait for data on sockfd

        //        if(connection is dead){
        //            delete player;
        //        }
        std::cout << "socketid " << socketid << " waiting for data, player at " << player << "\n";
        int recvd = recv(socketid, data, MAXDATASIZE, 0);
        if (recvd < 0) {
            std::cout << "recv error, ret " << recvd << ", listener dying\n";
            return;
        }
        std::cout << "wait_data recv socket " << socketid << " data ";
        // dump all recieved data
        for (int i = 0; i < recvd; i++) {
            std::cout << (int) data[i] << data[i] << ",";
        }
        std::cout << "\n";
        if (recvd < 1) { // same as ==0
            std::cout << "recv len = 0, listener dying, probably shouldn't" << std::endl;
            return; // TODO: Just reset for loop
        }
        switch (data[0]) { //use the first byte of data in the data array to determine what kind of packet it is
            case 0: // Handshake
                std::cout << "This is a handshake packet" << std::endl;
                handshake_t * handshake;
                handshake = new handshake_t(data, recvd);

                if (strcmp(handshake->gameid, "new game") == 0) { //start a new game
                    game_t * game;
                    game = new game_t;
                    // set the gameid
                    std::cout << "Trying to start a new game" << std::endl;

                    strcpy(game->gameid, "saul"); // TODO: !!!! NEEDS TO BE A RANDOM GAME ID !!!!

                    for (int i = 0; i < 10; i++) { //find a place to store the new game
                        // TODO: replace static 10 with a #define
                        if (gamearray[i] == NULL) { //that spot is empty and we can store the game there
                            std::cout << "found a spot!" << std::endl;
                            //                                gamearray[i]->addplayer(player, gamearray[i]);
                            gamearray[i] = game;
                            break;
                        } else { //that spot already contains a game
                            i = i;
                        } // if gamearray[i]==null
                    } // for each in gamearray
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

                // TODO: Send game board back to client
                break;
            case 1: // player move
                std::cout << "This is a move packet: ";
                move_t * move;
                move = new move_t(data, recvd);

                std::cout << move->loc.x << "," << move->loc.y << " performs " << (move->action == move->ACT_MOVE ? "move\n" : "place\n");
                break;
            default:
                std::cout << "Invalid packet received.\n";
                break;
        }
        // do stuff with the data ( see client.cpp wait_data )
    }
}
