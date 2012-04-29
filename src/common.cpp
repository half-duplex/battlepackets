/**
 * common.cpp
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

#include "common.h"
#include <cstring>
#include <iostream>
using namespace std;

location::location() {
}

location::location(uint8_t ix, uint8_t iy) {
    x = ix;
    y = iy;
}

void location::set(uint8_t ix, uint8_t iy) {
    x = ix;
    y = iy;
}

void location::set(location il) {
    x = il.x;
    y = il.y;
}

lboard_t::lboard_t() {
    for (int i = 0; i < BOARDSIZE; i++) {
        for (int j = 0; j < BOARDSIZE; j++) {
            board_data[i][j] = 0;
        }
    }
}

void lboard_t::import(uint8_t * board) {
    for (int i = 0; i < BOARDSIZE; i++) {
        for (int j = 0; j < BOARDSIZE; j++) {
            // Don't know why this doesn't work
            //            board_data[i][j] =
            //                    board[i][j];
        }
    }
}

// bool player: 0=self 1=enemy
// board_data format: 00dc00ba
// a = player 0 ship
// b = player 1 ship
// c = player 0 fired
// d = player 1 fired

bool lboard_t::get_ship(bool player, location loc) { // won't cheat on client: no data
    return board_data[loc.x][loc.y]&(1 << (player)); //returns true if there is a ship
}

void lboard_t::set_ship(bool player, location loc) {
    board_data[loc.x][loc.y] |= (1 << (player));
}

bool lboard_t::get_fired(bool player, location loc) {
    return board_data[loc.x][loc.y]&(1 << (4 + player));
}

void lboard_t::set_fired(bool player, location loc) {
    board_data[loc.x][loc.y] |= (1 << (4 + player));
}

uint8_t lboard_t::get_tile_raw(location loc) {
    return board_data[loc.x][loc.y];
}

//void lboard_t::set_tile_raw(location loc, uint8_t status) {
//    int i, l;
//    for (i = 0; i < BOARDSIZE; i++) {
//        for (l = 0; i < BOARDSIZE; i++) {
//            //set the existing board to the new board
//            //update each icon with:
//            //    boards[0].m_button[loc.x][loc.y].set_image(boards[0].m_img_set[2][loc.x][loc.y]);
//
//        }
//    }
//}

// Packets

handshake_t::handshake_t() {
    pktid = 0;
    boardsize = BOARDSIZE;
    protover = PROTOVERSION;
}

handshake_t::handshake_t(char * data, int datalen) { //for unpackaging data
    if (data[0] != 0) { // check packet id
        std::cout << "Wrong packet for handshake_t!\n";
    }
    if (datalen != sizeof (handshake_t)) { // check length
        std::cout << "Wrong packet size for handshake_t! " << datalen << " should be " << sizeof (handshake_t) << "\n";
        return;
    }

    pktid = ((handshake_t *) data)->pktid;
    protover = ((handshake_t *) data)->protover;
    boardsize = ((handshake_t *) data)->boardsize;
    strncpy(username, ((handshake_t *) data)->username, 19 - 3);
    strncpy(gameid, ((handshake_t *) data)->gameid, 52 - 20);

    if (protover != PROTOVERSION) {
        std::cout << "Old protocol version! " << protover << " should be " << PROTOVERSION << "\n";
        return;
    }
    if (boardsize != BOARDSIZE) {
        std::cout << "Different board size! " << boardsize << " should be " << BOARDSIZE << "\n";
        return;
    }
    username[19 - 3 - 1] = '\0';
    // gameid is restricted to printable characters less ' '
    gameid[52 - 20 - 1] = '\0';
}

move_t::move_t() {
    pktid = 1;
}

move_t::move_t(char* data, int datalen) {
    if (data[0] != 1) { // check packet id
        std::cout << "Wrong packet for move_t!\n";
    }
    if (datalen != sizeof (move_t)) { // check length
        std::cout << "Wrong packet size for move_t! " << datalen << " should be " << sizeof (move_t) << "\n";
        return;
    }

    pktid = ((move_t *) data)->pktid;
    loc = ((move_t *) data)->loc;
    action = ((move_t *) data)->action;

    if (loc.x >= BOARDSIZE || loc.y >= BOARDSIZE) {
        std::cout << "Invalid coordinates\n";
    }

    // proposal: dump action bits, use only absolute bits. or maybe action bit = the absolute bit changed?
}

refresh_t::refresh_t() {
    pktid = 2;
}

refresh_t::refresh_t(char* data, int datalen) {
    if (data[0] != 2) { // check packet id
        std::cout << "Wrong packet for refresh_t!\n";
    }
    if (datalen != sizeof (refresh_t)) { // check length
        std::cout << "Wrong packet size for refresh_t! " << datalen << " should be " << sizeof (refresh_t) << "\n";
        return;
    }

    pktid = ((refresh_t *) data)->pktid;
    board = ((refresh_t *) data)->board;
    mode = ((refresh_t *) data)->mode;

}

chat_t::chat_t() {
    pktid = 3;
    for (int i = 0; i < 255; i++) {
        msg[i] = '\0';
    }
}

chat_t::chat_t(char * data, int datalen) {
    if (data[0] != 3) { // check packet id
        std::cout << "Wrong packet for chat_t!\n";
    }
    if (datalen != sizeof (chat_t)) { // check length
        std::cout << "Wrong packet size for char_t! " << datalen << " should be " << sizeof (chat_t) << "\n";
        return;
    }

    msg[255] = ((chat_t *) data)->msg[255];

}
