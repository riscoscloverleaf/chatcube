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
 * adjuster.h
 *
 *  Created on: 6-Jul-2010
 *      Author: alanb
 */

#ifndef TBX_ADJUSTER_H_
#define TBX_ADJUSTER_H_

#include "gadget.h"
#include "listener.h"
#include "eventinfo.h"

namespace tbx
{

class AdjusterClickedListener;

/**
 * An Adjuster is a gadget that shows two arrows facing
 * in opposite directions either vertically or horizontally.
 */
class Adjuster: public tbx::Gadget
{
public:
	enum {TOOLBOX_CLASS = 768}; //!< Toolbox class for this gadget.

	Adjuster() {} //!< Construct an uninitialised adjuster.
	/**
	 * Destroy a adjuster gadget.
	 *
	 * This does not delete the underlying toolbox gadget.
	 */
	~Adjuster() {}

	/**
	 * Construct a adjuster from another adjuster.
	 *
	 * Both Adjuster will refer to the same underlying toolbox gadget.
	 */
	Adjuster(const Adjuster &other) : Gadget(other) {}

	/**
	 * Construct a adjuster from another gadget.
	 *
	 * The Adjuster and the Gadget will refer to the same
	 * underlying toolbox gadget.
	 *
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a adjuster
	 */
	Adjuster(const Gadget &other) : Gadget(other)	{check_toolbox_class(Adjuster::TOOLBOX_CLASS);}

	/**
	 * Construct a adjuster from a component.
	 *
	 * The Adjuster and the Component will refer to the same
	 * underlying toolbox component.
	 */
	Adjuster(const Component &other) : Gadget(other) {Window check(other.handle()); check_toolbox_class(Adjuster::TOOLBOX_CLASS);}

	/**
	 * Assign a adjuster to refer to the same underlying toolbox gadget
	 * as another.
	 */
	Adjuster &operator=(const Adjuster &other) {_handle = other._handle; _id = other._id; return *this;}

	/**
	 * Assign a adjuster to refer to the same underlying toolbox gadget
	 * as an existing Gadget.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a adjuster
	 */
	Adjuster &operator=(const Gadget &other) {_handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Assign a adjuster to refer to the same underlying toolbox component
	 * as an existing Gadget.
	 * @throws ObjectClassError if the component is not in a window.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a adjuster
	 */
	Adjuster &operator=(const Component &other) {Window check(other.handle()); _handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Check if this adjuster refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they refer to the same underlying toolbox gadget.
	 */
	bool operator==(const Gadget &other) const {return (_handle == other.handle() && _id == other.id());}

	/**
	 * Check if this adjuster refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they do not refer to the same underlying toolbox gadget.
	 */
	bool operator!=(const Gadget &other) const {return (_handle != other.handle() || _id != other.id());}

	void add_clicked_listener(AdjusterClickedListener *listener);
	void remove_clicked_listener(AdjusterClickedListener *listener);
};

/**
 * Event information for Adjuster Clicked event
 */
class AdjusterClickedEvent : public EventInfo
{
public:
	/**
	 * Construct from toolbox event information
	 */
	AdjusterClickedEvent(IdBlock &id_block, PollBlock &data) :
	  EventInfo(id_block, data) {}

	/**
	 * true if up/right arrow was clicked.
	 * false if down/left was clicked
	 */
	bool up() const {return (_data.word[4] != 0);}
	/**
	 * false if up/right arrow was clicked.
	 * true if down/left was clicked
	 */
	bool down() const {return (_data.word[4] == 0);}
};

/**
 * Listener for Adjuster Clicked events
 */
class AdjusterClickedListener : public Listener
{
public:
	virtual ~AdjusterClickedListener() {}
	/**
	 * Called when an adjuster arrow was clicked
	 */
	virtual void adjuster_clicked(const AdjusterClickedEvent &clicked_event) = 0;
};

}

#endif /* TBX_ADJUSTER_H_ */
