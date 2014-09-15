#ifndef FBDEVICE_HPP_
#define FBDEVICE_HPP_

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <unistd.h>

#include <time.h>

#include <iostream>
#include <string>
#include <sstream>

#include <cstdlib>

// define font headers
// please changes the font name in display_str()
// according to the variable in in the fonts
// eg, if you use font font_8x8.hpp, you should use fontdata_8x8
//     if you use font font_pearl_8x8.hpp, you should use fontdata_pearl8x8

// pleae note to change name for both 8x8 & 8x16

#include "font_8x8.hpp"
#include "font_8x16.hpp"
//#include "font_sun8x16.hpp"
//#include "font_pearl_8x8.hpp"
//#include "font_acorn_8x8.hpp"
//#include "vincent.hpp"

//pacman animation
#include "pacman_32x32.hpp"

//super mario animation
#include "super_mario_16x16.hpp"


using namespace std;

class fbdevice
{
  public:
    
    //device parameter
    int device;
    int16_t *device_p;
    
    // current cursor position
    uint16_t index_x;
    uint16_t index_y;
    
    // temp colour
    uint16_t colour;
    
    //frame buffer parameter
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    
    //time parameter
    time_t rawtime;
    struct tm * timeinfo;
    
    uint32_t seconds;
    uint16_t seconds_colour;
    
    uint16_t font_colour;
    
    fbdevice() : device(0), device_p(0), index_x(0), index_y(0), colour(White){}
    
    ~fbdevice();
    
    void version();
    
    // initilize frame buffer
    void init();
    
    // set text index
    void set_index(uint16_t x, uint16_t y);

    // set colour RGB565 format, colour space 0-255
    void set_colour(uint8_t r, uint8_t g, uint8_t b);

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
    void set_colour_by_code(const uint8_t *colour_index);
    
    // drwa pixel on screen with tmp colour
    void draw_pixel(uint16_t x, uint16_t y);

    // fill screen with specific colour
    void fill_colour(uint8_t r, uint8_t g, uint8_t b);

    // fill whole screen with random colour
    void fill_random_colour();

    // convert an integer to string
    string to_string( uint32_t tmp );

    // display the charactor, scaling capable
    // mode can be 8 or 16, which decides the height of the font
    void display_font(uint8_t *chara, uint8_t size, uint8_t mode);

    // display the string, scaling capable
    void display_str(string tmp, uint8_t size, uint8_t mode);

    // paint the pacman on the screen, scaling capable
    // code = 4 means random colour
    void draw_pacman(uint8_t code, uint8_t size);

    // paint the mario on the screen, scaling capable
    void draw_mario(uint8_t code, uint8_t size);
    
    // paint the a rectangle
    // mode 1 is solid colour, mode 2 is just line
    void draw_rectangle(uint16_t x_tmp, uint16_t y_tmp, uint16_t w_tmp, uint16_t h_tmp, uint8_t mode);

    // show the flash effect on the seconds
    string flash_sec(uint32_t sec);
    
    // put time information on the screen
    void print_time(uint8_t mode);
    
    // display text on the screen below the clock area
    // if string is "random", then show random charactor
    void fill_text(string tmp, uint8_t mode);
    
    // display text on the screen below the clock area
    // if string is "random_text", then show random charactor
    void fill_mario(uint8_t code, uint8_t mode);
    
    // draw random rectangle on the screeen
    void random_rectangle();
    
    // display some text on the bottom of the screen
    void info();
    
    // refresh the framebuffer device
    void refresh(uint8_t mode);
    
    // this method draws the pac man animation
    void pacman_eat_line(uint16_t line, uint8_t mode);
    
    // this method draws the mario animation
    void mario_eat_line(uint16_t line, uint8_t mode);
    
    // use select to select which animation to show
    // 1 for pacman, 2 for super mario
    void draw_main(uint8_t mode, uint8_t select);
};

#endif
