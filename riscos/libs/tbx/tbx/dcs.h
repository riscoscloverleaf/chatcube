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
 * dcs.h
 *
 *  Created on: 05-Jun-2009
 *      Author: alanb
 */

#ifndef TBX_DCS_H_
#define TBX_DCS_H_

#include "showfullobject.h"
#include "listener.h"
#include "eventinfo.h"
#include "window.h"

namespace tbx {

// Listeners
class AboutToBeShownListener;
class HasBeenHiddenListener;
class DCSDiscardListener;
class DCSSaveListener;
class DCSCancelListener;

namespace res
{
    class ResDCS;
}

/**
 * Class to show a toolbox DCS object that gives
 * the user an option to Discard, Cancel or Save
 * a file.
 *
 * This is usually used when someone tries to close a document that has
 * been modified, but not saved.
 */
class DCS : public tbx::ShowFullObject
{
public:
	enum { TOOLBOX_CLASS = 0x82a80 };

	/**
	 * Construct creates an unassigned DCS.
	 *
	 * It must be assigned to a value before the other methods can be used.
	 */
	DCS() {};
	/**
	 * Construct a DCS from another DCS.
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other DCS
	 *
	 * @param other DCS to copy.
	 */
	DCS(const DCS &other) : ShowFullObject(other._handle) {}
	/**
	 * Construct a DCS from an Object that refers to a DCS
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other object
	 *
	 * @param other Object to copy.
	 * @throws ObjectNullException if the other object refers to a null object handle
	 * @throws ObjectClassException if the other object is not a DCS
	 */
	DCS(const Object &other) : ShowFullObject(other)	{check_toolbox_class(DCS::TOOLBOX_CLASS);}
	/**
	 * Create a DCS from the named template
	 *
	 * @param template_name The name of the template to create the object from
	 * @throws OsError if an object cannot be created from the named template
	 * @throws ObjectClassException if the template is not for a DCS
	 */
	DCS(const std::string &template_name) : ShowFullObject(template_name)	{check_toolbox_class(DCS::TOOLBOX_CLASS);}
	/**
	 * Create a DCS from an in memory template resource
	 *
	 * @param object_template The template to create the object from
	 * @throws OsError if an object cannot be created from the template
	 */
	DCS(const res::ResDCS &object_template);

	/**
	 * Assign this DCS from another.
	 *
	 * Both objects will then refer to the same underlying toolbox object
	 *
	 * @param other DCS to copy
	 * @returns reference to this DCS
	 */
	DCS &operator=(const DCS &other) {_handle = other.handle(); return *this;}
	/**
	 * Assign this DCS from an Object that refers to a DCS
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other object
	 *
	 * @param other Object to copy.
	 * @returns reference to this DCS
	 * @throws ObjectNullException if the other object refers to a null object handle
	 * @throws ObjectClassException if the other object is not a DCS
	 */
	DCS &operator=(const Object &other) {_handle = other.handle(); check_toolbox_class(TOOLBOX_CLASS); return *this;}
	/**
	 * Check if this DCS refers to the same underlying toolbox
	 * object as another.
	 *
	 * @param other Object to compare to
	 * @returns true if they refer to the same underlying toolbox object
	 */
	bool operator==(const Object &other) const {return (_handle == other.handle());}
	/**
	 * Check if this DCS does not refers to the same underlying toolbox
	 * object as another.
	 *
	 * @param other Object to compare to
	 * @returns true if they do not refer to the same underlying toolbox object
	 */
	bool operator!=(const Object &other) const {return (_handle != other.handle());}


	// Properties
	/**
	 * Return the underlying toolbox window used to implement the DCS object
	 */
	Window window() const {return Window((ObjectId)int_property(0));}

	/**
	 * Set message to display in window
	 */
	void message(std::string msg) {string_property(1, msg);}

	/**
	 * Return message
	 */
	std::string message() const {return string_property(2);}
	/**
	 * Return length of message
	 */
	int message_length() const {return string_property_length(2);}

	/**
	 * Set title of window
	 */
	void title(std::string msg) {string_property(3, msg);}

	/**
	 * Return title
	 */
	std::string title() const {return string_property(4);}
	/**
	 * Return length of title
	 */
	int title_length() const {return string_property_length(4);}

	// Listeners
	void add_about_to_be_shown_listener(AboutToBeShownListener *listener);
	void remove_about_to_be_shown_listener(AboutToBeShownListener *listener);
	void add_has_been_hidden_listener(HasBeenHiddenListener *listener);
	void remove_has_been_hidden_listener(HasBeenHiddenListener *listener);
	void add_discard_listener(DCSDiscardListener *listener);
    void remove_discard_listener(DCSDiscardListener *listener);
	void add_save_listener(DCSSaveListener *listener);
    void remove_save_listener(DCSSaveListener *listener);
	void add_cancel_listener(DCSCancelListener *listener);
    void remove_cancel_listener(DCSCancelListener *listener);
};

/**
 * Listener for when a discard is selected from the DCS dialog
 */
class DCSDiscardListener : public Listener
{
public:
	/**
	 * Called when discard is selected from a DCS dialog
	 */
	virtual void dcs_discard(const EventInfo &discard_event) = 0;
};

/**
 * Listener for when a save is selected from the DCS dialog
 */
class DCSSaveListener : public Listener
{
public:
	/**
	 * Called when save is selected from a DCS dialog
	 */
	virtual void dcs_save(const EventInfo &save_event) = 0;
};

/**
 * Listener for when a cancel is selected from the DCS dialog
 */
class DCSCancelListener : public Listener
{
public:
	/**
	 * Called when cancel is selected from a DCS dialog
	 */
	virtual void dcs_cancel(const EventInfo &cancel_event) = 0;
};


}

#endif /* TBX_DCS_H_ */
