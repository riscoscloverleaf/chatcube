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

#include "object.h"
#include "commandrouter.h"
#include "command.h"
#include "eventrouter.h"
#include "swixcheck.h"
#include "tbxexcept.h"
#include "objectdeletedlistener.h"
#include "usereventlistener.h"
#include "component.h"
#include "res/resobject.h"

#include <stdexcept>
#include <swis.h>

using namespace tbx;

/**
 * Create a toolbox object with the given name in the application
 * resources and assign a reference to it in this object.
 *
 * @param template_name name of template used to create the object
 * @throws OsError creation of the toolbox object failed
 */
Object::Object(const std::string &template_name)
{
	_handle = NULL_ObjectId;

	swix_check(_swix(0x44EC0, _INR(0,1) |_OUT(0),
			0,
			reinterpret_cast<int>(template_name.c_str()),
			&_handle
			));
}

/**
 * Protected constructor to create an object
 */
Object::Object(const res::ResObject &object_template)
{
	_handle = NULL_ObjectId;

	swix_check(_swix(0x44EC0, _INR(0,1) |_OUT(0),
			1,
			reinterpret_cast<int>(object_template.object_header()),
			&_handle
			));

}

/**
 * Delete the underlying toolbox object.
 *
 * The toolbox will raise an ObjectDeletedEvent
 * that will remove all the listeners on this
 * object.
 *
 * Once deleted the internal object handle is
 * set to NULL so that all methods will throw
 * an exception until it is assigned to another
 * live object.
 *
 * @throws ObjectNullError toolbox handle is NULL_ObjectId
 * @throws OsError Underlying call to toolbox failed.
 */
void Object::delete_object()
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	// Delete the toolbox object
	swix_check(_swix(0x44EC1, _INR(0,1), 0, _handle));
	_handle = NULL_ObjectId;
}

/**
 * Get the toolbox class of this object.
 *
 * @returns toolbox class or 0 if uninitialised (null) object.
 * @throws OsError if the underlying toolbox object is no longer valid.
 */
int Object::toolbox_class() const
{
	int result = 0;
	if (_handle)
	{
		swix_check(_swix(0x44EC9, _INR(0,1) | _OUT(0), 0, _handle, &result));
	}
	return result;
}

/**
 * Check if this objects toolbox class is as specified.
 *
 * @param class_id the toolbox class id to check against
 * @throws ObjectNullError if this object is uninitialised (null)
 * @throws OsError if the underlying toolbox object is no longer valid
 * @throws ObjectClassError if the class id doesn't match the underlying class id
 */
void Object::check_toolbox_class(int class_id) const
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	int result;
	swix_check(_swix(0x44EC9, _INR(0,1) | _OUT(0), 0, _handle, &result));
	if (result != class_id) throw ObjectClassError();
}

/**
 * Return user defined handle stored against this object.
 *
 * @returns void pointer to client handle
 */
void *Object::client_handle() const
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	int result;
	swix_check(_swix(0x44EC8, _INR(0,1) | _OUT(0), 0, _handle, &result));

	return (void *)result;
}

/**
 * Set a user defined handle for this object.
 *
 * @param client_handle void pointer to client handle
 */
void Object::client_handle(void *client_handle)
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	swix_check(_swix(0x44EC7, _INR(0,2), 0, _handle, client_handle));
}

/**
 * Get parent object.
 *
 * Return object will be Object::null() if there is no parent.
 */
Object Object::parent_object() const
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();
	ObjectId obj;
	ComponentId comp;

	swix_check(_swix(0x44ECA, _INR(0,1) | _OUTR(0,1), 0, _handle,
			&obj, &comp));

	return Object(obj);
}

/**
 * Get parent component.
 *
 * Returned component will be Component::null if there is no parent
 * component.
 */
Component Object::parent_component() const
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();
	ObjectId obj;
	ComponentId comp;

	swix_check(_swix(0x44ECA, _INR(0,1) | _OUTR(0,1), 0, _handle,
			&obj, &comp));

	return Component(obj, comp);
}

/**
 * Get ancestor object.
 *
 * Return object will be Object::null() if there is no ancestor.
 */
Object Object::ancestor_object() const
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();
	ObjectId obj;
	ComponentId comp;

	swix_check(_swix(0x44ECB, _INR(0,1) | _OUTR(0,1), 0, _handle,
			&obj, &comp));

	return Object(obj);
}

/**
 * Get ancestor component.
 *
 * Returned component will be Component::null if there is no ancestor
 * component.
 */
Component Object::ancestor_component() const
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();
	ObjectId obj;
	ComponentId comp;

	swix_check(_swix(0x44ECB, _INR(0,1) | _OUTR(0,1), 0, _handle,
			&obj, &comp));

	return Component(obj, comp);
}


/**
 * Show the object at the default place.
 *
 * Typically this means:
 *    Last place for a Window (uses template location first time)
 *    64 OS units to the left of the pointer for a menu.
 *
 * @throws ObjectNullError toolbox handle is NULL_ObjectId
 * @throws OsError Underlying call to toolbox failed.
 */

void Object::show()
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	_kernel_swi_regs regs;
    regs.r[0] = 0; // 0 - Show normally
    regs.r[1] = _handle;
    regs.r[2] = 0; // Show at default place
    regs.r[3] = 0; // Show Type data
    regs.r[4] = 0; // Parent object id
    regs.r[5] = -1; // Parent componend id

    swix_check(_kernel_swi(0x44EC3, &regs, &regs));
}

/**
 * Show the object in the centre of the screen
 *
 * @throws ObjectNullError toolbox handle is NULL_ObjectId
 * @throws OsError Underlying call to toolbox failed.
 */
void Object::show_centered()
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

    _kernel_swi_regs regs;
    regs.r[0] = 0; // 0 - Show normally
    regs.r[1] = _handle;
    regs.r[2] = 3; // Show centered
    regs.r[3] = 0; // Show Type data
    regs.r[4] = 0; // Parent object id
    regs.r[5] = -1; // Parent componend id

    swix_check(_kernel_swi(0x44EC3, &regs, &regs));
}

/**
 * Show the object at the current mouse position
 *
 * @throws ObjectNullError toolbox handle is NULL_ObjectId
 * @throws OsError Underlying call to toolbox failed.
 */
void Object::show_at_pointer()
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

    _kernel_swi_regs regs;
    regs.r[0] = 0; // 0 - Show normally
    regs.r[1] = _handle;
    regs.r[2] = 4; // Show at pointer
    regs.r[3] = 0; // Show Type data
    regs.r[4] = 0; // Parent object id
    regs.r[5] = -1; // Parent componend id

    swix_check(_kernel_swi(0x44EC3, &regs, &regs));
}

/**
 * Hide the object.
 *
 * @throws ObjectNullError toolbox handle is NULL_ObjectId
 * @throws OsError Underlying call to toolbox failed.
 */
void Object::hide()
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	_kernel_swi_regs regs;
	regs.r[0] = 0; // Flags
	regs.r[1] = _handle;
	swix_check(_kernel_swi(0x44EC4, &regs, &regs));
}

/**
 * Check if an object is currently showing
 */
bool Object::showing() const
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	_kernel_swi_regs regs;
	regs.r[0] = 0; // Flags
	regs.r[1] = _handle;
	swix_check(_kernel_swi(0x44EC5, &regs, &regs));

	return ((regs.r[0] & 1)!= 0);
}


/**
 * Remove all listeners and commands from this object
 *
 * @throws ObjectNullError toolbox handle is NULL_ObjectId
 */
void Object::remove_all_listeners()
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	event_router()->remove_all_listeners(_handle);
}

/**
 * Add a command to this Object.
 *
 * For more details on events and commands see
 * @subpage events
 *
 * @param command_id The event id to associate the command with.
 * @param command The command to run when this event is received.
 * @throws ObjectNullError toolbox handle is NULL_ObjectId
 */
void Object::add_command(int command_id, Command *command)
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	event_router()->add_object_listener(_handle, NULL_ComponentId, command_id, command, &command_router);
}

/**
 * Remove a command from this object.
 *
 * @param command_id The event id that was associated with the command.
 * @param command The command to remove.
 * @throws ObjectNullError toolbox handle is NULL_ObjectId
 */
void Object::remove_command(int command_id, Command *command)
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	event_router()->remove_object_listener(_handle, NULL_ComponentId, command_id, command);
}

/**
 * Add a user event listener.
 *
 * For more details on events and commands see
 * @subpage events
 *
 * @param event_id The event ID to associate the listener with.
 * @param listener The listener to use.
 */
void Object::add_user_event_listener(int event_id, UserEventListener *listener)
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	event_router()->add_object_listener(_handle, NULL_ComponentId, event_id, listener, &user_event_router);
}

/**
 * Remove a user event listener  from this object.
 *
 * @param event_id The event id that was associated with the listener.
 * @param listener The listener to remove.
 * @throws ObjectNullError toolbox handle is NULL_ObjectId
 */
void Object::remove_user_event_listener(int event_id, UserEventListener *listener)
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	event_router()->remove_object_listener(_handle, NULL_ComponentId, event_id, listener);
}

/**
 * Add listener for when the toolbox object referenced by this object
 * has been deleted.
 *
 * @param listener listener to add
 */
void Object::add_object_deleted_listener(ObjectDeletedListener *listener)
{
	add_listener(0x44EC2, listener, ObjectDeletedListener::handler);
}

/**
 * Remove listener for when the toolbox object referenced by this object
 * has been deleted.
 *
 * @param listener listener to remove
 */
void Object::remove_object_deleted_listener(ObjectDeletedListener *listener)
{
	remove_listener(0x44EC2, listener);
}

/**
 * Add a listener for the given toolbox event.
 *
 * This is a helper function that can be called from derived classes
 * to implement there toolbox event listeners
 *
 * @param action Toolbox event id
 * @param listener listener to add
 * @param handler function to translate event to listener to the event id.
 */
void Object::add_listener( int action, Listener *listener, RawToolboxEventHandler handler)
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	event_router()->add_object_listener(_handle, NULL_ComponentId, action, listener, handler);
}

/**
 * Remove listener for toolbox event
 *
 * @param action toolbox event id for listener
 * @param listener listener to remove
 */
void Object::remove_listener(int action, Listener *listener)
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	event_router()->remove_object_listener(_handle, NULL_ComponentId, action, listener);
}

/**
 * Set handler for an event that should not have more than one listener
 *
 * @param action toolbox event id
 * @param listener listener to set
 * @param handler function to translate event to specific listener for the event id
 */
void Object::set_handler(int action, Listener *listener, RawToolboxEventHandler handler)
{
	event_router()->set_object_handler(_handle, action, listener, handler);
}

/**
 * Return the value of a property that returns an integer
 *
 * This is a helper function for derived classes to make it
 * easy to implement a wrapper to a toolbox property.
 *
 * @param property_id ID of property to return
 * @returns integer value of the property
 * @throws ObjectNullError this is an uninitialised object
 * @throws OsError failed to retrieve property from toolbox object
 */
int Object::int_property(int property_id) const
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	int value;

    // Run Toolbox_ObjectMiscOp
    swix_check(_swix(0x44ec6, _INR(0,2) | _OUT(0), 0, _handle, property_id, &value));

    return value;
}

/**
 * Set the value of a property that requires an integer
 *
 * This is a helper function for derived classes to make it
 * easy to implement a wrapper to a toolbox property.
 *
 * @param property_id ID of property to set
 * @param value the new value for the property
 * @throws ObjectNullError this is an uninitialised object
 * @throws OsError failed to set the property on the toolbox object
 */
void Object::int_property(int property_id, int value)
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

   // Run Toolbox_ObjectMiscOp
    swix_check(_swix(0x44ec6, _INR(0,3), 0, _handle, property_id, value));
}

/**
 * Return the value of a property that returns text
 *
 * This is a helper function for derived classes to make it
 * easy to implement a wrapper to a toolbox property.
 *
 * @param property_id ID of property to return
 * @returns string value of the property
 * @throws ObjectNullError this is an uninitialised object
 * @throws OsError failed to retrieve property from toolbox object
 */
std::string Object::string_property(int property_id) const
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

    // Run Toolbox_ObjectMiscOp to get the size of the buffer
    int len;
    swix_check(_swix(0x44ec6, _INR(0,4) | _OUT(4), 0, _handle, property_id, 0, 0, &len));

    std::string value;
    if (len)
    {
    	char buffer[len];
       // Run Toolbox_ObjectMiscOp to get the buffer
       swix_check(_swix(0x44ec6, _INR(0,4), 0, _handle, property_id,
    		   reinterpret_cast<int>(buffer),
    		   len));
       value = buffer;
    }

    return value;
}

/**
 * Set the value of a property that requires a string
 *
 * This is a helper function for derived classes to make it
 * easy to implement a wrapper to a toolbox property.
 *
 * @param property_id ID of property to set
 * @param value the new value for the property
 * @throws ObjectNullError this is an uninitialised object
 * @throws OsError failed to set the property on the toolbox object
 */
void Object::string_property(int property_id, const std::string &value)
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

    // Run Toolbox_ObjectMiscOp
    swix_check(_swix(0x44ec6, _INR(0,3), 0, _handle, property_id,
    		reinterpret_cast<int>(const_cast<char *>(value.c_str()))));
}

/**
 * Gets the size of a string properties buffer
 *
 * This is a helper function for derived classes to make it
 * easy to implement a wrapper to a toolbox property.
 *
 * @param property_id ID of property to return buffer size for
 * @returns string buffer size of the property
 * @throws ObjectNullError this is an uninitialised object
 * @throws OsError failed to retrieve property from toolbox object
 */
int Object::string_property_length(int property_id) const
{
	if (_handle == NULL_ObjectId) throw ObjectNullError();

	int len;

    // Run Toolbox_ObjectMiscOp to get the size of the buffer
	swix_check(_swix(0x44ec6, _INR(0,4) | _OUT(4), 0, _handle, property_id, 0, 0, &len));

	return len - 1;
}


/**
 *  Get a boolean property from the toolbox Cobject.
 *
 *  Helper function to implement specific properties in subclasses.
 *  Calls Toolbox_ObjectMiscOp and returns value in r0.
 *
 * @param property_id the method code to get the property
 * @returns value of property
 * @throws  OsError failed to retrieve the property from the toolbox object
 */
bool Object::bool_property(int property_id) const
{
    _kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = property_id;

    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));

    return (regs.r[0] != 0);
}

/**
 *  Set a boolean property from the toolbox object.
 *
 *  Helper function to implement specific properties in subclasses.
 *  Calls Toolbox_ObjectMiscOp and with value in r4.
 *
 * @param property_id the method code to get the property
 * @param value new value for the property
 * @throws OsError failed to set the property on the toolbox object
 */
void Object::bool_property(int property_id, bool value)
{
    _kernel_swi_regs regs;
    regs.r[0] = 0; // Flags are zero
    regs.r[1] = _handle;
    regs.r[2] = property_id;
    regs.r[3] = value;

    // Run Toolbox_ObjectMiscOp
    swix_check(_kernel_swi(0x44ec6, &regs, &regs));
}
