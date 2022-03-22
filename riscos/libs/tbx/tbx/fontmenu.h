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
 * fontmenu.h
 *
 *  Created on: 01-Jul-2010
 *      Author: alanb
 */

#ifndef TBX_FONTMENU_H_
#define TBX_FONTMENU_H_

#include "showpointobject.h"
#include "listener.h"
#include "eventinfo.h"
#include "window.h"

namespace tbx {

// Listeners
class AboutToBeShownListener;
class HasBeenHiddenListener;
class FontMenuSelectionListener;

namespace res
{
    class ResFontMenu;
}

/**
 * Class to show a toolbox FontMenu object.
 * Shows a menu where a font can be selected.static void saveas_dialog_completed_router(IdBlock &id_block, PollBlock &data, Listener *listener)
 */
class FontMenu : public tbx::ShowPointObject
{
public:
	enum { TOOLBOX_CLASS = 0x82a40 };

	/**
	 * Construct creates an unassigned FontMenu.
	 *
	 * It must be assigned to a value before the other methods can be used.
	 */
	FontMenu() {};
	/**
	 * Construct a FontMenu from another FontMenu.
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other FontMenu
	 *
	 * @param other FontMenu to copy.
	 */
	FontMenu(const FontMenu &other) : ShowPointObject(other._handle) {}
	/**
	 * Construct a FontMenu from an Object that refers to a FontMenu
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other object
	 *
	 * @param other Object to copy.
	 * @throws ObjectNullException if the other object refers to a null object handle
	 * @throws ObjectClassException if the other object is not a FontMenu
	 */
	FontMenu(const Object &other) : ShowPointObject(other)	{check_toolbox_class(FontMenu::TOOLBOX_CLASS);}
	/**
	 * Create a FontMenu from the named template
	 *
	 * @param template_name The name of the template to create the object from
	 * @throws OsError if an object cannot be created from the named template
	 * @throws ObjectClassException if the template is not for a FontMenu
	 */
	FontMenu(const std::string &template_name) : ShowPointObject(template_name)	{check_toolbox_class(FontMenu::TOOLBOX_CLASS);}
	/**
	 * Create a FontMenu from an in memory template resource
	 *
	 * @param object_template The template to create the object from
	 * @throws OsError if an object cannot be created from the template
	 */
	FontMenu(const res::ResFontMenu &object_template);

	/**
	 * Assign this FontMenu from another.
	 *
	 * Both objects will then refer to the same underlying toolbox object
	 *
	 * @param other FontMenu to copy
	 * @returns reference to this FontMenu
	 */
	FontMenu &operator=(const FontMenu &other) {_handle = other.handle(); return *this;}
	/**
	 * Assign this FontMenu from an Object that refers to a FontMenu
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other object
	 *
	 * @param other Object to copy.
	 * @returns reference to this FontMenu
	 * @throws ObjectNullException if the other object refers to a null object handle
	 * @throws ObjectClassException if the other object is not a FontMenu
	 */
	FontMenu &operator=(const Object &other) {_handle = other.handle(); check_toolbox_class(TOOLBOX_CLASS); return *this;}
	/**
	 * Check if this FontMenu refers to the same underlying toolbox
	 * object as another.
	 *
	 * @param other Object to compare to
	 * @returns true if they refer to the same underlying toolbox object
	 */
	bool operator==(const Object &other) const {return (_handle == other.handle());}
	/**
	 * Check if this FontMenu does not refers to the same underlying toolbox
	 * object as another.
	 *
	 * @param other Object to compare to
	 * @returns true if they do not refer to the same underlying toolbox object
	 */
	bool operator!=(const Object &other) const {return (_handle != other.handle());}


	// Properties
	/**
	 * Set font to select.
	 *
	 * @param value name of font to select or "SystemFont" for the System entry.
	 */
	void font(std::string value) {string_property(0, value);}

	/**
	 * Set dialog to no font selected
	 */
	void no_font() {int_property(0, 0);}

	/**
	 * Return selected font
	 */
	std::string font() const {return string_property(1);}

	/**
	 * Return selected font name length
	 */
	int font_length() const {return string_property_length(1);}

	// Listeners
	void add_about_to_be_shown_listener(AboutToBeShownListener *listener);
	void remove_about_to_be_shown_listener(AboutToBeShownListener *listener);
	void add_has_been_hidden_listener(HasBeenHiddenListener *listener);
	void remove_has_been_hidden_listener(HasBeenHiddenListener *listener);
	void add_selection_listener(FontMenuSelectionListener *listener);
	void remove_selection_listener(FontMenuSelectionListener *listener);
};


/**
 * Class to return information on font menu selection event
 */
class FontMenuSelectionEvent : public EventInfo
{
public:
	/**
	 * Constructor from event data returned from the toolbox
	 *
	 * @param id_block Objects/Components where the event was generated
	 * @param data raw data of the event
	 */
	FontMenuSelectionEvent(IdBlock &id_block, PollBlock & data) :
		EventInfo(id_block, data) {}

	/**
	 * Returns id of the font selected or "SystemFont" for the system entry.
	 */
	std::string font() const {return reinterpret_cast<const char *>(_data.word[4]);}
};

/**
 * Listener for font menu selection events
 */
class FontMenuSelectionListener : public Listener
{
public:
	~FontMenuSelectionListener() {};

	/**
	 * Called when a menu option has been selected.
	 *
	 * @param font_selected Information on the font selected
	 */
	virtual void fontmenu_selection(const FontMenuSelectionEvent &font_selected) = 0;
};

}

#endif /* TBX_FONTMENU_H_ */
