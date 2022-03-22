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

#ifndef TBX_CARETLISTENER_H_
#define TBX_CARETLISTENER_H_

#include "bbox.h"
#include "listener.h"
#include "eventinfo.h"
#include "window.h"
#include "gadget.h"

namespace tbx
{
	/**
	 * Event data for lose or gain caret listener
	 */
	class CaretEvent : public EventInfo
	{

	public:
		/**
		 * Construct from toolbox event information
		 *
		 * @param id_block Object/component data
		 * @param poll_block event information
		 */
		CaretEvent(IdBlock &id_block, PollBlock &poll_block) :
			EventInfo(id_block, poll_block)
			{}

		/**
		 * Get Window that is losing (LoseCaretListener)
		 * or gaining (GainCaretListener) the caret.
		 *
		 * @returns window or a null window handle if no window has
		 * the caret
		 */
		Window window() const {return id_block().self_object();}

		/**
		 * Get the gadget that is losing (LoseCaretListener)
		 * or gaining (GainCaretListener) the focus.
		 *
		 * @returns gadget or a null gadget it no gadget had the caret
		 */
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
	};

	/**
	 * Listener for LoseCaret Wimp event.
	 *
	 * The event isn't generated if the input focus only changes
	 * position within the same window
	 */
	class LoseCaretListener : public Listener
	{
	public:
		/**
		 * virtual function called when the application receives the lose caret
		 * window event
		 *
		 * @param event details of where the caret was lost from
		 */
		virtual void lose_caret(CaretEvent &event) = 0;
	};

	/**
	 * Listener for gain caret Wimp event.
	 *
	 * The event isn't generated if the input focus only changes
	 * position within the same window
	 */
	class GainCaretListener : public Listener
	{
	public:
		/**
		 * virtual function called when the application receives the gain caret
		 * window event
		 *
		 * @param event details of where the caret has been moved to
		 */
		virtual void gain_caret(CaretEvent &event) = 0;
	};

}

#endif /* TBX_CARETLISTENER_H_ */
