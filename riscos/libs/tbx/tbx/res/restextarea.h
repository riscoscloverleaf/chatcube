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

#ifndef TBX_RES_RESTEXTAREA_H
#define TBX_RES_RESTEXTAREA_H

#include "resgadget.h"

namespace tbx {
namespace res {

/**
 * Class for TextArea gadget template
 */
class ResTextArea : public ResGadget
{

public:
	enum {TYPE_ID = 0x4018 };

	/**
	 * Construct an text area gadget resource
	 *
	 * @param other ResGadget to copy resource from
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an text area
	 */
	ResTextArea(const ResGadget &other) : ResGadget(other)
	{
     check_type(TYPE_ID);
	}

	/**
	 * Construct an text area gadget resource
	 *
	 * @param other text area to copy resource from
	 */
	ResTextArea(const ResTextArea &other) : ResGadget(other)
	{
	}

	virtual ~ResTextArea() {}

	/**
	 * Assign from a ResGadget
	 *
	 * @param other ResGadget to copy
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an text area
	 */
	ResTextArea &operator=(const ResGadget &other)
	{
		other.check_type(TYPE_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another text area gadget resource
	 *
	 * @param other text area gadget resource to copy
	 */
	ResTextArea &operator=(const ResTextArea &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a text area gadget resource.
	 *
	 * All options are false, events unset and messages are null.
	 */
	ResTextArea()
	  : ResGadget(0x4018,56)
	{
		init_message(44,0); // text
		background(0xffffff00); // white
	}

	/**
	 * Check if text area has a vertical scroll bar
	 *
	 * @returns true if there is a vertical scroll bar
	 */
	bool has_vertical_scrollbar() const {return flag(0, 1<<0);}
	/**
	 * Set if text area has a vertical scroll bar
	 *
	 * @param value set to true to include a vertical scroll bar
	 */
	void has_vertical_scrollbar(bool value) {flag(0,1<<0,value);}
	/**
	 * Check if text area has a horizontal scroll bar
	 *
	 * @returns true if there is a horizontal scroll bar
	 */
	bool has_horizontal_scrollbar() const {return flag(0, 1<<1);}
	/**
	 * Set if text area has a horizontal scroll bar
	 *
	 * @param value set to true to include a horizontal scroll bar
	 */
	void has_horizontal_scrollbar(bool value) {flag(0,1<<1,value);}
	/**
	 * Check if text will be word wrapped
	 *
	 * @returns true if the text will be word wrapped
	 */
	bool wordwrap() const {return flag(0, 1<<2);}
	/**
	 * Set if text will be word wrapped
	 *
	 * @param value set to true to word wrap the text
	 */
	void wordwrap(bool value) {flag(0,1<<2,value);}
	/**
	 * Check if text can be selected by the mouse
	 *
	 * @returns true if text can be selected
	 */
	bool allow_selection() const {return flag(0, 1<<3);}
	/**
	 * Set if text can be selected by the mouse
	 *
	 * @param value set to true to allow text to be selected
	 */
	void allow_selection(bool value) {flag(0,1<<3,value);}
	/**
	 * Check if text can be saved.
	 *
	 * Text is saved by selecting some text and dragging it to
	 * another application or the filer.
	 *
	 * @returns true if text can be saved
	 */
	bool allow_save() const {return flag(0, 1<<4);}
	/**
	 * Set if text can be saved.
	 *
	 * Text is saved by selecting some text and dragging it to
	 * another application or the filer.
	 *
	 * @param value set to true if text can be saved
	 */
	void allow_save(bool value) {flag(0,1<<4,value);}
	/**
	 * Check if text can be loaded by dragging it to the text area
	 *
	 * @returns true if text can be loaded
	 */
	bool allow_load() const {return flag(0, 1<<5);}
	/**
	 * Set if text can be loaded by dragging it to the text area
	 *
	 * @param value set to true to allow text to be loaded
	 */
	void allow_load(bool value) {flag(0,1<<5,value);}
	/**
	 * Check if clipboard support is enabled
	 *
	 * @returns true if clipboard support is enabled
	 */
	bool clipboard() const {return flag(0, 1<<6);}
	/**
	 * Set if clipboard support is enabled
	 *
	 * @param value set to true to enable clipboard support
	 */
	void clipboard(bool value) {flag(0,1<<6,value);}
	/**
	 * Check if the text field will automatically scroll
	 * as text is entered.
	 *
	 * @returns true if auto scroll is enabled
	 */
	bool auto_scroll() const {return flag(0, 1<<7);}
	/**
	 * Set if the text field will automatically scroll
	 * as text is entered.
	 *
	 * @param value set to true to enable auto scroll
	 */
	void auto_scroll(bool value) {flag(0,1<<7,value);}

	/**
	 * Get the type of the text area gadget
	 *
	 * @returns type of the text area gadget
	 */
	int type() const {return int_value(36);}
	//TODO: Find out what this actually is
	/**
	 * Set the type of the text area gadget
	 *
	 * @param value type of the text area gadget
	 */
	void type(int value) {int_value(36,value);}
	/**
	 * Get the event for the text area gadget
	 *
	 * @returns ID of event.
	 */
	int event() const {return int_value(40);}
	/**
	 * Set the event for the text area gadget
	 *
	 * @param value new event ID
	 */
	void event(int value) {int_value(40,value);}
	/**
	 * Get the initial text for the text area
	 *
	 * @returns pointer to zero terminated string or 0 if none
	 */
	const char *text() const {return message(44);}
	/**
	 * Set the initial text for the text area
	 *
	 * @param value pointer to zero terminated string or 0 if none
	 */
	void text(const char *value) {message(44, value);}
	/**
	 * Set the initial text for the text area
	 *
	 * @param value initial text
	 */
	void text(const std::string &value) {message(44, value);}
	/**
	 * Get foreground/text colour for the text area
	 *
	 * @returns foreground/text colour
	 */
	int foreground() const {return int_value(48);}
	/**
	 * Set foreground/text colour for the text area
	 *
	 * @param value foreground/text colour
	 */
	void foreground(int value) {int_value(48,value);}
	/**
	 * Get background colour for the text area
	 *
	 * @returns background
	 */
	int background() const {return int_value(52);}
	/**
	 * Set background colour for the text area
	 *
	 * @param value background colour
	 */
	void background(int value) {int_value(52,value);}

};

}
}

#endif // TBX_RES_RESTEXTAREA_H

