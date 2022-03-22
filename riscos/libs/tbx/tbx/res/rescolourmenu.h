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

#ifndef TBX_RES_RESCOLOURMENU_H
#define TBX_RES_RESCOLOURMENU_H

#include "resobject.h"

namespace tbx {
namespace res {

/**
 * Class for ColourMenu object template
 */
class ResColourMenu : public ResObject
{

public:
	enum {CLASS_ID = 0x82980 };

	/**
	 * Construct a colour menu resource from a ResObject
	 *
	 * @param other ResObject to construct from
	 * @throws ResObjectClassMismatch if the ResObject is not a colour menu resource
	 */
	ResColourMenu(const ResObject &other) : ResObject(other)
	{
     check_class_id(CLASS_ID);
	}

	/**
	 * Construct a colour menu resource by copying another
	 *
	 * @param other ResColourMenu to copy
	 */
	ResColourMenu(const ResColourMenu &other) : ResObject(other)
	{
	}

	virtual ~ResColourMenu() {}

	/**
	 * Assign from a ResObject
	 *
	 * @param other ResObject to copy
	 * @throws ResObjectClassMismatch if the ResObject is not a colour menu resource
	 */
	ResColourMenu &operator=(const ResObject &other)
	{
		other.check_class_id(CLASS_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another colour menu resource
	 *
	 * @param other button gadget resource to copy
	 */
	ResColourMenu &operator=(const ResColourMenu &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a colour menu resource
	 *
	 * All options are false, events unset and messages are null.
	 *
	 * @param name the name of this object (maximum 12 characters)
	 */
	ResColourMenu(std::string name)
	  : ResObject(name, CLASS_ID, 100, 36 + 16)
	{
		init_message(4,0); // title
	}

	/**
	 * Raw access to all the object flags.
	 *
	 * It is recommended you use the other methods in this
	 * class rather than use this directly.
	 *
	 * @returns integer with individual bits specifying various flags
	 */
	int flags() const {return int_value(0);}
	/**
	 * Set all the flags
	 *
	 * It is recommended you use the other methods in this
	 * class rather than use this directly.
	 *
	 * @param value the new flags
	 */
	void flags(int value) {int_value(0,value);}
	/**
	 * Check if the about to be shown event should be generated
	 *
	 * @returns true if the about to be shown event should be generated
	 */
	bool generate_about_to_be_shown() const {return flag(0, 1<<0);}
	/**
	 * Set if the about to be shown event should be generated
	 *
	 * @param value set to true if the about to be shown event should be generated
	 */
	void generate_about_to_be_shown(bool value) {flag(0,1<<0,value);}
	/**
	 * Check if the has been hidden event should be generated
	 *
	 * @returns true if the has been hidden event should be generated
	 */
	bool generate_has_been_hidden() const {return flag(0, 1<<1);}
	/**
	 * Set if the has been hidden event should be generated
	 *
	 * @param value set to true if the has been hidden event should be generated
	 */
	void generate_has_been_hidden(bool value) {flag(0,1<<1,value);}
	/**
	 * Check if an option to select no colour should be included
	 *
	 * @returns true if the no colour option should be included
	 */
	bool include_none() const {return flag(0, 1<<2);}
	/**
	 * Set if an option to select no colour should be included
	 *
	 * @param value set to true if the no colour option should be included
	 */
	void include_none(bool value) {flag(0,1<<2,value);}

	/**
	 * Get the title of the menu
	 *
	 * @returns zero terminated string with title or 0 for the default title
	 */
	const char *title() const {return message(4);}
	/**
	 * Set the title of the menu
	 *
	 * @param value zero terminated string with the title or 0 for the default
	 * @param max_length maximum length the title will be changed to.
	 * -1 (the default) to use the length of the title given.
	 */
	void title(const char *value, int max_length = -1) {message_with_length(4, value, max_length);}
	/**
	 * Set the title of the menu
	 *
	 * @param value new title
	 * @param max_length maximum length the title will be changed to.
	 * -1 (the default) to use the length of the title given.
	 */
	void title(const std::string &value, int max_length = -1) {message_with_length(4, value, max_length);}
	/**
	 * Get the maximum size the title can be
	 */
	int max_title() const {return int_value(8);}
	/**
	 * Get the initial selected colour
	 *
	 * @returns colour as an RGB value packed into an integer
	 */
	int colour() const {return int_value(12);}
	/**
	 * Set the initial selected colour
	 *
	 * @param value colour as an RGB value packed into an integer
	 */
	void colour(int value) {int_value(12,value);}
};

}
}

#endif // TBX_RES_RESCOLOURMENU_H

