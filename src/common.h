/**
 * common.h
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

#ifndef COMMON_H
#define	COMMON_H

#define SERVPORT 7777
#define BOARDSIZE 10
#define PROTOVERSION 0

#include <stdint.h>
#include <string.h>
#include "iostream"

struct location {
    uint8_t x;
    uint8_t y;
    location();
    location(uint8_t ix, uint8_t iy);
    void set(uint8_t ix, uint8_t iy);
    void set(location il);
};

struct lboard_t {
public:
    lboard_t();
    void import(uint8_t * board);
    // bool player: 0=self 1=enemy
    bool get_ship(bool player, location loc); // won't cheat on client: no data
    void set_ship(bool player, location loc);
    bool get_fired(bool player, location loc);
    void set_fired(bool player, location loc);
    uint8_t get_tile_raw(location loc);
private:
    uint8_t board_data[BOARDSIZE][BOARDSIZE];
};


// Packet types

struct handshake_t {
    char pktid; // = 0
    char protover; // = PROTOVERSION
    char boardsize; // = BOARDSIZE
    char username[19 - 3];
    char gameid[52 - 20];

    handshake_t();
    handshake_t(char * data, int datalen);
};

struct move_t {
    char ID; // = 1
    location loc;
    char action; // (see protocol)

    move_t();
    move_t(char * data, int datalen);
};

struct refresh {
    char ID; // = 2
    char board[15][15]; //each [x][y] cordinate will have a specific absolute state (0-4) (see protocol)
};

struct chat {
    char ID; // = 3
    char sender; //s->c only, will = 0 if its from the server and 1 if its from the opponent
    short size;
    char msg[100];
};

#endif	/* COMMON_H */
