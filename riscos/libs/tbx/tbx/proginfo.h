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
 * proginfo.h
 *
 *  Created on: 01-Jul-2010
 *      Author: alanb
 */

#ifndef TBX_PROGINFO_H_
#define TBX_PROGINFO_H_

#include "showfullobject.h"
#include "listener.h"
#include "window.h"

namespace tbx {

// Listeners
class AboutToBeShownListener;
class HasBeenHiddenListener;

namespace res
{
    class ResProgInfo;
}


/**
 * Class to show a toolbox ProgInfo object.
 * The ProgInfo object shows information about the application.
 * It is usually shown from the applications iconbar menu
 */
class ProgInfo : public tbx::ShowFullObject
{
public:
	enum { TOOLBOX_CLASS = 0x82b40 };

	/**
	 * Construct creates an unassigned ProgInfo.
	 *
	 * It must be assigned to a value before the other methods can be used.
	 */
	ProgInfo() {};
	/**
	 * Construct a ProgInfo from another ProgInfo.
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other ProgInfo
	 *
	 * @param other ProgInfo to copy.
	 */
	ProgInfo(const ProgInfo &other) : ShowFullObject(other._handle) {}
	/**
	 * Construct a ProgInfo from an Object that refers to a ProgInfo
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other object
	 *
	 * @param other Object to copy.
	 * @throws ObjectNullException if the other object refers to a null object handle
	 * @throws ObjectClassException if the other object is not a ProgInfo
	 */
	ProgInfo(const Object &other) : ShowFullObject(other)	{check_toolbox_class(ProgInfo::TOOLBOX_CLASS);}
	/**
	 * Create a ProgInfo from the named template
	 *
	 * @param template_name The name of the template to create the object from
	 * @throws OsError if an object cannot be created from the named template
	 * @throws ObjectClassException if the template is not for a ProgInfo
	 */
	ProgInfo(const std::string &template_name) : ShowFullObject(template_name)	{check_toolbox_class(ProgInfo::TOOLBOX_CLASS);}
	/**
	 * Create a ProgInfo from an in memory template resource
	 *
	 * @param object_template The template to create the object from
	 * @throws OsError if an object cannot be created from the template
	 */
	ProgInfo(const res::ResProgInfo &object_template);

	/**
	 * Assign this ProgInfo from another.
	 *
	 * Both objects will then refer to the same underlying toolbox object
	 *
	 * @param other ProgInfo to copy
	 * @returns reference to this ProgInfo
	 */
	ProgInfo &operator=(const ProgInfo &other) {_handle = other.handle(); return *this;}
	/**
	 * Assign this ProgInfo from an Object that refers to a ProgInfo
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other object
	 *
	 * @param other Object to copy.
	 * @returns reference to this ProgInfo
	 * @throws ObjectNullException if the other object refers to a null object handle
	 * @throws ObjectClassException if the other object is not a ProgInfo
	 */
	ProgInfo &operator=(const Object &other) {_handle = other.handle(); check_toolbox_class(TOOLBOX_CLASS); return *this;}
	/**
	 * Check if this ProgInfo refers to the same underlying toolbox
	 * object as another.
	 *
	 * @param other Object to compare to
	 * @returns true if they refer to the same underlying toolbox object
	 */
	bool operator==(const Object &other) const {return (_handle == other.handle());}
	/**
	 * Check if this ProgInfo does not refers to the same underlying toolbox
	 * object as another.
	 *
	 * @param other Object to compare to
	 * @returns true if they do not refer to the same underlying toolbox object
	 */
	bool operator!=(const Object &other) const {return (_handle != other.handle());}


	// Properties
	/**
	 * Return the underlying toolbox window used to implement the ProgInfo object
	 */
	Window window() const {return Window((ObjectId)int_property(0));}

	/**
	 * Set version
	 *
	 * @param value version string
	 */
	void version(std::string value) {string_property(1, value);}

	/**
	 * Return version string

	 */
	std::string version() const {return string_property(2);}

	/**
	 * Return version string length

	 */
	int version_length() const {return string_property_length(2);}

	/**
	 * Enumeration of license types
	 */
	enum LicenceType
	{
		PUBLIC_DOMAIN,
		SINGLE_USER,
		SINGLE_MACHINE,
		SITE,
		NETWORK,
		AUTHORIY
	};

	/**
	 * Set the licence type
	 */
	void licence_type(LicenceType type) {int_property(3, type);}

	/**
	 * Get the licence type
	 */
	LicenceType licence_type() const {return (LicenceType)int_property(4);}

	/**
	 * Set the title
	 */
	void title(std::string value) {string_property(5, value);}

	/**
	 * Return the title
	 */
	std::string title() const {return string_property(6);}

	/**
	 * Return the title length
	 */
	int title_length() const {return string_property_length(6);}

	// Listeners
	void add_about_to_be_shown_listener(AboutToBeShownListener *listener);
	void remove_about_to_be_shown_listener(AboutToBeShownListener *listener);
	void add_has_been_hidden_listener(HasBeenHiddenListener *listener);
	void remove_has_been_hidden_listener(HasBeenHiddenListener *listener);
};


}

#endif /* TBX_PROGINFO_H_ */
