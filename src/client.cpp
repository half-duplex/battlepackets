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

#include "client.h"
#include <iostream>
#include <boost/thread.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define log(a) m_log_buf->insert(m_log_buf->get_iter_at_offset(0), a);
#define MAXDATASIZE 100
int socketid; //global

using namespace std;

/*
 * Main client function
 * Arguments:
 *      int argc
 *              the number of arguments on the command line
 *      int argv
 *              the text of the arguments
 */
int main_client(int argc, char** argv) {
    // Create structures
    Gtk::Main kit(argc, argv);
    BPwin bpwin;
    // spawn net listener
    //boost::thread netin(netrecv, nethandler);

    // gtkmm main loop
    Gtk::Main::run(bpwin);


    // die
    //netin.interrupt();
    return 0;
}

BPwin::BPwin() {
    gamemode = GM_START; // TODO: Put back to GM_START once connecting is implemented
    placing.x = 255;

    set_title("Battlepackets!");
    set_border_width(10);
    resize(1, 1);
    add(m_box_everything);
    m_box_everything.pack_start(m_box_boards);
    boards[0].init(*this, 0);
    boards[1].init(*this, 1);
    m_box_boards.pack_start(boards[0].m_box_board);
    m_box_boards.pack_start(boards[1].m_box_board);
    m_box_everything.pack_start(m_box_chat);
    m_box_chat.pack_start(m_box_chat_input);
    m_btn_connect.set_size_request(0, 0);
    m_box_chat_input.pack_start(m_btn_connect);
    m_box_chat_input.pack_start(m_entry);
    m_box_chat_input.pack_start(m_btn_chat_send);
    m_box_chat.pack_start(m_log_scroll);
    m_log_scroll.add(m_log);
    m_log.set_size_request(-1, 100);
    m_log.set_editable(false);
    m_log.set_cursor_visible(false);
    m_log_buf = m_log.get_buffer();
    m_entry.signal_key_press_event().connect(
            sigc::mem_fun(*this, &BPwin::chat_key_press)
            , false);
    m_btn_chat_send.set_label("Send");
    m_btn_chat_send.signal_clicked().connect(
            sigc::mem_fun(*this, &BPwin::chat_send));
    m_btn_connect.set_label("Connect");
    m_btn_connect.signal_clicked().connect(
            sigc::mem_fun(*this, &BPwin::draw_connect_window));

    show_all_children();
}

BPwin::vboard::vboard() {
    for (int j = 0; j < BOARDSIZE; j++) { // for each column
        m_box_board.pack_start(m_box_tile_column[j]);
        for (int i = 0; i < BOARDSIZE; i++) { // for each in a column
            // Set up the tile
            m_button[j][i].set_image_position(Gtk::POS_LEFT);
            m_button[j][i].set_border_width(0);
        }
    }
}

void BPwin::vboard::init(BPwin & that, char which) { // which: 0=mine,1=enemy
    if (which > 1) {
        cout << "What did you break -.-\n";
        return;
    }

    int i, j;
    for (j = 0; j < BOARDSIZE; j++) { // for each column
        for (i = 0; i < BOARDSIZE; i++) { // for each in a column
            // create images
            m_img_set[0][j][i].set(M_IMG_EMPTY);
            m_img_set[0][j][i].set_padding(0, 0);
            m_img_set[1][j][i].set(M_IMG_HIT);
            m_img_set[1][j][i].set_padding(0, 0);
            if (!which) {
                m_img_set[2][j][i].set(M_IMG_SHIP);
                m_img_set[2][j][i].set_padding(0, 0);
            } else {
                m_img_set[2][j][i].set(M_IMG_MISS);
                m_img_set[2][j][i].set_padding(0, 0);
            }

            // assign image
            m_button[j][i].set_image_position(Gtk::POS_LEFT);
            m_button[j][i].set_image(m_img_set[0][j][i]);
            m_button[j][i].set_border_width(0);
            m_box_tile_column[j].pack_start(m_button[j][i]);
            if (which == 0) { // only my board needs signals
                m_button[j][i].signal_clicked().connect(
                        sigc::bind<int>(
                        sigc::mem_fun(that, &BPwin::tile_clicked)
                        , i * BOARDSIZE + j));
            }
        }
    }
}

BPwin::~BPwin() {
}

void BPwin::tile_clicked(int btn_num) {
    location loc((btn_num % BOARDSIZE), (btn_num / BOARDSIZE));
    std::cout << "Clicked: " << (int) loc.x << "," << (int) loc.y << ", gm=" << gamemode << "\n";
    switch (gamemode) {
        case GM_START: // not connected, in a game, etc.: just started the app
            log("Please wait for a server connection.\n");
            break;
        case GM_CONNECT: // connected, no initial board received
            log("Server has not sent game info!\n");
            break;
        case GM_SHIP1: // placing ship: see client.h typedef enum t_gamemode

            log("You placed a submarine!\n");
            log("Next, is another submarine, which is one block =)\n");

            lboard.set_ship(0, loc);
            cout << "Ship at loc: " << (int) lboard.get_ship(0, loc) << endl;
            boards[0].m_button[loc.x][loc.y].set_image(boards[0].m_img_set[2][loc.x][loc.y]);
            // TODO: Send to server
            gamemode = GM_SHIP2;
            break;
        case GM_SHIP2: // placing ship: see client.h typedef enum t_gamemode

            if (lboard.get_ship(0, loc)) {
                log("Dude. There's already a ship there. Try again.\n");
                break;
            } else
                lboard.set_ship(0, loc);
            cout << "Ship at loc: " << (int) lboard.get_ship(0, loc) << endl;
            boards[0].m_button[loc.x][loc.y].set_image(boards[0].m_img_set[2][loc.x][loc.y]);

            log("You placed a submarine!\n");
            log("Next, is a destroyer, which is two blocks =)\n");

            gamemode = GM_SHIP3;

            break;
        case GM_SHIP3: // placing ship: see client.h typedef enum t_gamemode
            if (lboard.get_ship(0, loc)) {
                log("Dude. There's already a ship there. Try again.\n");
                break;
            }
            if (count == 0) {
                log("You placed destroyer block!\n");
                lboard.set_ship(0, loc);
                cout << "Ship at loc: " << (int) lboard.get_ship(0, loc) << endl;
                boards[0].m_button[loc.x][loc.y].set_image(boards[0].m_img_set[2][loc.x][loc.y]);
                prev.x = (int) loc.x;
                prev.y = (int) loc.y;

                count++;
                break;

            } else if (count == 1) {
                loc.x = (int) loc.x;
                loc.y = (int) loc.y;

                if ((loc.x == prev.x && ((loc.y == (prev.y - 1)) || loc.y == (prev.y + 1))) || (loc.y == prev.y && ((loc.x == (prev.x - 1)) || loc.x == (prev.x + 1)))) {

                    log("You placed destroyer block !!!\n");
                    log("Next, is another destroyer, which is two blocks =)\n");

                    lboard.set_ship(0, loc);
                    cout << "Ship at loc: " << (int) lboard.get_ship(0, loc) << endl;
                    boards[0].m_button[loc.x][loc.y].set_image(boards[0].m_img_set[2][loc.x][loc.y]);
                    count = 0;
                    gamemode = GM_SHIP4;

                    break;
                } else {
                    log("You can't split up your ship!!!\n");
                    break;
                }

            }

        case GM_SHIP4: // placing ship: see client.h typedef enum t_gamemode
            if (lboard.get_ship(0, loc)) {
                log("Dude. There's already a ship there. Try again.\n");
                break;
            }
            if (count == 0) {
                log("You placed destroyer block!\n");

                lboard.set_ship(0, loc);
                cout << "Ship at loc: " << (int) lboard.get_ship(0, loc) << endl;
                boards[0].m_button[loc.x][loc.y].set_image(boards[0].m_img_set[2][loc.x][loc.y]);
                prev.x = (int) loc.x;
                prev.y = (int) loc.y;

                count++;
                break;

            } else if (count == 1) {
                loc.x = (int) loc.x;
                loc.y = (int) loc.y;

                if ((loc.x == prev.x && ((loc.y == (prev.y - 1)) || loc.y == (prev.y + 1))) || (loc.y == prev.y && ((loc.x == (prev.x - 1)) || loc.x == (prev.x + 1)))) {

                    log("You placed destroyer block!\n");
                    log("Next, is a cruiser, which is three blocks =)\n");

                    lboard.set_ship(0, loc);
                    cout << "Ship at loc: " << (int) lboard.get_ship(0, loc) << endl;
                    boards[0].m_button[loc.x][loc.y].set_image(boards[0].m_img_set[2][loc.x][loc.y]);
                    count = 0;
                    gamemode = GM_SHIP5;

                    break;
                } else {
                    log("You can't split up your ship!!!\n");
                    break;
                }
            }

            break;
        case GM_SHIP5: // placing ship: see client.h typedef enum t_gamemode
            if (lboard.get_ship(0, loc)) {
                log("Dude. There's already a ship there. Try again.\n");
                break;
            }
            if (count == 0) {
                log("You placed a cruiser block!\n");
                lboard.set_ship(0, loc);
                cout << "Ship at loc: " << (int) lboard.get_ship(0, loc) << endl;
                boards[0].m_button[loc.x][loc.y].set_image(boards[0].m_img_set[2][loc.x][loc.y]);
                prev.x = (int) loc.x;
                prev.y = (int) loc.y;

                count++;
                break;

            } else if (count == 1 || count == 2) {
                loc.x = (int) loc.x;
                loc.y = (int) loc.y;

                if ((loc.x == prev.x && ((loc.y == (prev.y - 1)) || loc.y == (prev.y + 1))) || (loc.y == prev.y && ((loc.x == (prev.x - 1)) || loc.x == (prev.x + 1)))) {
                    log("You placed a cruiser block!!!\n");

                    lboard.set_ship(0, loc);
                    cout << "Ship at loc: " << (int) lboard.get_ship(0, loc) << endl;
                    boards[0].m_button[loc.x][loc.y].set_image(boards[0].m_img_set[2][loc.x][loc.y]);
                    prev.x = (int) loc.x;
                    prev.y = (int) loc.y;
                    if (count == 2) {
                        count = 0;
                        log("Next, is a battleship, which is four blocks =)\n");
                        gamemode = GM_SHIP6;
                        break;
                    }
                    count++;

                    break;
                } else {
                    log("You can't split up your ship!!!\n");
                    break;
                }
            }

        case GM_SHIP6: // placing ship: see client.h typedef enum t_gamemode
            if (lboard.get_ship(0, loc)) {
                log("Dude. There's already a ship there. Try again.\n");
                break;
            }
            if (count == 0) {
                log("You placed a battleship block!\n");
                lboard.set_ship(0, loc);
                cout << "Ship at loc: " << (int) lboard.get_ship(0, loc) << endl;
                boards[0].m_button[loc.x][loc.y].set_image(boards[0].m_img_set[2][loc.x][loc.y]);
                prev.x = (int) loc.x;
                prev.y = (int) loc.y;

                count++;
                break;

            } else if (count == 1 || count == 2 || count == 3) {
                loc.x = (int) loc.x;
                loc.y = (int) loc.y;

                if ((loc.x == prev.x && ((loc.y == (prev.y - 1)) || loc.y == (prev.y + 1))) || (loc.y == prev.y && ((loc.x == (prev.x - 1)) || loc.x == (prev.x + 1)))) {
                    log("You placed a battleship block!!\n");
                    lboard.set_ship(0, loc);
                    cout << "Ship at loc: " << (int) lboard.get_ship(0, loc) << endl;
                    boards[0].m_button[loc.x][loc.y].set_image(boards[0].m_img_set[2][loc.x][loc.y]);
                    prev.x = (int) loc.x;
                    prev.y = (int) loc.y;
                    if (count == 3) {
                        count = 0;
                        log("Next, is a carrier, which is five blocks =)\n");
                        gamemode = GM_SHIP7;
                        break;
                    }
                    count++;

                    break;
                } else {
                    log("You can't split up your ship!!!\n");
                    break;
                }
            }

        case GM_SHIP7: // placing ship: see client.h typedef enum t_gamemode
            if (lboard.get_ship(0, loc)) {
                log("Dude. There's already a ship there. Try again.\n");
                break;
            }
            if (count == 0) {
                log("You placed a carrier block!\n");
                lboard.set_ship(0, loc);
                cout << "Ship at loc: " << (int) lboard.get_ship(0, loc) << endl;
                boards[0].m_button[loc.x][loc.y].set_image(boards[0].m_img_set[2][loc.x][loc.y]);
                prev.x = (int) loc.x;
                prev.y = (int) loc.y;

                count++;
                break;

            } else if (count == 1 || count == 2 || count == 3 || count == 4) {
                loc.x = (int) loc.x;
                loc.y = (int) loc.y;

                if ((loc.x == prev.x && ((loc.y == (prev.y - 1)) || loc.y == (prev.y + 1))) || (loc.y == prev.y && ((loc.x == (prev.x - 1)) || loc.x == (prev.x + 1)))) {
                    log("You placed a carrier block!!\n");
                    lboard.set_ship(0, loc);
                    cout << "Ship at loc: " << (int) lboard.get_ship(0, loc) << endl;
                    boards[0].m_button[loc.x][loc.y].set_image(boards[0].m_img_set[2][loc.x][loc.y]);
                    prev.x = (int) loc.x;
                    prev.y = (int) loc.y;
                    if (count == 4) {
                        log("Done placing ships!\n");
                        count = 0;
                        gamemode = GM_PLAYTIME;
                        break;
                    }
                    count++;

                    break;
                } else {
                    log("You can't split up your ship!!!\n");
                    break;
                }
            }
            // TODO: Do stuff
            break;
        case GM_PLAYTIME: // ingame
            log("Server has not sent game info!\n");
            // TODO: Do stuff
            // TODO: TODO: Change board based on clicked tile
            // TODO: TODO: Send change, if valid, to server
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
        // TODO: Send chat to server
        std::cout << "Send chat: " << m_entry.get_text() << std::endl;
        m_entry.set_text("");
        return true;
    }

    // else a key other than enter
    return false;
}

void BPwin::chat_send() {
    GdkEventKey event;
    event.keyval = 65293;
    bool bunk;
    bunk = chat_key_press(&event);
}

void BPwin::draw_connect_window() {
    Connwin connwin;
    Gtk::Main::run(connwin);
}

BPwin::Connwin::Connwin() {
    set_title("Connect");
    set_border_width(10);
    resize(1, 1);
    add(m_box_everything);
    m_box_everything.pack_start(m_user);
    //m_box_everything.pack_start(m_pass);
    m_box_everything.pack_start(m_game);
    m_box_everything.pack_start(m_btn_go);
    m_btn_go.set_label("Connect");
    m_btn_go.signal_clicked().connect(sigc::mem_fun(*this, &BPwin::Connwin::do_connect));
    m_user.set_text("username");
    m_game.set_text("game (empty=new)");

    show_all_children();
}

BPwin::Connwin::~Connwin() {
}

void BPwin::Connwin::do_connect() {
    if (m_user.get_text_length() <= 0) {
        cout << "You need to enter a username!\n"; // can't get to log from here
        return;
    }
    cout << "Connecting as user " << m_user.get_text() << " to game " << m_game.get_text() << endl;
    connect();
    // TODO: Send handshake packet
    handshake_t handshake_pkt; //create an instance of the handshake_t structure

    string username = m_user.get_text();
    string gameid = m_game.get_text();
    /*set all the elements for the handshake packet*/
//    strcpy(handshake_pkt.username, username);
    size_t u_length;
    size_t g_length;
    u_length = username.copy(handshake_pkt.username, 16, 0);
    g_length = gameid.copy(handshake_pkt.gameid, 32, 0);
    handshake_pkt.username[u_length] = '\0';
    handshake_pkt.gameid[g_length] = '\0'; 
    cout << handshake_pkt.username << endl;
    cout << handshake_pkt.gameid << endl;
//    handshake_pkt.username = m_user.get_text();
//    handshake_pkt.gameid = m_game.get_text();
    handshake_pkt.pktid = 0;
    handshake_pkt.boardsize = BOARDSIZE;
    handshake_pkt.protover = PROTOVERSION;

//    send_data((void*)handshake_pkt, sizeof(handshake_t));

    gtk_main_quit();
}

void connect() {
    char* serveraddr = "127.0.0.1";
    int port = SERVPORT;
    // connect, spawn wait_data with socketid
    struct sockaddr_in client;
    int socketid;
    int * ptr;

    socketid = socket(AF_INET, SOCK_STREAM, 0);
    if (socketid < 0) {
        std::cout << "client socket creation error" << std::endl;
    }

    ptr = (int*) malloc(sizeof (int));
    *ptr = 1;

    if (setsockopt(socketid, SOL_SOCKET, SO_KEEPALIVE, (char*) ptr, sizeof (int)) < 0) {
        std::cout << "client set options error" << std::endl;
    }

    free(ptr);

    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    memset(&(client.sin_zero), 0, 8);
    client.sin_addr.s_addr = inet_addr(serveraddr);

    if (connect(socketid, (struct sockaddr*) &client, sizeof (client)) < 0) {
        std::cout << "error connecting socket" << std::endl;
    }

    boost::thread clientthread(wait_data); //in this case, clientsocket is the socket ON THE CLIENT that data
    //will be sent to from the server 

}

void wait_data() {

//    for (;;) {
//        char data[MAXDATASIZE];
//        int datalen;
//        if (recv(socketid, data, datalen, 0) < 0) {
//            cout << "client error recieving data" << endl;
//        } else { //handle the data
//            //         wait for data on socketid (global)
//            if (datalen < 1) return;
//            switch (data[0]) {
//                case 0: // Handshake
//                    handshake_t * handshake;
//                    handshake = new handshake_t(data, datalen);
//                    // display game ID
//                    break;
//                default:
//                    cout << "client Invalid packet recieved.\n";
//                    break;
//            }
//        }
//    }
}

// called like:
// send_data((void*)myhandshake, sizeof(handshake_t));

void send_data(void * data, int datalen) {
    // send data using socketid (global)
    if (send(socketid, data, datalen, 0) < 0) {
        cout << "client send data error" << endl;
    }


}
