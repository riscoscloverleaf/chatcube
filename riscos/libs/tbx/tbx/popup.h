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
 * popup.h
 *
 *  Created on: 6-Jul-2010
 *      Author: alanb
 */

#ifndef TBX_POPUP_H_
#define TBX_POPUP_H_

#include "gadget.h"
#include "listener.h"
#include "eventinfo.h"
#include "menu.h"

namespace tbx
{

class PopUpAboutToBeShownListener;

/**
 * A PopUp is a gadget that shows a menu arrow icon that
 * shows an associated menu when clicked
 */
class PopUp: public tbx::Gadget
{
public:
	enum {TOOLBOX_CLASS = 704}; //!< Toolbox class for this gadget.

	PopUp() {} //!< Construct an uninitialised popup.
	/**
	 * Destroy a popup gadget.
	 *
	 * This does not delete the underlying toolbox gadget.
	 */
	~PopUp() {}

	/**
	 * Construct a popup from another popup.
	 *
	 * Both PopUp will refer to the same underlying toolbox gadget.
	 */
	PopUp(const PopUp &other) : Gadget(other) {}

	/**
	 * Construct a popup from another gadget.
	 *
	 * The PopUp and the Gadget will refer to the same
	 * underlying toolbox gadget.
	 *
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a popup
	 */
	PopUp(const Gadget &other) : Gadget(other)	{check_toolbox_class(PopUp::TOOLBOX_CLASS);}

	/**
	 * Construct a popup from a component.
	 *
	 * The PopUp and the Component will refer to the same
	 * underlying toolbox component.
	 */
	PopUp(const Component &other) : Gadget(other) {Window check(other.handle()); check_toolbox_class(PopUp::TOOLBOX_CLASS);}

	/**
	 * Assign a popup to refer to the same underlying toolbox gadget
	 * as another.
	 */
	PopUp &operator=(const PopUp &other) {_handle = other._handle; _id = other._id; return *this;}

	/**
	 * Assign a popup to refer to the same underlying toolbox gadget
	 * as an existing Gadget.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a popup
	 */
	PopUp &operator=(const Gadget &other) {_handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Assign a popup to refer to the same underlying toolbox component
	 * as an existing Gadget.
	 * @throws ObjectClassError if the component is not in a window.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a popup
	 */
	PopUp &operator=(const Component &other) {Window check(other.handle()); _handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Check if this popup refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they refer to the same underlying toolbox gadget.
	 */
	bool operator==(const Gadget &other) const {return (_handle == other.handle() && _id == other.id());}

	/**
	 * Check if this popup refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they do not refer to the same underlying toolbox gadget.
	 */
	bool operator!=(const Gadget &other) const {return (_handle != other.handle() || _id != other.id());}

	/**
	 * Set menu for popup
	 */
	void menu(Menu menu) {int_property(704, (int)menu.handle());}

	/**
	 * Get menu for popup
	 */
	Menu menu() const {return Menu((ObjectId)int_property(705));}

	void add_popup_about_to_be_shown_listener(PopUpAboutToBeShownListener *listener);
	void remove_popup_about_to_be_shown_listener(PopUpAboutToBeShownListener *listener);
};

/**
 * Event information for PopUp AboutToBeShown event
 */
class PopUpAboutToBeShownEvent : public EventInfo
{
public:
	/**
	 * Construct the event from information returned from the Toolbox/WIMP
	 */
	PopUpAboutToBeShownEvent(IdBlock &id_block, PollBlock &data) :
	  EventInfo(id_block, data) {}

	/**
	 * Menu that is about to be shown
	 */
	Menu menu() const {return Menu((ObjectId)_data.word[4]);}
};

/**
 * Listener for PopUp AboutToBeShown events
 */
class PopUpAboutToBeShownListener : public Listener
{
public:
	virtual ~PopUpAboutToBeShownListener() {}
	/**
	 * Method called when the popup menu is about to be shown
	 *
	 * @param popup_event information on which popup menu is about to be shown
	 */
	virtual void popup_about_to_be_shown(const PopUpAboutToBeShownEvent &popup_event) = 0;
};

}

#endif /* TBX_POPUP_H_ */
