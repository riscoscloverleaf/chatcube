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
 * quit.h
 *
 *  Created on: 09-Jun-2009
 *      Author: alanb
 */

#ifndef TBX_QUIT_H_
#define TBX_QUIT_H_

#include "showfullobject.h"
#include "listener.h"
#include "eventinfo.h"
#include "window.h"

namespace tbx {

// Listeners
class AboutToBeShownListener;
class HasBeenHiddenListener;
class QuitQuitListener;
class QuitCancelListener;

namespace res
{
    class ResQuit;
}

/**
 * Class to show a toolbox Quit object that gives
 * the user an option to Quit or Cancel the application.
 *
 * This is usually used when the prequit message is sent from the
 * WIMP and there are some modified documents.
 */
class Quit : public tbx::ShowFullObject
{
public:
	enum { TOOLBOX_CLASS = 0x82a90 };

	/**
	 * Construct creates an unassigned Quit.
	 *
	 * It must be assigned to a value before the other methods can be used.
	 */
	Quit() {};
	/**
	 * Construct a Quit from another Quit.
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other Quit
	 *
	 * @param other Quit to copy.
	 */
	Quit(const Quit &other) : ShowFullObject(other._handle) {}
	/**
	 * Construct a Quit from an Object that refers to a Quit
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other object
	 *
	 * @param other Object to copy.
	 * @throws ObjectNullException if the other object refers to a null object handle
	 * @throws ObjectClassException if the other object is not a Quit
	 */
	Quit(const Object &other) : ShowFullObject(other)	{check_toolbox_class(Quit::TOOLBOX_CLASS);}
	/**
	 * Create a Quit from the named template
	 *
	 * @param template_name The name of the template to create the object from
	 * @throws OsError if an object cannot be created from the named template
	 * @throws ObjectClassException if the template is not for a Quit
	 */
	Quit(const std::string &template_name) : ShowFullObject(template_name)	{check_toolbox_class(Quit::TOOLBOX_CLASS);}
	/**
	 * Create a Quit from an in memory template resource
	 *
	 * @param object_template The template to create the object from
	 * @throws OsError if an object cannot be created from the template
	 */
	Quit(const res::ResQuit &object_template);

	/**
	 * Assign this Quit from another.
	 *
	 * Both objects will then refer to the same underlying toolbox object
	 *
	 * @param other Quit to copy
	 * @returns reference to this Quit
	 */
	Quit &operator=(const Quit &other) {_handle = other.handle(); return *this;}
	/**
	 * Assign this Quit from an Object that refers to a Quit
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other object
	 *
	 * @param other Object to copy.
	 * @returns reference to this Quit
	 * @throws ObjectNullException if the other object refers to a null object handle
	 * @throws ObjectClassException if the other object is not a Quit
	 */
	Quit &operator=(const Object &other) {_handle = other.handle(); check_toolbox_class(TOOLBOX_CLASS); return *this;}
	/**
	 * Check if this Quit refers to the same underlying toolbox
	 * object as another.
	 *
	 * @param other Object to compare to
	 * @returns true if they refer to the same underlying toolbox object
	 */
	bool operator==(const Object &other) const {return (_handle == other.handle());}
	/**
	 * Check if this Quit does not refers to the same underlying toolbox
	 * object as another.
	 *
	 * @param other Object to compare to
	 * @returns true if they do not refer to the same underlying toolbox object
	 */
	bool operator!=(const Object &other) const {return (_handle != other.handle());}


	// Properties
	/**
	 * Return the underlying toolbox window used to implement the Quit object
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
	void add_quit_listener(QuitQuitListener *listener);
    void remove_quit_listener(QuitQuitListener *listener);
	void add_cancel_listener(QuitCancelListener *listener);
    void remove_cancel_listener(QuitCancelListener *listener);
};

/**
 * Listener for when a Quit is selected from the Quit dialog
 */
class QuitQuitListener : public Listener
{
public:
	/**
	 * Called when quit is selected from a Quit dialog
	 */
	virtual void quit_quit(const EventInfo &quit_event) = 0;
};

/**
 * Listener for when a cancel is selected from the Quit dialog
 */
class QuitCancelListener : public Listener
{
public:
	/**
	 * Called when cancel is selected from a Quit dialog
	 */
	virtual void quit_cancel(const EventInfo &cancel_event) = 0;
};


}

#endif /* TBX_QUIT_H_ */
