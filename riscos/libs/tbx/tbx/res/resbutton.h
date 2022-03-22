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

#ifndef TBX_RES_RESBUTTON_H
#define TBX_RES_RESBUTTON_H

#include "resgadget.h"

namespace tbx {
namespace res {

/**
 * Class for Button gadget template.
 */
class ResButton : public ResGadget
{

public:
	enum {TYPE_ID = 960 };

	/**
	 * Construct an button gadget resource
	 *
	 * @param other ResGadget to copy resource from
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an button
	 */
	ResButton(const ResGadget &other) : ResGadget(other)
	{
     check_type(TYPE_ID);
	}

	/**
	 * Construct an button gadget resource
	 *
	 * @param other button to copy resource from
	 */
	ResButton(const ResButton &other) : ResGadget(other)
	{
	}

	virtual ~ResButton() {}

	/**
	 * Assign from a ResGadget
	 *
	 * @param other ResGadget to copy
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an button
	 */
	ResButton &operator=(const ResGadget &other)
	{
		other.check_type(TYPE_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another button gadget resource
	 *
	 * @param other button gadget resource to copy
	 */
	ResButton &operator=(const ResButton &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a button gadget resource.
	 *
	 * All options are false, events unset and messages are null.
	 */
	ResButton()
	  : ResGadget(960,56)
	{
		int_value(36, 0x7000110); // foreground 7, icon data indirected (always set)
		init_message(40,0); // value
		init_string(48,0); // validation
	}

	/**
	 * Check if button uses the task sprite area
	 *
	 * @returns true if button uses the task sprite area, false if
	 * button uses the WIMP sprite area
	 */
	bool use_task_sprite_area() const {return flag(0, 1<<0);}
	/**
	 * Set if button uses the task sprite area
	 *
	 * @param value true to make the button use the task sprite area, false to
	 * make the button use the WIMP sprite area
	 */
	void use_task_sprite_area(bool value) {flag(0,1<<0,value);}
	/**
	 * Check if button will return clicks with the menu button
	 *
	 * @returns true if menu clicks are returned
	 */
	bool return_menu_clicks() const {return flag(0, 1<<1);}
	/**
	 * Set if button will return clicks with the menu button
	 *
	 * @param value true to make the button return menu clicks
	 */
	void return_menu_clicks(bool value) {flag(0,1<<1,value);}

	/**
	 * Raw access to all button flags.
	 *
	 * It is recommended you use the other methods in this
	 * class rather than use this directly.
	 */
	int button_flags() const {return int_value(36);}
	/**
	 * Raw access to all button flags.
	 *
	 * It is recommended you use the other methods in this
	 * class rather than use this directly.
	 *
	 * @param value new button flags
	 */
	void button_flags(int value) {int_value(36,value);}

	/**
	 * Check if the button shows text
	 *
	 * @returns true if the button shows text
	 */
	bool has_text() const {return flag(36,1);}
	/**
	 * Set if the button shows text
	 *
	 * @param value set to true if the button should show text
	 */
	void has_text(bool value) {flag(36, 1, value );}
	/**
	 * Check if the button shows a sprite
	 *
	 * @returns true if the button shows a sprite
	 */
	bool is_sprite() const {return flag(36,1<<1);}
	/**
	 * Set if the button shows a sprite
	 *
	 * @param value true if the button will show a sprite
	 */
	void is_sprite(bool value) {flag(36, 1<<1, value);}
	/**
	 * Check if the button has a border
	 *
	 * @returns true if the button border
	 */
	bool has_border() const {return flag(36,1<<2);}
	/**
	 * Set if the button has a border
	 *
	 * @param value true to give the button a border
	 */
	void has_border(bool value) {flag(36, 1<<2, value);}
	/**
	 * Check if the contents of the button are horizontally centred
	 *
	 * @returns true if the button is horizontally centred
	 */
	bool hcentred() const {return flag(36,1<<3);}
	/**
	 * Set if the contents of the button are horizontally centred
	 *
	 * @param value true to horizontally centre the button
	 */
	void hcentred(bool value) {flag(36, 1<<3, value);}
	/**
	 * Check if the contents of the button are vertically centred
	 *
	 * @returns true if the button is vertically centred
	 */
	bool vcentred() const {return flag(36,1<<4);}
	/**
	 * Set if the contents of the button are vertically centred
	 *
	 * @param value true to vertically centre the button
	 */
	void vcentred(bool value) {flag(36, 1<<4, value);}
	/**
	 * Check if the button background is filled
	 *
	 * @returns true if the button background is filled
	 */
	bool filled() const {return flag(36, 1<<5);}
	/**
	 * Set if the button background is filled
	 *
	 * @param value true to fill the button background
	 */
	void filled(bool value) {flag(36, 1<<5, value);}
	/**
	 * Check if the button uses an anti aliased font
	 *
	 * @returns true if the button uses an anti aliased font
	 */
	bool anti_aliased_font() const {return flag(36, 1 << 6);}
	/**
	 * Set if the button uses an anti aliased font
	 *
	 * Note: if the anti-aliased flag is set the font_handle methods become
	 * available, but the foreground and background methods should not be
	 * used.
	 *
	 * @param value true to make the button use an anti aliased font
	 */
	void anti_aliased_font(bool value) {flag(36, 1<<6, value);}
	// 6  text is an anti-aliased font (affects meaning of bits 24 - 31)  
	/**
	 * Check if the button need help from the user application to be redrawn
	 *
	 * @returns true if the button needs help to be redrawn
	 */
	bool redraw_needs_help() const {return flag(36, 1 << 7);}
	/**
	 * Set if the button need help from the user application to be redrawn
	 *
	 * @param value true if the button needs help to be redrawn
	 */
	void redraw_needs_help(bool value) {flag(36, 1<<7, value);}
	// bit 8 - icon data is indirected always set
	/**
	 * Check if the text on the button is right justified
	 *
	 * @returns true if the text on the button is right justified
	 */
	bool text_right_justified() const {return flag(36, 1 << 9);}
	/**
	 * Set if the text on the button is right justified
	 *
	 * @param value true to make the text on the button right justified
	 */
	void text_right_justified(bool value) {flag(36, 1<<9, value);}
	/**
	 * Check if the adjust does not cancel other selections in the same ESG group
	 *
	 * @returns true if the button adjust does not cancel other selections
	 */
	bool adjust_does_not_cancel() const {return flag(36, 1 << 10);}
	/**
	 * Set if the adjust does not cancel other selections in the same ESG group
	 *
	 * @param value true so set the button so adjust does not cancel other selections
	 */
	void adjust_does_not_cancel(bool value) {flag(36, 1<<10, value);}
	/**
	 * Check if the button display the sprite half size
	 *
	 * @returns true if the button display the sprite half size
	 */
	bool half_size_sprite() const {return flag(36, 1 << 11);}
	/**
	 * Set if the button shows the sprite half size
	 *
	 * @param value true to set the button to display the sprite half size
	 */
	void half_size_sprite(bool value) {flag(36, 1<<11, value);}

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
	ButtonType button_type() const {return ButtonType(flag_value(36, 0xF000) >> 12);}
	/**
	 * Set how the button processes mouse clicks
	 *
	 * @param type ButtonType for how clicks are handled
	 */
	void button_type(ButtonType type) {flag_value(36, ((int)type) << 12);}

	/**
	 * Return the exclusive selection group (ESG) for the button
	 *
	 * Selecting one button that is an ESG will unselect all the others in
	 * the same ESG.
	 *
	 * @returns ESG number (0-31). 0 Means not in a group.
	 */
	int esg() const {return flag_value(36, 0x1F0000) >> 16;}
	/**
	 * Set the exclusive selection group (ESG) for the button
	 *
	 * Selecting one button that is an ESG will unselect all the others in
	 * the same ESG.
	 *
	 * @param value ESG number (0-31). 0 Means not in a group.
	 */
	void esg(int value) {flag_value(36, value << 16);}

	/**
	 * Return the foreground WIMP colour
	 *
	 * This method is only valid when anti_aliased_font() == false
	 *
	 * @returns foreground WIMP colour (0-15)
	 */
	int foreground() const {return flag_value(36,0xF000000)>>24;} // !anti_aliased_font
	/**
	 * Set the foreground WIMP colour
	 *
	 * This method is only valid when anti_aliased_font() == false
	 *
	 * @param value foreground WIMP colour (0-15)
	 */
	void foreground(int value) {flag_value(36, value << 24);}
	/**
	 * Return the background WIMP colour
	 *
	 * This method is only valid when anti_aliased_font() == false
	 *
	 * @returns background WIMP colour (0-15)
	 */
	int background() const {return flag_value(36,0xF0000000)>>28;} // !anti_aliased_font
	/**
	 * Set the background WIMP colour
	 *
	 * This method is only valid when anti_aliased_font() == false
	 *
	 * @param value background WIMP colour (0-15)
	 */
	void background(int value) {flag_value(36, value << 28);}

	/**
	 * Get the WIMP font handle for drawing text
	 *
	 * This method is only valid when anti_aliased_font() == true
	 *
	 * @return WIMP font handle
	 */
	int font_handle() const {return flag_value(36,0xFF000000) >> 24;} // anti_aliased_font
	/**
	 * Set the WIMP font handle for drawing text
	 *
	 * This method is only valid when anti_aliased_font() == true
	 *
	 * @return WIMP font handle
	 */
	void font_handle(int value) {flag_value(36, value << 24);}

	/**
	 * Get the value (text or sprite name) of the button
	 *
	 * @returns pointer to null terminate value string
	 */
	const char *value() const {return message(40);}
	/**
	 * Set the value (text or sprite name) of the button
	 *
	 * @param value pointer to null terminate value string
	 * @param max_length maximum length the value will be set to or
	 *  -1 (the default) to use the length of the value.
	 */
	void value(const char *value, int max_length = -1) {message_with_length(40, value, max_length);}
	/**
	 * Set the value (text or sprite name) of the button
	 *
	 * @param value new value
	 * @param max_length maximum length the value will be set to or
	 *  -1 (the default) to use the length of the value.
	 */
	void value(const std::string &value, int max_length = -1) {message_with_length(40, value, max_length);}
	/**
	 * Get the maximum length of the value string
	 *
	 * @returns maximum length
	 */
	int max_value() const {return int_value(44);}
	/**
	 * Get the validation (sprite name) for the button
	 *
	 * @returns pointer to null terminated validation string or 0 if no validation
	 */
	const char *validation() const {return string(48);}
	/**
	 * Set the validation (sprite name) for the button
	 *
	 * @param value new validation or 0 for none
	 * @param max_length maximum length for the validation string
	 */
	void validation(const char *value, int max_length = -1) {string_with_length(48, value, max_length);}
	/**
	 * Set the validation (sprite name) for the button
	 *
	 * @param value new validation or 0 for none
	 * @param max_length maximum length for the validation string
	 */
	void validation(const std::string &value, int max_length = -1) {string_with_length(48, value, max_length);}
	/**
	 * Get the maximum length the validation (sprite name) that can be used
	 */
	int max_validation() const {return int_value(52);}
};

}
}

#endif // TBX_RES_RESBUTTON_H

