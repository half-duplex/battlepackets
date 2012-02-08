/**
 * net.h
 * Battlepackets
 * Norwich University IS460 Spring 2012
 * Battlepackets Group - battlepackets@groups.google.com

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

#ifndef NET_H
#define	NET_H

/* netconnect
 * Creates a connection out to a server
 * arguments:   char * addr[] - the address to connect out to
 *              int addrlen - the length of the address array
 *              int port - the target port
 * returns:     bool - success true/false
 */
bool netconnect(char * addr[], int addrlen, int port);

/* netlisten
 * Creates a socket for incoming connections
 * arguments:   int port - the target port
 * returns:     int - the file descriptor of the socket, OR -1 if failure
 */
int netlisten(int port);

/* netsend
 * Sends RAW/PREFORMATTED data over an existing connection
 * arguments:   int sockfd - the socket file descriptor to send on
 *              char * data[] - the data to send
 *              int datalen - the length of the data
 * returns:     bool - success true/false
 */
bool netsend(int sockfd, char * data[], int datalen);

/* netrecv
 * A function that waits for incoming data and uses the callback to pass it on
 * arguments:   void (*handler)(char * data[], int datalen)
 *                      This argument is a function that will be called with
 *                      any data recieved.
 */
void netrecv(void (*handler)(char * data[], int datalen));

#endif	/* NET_H */
