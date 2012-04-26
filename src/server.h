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
#include <boost/thread.hpp>

struct player_t;
struct game_t;

int main_server(int argc, char** argv);
void wait_data(player_t * player);

struct player_t {
public:
    player_t(int new_sockfd);
    ~player_t();
    int get_sockid(); //return sockid    
    bool setgame(); // returns success
    void send_data(void * data, int datalen);
    game_t * game;
private:
    int sockfd;
    
    boost::thread *tid;
};

struct game_t {
public:
    game_t();
    ~game_t();
    char gameid[32];
    //timestamp with 3 day expire time
    bool addplayer(player_t * player); // returns success
    player_t * players[2];
private:
    lboard_t board;
    
};

#endif	/* SERVER_H */