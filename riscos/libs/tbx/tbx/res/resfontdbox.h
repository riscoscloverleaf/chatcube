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

#ifndef TBX_RES_RESFONTDBOX_H
#define TBX_RES_RESFONTDBOX_H

#include "resobject.h"

namespace tbx {
namespace res {

/**
 * Class for FontDbox object template
 */
class ResFontDbox : public ResObject
{

public:
	enum {CLASS_ID = 0x82a00 };

	/**
	 * Construct a font dialogue resource from a ResObject
	 *
	 * @param other ResObject to construct from
	 * @throws ResObjectClassMismatch if the ResObject is not a font dialogue resource
	 */
	ResFontDbox(const ResObject &other) : ResObject(other)
	{
     check_class_id(CLASS_ID);
	}

	/**
	 * Construct a font dialogue resource by copying another
	 *
	 * @param other ResFontDbox to copy
	 */
	ResFontDbox(const ResFontDbox &other) : ResObject(other)
	{
	}

	virtual ~ResFontDbox() {}

	/**
	 * Assign from a ResObject
	 *
	 * @param other ResObject to copy
	 * @throws ResObjectClassMismatch if the ResObject is not a font dialogue resource
	 */
	ResFontDbox &operator=(const ResObject &other)
	{
		other.check_class_id(CLASS_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another font dialogue resource
	 *
	 * @param other button gadget resource to copy
	 */
	ResFontDbox &operator=(const ResFontDbox &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a font dialogue resource
	 *
	 * All options are false, events unset and messages are null.
	 *
	 * @param name the name of this object (maximum 12 characters)
	 */
	ResFontDbox(std::string name)
	  : ResObject(name, CLASS_ID, 100, 36 + 32)
	{
		init_message(4,0); // title
		init_string(12,0); // initial_font
		initial_height(12);
		initial_aspect(100);
		init_message(24,"The quick brown fox jumps over the lazy dog"); // try_string
		init_string(28,0); // window
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
	 * Check if the dialogue completed event should be generated
	 *
	 * @returns true if the dialogue completed event should be generated
	 */
	bool generate_dialogue_completed() const {return flag(0, 1<<1);}
	/**
	 * Set if the dialogue completed event should be generated
	 *
	 * @param value set to true if the dialogue completed event should be generated
	 */
	void generate_dialogue_completed(bool value) {flag(0,1<<1,value);}
	/**
	 * Check if the system font should be included in the dialogue
	 *
	 * @returns true if the system font should be included
	 */
	bool include_system_font() const {return flag(0, 1<<2);}
	/**
	 * Set if the system font should be included in the dialogue
	 *
	 * @param value set to true if the system font should be included
	 */
	void include_system_font(bool value) {flag(0,1<<2,value);}

	/**
	 * Get the title of the dialogue
	 *
	 * @returns zero terminated string with title or 0 for the default title
	 */
	const char *title() const {return message(4);}
	/**
	 * Set the title of the dialogue
	 *
	 * @param value zero terminated string with the title or 0 for the default
	 * @param max_length maximum length the title will be changed to.
	 * -1 (the default) to use the length of the title given.
	 */
	void title(const char *value, int max_length = -1) {message_with_length(4, value, max_length);}
	/**
	 * Set the title of the dialogue
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
	 * Get the name of the selected font
	 *
	 * @returns pointer to zero terminated name of font to select, "SystemFont" for the System entry or 0 for the default
	 */
	const char *initial_font() const {return string(12);}
	/**
	 * Set the name of the selected font
	 *
	 * @param value pointer to zero terminated name of font to select, "SystemFont" for the System entry or 0 for the default
	 */
	void initial_font(const char *value) {string(12, value);}
	/**
	 * Set the name of the selected font
	 *
	 * @param value name of font to select, "SystemFont" for the System entry
	 */
	void initial_font(const std::string &value) {string(12, value);}
	/**
	 * Get the initial height in the dialogue
	 *
	 * @returns initial height (in points)
	 */
	int initial_height() const {return int_value(16);}
	/**
	 * Set the initial height in the dialogue
	 *
	 * @param value initial height (in points)
	 */
	void initial_height(int value) {int_value(16,value);}
	/**
	 * Get the aspect ratio of the font
	 *
	 * @returns aspect ratio (percentage)
	 */
	int initial_aspect() const {return int_value(20);}
	/**
	 * Set the aspect ratio of the font
	 *
	 * @param value the initial aspect ratio (percentage)
	 */
	void initial_aspect(int value) {int_value(20,value);}
	/**
	 * Get the string that is used to display an example of the
	 * currently selected font.
	 *
	 * @returns pointer to zero terminated string or 0 if default is used
	 */
	const char *try_string() const {return message(24);}
	/**
	 * Set the string that is used to display an example of the
	 * currently selected font.
	 *
	 * @param value pointer zero terminated try string or 0 if the default
	 * is to be used
	 */
	void try_string(const char *value) {message(24, value);}
	/**
	 * Set the string that is used to display an example of the
	 * currently selected font.
	 *
	 * @param value pointer zero terminated try string
	 */
	void try_string(const std::string &value) {message(24, value);}
	/**
	 * Get the name of the window template that provides the window for this
	 * object.
	 *
	 * @returns name of window to use or 0 if default internal window will be used
	 */
	const char *window() const {return string(28);}
	/**
	 * Set the name of the window template that provides the window for this
	 * object.
	 *
	 * @param value The name of window to use or 0 if default internal window will be used
	 */
	void window(const char *value) {string(28, value);}
	/**
	 * Set the name of the window template that provides the window for this
	 * object.
	 *
	 * @param value The name of window to use or 0 if default internal window will be used
	 */
	void window(const std::string &value) {string(28, value);}

};

}
}

#endif // TBX_RES_RESFONTDBOX_H


