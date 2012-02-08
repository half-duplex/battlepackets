/**
 * client.cpp
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

    // Create GTK gui

    // spawn net listener. join it, for now?
    boost::thread netin(netrecv, nethandler);


    netin.interrupt();
    return 0;
}

void nethandler(char * data[], int datalen) {
    // grab first char (packet id)
    // switch(packetID){etc}
    // do stuff like update the gui
}