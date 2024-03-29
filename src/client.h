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
#include <stdio.h>  /* defines FILENAME_MAX */
#include <unistd.h> // for getcwd
#include <gtkmm/box.h>
#include <gtkmm/main.h>
#include <gtkmm/entry.h>
#include <gtkmm/image.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/textview.h>
#include <gtkmm/menubar.h>
#include <gtkmm/scrolledwindow.h>

#define M_IMG_EMPTY "images/ocean.png"
#define M_IMG_SHIP "images/ship.png"
#define M_IMG_HIT "images/hit.png"
#define M_IMG_MISS "images/miss.png"

int main_client(int argc, char** argv);

/* wait_data
 * spawned as a thread to wait for and handle data
 */
void wait_data();

void connect();

class BPwin : public Gtk::Window {
public:
    BPwin();

    lboard_t lboard; //backend, absolute state data
    void set_tile(uint8_t boarda, uint8_t statea, location loca);

protected:
    // for tracking ship placing
    location placing; // for the first click for placing ships
    location prev;
    int count;

    // Signal handlers
    void tile_clicked_me(int btn_num);
    void tile_clicked_opponent(int btn_num);
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
