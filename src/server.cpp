/**
 * server.cpp
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

#include <iostream>
#include <boost/thread.hpp>
#include "server.h"
#include "common.h"
#include "net/net.h"
#include "sys/socket.h"
#define DEBUG

using namespace std;

/*
 * Main server function
 * Arguments:
 *      int argc
 *              the number of arguments on the command line
 *      int argv
 *              the text of the arguments
 */

void serverhandler(int socket) {
    
    while(1) {
        //wait for data, then when it gets some process it
    }
}

int main_server(int argc, char** argv) {
#ifdef DEBUG
    if (argc != 2) {
        cout << "Arguments:\n";
        for (int i = 0; i < argc; i++) {
            cout << argv[i] << endl;
        }
    }
#endif
 
   

    

    // Set up shared stuff
    // Launch first net listener

    boost::thread listen(netlisten, '7777'); //"bind" problem thread
                                                                //serverhandler is passed as the callback
    listen.join();
    
    

    
    



    //boost::thread nameOfThreadInstance(functionToRun, firstArgument, secondArgument, callbackFunction, etc);




    // Do something so that it doesn't quit until the last thread is gone?
    // semaphore, maybe?

    return 0;
}

