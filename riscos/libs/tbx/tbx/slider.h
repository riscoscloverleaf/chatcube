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
 * slider.h
 *
 *  Created on: 27-Mar-2009
 *      Author: alanb
 */

#ifndef TBX_SLIDER_H_
#define TBX_SLIDER_H_

#include "gadget.h"
#include "valuechangedlistener.h"
#include "colour.h"

namespace tbx
{

class SliderValueChangedListener;

/**
 * A Slider is a gadget that shows bar in a well which may be draggable
 * by the user.
 */
class Slider: public tbx::Gadget
{
public:
	enum {TOOLBOX_CLASS = 576}; //!< Toolbox class for this gadget.

	Slider() {} //!< Construct an uninitialised slider.
	/**
	 * Destroy a slider gadget.
	 *
	 * This does not delete the underlying toolbox gadget.
	 */
	~Slider() {}

	/**
	 * Construct a slider from another slider.
	 *
	 * Both Slider will refer to the same underlying toolbox gadget.
	 */
	Slider(const Slider &other) : Gadget(other) {}

	/**
	 * Construct a slider from another gadget.
	 *
	 * The Slider and the Gadget will refer to the same
	 * underlying toolbox gadget.
	 *
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a slider
	 */
	Slider(const Gadget &other) : Gadget(other)	{check_toolbox_class(Slider::TOOLBOX_CLASS);}

	/**
	 * Construct a slider from a gadget.
	 *
	 * The Slider and the Component will refer to the same
	 * underlying toolbox component.
	 */
	Slider(const Component &other) : Gadget(other) {Window check(other.handle()); check_toolbox_class(Slider::TOOLBOX_CLASS);}

	/**
	 * Assign a slider to refer to the same underlying toolbox gadget
	 * as another.
	 */
	Slider &operator=(const Slider &other) {_handle = other._handle; _id = other._id; return *this;}

	/**
	 * Assign a slider to refer to the same underlying toolbox gadget
	 * as an existing Gadget.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a slider
	 */
	Slider &operator=(const Gadget &other) {_handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Assign a slider to refer to the same underlying toolbox component
	 * as an existing Gadget.
	 * @throws ObjectClassError if the component is not in a window.
	 * @throws GadgetClassError if the underlying toolbox gadget
	 *         isn't a slider
	 */
	Slider &operator=(const Component &other) {Window check(other.handle()); _handle = other.handle(); _id = other.id(); check_toolbox_class(TOOLBOX_CLASS); return *this;}

	/**
	 * Check if this slider refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they refer to the same underlying toolbox gadget.
	 */
	bool operator==(const Gadget &other) const {return (_handle == other.handle() && _id == other.id());}

	/**
	 * Check if this slider refers to the same underlying toolbox gadget
	 * as another gadget.
	 * @returns true if they do not refer to the same underlying toolbox gadget.
	 */
	bool operator!=(const Gadget &other) const {return (_handle != other.handle() || _id != other.id());}

	/**
	 * Set the value of the slider.
	 *
	 */
	void value(int value) {int_property(576, value);}

	/**
	 * Get the value of the slider.
	 *
	 */
	int value() const {return int_property(577);}

	void set_bounds(int lower, int upper, int step_size);
	void get_bounds(int &lower, int &upper, int &step_size) const;
	void set_bounds(int lower, int upper);
	void get_bounds(int &lower, int &upper) const;
	void lower_bound(int value);
	int lower_bound() const;
	void upper_bound(int value);
	int upper_bound() const;
	void step_size(int value);
	int step_size() const;

	void set_colour(WimpColour bar, WimpColour background);
	void get_colour(WimpColour &bar, WimpColour &background);

    void bar_colour(WimpColour value);
    WimpColour bar_colour() const;
    void background_colour(WimpColour value);
    WimpColour background_colour() const;

	void add_value_changed_listener(SliderValueChangedListener *listener);
	void remove_value_changed_listener(SliderValueChangedListener *listener);

	void add_value_changed_listener(ValueChangedListener *listener);
	void remove_value_changed_listener(ValueChangedListener *listener);

};

/**
 * Information on event generated when the value of the slider has been
 * changed
 */
class SliderValueChangedEvent : public ValueChangedEvent
{
public:
	/**
	 * Construct the event from Toolbox and WIMP event data
	 *
	 * @param id_block Toolbox IDs for this event
	 * @param data Information returned from the WIMP for this event
	 */
	SliderValueChangedEvent(IdBlock &id_block, PollBlock &data) :
			ValueChangedEvent(id_block, data) {};

	/**
	 * Check if value was changed by a click or start of dragging of the
	 * bar.
	 *
	 * @returns true if value was changed by a click or start of dragging of the
	 * bar.
	 */
	bool drag_start_or_click() const {return (_data.word[3]  & 1)!=0;}
	/**
	 * Check if the drag is in progress
	 *
	 * @returns true if a drag of the slider is in progress
	 */
	bool drag_in_progress() const {return (_data.word[3]  & 2)!=0;}
	/**
	 * Check if this is the end of a drag
	 *
	 * @returns true if this event is the end of a slider drag
	 */
	bool drag_ended() const {return (_data.word[3]  & 4)!=0;}
};

/**
 * Listener for changes in the slider value
 */
class SliderValueChangedListener : public Listener
{
public:
	SliderValueChangedListener() {}
	virtual ~SliderValueChangedListener() {}

	/**
	 * Method called when the slider value has changed
	 *
	 * @param event information on the new value and how it was changed
	 */
	virtual void slider_value_changed(const SliderValueChangedEvent &event) = 0;
};

}

#endif /* TBX_SLIDER_H_ */
