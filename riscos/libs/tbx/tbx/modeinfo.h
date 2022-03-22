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

#ifndef  tbx_MODEINFO_H_
#define  tbx_MODEINFO_H_

#include "kernel.h"
#include "swis.h"
#include "point.h"
#include "size.h"

namespace tbx
{

/**
 * Class to return information on a screen mode
 *
 * The mode number referred to in this class is either
 * the standard screen mode number or for RISC OS 3.5 and
 * above can be a mode specifier or a sprite mode word.
 */
class ModeInfo
{
  public:
	/**
	 * Constructor to retrieve information about the current screen mode
	 */
     ModeInfo()              {_mode = -1;};
     /**
      * Constructor to retrieve information about the given screen mode
      *
      * @param mode screen mode number
      */
     ModeInfo(int mode)      {_mode = mode;};

     /**
      * Set the mode to return information about
      *
      * @param mode screen mode number
      */
     void mode(int mode = -1) {_mode = mode;};

     /**
      * Get to current sreen mode.
      *
      * This is the actual mode value and not -1 (which is
      * used in some APIs to mean the current mode).
      *
      * This value is only avaliable on RISC OS 3.5 or later
      *
      * @returns the current screen mode
      */
     static int screen_mode();

     /**
      * Get the mode number information will be returned about
      */
     int mode() const {return _mode;};

     /**
      * Return the number of colours for the mode
      *
      * @returns number of colours (or 0 for 16M colour modes)
      */
     inline int colours() const;

     /**
      * Return the eigen factors for the mode
      *
      * The eigen factors are the binary shift to convert from
      * screen pixels to OS units.
      *
      * e.g. and eigen factor of 2 will give 1 pixels = 1<<2 or 4 os units
      *
      * @returns eigin factors in a point
      */
	 inline Point eig() const;

	 /**
	  * Get the size of one pixel in OS units
	  *
	  * @returns Size containing the size of one pixel in OS units
	  */
	 inline Size screen_unit() const;

	 /**
	  * Get the screen size in pixels
	  *
	  * @returns screen size in pixels
	  */
     inline Size pixel_size() const;
     /**
      * Get the screen size in OS units
      *
      * @returns screen size in OS units
      */
     inline Size screen_size() const;

  protected:
     /**
      * Screen mode number interrogated for details
      */
     int _mode;
};

inline int ModeInfo::colours() const
{
   int colours;
   _swix(OS_ReadModeVariable,_IN(0) | _IN(1) | _OUT(2), _mode, 3, &colours);
   if (colours == 63) colours = 255;
   return (colours+1);
}

inline Point ModeInfo::eig() const
{
   int x,y;
   _swix(OS_ReadModeVariable,_IN(0) | _IN(1) | _OUT(2), _mode, 4, &x);
   _swix(OS_ReadModeVariable,_IN(0) | _IN(1) | _OUT(2), _mode, 5, &y);
   return Point(x,y);
}

inline Size ModeInfo::screen_unit() const
{
   int x,y;
   _swix(OS_ReadModeVariable,_IN(0) | _IN(1) | _OUT(2), _mode, 4, &x);
   _swix(OS_ReadModeVariable,_IN(0) | _IN(1) | _OUT(2), _mode, 5, &y);
   return Size((1<<x),(1<<y));
}

inline Size ModeInfo::pixel_size() const
{
   int x,y;
   _swix(OS_ReadModeVariable,_IN(0) | _IN(1) | _OUT(2), _mode, 11, &x);
   _swix(OS_ReadModeVariable,_IN(0) | _IN(1) | _OUT(2), _mode, 12, &y);
   return Size(x+1,y+1);
}

inline Size ModeInfo::screen_size() const
{
   int eig, x,y;
   _swix(OS_ReadModeVariable,_IN(0) | _IN(1) | _OUT(2), _mode, 4, &eig);
   _swix(OS_ReadModeVariable,_IN(0) | _IN(1) | _OUT(2), _mode, 11, &x);
   x = (x + 1) << eig;
   _swix(OS_ReadModeVariable,_IN(0) | _IN(1) | _OUT(2), _mode, 5, &eig);
   _swix(OS_ReadModeVariable,_IN(0) | _IN(1) | _OUT(2), _mode, 12, &y);
   y = (y + 1) << eig;

   return Size(x,y);
}

inline int ModeInfo::screen_mode()
{
	int mode = 0;
	_swix(OS_ScreenMode, _IN(0)|_OUT(1), 1, &mode);
	return mode;
}

};

#endif
