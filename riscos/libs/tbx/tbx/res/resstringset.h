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

#ifndef TBX_RES_RESSTRINGSET_H
#define TBX_RES_RESSTRINGSET_H

#include "resgadget.h"
#include "resjustification.h"
#include "../handles.h"

namespace tbx {
namespace res {

/**
 * Class for StringSet gadget template
 */
class ResStringSet : public ResGadget
{

public:
	enum {TYPE_ID = 896 };

	/**
	 * Construct an string set gadget resource
	 *
	 * @param other ResGadget to copy resource from
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an string set
	 */
	ResStringSet(const ResGadget &other) : ResGadget(other)
	{
     check_type(TYPE_ID);
	}

	/**
	 * Construct an string set gadget resource
	 *
	 * @param other string set to copy resource from
	 */
	ResStringSet(const ResStringSet &other) : ResGadget(other)
	{
	}

	virtual ~ResStringSet() {}

	/**
	 * Assign from a ResGadget
	 *
	 * @param other ResGadget to copy
	 * @throws ResGadgetTypeMismatch if the ResGadget is not an string set
	 */
	ResStringSet &operator=(const ResGadget &other)
	{
		other.check_type(TYPE_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another string set gadget resource
	 *
	 * @param other string set gadget resource to copy
	 */
	ResStringSet &operator=(const ResStringSet &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a string set gadget resource.
	 *
	 * All options are false, events unset and messages are null.
	 */
	ResStringSet()
	  : ResGadget(896,68)
	{
		init_message(36,0); // string_set
		init_message(40,0); // Title
		init_message(44,0); // initial_selected_string
		init_message(52,0); // allowable
		before(-1);
		after(-1);
	}

	/**
	 * Check if the value changed event will be generated
	 *
	 * @returns true if the event will be generated
	 */
	bool generate_value_changed() const {return flag(0, 1<<0);}
	/**
	 * Set if the value changed event will be generated
	 *
	 * @param value set to true to generate the event
	 */
	void generate_value_changed(bool value) {flag(0,1<<0,value);}
	/**
	 * Check if user can enter values in the string set
	 *
	 * @returns true if string set is writable
	 */
	bool writable() const {return flag(0, 1<<1);}
	/**
	 * Set if user can enter values in the string set
	 *
	 * @param value set to true to make the string set is writable
	 */
	void writable(bool value) {flag(0,1<<1,value);}
	/**
	 * Check if an event will be generated before the menu is shown
	 *
	 * @returns true if the event will be generated
	 */
	bool generate_event_before_showing_menu() const {return flag(0, 1<<3);}
	/**
	 * Set if an event will be generated before the menu is shown
	 *
	 * @param value set to true to generate the event
	 */
	void generate_event_before_showing_menu(bool value) {flag(0,1<<3,value);}
	/**
	 * Check if string set has no display or writable field
	 *
	 * @returns true if no value field is shown
	 */
	bool no_value_field() const {return flag(0, 1<<4);}
	/**
	 * Set if string set has no display or writable field
	 *
	 * @param value set to true if no value field should be shown
	 */
	void no_value_field(bool value) {flag(0,1<<4,value);}
	/**
	 * Get justification of the value
	 *
	 * @returns ResJustification value specifying justification
	 */
	ResJustification justification() const {return ResJustification(flag_value(0, 96)>>5);}
	/**
	 * Set justification of the value
	 *
	 * @param value ResJustification value specifying justification
	 */
	void justification(ResJustification value) {flag_value(0,96,(int)value<<5);}

	/**
	 * Get the set of values shown in the string set menu
	 *
	 * The values are separated by commas. To include a comma use "\,"
	 * to include a backslash use "\\".
	 *
	 * @returns pointer to zero terminated list of strings or 0 if none
	 */
	const char *string_set() const {return message(36);}
	/**
	 * Set the set of values shown in the string set menu
	 *
	 * The values are separated by commas. To include a comma use "\,"
	 * to include a backslash use "\\".
	 *
	 * @param value pointer to zero terminated list of strings or 0 if none
	 */
	void string_set(const char *value) {message(36, value);}
	/**
	 * Set the set of values shown in the string set menu
	 *
	 * The values are separated by commas. To include a comma use "\,"
	 * to include a backslash use "\\".
	 *
	 * @param value list of strings
	 */
	void string_set(const std::string &value) {message(36, value);}

	/**
	 * Get the title for the menu
	 *
	 * @returns pointer to zero terminated menu title or 0 if none
	 */
	const char *title() const {return message(40);}
	/**
	 * Set the title for the menu
	 *
	 * @param value pointer to zero terminated menu title or 0 if none
	 */
	void title(const char *value) {message(40, value);}
	/**
	 * Set the title for the menu
	 *
	 * @param value menu title
	 */
	void title(const std::string &value) {message(40, value);}
	/**
	 * Get the initially selected string
	 *
	 * @returns pointer to zero terminated string or 0 if none
	 */
	const char *selected_string() const {return message(44);}
	/**
	 * Set the initially selected string
	 *
	 * @param value pointer to zero terminated string or 0 if none
	 * @param max_length The maximum length the selected string can be increased
	 * to when using the string set or -1 (the default) to use the length of value.
	 */
	void selected_string(const char *value, int max_length = -1) {message_with_length(44, value, max_length);}
	/**
	 * Set the initially selected string
	 *
	 * @param value selected string
	 * @param max_length The maximum length the selected string can be increased
	 * to when using the string set or -1 (the default) to use the length of value.
	 */
	void selected_string(const std::string &value, int max_length = -1) {message_with_length(44, value, max_length);}
	/**
	 * Get the maximum length the selected string can be increased
	 * to when using the string set
	 */
	int max_selected_string() const {return int_value(48);}
	/**
	 * Get the allowable characters in a writable string set
	 *
	 * @see tbx::WritableField::allowable()
	 *
	 * @returns pointer to zero terminated string of allowable characters or 0
	 * if there is no restriction
	 */
	const char *allowable() const {return message(52);}
	/**
	 * Set the allowable characters in a writable string set
	 *
	 * @see tbx::WritableField::allowable()
	 *
	 * @param value pointer to zero terminated string of allowable characters or 0
	 * if there is no restriction
	 * @param max_length maximum length the allowable character can be changed
	 * to when the gadget is shown or -1 (the default) for the length of
	 * value.
	 */
	void allowable(const char *value, int max_length = -1) {message_with_length(52, value, max_length);}
	/**
	 * Set the allowable characters in a writable string set
	 *
	 * @see tbx::WritableField::allowable()
	 *
	 * @param value string of allowable characters
	 * @param max_length maximum length the allowable character can be changed
	 * to when the gadget is shown or -1 (the default) for the length of
	 * value.
	 */
	void allowable(const std::string &value, int max_length = -1) {message_with_length(52, value, max_length);}
	/**
	 * Get the maximum length the allowable character can be changed
	 * to when the gadget is shown.
	 */
	int max_allowable() const {return int_value(56);}
	/**
	 * Get the component ID of the gadget before this one in the Tab order
	 *
	 * @returns component ID of previous gadget
	 */
	ComponentId before() const {return ComponentId(int_value(60));}
	/**
	 * Set the component ID of the gadget before this one in the Tab order
	 *
	 * @param value component ID of previous gadget
	 */
	void before(ComponentId value) {int_value(60,(int)value);}
	/**
	 * Get the component ID of the gadget after this one in the Tab order
	 *
	 * @returns component ID of next gadget
	 */
	ComponentId after() const {return ComponentId(int_value(64));}
	/**
	 * Set the component ID of the gadget after this one in the Tab order
	 *
	 * @param value component ID of next gadget
	 */
	void after(ComponentId value) {int_value(64,(int)value);}

};

}
}

#endif // TBX_RES_RESSTRINGSET_H

