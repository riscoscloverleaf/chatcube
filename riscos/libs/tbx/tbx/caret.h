/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2021 Alan Buckley   All Rights Reserved.
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

#ifndef TBX_CARET_H_
#define TBX_CARET_H_

#include "window.h"

namespace tbx
{
	/**
	 * Class to read and control the WimpCaret.
	 * 
	 * Writable gadgets and gadgets that contain writable fields
	 * will manage the caret themselves.
	 *
	 * Use this class to control the caret within a window manually.
	 *
	 * This class stores the values needed for the caret.
     * Call the set() update the system WIMP caret
	 */
	class Caret
	{

	public:
		Caret(bool get_current = false);
		Caret(const Caret &other);
		Caret(WindowHandle window, IconHandle icon, int x, int y, int height_and_flags, int index);
		Caret(tbx::Window window);
		Caret(tbx::Window window, int x, int y);
		Caret(tbx::Window window, int x, int y, int height, bool vdu = false);
		Caret(tbx::Window window, int x, int y, int height, int colour, bool untranslated = false, bool vdu = false);
		
		Caret& operator=(const Caret &other);
		
		void get();
		void set();
		void move(int x, int y);
		static void turn_off();
		
		/**
		 * Get toolbox window for Caret (if possible)
		 * @returns toolbox window or null window if it can't be determined
		 */
		tbx::Window window() const;
		
		/**
		 * Get Wimp window handle for the caret
		 *
		 * @returns window handle or -1 if no window has the caret
		 */
		WindowHandle window_handle() const {return _window;}

		/**
		 * Get the Wimp icon that contains the caret
		 *
		 * @returns The IconHandle or -1 if none.
		 */
		IconHandle icon_handle() const {return _icon;}

		/**
		 * Get the current x offset of the caret relative
		 * to the window work area origin
		 */
		int x_offset() const {return _x_offset;}

		/**
		 * Get the current y offset of the caret relative
		 * to the window work area origin
		 */
		int y_offset() const {return _y_offset;}

		/**
		 * Get the height of the caret
		 */
		int height() const { return (_height_and_flags & 0xFFFF); }

		/**
		 * Get caret colour - if colour set
		 */
		int colour() const {return (_height_and_flags >> 16) & 0xFF;}

		/**
		 * Is it a vdu caret
		 * @returns true for vdu caret, false for antialiased caret
		 */
		bool vdu() const {return (_height_and_flags & (1<<24))!=0;}

		/**
		 * Invisible caret
		 */
		bool invisible() const  {return (_height_and_flags & (1<<25));}

		/**
		 * caret_color is used for the colour otherwise caret is Wimp colour 11
		 */
		bool uses_colour() const  {return (_height_and_flags & (1<<26));}

		/**
		 * Colour is untranslated, otherwise they are a Wimp colour
		 */
		bool colour_untranslated() const  {return (_height_and_flags & (1<<27));}

		/**
		 * The index of the caret into the writeable field or -1 if not in
		 * a writeable field.
		 */
		int index() const {return _index;}
	private:
		WindowHandle _window;
		IconHandle _icon;
		int _x_offset;
		int _y_offset;
		int _height_and_flags;
		int _index;		
	};

}

#endif /* TBX_CARET_H_ */
