///**
// * net.h
// * Battlepackets
// * Norwich University IS460 Spring 2012
// * Battlepackets Group - battlepackets@googlegroups.com
//
//    Copyright 2012 Battlepackets Group
//
//    This file is part of Battlepackets.
//
//    Battlepackets is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    Battlepackets is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Battlepackets.  If not, see <http://www.gnu.org/licenses/>.
// */
//
//#ifndef NET_H
//#define	NET_H
//#include "../common.h"
//
///* netconnect (for client)
// * Creates a connection out to a server
// * arguments:   char * addr - the address to connect out to
// *              int addrlen - the length of the address array
// *              int port - the target port
// * returns:     int - the socket created
// */
//int netconnect(char * addr, int addrlen, int port, void (*callback)(int sockfd, char * data, int datalen));
//
///* netlisten (for server)
// * Creates a socket for incoming connections
// (* arguments:   int port - the target port) << need to work on, listens on port 7777 for now
// * returns:     int - the socket created, OR -1 if failure
// */
//int netlisten(int port, void (*handler)(int));
//
///* netsend (for client and server)
// * Sends RAW/PREFORMATTED data over an existing connection
// * arguments:   int sockfd - the socket file descriptor to send on
// *              char * data - the data to send
// *              int datalen - the length of the data
// * returns:     bool - success true/false
// */
//bool netsend(int sockfd, char * data, int datalen);
//
///* netrecv (for client and server)
// * A function that waits for incoming data and uses the callback to pass it on
// * arguments:   void (*handler)(int sockfd, char * data, int datalen)
// *                      This argument is a function that will be called with
// *                      any data received.
// */
//void netrecv(int sockfd, char *data, int datalen);
//  
//
//
//void* handleclient(int socket);
//
///
//
//#endif	/* NET_H */
