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

#ifndef TBX_TEXTCHANGEDLISTENER_H_
#define TBX_TEXTCHANGEDLISTENER_H_

#include "listener.h"
#include "eventinfo.h"
#include "gadget.h"

namespace tbx
{
    /**
     * Event details for a change of text in a gadget.
     *
     * @see TextChangedListener
     */
	class TextChangedEvent : public EventInfo
	{
	public:
		/**
		 * Construct the event.
		 */
		TextChangedEvent(IdBlock &id_block, PollBlock &data) :
			EventInfo(id_block, data) {};

		/**
		 * Virtual destructor
		 */
		virtual ~TextChangedEvent() {};


		/**
		 * Check if new text was delivered with the toolbox event.
		 *
		 * @returns true If the gadget contained more text than would
		 *          fit in the event delivery buffer.
		 */
		bool too_long() const {return (_data.word[3] & 1)!=0;}

		/**
		 * Get the text of the gadget that delivered the event.
		 *
		 * This will be the text delivered with the event if the too_long
		 * parameter is false. Otherwise the text will be retrieved from
		 * the gadget.
		 *
		 * @returns the gadget text.
		 */
		std::string text() const {return (too_long()) ? text_from_gadget() : text_from_event();}

	protected:
		/**
		 * Override in sub class to fetch text from the gadget.
		 *
		 * Called from the text method if the text wasn't delivered in
		 * the event buffer.
		 */
		virtual std::string text_from_gadget() const = 0;

		/**
		 * text from event data
		 */
		std::string text_from_event() const {return std::string(reinterpret_cast<const char *>(_data.word +4));}

	};

	/**
	 * Listener for text changed events
	 */
	class TextChangedListener : public Listener
	{
	public:
		/**
		 * The text has been changed.
		 */
		virtual void text_changed(TextChangedEvent &event) = 0;
	};
}

#endif /* TBX_TEXTCHANGEDLISTENER_H_ */
