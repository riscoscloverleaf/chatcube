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

/*
 * keylistener.h
 *
 *  Created on: 9 Jul 2010
 *      Author: alanb
 */

#ifndef TBX_KEYLISTENER_H_
#define TBX_KEYLISTENER_H_

#include "listener.h"
#include "eventinfo.h"
#include "window.h"
#include "gadget.h"

namespace tbx
{
	/**
	 * Event data for lose or gain caret listener
	 */
	class KeyEvent : public EventInfo
	{
	private:
		bool _used;

	public:
		/**
		 * Construct KeyEvent from information returned from the WIMP
		 *
		 * @param id_block Toolbox ID block
		 * @param poll_block WIMP message block
		 */
		KeyEvent(IdBlock &id_block, PollBlock &poll_block) :
			EventInfo(id_block, poll_block), _used(false)
			{}

		/**
		 * Get Window that the key is pressed in
		 */
		Window window() const {return id_block().self_object();}

		/**
		 * Get the gadget that the key is pressed in
		 *
		 * @returns gadget or a null gadget focus on a window
		 */
#undef self_component
		Gadget gadget() const {return id_block().self_component();}

		/**
		 * Get the current x offset of the caret relative
		 * to the window origin
		 */
		int x_offset() const {return _data.word[2];}

		/**
		 * Get the current y offset of the caret relative
		 * to the window origin
		 */
		int y_offset() const {return _data.word[3];}

		/**
		 * Get the height of the caret
		 */
		int height() const { return (_data.word[4] & 0xFFFF); }

		/**
		 * Get caret colour - if colour set
		 */
		int colour() const {return (_data.word[4] >> 16) & 0xFF;}

		/**
		 * Is it a vdu caret
		 * @returns true for vdu caret, false for antialiased caret
		 */
		bool vdu() const {return (_data.word[4] & (1<<24))!=0;}

		/**
		 * Invisible caret
		 */
		bool invisible() const  {return (_data.word[4] & (1<<25));}

		/**
		 * caret_color is used for the colour otherwise caret is Wimp colour 11
		 */
		bool uses_colour() const  {return (_data.word[4] & (1<<26));}

		/**
		 * Colour is untranslated, otherwise they are a Wimp colour
		 */
		bool colour_untranslated() const  {return (_data.word[4] & (1<<27));}

		/**
		 * The index of the caret into the writeable field or -1 if not in
		 * a writeable field.
		 */
		int index() const {return _data.word[5];}

		/**
		 * key code of character press.
		 *
		 * As well as ascii values can also be from the
		 * SpecialKeys enumeration.
		 */
		int key() const {return _data.word[6];}

		/**
		 * Enumeration for special key codes passed from the WIMP
		 */
		enum SpecialKeys
		{
			Escape = 0x1B,
			Print = 0x180, Shift_Print = 0x190, Control_Print = 0x1A0, ControlShift_Print = 0x1B0,
			F1  = 0x181, Shift_F1 = 0x191, Control_F1 = 0x1A1, ControlShift_F1 = 0x1B1,
			F2  = 0x182, Shift_F2 = 0x192, Control_F2 = 0x1A2, ControlShift_F2 = 0x1B2,
			F3  = 0x183, Shift_F3 = 0x193, Control_F3 = 0x1A3, ControlShift_F3 = 0x1B3,
			F4  = 0x184, Shift_F4 = 0x194, Control_F4 = 0x1A4, ControlShift_F4 = 0x1B4,
			F5  = 0x185, Shift_F5 = 0x195, Control_F5 = 0x1A5, ControlShift_F5 = 0x1B5,
			F6  = 0x186, Shift_F6 = 0x196, Control_F6 = 0x1A6, ControlShift_F6 = 0x1B6,
			F7  = 0x187, Shift_F7 = 0x197, Control_F7 = 0x1A7, ControlShift_F7 = 0x1B7,
			F8  = 0x188, Shift_F8 = 0x198, Control_F8 = 0x1A8, ControlShift_F8 = 0x1B8,
			F9  = 0x189, Shift_F9 = 0x199, Control_F9 = 0x1A9, ControlShift_F9 = 0x1B9,
			Tab = 0x18A, Shift_Tab = 0x19A, Control_Tab = 0x1AA, ControlShift_Tab = 0x1BA,
			Copy  = 0x18B, Shift_Copy = 0x19B, Control_Copy = 0x1AB, ControlShift_Copy = 0x1BB,
			LeftArrow  = 0x18C, Shift_LeftArrow = 0x19C, Control_LeftArrow = 0x1AC, ControlShift_LeftArrow = 0x1BC,
			RightArrow  = 0x18D, Shift_RightArrow = 0x19D, Control_RightArrow = 0x1AD, ControlShift_RightArrow = 0x1BD,
			DownArrow  = 0x18E, Shift_DownArrow = 0x19E, Control_DownArrow = 0x1AE, ControlShift_DownArrow = 0x1BE,
			UpArrow  = 0x18F, Shift_UpArrow = 0x19F, Control_UpArrow = 0x1AF, ControlShift_UpArrow = 0x1BF,
			PageDown  = 0x19E, Shift_PageDown = 0x18E, Control_PageDown = 0x1BE, ControlShift_PageDown = 0x1AE,
			PageUp  = 0x19F, Shift_PageUp = 0x18F, Control_PageUp = 0x1BF, ControlShift_PageUp = 0x1AF,
			F10  = 0x1CA, Shift_F10 = 0x1DA, Control_F10 = 0x1EA, ControlShift_F10 = 0x1FA,
			F11  = 0x1CB, Shift_F11 = 0x1DB, Control_F11 = 0x1EB, ControlShift_F11 = 0x1FB,
			F12  = 0x1CC, Shift_F12 = 0x1DC, Control_F12 = 0x1EC, ControlShift_F12 = 0x1FC,
			Insert  = 0x1CD, Shift_Insert = 0x1DD, Control_Insert = 0x1ED, ControlShift_Insert = 0x1FD
		};

		/**
		 * Call if the key has been used by the listener
		 */
		void key_used() {_used = true;}

		/**
		 * Returns true if a listener has used the key
		 */
		bool is_key_used() const {return _used;}
	};

	/**
	 * Listener for KeyPressed WIMP event.
	 *
	 * If the listener uses the key it should call KeyEvent::key_used
	 * so no other listeners can use it and it is not passed on to the
	 * WIMP.
	 */
	class KeyListener : public Listener
	{
	public:
		/**
		 * virtual function called when the application receives a key pressed
		 * window event
		 *
		 * @param event details of where the key pressed and it's location
		 */
		virtual void key(KeyEvent &event) = 0;
	};

}


#endif /* TBX_KEYLISTENER_H_ */
