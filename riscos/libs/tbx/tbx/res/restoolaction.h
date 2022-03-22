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

#ifndef TBX_RES_RESTOOLACTION_H
#define TBX_RES_RESTOOLACTION_H

#include "resgadget.h"

namespace tbx {
namespace res {

/**
 * Class for ToolAction gadget template
 */
class ResToolAction : public ResGadget
{

public:
	enum {TYPE_ID = 0x4014 };

	/**
	 * Construct an tool action gadget resource
	 *
	 * @param other ResGadget to copy resource from
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an tool action
	 */
	ResToolAction(const ResGadget &other) : ResGadget(other)
	{
     check_type(TYPE_ID);
	}

	/**
	 * Construct an tool action gadget resource
	 *
	 * @param other tool action to copy resource from
	 */
	ResToolAction(const ResToolAction &other) : ResGadget(other)
	{
	}

	virtual ~ResToolAction() {}

	/**
	 * Assign from a ResGadget
	 *
	 * @param other ResGadget to copy
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an tool action
	 */
	ResToolAction &operator=(const ResGadget &other)
	{
		other.check_type(TYPE_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another tool action gadget resource
	 *
	 * @param other tool action gadget resource to copy
	 */
	ResToolAction &operator=(const ResToolAction &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a tool action gadget resource.
	 *
	 * All options are false, events unset and messages are null.
	 */
	ResToolAction()
	  : ResGadget(0x4014,76)
	{
		init_string(36,0); // off_text
		init_string(44,0); // on_text
		init_string(56,0); // select_show
		init_string(64,0); // adjust_show
		init_string(68,0); // fade_text
	}

	/**
	 * Check if the select event will be generated
	 *
	 * @returns true if the select event will be generated
	 */
	bool generate_select() const {return flag(0, 1<<0);}
	/**
	 * Set if the select event will be generated
	 *
	 * @param value set to true if the select event will be generated
	 */
	void generate_select(bool value) {flag(0,1<<0,value);}
	/**
	 * Check if tool action shows text
	 *
	 * @returns true if the tool action shows text
	 */
	bool has_text() const {return flag(0, 1<<1);}
	/**
	 * Set if tool action shows text
	 *
	 * @param value set to true if the tool action shows text
	 */
	void has_text(bool value) {flag(0,1<<1,value);}
	/**
	 * Check if the tool action starts off in the on state
	 *
	 * @returns true if the tool action is on
	 */
	bool on() const {return flag(0, 1<<2);}
	/**
	 * Set if the tool action starts off in the on state
	 *
	 * @param value set to true to set the tool action on
	 */
	void on(bool value) {flag(0,1<<2,value);}
	/**
	 * Check if the tool action should automatically toggle between states
	 *
	 * @returns true if auto toggle is on
	 */
	bool auto_toggle() const {return flag(0, 1<<3);}
	/**
	 * Set if the tool action should automatically toggle between states
	 *
	 * @param value set to true to turn auto toggle on
	 */
	void auto_toggle(bool value) {flag(0,1<<3,value);}
	/**
	 * Check if there is no sprite for the pressed state
	 *
	 * @returns true if there is no pressed sprite
	 */
	bool no_pressed_sprite() const {return flag(0, 1<<4);}
	/**
	 * Set if there is no sprite for the pressed state
	 *
	 * @param value set to true if there is no pressed sprite
	 */
	void no_pressed_sprite(bool value) {flag(0,1<<4,value);}
	/**
	 * Check if button presses auto repeat
	 *
	 * @returns true if button presses auto repeat
	 */
	bool auto_repeat() const {return flag(0, 1<<8);}
	/**
	 * Set if button presses auto repeat
	 *
	 * @param value set to true to make button presses auto repeat
	 */
	void auto_repeat(bool value) {flag(0,1<<8,value);}
	/**
	 * Check if the select show object will be shown transiently
	 *
	 * @returns true if the select show object will be shown transiently
	 */
	bool show_transient() const {return flag(0, 1<<9);}
	/**
	 * Set if the select show object will be shown transiently
	 *
	 * @param value set to true if the select show object should be shown transiently
	 */
	void show_transient(bool value) {flag(0,1<<9,value);}
	/**
	 * Check if the select show object will be shown as a pop up
	 *
	 * @returns true if the select show object will be shown as a pop up
	 */
	bool show_as_pop_up() const {return flag(0, 1<<10);}
	/**
	 * Set if the select show object will be shown pop up
	 *
	 * @param value set to true if the select show object should be shown pop up
	 */
	void show_as_pop_up(bool value) {flag(0,1<<10,value);}
	/**
	 * Check if the tool action has a faded sprite provided
	 *
	 * @returns true if a faded sprite is provided
	 */
	bool has_fade_sprite() const {return flag(0, 1<<11);}
	/**
	 * Set if the tool action has a faded sprite provided
	 *
	 * @param value set to  true if a faded sprite is provided
	 */
	void has_fade_sprite(bool value) {flag(0,1<<11,value);}

	/**
	 * Get the text or sprite name shown when the tool action is off
	 *
	 * The has_text() method determines if this refers to text or a sprite
	 *
	 * @returns pointer to zero terminated text/sprite name or 0 if none
	 */
	const char *off_ident() const {return string(36);}
	/**
	 * Set the text or sprite name shown when the tool action is off
	 *
	 * The has_text() method determines if this refers to text or a sprite
	 *
	 * @param value pointer to zero terminated text/sprite name or 0 if none
	 * @param max_length The maximum length the off text will be changed to
	 * when the tool action is shown or -1 (the default) for the length of
	 * value.
	 */
	void off_ident(const char *value, int max_length = -1) {message_with_length(36, value, max_length);}
	/**
	 * Set the text or sprite name shown when the tool action is off
	 *
	 * The has_text() method determines if this refers to text or a sprite
	 *
	 * @param value text/sprite name
	 * @param max_length The maximum length the off text will be changed to
	 * when the tool action is shown or -1 (the default) for the length of
	 * value.
	 */
	void off_ident(const std::string &value, int max_length = -1) {message_with_length(36, value, max_length);}
	/**
	 * Get the maximum length of the off identifier.
	 */
	int max_off_ident() const {return int_value(40);}
	/**
	 * Get the text or sprite name shown when the tool action is on
	 *
	 * The has_text() method determines if this refers to text or a sprite
	 *
	 * @returns pointer to zero terminated text/sprite name or 0 if none
	 */
	const char *on_ident() const {return message(44);}
	/**
	 * Set the text or sprite name shown when the tool action is on
	 *
	 * The has_text() method determines if this refers to text or a sprite
	 *
	 * @param value pointer to zero terminated text/sprite name or 0 if none
	 * @param max_length The maximum length the on text will be changed to
	 * when the tool action is shown or -1 (the default) for the length of
	 * value.
	 */
	void on_ident(const char *value, int max_length = -1) {message_with_length(44, value, max_length);}
	/**
	 * Set the text or sprite name shown when the tool action is on
	 *
	 * The has_text() method determines if this refers to text or a sprite
	 *
	 * @param value text/sprite name
	 * @param max_length The maximum length the on text will be changed to
	 * when the tool action is shown or -1 (the default) for the length of
	 * value.
	 */
	void on_ident(const std::string &value, int max_length = -1) {message_with_length(44, value, max_length);}
	/**
	 * Get the maximum length of the on identifier.
	 */
	int max_on_ident() const {return int_value(48);}
	/**
	 * Get the event that will generated when the tool action is clicked
	 *
	 * @returns event ID of event generated or 0 for the default
	 */
	int click_event() const {return int_value(52);}
	/**
	 * Set the event that will generated when the tool action is clicked
	 *
	 * @param value event ID of event generated or 0 for the default
	 */
	void click_event(int value) {int_value(52,value);}
	/**
	 * Get the name of the object shown when select is clicked
	 *
	 * @returns pointer to zero terminate object name or 0 if none
	 */
	const char *select_show() const {return string(56);}
	/**
	 * Set the name of the object shown when select is clicked
	 *
	 * @param value pointer to zero terminate object name or 0 if none
	 */
	void select_show(const char *value) {string(56, value);}
	/**
	 * Set the name of the object shown when select is clicked
	 *
	 * @param value object name
	 */
	void select_show(const std::string &value) {string(56, value);}
	/**
	 * Get the event that will generated when the tool action is clicked with adjust
	 *
	 * @returns event ID of event generated or 0 for the default
	 */
	int adjust_event() const {return int_value(60);}
	/**
	 * Set the event that will generated when the tool action is clicked with adjust
	 *
	 * @param value event ID of event generated or 0 for the default
	 */
	void adjust_event(int value) {int_value(60,value);}
	/**
	 * Get the name of the object shown when adjust is clicked
	 *
	 * @returns pointer to zero terminate object name or 0 if none
	 */
	const char *adjust_show() const {return string(64);}
	/**
	 * Set the name of the object shown when adjust is clicked
	 *
	 * @param value pointer to zero terminate object name or 0 if none
	 */
	void adjust_show(const char *value) {string(64,value);}
	/**
	 * Set the name of the object shown when adjust is clicked
	 *
	 * @param value object name
	 */
	void adjust_show(const std::string &value) {string(64, value);}
	/**
	 * Get the text or sprite name for the faded tool action
	 *
	 * The has_text() method determines if this refers to text or a sprite
	 *
	 * @returns pointer to zero terminated text/sprite name or 0 if none
	 */
	const char *fade_ident() const {return message(68);}
	/**
	 * Set the text or sprite name shown when the tool action is faded
	 *
	 * The has_text() method determines if this refers to text or a sprite
	 *
	 * @param value pointer to zero terminated text/sprite name or 0 if none
	 * @param max_length The maximum length the faded text will be changed to
	 * when the tool action is shown or -1 (the default) for the length of
	 * value.
	 */
	void fade_ident(const char *value, int max_length = -1) {message_with_length(68, value, max_length);}
	/**
	 * Set the text or sprite name shown when the tool action is faded
	 *
	 * The has_text() method determines if this refers to text or a sprite
	 *
	 * @param value text/sprite name
	 * @param max_length The maximum length the faded text will be changed to
	 * when the tool action is shown or -1 (the default) for the length of
	 * value.
	 */
	void fade_ident(const std::string &value, int max_length = -1) {message_with_length(68, value, max_length);}
	/**
	 * Get the maximum length of the faded identifier.
	 */
	int max_fade() const {return int_value(72);}

};

}
}

#endif // TBX_RES_RESTOOLACTION_H

