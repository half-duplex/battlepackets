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

#define log(a) m_log_buf->insert(m_log_buf->get_iter_at_offset(0), a);

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

    // spawn net listener
    boost::thread netin(netrecv, nethandler);

    // gtkmm main loop
    Gtk::Main::run(bpwin);

    // die
    netin.interrupt();
    return 0;
}

BPwin::BPwin() {
    gamemode = GM_START;
    placing.x = 255;

    set_title("Battlepackets!");
    set_border_width(10);
    resize(1, 1);
    add(m_box_everything);
    m_box_everything.pack_start(m_menu_bar);
    m_box_everything.pack_start(m_box_boards);
    m_box_boards.pack_start(m_box_board_me);
    m_box_boards.pack_start(m_box_board_enemy);
    m_box_everything.pack_start(m_box_chat);
    m_box_chat.pack_start(m_entry);
    m_box_chat.pack_start(m_log_scroll);
    m_log_scroll.add(m_log);
    m_log.set_size_request(-1, 100);
    m_log.set_editable(false);
    m_log.set_cursor_visible(false);
    m_log_buf = m_log.get_buffer();

    // Menus
    m_menu_bar.add(m_menu_game);

    Gtk::Image * m_image[2][BOARDSIZE][BOARDSIZE];
    for (int j = 0; j < BOARDSIZE; j++) { // for each column
        m_box_board_me.pack_start(m_box_tile_column[0][j]);
        m_box_board_enemy.pack_start(m_box_tile_column[1][j]);
        for (int i = 0; i < BOARDSIZE; i++) { // for each in a column
            // Set up the tile
            m_image[0][i][j] = new Gtk::Image(M_IMG_EMPTY);
            m_image[0][i][j]->set_padding(0, 0);
            m_button[0][i][j].set_image_position(Gtk::POS_LEFT);
            m_button[0][i][j].set_image(*m_image[0][i][j]);
            m_button[0][i][j].set_border_width(0);
            m_button[0][i][j].signal_clicked().connect(
                    sigc::bind<int>(
                    sigc::mem_fun(*this, &BPwin::tile_clicked)
                    , i * BOARDSIZE + j));
            m_entry.signal_key_press_event().connect(
                    sigc::mem_fun(*this, &BPwin::chat_key_press)
                    , false);
            m_box_tile_column[0][j].pack_start(m_button[0][i][j]);
        }
    }


    show_all_children();
}

BPwin::~BPwin() {
}

void BPwin::tile_clicked(int btn_num) {
    location loc((btn_num / BOARDSIZE), (btn_num % BOARDSIZE));
    std::cout << "Clicked: " << (int) loc.x << "," << (int) loc.y << ", gm=" << gamemode << "\n";
    switch (gamemode) {
        case GM_START: // not connected, in a game, etc.: just started the app
            log("You need to connect to a server first!\n");
            break;
        case GM_CONNECT: // connected, no initial board received
            log("Server has not sent game info!\n");
            break;
        case GM_SHIP1: // placing ship: see client.h typedef enum t_gamemode
            log("You placed a submarine!\n");
#warning Change tile
#warning Send to server
            gamemode = GM_SHIP2;
            break;
        case GM_SHIP2: // placing ship: see client.h typedef enum t_gamemode
            log("You placed a submarine!\n");
#warning Do stuff
            break;
        case GM_SHIP3: // placing ship: see client.h typedef enum t_gamemode

            log("You placed a destroyer!\n");
#warning Do stuff
            break;
        case GM_SHIP4: // placing ship: see client.h typedef enum t_gamemode
            log("Server has not sent game info!\n");
#warning Do stuff
            break;
        case GM_SHIP5: // placing ship: see client.h typedef enum t_gamemode
            log("Server has not sent game info!\n");
#warning Do stuff
            break;
        case GM_SHIP6: // placing ship: see client.h typedef enum t_gamemode
            log("Server has not sent game info!\n");
#warning Do stuff
            break;
        case GM_SHIP7: // placing ship: see client.h typedef enum t_gamemode
            log("Server has not sent game info!\n");
#warning Do stuff
            break;
        case GM_PLAYTIME: // ingame
            log("Server has not sent game info!\n");
#warning Do stuff
#warning TODO: Change board based on clicked tile
#warning TODO: Send change, if valid, to server
            break;
        default: // should never get here
            log("What just happened?!\n");
            break;
    }
}

/* Called when a key is pressed in the chat input box
 * return value is apparently whether or not to show the char in the box?
 * possibly whether or not the event has been handled - seems to spam this when false.
 */
bool BPwin::chat_key_press(GdkEventKey* k) {
    //std::cout << "chat box key press: " << k->keyval << "=" << (char)k->keyval << "\n";
    if (k->keyval == 65293) { // return
        if (m_entry.get_text_length() < 1) {
            return true;
        }
        //log
        log("\n");
        log(m_entry.get_text());
        log("Me: ");
        //send
#warning TODO: Send chat to server
        std::cout << "Send chat: " << m_entry.get_text() << std::endl;
        m_entry.set_text("");
        return true;
    }

    // else a key other than enter
    return false;
}

void nethandler(int sockfd, char * data[], int datalen) {
    // grab first char (packet id)
    // switch(packetID){etc}
    // do stuff like update the gui
}
