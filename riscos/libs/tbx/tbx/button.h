/*
 * tbx RISC OS toolbox library
 *
 * Copyright (C) 2010-2011 Alan Buckley   All Rights Reserved.
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
 * button.h
 *
 *  Created on: 6-Jul-2010
 *      Author: alanb
 */

#ifndef TBX_BUTTON_H_
#define TBX_BUTTON_H_

#include "gadget.h"
#include "mouseclicklistener.h"
#include "eventinfo.h"
#include "colour.h"

namespace tbx
{

/**
 * A Button is a gadget that show a sprite or some text in the window.
 * It is similar to a Wimp icon, but has the following restrictions.
 * A Button created as sprite only cannot be made into any sort of text Button.
 * A Button created as text only cannot be made into a sprite only Button.
 * A sprite only Button can only refer to sprites by name and these must be in the Wimp sprite pool or the task's sprite area.
 */
class Button: public tbx::Gadget
{
public:
	enum {TOOLBOX_CLASS = 960}; //!< Toolbox class for this gadget.

	Button() {} //!< Construct an uninitialised button.
	/**
	 * Destroy a button gadget.
	 *
	 * This does not delete the underlying toolbox gadget.
	 */
	~Button() {}

	/**
	 * Construct a button from another button.
	 *
	 * Both Button will refer to the same underlying toolbox gadget.
	 */
	Button(const Button &other) : Gadget(other) {}

	/**
	 * Construct a button from another gadget.
	 *
	 * The Button and the Gadget will refer to the same
	 * underlying toolbox gadget.
	 *
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a button
	 */
	Button(const Gadget &other) : Gadget(other)	{check_toolbox_class(Button::TOOLBOX_CLASS);}

	/**
	 * Construct a button from a component.
	 *
	 * The Button and the Component will refer to the same
	 * underlying toolbox component.
	 */
	Button(const Component &other) : Gadget(other) {Window check(other.handle()); check_toolbox_class(Button::TOOLBOX_CLASS);}

	/**
	 * Assign a button to refer to the same underlying toolbox gadget
	 * as another.
	 */
	Button &operator=(const Button &other) {_handle = other._handle; _id = other._id; return *this;}

	/**
	 * Assign a button to refer to the same underlying toolbox gadget
	 * as an existing Gadget.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a button
	 */
	Button &operator=(const Gadget &other) {_handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Assign a button to refer to the same underlying toolbox component
	 * as an existing Gadget.
	 * @throws ObjectClassError if the component is not in a window.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a button
	 */
	Button &operator=(const Component &other) {Window check(other.handle()); _handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Check if this button refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they refer to the same underlying toolbox gadget.
	 */
	bool operator==(const Gadget &other) const {return (_handle == other.handle() && _id == other.id());}

	/**
	 * Check if this button refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they do not refer to the same underlying toolbox gadget.
	 */
	bool operator!=(const Gadget &other) const {return (_handle != other.handle() || _id != other.id());}

	/**
	 * Returns the flags for the button, these are the
	 * same as for a wimp icon. See the RISC OS PRM for
	 * details of what the bits of the flags mean.
	 *
	 * Other methods are provided to check the individual elements
	 * of these flags by name.
	 */
	int flags() const {return int_property(960);}

	/**
	 * Set the button flags.
	 *
	 * This is similar to Wimp_SetIconState but not all flags are settable.
	 * See the RISC OS PRM for details of the flags.
	 *
	 * Other methods are provided to set the individual elements of these
	 * flags by name.
	 *
	 * @param clear bits to clear
	 * @param eor bits to exclusive or
	 */
	void flags(int clear, int eor);
    /**
	 * Check if the button shows text
	 *
	 * @returns true if the button shows text
	 */
	bool has_text() const {return (flags() & 1)!=0;}
	/**
	 * Set if the button shows text
	 *
	 * @param value set to true if the button should show text
	 */
	void has_text(bool value) {flags(1, value?1:0 );}
	/**
	 * Check if the button shows a sprite
	 *
	 * @returns true if the button shows a sprite
	 */
	bool is_sprite() const {return (flags()&2)!=0;}
	/**
	 * Set if the button shows a sprite
	 *
	 * @param value true if the button will show a sprite
	 */
	void is_sprite(bool value) {flags(2,value?2:0);}
	/**
	 * Check if the button has a border
	 *
	 * @returns true if the button border
	 */
	bool has_border() const {return (flags() & 4)!=0;}
	/**
	 * Set if the button has a border
	 *
	 * @param value true to give the button a border
	 */
	void has_border(bool value) {flags(4, value?4:0);}
	/**
	 * Check if the contents of the button are horizontally centred
	 *
	 * @returns true if the button is horizontally centred
	 */
	bool hcentred() const {return (flags()&8)!=0;}
	/**
	 * Set if the contents of the button are horizontally centred
	 *
	 * @param value true to horizontally centre the button
	 */
	void hcentred(bool value) {flags(8, value?8:0);}
	/**
	 * Check if the contents of the button are vertically centred
	 *
	 * @returns true if the button is vertically centred
	 */
	bool vcentred() const {return (flags()&16)!=0;}
	/**
	 * Set if the contents of the button are vertically centred
	 *
	 * @param value true to vertically centre the button
	 */
	void vcentred(bool value) {flags(16,value?16:0);}
	/**
	 * Check if the button background is filled
	 *
	 * @returns true if the button background is filled
	 */
	bool filled() const {return (flags()&32)!=0;}
	/**
	 * Set if the button background is filled
	 *
	 * @param value true to fill the button background
	 */
	void filled(bool value) {flags(32, value?32:0);}
	/**
	 * Check if the button uses an anti aliased font
	 *
	 * @returns true if the button uses an anti aliased font
	 */
	bool anti_aliased_font() const {return (flags()&64)!=0;}
	/**
	 * Set if the button uses an anti aliased font
	 *
	 * Note: if the anti-aliased flag is set the font_handle methods become
	 * available, but the foreground and background methods should not be
	 * used.
	 *
	 * @param value true to make the button use an anti aliased font
	 */
	void anti_aliased_font(bool value) {flags(64, value?64:0);}
	// 6  text is an anti-aliased font (affects meaning of bits 24 - 31)
	/**
	 * Check if the button need help from the user application to be redrawn
	 *
	 * @returns true if the button needs help to be redrawn
	 */
	bool redraw_needs_help() const {return (flags()&128)!=0;}
	/**
	 * Set if the button need help from the user application to be redrawn
	 *
	 * @param value true if the button needs help to be redrawn
	 */
	void redraw_needs_help(bool value) {flags(128,value?128:0);}
	// bit 8 - icon data is indirected always set
	/**
	 * Check if the text on the button is right justified
	 *
	 * @returns true if the text on the button is right justified
	 */
	bool text_right_justified() const {return (flags() & 512)!=0;}
	/**
	 * Set if the text on the button is right justified
	 *
	 * @param value true to make the text on the button right justified
	 */
	void text_right_justified(bool value) {flags(512, value?512:0);}
	/**
	 * Check if the adjust does not cancel other selections in the same ESG group
	 *
	 * @returns true if the button adjust does not cancel other selections
	 */
	bool adjust_does_not_cancel() const {return (flags()&1024)!=0;}
	/**
	 * Set if the adjust does not cancel other selections in the same ESG group
	 *
	 * @param value true so set the button so adjust does not cancel other selections
	 */
	void adjust_does_not_cancel(bool value) {flags(1024,value?1024:0);}
	/**
	 * Check if the button display the sprite half size
	 *
	 * @returns true if the button display the sprite half size
	 */
	bool half_size_sprite() const {return (flags()&2048)!=0;}
	/**
	 * Set if the button shows the sprite half size
	 *
	 * @param value true to set the button to display the sprite half size
	 */
	void half_size_sprite(bool value) {flags(2048, value?2048:0);}

	/**
	 * Enumeration for how the button processes clicks
	 */
	enum ButtonType
	{
		IGNORE_CLICKS, // ignore mouse clicks or movements over the icon (except Menu)
		CONTINUOUS,    // notify task continuously while pointer is over this icon
		CLICK_AUTO_REPEAT, // click notifies task (auto-repeat)
		CLICK_ONCE, // click notifies task (once only)
		CLICK_SELECTS, // click selects the icon; release over the icon notifies task; moving the pointer away deselects the icon
		CLICK_SELECTS_DOUBLE, // click selects; double click notifies task
		CLICK_ONCE_DRAG, // click notifies, but can also drag (returns button state * 16)
		CLICK_SELECTS_DRAG, // as CLICK_SELECTS, but can also drag (returns button state * 16) and moving away from the icon doesn't deselect it
		CLICK_SELECTS_DOUBLE_DRAG, // as CLICK_SELECTS_DOUBLE, but can also drag (returns button state * 16)
		OVER_SELECTS, // pointer over icon selects; moving away from icon deselects; click over icon notifies task ('menu' icon)
		CLICK_DOUBLE_DRAG, //click returns button state * 256, drag returns button state * 16, double click returns button state * 1
		CLICK_NOTIFY_AND_SELECTS_DRAG, // click selects icon and returns button state, drag returns button state*16
		FOCUS_AND_DRAG = 14, // clicks cause the icon to gain the caret and its parent window to become the input focus and can also drag (writable icon). For example, this is used by the FormEd application
		WRITEABLE, // clicks cause the icon to gain the caret and its parent window to become the input focus (writable icon)
	};

	/**
	 * Get how the button processes mouse clicks
	 *
	 * @returns ButtonType value for how clicks are handled
	 */
	ButtonType button_type() const {return ButtonType((flags() & 0xF000) >> 12);}
	/**
	 * Set how the button processes mouse clicks
	 *
	 * @param type ButtonType for how clicks are handled
	 */
	void button_type(ButtonType type) {flags(0xF000, ((int)type) << 12);}

	/**
	 * Return the exclusive selection group (ESG) for the button
	 *
	 * Selecting one button that is an ESG will unselect all the others in
	 * the same ESG.
	 *
	 * @returns ESG number (0-31). 0 Means not in a group.
	 */
	int esg() const {return (flags() & 0x1F0000) >> 16;}
	/**
	 * Set the exclusive selection group (ESG) for the button
	 *
	 * Selecting one button that is an ESG will unselect all the others in
	 * the same ESG.
	 *
	 * @param value ESG number (0-31). 0 Means not in a group.
	 */
	void esg(int value) {flags(0x1F0000, value << 16);}

	/**
	 * Return the foreground WIMP colour
	 *
	 * This method is only valid when anti_aliased_font() == false
	 *
	 * @returns foreground WIMP colour (0-15)
	 */
	WimpColour foreground() const {return WimpColour((flags()&0xF000000)>>24);} // !anti_aliased_font
	/**
	 * Set the foreground WIMP colour
	 *
	 * This method is only valid when anti_aliased_font() == false
	 *
	 * @param value foreground WIMP colour (0-15)
	 */
	void foreground(WimpColour value) {flags(0xF000000, ((int)value) << 24);}
	/**
	 * Return the background WIMP colour
	 *
	 * This method is only valid when anti_aliased_font() == false
	 *
	 * @returns background WIMP colour (0-15)
	 */
	WimpColour background() const {return WimpColour((flags()&0xF0000000)>>28);} // !anti_aliased_font
	/**
	 * Set the background WIMP colour
	 *
	 * This method is only valid when anti_aliased_font() == false
	 *
	 * @param value background WIMP colour (0-15)
	 */
	void background(WimpColour value) {flags(0xF0000000, ((int)value) << 28);}

	/**
	 * Get the WIMP font handle for drawing text
	 *
	 * This method is only valid when anti_aliased_font() == true
	 *
	 * @return WIMP font handle
	 */
	int font_handle() const {return (flags() & 0xFF000000) >> 24;} // anti_aliased_font
	/**
	 * Set the WIMP font handle for drawing text
	 *
	 * This method is only valid when anti_aliased_font() == true
	 *
	 * @return WIMP font handle
	 */
	void font_handle(int value) {flags(0xFF000000, value << 24);}


	/**
	 * Set the button value. i.e. the text or sprite name.
	 */
	void value(std::string v) {string_property(962, v);}

	/**
	 * Get the button value
	 */
	std::string value() const {return string_property(963);}

	/**
	 * Return the length of the button value
	 */
	int value_length() const {return string_property_length(963);}

	/**
	 * Set the button validation
	 */
	void validation(std::string v) {string_property(964, v);}

	/**
	 * Get the button validation
	 */
	std::string validation() const {return string_property(965);}

	/**
	 * Return the length of the button validation
	 */
	int validation_length() const {return string_property_length(965);}

	void font(const std::string &name, int width, int height);
	void system_font(int width, int height);

	void add_mouse_click_listener(MouseClickListener *listener);
	void remove_mouse_click_listener(MouseClickListener *listener);
};

}

#endif /* TBX_BUTTON_H_ */
