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
    set(ix, iy);
}

void location::set(uint8_t ix, uint8_t iy) {
    if (ix >= BOARDSIZE || iy >= BOARDSIZE) {
        cout << "Invalid location!\n";
        return;
    }
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

void lboard_t::import(lboard_t * board) {
    location locb;
    for (int i = 0; i < BOARDSIZE; i++) {
        for (int j = 0; j < BOARDSIZE; j++) {
            locb.set(i, j);
            board_data[i][j] = board->get_tile_raw(locb);
        }
    }
}

// bool player: 0=self 1=enemy
// board_data format: 00dc00ba
// a = player 0 ship
// b = player 0 fired
// c = player 1 ship
// d = player 1 fired

bool lboard_t::get_ship(bool player, location loc) { // won't cheat on client: no data
    bool a = board_data[loc.x][loc.y]&(1 << (player * 4));
    return a;
}

void lboard_t::set_ship(bool player, location loc) {
    board_data[loc.x][loc.y] |= (1 << (player * 4));
}

bool lboard_t::get_fired(bool player, location loc) {
    return board_data[loc.x][loc.y]&(1 << (1 + (4 * player)));
}

void lboard_t::set_fired(bool player, location loc) {
    board_data[loc.x][loc.y] |= (1 << (1 + (4 * player)));
}

uint8_t lboard_t::get_tile_raw(location loc) {
    return board_data[loc.x][loc.y];
}

void lboard_t::set_tile_raw(location loc, uint8_t status) {
    board_data[loc.x][loc.y] = status;
}

uint8_t lboard_t::invert(uint8_t raw) {
    return ((raw << 4) | (raw >> 4));
}

uint8_t lboard_t::stripenemyships(uint8_t raw) {
    if ((raw & (1 << 1))) { // if player 0 has fired
        return raw;
    } else {
        return (raw & (255 ^ (1 << 4)));
    }
}

void parse_conn_err(int errnum) {
    switch (errnum) {
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
        case EPERM:
            std::cout << "The user tried to connect to a broadcast address without having the socket broadcast flag enabled or the connection request failed because of a local firewall rule. \n";
            break;
        case EAFNOSUPPORT:
            std::cout << "The passed address didn't have the correct address family in its sa_family field. \n";
            break;
        case EAGAIN:
            std::cout << "No more free local ports or insufficient entries in the routing cache. For AF_INET see the description of /proc/sys/net/ipv4/ip_local_port_range ip(7) for information on how to increase the number of local ports. \n";
            break;
        case EALREADY:
            std::cout << "The socket is nonblocking and a previous connection attempt has not yet been completed. \n";
            break;
        case ECONNREFUSED:
            std::cout << "No-one listening on the remote address. \n";
            break;
        case EINPROGRESS:
            std::cout << "The socket is nonblocking and the connection cannot be completed immediately. It is possible to select(2) or poll(2) for completion by selecting the socket for writing. After select(2) indicates writability, use getsockopt(2) to read the SO_ERROR option at level SOL_SOCKET to determine whether connect() completed successfully (SO_ERROR is zero) or unsuccessfully (SO_ERROR is one of the usual error codes listed here, explaining the reason for the failure). \n";
            break;
        case EINTR:
            std::cout << "The system call was interrupted by a signal that was caught; see signal(7). \n";
            break;
        case EISCONN:
            std::cout << "The socket is already connected. \n";
            break;
        case ENETUNREACH:
            std::cout << "Network is unreachable. \n";
            break;
        case ETIMEDOUT:
            std::cout << "Timeout while attempting connection. The server may be too busy to accept new connections. Note that for IP sockets the timeout may be very long when syncookies are enabled on the server.\n";
            break;
        default:
            std::cout << "Unknown error.\n";
            break;
    }
}


// Packets

handshake_t::handshake_t() {
    for (int i = sizeof (handshake_t) - 1; i >= 0; i--) ((uint8_t *)this)[i] = 0;
    pktid = 0;
    boardsize = BOARDSIZE;
    protover = PROTOVERSION;
}

handshake_t::handshake_t(char * data, int datalen) { //for unpackaging data
    for (int i = sizeof (handshake_t) - 1; i >= 0; i--) ((uint8_t *)this)[i] = 0;
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
    strncpy(username, ((handshake_t *) data)->username, 19 - 3 - 1);
    strncpy(gameid, ((handshake_t *) data)->gameid, 52 - 20 - 1);

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
    for (int i = sizeof (move_t) - 1; i >= 0; i--) ((uint8_t *)this)[i] = 0;
    pktid = 1;
}

move_t::move_t(char* data, int datalen) {
    for (int i = sizeof (move_t) - 1; i >= 0; i--) ((uint8_t *)this)[i] = 0;
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
}

refresh_t::refresh_t() {
    for (int i = sizeof (refresh_t) - 1; i >= 0; i--) ((uint8_t *)this)[i] = 0;
    pktid = 2;
}

refresh_t::refresh_t(char* data, int datalen) {
    for (int i = sizeof (refresh_t) - 1; i >= 0; i--) ((uint8_t *)this)[i] = 0;
    if (data[0] != 2) { // check packet id
        std::cout << "Wrong packet for refresh_t!\n";
    }
    if (datalen != sizeof (refresh_t)) { // check length
        std::cout << "Wrong packet size for refresh_t! " << datalen << " should be " << sizeof (refresh_t) << "\n";
        return;
    }

    pktid = ((refresh_t *) data)->pktid;
    board.import(&((refresh_t *) data)->board);
    mode = ((refresh_t *) data)->mode;
}

chat_t::chat_t() {
    for (int i = sizeof (chat_t) - 1; i >= 0; i--) ((uint8_t *)this)[i] = 0;
    pktid = 3;
    for (int i = 0; i < 255; i++) {
        msg[i] = '\0';
    }
}

chat_t::chat_t(char * data, int datalen) {
    for (int i = sizeof (chat_t) - 1; i >= 0; i--) ((uint8_t *)this)[i] = 0;
    if (data[0] != 3) { // check packet id
        std::cout << "Wrong packet for chat_t!\n";
    }
    if (datalen != sizeof (chat_t)) { // check length
        std::cout << "Wrong packet size for char_t! " << datalen << " should be " << sizeof (chat_t) << "\n";
        return;
    }

    strncpy(msg, ((chat_t *) data)->msg, 254); // -1 so we leave one null byte, just in case
}
