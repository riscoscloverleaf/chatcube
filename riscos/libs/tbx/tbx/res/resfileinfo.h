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

#ifndef TBX_RES_RESFILEINFO_H
#define TBX_RES_RESFILEINFO_H

#include "resobject.h"

#include "../path.h"

namespace tbx {
namespace res {

/**
 * Class for FileInfo object template
 */
class ResFileInfo : public ResObject
{

public:
	enum {CLASS_ID = 0x82ac0 };

	/**
	 * Construct a file information window resource from a ResObject
	 *
	 * @param other ResObject to construct from
	 * @throws ResObjectClassMismatch if the ResObject is not a file information window resource
	 */
	ResFileInfo(const ResObject &other) : ResObject(other)
	{
     check_class_id(CLASS_ID);
	}

	/**
	 * Construct a file information window resource by copying another
	 *
	 * @param other ResFileInfo to copy
	 */
	ResFileInfo(const ResFileInfo &other) : ResObject(other)
	{
	}

	virtual ~ResFileInfo() {}

	/**
	 * Assign from a ResObject
	 *
	 * @param other ResObject to copy
	 * @throws ResObjectClassMismatch if the ResObject is not a file information window resource
	 */
	ResFileInfo &operator=(const ResObject &other)
	{
		other.check_class_id(CLASS_ID);
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Assign from another file information window resource
	 *
	 * @param other button gadget resource to copy
	 */
	ResFileInfo &operator=(const ResFileInfo &other)
	{
		ResBase::operator=(other);
		return *this;
	}

	/**
	 * Construct a file information window resource
	 *
	 * All options are false, events unset and messages are null.
	 *
	 * @param name the name of this object (maximum 12 characters)
	 */
	ResFileInfo(std::string name)
	  : ResObject(name, CLASS_ID, 100, 36+40)
	{
		init_message(4,0); // title
		init_message(20,0); // file name
		filesize(1024);
		init_string(36,0); // window
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
	 * Get modified state
	 *
	 * @returns non-zero if modified
	 */
	int modified() const {return int_value(12);}
	/**
	 * Set modified state
	 *
	 * @param value set to any non-zero value if modified
	 */
	void modified(int value) {int_value(12,value);}

	/**
	 * Get the initial file type for the file info resource
	 *
	 * @returns file type
	 */
	int file_type() const {return int_value(16);}
	/**
	 * Set the initial file type for the file info resource
	 *
	 * @param value new file type
	 */
	void file_type(int value) {int_value(16,value);}
	/**
	 * Get the initial file name for the file info
	 *
	 * @returns pointer to zero terminated file name string or 0 for the default.
	 */
	const char *file_name() const {return message(20);}
	/**
	 * Set the initial file name for the file info
	 *
	 * @param value pointer to zero terminated file name string or 0 for the default.
	 */
	void file_name(const char *value) {message(20, value);}
	/**
	 * Set the initial file name for the file info
	 *
	 * @param value new file name
	 */
	void file_name(const std::string &value) {message(20, value);}
	/**
	 * Get the file size reported by the file info
	 *
	 * @return file size in bytes
	 */
	int filesize() const {return int_value(24);}
	/**
	 * Set the file size reported by the file info
	 *
	 * @param value file size in bytes
	 */
	void filesize(int value) {int_value(24,value);}

	/**
	 * Get the data for the file info
	 *
	 * @returns UTCTime containing the date in the file info
	 */
	UTCTime date() const {return UTCTime(int_value(28), int_value(32));}
	/**
	 * Set the data for the file info
	 *
	 * @param value UTCTime containing the new date
	 */
	void date(UTCTime value) {int_value(28,value.centiseconds() & 0xFFFFFFFF); int_value(32, (value.centiseconds() >> 32));}
	/**
	 * Get the name of the window template that provides the window for this
	 * object.
	 *
	 * @returns name of window to use or 0 if default internal window will be used
	 */
	const char *window() const {return string(36);}
	/**
	 * Set the name of the window template that provides the window for this
	 * object.
	 *
	 * @param value The name of window to use or 0 if default internal window will be used
	 */
	void window(const char *value) {string(36, value);}
	/**
	 * Set the name of the window template that provides the window for this
	 * object.
	 *
	 * @param value The name of window to use or 0 if default internal window will be used
	 */
	void window(const std::string &value) {string(36, value);}

};

}
}

#endif // TBX_RES_RESFILEINFO_H

