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

#ifndef TBX_RES_RESSCALE_H
#define TBX_RES_RESSCALE_H

#include "resobject.h"

namespace tbx {
namespace res {

/**
 * Class for Scale object template
 */
class ResScale : public ResObject
{

public:
	enum {CLASS_ID = 0x82c00 };

	/**
	 * Construct a scale resource from a ResObject
	 *
	 * @param other ResObject to construct from
	 * @throws ResObjectClassMismatch if the ResObject is not a scale resource
	 */
	ResScale(const ResObject &other) : ResObject(other)
	{
     check_class_id(CLASS_ID);
	}

	/**
	 * Construct a scale resource by copying another
	 *
	 * @param other ResScale to copy
	 */
	ResScale(const ResScale &other) : ResObject(other)
	{
	}

	virtual ~ResScale() {}

	/**
	 * Assign from a ResObject
	 *
	 * @param other ResObject to copy
	 * @throws ResObjectClassMismatch if the ResObject is not a scale resource
	 */
	ResScale &operator=(const ResObject &other)
	{
		other.check_class_id(CLASS_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another scale resource
	 *
	 * @param other button gadget resource to copy
	 */
	ResScale &operator=(const ResScale &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a scale resource
	 *
	 * All options are false, events unset and messages are null.
	 *
	 * @param name the name of this object (maximum 12 characters)
	 */
	ResScale(std::string name)
	  : ResObject(name, CLASS_ID, 100, 36+44)
	{
		min_val(10);
		max_val(400);
		step_size(1);
		init_message(16,0); // title
		init_string(24,0); // window
		std1_value(33);
		std2_value(80);
		std3_value(100);
		std4_value(120);
	}

	  /**
	   * Get all flags as a word raw access.
	   *
	   * Not normally used as there are named methods that allow access
	   * to the individual items in the flags
	   */
	  unsigned int flags() const {return uint_value(0);}

	  /**
	   * Set all flags as a word
	   *
	   * Not normally used as there are named methods that allow access
	   * to the individual items in the flags
	   */
	  void flags(unsigned int value) {uint_value(0, value);}
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
     * Check if scale to fit option is included on the dialogue
     *
     * @returns true if dialogue contains the scale to fit option
     */
	bool has_scale_to_fit() const {return flag(0, 1<<2);}
    /**
     * Set if scale to fit option is included on the dialogue
     *
     * @param value set to true to include the scale to fit option
     */
	void has_scale_to_fit(bool value) {flag(0,1<<2,value);}

	/**
	 * Get the minimum value for the scaling
	 *
	 * @returns minimum scale value percentage
	 */
	int min_val() const {return int_value(4);}
	/**
	 * Set the minimum value for the scaling
	 *
	 * @param value minimum scale value percentage
	 */
	void min_val(int value) {int_value(4,value);}
	/**
	 * Get the maximum value for the scaling
	 *
	 * @returns maximum scale value percentage
	 */
	int max_val() const {return int_value(8);}
	/**
	 * Set the maximum value for the scaling
	 *
	 * @param value maximum scale value percentage
	 */
	void max_val(int value) {int_value(8,value);}
	/**
	 * Get the step size for incrementing/decrementing
	 * the scale
	 *
	 * @returns the step size
	 */
	int step_size() const {return int_value(12);}
	/**
	 * Set the step size for incrementing/decrementing
	 * the scale
	 *
	 * @param value the step size
	 */
	void step_size(int value) {int_value(12,value);}
	/**
	 * Get the title of the dialogue
	 *
	 * @returns zero terminated string with title or 0 for the default title
	 */
	const char *title() const {return message(16);}
	/**
	 * Set the title of the dialogue
	 *
	 * @param value zero terminated string with the title or 0 for the default
	 * @param max_length maximum length the title will be changed to.
	 * -1 (the default) to use the length of the title given.
	 */
	void title(const char *value, int max_length = -1) {message_with_length(16, value, max_length);}
	/**
	 * Set the title of the dialogue
	 *
	 * @param value new title
	 * @param max_length maximum length the title will be changed to.
	 * -1 (the default) to use the length of the title given.
	 */
	void title(const std::string &value, int max_length = -1) {message_with_length(16, value, max_length);}
	/**
	 * Get the maximum size the title can be
	 */
	int max_title() const {return int_value(20);}
	/**
	 * Get the name of the window template that provides the window for this
	 * object.
	 *
	 * @returns name of window to use or 0 if default internal window will be used
	 */
	const char *window() const {return string(24);}
	/**
	 * Set the name of the window template that provides the window for this
	 * object.
	 *
	 * @param value The name of window to use or 0 if default internal window will be used
	 */
	void window(const char *value) {string(24, value);}
	/**
	 * Set the name of the window template that provides the window for this
	 * object.
	 *
	 * @param value The name of window to use or 0 if default internal window will be used
	 */
	void window(const std::string &value) {string(24, value);}
	/**
	 * Get the value of the first standard button
	 *
	 * @returns the percentage the first button will set the scale to
	 */
	int std1_value() const {return int_value(28);}
	/**
	 * Set the value of the first standard button
	 *
	 * @param value the percentage the first button will set the scale to
	 */
	void std1_value(int value) {int_value(28,value);}
	/**
	 * Get the value of the second standard button
	 *
	 * @returns the percentage the second button will set the scale to
	 */
	int std2_value() const {return int_value(32);}
	/**
	 * Set the value of the second standard button
	 *
	 * @param value the percentage the second button will set the scale to
	 */
	void std2_value(int value) {int_value(32,value);}
	/**
	 * Get the value of the third standard button
	 *
	 * @returns the percentage the third button will set the scale to
	 */
	int std3_value() const {return int_value(36);}
	/**
	 * Set the value of the third standard button
	 *
	 * @param value the percentage the third button will set the scale to
	 */
	void std3_value(int value) {int_value(36,value);}
	/**
	 * Get the value of the fourth standard button
	 *
	 * @returns the percentage the fourth button will set the scale to
	 */
	int std4_value() const {return int_value(40);}
	/**
	 * Set the value of the fourth standard button
	 *
	 * @param value the percentage the fourth button will set the scale to
	 */
	void std4_value(int value) {int_value(40,value);}
};

}
}

#endif // TBX_RES_RESSCALE_H
