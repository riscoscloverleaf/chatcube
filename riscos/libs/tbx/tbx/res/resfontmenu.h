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

#ifndef TBX_RES_RESFONTMENU_H
#define TBX_RES_RESFONTMENU_H

#include "resobject.h"

namespace tbx {
namespace res {

/**
 * Class for FontMenu object template
 */
class ResFontMenu : public ResObject
{

public:
	enum {CLASS_ID = 0x82a40 };

	/**
	 * Construct a font menu resource from a ResObject
	 *
	 * @param other ResObject to construct from
	 * @throws ResObjectClassMismatch if the ResObject is not a font menu resource
	 */
	ResFontMenu(const ResObject &other) : ResObject(other)
	{
     check_class_id(CLASS_ID);
	}

	/**
	 * Construct a font menu resource by copying another
	 *
	 * @param other ResFontMenu to copy
	 */
	ResFontMenu(const ResFontMenu &other) : ResObject(other)
	{
	}

	virtual ~ResFontMenu() {}

	/**
	 * Assign from a ResObject
	 *
	 * @param other ResObject to copy
	 * @throws ResObjectClassMismatch if the ResObject is not a font menu resource
	 */
	ResFontMenu &operator=(const ResObject &other)
	{
		other.check_class_id(CLASS_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another font menu resource
	 *
	 * @param other button gadget resource to copy
	 */
	ResFontMenu &operator=(const ResFontMenu &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a font menu resource
	 *
	 * All options are false, events unset and messages are null.
	 *
	 * @param name the name of this object (maximum 12 characters)
	 */
	ResFontMenu(std::string name)
	  : ResObject(name, CLASS_ID, 100, 36 + 8)
	{
		init_string(4,0); // ticked_font
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
	 * Check if an option to select the system font should be included
	 *
	 * @returns true if the system font option should be included
	 */
	bool include_system_font() const {return flag(0, 1<<2);}
	/**
	 * Set if an option to select the system font should be included
	 *
	 * @param value set to true if the system font option should be included
	 */
	void include_system_font(bool value) {flag(0,1<<2,value);}

	/**
	 * Get the name of the font that should be selected
	 *
	 * @return pointer to zero terminated name of initial font selected
	 * ("SystemFont" for the system font) or 0 if no font is selected.
	 */
	const char *ticked_font() const {return string(4);}
	/**
	 * Set the name of the font that should be selected
	 *
	 * @param value pointer to zero terminated name of initial font selected
	 * ("SystemFont" for the system font) or 0 if no font is selected.
	 */
	void ticked_font(const char *value) {string(4, value);}
	/**
	 * Set the name of the font that should be selected
	 *
	 * @param value pointer to zero terminated name of initial font selected
	 * ("SystemFont" for the system font).
	 */
	void ticked_font(const std::string &value) {string(4, value);}

};

}
}

#endif // TBX_RES_RESFONTMENU_H
