/**
 * client.h
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

#include <gtkmm.h>

/* nethandler
 * called by net/netrecv when it gets data
 * arguments:   int sockfd - the socket data was recieved on
 *              char * data[] - the data to send
 *              int datalen - the length of the data
 */
void nethandler(int sockfd, char * data[], int datalen);

class BPwin : public Gtk::Window {
public:
    BPwin();
    virtual ~BPwin();

protected:
    // Signal handlers
    void tile_clicked();

    // Child widgets
    Gtk::Button m_button[BOARDSIZE][BOARDSIZE];
};

#endif	/* SERVER_H */
