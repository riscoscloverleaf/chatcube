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

/*
 * fileinfo.h
 *
 *  Created on: 01-Jul-2010
 *      Author: alanb
 */

#ifndef TBX_FILEINFO_H_
#define TBX_FILEINFO_H_

#include "showfullobject.h"
#include "listener.h"
#include "window.h"
#include "path.h"

namespace tbx {

// Listeners
class AboutToBeShownListener;
class HasBeenHiddenListener;

namespace res
{
    class ResFileInfo;
}

/**
 * Class to show a toolbox FileInfo object.
 * This object shows information about a file, directory or application
 */
class FileInfo : public tbx::ShowFullObject
{
public:
	enum { TOOLBOX_CLASS = 0x82ac0 };

	/**
	 * Construct creates an unassigned FileInfo.
	 *
	 * It must be assigned to a value before the other methods can be used.
	 */
	FileInfo() {};
	/**
	 * Construct a FileInfo from another FileInfo.
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other FileInfo
	 *
	 * @param other FileInfo to copy.
	 */
	FileInfo(const FileInfo &other) : ShowFullObject(other._handle) {}
	/**
	 * Construct a FileInfo from an Object that refers to a FileInfo
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other object
	 *
	 * @param other Object to copy.
	 * @throws ObjectNullException if the other object refers to a null object handle
	 * @throws ObjectClassException if the other object is not a FileInfo
	 */
	FileInfo(const Object &other) : ShowFullObject(other)	{check_toolbox_class(FileInfo::TOOLBOX_CLASS);}
	/**
	 * Create a FileInfo from the named template
	 *
	 * @param template_name The name of the template to create the object from
	 * @throws OsError if an object cannot be created from the named template
	 * @throws ObjectClassException if the template is not for a FileInfo
	 */
	FileInfo(const std::string &template_name) : ShowFullObject(template_name)	{check_toolbox_class(FileInfo::TOOLBOX_CLASS);}
	/**
	 * Create a FileInfo from an in memory template resource
	 *
	 * @param object_template The template to create the object from
	 * @throws OsError if an object cannot be created from the template
	 */
	FileInfo(const res::ResFileInfo &object_template);

	/**
	 * Assign this FileInfo from another.
	 *
	 * Both objects will then refer to the same underlying toolbox object
	 *
	 * @param other FileInfo to copy
	 * @returns reference to this FileInfo
	 */
	FileInfo &operator=(const FileInfo &other) {_handle = other.handle(); return *this;}
	/**
	 * Assign this FileInfo from an Object that refers to a FileInfo
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other object
	 *
	 * @param other Object to copy.
	 * @returns reference to this FileInfo
	 * @throws ObjectNullException if the other object refers to a null object handle
	 * @throws ObjectClassException if the other object is not a FileInfo
	 */
	FileInfo &operator=(const Object &other) {_handle = other.handle(); check_toolbox_class(TOOLBOX_CLASS); return *this;}
	/**
	 * Check if this FileInfo refers to the same underlying toolbox
	 * object as another.
	 *
	 * @param other Object to compare to
	 * @returns true if they refer to the same underlying toolbox object
	 */
	bool operator==(const Object &other) const {return (_handle == other.handle());}
	/**
	 * Check if this FileInfo does not refers to the same underlying toolbox
	 * object as another.
	 *
	 * @param other Object to compare to
	 * @returns true if they do not refer to the same underlying toolbox object
	 */
	bool operator!=(const Object &other) const {return (_handle != other.handle());}


	// Properties
	/**
	 * Return the underlying toolbox window used to implement the FileInfo object
	 */
	Window window() const {return Window((ObjectId)int_property(0));}

	/**
	 * Set to indicate the file has been modified
	 */
	void modified(bool value) {bool_property(1, value);}

	/**
	 * Read modified flag from dialogue box
	 */
	bool modified() const {return bool_property(2);}

	/**
	 * Set the file type
	 */
	void file_type(int file_type) {int_property(3, file_type);}

	/**
	 * Get the file type
	 */
	int file_type() const {return int_property(4);}


	/**
	 * Set the file name
	 */
	void file_name(std::string name) {string_property(5, name);}

	/**
	 * Return the file name
	 */
	std::string file_name() const {return string_property(6);}

	/**
	 * Return the length of the file name
	 */
	int file_name_length() const {return string_property_length(6);}

	/**
	 * Set the file size
	 */
	void file_size(int size) {int_property(7, size);}

	/**
	 * Get the file size
	 */
	int file_size() const {return int_property(8);}

	/**
	 * Set the date
	 */
	void date(const UTCTime &date);

	/**
	 * Get the date
	 */
	UTCTime date() const;

	/**
	 * Set the title
	 */
	void title(std::string name) {string_property(11, name);}

	/**
	 * Return the title
	 */
	std::string title() const {return string_property(12);}

	/**
	 * Return the length of the title
	 */
	int title_length() const {return string_property_length(12);}

	// Listeners
	void add_about_to_be_shown_listener(AboutToBeShownListener *listener);
	void remove_about_to_be_shown_listener(AboutToBeShownListener *listener);
	void add_has_been_hidden_listener(HasBeenHiddenListener *listener);
	void remove_has_been_hidden_listener(HasBeenHiddenListener *listener);
};


}

#endif /* TBX_FILEINFO_H_ */
