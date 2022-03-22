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
 * radiobutton.h
 *
 *  Created on: 6-Jul-2010
 *      Author: alanb
 */

#ifndef TBX_RADIOBUTTON_H_
#define TBX_RADIOBUTTON_H_

#include "gadget.h"
#include "listener.h"
#include "eventinfo.h"

namespace tbx
{

class RadioButtonStateChangedListener;

/**
 * A RadioButton is a gadget that is shown with other RadioButtons to allow
 * the choice of one item of a group of options.
 */
class RadioButton: public tbx::Gadget
{
public:
	enum {TOOLBOX_CLASS = 384}; //!< Toolbox class for this gadget.

	RadioButton() {} //!< Construct an uninitialised radiobutton.
	/**
	 * Destroy a radiobutton gadget.
	 *
	 * This does not delete the underlying toolbox gadget.
	 */
	~RadioButton() {}

	/**
	 * Construct a radiobutton from another radiobutton.
	 *
	 * Both RadioButton will refer to the same underlying toolbox gadget.
	 */
	RadioButton(const RadioButton &other) : Gadget(other) {}

	/**
	 * Construct a radiobutton from another gadget.
	 *
	 * The RadioButton and the Gadget will refer to the same
	 * underlying toolbox gadget.
	 *
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a radiobutton
	 */
	RadioButton(const Gadget &other) : Gadget(other)	{check_toolbox_class(RadioButton::TOOLBOX_CLASS);}

	/**
	 * Construct a radiobutton from a component.
	 *
	 * The RadioButton and the Component will refer to the same
	 * underlying toolbox component.
	 */
	RadioButton(const Component &other) : Gadget(other) {Window check(other.handle()); check_toolbox_class(RadioButton::TOOLBOX_CLASS);}

	/**
	 * Assign a radiobutton to refer to the same underlying toolbox gadget
	 * as another.
	 */
	RadioButton &operator=(const RadioButton &other) {_handle = other._handle; _id = other._id; return *this;}

	/**
	 * Assign a radiobutton to refer to the same underlying toolbox gadget
	 * as an existing Gadget.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a radiobutton
	 */
	RadioButton &operator=(const Gadget &other) {_handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Assign a radiobutton to refer to the same underlying toolbox component
	 * as an existing Gadget.
	 * @throws ObjectClassError if the component is not in a window.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a radiobutton
	 */
	RadioButton &operator=(const Component &other) {Window check(other.handle()); _handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Check if this radiobutton refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they refer to the same underlying toolbox gadget.
	 */
	bool operator==(const Gadget &other) const {return (_handle == other.handle() && _id == other.id());}

	/**
	 * Check if this radiobutton refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they do not refer to the same underlying toolbox gadget.
	 */
	bool operator!=(const Gadget &other) const {return (_handle != other.handle() || _id != other.id());}

	/**
	 * Set the label for the radio button
	 */
	void label(std::string value) {string_property(384, value);}

	/**
	 * Get the label of the radio button
	 */
	std::string label() const {return string_property(385);}

	/**
	 * Get the length of the label
	 */
	int label_length() const {return string_property_length(385);}

	/**
	 * Set the event that will be raised when the radio buttons state changes
	 */
	void event(int id) {int_property(386, id);}

	/**
	 * Get the event that is raised when the radio buttons state changes
	 */
	int event() const {return int_property(387);}

	/**
	 * Turn radio button on or off
	 *
	 * If turned on, the current radio button that is on in the group will be
	 * turned off.
	 *
	 * If turned off and no other radio button in the group is on an
	 * exception will be thrown.
	 *
	 * @param turn_on true to turn radio button on, false to turn it off.
	 */
	void on(bool turn_on) {bool_property(388, turn_on);}

	/**
	 * Return true if radio button is on
	 */
	bool on() const {return bool_property(389);}

	void add_state_changed_listener(RadioButtonStateChangedListener *listener);
	void remove_state_changed_listener(RadioButtonStateChangedListener *listener);
};

/**
 * Event information for RadioStateButton event that
 * is generated when a radio buttons state changes
 */
class RadioButtonStateChangedEvent : public EventInfo
{
public:
	/**
	 * Construct the event from Toolbox and WIMP event data
	 *
	 * @param id_block Toolbox IDs for this event
	 * @param data Information returned from the WIMP for this event
	 */
	RadioButtonStateChangedEvent(IdBlock &id_block, PollBlock &data) :
	  EventInfo(id_block, data) {}

	/**
	 * return true if state was changed by adjust
	 */
	bool adjust() const {return (_data.word[3] & 1) !=0;}

	/**
	 * return true if state was changed by select
	 */
	bool select() const {return (_data.word[3] & 4) !=0;}

	/**
	 * return true if state changed to on, otherwise false
	 */
	bool on() const {return (_data.word[4] !=0);}

	/**
	 * Return the previous radio button that was on
	 */
	RadioButton old_on() const {return Component(id_block().self_object().handle(), (ComponentId)_data.word[5]);}
};

/**
 * Listener for RadioButton state changed event
 */
class RadioButtonStateChangedListener : public Listener
{
public:
	virtual ~RadioButtonStateChangedListener() {}
	/**
	 * Method called when radio button state changes
	 *
	 * @param state_changed_event details on the state change
	 */
	virtual void radiobutton_state_changed(const RadioButtonStateChangedEvent &state_changed_event) = 0;
};

}

#endif /* TBX_RADIOBUTTON_H_ */
