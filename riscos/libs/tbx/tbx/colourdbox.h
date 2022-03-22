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
 * colourdbox.h
 *
 *  Created on: 01-Jul-2010
 *      Author: alanb
 */

#ifndef TBX_COLOURDBOX_H_
#define TBX_COLOURDBOX_H_

#include "showpointobject.h"
#include "listener.h"
#include "window.h"

namespace tbx {

// Listeners
class AboutToBeShownListener;
class HasBeenHiddenListener;
class ColourSelectedListener;
class ColourDboxDialogueCompletedListener;

namespace res
{
	class ResColourDbox;
}

/**
 * Class to show a toolbox ColourDbox object which allows a user to
 * pick a colour using a variety of colour models.
 *
 * The ColourDbox object uses the system ColourPicker module to implement
 * the Colour dialogue.
 */
class ColourDbox : public tbx::ShowPointObject
{
public:
	enum { TOOLBOX_CLASS = 0x829C0 };

	/**
	 * Construct creates an unassigned ColourDbox.
	 *
	 * It must be assigned to a value before the other methods can be used.
	 */
	ColourDbox() {};
	/**
	 * Construct a ColourDbox from another ColourDbox.
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other ColourDbox
	 *
	 * @param other ColourDbox to copy.
	 */
	ColourDbox(const ColourDbox &other) : ShowPointObject(other._handle) {}
	/**
	 * Construct a ColourDbox from an Object that refers to a ColourDbox
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other object
	 *
	 * @param other Object to copy.
	 * @throws ObjectNullException if the other object refers to a null object handle
	 * @throws ObjectClassException if the other object is not a ColourDbox
	 */
	ColourDbox(const Object &other) : ShowPointObject(other)	{check_toolbox_class(ColourDbox::TOOLBOX_CLASS);}
	/**
	 * Create a ColourDbox from the named template
	 *
	 * @param template_name The name of the template to create the object from
	 * @throws OsError if an object cannot be created from the named template
	 * @throws ObjectClassException if the template is not for a ColourDbox
	 */
	ColourDbox(const std::string &template_name) : ShowPointObject(template_name)	{check_toolbox_class(ColourDbox::TOOLBOX_CLASS);}
	/**
	 * Create a ColourDbox from an in memory template resource
	 *
	 * @param object_template The template to create the object from
	 * @throws OsError if an object cannot be created from the template
	 */
	ColourDbox(const res::ResColourDbox &object_template);

	/**
	 * Assign this ColourDbox from another.
	 *
	 * Both objects will then refer to the same underlying toolbox object
	 *
	 * @param other ColourDbox to copy
	 * @returns reference to this ColourDbox
	 */
	ColourDbox &operator=(const ColourDbox &other) {_handle = other.handle(); return *this;}

	/**
	 * Assign this ColourDbox from an Object that refers to a ColourDbox
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other object
	 *
	 * @param other Object to copy.
	 * @returns reference to this ColourDbox
	 * @throws ObjectNullException if the other object refers to a null object handle
	 * @throws ObjectClassException if the other object is not a ColourDbox
	 */
	ColourDbox &operator=(const Object &other) {_handle = other.handle(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Check if this ColourDbox refers to the same underlying toolbox
	 * object as another.
	 *
	 * @param other Object to compare to
	 * @returns true if they refer to the same underlying toolbox object
	 */
	bool operator==(const Object &other) const {return (_handle == other.handle());}
	/**
	 * Check if this ColourDbox does not refers to the same underlying toolbox
	 * object as another.
	 *
	 * @param other Object to compare to
	 * @returns true if they do not refer to the same underlying toolbox object
	 */
	bool operator!=(const Object &other) const {return (_handle != other.handle());}

	// Properties
	/**
	 * Return the underlying wimp window used by the ColourPicker module.
	 */
	WindowHandle wimp_handle() const {return (WindowHandle)int_property(0);}

	/**
	 * Return the underlying dialogue handle used by the ColourPicker module.
	 */
	int dialog_handle() const {return int_property(1);}

	/**
	 * Enumeration for the colour models
	 */
	enum ColourModel {RGB, CYMK, HSV};

	/**
	 * Set if an option for no colour is included
	 */
	void none_available(bool available) {bool_property(6, available);}
	/**
	 * Return true if a no colour option is included on the dialog box
	 */
	bool none_available() const {return bool_property(7);}

	// Listeners
	void add_about_to_be_shown_listener(AboutToBeShownListener *listener);
	void remove_about_to_be_shown_listener(AboutToBeShownListener *listener);
	void add_dialogue_completed_listener(ColourDboxDialogueCompletedListener *listener);
	void remove_dialogue_completed_listener(ColourDboxDialogueCompletedListener *listener);
	void add_colour_selected_listener(ColourSelectedListener *listener);
	void remove_colour_selected_listener(ColourSelectedListener *listener);

};

/**
 * Listener for when the Colour dialogue box has been closed
 */
class ColourDboxDialogueCompletedListener : public Listener
{
public:
	/**
	 * Called when dialogue has be closed
	 *
	 * @param colour_dbox The dialogue that generated the event.
	 * @param colour_selected true if the dialogue box was closed after a colour was selected.
	 * This value will also be false if a colour was selected by a click on OK with adjust
	 * and then cancelling the dialogue box.
	 */
	virtual void colourdbox_dialogue_completed(ColourDbox colour_dbox, bool colour_selected) = 0;
};

/**
 * Event data for colour selected event
 */
class ColourSelectedEvent
{
	ColourDbox _colour_dbox;
	bool _none;
	unsigned char *_block;
public:
	ColourSelectedEvent(ColourDbox cbox, bool none, const unsigned char *block);
	ColourSelectedEvent(const ColourSelectedEvent &other);
	~ColourSelectedEvent() {delete _block;}
	ColourSelectedEvent &operator=(const ColourSelectedEvent &other);

	/**
	 * Return colour dbox that generated this event
	 */
	ColourDbox colour_dbox() const {return _colour_dbox;}

	/**
	 * None colour was selected.
	 */
	bool none() const {return _none;}

	/**
	 * Blue amount selected
	 */
	unsigned char blue() const {return _block[1];}
	/**
	 * Green amount selected
	 */
	unsigned char green() const {return _block[2];}
	/**
	 * Red amount selected
	 */
	unsigned char red() const {return _block[3];}
	/**
	 * Colour model selected
	 */
	ColourDbox::ColourModel model() const {return ColourDbox::ColourModel(_block[8]);}
};

/**
 * Event listener for colour selected event
 */
class ColourSelectedListener : public Listener
{
public:
	virtual ~ColourSelectedListener() {};

	/**
	 * Colour has been selected in ColourDbox nd OK is pressed.
	 */
	virtual void colour_selected(const ColourSelectedEvent &event) = 0;
};


}

#endif /* TBX_COLOURDBOX_H_ */
