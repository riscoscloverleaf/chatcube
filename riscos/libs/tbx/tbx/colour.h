/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010 Alan Buckley   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#ifndef tbx_colour_H
#define tbx_colour_H

namespace tbx
{

/**
 * Class to represent a RGB colour.
 *
 * The colour is stored as an unsigned integer
 * in the format 0xbbggrrxx
 *   where xx is normally 0
 *         rr is the red component
 *         gg is the green component
 *         bb is the blue component.
 * All components set to 0xFF (255) is used to represent no colour.
 */
class Colour
{
  union {
    unsigned _colour;
    struct { char x, r, g, b; } _k;
  };

public:
  /**
   * Default constructor (creates black)
   */
  Colour(): _colour(black) {}
  /**
   * Construct from a packed colour
   *
   * @param c colour in format 0xbbggrrxx
   */
  Colour(unsigned c): _colour(c) {}
  /**
   * Construct colour from rgb components.
   *
   * Each component consists of a value from 0 to 255 with 0
   * being none and 255 being full.
   *
   * @param red amount of red 0 to 255
   * @param green amount of green 0 to 255
   * @param blue amount of blue 0 to 255
   */
  Colour(int red, int green, int blue) {_k.r=red;_k.g=green;_k.b=blue;_k.x=0;}

  /**
   * Change the red component of the colour
   *
   * @param r amount of red from 0 to 255
   */
  void red(int r) { _k.r = r; }
  /**
   * Change the green component of the colour
   *
   * @param g amount of green from 0 to 255
   */
  void green(int g) { _k.g = g; }
  /**
   * Change the blue component of the colour
   *
   * @param b amount of blue from 0 to 255
   */
  void blue(int b) { _k.b = b; }

  /**
   * Return the amount of red in the colour
   *
   * @returns amount of red from 0 to 255
   */
  int red() const { return _k.r; }
  /**
   * Return the amount of green in the colour
   *
   * @returns amount of green from 0 to 255
   */
  int green() const { return _k.g; }
  /**
   * Return the amount of blue in the colour
   *
   * @returns amount of blue from 0 to 255
   */
  int blue() const { return _k.b; }

  /**
   * Return the colours packed into an unsigned integer
   *
   * @returns the colour in the form 0xbbggrrxx
   */
  operator unsigned() const { return _colour; }
  /**
   * Set the colour from an unsigned integer
   *
   * @param c colour in the format 0xbbggrrxx
   * @returns a reference to this colour
   */
  Colour &operator=(unsigned c) { _colour = c; return *this; }

  /**
   * Enumeration of some common colours
   */
  enum { black = 0x00000000, white = 0xFFFFFF00,
	     light_red = 0x0000FF00, light_green = 0x00FF0000, light_blue = 0xFF000000,
	     magenta = 0xFFFF0000, yellow = 0x00FFFF00, cyan =  0xFFFF0000,
	     // Wimp colour equivalents
	     wimp_grey0 = 0xFFFFFF00, wimp_grey1 = 0xDDDDDD00, wimp_grey2 = 0xBBBBBB00,
	     wimp_grey3 = 0x99999900, wimp_grey4 = 0x77777700, wimp_grey5 = 0x55555500,
	     wimp_grey6 = 0x33333300, wimp_grey7 = 0x00000000,
	     wimp_dark_blue = 0x99440000, wimp_yellow = 0x00EEEE00,
	     wimp_light_green = 0x00CC0000, wimp_red = 0x0000DD00,
	     wimp_cream = 0xBBEEEE00,  wimp_dark_green = 0x00885500,
	     wimp_orange = 0x00BBFF00, wimp_light_blue = 0xFFBB0000,
	     no_colour = 0xFFFFFFFF };
};

/**
 * Class to represent a standard desktop WIMP colour.
 * This is an integer from 0 to 15 or -1 for no colour.
 */
class WimpColour
{
  int _colour;

public:
  /**
   * Constructor creates black WIMP colour
   */
  WimpColour(): _colour(black) {}
  /**
   * Construct from an integer colour number
   *
   * @param c colour number -1 to 15.
   */
  WimpColour(int c): _colour(c) {}

  /**
   * Return colour number
   *
   * @returns colour number from -1 to 15
   */
  operator int() const { return _colour; }

  /**
   * Set colour number
   *
   * @param c colour number from -1 to 15
   * @returns reference to this object
   */
  WimpColour &operator=(int c) { _colour = c; return *this; }

  /**
   * Enumeration specifying all the possible WIMP colours.
   */
  enum { white = 0, grey0 = 0, grey1 = 1, grey2 = 2, grey3 = 3,
	 grey4 = 4, grey5 = 5, grey6 = 6, grey7 = 7, black = 7,
	 dark_blue = 8, yellow = 9, light_green = 10, red = 11, cream = 12,
	 dark_green = 13, orange = 14, light_blue = 15, no_colour = -1,
	 minimum = 0, maximum = 15 };
};

};

#endif
