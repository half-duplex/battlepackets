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

#include <gtkmm/box.h>
#include <gtkmm/main.h>
#include <gtkmm/entry.h>
#include <gtkmm/image.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/textview.h>
#include <gtkmm/scrolledwindow.h>

#define M_IMG_EMPTY "/usr/share/icons/gnome/32x32/actions/add.png"
#define M_IMG_SHIP "/usr/share/icons/gnome/32x32/actions/add.png"
#define M_IMG_HIT "/usr/share/icons/gnome/32x32/actions/add.png"
#define M_IMG_MISS "/usr/share/icons/gnome/32x32/actions/add.png"

/* nethandler
 * called by net/netrecv when it gets data
 * arguments:   int sockfd - the socket data was recieved on
 *              char * data[] - the data to send
 *              int datalen - the length of the data
 */
//void nethandler(int sockfd, char * data[], int datalen); 
//not going to need this because the netrecv will handle the data and call the appropraite function

class BPwin : public Gtk::Window {
public:
    BPwin();
    virtual ~BPwin();

protected:
    // Signal handlers
    void tile_clicked(int btn_num);
    bool chat_key_press(GdkEventKey* k);

    // frames
    Gtk::VBox m_box_everything;
    Gtk::HBox m_box_board;
    Gtk::VBox m_box_tile_column[BOARDSIZE];
    Gtk::VBox m_box_chat;

    // tile types
    Gtk::Image *m_img_hit;
    Gtk::Image *m_img_empty;
    Gtk::Image *m_img_ship;
    Gtk::Image *m_img_miss;

    Gtk::Button m_button[BOARDSIZE][BOARDSIZE]; // buttons
    Gtk::Entry m_entry;
    Gtk::ScrolledWindow m_log_scroll;

    // helpful for buffers and textview:
    // http://inti.sourceforge.net/tutorial/libinti/textwidget.html
    Gtk::TextView m_log;
    Glib::RefPtr<Gtk::TextBuffer> log;
};

#endif	/* SERVER_H */
