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
 * toolaction.h
 *
 *  Created on: 6-Jul-2010
 *      Author: alanb
 */

#ifndef TBX_TOOLACTION_H_
#define TBX_TOOLACTION_H_

#include "gadget.h"
#include "listener.h"
#include "eventinfo.h"

namespace tbx
{

class ToolActionSelectedListener;
class Command;

/**
 * A ToolAction is a gadget that shows a button that changes
 * it's sprite or text depending on if it is on/off or faded
 */
class ToolAction: public tbx::Gadget
{
public:
	enum {TOOLBOX_CLASS = 0x4014}; //!< Toolbox class for this gadget.

	ToolAction() {} //!< Construct an uninitialised toolaction.
	/**
	 * Destroy a toolaction gadget.
	 *
	 * This does not delete the underlying toolbox gadget.
	 */
	~ToolAction() {}

	/**
	 * Construct a toolaction from another toolaction.
	 *
	 * Both ToolAction will refer to the same underlying toolbox gadget.
	 */
	ToolAction(const ToolAction &other) : Gadget(other) {}

	/**
	 * Construct a toolaction from another gadget.
	 *
	 * The ToolAction and the Gadget will refer to the same
	 * underlying toolbox gadget.
	 *
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a toolaction
	 */
	ToolAction(const Gadget &other) : Gadget(other)	{check_toolbox_class(ToolAction::TOOLBOX_CLASS);}

	/**
	 * Construct a toolaction from a component.
	 *
	 * The ToolAction and the Component will refer to the same
	 * underlying toolbox component.
	 */
	ToolAction(const Component &other) : Gadget(other) {Window check(other.handle()); check_toolbox_class(ToolAction::TOOLBOX_CLASS);}

	/**
	 * Assign a toolaction to refer to the same underlying toolbox gadget
	 * as another.
	 */
	ToolAction &operator=(const ToolAction &other) {_handle = other._handle; _id = other._id; return *this;}

	/**
	 * Assign a toolaction to refer to the same underlying toolbox gadget
	 * as an existing Gadget.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a toolaction
	 */
	ToolAction &operator=(const Gadget &other) {_handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Assign a toolaction to refer to the same underlying toolbox component
	 * as an existing Gadget.
	 * @throws ObjectClassError if the component is not in a window.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a toolaction
	 */
	ToolAction &operator=(const Component &other) {Window check(other.handle()); _handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Check if this toolaction refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they refer to the same underlying toolbox gadget.
	 */
	bool operator==(const Gadget &other) const {return (_handle == other.handle() && _id == other.id());}

	/**
	 * Check if this toolaction refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they do not refer to the same underlying toolbox gadget.
	 */
	bool operator!=(const Gadget &other) const {return (_handle != other.handle() || _id != other.id());}

	// Idents (text or sprite)
	void on_ident(std::string text);
	std::string on_ident() const;
	void off_ident(std::string text);
	std::string off_ident() const;
	void fade_ident(std::string text);
	std::string fade_ident() const;

	// Actions
	void set_action(int select_action, int adjust_action);
	void get_action(int &select_action, int &adjust_action) const;

	// click show
	void set_click_show(Object select_object, Object adjust_object);
	void get_click_show(Object &select_object, Object &adjust_object) const;

	/**
	 * Set the on state
	 */
	void on(bool value) {bool_property(0x140146, value);}

	/**
	 * Get the on state
	 */
	bool on() const {return bool_property(0x140147);}

	/**
	 * Set the pressed state
	 */
	void pressed(bool value) {bool_property(0x140148, value);}

	/**
	 * Get the pressed state
	 */
	bool pressed() const {return bool_property(0x140149);}

	void add_selected_listener(ToolActionSelectedListener *listener);
	void remove_selected_listener(ToolActionSelectedListener *listener);

	void add_selected_command(Command *command);
	void remove_selected_command(Command *command);

	void add_select_command(Command *command);
	void remove_select_command(Command *command);

	void add_adjust_command(Command *command);
	void remove_adjust_command(Command *command);
};

/**
 * Event information for ToolAction Selected events
 */
class ToolActionSelectedEvent : public EventInfo
{
public:
	/**
	 * Construct the event from Toolbox and WIMP event data
	 *
	 * @param id_block Toolbox IDs for this event
	 * @param data Information returned from the WIMP for this event
	 */
	ToolActionSelectedEvent(IdBlock &id_block, PollBlock &data) :
	  EventInfo(id_block, data) {}

	/**
	 * Check if adjust was held down.
	 */
	bool adjust() const {return (_data.word[3] & 1)!=0;}

	/**
	 * Check if select was held down
	 */
	bool select() const {return (_data.word[3] & 4)!=0;}

	/**
	 * Return true if tool action has been turned on
	 */
	bool on() const {return (_data.word[4] != 0);}
};

/**
 * Listener for ToolAction Selected events
 */
class ToolActionSelectedListener : public Listener
{
public:
	virtual ~ToolActionSelectedListener() {}

	/**
	 * Method called when the tool action button is selected
	 *
	 * @param selected_event details of the new state and how it was selected
	 */
	virtual void toolaction_selected(const ToolActionSelectedEvent &selected_event) = 0;
};

}

#endif /* TBX_TOOLACTION_H_ */
