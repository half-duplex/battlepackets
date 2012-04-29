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
#define MAXGAMES 10

game_t * gamearray[MAXGAMES]; //to store the games

/*
 * Main server function
 * Arguments:
 *      int argc
 *              the number of arguments on the command line
 *      int argv
 *              the text of the arguments
 */
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

    if (setsockopt(listensocket, SOL_SOCKET, SO_REUSEADDR + SO_KEEPALIVE + TCP_NODELAY, (char*) ptr, sizeof (int)) < 0) {
        std::cout << "error creating socket" << std::endl;
    }
    free(ptr);

    if (bind(listensocket, (sockaddr*) & server, sizeof (server)) < 0) { //bind a socket to the port
        std::cout << "Bind error " << errno << ": ";
        switch (errno) {
            case EACCES:
                std::cout << "The address is protected, and the user is not the superuser.\n";
                std::cout << "Search permission is denied on a component of the path prefix.\n";
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
                std::cout << "sockfd is a descriptor for a file, not a socket.\nThe following errors are specific to Unix domain(AF_UNIX) sockets.\n";
                break;
            case EADDRNOTAVAIL:
                std::cout << "A nonexistent interface was requested or the requested address was not local.\n";
                break;
            case EFAULT:
                std::cout << "Address points outside the user's accessible address space.\n";
                break;
            case ELOOP:
                std::cout << "Too many symbolic links were encountered in resolving addr.\n";
                break;
            case ENAMETOOLONG:
                std::cout << "Address is too long.\n";
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
            std::cout << "accept failed" << std::endl;
        }

    }

    return 0;
}

game_t::game_t() {
    players[0] = NULL;
    players[1] = NULL;
}

game_t::~game_t() {
    // TODO: clean up
}

bool game_t::addplayer(player_t * player) {
    if (this->players[0] == NULL) { //this slot is empty
        players[0] = player;
        player->iszero = true;
        // is player->setgame(this); needed, or done before addplayer is called?
        return true;
    }
    if (this->players[1] == NULL) { //this slot is empty
        players[1] = player;
        player->iszero = false;
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
        //        std::cout << "socketid " << socketid << " waiting for data, player at " << player << "\n";
        int recvd = recv(socketid, data, MAXDATASIZE, 0);
        if (recvd < 0) {
            std::cout << "recv error, ret " << recvd << ", listener dying\n";
            return;
        }
        //        std::cout << "wait_data recv socket " << socketid << " data ";
        // dump all recieved data
        //        for (int i = 0; i < recvd; i++) {
        //            std::cout << (int) data[i] << data[i] << ",";
        //        }
        //        std::cout << "\n";
        if (recvd < 1) { // same as ==0
            std::cout << "recv len = 0, listener dying" << std::endl;
            // maybe better way to detect closed socket before deleting player / returning?
            delete player;
            return; // TODO: Just reset for loop IF socket not dead
        }
        switch (data[0]) { //use the first byte of data in the data array to determine what kind of packet it is
            case 0: // Handshake
                std::cout << "This is a handshake packet\n";
                handshake_t * handshake;
                handshake = new handshake_t(data, recvd);

                handshake_t * svrhand;
                svrhand = new handshake_t;

                if (strcmp(handshake->gameid, "new game") == 0) { //start a new game
                    game_t * game;
                    game = new game_t;
                    // set the gameid
                    std::cout << "Creating new game: ";
                    char printables[63] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
                    time_t secs;
                    time(&secs);
                    srand((unsigned int) secs);

                    for (int i = 0; i < 15; i++) {
                        game->gameid[i] = printables[rand() % 62];
                    }
                    std::cout << game->gameid << std::endl;

                    //                    strcpy(game->gameid, "saul"); // TODO: !!!! NEEDS TO BE A RANDOM GAME ID !!!!

                    for (int i = 0; i < 10; i++) { //find a place to store the new game
                        // TODO: replace static 10 with a #define
                        if (gamearray[i] == NULL) { //that spot is empty and we can store the game there

                            gamearray[i] = game;
                            player->game = gamearray[i];
                            break;

                        } else { //that spot already contains a game
                            i = i; // TODO: wtf is this?
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
                                player->game = gamearray[i];
                                break;
                            }


                        }

                    }
                }

                std::cout << "Sending handshake\n";
                strcpy(svrhand->username, handshake->username);
                strcpy(svrhand->gameid, player->game->gameid);
                send(player->sockfd, svrhand, sizeof (handshake_t), 0); //send the handshake back to the client

                break;
            case 1: // player move
                move_t * move;
                move = new move_t(data, recvd);

                move_t * svrmove;
                svrmove = new move_t;

                std::cout << "Move from " << player << "/" << !player->iszero << " at " << (int) move->loc.x << "," << (int) move->loc.y << " performs " << (move->action == ACT_MOVE ? "move: " : "place: ");

                //error checking->already in BOARDSIZE/BOARDSIZE because of move_t(data, len)

                //update board
                switch (move->action) {
                    case ACT_MOVE: //this is a fire
                        if (player->iszero == true) { //this is player[0]
                            if (player->game->board.get_fired(1, move->loc) == true) { //fire at the spot on the enemy's (player[1]) board and HIT
                                //update both clients telling them that it was a hit using a move pkt
                                svrmove->action = YOU_HIT; //tell the client they hit
                                svrmove->loc = move->loc;
                                send(player->sockfd, svrmove, sizeof (move_t), 0);

                                std::cout << "Hit\n";
                            } else { //it was a miss
                                //tell both clients it was a miss using a move pkt
                                svrmove->action = YOU_MISS; //tell the client they hit
                                svrmove->loc = move->loc;
                                send(player->sockfd, svrmove, sizeof (move_t), 0);

                                std::cout << "Miss\n";
                            }
                            svrmove->action = THEY_FIRED; //tell the enemy (player[1]) that they've been hit
                            send(player->game->players[1]->sockfd, svrmove, sizeof (move_t), 0);
                        }
                        if (player->iszero == false) { //this is player[1]
                            if (player->game->board.get_fired(0, move->loc) == true) { //fire at the spot on the enemy's (player[0]) board and HIT
                                //update both clients telling them that it was a hit using a move pkt
                                svrmove->action = YOU_HIT; //tell the client they hit
                                svrmove->loc = move->loc;
                                send(player->sockfd, svrmove, sizeof (move_t), 0);

                                std::cout << "Hit\n";
                            } else { //it was a miss
                                //tell both clients it was a miss using a move pkt
                                svrmove->action = YOU_MISS; //tell the client they hit
                                svrmove->loc = move->loc;
                                send(player->sockfd, svrmove, sizeof (move_t), 0);

                                std::cout << "Miss\n";

                            }
                            svrmove->action = THEY_FIRED; //tell the enemy (player[1]) that they've been hit
                            send(player->game->players[0]->sockfd, svrmove, sizeof (move_t), 0);
                        }
                        break;
                    case ACT_PLACE://this is a place
                        if (player->game->board.get_ship(0, move->loc) == false) { //there's not a ship there so it can be placed
                            player->game->board.set_ship(0, move->loc);

                            //send move to this client only
                            svrmove->action = ACT_PLACE;
                            svrmove->loc = move->loc;
                            send(player->sockfd, svrmove, sizeof (move_t), 0);

                            std::cout << "Place\n";
                        } else { //there is a ship there
                            chat_t * place_err;
                            place_err = new chat_t;
                            strcpy(place_err->msg, "You already have a ship there!\n");
                            send(player->sockfd, place_err, sizeof (chat_t), 0); //tell this client only that they can't send a ship

                            std::cout << "Placed / already\n";
                        }
                        break;
                    default:
                        std::cout << "This shouldn't be happening: move action #" << move->action << "\n";
                        break;
                }
                break;
            case 2: //board refresh
            {
                // client -> server means GIMME ONE, we don't need any checking or import, just reply
                std::cout << "Sending board refresh\n";

                refresh_t * update;
                update = new refresh_t;

                // can't just import board: needs to have enemy ship info stripped
                // and needs to be swapped if !iszero
                int m, n;
                location locb;
                for (m = 0; m < BOARDSIZE; m++) {
                    for (n = 0; n < BOARDSIZE; n++) {
                        locb.set(m, n);
                        if (player->iszero) {
                            update->board.set_tile_raw(locb, update->board.stripenemyships(player->game->board.get_tile_raw(locb)));
                        } else {
                            update->board.set_tile_raw(locb, update->board.stripenemyships(update->board.invert(player->game->board.get_tile_raw(locb))));
                        }
                    }
                }
                send(player->sockfd, update, sizeof (refresh_t), 0); //send the board update to the client
                break;
            }
            case 3: //chat
                std::cout << "This is a chat packet.\n";
                chat_t * chatmsg;
                chatmsg = new chat_t(data, recvd);

                chat_t * svrreply; //the chat reply from the server to the client(s)
                svrreply = new chat_t;

                //look up the game this is in
                if (player->game == NULL) { //the player isn't in a game yet, this shouldn't happen!

                    std::cout << "Silly client... they're not in a game.\n";
                    strcpy(svrreply->msg, "You aren't connected to a game: you can't chat yet.\n");
                    send(socketid, (void *) svrreply, sizeof (chat_t), 0); //send the error message to the client

                } else { //the client is in a game
                    if (player->game->players[0] == player) { //this is player[0]
                        //                   TODO: fix and test all this chat stuff
                        std::cout << "player[0] sent a chat\n";
                        strcpy(svrreply->msg, "Me: ");
                        strcpy(svrreply->msg + 4, chatmsg->msg);
                        send(socketid, (void *) svrreply, sizeof (chat_t), 0); //reply to the sender

                        strcpy(svrreply->msg, "Them: ");
                        strcpy(svrreply->msg + 6, chatmsg->msg);
                        send(player->game->players[1]->sockfd, (void *) svrreply, sizeof (chat_t), 0); //reply to the other player

                    } else { //this is player[1]
                        std::cout << "player[1] sent a chat\n";
                        strcpy(svrreply->msg, "Me: ");
                        strcpy(svrreply->msg + 4, chatmsg->msg);
                        send(socketid, (void *) svrreply, sizeof (chat_t), 0); //reply to the sender

                        strcpy(svrreply->msg, "Them: ");
                        strcpy(svrreply->msg + 6, chatmsg->msg);
                        send(player->game->players[0]->sockfd, (void *) svrreply, sizeof (chat_t), 0); //reply to the other player

                    }
                }
                //make sure player is in a game (its pointer to game isnt NULL)
                //if this is player[0], send to player[1] as "they said" and player[0] as "you said"
                //if this is player[1], send to player[0] as "they said" and player[1] as "you said"
                break;


            default:
                std::cout << "Invalid packet received.\n";
                break;
        }
        // do stuff with the data ( see client.cpp wait_data )
    }
}
