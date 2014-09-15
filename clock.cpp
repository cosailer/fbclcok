/*
 * v1.00
 * clock, small clock program that runs on the DF3120
 * 
 * the source code is free, you can use it however you want.
 * but proceed at your own risk, I offer absolutely no warranty.
 * I am not responsible if this program burns down your house
 * or something, sorry :-).
 * 
 * the fonts used in this program are found under linux source
 * directory linux/drivers/video/console/ you can download these
 * fonts online, the copyright of these fonts belong to the
 * original author. 
 * 
 * Copyright (C) 2013 by cosailer <cosailer@gmail.com>
 */

#include "fbdevice.hpp"

#include <iostream>
#include <exception>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[])
{
    
    uint8_t mode = 8;
    uint8_t select = 1;
    
    fbdevice framebuffer;
    
    try
    {
        if( 3 != argc )
        {
            framebuffer.version();
            exit(1);
        }
        
        // set mode
        if( *argv[1] == '1' ) { mode = 8; }
    
        else if( *argv[1] == '2' ) { mode = 16; }
        
        else
        {
            cerr << ">> wrong mode ! exiting, mode = "<< mode << endl;
            framebuffer.version();
            exit(1);
        }
        
        // set animation
        if( *argv[2] == '1' ) { select = 1; }
        
        else if( *argv[2] == '2' ) { select = 2; }
        
        else
        {
            cerr << ">> wrong select ! exiting, select = " << select << endl;
            framebuffer.version();
            exit(1);
        }
        
        // init
        framebuffer.init();
        
        // start the clock
        framebuffer.draw_main(mode, select);
        
    }
    
    catch(exception& e)
    {
        cerr << "something is wrong, code = " << e.what() << endl;
        exit(1);
    }
    
    return 0;
  
}


