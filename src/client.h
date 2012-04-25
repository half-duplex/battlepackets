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

#ifndef CLIENT_H
#define	CLIENT_H

#include "common.h"
#include <gtkmm/box.h>
#include <gtkmm/main.h>
#include <gtkmm/entry.h>
#include <gtkmm/image.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/textview.h>
#include <gtkmm/menubar.h>
#include <gtkmm/scrolledwindow.h>

#define M_IMG_EMPTY "/usr/share/icons/gnome/16x16/actions/add.png"
#define M_IMG_SHIP "/usr/share/icons/gnome/16x16/actions/address-book-new.png"
#define M_IMG_HIT "/usr/share/icons/gnome/16x16/actions/call-stop.png"
#define M_IMG_MISS "/usr/share/icons/gnome/16x16/actions/format-justify-center.png"

int main_client(int argc, char** argv);

/* wait_data
 * spawned as a thread to wait for and handle data
 */
void wait_data();
void send_data(void * data, int datalen);

void connect();

class BPwin : public Gtk::Window {
public:
    BPwin();
    virtual ~BPwin();

protected:
    // Variables

    // The actual board
    lboard_t lboard;

    typedef enum {
        GM_START = 0, // just started the program
        GM_CONNECT = 1, // connected, waiting for initial board
        GM_SHIP1 = 10, // placing ship 1: submarine: 1 piece
        GM_SHIP2 = 11, // placing ship 2: submarine: 1 piece
        GM_SHIP3 = 12, // placing ship 3: destroyer: 2 pieces
        GM_SHIP4 = 13, // placing ship 4: destroyer: 2 pieces
        GM_SHIP5 = 14, // placing ship 5: cruiser: 3 pieces
        GM_SHIP6 = 15, // placing ship 6: battleship: 4 pieces
        GM_SHIP7 = 16, // placing ship 7: carrier: 5 pieces
        GM_PLAYTIME = 255
    } t_gamemode;
    t_gamemode gamemode;
    location placing; // for the first click for placing ships
    location prev; // TODO: Remove: Should not be needed
    int count; // TODO: Remove: Should not be needed

    // Signal handlers
    void tile_clicked(int btn_num); // ONLY FOR MY BOARD, not needed for enemy
    bool chat_key_press(GdkEventKey* k);

    // Big frames
    Gtk::VBox m_box_everything;
    Gtk::HBox m_box_boards;
    Gtk::HBox m_box_chat_input;
    Gtk::Button m_btn_chat_send;
    void chat_send();
    void draw_connect_window();

    struct vboard {
        Gtk::HBox m_box_board;

        // images: must all exist, else memory leak...?
        Gtk::Image m_img_set[3][BOARDSIZE][BOARDSIZE];
        // the 3: 0=ocean/empty,1=hit,2=(my_board?ship:miss)

        // columns/buttons: see google doc
        Gtk::VBox m_box_tile_column[BOARDSIZE]; // columns
        Gtk::Button m_button[BOARDSIZE][BOARDSIZE]; // buttons

        vboard();
        void init(BPwin & that, char which); // which: 0=my,1=enemy
    };
    vboard boards[2]; // 0 is me, 1 is enemy

    // Chat
    Gtk::VBox m_box_chat;
    Gtk::ScrolledWindow m_log_scroll;
    Gtk::Entry m_entry;
    // helpful for buffers and textview:
    // http://inti.sourceforge.net/tutorial/libinti/textwidget.html
    Gtk::TextView m_log;
    Glib::RefPtr<Gtk::TextBuffer> m_log_buf;

    // Connect button
    Gtk::Button m_btn_connect;

    class Connwin : public Gtk::Window {
    public:
        Connwin();
        virtual ~Connwin();
    protected:
        Gtk::VBox m_box_everything;
        Gtk::Entry m_user;
        Gtk::Entry m_pass;
        Gtk::Entry m_game;
        Gtk::Button m_btn_go;
        void do_connect();
    };
};

#endif	/* CLIENT_H */
