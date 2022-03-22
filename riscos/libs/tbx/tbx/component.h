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


#ifndef TBX_COMPONENT_H_
#define TBX_COMPONENT_H_

#include "handles.h"
#include "pollinfo.h"
#include "object.h"

namespace tbx {

class Command;
class UserEventListener;

/**
 * Base class for components in an object.
 *
 */
class Component {

protected:
	ObjectId _handle; //!< Underlying toolbox handle
	int _id;          //!< Underlying toolbox component id

public:
	/**
	 * Construct an uninitialised component.
	 *
	 * The component should be assigned to an initialised component before
	 * it is used.
	 */
	Component() : _handle(NULL_ObjectId), _id(NULL_ComponentId) {}
	/**
	 * Construct a component from an object and component id
	 *
	 * Note: There is no checking of the object or id for validity
	 *
	 * @param obj object that contains the component
	 * @param id toolbox component id
	 */
	Component(Object obj, ComponentId id) : _handle(obj.handle()), _id(id) {}
	/**
	 * Construct a component from an object handle and component id
	 *
	 * Note: There is no checking of the object handle or id for validity
	 *
	 * @param handle Object toolbox handle
	 * @param id toolbox component id
	 */
	Component(ObjectId handle, ComponentId id) : _handle(handle), _id(id) {}

	/**
	 * Check if this component is in an uninitialised state.
	 *
	 * @returns true if the component is uninitialised.
	 */
	bool null() const {return (_id == NULL_ComponentId);}

	/**
	 * Return the object his component belongs to
	 */
	Object object() {return Object(_handle);}

	/**
	 * Return the object his component belongs to
	 */
	Object object() const {return Object(_handle);}

	/**
	 * Return the object handle for his component.
	 */
	ObjectId handle() const {return _handle;}

	/**
	 * Get the component ID of this component
	 *
	 * @return Component id of this Component. This will be NULL_ComponentId
	 *         if the component is uninitialised.
	 */
	ComponentId id() const {return _id;}

	/**
	 * Assign the Component to refer to the same underlying toolbox component.
	 */
	Component &operator=(const Component &other) {_handle  = other._handle; _id = other._id; return *this;}

	/**
	 * Check if this component refers to the same underlying toolbox component
	 *
	 * @returns true if the components are the same.
	 */
	bool operator==(const Component &other) const {return (_handle == other._handle && _id == other._id);}

	/**
	 * Check if this component refers to the same underlying toolbox gadget
	 *
	 * @returns true if the components are not the same.
	 */
	bool operator!=(const Component &other) const {return (_handle != other._handle || _id != other._id);}

	/**
	 * Operator to check if this component has been initialised
	 *
	 * @returns true if the component has been initialised
	 */
	operator bool() const {return (_id != NULL_ComponentId);}

	// Listeners for all components
	void add_command(int command_id, Command *command);
	void remove_command(int command_id, Command *command);

	void add_user_event_listener(int event_id, UserEventListener *listener);
	void remove_user_event_listener(int event_id, UserEventListener *listener);

	/**
	 * Remove all the listeners on this component.
	 */
	void remove_all_listeners();

protected:
	// Helpers for derived classes

	// Add listener helper
	void add_listener( int action, Listener *listener, RawToolboxEventHandler handler);
	void remove_listener(int action, Listener *listener);

	// Add listener for WIMP window event helper
	void add_window_listener(int event_code, Listener *listener);
	void remove_window_listener(int event_code, Listener *listener);

    // Property Helpers
    int int_property(int property_id) const;
    void int_property(int property_id, int value);
    bool bool_property(int property_id) const;
    void bool_property(int property_id, bool value);
    std::string string_property(int property_id) const;
	int string_property_length(int property_id) const;
    void string_property(int property_id, const std::string &value);

    bool flag_property(int property_id, int flag) const;
    void flag_property(int property_id, int flag, bool value);
};

}

#endif /* COMPONENT_H_ */
