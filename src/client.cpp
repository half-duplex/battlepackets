/**
 * client.cpp
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
#include "client.h"
#include "net/net.h"
#include <iostream>
#include <boost/thread.hpp>

using namespace std;

/*
 * Main client function
 * Arguments:
 *      int argc
 *              the number of arguments on the command line
 *      int argv
 *              the text of the arguments
 */
int main(int argc, char** argv) {
    // Create structures
    Gtk::Main kit(argc, argv);
    BPwin bpwin;

    // ...

    // spawn net listener
    boost::thread netin(netrecv, nethandler);

    // gtkmm main loop
    Gtk::Main::run(bpwin);

    // die
    netin.interrupt();
    return 0;
}

BPwin::BPwin() {
    set_title("Battlepackets!");
    set_border_width(10);
    resize(500, 500);
    add(m_box_everything);
    m_box_everything.pack_start(m_box_board);
    m_box_everything.pack_start(m_box_chat);

    Gtk::Image * m_image[BOARDSIZE][BOARDSIZE];
    for (int j = 0; j < BOARDSIZE; j++) { // for each column
        m_box_board.pack_start(m_box_tile_column[j]);
        for (int i = 0; i < BOARDSIZE; i++) { // for each in a column
            // Set up the tile
            m_image[i][j] = new Gtk::Image(M_IMG_EMPTY);
            m_image[i][j]->set_padding(0,0); // doesn't appear to do anything -.-
            m_button[i][j].set_image_position(Gtk::POS_LEFT);
            m_button[i][j].set_image(*m_image[i][j]);
            m_button[i][j].set_border_width(0);
            //m_image[i][j]->show();
            m_button[i][j].signal_clicked().connect(sigc::mem_fun(*this, &BPwin::tile_clicked)); // attach event to button

            m_box_tile_column[j].pack_start(m_button[i][j]);
        }
    }

    show_all_children();
}

BPwin::~BPwin() {
}

void BPwin::tile_clicked() {
    std::cout << "Clicked: " << "\n";
}

void nethandler(int sockfd, char * data[], int datalen) {
    // grab first char (packet id)
    // switch(packetID){etc}
    // do stuff like update the gui
}
