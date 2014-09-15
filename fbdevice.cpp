/*
 * v1.00
 * fbdevice, this is a small wrap libaray to display fonts and
 * pacman/mario animations on framebuffer fevices
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

#include <iostream>
#include "fbdevice.hpp"

using namespace std;

fbdevice::~fbdevice()
{
    //clean up
    cout << ">> good bye !" << endl;
    munmap(device_p, finfo.smem_len);
    close(device);
}

void fbdevice::version()
{
    cout << "\n>> DF3120 FrameBuffer Clock Utility v1.1" << endl;
    cout << ">> by CoSailer    10.03.2014" << endl;
    cout << ">> Usage: clock [mode] [select]" << endl;
    cout << "          mode : 1  use 8x8 font to show time" << endl;
    cout << "          mode : 2  use 8x16 font to show time\n" << endl;
    cout << "          select : 1  use pacman animation" << endl;
    cout << "          select : 2  use super mario animation\n" << endl;
}

// initilize frame buffer
void fbdevice::init()
{
    device = open("/dev/fb0", O_RDWR);
    
    if (!device)
    {
        cout << ">> open framebuffer device failed !" << endl;
        _exit(1);
    }
    else cout << "\n>> /dev/fb0 opened successfully !" << endl;
    
    // Get variable screen information
    if (ioctl(device, FBIOGET_VSCREENINFO, &vinfo))
    {
        cout << ">> failed reading variable information !" << endl;
        _exit(1);
    }
    else
    {
        cout << ">> LCD resolution: " << vinfo.xres << "x" << vinfo.yres << ", colour: ";
        cout << vinfo.bits_per_pixel << " bpp" <<  endl;
    }
    
    // Get fixed screen information
    if (ioctl(device, FBIOGET_FSCREENINFO, &finfo))
    {
        cout << ">> failed reading fixed information !" << endl;
        _exit(1);
    }
    
    else cout << ">> frame buffer size : " << finfo.smem_len << " bytes" << endl;
    
    // map framebuffer to user accessible memory
    device_p = (int16_t *)mmap(0,  finfo.smem_len,  PROT_READ | PROT_WRITE, MAP_SHARED, device, 0);
    
    if ((int64_t)device_p == -1)
    {
        cout << ">> failed to mmap() !" << endl;
        _exit(1);
    }
    else  cout << ">> mmap() OK !\n" << endl;
    
    //clear screen
    fill_colour(0, 0, 0);
    
}

// set text index
void fbdevice::set_index(uint16_t x, uint16_t y)
{
    index_x = x;
    index_y = y;
}

// set colour RGB565 format, colour space 0-255
void fbdevice::set_colour(uint8_t r, uint8_t g, uint8_t b)
{
    colour = ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);
}

// set colour by charactor definition
// colour definition:
// . : black
// W : white
// R : red
// B : blue
// b : brown
// O : orange
// G : green
// g : gray
// Y : yellow
// p : pink
void fbdevice::set_colour_by_code(const uint8_t *colour_index)
{
    // set the pixel colour for each dot
    if ( *colour_index == '.' )  {  set_colour(0, 0, 0);  }
    else if ( *colour_index == 'W' ){  set_colour(255, 255, 255);  }
    else if ( *colour_index == 'R' ){  set_colour(255, 0, 0);  }
    else if ( *colour_index == 'B' ){  set_colour(0, 0, 255);  }
    else if ( *colour_index == 'b' ){  set_colour(150, 75, 0);  }
    else if ( *colour_index == 'O' ){  set_colour(255, 125, 0);  }
    else if ( *colour_index == 'G' ){  set_colour(0, 255, 0);  }
    else if ( *colour_index == 'g' ){  set_colour(100, 100, 100);  }
    else if ( *colour_index == 'Y' ){  set_colour(255, 255, 0);  }
    else if ( *colour_index == 'p' ){  set_colour(255, 180, 200);  }
}

// drwa pixel on screen with tmp colour
void fbdevice::draw_pixel(uint16_t x, uint16_t y)
{
    // calculate the pixel's byte offset inside the buffer
    if( x > 320) {  x -= 320; }
    if( y > 240) {  y -= 240; }
    
    uint32_t pix_offset = x + y * vinfo.xres;
    device_p[pix_offset] = colour;
}

// fill screen with specific colour
void fbdevice::fill_colour(uint8_t r, uint8_t g, uint8_t b)
{
    set_colour(r, g, b);
    
    for (uint16_t y = 0; y < vinfo.yres; ++y)
    {
        for (uint16_t x = 0; x < vinfo.xres; ++x)
        {
            draw_pixel(x, y);
        }
    }
}

// fill whole screen with random colour
void fbdevice::fill_random_colour()
{
    for (uint16_t y = 0; y < vinfo.yres; ++y)
    {
        for (uint16_t x = 0; x < vinfo.xres; ++x)
        {
            set_colour( rand()%256, rand()%256, rand()%256);
            draw_pixel(x, y);
        }
    }
}

// convert an integer to string
string fbdevice::to_string( uint32_t tmp )
{
    ostringstream stm ;
    stm << tmp ;
    if(tmp < 10) return ("0" + stm.str());
    else return stm.str();
}

// display the charactor, scaling capable
// mode can be 8 or 16, which decides the height of the font
void fbdevice::display_font(uint8_t *chara, uint8_t size, uint8_t mode)
{
    uint16_t width = 8 * size;
    uint16_t* buffer_ptr = (uint16_t*)device_p;
    
    // Move screen ptr to the current index point
    buffer_ptr += (index_x + vinfo.xres * index_y);
    
    for( uint8_t i = 0; i < mode; ++i )
    {
        for(uint8_t k = 0; k < size; ++k)
        {
          for( uint8_t j = 0; j < 8; ++j )
          {
              for(uint8_t l = 0; l < size; ++l)
              {
                if(  chara[i] & (0x80>>j)  )  {  *buffer_ptr = colour; }
                
                else { *buffer_ptr = 0; }
                
                buffer_ptr++;
              }
          }
          
          buffer_ptr -= width;
          buffer_ptr += vinfo.xres;  // onto the next line
        }
    }
    
    // ready for the next charactor
    set_index( index_x + width, index_y);
    
    // move to the next line if current line full
    if( index_x > ( vinfo.xres - 8*size ) )
    {
        set_index( 0, index_y + 8*size);
    }
    
    // move to the start position if screen full
    if( index_y > ( vinfo.yres - mode*size ) )
    {
        set_index(0, 0);
    }
}

// display the string, scaling capable
void fbdevice::display_str(string tmp, uint8_t size, uint8_t mode)
{
    uint64_t offset = 0;
    uint8_t *font_index;
    uint8_t scal = 1;
    
    // if mode = 8, max scal = 30; if mode = 16, max scal = 15
    if( size > (240/mode) )
    {
        cout << "scal out of range, using default 1" << endl;
    }
    else { scal = size; }
    
    for (uint32_t i = 0; i < tmp.length(); ++i)
    {
        //calculate char offset in font
        offset = tmp.at(i) * mode;
        
        if( mode == 8 )  {  font_index = (uint8_t *)fontdata_8x8 + offset; }
        else if( mode == 16 ) { font_index = (uint8_t *)fontdata_8x16 + offset; }
        else { cout << "Wrong text mode !" << endl; return ;}
        
        display_font( font_index, scal, mode);
    }
}
    
// paint the pacman on the screen, scaling capable
// code = 4 means random colour
void fbdevice::draw_pacman(uint8_t code, uint8_t size)
{
    const uint8_t* font_ptr;
    font_ptr = pacman + code * PACMAN_SIZE * PACMAN_SIZE;
    uint16_t width = PACMAN_SIZE * size;
    uint16_t* buffer_ptr = (uint16_t*)device_p;
    
    // Move screen ptr to top-left of character
    buffer_ptr += (index_x + vinfo.xres*index_y);
    
    for (uint8_t y = 0; y < PACMAN_SIZE; ++y)
    {
        //print multiple times of the same line of pacman
        for(uint8_t k = 0; k < size; ++k)
        {
          //print one line of pacman
          for (uint8_t x = 0; x < PACMAN_SIZE; ++x)
          {
              for(uint8_t l = 0; l < size; ++l)
              {
                  set_colour_by_code(font_ptr); 
                  *buffer_ptr = colour;
                  
                  buffer_ptr++; 
              }
              
              font_ptr++;
          }
          
          // need to rewind the font ptr if size > 1, and more than one loop
          // this is only needed for customized image
          if( k < size - 1 ) {  font_ptr -= PACMAN_SIZE;  }
          
          // move to the next line for drawing
          buffer_ptr -= width;
          buffer_ptr += vinfo.xres;
        }
    }
}

// paint the mario on the screen, scaling capable
void fbdevice::draw_mario(uint8_t code, uint8_t size)
{
    const uint8_t* font_ptr;
    font_ptr = mario + code * MARIO_SIZE * MARIO_SIZE;
    uint16_t width = MARIO_SIZE * size;
    uint16_t* buffer_ptr = (uint16_t*)device_p;
    
    // Move screen ptr to top-left of character
    buffer_ptr += (index_x + vinfo.xres*index_y);
    
    for (uint8_t y = 0; y < MARIO_SIZE; ++y)
    {
        //print multiple times of the same line of pacman
        for(uint8_t k = 0; k < size; ++k)
        {
          //print one line of pacman
          for (uint8_t x = 0; x < MARIO_SIZE; ++x)
          {
              for(uint8_t l = 0; l < size; ++l)
              {
                  set_colour_by_code(font_ptr); 
                  *buffer_ptr = colour;
                  
                  buffer_ptr++;
              }
              
              font_ptr++;
          }
          
          // need to rewind the font ptr if size > 1, and more than one loop
          // this is only needed for customized image
          if( k < size - 1 ) {  font_ptr -= MARIO_SIZE;  }
          
          // move to the next line for drawing
          buffer_ptr -= width;
          buffer_ptr += vinfo.xres;
        }
    }
}

// paint the a rectangle
// mode 1 is solid colour, mode 2 is just line
void fbdevice::draw_rectangle(uint16_t x_tmp, uint16_t y_tmp, uint16_t w_tmp, uint16_t h_tmp, uint8_t mode)
{
    if(1 == mode)
    {
        for(uint16_t x = x_tmp; x < x_tmp + w_tmp; ++x)
        {
            for(uint16_t y = y_tmp; y < y_tmp + h_tmp; ++y)
            {
                draw_pixel(x, y);
            }
        }
    }
    
    else if(2 == mode)
    {
        for(uint16_t x = x_tmp; x <= x_tmp + w_tmp; ++x)
        {
            draw_pixel(x, y_tmp);
            draw_pixel(x, y_tmp + h_tmp);
        }
        
        for(uint16_t y = y_tmp; y <= y_tmp + h_tmp; ++y)
        {
            draw_pixel(x_tmp, y);
            draw_pixel(x_tmp + w_tmp, y);
        }
    }
}

string fbdevice::flash_sec(uint32_t sec)
{
    if( seconds != sec)
    {
        set_colour( rand()%256, rand()%256, rand()%256);
        seconds_colour = colour;
        seconds = sec;
    }
    
    // choose which animation
    switch( sec%4 )
    {
      case 0 :
        return string(1, 24);
      case 1 :
        return string(1, 26);
      case 2 :
        return string(1, 25);
      case 3 :
        return string(1, 27);
    
      default :
        return string(1, 0);
    }
}

// put time information on the screen
void fbdevice::print_time(uint8_t mode)
{
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    
    // print year
    set_index(0, 0);
    
    set_colour(255,0,0);
    display_str( to_string(timeinfo->tm_year + 1900), 4, mode);
    
    // hyphon
    set_colour(255,255,255);
    display_str( "-", 4, mode);
    
    // print month
    set_colour(0,255,0);
    display_str( to_string(timeinfo->tm_mon + 1), 4, mode);
    
    // hyphon
    set_colour(255,255,255);
    display_str( "-", 4, mode);
    
    // print day
    set_colour(0,128,255);
    display_str( to_string(timeinfo->tm_mday), 4, mode);
    
    // print hours
    set_index(0, mode*4);
    
    set_colour(0,255,0);
    display_str( to_string( timeinfo->tm_hour ), 8, mode);
    
    // flash seconds
    colour = seconds_colour;
    display_str( flash_sec(timeinfo->tm_sec), 8, mode);
    
    // print min
    set_colour(255,255,0);
    display_str( to_string(timeinfo->tm_min), 8, mode);
    
}

// display text on the screen below the clock area
// if string is "random", then show random charactor
void fbdevice::fill_text(string tmp, uint8_t mode)
{
    set_index(0, 12*mode);
    
    // the number of text can be showed under the clock,
    // 40 for mode 8 and 10 for mode 16
    uint16_t text_num = 0;
    
    if(mode == 8) {  text_num = 40; }
    else if (mode == 16) {  text_num = 10; }
    
    set_colour(255,0,0);
    
    if( !tmp.compare("random") )
    {
        for(int i = 0; i < text_num; ++i)
        {
            if( 0 != rand()%5)
            {
                set_colour(255,0,0);
                display_str( string(1, rand()%256), 4, 8);
            }
            
            else
            {
                draw_mario( rand()%26, 2 );
                
                // ready for the next charactor
                set_index( index_x + 32, index_y);
                
                // move to the next line if current line full
                if( index_x > ( vinfo.xres - 32 ) )  { set_index( 0, index_y + 32); }
            }
        }
    }
    
    // fill the text area with the string provided as much as possible
    else
    {
        while( tmp.length() < 40 )
        {  tmp = tmp + tmp;  }
        
        if( tmp.length() > 40 ) { tmp = tmp.substr(0,40); }
        
        display_str( tmp, 4, 8);
    }
}

// display text on the screen below the clock area
// if string is "random_text", then show random charactor
void fbdevice::fill_mario(uint8_t code, uint8_t mode)
{
    set_index(0, 12*mode);
    
    // the number of text can be showed under the clock,
    // 40 for mode 8 and 10 for mode 16
    uint16_t text_num = 0;
    
    if(mode == 8) {  text_num = 40; }
    else if (mode == 16) {  text_num = 10; }
    
    set_colour(255,0,0);
    
    if( 255 == code )
    {
        for(int i = 0; i < text_num; ++i)
        {   
            draw_mario( rand()%26, 2 );
            
            // ready for the next charactor
            set_index( index_x + 32, index_y);
            
            // move to the next line if current line full
            if( index_x > ( vinfo.xres - 32 ) )  { set_index( 0, index_y + 32); } 
        }
    }
    
    // fill the text area with the string provided as much as possible
    else
    {
        for(int i = 0; i < text_num; ++i)
        {   
            draw_mario( code, 2 );
            
            // ready for the next charactor
            set_index( index_x + 32, index_y);
            
            // move to the next line if current line full
            if( index_x > ( vinfo.xres - 32 ) )  { set_index( 0, index_y + 32); }
        }
    }
}

void fbdevice::random_rectangle()
{
    while(true)
    {
        set_colour( rand()%256, rand()%256, rand()%256);
        
        draw_rectangle(rand()%320, rand()%240, rand()%150, rand()%100, 2);
        
        usleep(100000);
    }
}

// display some version info
void fbdevice::info()
{
    // print some other text
    set_index(0, 224);
    set_colour(255,255,255);
    display_str( "         Happy everyday !! " + string(1, 1) + "    ", 1, 8);
    
    set_index(0, 232);
    display_str( "DF3120 fbclock    by CoSailer 10.03.2014", 1, 8);
}

// refresh the framebuffer device
void fbdevice::refresh(uint8_t mode)
{
    print_time(mode);
    info();
}

// this method draws the pac man animation
void fbdevice::pacman_eat_line(uint16_t line, uint8_t mode)
{
    if( 0 == (line/PACMAN_SIZE)%2 )
    {
        set_index( 0, line );
        
        for(uint16_t i = 0; i < vinfo.xres-PACMAN_SIZE; ++i)
        {
            if( (i%6)<3 ) { draw_pacman(0, 1); }
            else { draw_pacman(1, 1); }
            
            usleep(40000);
            refresh(mode);
            set_index(i, line);
        }
    }
    
    else
    {
        set_index( vinfo.xres - PACMAN_SIZE, line );
        
        for(uint16_t i = vinfo.xres - PACMAN_SIZE; i > 0 ; --i)
        {
            if( (i%6)<3 ) { draw_pacman(2, 1); }
            else { draw_pacman(3, 1); }
            
            usleep(40000);
            refresh(mode);
            set_index(i, line);
        }
    }
}

// this method draws the mario animation
void fbdevice::mario_eat_line(uint16_t line, uint8_t mode)
{
    if( 0 == (line/32)%2 )
    {
        set_index( 0, line );
        
        for(uint16_t i = 0; i < vinfo.xres-32; ++i)
        {
            if( (i%12)<3 ) { draw_mario(26, 2); }
            else if( (i%12)<6 ) { draw_mario(27, 2); }
            else if( (i%12)<9 ) { draw_mario(26, 2); }
            else { draw_mario(28, 2); }
            
            usleep(40000);
            refresh(mode);
            set_index(i, line);
        }
    }
    
    else
    {
        set_index( vinfo.xres - 32, line );
        
        for(uint16_t i = vinfo.xres - 32; i > 0 ; --i)
        {
            if( (i%12)<3 ) { draw_mario(29, 2); }
            else if( (i%12)<6 ) { draw_mario(30, 2); }
            else if( (i%12)<9 ) { draw_mario(29, 2); }
            else { draw_mario(31, 2); }
            
            usleep(40000);
            refresh(mode);
            set_index(i, line);
        }
    }
}
    
    
// use select to select which animation to show
// 1 for pacman, 2 for super mario
void fbdevice::draw_main(uint8_t mode, uint8_t select)
{
    uint16_t line_index = 12 * mode;
    
    //fill_text(string(1, 219), mode);
    fill_text("**********  Hello !  fbclock  Started !!", mode);
    
    while(true)
    { 
        if( 1 == select ) { pacman_eat_line(line_index, mode); }
        else if ( 2 == select ) { mario_eat_line(line_index, mode); }
        
        display_str( " ", 4, 8);
        line_index += 32;
        
        if(line_index > 208)
        {
            line_index = 12 * mode;
            fill_text("random", mode);
        }
    }
}
