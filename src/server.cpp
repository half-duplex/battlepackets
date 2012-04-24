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
#include "net/net.h"
#define DEBUG

using namespace std;

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
        cout << "Arguments:\n";
        for (int i = 0; i < argc; i++) {
            cout << argv[i] << endl;
        }
    }
#endif

    player_t * player;
    int sockfd;

    // make socket

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
