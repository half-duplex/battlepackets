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
#define MAXDATASIZE 1500

#include <stdint.h>
#include <string.h>
#include "iostream"
#include <netinet/tcp.h>
#include <boost/thread.hpp>

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
    void import(lboard_t * board);
    // bool player: 0=self 1=enemy
    //DO 0 AND 1 ACTUALLY DO ANYTHING YET?!?!!? ie lookup the other player's game/board
    /* 0 should = player
     1 should = player->game->player[1] IFF player[0] == player*/
    bool get_ship(bool player, location loc); // won't cheat on client: no data
    void set_ship(bool player, location loc);
    bool get_fired(bool player, location loc);
    void set_fired(bool player, location loc);
    uint8_t get_tile_raw(location loc);
    void set_tile_raw(location loc, uint8_t status);
    uint8_t invert(uint8_t raw);
    uint8_t stripenemyships(uint8_t raw);
private:
    uint8_t board_data[BOARDSIZE][BOARDSIZE];
};

typedef enum {
    GM_START = 0, // just started the program
    GM_WAIT_HANDSHAKE = 1, // connected, waiting for initial board
    GM_WAIT_BOARD = 2, // connected, got handshake, waiting for inital board
    GM_SHIP1 = 10, // placing ship 1: submarine: 1 piece
    GM_SHIP2 = 11, // placing ship 2: submarine: 1 piece
    GM_SHIP3 = 12, // placing ship 3: destroyer: 2 pieces
    GM_SHIP4 = 13, // placing ship 4: destroyer: 2 pieces
    GM_SHIP5 = 14, // placing ship 5: cruiser: 3 pieces
    GM_SHIP6 = 15, // placing ship 6: battleship: 4 pieces
    GM_SHIP7 = 16, // placing ship 7: carrier: 5 pieces
    GM_PLAYTIME = 255
} gamemode_t;

typedef enum {
    ACT_MOVE = 1, //fire  c->s
    ACT_PLACE = 2, //c->s //s->c, you were able to place a ship
    // YOU_SHIP = 2, // same thing...?
    YOU_HIT = 3, //s->c, you got a hit on the enemy!
    YOU_MISS = 4, //s->c, this shot was a miss on the enemy's board
    THEY_FIRED = 5 //s->c, then from this the client has to derive if it was a hit
} move_action_t;


// Packet types

struct handshake_t {
private:
    char pktid; // = 0
public:
    char protover; // = PROTOVERSION
    char boardsize; // = BOARDSIZE
    char username[19 - 3];
    char gameid[52 - 20];

    handshake_t();
    handshake_t(char * data, int datalen);
};

struct move_t {
private:
    char pktid; // = 1
public:
    location loc; //x/y
    move_action_t action; // (see protocol)

    move_t();
    move_t(char * data, int datalen);
};

struct refresh_t {
private:
    char pktid; // = 2
public:
    lboard_t board; //each [x][y] coordinate will have a specific absolute state (see protocol)
    gamemode_t mode;

    refresh_t();
    refresh_t(char * data, int datalen);
};

struct chat_t {
private:
    char pktid; // = 3
public:
    char msg[255];
    chat_t();
    chat_t(char * data, int datalen);
};


#endif	/* COMMON_H */
