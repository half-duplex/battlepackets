/**
 * server.h
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

#ifndef SERVER_H
#define	SERVER_H

#include "common.h"
#include <time.h> // for time() and time_t, needed for randomness
#include <cstdlib> // for rand and srand
#include <cstring>

#define SENDFLAGS 0

struct player_t;
struct game_t;

int main_server(int argc, char** argv);
void wait_data(player_t * player);

struct player_t {
public:
    player_t(int new_sockfd);
    int get_sockid(); //return sockid
    game_t * get_game(); // returns success
    bool set_game(game_t * new_game); // returns success
    void send_message(char * message);
    void send_message_s(std::string message);
    player_t * otherplayer();
    int sockfd;
    uint8_t playernum;
    char username[16];
private:
    game_t * game;
    boost::thread *tid;
};

struct game_t {
public:
    game_t();
    ~game_t();
    char gameid[32];
    //timestamp with 3 day expire time
    bool add_player(player_t * player); // returns success
    player_t * get_player(uint8_t playernum);
    void del_player(uint8_t playernum);
    char playernames[2][16];
    gamemode_t modes[2];
    bool turn;
    lboard_t board;
private:
    player_t * players[2];
};

#endif	/* SERVER_H */