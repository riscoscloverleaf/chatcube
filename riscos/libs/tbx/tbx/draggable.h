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
 * draggable.h
 *
 *  Created on: 6-Jul-2010
 *      Author: alanb
 */

#ifndef TBX_DRAGABLE_H_
#define TBX_DRAGABLE_H_

#include "gadget.h"
#include "mouseclicklistener.h"

namespace tbx
{

class DragStartedListener;
class DragEndedListener;
class PointerInfo;

/**
 * A Draggable is a gadget that shows 
 */
class Draggable: public tbx::Gadget
{
public:
	enum {TOOLBOX_CLASS = 640}; //!< Toolbox class for this gadget.

	Draggable() {} //!< Construct an uninitialised draggable.
	/**
	 * Destroy a draggable gadget.
	 *
	 * This does not delete the underlying toolbox gadget.
	 */
	~Draggable() {}

	/**
	 * Construct a draggable from another draggable.
	 *
	 * Both Draggable will refer to the same underlying toolbox gadget.
	 */
	Draggable(const Draggable &other) : Gadget(other) {}

	/**
	 * Construct a draggable from another gadget.
	 *
	 * The Draggable and the Gadget will refer to the same
	 * underlying toolbox gadget.
	 *
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a draggable
	 */
	Draggable(const Gadget &other) : Gadget(other)	{check_toolbox_class(Draggable::TOOLBOX_CLASS);}

	/**
	 * Construct a draggable from a component.
	 *
	 * The Draggable and the Component will refer to the same
	 * underlying toolbox component.
	 */
	Draggable(const Component &other) : Gadget(other) {Window check(other.handle()); check_toolbox_class(Draggable::TOOLBOX_CLASS);}

	/**
	 * Assign a draggable to refer to the same underlying toolbox gadget
	 * as another.
	 */
	Draggable &operator=(const Draggable &other) {_handle = other._handle; _id = other._id; return *this;}

	/**
	 * Assign a draggable to refer to the same underlying toolbox gadget
	 * as an existing Gadget.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a draggable
	 */
	Draggable &operator=(const Gadget &other) {_handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Assign a draggable to refer to the same underlying toolbox component
	 * as an existing Gadget.
	 * @throws ObjectClassError if the component is not in a window.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a draggable
	 */
	Draggable &operator=(const Component &other) {Window check(other.handle()); _handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Check if this draggable refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they refer to the same underlying toolbox gadget.
	 */
	bool operator==(const Gadget &other) const {return (_handle == other.handle() && _id == other.id());}

	/**
	 * Check if this draggable refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they do not refer to the same underlying toolbox gadget.
	 */
	bool operator!=(const Gadget &other) const {return (_handle != other.handle() || _id != other.id());}

	/**
	 * Set name of sprite
	 */
	void sprite(std::string name) {string_property(640, name);}

	/**
	 * Get name of sprite
	 */
	std::string sprite() const {return string_property(641);}

	/**
	 * Return length of sprite name
	 */
	int sprite_length() const {return string_property_length(641);}

	/**
	 * Set text
	 */
	void text(std::string name) {string_property(642, name);}

	/**
	 * Get text
	 */
	std::string text() const {return string_property(643);}

	/**
	 * Return length of text
	 */
	int text_length() const {return string_property_length(643);}

	/**
	 * Set selected state of draggable
	 */
	void selected(bool value) {bool_property(644, value);}

	/**
	 * Get selected state of draggable
	 */
	bool selected() const {return bool_property(645);}

	void add_drag_started_listener(DragStartedListener *listener);
	void remove_drag_started_listener(DragStartedListener *listener);
	void add_drag_ended_listener(DragEndedListener *listener);
	void remove_drag_ended_listener(DragEndedListener *listener);

	void add_mouse_click_listener(MouseClickListener *listener);
	void remove_mouse_click_listener(MouseClickListener *listener);
};

/**
 * Event information for Drag started event
 */
class DragStartedEvent : public EventInfo
{
public:
	/**
	 * Construct a drag started event from the information returned
	 * for the toolbox
	 *
	 * @param id_block Block containing objects/components for the event
	 * @param data Additional data for the event.
	 */
	DragStartedEvent(IdBlock &id_block, PollBlock &data) :
	  EventInfo(id_block, data) {}

	/**
	 * return true if adjust is held down
	 */
	bool adjust() const {return (_data.word[3] & 1) != 0;}

	/**
	 * return true if select is held down
	 */
	bool select() const {return (_data.word[3] & 4) != 0;}

	/**
	 * return true if shift is held down
	 */
	bool shift() const {return (_data.word[3] & 8) != 0;}
	/**
	 * return true if control is held down
	 */
	bool control() const {return (_data.word[3] & 16) != 0;}

};

/**
 * Listener for DragStarted events
 */
class DragStartedListener : public Listener
{
public:
	virtual ~DragStartedListener() {}

	/**
	 * Function called when the drag is started from the draggable.
	 *
	 * @param drag_started_event information on the drag start.
	 */
	virtual void drag_started(const DragStartedEvent &drag_started_event) = 0;
};

/**
 * Event information for Drag ended event.
 *
 * A bit set on the draggable object determines if the result will be
 * toolbox object/component or Window/Icon WIMP handles.
 *
 * Even when the toolbox object/component is requested, WIMP handles will be
 * returned if the toolbox items can not be determined.
 */
class DragEndedEvent : public EventInfo
{
public:
	/**
	 * Construct the event from the information returned by the toolbox
	 *
	 * @param id_block Block containing objects/components for the event
	 * @param data Additional data for the event.
	 */
	DragEndedEvent(IdBlock &id_block, PollBlock &data) :
	  EventInfo(id_block, data) {}

	/**
	 * Returns true if toolbox ids are returned at end
	 * of drag.
	 */
	bool toolbox_ids() const {return (_data.word[3] & 1) != 0;}

	/**
	 * Returns toolbox object at end of drag if toolbox_ids is
	 * true.
	 * Object null is returned if object can not be determined
	 */
	Object object() const {return (toolbox_ids()) ? Object((ObjectId)_data.word[4]) : Object();}

	/**
	 * Return toolbox component at end of drag if toolbox_ids is true.
	 * Component null is returned if object can't be determined or drag
	 * did not end on a component
	 */
	Component component() const {return (toolbox_ids()) ? Component((ObjectId)_data.word[4], (ComponentId)_data.word[5]) : Component();}

	/**
	 * Returns WIMP Window handle if toolbox_ids if false
	 */
	WindowHandle window_handle() const {return (WindowHandle)_data.word[4];}

	/**
	 * Return WIMP icon handle if toolbox_ids is false
	 */
	IconHandle icon_handle() const {return (IconHandle)_data.word[5];}

	/**
	 * x coordinate of drag finish
	 */
	int x() const {return _data.word[6];}

	/**
	 * y coordinate of drag finish
	 */
	int y() const {return _data.word[7];}

	/**
	 * Return drag information as PointerInfo class
	 */
	PointerInfo where() const;
};

/**
 * Listener for DragStarted events
 */
class DragEndedListener : public Listener
{
public:
	virtual ~DragEndedListener() {}
	/**
	 * Method called when the drag has finished
	 *
	 * @param drag_ended_event details of where the drag ended.
	 */
	virtual void drag_ended(const DragEndedEvent &drag_ended_event) = 0;
};

}

#endif /* TBX_DRAGABLE_H_ */
