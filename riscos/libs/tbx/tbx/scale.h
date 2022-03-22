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
 * scale.h
 *
 *  Created on: 01-Jul-2010
 *      Author: alanb
 */

#ifndef TBX_SCALE_H_
#define TBX_SCALE_H_

#include "showfullobject.h"
#include "listener.h"
#include "eventinfo.h"
#include "window.h"

namespace tbx {

// Listeners
class AboutToBeShownListener;
class HasBeenHiddenListener;
class ScaleApplyFactorListener;

namespace res
{
    class ResScale;
}

/**
 * Class to show a toolbox Scale object.
 *
 * The scale object show a dialogue that allow the scale factors of
 * a document to be set.
 */
class Scale : public tbx::ShowFullObject
{
public:
	enum { TOOLBOX_CLASS = 0x82c00 };

	/**
	 * Uninitialised constructor
	 *
	 * The Scale object must be assigned to a valid Toolbox object before
	 * its properties or methods are used
	 */
	Scale() {};
	/**
	 * Construct from another Scale object
	 *
	 * This object then refers to the same underlying Toolbox object
	 * as the other Scale object
	 *
	 * @param other Scale to copy reference from.
	 */
	Scale(const Scale &other) : ShowFullObject(other._handle) {}
	/**
	 * Construct a Scale from an Object that refers to a Scale
	 *
	 * This class then refers to the same underlying Toolbox object
	 * as the other object
	 *
	 * @param other Object to copy.
	 * @throws ObjectNullException if the other object refers to a null object handle
	 * @throws ObjectClassException if the other object is not a Scale
	 */
	Scale(const Object &other) : ShowFullObject(other)	{check_toolbox_class(Scale::TOOLBOX_CLASS);}
	/**
	 * Create a Scale from the named template
	 *
	 * @param template_name The name of the template to create the object from
	 * @throws OsError if an object cannot be created from the named template
	 * @throws ObjectClassException if the template is not for a Scale
	 */
	Scale(const std::string &template_name) : ShowFullObject(template_name)	{check_toolbox_class(Scale::TOOLBOX_CLASS);}
	/**
	 * Create a Scale from an in memory template resource
	 *
	 * @param object_template The template to create the object from
	 * @throws OsError if an object cannot be created from the template
	 */
	Scale(const res::ResScale &object_template);

	/**
	 * Assign this Scale from another.
	 *
	 * Both objects will then refer to the same underlying toolbox object
	 *
	 * @param other Scale to copy
	 * @returns reference to this Scale
	 */
	Scale &operator=(const Scale &other) {_handle = other.handle(); return *this;}
	/**
	 * Assign this Scale from an Object that refers to a Scale
	 *
	 * This class then refers to the same underlying toolbox object
	 * as the other object
	 *
	 * @param other Object to copy.
	 * @returns reference to this Scale
	 * @throws ObjectNullException if the other object refers to a null object handle
	 * @throws ObjectClassException if the other object is not a Scale
	 */
	Scale &operator=(const Object &other) {_handle = other.handle(); check_toolbox_class(TOOLBOX_CLASS); return *this;}
	/**
	 * Check if this Scale refers to the same underlying toolbox
	 * object as another.
	 *
	 * @param other Object to compare to
	 * @returns true if they refer to the same underlying toolbox object
	 */
	bool operator==(const Object &other) const {return (_handle == other.handle());}
	/**
	 * Check if this Scale does not refers to the same underlying toolbox
	 * object as another.
	 *
	 * @param other Object to compare to
	 * @returns true if they do not refer to the same underlying toolbox object
	 */
	bool operator!=(const Object &other) const {return (_handle != other.handle());}

	// Properties
	/**
	 * Return the underlying toolbox window used to implement the Scale object
	 */
	Window window() const {return Window((ObjectId)int_property(0));}

	/**
	 * Set the scale percentage
	 *
	 * @param pc the new percentage for the scale
	 */
	void value(int pc) {int_property(1, pc);}

	/**
	 * Get the scale percentage
	 */
	int value() const {return int_property(2);}

	// Bounds and step
	void lower_bound(int value);
	int lower_bound() const;
	void upper_bound(int value);
	int upper_bound() const;
	void step_size(int value);
	int step_size() const;

	/**
	 * Set the title for the scale dialogue
	 */
	void title(std::string msg) {string_property(5, msg);}

	/**
	 * Return the title of the scale dialogue
	 */
	std::string title() const {return string_property(6);}

	/**
	 * Return the title length
	 */
	int title_length() const {return string_property_length(6);}

	// Listeners
	void add_about_to_be_shown_listener(AboutToBeShownListener *listener);
	void remove_about_to_be_shown_listener(AboutToBeShownListener *listener);
	void add_has_been_hidden_listener(HasBeenHiddenListener *listener);
	void remove_has_been_hidden_listener(HasBeenHiddenListener *listener);
	void add_apply_factor_listener(ScaleApplyFactorListener *listener);
	void remove_apply_factor_listener(ScaleApplyFactorListener *listener);
};

/**
 * Class to provide apply factor event information for Scale object
 */
class ScaleApplyFactorEvent : public EventInfo
{
public:
	/**
	 * Construct the event from Toolbox and WIMP event data
	 *
	 * @param id_block Toolbox IDs for this event
	 * @param data Information returned from the WIMP for this event
	 */
	ScaleApplyFactorEvent(IdBlock &id_block, PollBlock &data) :
		EventInfo(id_block, data) {}

	/**
	 * Return the choses scale factor
	 */
	int factor() const {return _data.word[4];}
};

/**
 * Listener for scale factor events
 */
class ScaleApplyFactorListener : public Listener
{
public:
	~ScaleApplyFactorListener() {}

	/**
	 * Method called when scale factors should be applied
	 *
	 * @param apply_event information on event including the scale factor to use
	 */
	virtual void scale_apply_factor(const ScaleApplyFactorEvent &apply_event) = 0;
};

}

#endif /* TBX_SCALE_H_ */
