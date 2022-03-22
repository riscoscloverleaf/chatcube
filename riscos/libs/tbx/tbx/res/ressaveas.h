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

#ifndef TBX_RES_RESSAVEAS_H
#define TBX_RES_RESSAVEAS_H

#include "resobject.h"

namespace tbx {
namespace res {

/**
 * Class for SaveAs object template
 */
class ResSaveAs : public ResObject
{

public:
	enum {CLASS_ID = 0x82bc0 };

	/**
	 * Construct a save as resource from a ResObject
	 *
	 * @param other ResObject to construct from
	 * @throws ResObjectClassMismatch if the ResObject is not a save as resource
	 */
	ResSaveAs(const ResObject &other) : ResObject(other)
	{
     check_class_id(CLASS_ID);
	}

	/**
	 * Construct a save as resource by copying another
	 *
	 * @param other ResSaveAs to copy
	 */
	ResSaveAs(const ResSaveAs &other) : ResObject(other)
	{
	}

	virtual ~ResSaveAs() {}

	/**
	 * Assign from a ResObject
	 *
	 * @param other ResObject to copy
	 * @throws ResObjectClassMismatch if the ResObject is not a save as resource
	 */
	ResSaveAs &operator=(const ResObject &other)
	{
		other.check_class_id(CLASS_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another save as resource
	 *
	 * @param other button gadget resource to copy
	 */
	ResSaveAs &operator=(const ResSaveAs &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a save as resource
	 *
	 * All options are false, events unset and messages are null.
	 *
	 * @param name the name of this object (maximum 12 characters)
	 */
	ResSaveAs(std::string name)
	  : ResObject(name, CLASS_ID, 100, 36 + 24)
	{
		flags(12); // use buffer and no selection
		init_message(12,0); // title
		init_message(4, "Untitled"); // file name
		init_string(20,0); // window
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
	 * Check if the selection option is omitted from the dialogue
	 *
	 * @returns true if the selection option is not shown
	 */
	bool no_selection() const {return flag(0, 1<<2);}
	/**
	 * Set if the selection option is omitted from the dialogue
	 *
	 * @param value Set to true to suppress the selection option
	 */
	void no_selection(bool value) {flag(0,1<<2,value);}
	/**
	 * Check if the save as will use a single provided buffer
	 *
	 * @returns true if the transfer use a single buffer
	 */
	bool use_buffer() const {return flag(0, 1<<3);}
	/**
	 * Set if the save as will use a single provided buffer
	 *
	 * @param value set to true to use a single buffer
	 */
	void use_buffer(bool value) {flag(0,1<<3,value);}
	/**
	 * Check if save will use the RAM transfer protocol
	 *
	 * @returns true if RAM transfer will be used
	 */
	bool ram_transfer() const {return flag(0, 1<<4);}
	/**
	 * Set if save will use the RAM transfer protocol
	 *
	 * @param value set to true if RAM transfer will be used
	 */
	void ram_transfer(bool value) {flag(0,1<<4,value);}

	/**
	 * Get the initial file name for the dialogue
	 *
	 * @return pointer to zero terminated file name or 0 to use the default
	 */
	const char *file_name() const {return message(4);}
	/**
	 * Set the initial file name for the dialogue
	 *
	 * @param value pointer to zero terminated file name or 0 to use the default
	 */
	void file_name(const char *value) {message(4, value);}
	/**
	 * Set the initial file name for the dialogue
	 *
	 * @param value initial file name
	 */
	void file_name(const std::string &value) {message(4, value);}
	/**
	 * Get the file type for the dialogue
	 *
	 * @returns file type for files saved from the dialogue
	 */
	int file_type() const {return int_value(8);}
	/**
	 * Set the file type for the dialogue
	 *
	 * @param value set the file type for files saved from the dialogue
	 */
	void file_type(int value) {int_value(8,value);}
	/**
	 * Get the title of the dialogue
	 *
	 * @returns zero terminated string with title or 0 for the default title
	 */
	const char *title() const {return message(12);}
	/**
	 * Set the title of the dialogue
	 *
	 * @param value zero terminated string with the title or 0 for the default
	 * @param max_length maximum length the title will be changed to.
	 * -1 (the default) to use the length of the title given.
	 */
	void title(const char *value, int max_length = -1) {message_with_length(12, value, max_length);}
	/**
	 * Set the title of the dialogue
	 *
	 * @param value new title
	 * @param max_length maximum length the title will be changed to.
	 * -1 (the default) to use the length of the title given.
	 */
	void title(const std::string &value, int max_length = -1) {message_with_length(12, value, max_length);}
	/**
	 * Get the maximum size the title can be
	 */
	int max_title() const {return int_value(16);}
	/**
	 * Get the name of the window template that provides the window for this
	 * object.
	 *
	 * @returns name of window to use or 0 if default internal window will be used
	 */
	const char *window() const {return string(20);}
	/**
	 * Set the name of the window template that provides the window for this
	 * object.
	 *
	 * @param value The name of window to use or 0 if default internal window will be used
	 */
	void window(const char *value) {string(20, value);}
	/**
	 * Set the name of the window template that provides the window for this
	 * object.
	 *
	 * @param value The name of window to use or 0 if default internal window will be used
	 */
	void window(const std::string &value) {string(20, value);}

};

}
}

#endif // TBX_RES_RESSAVEAS_H
