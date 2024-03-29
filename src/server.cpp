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
        parse_conn_err(errno);
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
    players[0] = players[1] = NULL;
    playernames[0][0] = playernames[1][0] = '\0';
    modes[0] = modes[1] = GM_SHIP1;
}

game_t::~game_t() {
    // TODO: clean up
}

bool game_t::add_player(player_t * player) {
    if (strcmp(playernames[0], player->username) == 0 && players[0] == NULL) { // returning to p0
        std::cout << "Reassigning " << player->username << " to " << this << " as 0/" << playernames[0] << "\n";
        players[0] = player;
        player->playernum = 0;
        player->set_game(this);
        return true;
    }
    if (strcmp(playernames[1], player->username) == 0 && players[1] == NULL) { // returning to p1
        std::cout << "Reassigning " << player->username << " to " << this << " as 1/" << playernames[1] << "\n";
        players[1] = player;
        player->playernum = 1;
        player->set_game(this);
        return true;
    }
    if (players[0] == NULL && playernames[0][0] == '\0') { // this slot is empty and unreserved
        std::cout << "Assigning " << player->username << " to " << this << " as 0\n";
        players[0] = player;
        player->playernum = 0;
        player->set_game(this);
        strncpy(playernames[0], player->username, 15);
        return true;
    }
    if (players[1] == NULL && playernames[1][0] == '\0') { // that slot is empty and unreserved
        std::cout << "Assigning " << player->username << " to " << this << " as 1\n";
        players[1] = player;
        player->playernum = 1;
        player->set_game(this);
        strncpy(playernames[1], player->username, 15);
        return true;
    }
    return false; // failed: players already exist
}

player_t * game_t::get_player(uint8_t playernum) {
    if (playernum > 1) {
        std::cout << "Trying to get to a DNE player\n";
        return NULL;
    }
    return players[playernum];
}

void game_t::del_player(uint8_t playernum) {
    if (playernum > 1) {
        std::cout << "Trying to get to a DNE player\n";
        return;
    }
    players[playernum] = NULL;
}

player_t::player_t(int new_sockfd) {
    std::cout << "New player, fd: " << new_sockfd << " player: " << this << "\n";
    sockfd = new_sockfd;
    boost::thread waiter(wait_data, this);
}

void player_t::send_message(char * message) {
    chat_t * chat;
    chat = new chat_t;
    strcpy(chat->msg, message);
    std::cout << "Sending \"" << chat->msg << "\" to " << (int) playernum << '\n';
    send(sockfd, (const void *) chat, sizeof (chat_t), SENDFLAGS);
}

void player_t::send_message_s(std::string message) {
    chat_t * chat;
    chat = new chat_t;
    message.copy(chat->msg, 254, 0);
    std::cout << "Sending \"" << chat->msg << "\" to " << (int) playernum << '\n';
    send(sockfd, (const void *) chat, sizeof (chat_t), SENDFLAGS);
}

int player_t::get_sockid() {

    return sockfd;
}

player_t * player_t::otherplayer() {
    if (game == NULL) return NULL;
    // don't need to check for null other player: null other player will return null herp derp
    return game->get_player(!playernum);
}

bool player_t::set_game(game_t * new_game) {
    game = new_game;
    return true;
}

game_t * player_t::get_game() {
    return game;
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
            player->get_game()->del_player(player->playernum);
            delete player;
            return; // TODO: Just reset for loop IF socket not dead
        }
        switch (data[0]) { //use the first byte of data in the data array to determine what kind of packet it is
            case 0: // Handshake
                std::cout << "handshake: ";

                handshake_t * handshake; // received
                handshake = new handshake_t(data, recvd);

                handshake_t * svrhand; // response
                svrhand = new handshake_t;

                strcpy(player->username, handshake->username);

                if (strcmp(handshake->gameid, "new game") == 0) { //start a new game
                    for (int i = 0; i < MAXGAMES; i++) { //find a place to store the new game
                        if (gamearray[i] == NULL) { //that spot is empty and we can store the game there
                            gamearray[i] = new game_t;

                            // set the gameid
                            std::cout << "new game: ";
                            char printables[63] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
                            time_t secs;
                            time(&secs);
                            srand((unsigned int) secs);
                            for (int j = 0; j < 5; j++) { // can be up to <15
                                gamearray[i]->gameid[j] = printables[rand() % 62];
                            }

                            std::cout << gamearray[i]->gameid << std::endl;
                            if (!gamearray[i]->add_player(player)) {
                                char msg[] = "That game is full! Try again in a few minutes.";
                                player->send_message(msg);
                            }
                            break;
                        } // if gamearray[i]==null
                    } // for each in gamearray
                } else { //join an existing game
                    std::cout << handshake->username << " joins " << handshake->gameid << std::endl;
                    for (int i = 0; i < MAXGAMES; i++) { // for each game
                        // skip empties
                        if (gamearray[i] != NULL) {
                            if (strcmp(handshake->gameid, gamearray[i]->gameid) == 0) {
                                std::cout << "handshake: found game, trying to assign " << handshake->username << "\n";
                                gamearray[i]->add_player(player); //add player to game
                                player->set_game(gamearray[i]);
                                break;
                            }
                        }
                    }
                }
                if (player->get_game() == NULL) {
                    char msg[] = "Couldn't find that game!";
                    player->send_message(msg);
                    // TODO: Disconnect player
                    break;
                }

                std::cout << "Sending handshake response\n";
                strcpy(svrhand->username, handshake->username);
                strcpy(svrhand->gameid, player->get_game()->gameid);
                send(player->sockfd, svrhand, sizeof (handshake_t), SENDFLAGS); //send the handshake back to the client

                if (player->otherplayer() != NULL) {
                    char msg[64] = "";
                    strcat(msg, player->username);
                    strcat(msg, " has joined the game");
                    player->otherplayer()->send_message(msg);
                }

                break;
            case 1: // player move
                move_t * move;
                move = new move_t(data, recvd);

                move_t * svrmove;
                svrmove = new move_t;

                std::cout << "move: " << player << "/" << (int) player->playernum << " at " << (int) move->loc.x << "," << (int) move->loc.y << " performs " << (move->action == ACT_MOVE ? "move: " : "place: ");

                //update board
                switch (move->action) {
                    case ACT_MOVE: //this is a fire
                        if (player->get_game()->modes[player->playernum] == GM_GAMEOVER) {
                            player->send_message_s("The game is over! Start a new one.");
                            break;
                        }
                        player->get_game()->modes[player->playernum] = GM_PLAYTIME;
                        if (!player->get_game()->modes[!(player->playernum)] == GM_PLAYTIME) {
                            char msg[] = "Wait for your opponent to finish placing ships!";
                            player->send_message(msg);
                            break;
                        }
                        if (!(player->playernum == player->get_game()->turn)) {
                            char msg[] = "It's not your turn!";
                            player->send_message(msg);
                            std::cout << '\n'; // formatting in console
                            break;
                        }
                        player->get_game()->turn = !(player->playernum);
                        player->get_game()->board.set_fired(player->playernum, move->loc);
                        if (player->get_game()->board.get_ship(!(player->playernum), move->loc) == true) { // hit
                            //update both clients telling them that it was a hit using a move pkt
                            svrmove->action = YOU_HIT; //tell the client they hit
                            svrmove->loc = move->loc;
                            send(player->sockfd, svrmove, sizeof (move_t), SENDFLAGS);

                            std::cout << "Hit\n";

                            // check for endgame
                            location locc;
                            bool iwon = true;
                            uint8_t i = 0, j;
                            for (; i < BOARDSIZE && iwon; i++) {
                                for (j = 0; j < BOARDSIZE && iwon; j++) {
                                    locc.set(i, j);

                                    if (player->get_game()->board.get_ship(!(player->playernum), locc)
                                            && !(player->get_game()->board.get_fired(player->playernum, locc))) {
                                        iwon = false;
                                    }
                                }
                            }
                            if (iwon) {
                                std::cout << "Game over! Winner: " << (int) player->playernum << '\n';
                                uint8_t cyc = 0;
                                for (; cyc < 2; cyc++) {
                                    refresh_t * update;
                                    update = new refresh_t;
                                    update->mode = GM_GAMEOVER;
                                    player->get_game()->modes[cyc] = GM_GAMEOVER;

                                    uint8_t o = 0, p;
                                    location locb;
                                    for (; o < BOARDSIZE; o++) {
                                        for (p = 0; p < BOARDSIZE; p++) {
                                            locb.set(o, p);
                                            if (cyc == player->playernum) {
                                                update->board.set_tile_raw(locb, update->board.stripenemyships(player->get_game()->board.get_tile_raw(locb)));
                                            } else {
                                                update->board.set_tile_raw(locb, update->board.stripenemyships(update->board.invert(player->get_game()->board.get_tile_raw(locb))));
                                            }
                                        }
                                    }
                                    update->mode = player->get_game()->modes[player->playernum];
                                    send(player->sockfd, update, sizeof (refresh_t), SENDFLAGS);
                                } // for each player
                                usleep(15000);
                                if (player->otherplayer() != NULL)
                                    player->otherplayer()->send_message_s("Game over, you lost!");
                                player->send_message_s("Game over, you won!");
                            }
                        } else { //it was a miss
                            //tell both clients it was a miss using a move pkt
                            svrmove->action = YOU_MISS; //tell the client they hit
                            svrmove->loc = move->loc;
                            send(player->sockfd, svrmove, sizeof (move_t), SENDFLAGS);

                            std::cout << "Miss\n";
                        }
                        svrmove->action = THEY_FIRED; //tell the enemy (player[1]) that they've been hit
                        if (!(player->otherplayer() == NULL))
                            send(player->otherplayer()->sockfd, svrmove, sizeof (move_t), SENDFLAGS);
                        break;
                    case ACT_PLACE://this is a place
                        if (player->get_game()->modes[player->playernum] == GM_PLAYTIME) break;
                        if (player->get_game()->board.get_ship(player->playernum, move->loc) == false) { //there's not a ship there so it can be placed
                            player->get_game()->board.set_ship(player->playernum, move->loc);

                            //send move to this client only
                            svrmove->action = ACT_PLACE;
                            svrmove->loc = move->loc;
                            send(player->sockfd, svrmove, sizeof (move_t), SENDFLAGS);

                            std::cout << "Place\n";
                        } else { //there is a ship there
                            chat_t * place_err;
                            place_err = new chat_t;
                            strcpy(place_err->msg, "You already have a ship there!\n");
                            send(player->sockfd, place_err, sizeof (chat_t), SENDFLAGS); //tell this client only that they can't send a ship

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
                        if (player->playernum == 0) {
                            update->board.set_tile_raw(locb, update->board.stripenemyships(player->get_game()->board.get_tile_raw(locb)));
                        } else {
                            update->board.set_tile_raw(locb, update->board.stripenemyships(update->board.invert(player->get_game()->board.get_tile_raw(locb))));
                        }
                    }
                }
                update->mode = player->get_game()->modes[player->playernum];
                send(player->sockfd, update, sizeof (refresh_t), SENDFLAGS); //send the board update to the client
                break;
            }
            case 3: //chat
            {
                std::cout << "This is a chat packet.\n";
                chat_t * chatmsg;
                chatmsg = new chat_t(data, recvd);

                chat_t * svrreply; //the chat reply from the server to client1
                svrreply = new chat_t;

                //look up the game this is in
                if (player->get_game() == NULL) { //the player isn't in a game yet, this shouldn't happen!

                    std::cout << "Silly client... they're not in a game.\n";
                    strcpy(svrreply->msg, "You aren't connected to a game: you can't chat yet.\n");
                    send(socketid, (void *) svrreply, sizeof (chat_t), SENDFLAGS); //send the error message to the client

                } else { //the client is in a game
                    if (player->get_game()->get_player(0) == player) { //this is player[0]

                        std::cout << "player[0] sent a chat\n";
                        strcpy(svrreply->msg, "Me: ");
                        strcpy(svrreply->msg + 4, chatmsg->msg);
                        send(socketid, (void *) svrreply, sizeof (chat_t), SENDFLAGS); //reply to the sender

                        strcpy(svrreply->msg, "Them: ");
                        strcpy(svrreply->msg + 6, chatmsg->msg);
                        send(player->get_game()->get_player(1)->sockfd, (void *) svrreply, sizeof (chat_t), SENDFLAGS); //reply to the other player

                    } else { //this is player[1]
                        std::cout << "player[1] sent a chat\n";
                        strcpy(svrreply->msg, "Me: ");
                        strcpy(svrreply->msg + 4, chatmsg->msg);
                        send(socketid, (void *) svrreply, sizeof (chat_t), SENDFLAGS); //reply to the sender

                        strcpy(svrreply->msg, "Them: ");
                        strcpy(svrreply->msg + 6, chatmsg->msg);
                        send(player->get_game()->get_player(0)->sockfd, (void *) svrreply, sizeof (chat_t), SENDFLAGS); //reply to the other player
                    }
                }
                break;
            }
            default:
            {
                std::cout << "Invalid packet received.\n";
                break;
            }


        }
        // do stuff with the data ( see client.cpp wait_data )
    }
}
