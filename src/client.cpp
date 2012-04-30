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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define log(a) m_log_buf->insert(m_log_buf->get_iter_at_offset(0), a);
Glib::RefPtr<Gtk::TextBuffer> m_log_buf;

#define SENDFLAGS 0
#define SERVADDR "blackcatstudios.org"

gamemode_t gamemode;
int socketid;
BPwin * bpwin;

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
    Gtk::Main kit(argc, argv);

    bpwin = new BPwin;

    // gtkmm main loop
    Gtk::Main::run(*bpwin);

    std::cout << "So long, and thanks for all the fish!\n";
    return 0;
}

BPwin::BPwin() {
    gamemode = GM_START;
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
        cout << "Something dun bust...\n";
        return;
    }

    int i, j;
    for (j = 0; j < BOARDSIZE; j++) { // for each column
        for (i = 0; i < BOARDSIZE; i++) { // for each in a column
            // create images

            // getcwd tips from http://stackoverflow.com/questions/143174/c-c-how-to-obtain-the-full-path-of-current-directory
            char path[FILENAME_MAX];

            getcwd(path, FILENAME_MAX);
            strcat(path, "/");
            strcat(path, M_IMG_EMPTY);
            m_img_set[0][j][i].set(path);
            m_img_set[0][j][i].set_padding(0, 0);
            getcwd(path, FILENAME_MAX);
            strcat(path, "/");
            strcat(path, M_IMG_HIT);
            m_img_set[1][j][i].set(path);
            m_img_set[1][j][i].set_padding(0, 0);
            if (!which) {
                getcwd(path, FILENAME_MAX);
                strcat(path, "/");
                strcat(path, M_IMG_SHIP);
                m_img_set[2][j][i].set(path);
                m_img_set[2][j][i].set_padding(0, 0);
            } else {
                getcwd(path, FILENAME_MAX);
                strcat(path, "/");
                strcat(path, M_IMG_MISS);
                m_img_set[2][j][i].set(path);
                m_img_set[2][j][i].set_padding(0, 0);
            }

            // assign image
            m_button[j][i].set_image_position(Gtk::POS_LEFT);
            m_button[j][i].set_image(m_img_set[0][j][i]);
            m_button[j][i].set_border_width(0);
            m_box_tile_column[j].pack_start(m_button[j][i]);
            if (which == 0) {
                m_button[j][i].signal_clicked().connect(
                        sigc::bind<int>(
                        sigc::mem_fun(that, &BPwin::tile_clicked_me)
                        , i * BOARDSIZE + j));
            } else {
                m_button[j][i].signal_clicked().connect(
                        sigc::bind<int>(
                        sigc::mem_fun(that, &BPwin::tile_clicked_opponent)
                        , i * BOARDSIZE + j));
            }
        }
    }
}

BPwin::~BPwin() {
}

void BPwin::tile_clicked_me(int btn_num) {
    location loc((btn_num % BOARDSIZE), (btn_num / BOARDSIZE));
    std::cout << "Clicked my " << (int) loc.x << "," << (int) loc.y << ", gm=" << gamemode << "\n";

    // prepare for sending move to server
    move_t * move;
    move = new move_t;
    move->action = ACT_PLACE;
    move->loc = loc;

    switch (gamemode) {
        case GM_START: // not connected, in a game, etc.: just started the app
            log("Please connect first!\n");
            break;
        case GM_WAIT_HANDSHAKE: // connected, no initial handshake received
            log("Server has not replied with a handshake!\n");
            break;
        case GM_WAIT_BOARD: // connected, no initial board received
            log("Server has not replied with the board!\n");
            break;
        case GM_SHIP1: // placing ship: see client.h typedef enum t_gamemode

            send(socketid, (void*) move, sizeof (move_t), 0);

            log("You placed a submarine!\n");
            log("Next, is another submarine, which is one block =)\n");

            gamemode = GM_SHIP2;
            break;
        case GM_SHIP2: // placing ship: see client.h typedef enum t_gamemode

            if (lboard.get_ship(0, loc)) {
                log("Dude. There's already a ship there. Try again.\n");
                break;
            } else {

                send(socketid, (void*) move, sizeof (move_t), 0);

                log("You placed a submarine!\n");
                log("Next, is a destroyer, which is two blocks =)\n");

                gamemode = GM_SHIP3;
            }
            break;
        case GM_SHIP3: // placing ship: see client.h typedef enum t_gamemode
            if (lboard.get_ship(0, loc)) {
                log("Dude. There's already a ship there. Try again.\n");
                break;
            }
            if (count == 0) {
                log("You placed destroyer block!\n");
                prev.x = (int) loc.x;
                prev.y = (int) loc.y;

                send(socketid, (void*) move, sizeof (move_t), 0);

                count++;
                break;

            } else if (count == 1) {
                loc.x = (int) loc.x;
                loc.y = (int) loc.y;

                if ((loc.x == prev.x && ((loc.y == (prev.y - 1)) || loc.y == (prev.y + 1))) || (loc.y == prev.y && ((loc.x == (prev.x - 1)) || loc.x == (prev.x + 1)))) {

                    log("You placed destroyer block !!!\n");
                    log("Next, is another destroyer, which is two blocks =)\n");

                    count = 0;
                    gamemode = GM_SHIP4;

                    send(socketid, (void*) move, sizeof (move_t), 0);

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

                prev.x = (int) loc.x;
                prev.y = (int) loc.y;

                send(socketid, (void*) move, sizeof (move_t), 0);

                count++;
                break;

            } else if (count == 1) {
                loc.x = (int) loc.x;
                loc.y = (int) loc.y;

                if ((loc.x == prev.x && ((loc.y == (prev.y - 1)) || loc.y == (prev.y + 1))) || (loc.y == prev.y && ((loc.x == (prev.x - 1)) || loc.x == (prev.x + 1)))) {

                    log("You placed destroyer block!\n");
                    log("Next, is a cruiser, which is three blocks =)\n");

                    count = 0;
                    gamemode = GM_SHIP5;

                    send(socketid, (void*) move, sizeof (move_t), 0);

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
                prev.x = (int) loc.x;
                prev.y = (int) loc.y;

                send(socketid, (void*) move, sizeof (move_t), 0);

                count++;
                break;

            } else if (count == 1 || count == 2) {
                loc.x = (int) loc.x;
                loc.y = (int) loc.y;

                if ((loc.x == prev.x && ((loc.y == (prev.y - 1)) || loc.y == (prev.y + 1))) || (loc.y == prev.y && ((loc.x == (prev.x - 1)) || loc.x == (prev.x + 1)))) {
                    log("You placed a cruiser block!!!\n");
                    prev.x = (int) loc.x;
                    prev.y = (int) loc.y;

                    send(socketid, (void*) move, sizeof (move_t), 0);

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
                prev.x = (int) loc.x;
                prev.y = (int) loc.y;

                send(socketid, (void*) move, sizeof (move_t), 0);

                count++;
                break;

            } else if (count == 1 || count == 2 || count == 3) {
                loc.x = (int) loc.x;
                loc.y = (int) loc.y;

                if ((loc.x == prev.x && ((loc.y == (prev.y - 1)) || loc.y == (prev.y + 1))) || (loc.y == prev.y && ((loc.x == (prev.x - 1)) || loc.x == (prev.x + 1)))) {
                    log("You placed a battleship block!!\n");
                    prev.x = (int) loc.x;
                    prev.y = (int) loc.y;

                    send(socketid, (void*) move, sizeof (move_t), 0);

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
                prev.x = (int) loc.x;
                prev.y = (int) loc.y;

                send(socketid, (void*) move, sizeof (move_t), 0);

                count++;
                break;

            } else if (count == 1 || count == 2 || count == 3 || count == 4) {
                loc.x = (int) loc.x;
                loc.y = (int) loc.y;

                if ((loc.x == prev.x && ((loc.y == (prev.y - 1)) || loc.y == (prev.y + 1))) || (loc.y == prev.y && ((loc.x == (prev.x - 1)) || loc.x == (prev.x + 1)))) {
                    log("You placed a carrier block!!\n");
                    prev.x = (int) loc.x;
                    prev.y = (int) loc.y;

                    send(socketid, (void*) move, sizeof (move_t), 0);

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
            log("Eek! Shoot at the ENEMY's board!\n");
            break;
        default: // should never get here
            log("What just happened?!\n");
            break;
    }
    delete move;
}

void BPwin::tile_clicked_opponent(int btn_num) {
    location loc((btn_num % BOARDSIZE), (btn_num / BOARDSIZE));
    std::cout << "Clicked enemy " << (int) loc.x << "," << (int) loc.y << ", gm=" << gamemode << "\n";

    // prepare for sending move to server
    move_t * move;
    move = new move_t;
    move->action = ACT_MOVE;
    move->loc = loc;

    switch (gamemode) {
        case GM_PLAYTIME:
            if (lboard.get_fired(0, loc)) {
                log("We've already hit those coordinates!\n");
                break;
            }
            send(socketid, (void*) move, sizeof (move_t), 0);
            break;
        default:
            log("Finish connecting and placing your ships first!\n");
            break;
    }

    delete move;
}

/* Called when a key is pressed in the chat input box
 * return value is apparently whether or not to show the char in the box?
 * possibly whether or not the event has been handled - seems to spam this when false.
 */
bool BPwin::chat_key_press(GdkEventKey * k) {
    //std::cout << "chat box key press: " << k->keyval << "=" << (char)k->keyval << "\n";
    if (k->keyval == 65293) { // return
        if (m_entry.get_text_length() < 1) {
            return true;
        }

        //log
        //        log("\n");
        //        log(m_entry.get_text());
        //log("Me: ");
        //send
        // Send chat to server
        chat_t * chatmsg;
        chatmsg = new chat_t;
        string chat = m_entry.get_text();
        size_t c_length;
        c_length = chat.copy(chatmsg->msg, 255, 0);

        std::cout << "Send chat: " << m_entry.get_text() << std::endl;
        send(socketid, chatmsg, sizeof (chat_t), SENDFLAGS);
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
    m_game.set_text("new game");

    show_all_children();
}

BPwin::Connwin::~Connwin() {
}

void BPwin::Connwin::do_connect() {
    if (m_user.get_text_length() <= 0) {
        log("You need to enter a username!\n");
        return;
    }
    if (m_game.get_text_length() <= 0) {
        log("You need to enter game ID! Use \"new game\" to start a new game.\n");
        return;
    }
    cout << "Connecting as user " << m_user.get_text() << " to game " << m_game.get_text() << endl;
    connect();

    handshake_t * handshake_pkt;
    handshake_pkt = new handshake_t; //create an instance of the handshake_t structure

    string username = m_user.get_text();
    string gameid = m_game.get_text();
    /*set all the elements for the handshake packet*/
    //    strcpy(handshake_pkt.username, username);
    size_t u_length;
    size_t g_length;
    u_length = username.copy(handshake_pkt->username, 16, 0);
    g_length = gameid.copy(handshake_pkt->gameid, 32, 0);
    //    cout << handshake_pkt->username << endl;
    //    cout << handshake_pkt->gameid << endl;
    //    handshake_pkt->username = m_user.get_text();
    //    handshake_pkt->gameid = m_game.get_text();
    handshake_pkt->boardsize = BOARDSIZE;
    handshake_pkt->protover = PROTOVERSION;
    int test;
    test = send(socketid, (void *) handshake_pkt, sizeof (handshake_t), 0);
    if (test < 0) {
        cout << "do_connect: send error: handshake: result " << test
                << " should be " << sizeof (handshake_t) << ", error " << errno << endl;
    }

    gtk_main_quit();

    gamemode = GM_WAIT_HANDSHAKE;
    log("Waiting for server response\n");
    cout << "Connected, wait handshake\n";
}

void connect() {
    char serveraddr[] = SERVADDR;
    int port = SERVPORT;
    // connect, spawn wait_data with socketid
    struct sockaddr_in client;
    int * ptr;

    socketid = socket(AF_INET, SOCK_STREAM, 0);
    if (socketid < 0) {
        std::cout << "client socket creation error" << std::endl;
    }

    ptr = (int*) malloc(sizeof (int));
    *ptr = 1;

    if (setsockopt(socketid, SOL_SOCKET, SO_KEEPALIVE + TCP_NODELAY, (char*) ptr, sizeof (int)) < 0) {
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

    boost::thread clientthread(wait_data);
}

void wait_data() {
    for (;;) {
        char data[MAXDATASIZE];
        int recvd = recv(socketid, data, MAXDATASIZE, 0);
        if (recvd < 0) {
            std::cout << "server error receiving data: recv ret " << recvd << std::endl;
            return;
        }
        std::cout << "wait_data recv socket " << socketid << " data ";
        // dump all recieved data
        //        for (int i = 0; i < recvd; i++) {
        //            std::cout << (int) data[i] << data[i] << ",";
        //        }
        //        std::cout << "\n";
        if (recvd < 1) {
            std::cout << "Tripping return because of data length" << std::endl;
            log("Disconnected from server!\n");
            return;
        }
        switch (data[0]) { //determine what kind of packet this is
            case 0: // Handshake
            {
                handshake_t * handshake;
                handshake = new handshake_t(data, recvd);
                log("\n");
                log(handshake->gameid);
                log("You are in game ID "); // TODO: allow packet imports to fail
                cout << "Now in game " << handshake->gameid << "\n";

                // request complete board from server
                refresh_t * refresh;
                refresh = new refresh_t;
                send(socketid, (const void *) refresh, sizeof (refresh_t), 0);

                cout << "Got handshake, wait for board.\n";
                gamemode = GM_WAIT_BOARD;
                break;
            }
            case 1: //move
            {
                move_t * move;
                move = new move_t(data, recvd); //populates move with the info in data sent from the server

                //update the game stuff depending on the action type that is being sent from the server
                switch (move->action) {
                    case ACT_MOVE:
                        std::cout << "Client shouldn't be getting this, moves are sent client->server.\n";
                        break;
                    case ACT_PLACE: //the place you sent to the server was a valid move, place a piece at move.loc
                        bpwin->lboard.set_ship(0, move->loc);
                        bpwin->set_tile(0, 2, move->loc);
                        // TODO: move meg's code here?
                        break;
                    case YOU_HIT: //the move you sent to the server was a hit, update at move.loc
                        log("You hit!\n");
                        bpwin->lboard.set_fired(0, move->loc);
                        bpwin->set_tile(1, 1, move->loc);
                        break;
                    case YOU_MISS: //the move you sent to the server was a miss, update at move.loc
                        log("You missed.\n");
                        bpwin->lboard.set_fired(0, move->loc);
                        bpwin->set_tile(1, 2, move->loc);
                        break;
                    case THEY_FIRED: //the enemy fired at you, determine if this was a hit and update YOUR board only,
                        if (bpwin->lboard.get_ship(0, move->loc)) {
                            log("Enemy hit!\n");
                            bpwin->lboard.set_fired(1, move->loc);
                            bpwin->set_tile(0, 1, move->loc);
                        } else {
                            log("Enemy missed!\n");
                            bpwin->lboard.set_fired(1, move->loc);
                            // no vboard update: no my board missed tile
                        }
                        break; //the server already told the enemy if they hit
                        // no default: we covered all cases
                }
                break;
            }
            case 2: //update entire board
            {
                refresh_t * update;
                update = new refresh_t(data, recvd);
                int x, y;
                location locb;
                for (x = 0; x < BOARDSIZE; x++) { //iterate through x
                    for (y = 0; y < BOARDSIZE; y++) { //iterate through y
                        cout << "Setting tile/board at " << x << "," << y << "\n";
                        locb.set(x, y);
                        bpwin->lboard.set_tile_raw(locb, update->board.get_tile_raw(locb));
                        // enemy's board
                        switch (bpwin->lboard.get_fired(0, locb)) { // i've fired
                            case true:
                                switch (bpwin->lboard.get_ship(1, locb)) {
                                    case true: // i've hit
                                        bpwin->set_tile(1, 1, locb);
                                        break;
                                    case false: // i've missed
                                        bpwin->set_tile(1, 2, locb);
                                        break;
                                }
                                break;
                            case false: // I haven't fired: show ocean/empty
                                if (bpwin->lboard.get_ship(1, locb)) {
                                    cout << "SERVER SHOULD NOT SEND ENEMY SHIP STATES UNLESS I'VE FIRED ON THEM\n";
                                }
                                bpwin->set_tile(1, 0, locb);
                                break;
                        }
                        // my board
                        switch (bpwin->lboard.get_ship(0, locb)) { // I have a ship
                            case true:
                                switch (bpwin->lboard.get_fired(1, locb)) {
                                    case true: // they fired / hit
                                        bpwin->set_tile(0, 1, locb);
                                        break;
                                    case false: // they haven't fired, but I have a ship
                                        bpwin->set_tile(0, 2, locb);
                                        break;
                                }
                                break;
                            case false: // I have no ship // for now we're showing ocean instead of misses
                                bpwin->set_tile(0, 0, locb);
                                break;
                        }
                    }
                }
                cout << "Got handshake, place ships now.\n";
                gamemode = GM_SHIP1;
                // TODO: set gamemode to recieved one
                break;
            }
            case 3: //chat
            {
                chat_t * chatmsg; //created a new chat msg
                chatmsg = new chat_t(data, recvd);

                cout << "Got chat: " << chatmsg->msg << endl;

                log("\n");
                log(chatmsg->msg);
                break;
            }
            default:
            {
                cout << "client Invalid packet received.\n";
                break;
            }
        }
    }
}

void BPwin::set_tile(uint8_t boarda, uint8_t statea, location loca) {
    if (boarda > 1 || statea > 2 || loca.x >= BOARDSIZE || loca.y >= BOARDSIZE) {
        cout << "set_tile invalid param: " << (int) boarda << " " << (int) statea << " " << (int) loca.x << " " << (int) loca.y << "\n";
        return;
    }
    // states: (from m_img_set)
    // the 3: 0=ocean/empty,1=hit,2=(my_board?ship:miss)
    Gtk::Image * img = &boards[boarda].m_img_set[statea][loca.x][loca.y];
    Gtk::Button * btn = &boards[boarda].m_button[loca.x][loca.y];
    img->show();
    btn->show();
    usleep(10000);
    btn->set_image(*img);
}
